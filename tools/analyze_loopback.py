#!/usr/bin/env python3
"""
Analyze a WAV captured from SpideyLoopbackCapture for repeating audio.

No third-party packages required. Supports the Windows shared-mode formats we
use for loopback capture (IEEE float32 and PCM 8/16/24/32-bit).
"""

from __future__ import annotations

import argparse
import math
import struct
from pathlib import Path


def read_wave(path: Path):
    blob = path.read_bytes()
    if len(blob) < 12 or blob[:4] != b"RIFF" or blob[8:12] != b"WAVE":
        raise ValueError("not a RIFF/WAVE file")

    pos = 12
    fmt = None
    data = None
    while pos + 8 <= len(blob):
        chunk_id = blob[pos : pos + 4]
        size = struct.unpack_from("<I", blob, pos + 4)[0]
        start = pos + 8
        end = start + size
        if end > len(blob):
            raise ValueError("truncated WAV chunk")
        if chunk_id == b"fmt ":
            fmt = blob[start:end]
        elif chunk_id == b"data":
            data = blob[start:end]
            break
        pos = end + (size & 1)

    if fmt is None or data is None or len(fmt) < 16:
        raise ValueError("missing fmt/data chunk")

    tag, channels, rate, avg, align, bits = struct.unpack_from("<HHIIHH", fmt, 0)
    actual_tag = tag
    if tag == 0xFFFE and len(fmt) >= 40:
        # WAVEFORMATEXTENSIBLE SubFormat GUID begins with the canonical tag.
        actual_tag = struct.unpack_from("<I", fmt, 24)[0] & 0xFFFF

    return {
        "tag": tag,
        "actual_tag": actual_tag,
        "channels": channels,
        "rate": rate,
        "align": align,
        "bits": bits,
        "data": data,
    }


def sample_decoder(info):
    tag = info["actual_tag"]
    bits = info["bits"]

    if tag == 3 and bits == 32:
        return 4, lambda b, o: struct.unpack_from("<f", b, o)[0]

    if tag != 1:
        raise ValueError(f"unsupported WAVE format tag {tag}, bits={bits}")

    if bits == 8:
        return 1, lambda b, o: (b[o] - 128) / 128.0
    if bits == 16:
        return 2, lambda b, o: struct.unpack_from("<h", b, o)[0] / 32768.0
    if bits == 24:
        def read24(b, o):
            v = b[o] | (b[o + 1] << 8) | (b[o + 2] << 16)
            if v & 0x800000:
                v -= 0x1000000
            return v / 8388608.0
        return 3, read24
    if bits == 32:
        return 4, lambda b, o: struct.unpack_from("<i", b, o)[0] / 2147483648.0

    raise ValueError(f"unsupported PCM bit depth {bits}")


def rms_envelope(info, bin_ms: float):
    data = info["data"]
    channels = info["channels"]
    rate = info["rate"]
    align = info["align"]
    bytes_per_sample, decode = sample_decoder(info)

    frames = len(data) // align
    frames_per_bin = max(1, int(rate * bin_ms / 1000.0))
    env = []

    for base in range(0, frames, frames_per_bin):
        end = min(frames, base + frames_per_bin)
        ss = 0.0
        count = 0
        for frame in range(base, end):
            frame_off = frame * align
            for ch in range(channels):
                v = decode(data, frame_off + ch * bytes_per_sample)
                ss += v * v
                count += 1
        env.append(math.sqrt(ss / max(1, count)))

    return env, frames / float(rate)


def normalized_corr(x, lag):
    if lag <= 0 or lag >= len(x):
        return 0.0
    a2 = b2 = ab = 0.0
    for i in range(lag, len(x)):
        a = x[i]
        b = x[i - lag]
        ab += a * b
        a2 += a * a
        b2 += b * b
    den = math.sqrt(a2 * b2)
    return ab / den if den else 0.0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("wav", type=Path)
    ap.add_argument("--bin-ms", type=float, default=10.0)
    ap.add_argument("--min-period", type=float, default=0.10)
    ap.add_argument("--max-period", type=float, default=6.0)
    ap.add_argument("--top", type=int, default=20)
    ap.add_argument("--expected", type=float, action="append", default=[])
    args = ap.parse_args()

    info = read_wave(args.wav)
    env, duration = rms_envelope(info, args.bin_ms)
    mean = sum(env) / max(1, len(env))
    centered = [v - mean for v in env]

    step_s = args.bin_ms / 1000.0
    min_lag = max(1, int(args.min_period / step_s))
    max_lag = min(len(centered) // 2, int(args.max_period / step_s))

    scores = []
    for lag in range(min_lag, max_lag + 1):
        scores.append((normalized_corr(centered, lag), lag * step_s))

    print(f"path={args.wav}")
    print(f"duration_s={duration:.6f}")
    print(f"format_tag={info['tag']}")
    print(f"decoded_format_tag={info['actual_tag']}")
    print(f"channels={info['channels']}")
    print(f"sample_rate={info['rate']}")
    print(f"bits={info['bits']}")
    print(f"block_align={info['align']}")
    print(f"mean_rms={mean:.9f}")
    print(f"max_rms={max(env) if env else 0.0:.9f}")
    print()
    print("strongest_periods:")
    for score, period in sorted(scores, reverse=True)[: args.top]:
        print(f"  period_s={period:.4f} correlation={score:.6f}")

    if args.expected:
        print()
        print("expected_periods:")
        for expected in args.expected:
            lag = max(1, int(round(expected / step_s)))
            period = lag * step_s
            score = normalized_corr(centered, lag)
            print(
                f"  requested_s={expected:.4f} sampled_s={period:.4f} "
                f"correlation={score:.6f}"
            )


if __name__ == "__main__":
    main()
