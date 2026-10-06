from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

needle='''typedef void (__fastcall *SpideyRetailMysterioFireBoobiesFn)(
		CMysterio*,
		void*);
'''
insert='''typedef int (__fastcall *SpideyRetailSoftSpotHitFn)(
		CSoftSpot*,
		void*,
		SHitInfo*);

static unsigned long gSpideySoftSpotHitCalls =
	0;
static int gSpideySoftSpotHitInstalled =
	0;

// @Ok
// Telemetry-only wrapper around retail CSoftSpot::Hit. Retail itself decides
// whether a hit is destructive by checking SHitInfo.field_0 & 0x04. This
// wrapper records the incoming hit class and the player's active web mode, then
// calls retail unchanged.
static int __fastcall SpideyMysterioSoftSpotHitTelemetry(
		CSoftSpot* spot,
		void*,
		SHitInfo* hit)
{
	++gSpideySoftSpotHitCalls;

	int playerWebMode =
		-1;
	CPlayer* player =
		*(CPlayer* volatile*)0x006A9038;
	if (player)
		playerWebMode =
			(int)player->field_8F8;

	const int hpBefore =
		spot ?
			(int)*(volatile short*)(
				(unsigned char*)spot +
				0xE2) :
			0;
	const int partIndex =
		spot ?
			spot->field_324 :
			-1;
	const unsigned int flags =
		hit ?
			(unsigned int)hit->field_0 :
			0;
	const unsigned int damage =
		hit ?
			(unsigned int)hit->field_8 :
			0;

	SpideyRetailSoftSpotHitFn retail =
		(SpideyRetailSoftSpotHitFn)0x0045F940;
	const int result =
		retail(
			spot,
			0,
			hit);

	const int hpAfter =
		spot ?
			(int)*(volatile short*)(
				(unsigned char*)spot +
				0xE2) :
			0;

	FILE* log =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (log)
	{
		fprintf(
			log,
			"mysterio_softspot_hit call=%lu spot=0x%08lX part=%d flags=0x%02X destructive_bit=%d damage=%u hp=%d->%d player_web_mode=%d result=%d policy=telemetry_only_retail_hit_unchanged\\n",
			gSpideySoftSpotHitCalls,
			(unsigned long)spot,
			partIndex,
			flags,
			(flags & 0x04) != 0,
			damage,
			hpBefore,
			hpAfter,
			playerWebMode,
			result);
		fclose(log);
	}

	return result;
}

static int SpideyInstallMysterioSoftSpotHitTelemetry()
{
	const unsigned long original =
		0x0045F940UL;
	const unsigned long replacement =
		(unsigned long)
		(void*)&SpideyMysterioSoftSpotHitTelemetry;

	gSpideySoftSpotHitInstalled =
		SpideyPatchBytes(
			0x0053BB94,
			(const unsigned char*)&original,
			(const unsigned char*)&replacement,
			sizeof(original),
			"mysterio_softspot_hit_telemetry_vtable");

	FILE* log =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (log)
	{
		fprintf(
			log,
			"mysterio_softspot_hit_install installed=%d vtable=0x0053BB88 slot=0x0053BB94 retail=0x0045F940 wrapper=0x%08lX retail_damage_gate=SHitInfo.field_0_bit_0x04\\n",
			gSpideySoftSpotHitInstalled,
			replacement);
		fclose(log);
	}

	return gSpideySoftSpotHitInstalled;
}

typedef void (__fastcall *SpideyRetailMysterioFireBoobiesFn)(
		CMysterio*,
		void*);
'''
if needle not in s:
    raise SystemExit("softspot insertion needle missing")
s=s.replace(needle,insert,1)

needle='''	int mysterioLaserInstalled =
		0;
	const int mysterioLaserAttackTelemetryInstalled =
'''
insert='''	int mysterioLaserInstalled =
		0;
	const int mysterioSoftSpotHitTelemetryInstalled =
		SpideyInstallMysterioSoftSpotHitTelemetry();
	const int mysterioLaserAttackTelemetryInstalled =
'''
if needle not in s:
    raise SystemExit("softspot installer call needle missing")
s=s.replace(needle,insert,1)

old='"high_fps_compat mysterio_laser=%d attack_telemetry=%d attack_call=0x0045F489 attack_retail=0x0045D200 vtable=0x0053BB34 destructor_expected=0x%08lX destructor_found=0x%08lX move_expected=0x%08lX move_found=0x%08lX clock=gTimerRelated_60hz grace_ticks=%lu grace_ms=50 marker_offset=0x44 policy=elapsed_tick_liveness_plus_fireboobies_20hz_cadence\\n",'
new='"high_fps_compat mysterio_laser=%d softspot_hit_telemetry=%d attack_telemetry=%d attack_call=0x0045F489 attack_retail=0x0045D200 vtable=0x0053BB34 destructor_expected=0x%08lX destructor_found=0x%08lX move_expected=0x%08lX move_found=0x%08lX clock=gTimerRelated_60hz grace_ticks=%lu grace_ms=50 marker_offset=0x44 policy=elapsed_tick_liveness_plus_fireboobies_20hz_cadence\\n",'
if old not in s:
    raise SystemExit("high fps log format missing")
s=s.replace(old,new,1)

old='''			mysterioLaserInstalled,
			mysterioLaserAttackTelemetryInstalled,
'''
new='''			mysterioLaserInstalled,
			mysterioSoftSpotHitTelemetryInstalled,
			mysterioLaserAttackTelemetryInstalled,
'''
if old not in s:
    raise SystemExit("high fps log args missing")
s=s.replace(old,new,1)

p.write_text(s,encoding="utf-8")
print("added Mysterio soft-spot hit telemetry")
