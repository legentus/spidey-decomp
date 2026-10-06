from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")
needle='''static unsigned long gSpideyQuadBitDcxMismatchCalls =
	0;

'''
insert=r'''static unsigned long gSpideyQuadBitDcxMismatchCalls =
	0;
static unsigned long gSpideyQuadBitHorPlusDrawCalls =
	0;

typedef void (__cdecl *SpideyRetailQPoly3DFn)(
		float, float, float, float, float, u32,
		float, float, float, float, float, u32,
		float, float, float, float, float, u32,
		float, float, float, float, float, u32);

// Retail model projection applies the aspect scalar at 0x00550064 to the
// horizontal projection coefficient. CQuadBit instead projects through
// gte_rtps, whose single GeomScreen value is shared by X and Y and therefore
// cannot receive the Hor+ correction without also changing vertical FOV.
//
// DisplayQuadBitList already converts the fixed 512x240 GTE screen canvas into
// gGameResolutionX/Y before its two QPoly3D calls. Apply the same aspect scalar
// only to horizontal displacement from the modern logical screen center here.
// This keeps vertical placement, depth/RHW, UVs, colors, world coordinates and
// all non-QuadBit rendering untouched.
static float SpideyQuadBitHorPlusX(
		float x)
{
	const float scalar =
		*(float*)0x00550064;
	const float width =
		(float)*(DWORD*)0x00568154;

	if (width <= 0.0f ||
		scalar <= 0.0f)
	{
		return x;
	}

	const float center =
		width * 0.5f;

	return center +
		(x - center) * scalar;
}

static void __cdecl SpideyQuadBitQPoly3DHorPlus(
		float x0, float y0, float z0, float u0, float v0, u32 color0,
		float x1, float y1, float z1, float u1, float v1, u32 color1,
		float x2, float y2, float z2, float u2, float v2, u32 color2,
		float x3, float y3, float z3, float u3, float v3, u32 color3)
{
	SpideyRetailQPoly3DFn retail =
		(SpideyRetailQPoly3DFn)0x00508550;

	const float fixedX0 =
		SpideyQuadBitHorPlusX(x0);
	const float fixedX1 =
		SpideyQuadBitHorPlusX(x1);
	const float fixedX2 =
		SpideyQuadBitHorPlusX(x2);
	const float fixedX3 =
		SpideyQuadBitHorPlusX(x3);

	++gSpideyQuadBitHorPlusDrawCalls;

	if (gSpideyQuadBitHorPlusDrawCalls <= 8)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"DRAW");
		if (f)
		{
			fprintf(
				f,
				"quadbit_horplus sample=%lu scalar=%.6f logical_width=%lu x0=%.3f->%.3f x1=%.3f->%.3f x2=%.3f->%.3f x3=%.3f->%.3f\n",
				gSpideyQuadBitHorPlusDrawCalls,
				(double)*(float*)0x00550064,
				(unsigned long)*(DWORD*)0x00568154,
				(double)x0,
				(double)fixedX0,
				(double)x1,
				(double)fixedX1,
				(double)x2,
				(double)fixedX2,
				(double)x3,
				(double)fixedX3);
			fclose(f);
		}
	}

	retail(
		fixedX0, y0, z0, u0, v0, color0,
		fixedX1, y1, z1, u1, v1, color1,
		fixedX2, y2, z2, u2, v2, color2,
		fixedX3, y3, z3, u3, v3, color3);
}

'''
if needle not in s:
    raise SystemExit("globals needle not found")
s=s.replace(needle,insert,1)
needle2='''static void SpideyInstallQuadBitCameraAnchorCompat()
{
'''
insert2=r'''static void SpideyInstallQuadBitCameraAnchorCompat()
{
	const int horPlusCallOne =
		SpideyPatchDirectCall(
			0x0040A1A9,
			0x00508550,
			(void*)&SpideyQuadBitQPoly3DHorPlus,
			"quadbit_horplus_qpoly3d_1");
	const int horPlusCallTwo =
		SpideyPatchDirectCall(
			0x0040A367,
			0x00508550,
			(void*)&SpideyQuadBitQPoly3DHorPlus,
			"quadbit_horplus_qpoly3d_2");
'''
if needle2 not in s:
    raise SystemExit("install needle not found")
s=s.replace(needle2,insert2,1)
old='''			"quadbit_camera_anchor installed=%d registration_push=0x004081D4 retail_display=0x004097E0 wrapper=0x%08lX camera_transform=0x0056F1E4 gte_set_rot=0x0046D7B0 reason=%s\\n",
			installed,
			(unsigned long)&SpideyDisplayQuadBitListCameraAnchored,
			reason);'''
new='''			"quadbit_camera_anchor installed=%d registration_push=0x004081D4 retail_display=0x004097E0 wrapper=0x%08lX camera_transform=0x0056F1E4 gte_set_rot=0x0046D7B0 horplus_calls=%d,%d horplus_qpoly_wrapper=0x%08lX reason=%s\\n",
			installed,
			(unsigned long)&SpideyDisplayQuadBitListCameraAnchored,
			horPlusCallOne,
			horPlusCallTwo,
			(unsigned long)&SpideyQuadBitQPoly3DHorPlus,
			reason);'''
if old not in s:
    raise SystemExit("log needle not found")
s=s.replace(old,new,1)
p.write_text(s,encoding="utf-8")
print("patched QuadBit horizontal Hor+ projection")
