from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

old='''static unsigned long gSpideyMysterioLaserAttackCalls =
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
'''
new='''static CMysterio* gSpideyMysterioLaser20Boss =
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

// @Ok
// Cadence-gate only the authored state-6 FireBoobies routine. CMysterio::AI
// itself remains native 60 Hz so damage/state/object upkeep is never starved.
'''
if old not in s: raise SystemExit("early globals block missing")
s=s.replace(old,new,1)

# Remove later duplicate cadence globals/reset helper, but retain the explanatory
# comment and stats function that follow them.
start=s.find("// Mysterio laser-attack cadence compatibility.")
stats=s.find("static void SpideyLogMysterioLaser20Stats()",start)
if start<0 or stats<0: raise SystemExit("late cadence block missing")
comment='''// Mysterio laser-attack cadence compatibility.
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

'''
s=s[:start]+comment+s[stats:]

p.write_text(s,encoding="utf-8")
print("fixed FireBoobies cadence declaration order")
