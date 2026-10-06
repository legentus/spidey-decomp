from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

start=s.find("// Mysterio authored-cadence compatibility.")
end=s.find("struct SpideyChaseSchedulerStats", start)
if start<0 or end<0:
    raise SystemExit("Mysterio AI block not found")

replacement=r'''// Mysterio laser-attack cadence compatibility.
//
// Keep CMysterio::AI itself at native 60 Hz. The previous experiment gated the
// entire virtual AI and reproduced the laser cadence, but it also starved other
// boss responsibilities and coincided with invalid node-damage behavior and a
// runtime crash. The authored-rate dependency is narrower: state 6 dispatches
// CMysterio::FireBoobies from the single callsite at 0x0045F489.
//
// Gate only that retail subroutine to one update per three canonical 60-Hz
// ticks while boss type 311 is active. All other Mysterio AI logic, damage
// handling, animation/state upkeep, camera, physics and rendering remain 60 Hz.
static CMysterio* gSpideyMysterioLaser20Boss =
	0;
static long gSpideyMysterioLaser20LastTick =
	0;
static int gSpideyMysterioLaser20TickValid =
	0;
static int gSpideyMysterioLaser20AccumulatedTicks =
	0;
static unsigned long gSpideyMysterioLaser20Calls =
	0;
static unsigned long gSpideyMysterioLaser20RetailCalls =
	0;
static unsigned long gSpideyMysterioLaser20HeldCalls =
	0;
static unsigned long gSpideyMysterioLaser20MaxElapsed =
	0;

static void SpideyResetMysterioLaser20State(
		CMysterio* mysterio)
{
	gSpideyMysterioLaser20Boss =
		mysterio;
	gSpideyMysterioLaser20LastTick =
		0;
	gSpideyMysterioLaser20TickValid =
		0;
	gSpideyMysterioLaser20AccumulatedTicks =
		0;
}

static void SpideyLogMysterioLaser20Stats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"mysterio_laser_20hz_stats calls=%lu retail_calls=%lu held_calls=%lu max_elapsed=%lu boss_type=311 callsite=0x0045F489 policy=fireboobies_only_20hz_global_ai_60hz\\n",
		gSpideyMysterioLaser20Calls,
		gSpideyMysterioLaser20RetailCalls,
		gSpideyMysterioLaser20HeldCalls,
		gSpideyMysterioLaser20MaxElapsed);
	fclose(f);
}

'''

s=s[:start]+replacement+s[end:]

# Remove whole-AI install call.
old='''	SpideyInstallMysterioAI20HzCompat();
	SpideyInstallChasePlayerAI20HzCompat();
'''
new='''	SpideyInstallChasePlayerAI20HzCompat();
'''
if old not in s:
    raise SystemExit("Mysterio AI install call not found")
s=s.replace(old,new,1)

# Replace shutdown whole-AI stats with laser subroutine stats.
old='''		SpideyLogMysterioAI20Stats();
		SpideyLogChasePlayerAI20Stats();
'''
new='''		SpideyLogMysterioLaser20Stats();
		SpideyLogChasePlayerAI20Stats();
'''
if old not in s:
    raise SystemExit("Mysterio AI stats call not found")
s=s.replace(old,new,1)

# Replace telemetry wrapper body with cadence-gated FireBoobies wrapper.
start=s.find("static void __fastcall SpideyMysterioFireBoobiesTelemetry(")
end=s.find("\nstatic void SpideyLogHighFpsRetailBytes(",start)
if start<0 or end<0:
    raise SystemExit("FireBoobies wrapper block not found")

