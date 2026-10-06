from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

# 1) Stable Mysterio boss-active helper, early enough for UI and camera code.
needle='''static int SpideyRetailGetLevelId()
{
	SpideyRetailTrigGetLevelIdFn fn =
		(SpideyRetailTrigGetLevelIdFn)0x004DE770;
	return fn();
}
'''
insert=needle+'''
static int SpideyIsMysterioBossActive()
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
if needle not in s: raise SystemExit("level helper needle missing")
s=s.replace(needle,insert,1)

# 2) Modern aim must be logically inactive throughout the Mysterio boss fight.
needle='''static int SpideyModernAimIsEffectivelyActive(
		CPlayer* player)
{
	if (!player)
		return 0;
'''
insert='''static int SpideyModernAimIsEffectivelyActive(
		CPlayer* player)
{
	if (!player ||
		SpideyIsMysterioBossActive())
	{
		return 0;
	}
'''
if needle not in s: raise SystemExit("aim active needle missing")
s=s.replace(needle,insert,1)

# 3) Mode-3 modern camera explicitly yields to retail while Mysterio is active.
needle='''	if (!camera)
	{
		return;
	}

	if (camera->mCameraMode !=
		CAMERAMODE_DEMO)
'''
insert='''	if (!camera)
	{
		return;
	}

	if (SpideyIsMysterioBossActive())
	{
		SpideyModernCameraRelease(
			"mysterio_retail_boss_camera",
			camera,
			camera->mCameraMode);
		retail(
			camera,
			0);
		return;
	}

	if (camera->mCameraMode !=
		CAMERAMODE_DEMO)
'''
# target only first occurrence after mode3 function signature
pos=s.find("static void __fastcall SpideyModernMode3Camera(")
if pos<0: raise SystemExit("mode3 function missing")
sub=s[pos:]
if needle not in sub: raise SystemExit("mode3 null needle missing")
sub=sub.replace(needle,insert,1)
s=s[:pos]+sub

# 4) Final camera postprocess also forces release/retail-only on transition frames.
needle='''	if (!camera)
	{
		retail(
			camera,
			0);
		return;
	}

	CPlayer* player =
'''
insert='''	if (!camera)
	{
		retail(
			camera,
			0);
		return;
	}

	if (SpideyIsMysterioBossActive())
	{
		SpideyModernCameraRelease(
			"mysterio_retail_boss_postprocess",
			camera,
			camera->mCameraMode);
		retail(
			camera,
			0);
		return;
	}

	CPlayer* player =
'''
pos=s.find("static void __fastcall SpideyModernAimCameraPostprocess(")
if pos<0: raise SystemExit("camera postprocess missing")
sub=s[pos:]
if needle not in sub: raise SystemExit("postprocess null needle missing")
sub=sub.replace(needle,insert,1)
s=s[:pos]+sub

# 5) Mysterio-specific common-boss fill wrappers. Insert before install routine.
needle='''// @Ok
static void SpideyInstallGameplayUiScaleCompat()
{
'''
wrappers='''static unsigned long gSpideyMysterioBossUiScaledDraws =
	0;

// @Ok
static void __cdecl SpideyCompatMysterioBossQPoly2D(
		float x0,
		float y0,
		float u0,
		float v0,
		u32 color0,
		float x1,
		float y1,
		float u1,
		float v1,
		u32 color1,
		float x2,
		float y2,
		float u2,
		float v2,
		u32 color2,
		float x3,
		float y3,
		float u3,
		float v3,
		u32 color3,
		float z)
{
	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyCompatHealthBarQPoly2D(
			x0, y0, u0, v0, color0,
			x1, y1, u1, v1, color1,
			x2, y2, u2, v2, color2,
			x3, y3, u3, v3, color3,
			z);
		return;
	}

	SpideyRetailQPoly2DFn retail =
		(SpideyRetailQPoly2DFn)0x00507910;
	retail(
		x0, y0, u0, v0, color0,
		x1, y1, u1, v1, color1,
		x2, y2, u2, v2, color2,
		x3, y3, u3, v3, color3,
		z);
}

// @Ok
static void __cdecl SpideyCompatMysterioBossFlatPoly(
		float z,
		i32 x,
		i32 y,
		i32 width,
		i32 height,
		u8 red,
		u8 green,
		u8 blue,
		i32 option9,
		i32 option10)
{
	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyCompatPanelFlatPoly(
			z,
			x,
			y,
			width,
			height,
			red,
			green,
			blue,
			option9,
			option10);
		return;
	}

	SpideyRetailFlatUiPolyFn retail =
		(SpideyRetailFlatUiPolyFn)0x00462D60;
	retail(
		z,
		x,
		y,
		width,
		height,
		red,
		green,
		blue,
		option9,
		option10);
}

// @Ok
static void __cdecl SpideyCompatMysterioBossGouraudPoly(
		float z,
		i32 x,
		i32 y,
		i32 width,
		i32 height,
		u32 color0,
		u32 color1,
		u32 color2,
		u32 color3,
		i32 option10)
{
	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyCompatPanelGouraudPoly(
			z,
			x,
			y,
			width,
			height,
			color0,
			color1,
			color2,
			color3,
			option10);
		return;
	}

	SpideyRetailGouraudUiPolyFn retail =
		(SpideyRetailGouraudUiPolyFn)0x00462FB0;
	retail(
		z,
		x,
		y,
		width,
		height,
		color0,
		color1,
		color2,
		color3,
		option10);
}

