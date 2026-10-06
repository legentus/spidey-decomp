from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")

needle='''static unsigned long gSpideyMysterioBossUiScaledDraws =
	0;
'''
if needle not in s:
    # locate exact existing declaration
    idx=s.find("gSpideyMysterioBossUiScaledDraws")
    print("near declaration:", s[idx-120:idx+180])
    raise SystemExit("declaration needle not found")
insert='''static unsigned long gSpideyMysterioBossUiScaledDraws =
	0;
static unsigned long gSpideyMysterioHealthTelemetrySamples =
	0;

static void SpideyLogMysterioHealthRect(
		const char* source,
		float x0,
		float y0,
		float x1,
		float y1)
{
	if (gSpideyMysterioHealthTelemetrySamples >= 96)
		return;

	FILE* log =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (log)
	{
		fprintf(
			log,
			"mysterio_health_alignment source=%s sample=%lu logical=%lux%lu density_user=%d rect=%.2f,%.2f,%.2f,%.2f live_if_512x240=%.2f,%.2f,%.2f,%.2f\\n",
			source ? source : "unknown",
			gSpideyMysterioHealthTelemetrySamples,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight,
			gSpideyGameplayUiScalePercent,
			(double)x0,
			(double)y0,
			(double)x1,
			(double)y1,
			(double)x0 * (double)gSpideyModernLogicalWidth / 512.0,
			(double)y0 * (double)gSpideyModernLogicalHeight / 240.0,
			(double)x1 * (double)gSpideyModernLogicalWidth / 512.0,
			(double)y1 * (double)gSpideyModernLogicalHeight / 240.0);
		fclose(log);
	}

	++gSpideyMysterioHealthTelemetrySamples;
}

'''
s=s.replace(needle,insert,1)

# Insert dedicated holder wrappers immediately before Mysterio QPoly wrapper.
needle2='''// @Ok
static void __cdecl SpideyCompatMysterioBossQPoly2D(
'''
insert2=r'''// @Ok
static void __cdecl SpideyCompatMysterioBossHolderTexture(
		i32 x,
		i32 y,
		POLY_FT4* poly,
		void* texture,
		i32 width,
		i32 height)
{
	SpideyCompatPanelSetCoordsTexture(
		x,
		y,
		poly,
		texture,
		width,
		height);

	if (SpideyIsMysterioBossActive() &&
		poly)
	{
		SpideyLogMysterioHealthRect(
			"holder_texture_authored_after_compact",
			(float)poly->x0,
			(float)poly->y0,
			(float)poly->x3,
			(float)poly->y3);
	}
}

// @Ok
static void __cdecl SpideyCompatMysterioBossHolderFrame(
		i32 x,
		i32 y,
		POLY_FT4* poly,
		void* frame,
		i32 width,
		i32 height)
{
	SpideyCompatPanelSetCoordsFrame(
		x,
		y,
		poly,
		frame,
		width,
		height);

	if (SpideyIsMysterioBossActive() &&
		poly)
	{
		SpideyLogMysterioHealthRect(
			"holder_frame_authored_after_compact",
			(float)poly->x0,
			(float)poly->y0,
			(float)poly->x3,
			(float)poly->y3);
	}
}

// @Ok
static void __cdecl SpideyCompatMysterioBossQPoly2D(
'''
if needle2 not in s:
    raise SystemExit("qpoly wrapper marker not found")
s=s.replace(needle2,insert2,1)

# Add input logging to qpoly active branch.
needle3='''	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyCompatHealthBarQPoly2D(
'''
replace3='''	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyLogMysterioHealthRect(
			"fill_qpoly_live_before_compact",
			x0,
			y0,
			x3,
			y3);
		SpideyCompatHealthBarQPoly2D(
'''
if needle3 not in s:
    raise SystemExit("qpoly active branch not found")
s=s.replace(needle3,replace3,1)

# Flat active branch: add authored input log.
needle4='''	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyCompatPanelFlatPoly(
'''
replace4='''	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyLogMysterioHealthRect(
			"fill_flat_authored_before_compact",
			(float)x,
			(float)y,
			(float)(x + width),
			(float)(y + height));
		SpideyCompatPanelFlatPoly(
'''
if needle4 not in s:
    raise SystemExit("flat active branch not found")
s=s.replace(needle4,replace4,1)

# Gouraud active branch.
needle5='''	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyCompatPanelGouraudPoly(
'''
replace5='''	if (SpideyIsMysterioBossActive())
	{
		++gSpideyMysterioBossUiScaledDraws;
		SpideyLogMysterioHealthRect(
			"fill_gouraud_authored_before_compact",
			(float)x,
			(float)y,
			(float)(x + width),
			(float)(y + height));
		SpideyCompatPanelGouraudPoly(
'''
if needle5 not in s:
    raise SystemExit("gouraud active branch not found")
s=s.replace(needle5,replace5,1)

# Install exact holder call wrappers after broad frame/texture patching.
needle6='''	const int textureCalls =
		SpideyPatchAllRetailDirectCalls(
			0x00462CD0,
			(void*)&SpideyCompatPanelSetCoordsTexture);

'''
replace6='''	const int textureCalls =
		SpideyPatchAllRetailDirectCalls(
			0x00462CD0,
			(void*)&SpideyCompatPanelSetCoordsTexture);

	const int mysterioHolderTextureCall =
		SpideyPatchDirectCall(
			0x00464CDE,
			(unsigned long)(void*)&SpideyCompatPanelSetCoordsTexture,
			(void*)&SpideyCompatMysterioBossHolderTexture,
			"mysterio_health_holder_texture");
	const int mysterioHolderFrameCall =
		SpideyPatchDirectCall(
			0x00464EF8,
			(unsigned long)(void*)&SpideyCompatPanelSetCoordsFrame,
			(void*)&SpideyCompatMysterioBossHolderFrame,
			"mysterio_health_holder_frame");

'''
if needle6 not in s:
    raise SystemExit("installer broad patch block not found")
s=s.replace(needle6,replace6,1)

# Extend install log with holder hook statuses without altering vararg order too broadly.
old='mysterio_boss_fill=qpoly:%d,flat:%d,gouraud:%d,%d mysterio_boss_type=311'
new='mysterio_boss_fill=qpoly:%d,flat:%d,gouraud:%d,%d mysterio_holders=texture:%d,frame:%d mysterio_boss_type=311'
if old not in s:
    raise SystemExit("install log format token not found")
s=s.replace(old,new,1)

# Insert the two new arguments immediately after existing gouraud arguments.
needle7='''			mysterioBossGouraudOne,
			mysterioBossGouraudTwo,
			panelQPolyCalls,'''
replace7='''			mysterioBossGouraudOne,
			mysterioBossGouraudTwo,
			mysterioHolderTextureCall,
			mysterioHolderFrameCall,
			panelQPolyCalls,'''
if needle7 not in s:
    raise SystemExit("install log args token not found")
s=s.replace(needle7,replace7,1)

p.write_text(s,encoding="utf-8")
print("patched Mysterio holder/fill alignment telemetry")
