from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

needle1='''static void __cdecl SpideyCompatPanelSetCoordsTexture(
		i32 x,
		i32 y,
		POLY_FT4* poly,
		void* texture,
		i32 width,
		i32 height)
{
	SpideyRetailPanelSetCoordsFn retail =
		(SpideyRetailPanelSetCoordsFn)0x00462CD0;

	retail(
		x,
		y,
		poly,
		texture,
		width,
		height);

	SpideyCompactGameplayUiPoly(
		poly,
		"texture");
}
'''

insert1=needle1+'''
static unsigned long gSpideyVenomChaseBarScaledPolys =
	0;
static unsigned long gSpideyVenomChaseBarScaleSamples =
	0;

// @Ok
// The L5A1 Venom chase meter is one composite strip assembled from separate
// textured quads. The generic gameplay-HUD scaler chooses an anchor per quad;
// pieces on opposite sides of the 40/60 percent thresholds can therefore be
// pulled apart. Keep every L5A1 chase-meter piece on one top-center anchor.
static void SpideyCompactVenomChaseBarPoly(
		POLY_FT4* poly)
{
	if (!poly ||
		gSpideyFrontendUiActive ||
		!gSpideyShadowPreviewEnabled ||
		gSpideyModernLogicalWidth <= 640 ||
		gSpideyModernLogicalHeight <= 480)
	{
		return;
	}

	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	if (densityX >= 1.0f &&
		densityY >= 1.0f)
	{
		return;
	}

	const short beforeX0 = poly->x0;
	const short beforeY0 = poly->y0;
	const short beforeX1 = poly->x1;
	const short beforeY1 = poly->y1;
	const short beforeX2 = poly->x2;
	const short beforeY2 = poly->y2;
	const short beforeX3 = poly->x3;
	const short beforeY3 = poly->y3;

	const float anchorX =
		256.0f;
	const float anchorY =
		0.0f;

	poly->x0 =
		SpideyScaleGameplayUiCoord(
			poly->x0,
			anchorX,
			densityX);
	poly->x1 =
		SpideyScaleGameplayUiCoord(
			poly->x1,
			anchorX,
			densityX);
	poly->x2 =
		SpideyScaleGameplayUiCoord(
			poly->x2,
			anchorX,
			densityX);
	poly->x3 =
		SpideyScaleGameplayUiCoord(
			poly->x3,
			anchorX,
			densityX);

	poly->y0 =
		SpideyScaleGameplayUiCoord(
			poly->y0,
			anchorY,
			densityY);
	poly->y1 =
		SpideyScaleGameplayUiCoord(
			poly->y1,
			anchorY,
			densityY);
	poly->y2 =
		SpideyScaleGameplayUiCoord(
			poly->y2,
			anchorY,
			densityY);
	poly->y3 =
		SpideyScaleGameplayUiCoord(
			poly->y3,
			anchorY,
			densityY);

	++gSpideyVenomChaseBarScaledPolys;

	if (gSpideyVenomChaseBarScaleSamples <
		32)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"venom_chase_bar_scale level=0x501 policy=shared_top_center_anchor logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%d,%d,%d,%d,%d,%d,%d,%d after=%d,%d,%d,%d,%d,%d,%d,%d count=%lu\\n",
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				(double)densityX,
				(double)densityY,
				gSpideyGameplayUiScalePercent,
				(int)beforeX0,
				(int)beforeY0,
				(int)beforeX1,
				(int)beforeY1,
				(int)beforeX2,
				(int)beforeY2,
				(int)beforeX3,
				(int)beforeY3,
				(int)poly->x0,
				(int)poly->y0,
				(int)poly->x1,
				(int)poly->y1,
				(int)poly->x2,
				(int)poly->y2,
				(int)poly->x3,
				(int)poly->y3,
				gSpideyVenomChaseBarScaledPolys);
			fclose(log);
		}

		++gSpideyVenomChaseBarScaleSamples;
	}
}

// @Ok
static void __cdecl SpideyCompatVenomChaseBarSetCoordsTexture(
		i32 x,
		i32 y,
		POLY_FT4* poly,
		void* texture,
		i32 width,
		i32 height)
{
	SpideyRetailPanelSetCoordsFn retail =
		(SpideyRetailPanelSetCoordsFn)0x00462CD0;

	retail(
		x,
		y,
		poly,
		texture,
		width,
		height);

	if (SpideyRetailGetLevelId() ==
		0x501)
	{
		SpideyCompactVenomChaseBarPoly(
			poly);
	}
	else
	{
		SpideyCompactGameplayUiPoly(
			poly,
			"texture");
	}
}
'''

needle2='''	const int cartridgeTextInstalled =
		SpideyPatchDirectCall(
			0x00465A83,
			0x00458700,
			(void*)&SpideyCompatCartridgeCountText,
			"cartridge_count_text");
'''

insert2='''	const unsigned long venomChaseBarCoordSites[] =
	{
		0x004E7F44,
		0x004E8160,
		0x004E837E,
		0x004E859F,
		0x004E87B6
	};
	int venomChaseBarCoordCalls =
		0;
	for (int venomBarIndex = 0;
			venomBarIndex <
				(int)(sizeof(venomChaseBarCoordSites) /
				 sizeof(venomChaseBarCoordSites[0]));
			++venomBarIndex)
	{
		venomChaseBarCoordCalls +=
			SpideyPatchDirectCall(
				venomChaseBarCoordSites[venomBarIndex],
				(unsigned long)
					(void*)&SpideyCompatPanelSetCoordsTexture,
				(void*)&SpideyCompatVenomChaseBarSetCoordsTexture,
				"venom_chase_bar_shared_anchor");
	}

'''+needle2

needle3='''			"gameplay_ui_scale_install frame_target=0x00462C30 frame_calls=%d texture_target=0x00462CD0 texture_calls=%d cartridge_text=%d compass_arrow_qpoly=%d compass_live_qpoly_passthrough=2 health_qpoly=%d,%d,%d health_flat=%d,%d panel_qpoly=%d panel_gouraud=%d panel_flat=%d reference=512x240 baseline_output=640x480 policy=compact_holders_compass_arrow_only_cartridge_gouraud_flat_panel_qpoly_passthrough user_percent=%d\\n",
			frameCalls,
			textureCalls,
			cartridgeTextInstalled,
'''

insert3='''			"gameplay_ui_scale_install frame_target=0x00462C30 frame_calls=%d texture_target=0x00462CD0 texture_calls=%d venom_chase_bar_calls=%d venom_chase_bar_policy=level_0x501_shared_top_center_anchor cartridge_text=%d compass_arrow_qpoly=%d compass_live_qpoly_passthrough=2 health_qpoly=%d,%d,%d health_flat=%d,%d panel_qpoly=%d panel_gouraud=%d panel_flat=%d reference=512x240 baseline_output=640x480 policy=compact_holders_compass_arrow_only_cartridge_gouraud_flat_panel_qpoly_passthrough user_percent=%d\\n",
			frameCalls,
			textureCalls,
			venomChaseBarCoordCalls,
			cartridgeTextInstalled,
'''

for label,needle in [("wrapper",needle1),("install",needle2),("log",needle3)]:
    if needle not in s:
        raise SystemExit(label+" needle not found")

s=s.replace(needle1,insert1,1)
s=s.replace(needle2,insert2,1)
s=s.replace(needle3,insert3,1)
p.write_text(s,encoding="utf-8")
print("patched main.cpp")
