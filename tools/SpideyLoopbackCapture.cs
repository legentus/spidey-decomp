using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

enum EDataFlow { eRender = 0, eCapture = 1, eAll = 2 }
enum ERole { eConsole = 0, eMultimedia = 1, eCommunications = 2 }
enum AUDCLNT_SHAREMODE { SHARED = 0, EXCLUSIVE = 1 }

[ComImport, Guid("BCDE0395-E52F-467C-8E3D-C4579291692E")]
class MMDeviceEnumeratorComObject {}

[ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("A95664D2-9614-4F35-A746-DE8DB63617E6")]
interface IMMDeviceEnumerator {
    int EnumAudioEndpoints(EDataFlow dataFlow, uint dwStateMask, out object ppDevices);
    int GetDefaultAudioEndpoint(EDataFlow dataFlow, ERole role, out IMMDevice ppEndpoint);
    int GetDevice([MarshalAs(UnmanagedType.LPWStr)] string pwstrId, out IMMDevice ppDevice);
    int RegisterEndpointNotificationCallback(IntPtr pClient);
    int UnregisterEndpointNotificationCallback(IntPtr pClient);
}

[ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("D666063F-1587-4E43-81F1-B948E807363F")]
interface IMMDevice {
    int Activate(ref Guid iid, uint dwClsCtx, IntPtr pActivationParams, [MarshalAs(UnmanagedType.IUnknown)] out object ppInterface);
    int OpenPropertyStore(uint stgmAccess, out IntPtr ppProperties);
    int GetId([MarshalAs(UnmanagedType.LPWStr)] out string ppstrId);
    int GetState(out uint pdwState);
}

[ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("1CB9AD4C-DBFA-4c32-B178-C2F568A703B2")]
interface IAudioClient {
    int Initialize(AUDCLNT_SHAREMODE ShareMode, uint StreamFlags, long hnsBufferDuration, long hnsPeriodicity, IntPtr pFormat, ref Guid AudioSessionGuid);
    int GetBufferSize(out uint pNumBufferFrames);
    int GetStreamLatency(out long phnsLatency);
    int GetCurrentPadding(out uint pNumPaddingFrames);
    int IsFormatSupported(AUDCLNT_SHAREMODE ShareMode, IntPtr pFormat, out IntPtr ppClosestMatch);
    int GetMixFormat(out IntPtr ppDeviceFormat);
    int GetDevicePeriod(out long phnsDefaultDevicePeriod, out long phnsMinimumDevicePeriod);
    int Start();
    int Stop();
    int Reset();
    int SetEventHandle(IntPtr eventHandle);
    int GetService(ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppv);
}

[ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("C8ADBD64-E71E-48A0-A4DE-185C395CD317")]
interface IAudioCaptureClient {
    int GetBuffer(out IntPtr ppData, out uint pNumFramesToRead, out uint pdwFlags, out ulong pu64DevicePosition, out ulong pu64QPCPosition);
    int ReleaseBuffer(uint NumFramesRead);
    int GetNextPacketSize(out uint pNumFramesInNextPacket);
}

[StructLayout(LayoutKind.Sequential, Pack=2)]
struct WAVEFORMATEX {
    public ushort wFormatTag;
    public ushort nChannels;
    public uint nSamplesPerSec;
    public uint nAvgBytesPerSec;
    public ushort nBlockAlign;
    public ushort wBitsPerSample;
    public ushort cbSize;
}

class Program {
    const uint CLSCTX_ALL = 23;
    const uint AUDCLNT_STREAMFLAGS_LOOPBACK = 0x00020000;
    const uint AUDCLNT_BUFFERFLAGS_SILENT = 0x2;

    static void Check(int hr, string where) {
        if (hr < 0) Marshal.ThrowExceptionForHR(hr);
    }

    static void WriteU16(BinaryWriter bw, ushort v) { bw.Write(v); }
    static void WriteU32(BinaryWriter bw, uint v) { bw.Write(v); }

