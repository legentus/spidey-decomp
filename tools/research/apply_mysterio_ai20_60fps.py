from pathlib import Path

p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

# Restore global timer to native 60 Hz.
repls=[
    (
        "// Temporary Mysterio ground-truth reference: deliver the untouched retail\n"
        "// TimerCallback at ~20 Hz. Retail converts each ~50 ms interval to roughly\n"
        "// three canonical 60-Hz ticks, reproducing the authored full-engine 20-FPS\n"
        "// update quantum while preserving canonical elapsed time. Revert to 60/1\n"
        "// after the Mysterio laser reference trace is captured.\n"
        "static const unsigned long kSpideyPacingDiagnosticHz = 20UL;\n"
        "static const unsigned long kSpideyPacingExpectedVblanksPerCallback = 3UL;",
        "// Native-60 retail timer delivery: one canonical vblank per active callback.\n"
        "// Mysterio's authored attack cadence is handled locally in CMysterio::AI.\n"
        "static const unsigned long kSpideyPacingDiagnosticHz = 60UL;\n"
        "static const unsigned long kSpideyPacingExpectedVblanksPerCallback = 1UL;"
    ),
    ("if (interval > 60)\n\t\tinterval = 60;",
     "if (interval > 20)\n\t\tinterval = 20;"),
    ("first_delivery_target_ms=51 target_hz=20 expected_vblanks_per_callback=3 policy=mysterio_reference_periodic_1ms_dispatch_50ms_full_engine_20hz",
     "first_delivery_target_ms=17 target_hz=60 expected_vblanks_per_callback=1 policy=periodic_1ms_dispatch_16_17ms_60hz"),
    ("retail_match=16ms_periodic_main_exe target_hz=20 expected_vblanks_per_callback=3 policy=mysterio_reference_periodic_1ms_source_dispatch_50ms_full_engine_20hz",
     "retail_match=16ms_periodic_main_exe target_hz=60 expected_vblanks_per_callback=1 policy=periodic_1ms_source_dispatch_16_17ms_60hz"),
    ("const unsigned long now =\n\t\t(unsigned long)gTimerRelated;",
     "const unsigned long now =\n\t\t(unsigned long)*(volatile long*)0x006B4CA8;"),
]
for old,new in repls:
    if old not in s:
        raise SystemExit("missing restore/clock replacement: "+old[:100])
    s=s.replace(old,new,1)

