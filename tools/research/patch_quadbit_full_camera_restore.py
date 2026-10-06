from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")
start=s.index("typedef void (__cdecl *SpideyRetailDisplayQuadBitListFn)(")
end=s.index("static void SpideyInstallQuadBitCameraAnchorCompat()",start)
new=r'''typedef void (__cdecl *SpideyRetailDisplayQuadBitListFn)(
		void**);
typedef void (__cdecl *SpideyRetailSetRotMatrixFn)(
		MATRIX*);
typedef void (__cdecl *SpideyRetailMatrix4x4MulFn)(
		float*,
		const float*,
		const float*);

static unsigned long gSpideyQuadBitCameraRestoreCalls =
	0;
static unsigned long gSpideyQuadBitDcxMismatchCalls =
	0;

// Retail DisplayQuadBitList uses two camera-side transform paths.
//
// 1) GTE path: subtract gMikeCamera.Position, then gte_rtps through the active
//    camera rotation matrix.
// 2) DCX path: transform each original world-space corner through the 4x4
//    matrix copied from 0x0056E6F8 into the active DCX matrix at 0x0056E668.
//
// M3d_RenderSetup builds the pristine per-frame DCX camera/projection matrix as:
//     matrix4x4_ml(result, 0x0056E778, 0x0056E570)
// and copies that result to 0x0056E6F8. Model rendering later reuses and
// overwrites 0x0056E6F8 with model-local transforms. Restoring only the GTE
// rotation therefore leaves world-space QuadBits in a mixed camera/model space.
//
// Rebuild the exact retail DCX camera/projection matrix and restore the GTE
// camera rotation immediately before the untouched retail QuadBit renderer.
static void __cdecl SpideyDisplayQuadBitListCameraAnchored(
		void** list)
{
	SpideyRetailSetRotMatrixFn setRotMatrix =
		(SpideyRetailSetRotMatrixFn)0x0046D7B0;
	SpideyRetailMatrix4x4MulFn matrixMul =
		(SpideyRetailMatrix4x4MulFn)0x00476A00;
	SpideyRetailDisplayQuadBitListFn retail =
		(SpideyRetailDisplayQuadBitListFn)0x004097E0;

	MATRIX* activeCameraTransform =
		(MATRIX*)0x0056F1E4;
	float* dcxCamera =
		(float*)0x0056E778;
	float* dcxProjection =
		(float*)0x0056E570;
	float* dcxCombined =
		(float*)0x0056E6F8;
	float rebuiltDcx[16];

	matrixMul(
		rebuiltDcx,
		dcxCamera,
		dcxProjection);

	++gSpideyQuadBitCameraRestoreCalls;
	if (memcmp(
			dcxCombined,
			rebuiltDcx,
			sizeof(rebuiltDcx)) !=
		0)
	{
		++gSpideyQuadBitDcxMismatchCalls;
	}

	memcpy(
		dcxCombined,
		rebuiltDcx,
		sizeof(rebuiltDcx));

	setRotMatrix(
		activeCameraTransform);
	retail(
		list);
}

'''
p.write_text(s[:start]+new+s[end:],encoding="utf-8")
print("patched full QuadBit camera/DCX restore")
