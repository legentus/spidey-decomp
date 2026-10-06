from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")
repls=[
("static const unsigned long kSpideyPacingDiagnosticHz = 60UL;","static const unsigned long kSpideyPacingDiagnosticHz = 20UL;"),
("static const unsigned long kSpideyPacingExpectedVblanksPerCallback = 1UL;","static const unsigned long kSpideyPacingExpectedVblanksPerCallback = 3UL;"),
("if (interval > 20)\n\t\tinterval = 20;","if (interval > 60)\n\t\tinterval = 60;"),
("first_delivery_target_ms=17 target_hz=60 expected_vblanks_per_callback=1 policy=periodic_1ms_dispatch_16_17ms_60hz","first_delivery_target_ms=51 target_hz=20 expected_vblanks_per_callback=3 policy=mysterio_reference_periodic_1ms_dispatch_50ms_full_engine_20hz"),
("retail_match=16ms_periodic_main_exe target_hz=60 expected_vblanks_per_callback=1 policy=periodic_1ms_source_dispatch_16_17ms_60hz","retail_match=16ms_periodic_main_exe target_hz=20 expected_vblanks_per_callback=3 policy=mysterio_reference_periodic_1ms_source_dispatch_50ms_full_engine_20hz"),
("// Native-60 retail timer delivery: one canonical vblank per active callback.\n// The names are retained to keep the completed 20-FPS reference diff small.\nstatic const unsigned long kSpideyPacingDiagnosticHz = 20UL;\nstatic const unsigned long kSpideyPacingExpectedVblanksPerCallback = 3UL;",
"// Temporary Mysterio ground-truth reference: deliver the untouched retail\n// TimerCallback at ~20 Hz. Retail converts each ~50 ms interval to roughly\n// three canonical 60-Hz ticks, reproducing the authored full-engine 20-FPS\n// update quantum while preserving canonical elapsed time. Revert to 60/1\n// after the Mysterio laser reference trace is captured.\nstatic const unsigned long kSpideyPacingDiagnosticHz = 20UL;\nstatic const unsigned long kSpideyPacingExpectedVblanksPerCallback = 3UL;")
]
for old,new in repls:
    if old not in s: raise SystemExit("missing replacement: "+old[:80])
    s=s.replace(old,new,1)
p.write_text(s,encoding="utf-8")
print("enabled Mysterio full-engine 20 FPS reference mode")
