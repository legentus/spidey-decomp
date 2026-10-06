from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

needle='''static void SpideyLogHighFpsRetailBytes(
		const char* label,
'''
insert='''typedef void (__fastcall *SpideyRetailMysterioFireBoobiesFn)(
		CMysterio*,
		void*);

static unsigned long gSpideyMysterioLaserAttackCalls =
	0;
static unsigned long gSpideyMysterioLaserAttackStarts =
	0;
static unsigned long gSpideyMysterioLaserStageTwoCalls =
	0;
static unsigned long gSpideyMysterioLaserLastCallTick =
	0;
static int gSpideyMysterioLaserAttackInProgress =
	0;

// @Ok
// Diagnostic only: count authored Mysterio state-6 laser attacks at their one
// retail dispatch point. This does not alter attack cadence or state.
static void __fastcall SpideyMysterioFireBoobiesTelemetry(
		CMysterio* mysterio,
		void*)
{
	SpideyRetailMysterioFireBoobiesFn retail =
		(SpideyRetailMysterioFireBoobiesFn)0x0045D200;

	if (!mysterio)
	{
		retail(
			mysterio,
			0);
		return;
	}

	const unsigned long now =
		(unsigned long)gTimerRelated;
	const int stateBefore =
		(int)mysterio->field_31C.bothFlags;
	const int substateBefore =
		mysterio->dumbAssPad;
	const int elapsed =
		(int)mysterio->field_80;
	const int cooldown39C =
		mysterio->field_39C;
	const int leftArm =
		mysterio->field_34C != 0;
	const int rightArm =
		mysterio->field_350 != 0;

	++gSpideyMysterioLaserAttackCalls;

	if (!gSpideyMysterioLaserAttackInProgress)
	{
		++gSpideyMysterioLaserAttackStarts;
		gSpideyMysterioLaserAttackInProgress =
			1;
	}

	if (substateBefore ==
		2)
	{
		++gSpideyMysterioLaserStageTwoCalls;
	}

	retail(
		mysterio,
		0);

	const int stateAfter =
		(int)mysterio->field_31C.bothFlags;
	const int substateAfter =
		mysterio->dumbAssPad;

	if (stateAfter !=
		6)
	{
		gSpideyMysterioLaserAttackInProgress =
			0;
	}

	FILE* log =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (log)
	{
		fprintf(
			log,
			"mysterio_laser_attack call=%lu attack=%lu stage2_calls=%lu tick=%lu delta_from_last=%lu state=%d->%d substate=%d->%d field80=%d cooldown39c=%d arms=%d,%d boss_active=%d policy=telemetry_only\\n",
			gSpideyMysterioLaserAttackCalls,
			gSpideyMysterioLaserAttackStarts,
			gSpideyMysterioLaserStageTwoCalls,
			now,
			gSpideyMysterioLaserLastCallTick ?
				now -
					gSpideyMysterioLaserLastCallTick :
				0,
			stateBefore,
			stateAfter,
			substateBefore,
			substateAfter,
			elapsed,
			cooldown39C,
			leftArm,
			rightArm,
			SpideyIsMysterioBossActive());
		fclose(log);
	}

	gSpideyMysterioLaserLastCallTick =
		now;
}

static void SpideyLogHighFpsRetailBytes(
		const char* label,
'''
if needle not in s: raise SystemExit("telemetry insertion needle missing")
s=s.replace(needle,insert,1)

needle='''	int mysterioLaserInstalled =
		0;
	const unsigned long foundDestructor =
'''
insert='''	int mysterioLaserInstalled =
		0;
	const int mysterioLaserAttackTelemetryInstalled =
		SpideyPatchDirectCall(
			0x0045F489,
			0x0045D200,
			(void*)&SpideyMysterioFireBoobiesTelemetry,
			"mysterio_laser_attack_telemetry");
	const unsigned long foundDestructor =
'''
if needle not in s: raise SystemExit("install insertion needle missing")
s=s.replace(needle,insert,1)

old='"high_fps_compat mysterio_laser=%d vtable=0x0053BB34 destructor_expected=0x%08lX destructor_found=0x%08lX move_expected=0x%08lX move_found=0x%08lX clock=gTimerRelated_60hz grace_ticks=%lu grace_ms=50 marker_offset=0x44 policy=elapsed_tick_liveness\\n",'
new='"high_fps_compat mysterio_laser=%d attack_telemetry=%d attack_call=0x0045F489 attack_retail=0x0045D200 vtable=0x0053BB34 destructor_expected=0x%08lX destructor_found=0x%08lX move_expected=0x%08lX move_found=0x%08lX clock=gTimerRelated_60hz grace_ticks=%lu grace_ms=50 marker_offset=0x44 policy=elapsed_tick_liveness_plus_attack_state_telemetry\\n",'
if old not in s: raise SystemExit("high fps log format missing")
s=s.replace(old,new,1)

old='''			mysterioLaserInstalled,
			expectedDestructor,
'''
new='''			mysterioLaserInstalled,
			mysterioLaserAttackTelemetryInstalled,
			expectedDestructor,
'''
if old not in s: raise SystemExit("high fps log args missing")
s=s.replace(old,new,1)

p.write_text(s,encoding="utf-8")
print("patched Mysterio laser attack telemetry")
