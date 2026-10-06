
from pathlib import Path
p = Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s = p.read_text(encoding="utf-8")
start = s.index("// Chase Venom world-actor cadence compatibility.")
end = s.index("static CPlayer* gSpideyChaseRampPlayer = 0;", start)
new = r'''// Chase Venom world-actor / script-controller cadence compatibility.
//
// Retail Logic updates the two coupled lists back-to-back:
//   0x004554F5 -> Ob_AI(&BaddyList, 0)        [0x0056E990]
//   0x00455501 -> Ob_AI(&ControlBaddyList, 0) [0x0056E994]
//
// Venom is on BaddyList.  L5A1 node 76, the CScriptOnlyBaddy that pulses the
// node-74 fixed-camera transition, is constructed onto ControlBaddyList.
// Therefore gating Venom alone cannot reproduce the authored 20-FPS phase:
// the camera controller would still advance on all three 60-Hz Logic passes.
//
// During synthesized Chase control, both lists must share one cadence gate.
// They are allowed through on the same canonical tick every 3 ticks.  Because
// Ob_AI then executes each body's EveryFrame(), elapsed field_80 naturally
// becomes ~3 for both the actor and script-controller lists.
//
// The shared "due tick" is important: BaddyList executes first.  When it
// establishes a due tick, ControlBaddyList immediately follows in the same
// Logic pass and is also allowed through instead of being delayed another
// three ticks.
typedef void (__cdecl *SpideyRetailObAIFn)(
	CBody**,
	int);

static long gSpideyChaseWorldAI20LastRetailTick = 0;
static long gSpideyChaseWorldAI20DueTick = -1;
static int gSpideyChaseWorldAI20TickValid = 0;
static unsigned long gSpideyChaseWorldAI20GateEvents = 0;
static unsigned long gSpideyChaseWorldAI20MaxElapsed = 0;

static unsigned long gSpideyChaseBaddyAI20Calls = 0;
static unsigned long gSpideyChaseBaddyAI20RetailCalls = 0;
static unsigned long gSpideyChaseBaddyAI20HeldCalls = 0;
static unsigned long gSpideyChaseControlAI20Calls = 0;
static unsigned long gSpideyChaseControlAI20RetailCalls = 0;
static unsigned long gSpideyChaseControlAI20HeldCalls = 0;

static int gSpideyChaseBaddyAI20Installed = 0;
static int gSpideyChaseControlAI20Installed = 0;

static void SpideyResetChaseWorldAI20State()
{
	gSpideyChaseWorldAI20LastRetailTick = 0;
	gSpideyChaseWorldAI20DueTick = -1;
	gSpideyChaseWorldAI20TickValid = 0;
}

static void __cdecl SpideyChaseWorldAI20Hz(
		CBody** list,
		int arg)
{
	SpideyRetailObAIFn retail =
		(SpideyRetailObAIFn)
		0x00460FC0;

	const unsigned long listAddress =
		(unsigned long)list;
	const int isBaddyList =
		listAddress == 0x0056E990UL;
	const int isControlBaddyList =
		listAddress == 0x0056E994UL;

	if (isBaddyList)
		++gSpideyChaseBaddyAI20Calls;
	else if (isControlBaddyList)
		++gSpideyChaseControlAI20Calls;

	CPlayer* player = 0;
	__try
	{
		player =
			*(CPlayer**)0x006A9038;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		player = 0;
	}

	const int chaseScripted =
		player &&
		SpideyRetailGetLevelId() == 0x501 &&
		player->field_1AC;

	if (!chaseScripted)
	{
		if (gSpideyChaseWorldAI20TickValid)
		{
			SpideyResetChaseWorldAI20State();
		}

		retail(
			list,
			arg);
		return;
	}

	const long currentTick =
		*(volatile long*)0x006B4CA8;

	int allowRetail = 0;

	if (!gSpideyChaseWorldAI20TickValid)
	{
		gSpideyChaseWorldAI20LastRetailTick =
			currentTick;
		gSpideyChaseWorldAI20DueTick =
			-1;
		gSpideyChaseWorldAI20TickValid =
			1;
	}
	else if (gSpideyChaseWorldAI20DueTick ==
			 currentTick)
	{
		// The sibling list immediately following the first allowed list in
		// this same Logic pass must share the exact same authored phase.
		allowRetail = 1;
	}
	else
	{
		const long elapsed =
			currentTick -
			gSpideyChaseWorldAI20LastRetailTick;

		if (elapsed >= 3)
		{
			gSpideyChaseWorldAI20LastRetailTick =
				currentTick;
			gSpideyChaseWorldAI20DueTick =
				currentTick;
			allowRetail = 1;
			++gSpideyChaseWorldAI20GateEvents;

			if ((unsigned long)elapsed >
				gSpideyChaseWorldAI20MaxElapsed)
			{
				gSpideyChaseWorldAI20MaxElapsed =
					(unsigned long)elapsed;
			}
		}
	}

	if (!allowRetail)
	{
		if (isBaddyList)
			++gSpideyChaseBaddyAI20HeldCalls;
		else if (isControlBaddyList)
			++gSpideyChaseControlAI20HeldCalls;
		return;
	}

	retail(
		list,
		arg);

	if (isBaddyList)
		++gSpideyChaseBaddyAI20RetailCalls;
	else if (isControlBaddyList)
		++gSpideyChaseControlAI20RetailCalls;
}

static int SpideyInstallChaseBaddyAI20HzCompat()
{
	gSpideyChaseBaddyAI20Installed =
		SpideyPatchDirectCall(
			0x004554F5,
			0x00460FC0,
			(void*)&SpideyChaseWorldAI20Hz,
			"timing_chase_baddylist_20hz");

	gSpideyChaseControlAI20Installed =
		SpideyPatchDirectCall(
			0x00455501,
			0x00460FC0,
			(void*)&SpideyChaseWorldAI20Hz,
			"timing_chase_controlbaddylist_20hz");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"chase_world_ai_20hz_install baddy_installed=%d baddy_call=0x004554F5 baddy_list=0x0056E990 control_installed=%d control_call=0x00455501 control_list=0x0056E994 retail=0x00460FC0 wrapper=0x%08lX policy=phase_locked_baddy_and_control_lists_20hz_during_l5a1_synthesized_control\\n",
			gSpideyChaseBaddyAI20Installed,
			gSpideyChaseControlAI20Installed,
			(unsigned long)(void*)&SpideyChaseWorldAI20Hz);
		fclose(f);
	}

	return
		gSpideyChaseBaddyAI20Installed &&
		gSpideyChaseControlAI20Installed;
}

static void SpideyLogChaseBaddyAI20Stats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"chase_world_ai_20hz_stats baddy_installed=%d baddy_calls=%lu baddy_retail=%lu baddy_held=%lu control_installed=%d control_calls=%lu control_retail=%lu control_held=%lu gate_events=%lu max_elapsed=%lu level=0x501 policy=phase_locked_venom_and_scriptonly_camera_controller_lists\\n",
		gSpideyChaseBaddyAI20Installed,
		gSpideyChaseBaddyAI20Calls,
		gSpideyChaseBaddyAI20RetailCalls,
		gSpideyChaseBaddyAI20HeldCalls,
		gSpideyChaseControlAI20Installed,
		gSpideyChaseControlAI20Calls,
		gSpideyChaseControlAI20RetailCalls,
		gSpideyChaseControlAI20HeldCalls,
		gSpideyChaseWorldAI20GateEvents,
		gSpideyChaseWorldAI20MaxElapsed);
	fclose(f);
}

'''
s = s[:start] + new + s[end:]
p.write_text(s, encoding="utf-8")
print("patched", start, end)