    static int Main(string[] args) {
        if (args.Length < 2) {
            Console.Error.WriteLine("usage: SpideyLoopbackCapture.exe output.wav seconds");
            return 2;
        }
        string output = args[0];
        double seconds = double.Parse(args[1], System.Globalization.CultureInfo.InvariantCulture);

        IMMDeviceEnumerator en = (IMMDeviceEnumerator)new MMDeviceEnumeratorComObject();
        IMMDevice dev;
        Check(en.GetDefaultAudioEndpoint(EDataFlow.eRender, ERole.eMultimedia, out dev), "GetDefaultAudioEndpoint");

        Guid iidClient = new Guid("1CB9AD4C-DBFA-4c32-B178-C2F568A703B2");
        object oClient;
        Check(dev.Activate(ref iidClient, CLSCTX_ALL, IntPtr.Zero, out oClient), "Activate");
        IAudioClient client = (IAudioClient)oClient;

        IntPtr fmtPtr;
        Check(client.GetMixFormat(out fmtPtr), "GetMixFormat");
        WAVEFORMATEX fmt = (WAVEFORMATEX)Marshal.PtrToStructure(fmtPtr, typeof(WAVEFORMATEX));
        int fmtSize = 18 + fmt.cbSize;
        byte[] fmtBytes = new byte[fmtSize];
        Marshal.Copy(fmtPtr, fmtBytes, 0, fmtSize);

        Guid session = Guid.Empty;
        Check(client.Initialize(AUDCLNT_SHAREMODE.SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 10000000, 0, fmtPtr, ref session), "Initialize");

        Guid iidCapture = new Guid("C8ADBD64-E71E-48A0-A4DE-185C395CD317");
        object oCapture;
        Check(client.GetService(ref iidCapture, out oCapture), "GetService");
        IAudioCaptureClient capture = (IAudioCaptureClient)oCapture;

        uint bufferFrames;
        Check(client.GetBufferSize(out bufferFrames), "GetBufferSize");

        using (FileStream fs = new FileStream(output, FileMode.Create, FileAccess.Write, FileShare.Read))
        using (BinaryWriter bw = new BinaryWriter(fs)) {
            bw.Write(new byte[] {(byte)'R',(byte)'I',(byte)'F',(byte)'F'});
            long riffSizePos = fs.Position; WriteU32(bw, 0);
            bw.Write(new byte[] {(byte)'W',(byte)'A',(byte)'V',(byte)'E'});
            bw.Write(new byte[] {(byte)'f',(byte)'m',(byte)'t',(byte)' '});
            WriteU32(bw, (uint)fmtBytes.Length);
            bw.Write(fmtBytes);
            if ((fmtBytes.Length & 1) != 0) bw.Write((byte)0);
            bw.Write(new byte[] {(byte)'d',(byte)'a',(byte)'t',(byte)'a'});
            long dataSizePos = fs.Position; WriteU32(bw, 0);
            long dataStart = fs.Position;

            Check(client.Start(), "Start");
            DateTime end = DateTime.UtcNow.AddSeconds(seconds);
            byte[] managed = new byte[Math.Max(1, (int)bufferFrames * fmt.nBlockAlign)];

            while (DateTime.UtcNow < end) {
                uint packet;
                Check(capture.GetNextPacketSize(out packet), "GetNextPacketSize");
                while (packet > 0) {
                    IntPtr data;
                    uint frames, flags;
                    ulong devPos, qpc;
                    Check(capture.GetBuffer(out data, out frames, out flags, out devPos, out qpc), "GetBuffer");
                    int bytes = checked((int)(frames * fmt.nBlockAlign));
                    if (managed.Length < bytes) managed = new byte[bytes];
                    if ((flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0 || data == IntPtr.Zero) {
                        Array.Clear(managed, 0, bytes);
                    } else {
                        Marshal.Copy(data, managed, 0, bytes);
                    }
                    bw.Write(managed, 0, bytes);
                    Check(capture.ReleaseBuffer(frames), "ReleaseBuffer");
                    Check(capture.GetNextPacketSize(out packet), "GetNextPacketSize");
                }
                Thread.Sleep(5);
            }
            client.Stop();

            long endPos = fs.Position;
            uint dataSize = checked((uint)(endPos - dataStart));
            fs.Position = dataSizePos; WriteU32(bw, dataSize);
            fs.Position = riffSizePos; WriteU32(bw, checked((uint)(endPos - 8)));
            fs.Position = endPos;

            Console.WriteLine("captured={0}", output);
            Console.WriteLine("seconds={0:F3}", seconds);
            Console.WriteLine("format_tag={0}", fmt.wFormatTag);
            Console.WriteLine("channels={0}", fmt.nChannels);
            Console.WriteLine("sample_rate={0}", fmt.nSamplesPerSec);
            Console.WriteLine("bits={0}", fmt.wBitsPerSample);
            Console.WriteLine("block_align={0}", fmt.nBlockAlign);
            Console.WriteLine("data_bytes={0}", dataSize);
        }

        Marshal.FreeCoTaskMem(fmtPtr);
        return 0;
    }
}