# Insert exact Mysterio-AI cadence wrapper after the boss-active helper.
needle='''static int SpideyIsMysterioBossActive()
{
	int itemType =
		0;
	void* boss =
		0;

	__try
	{
		itemType =
			*(volatile int*)0x0060F654;
		boss =
			*(void* volatile*)0x0060F788;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		itemType =
			0;
		boss =
			0;
	}

	return itemType ==
			311 &&
		boss !=
			0;
}
'''
insert=needle+'''
// Mysterio authored-cadence compatibility.
//
// Retail CMysterio::AI is the sole dispatcher of the state-6 FireBoobies laser
// attack. At the authored 20-FPS update quantum the active beam-refresh stage
// runs once every ~50 ms. Native 60-Hz Logic otherwise services the same AI
// roughly three times in that interval, changing the attack/beam behavior even
// though the individual cooldown timers are elapsed-time aware.
//
// Keep only the Mysterio boss AI on the authored 3-canonical-tick boundary.
// CBody::EveryFrame/animation, player, retail boss camera, physics, rendering,
// other baddies and the rest of the engine continue at native 60 Hz.
typedef void (__fastcall *SpideyRetailMysterioAIFn)(
		CMysterio*,
		void*);

static CMysterio* gSpideyMysterioAI20Boss =
	0;
static long gSpideyMysterioAI20LastTick =
	0;
static int gSpideyMysterioAI20TickValid =
	0;
static int gSpideyMysterioAI20AccumulatedTicks =
	0;
static unsigned long gSpideyMysterioAI20Calls =
	0;
static unsigned long gSpideyMysterioAI20RetailCalls =
	0;
static unsigned long gSpideyMysterioAI20HeldCalls =
	0;
static unsigned long gSpideyMysterioAI20MaxElapsed =
	0;
static int gSpideyMysterioAI20Installed =
	0;

static void SpideyResetMysterioAI20State(
		CMysterio* mysterio)
{
	gSpideyMysterioAI20Boss =
		mysterio;
	gSpideyMysterioAI20LastTick =
		0;
	gSpideyMysterioAI20TickValid =
		0;
	gSpideyMysterioAI20AccumulatedTicks =
		0;
}

static void __fastcall SpideyMysterioAI20Hz(
		CMysterio* mysterio,
		void*)
{
	SpideyRetailMysterioAIFn retail =
		(SpideyRetailMysterioAIFn)0x0045EF10;

	++gSpideyMysterioAI20Calls;

	if (!mysterio ||
		!SpideyIsMysterioBossActive())
	{
		if (gSpideyMysterioAI20Boss != mysterio ||
			gSpideyMysterioAI20TickValid)
		{
			SpideyResetMysterioAI20State(
				mysterio);
		}

		retail(
			mysterio,
			0);
		return;
	}

	if (gSpideyMysterioAI20Boss !=
		mysterio)
	{
		SpideyResetMysterioAI20State(
			mysterio);
	}

	const long currentTick =
		*(volatile long*)0x006B4CA8;

	if (!gSpideyMysterioAI20TickValid)
	{
		gSpideyMysterioAI20LastTick =
			currentTick;
		gSpideyMysterioAI20TickValid =
			1;

		int initialElapsed =
			mysterio->field_80;
		if (initialElapsed < 0)
			initialElapsed = 0;
		if (initialElapsed > 6)
			initialElapsed = 6;

		gSpideyMysterioAI20AccumulatedTicks =
			initialElapsed;
	}
	else
	{
		int elapsed =
			(int)(
				currentTick -
				gSpideyMysterioAI20LastTick);
		gSpideyMysterioAI20LastTick =
			currentTick;

		if (elapsed < 0)
			elapsed = 0;
		if (elapsed > 6)
			elapsed = 6;

		gSpideyMysterioAI20AccumulatedTicks +=
			elapsed;
	}

	if (gSpideyMysterioAI20AccumulatedTicks <
		3)
	{
		++gSpideyMysterioAI20HeldCalls;
		return;
	}

	int simElapsed =
		gSpideyMysterioAI20AccumulatedTicks;
	if (simElapsed < 1)
		simElapsed = 1;
	if (simElapsed > 6)
		simElapsed = 6;

	const int originalField80 =
		mysterio->field_80;
	mysterio->field_80 =
		simElapsed;

	retail(
		mysterio,
		0);

	mysterio->field_80 =
		originalField80;
	gSpideyMysterioAI20AccumulatedTicks =
		0;

	++gSpideyMysterioAI20RetailCalls;
	if ((unsigned long)simElapsed >
		gSpideyMysterioAI20MaxElapsed)
	{
		gSpideyMysterioAI20MaxElapsed =
			(unsigned long)simElapsed;
	}
}

static int SpideyInstallMysterioAI20HzCompat()
{
	const unsigned long original =
		0x0045EF10UL;
	const unsigned long replacement =
		(unsigned long)
		(void*)&SpideyMysterioAI20Hz;

	gSpideyMysterioAI20Installed =
		SpideyPatchBytes(
			0x0053BABC,
			(const unsigned char*)&original,
			(const unsigned char*)&replacement,
			sizeof(original),
			"timing_mysterio_ai_20hz_vtable");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"mysterio_ai_20hz_install installed=%d vtable=0x0053BAB4 slot=0x0053BABC retail=0x0045EF10 wrapper=0x%08lX boss_type=311 policy=mysterio_ai_only_3_canonical_tick_boundary_render_world_camera_60hz\\n",
			gSpideyMysterioAI20Installed,
			replacement);
		fclose(f);
	}

	return gSpideyMysterioAI20Installed;
}

static void SpideyLogMysterioAI20Stats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"mysterio_ai_20hz_stats installed=%d calls=%lu retail_calls=%lu held_calls=%lu max_elapsed=%lu boss_type=311 policy=mysterio_ai_only_20hz_field80_accumulated_global_timer_60hz\\n",
		gSpideyMysterioAI20Installed,
		gSpideyMysterioAI20Calls,
		gSpideyMysterioAI20RetailCalls,
		gSpideyMysterioAI20HeldCalls,
		gSpideyMysterioAI20MaxElapsed);
	fclose(f);
}
'''
if needle not in s:
    raise SystemExit("boss helper insertion needle missing")
s=s.replace(needle,insert,1)

# Install Mysterio AI vtable compatibility in the timing installer.
needle='''	SpideyInstallChasePlayerAI20HzCompat();
	SpideyInstallChaseCameraAI20HzCompat();
	SpideyInstallChaseBaddyAI20HzCompat();
	SpideyInstallChaseSynth20HzCompat();
'''
insert='''	SpideyInstallMysterioAI20HzCompat();
	SpideyInstallChasePlayerAI20HzCompat();
	SpideyInstallChaseCameraAI20HzCompat();
	SpideyInstallChaseBaddyAI20HzCompat();
	SpideyInstallChaseSynth20HzCompat();
'''
if needle not in s:
    raise SystemExit("timing installer needle missing")
s=s.replace(needle,insert,1)

# Log Mysterio cadence stats on clean timer shutdown.
needle='''		SpideyLogChasePlayerAI20Stats();
		SpideyLogChaseCameraAI20Stats();
		SpideyLogChaseBaddyAI20Stats();
'''
insert='''		SpideyLogMysterioAI20Stats();
		SpideyLogChasePlayerAI20Stats();
		SpideyLogChaseCameraAI20Stats();
		SpideyLogChaseBaddyAI20Stats();
'''
if needle not in s:
    raise SystemExit("shutdown stats needle missing")
s=s.replace(needle,insert,1)

p.write_text(s,encoding="utf-8")
print("restored native 60 Hz and added Mysterio AI authored-cadence wrapper")