// @Ok
static void SpideyInstallGameplayUiScaleCompat()
{
'''
if needle not in s: raise SystemExit("UI install needle missing")
s=s.replace(needle,wrappers,1)

# 6) Install common-boss Mysterio fill hooks after the existing health fill hooks.
needle='''	const int healthFlatTwo =
		SpideyPatchDirectCall(
			0x0046499F,
			0x00462D60,
			(void*)&SpideyCompatHealthBarFlatPoly,
			"health_fill_flat_2");

	const unsigned long panelQPolySites[] =
'''
insert='''	const int healthFlatTwo =
		SpideyPatchDirectCall(
			0x0046499F,
			0x00462D60,
			(void*)&SpideyCompatHealthBarFlatPoly,
			"health_fill_flat_2");

	const unsigned long mysterioBossQPolySites[] =
	{
		0x00464C8A,
		0x00464EA5,
		0x004650B0
	};
	int mysterioBossQPolyCalls =
		0;
	for (int mysterioQPolyIndex = 0;
			mysterioQPolyIndex <
				(int)(sizeof(mysterioBossQPolySites) /
				 sizeof(mysterioBossQPolySites[0]));
			++mysterioQPolyIndex)
	{
		mysterioBossQPolyCalls +=
			SpideyPatchDirectCall(
				mysterioBossQPolySites[mysterioQPolyIndex],
				0x00507910,
				(void*)&SpideyCompatMysterioBossQPoly2D,
				"mysterio_boss_fill_qpoly");
	}

	const int mysterioBossFlatCall =
		SpideyPatchDirectCall(
			0x004650EB,
			0x00462D60,
			(void*)&SpideyCompatMysterioBossFlatPoly,
			"mysterio_boss_fill_flat");
	const int mysterioBossGouraudOne =
		SpideyPatchDirectCall(
			0x0046512D,
			0x00462FB0,
			(void*)&SpideyCompatMysterioBossGouraudPoly,
			"mysterio_boss_fill_gouraud_1");
	const int mysterioBossGouraudTwo =
		SpideyPatchDirectCall(
			0x00465162,
			0x00462FB0,
			(void*)&SpideyCompatMysterioBossGouraudPoly,
			"mysterio_boss_fill_gouraud_2");

	const unsigned long panelQPolySites[] =
'''
if needle not in s: raise SystemExit("health install needle missing")
s=s.replace(needle,insert,1)

# 7) Expand install telemetry with Mysterio-specific hook counts.
old='"gameplay_ui_scale_install frame_target=0x00462C30 frame_calls=%d texture_target=0x00462CD0 texture_calls=%d venom_chase_bar_calls=%d venom_chase_bar_policy=level_0x501_shared_top_center_anchor cartridge_text=%d compass_arrow_qpoly=%d compass_live_qpoly_passthrough=2 health_qpoly=%d,%d,%d health_flat=%d,%d panel_qpoly=%d panel_gouraud=%d panel_flat=%d reference=512x240 baseline_output=640x480 policy=compact_holders_compass_arrow_only_cartridge_gouraud_flat_panel_qpoly_passthrough user_percent=%d\\n",'
new='"gameplay_ui_scale_install frame_target=0x00462C30 frame_calls=%d texture_target=0x00462CD0 texture_calls=%d venom_chase_bar_calls=%d venom_chase_bar_policy=level_0x501_shared_top_center_anchor cartridge_text=%d compass_arrow_qpoly=%d compass_live_qpoly_passthrough=2 health_qpoly=%d,%d,%d health_flat=%d,%d mysterio_boss_fill=qpoly:%d,flat:%d,gouraud:%d,%d mysterio_boss_type=311 panel_qpoly=%d panel_gouraud=%d panel_flat=%d reference=512x240 baseline_output=640x480 policy=compact_holders_compass_arrow_only_cartridge_gouraud_flat_panel_qpoly_passthrough user_percent=%d\\n",'
if old not in s: raise SystemExit("UI install format missing")
s=s.replace(old,new,1)
old='''			healthFlatOne,
			healthFlatTwo,
			panelQPolyCalls,
'''
new='''			healthFlatOne,
			healthFlatTwo,
			mysterioBossQPolyCalls,
			mysterioBossFlatCall,
			mysterioBossGouraudOne,
			mysterioBossGouraudTwo,
			panelQPolyCalls,
'''
if old not in s: raise SystemExit("UI install args missing")
s=s.replace(old,new,1)

p.write_text(s,encoding="utf-8")
print("patched Mysterio UI + retail camera guards")