newwrap=r'''static void __fastcall SpideyMysterioFireBoobiesTelemetry(
		CMysterio* mysterio,
		void*)
{
	SpideyRetailMysterioFireBoobiesFn retail =
		(SpideyRetailMysterioFireBoobiesFn)0x0045D200;

	++gSpideyMysterioLaser20Calls;

	if (!mysterio ||
		!SpideyIsMysterioBossActive())
	{
		if (gSpideyMysterioLaser20Boss != mysterio ||
			gSpideyMysterioLaser20TickValid)
		{
			SpideyResetMysterioLaser20State(
				mysterio);
		}

		retail(
			mysterio,
			0);
		return;
	}

	if (gSpideyMysterioLaser20Boss !=
		mysterio)
	{
		SpideyResetMysterioLaser20State(
			mysterio);
	}

	const unsigned long now =
		(unsigned long)*(volatile long*)0x006B4CA8;

	// A large gap means state 6 ended and a new laser attack has begun.
	if (gSpideyMysterioLaser20TickValid &&
		(long)(
			now -
			(unsigned long)gSpideyMysterioLaser20LastTick) >
			6)
	{
		SpideyResetMysterioLaser20State(
			mysterio);
	}

	if (!gSpideyMysterioLaser20TickValid)
	{
		gSpideyMysterioLaser20LastTick =
			(long)now;
		gSpideyMysterioLaser20TickValid =
			1;

		int initialElapsed =
			mysterio->field_80;
		if (initialElapsed < 0)
			initialElapsed = 0;
		if (initialElapsed > 6)
			initialElapsed = 6;

		gSpideyMysterioLaser20AccumulatedTicks =
			initialElapsed;
	}
	else
	{
		int elapsed =
			(int)(
				(long)now -
				gSpideyMysterioLaser20LastTick);
		gSpideyMysterioLaser20LastTick =
			(long)now;

		if (elapsed < 0)
			elapsed = 0;
		if (elapsed > 6)
			elapsed = 6;

		gSpideyMysterioLaser20AccumulatedTicks +=
			elapsed;
	}

	const int stateBefore =
		(int)mysterio->field_31C.bothFlags;
	const int substateBefore =
		mysterio->dumbAssPad;
	const int cooldown39C =
		mysterio->field_39C;
	const int leftArm =
		mysterio->field_34C != 0;
	const int rightArm =
		mysterio->field_350 != 0;

	if (gSpideyMysterioLaser20AccumulatedTicks <
		3)
	{
		++gSpideyMysterioLaser20HeldCalls;

		FILE* heldLog =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (heldLog)
		{
			fprintf(
				heldLog,
				"mysterio_laser_attack event=hold call=%lu tick=%lu accumulated=%d state=%d substate=%d cooldown39c=%d arms=%d,%d boss_active=1 policy=fireboobies_20hz\\n",
				gSpideyMysterioLaser20Calls,
				now,
				gSpideyMysterioLaser20AccumulatedTicks,
				stateBefore,
				substateBefore,
				cooldown39C,
				leftArm,
				rightArm);
			fclose(heldLog);
		}
		return;
	}

	int simElapsed =
		gSpideyMysterioLaser20AccumulatedTicks;
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
	gSpideyMysterioLaser20AccumulatedTicks =
		0;

	++gSpideyMysterioLaser20RetailCalls;
	if ((unsigned long)simElapsed >
		gSpideyMysterioLaser20MaxElapsed)
	{
		gSpideyMysterioLaser20MaxElapsed =
			(unsigned long)simElapsed;
	}

	const int stateAfter =
		(int)mysterio->field_31C.bothFlags;
	const int substateAfter =
		mysterio->dumbAssPad;

	FILE* log =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (log)
	{
		fprintf(
			log,
			"mysterio_laser_attack event=retail call=%lu retail_call=%lu tick=%lu state=%d->%d substate=%d->%d sim_elapsed=%d cooldown39c=%d arms=%d,%d boss_active=1 policy=fireboobies_20hz\\n",
			gSpideyMysterioLaser20Calls,
			gSpideyMysterioLaser20RetailCalls,
			now,
			stateBefore,
			stateAfter,
			substateBefore,
			substateAfter,
			simElapsed,
			cooldown39C,
			leftArm,
			rightArm);
		fclose(log);
	}

	if (stateAfter !=
		6)
	{
		SpideyResetMysterioLaser20State(
			mysterio);
	}
}
'''

s=s[:start]+newwrap+s[end:]

# Update install telemetry wording.
s=s.replace(
    "policy=elapsed_tick_liveness_plus_attack_state_telemetry",
    "policy=elapsed_tick_liveness_plus_fireboobies_20hz_cadence"
)

p.write_text(s,encoding="utf-8")
print("replaced whole Mysterio AI gate with FireBoobies-only cadence gate")
