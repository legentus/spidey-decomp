#include "non_win32.h"

// #define BOOT_GAME
#define MODEL_PREVIEW

#include <stdlib.h>

// #define LOCK_VALIDATION

#include "main.h"
#include "my_assert.h"
#include "ob.h"
#include "vector.h"
#include "friction.h"
#include "bit.h"
#include "front.h"
#include "pshell.h"
#include "baddy.h"
#include "mj.h"
#include "submarin.h"
#include "venom.h"
#include "ps2funcs.h"
#include "blackcat.h"
#include "torch.h"
#include "hostage.h"
#include "cop.h"
#include "carnage.h"
#include "chopper.h"
#include "docock.h"
#include "jonah.h"
#include "lizard.h"
#include "lizman.h"
#include "mysterio.h"
#include "platform.h"
#include "rhino.h"
#include "scorpion.h"
#include "simby.h"
#include "spclone.h"
#include "superock.h"
#include "thug.h"
#include "turret.h"
#include "shell.h"
#include "web.h"
#include "bit2.h"
#include "camera.h"
#include "quat.h"
#include "mem.h"
#include "exp.h"
#include "m3dcolij.h"
#include "m3dinit.h"
#include "spidey.h"
#include "message.h"
#include "bullet.h"
#include "trig.h"
#include "effects.h"
#include "FontTools.h"
#include "wire.h"
#include "powerup.h"
#include "switch.h"
#include "chain.h"
#include "Image.h"
#include "ps2pad.h"
#include "bitmap256.h"
#include "PCTex.h"
#include "smoke.h"
#include "panel.h"
#include "manipob.h"
#include "mess.h"
#include "ai.h"
#include <cstring>
#include "spool.h"
#include "l1a3bomb.h"
#include "chunk.h"
#include "weapons.h"
#include "backgrnd.h"
#include "dcshellutils.h"
#include "pkr.h"
#include "pcdcFile.h"
#include "ps2lowsfx.h"
#include "PCInput.h"
#include "PCShell.h"
#include "stubs.h"
#include "SpideyDX.h"
#include "DXsound.h"
#include "DXinit.h"
#include "pack.h"
#include "pal.h"
#include "db.h"
#include "ps2m3d.h"
#include "PCGfx.h"
#include "ps2gamefmv.h"
#include "init.h"
#include "utils.h"
#include "reloc.h"
#include "my_bink.h"
#include "pcdcMem.h"
#include "dcmemcard.h"
#include "ps2card.h"
#include "pcdcBkup.h"
#include "pcdcPad.h"
#include "vram.h"
#include "m3dzone.h"
#include "PRE.h"
#include "dcfileio.h"
#include "PCMovie.h"
#include "flash.h"


#include "my_patch.h"

extern int FAIL_VALIDATION;

const i32 POLYBUFFERSIZE = 0x17000;

EXPORT i32 gMainStuff[0x1000];

// @Ok
// @Matching
void CalcPolyBufferEnd(void)
{
	PolyBufferEnd = reinterpret_cast<u8*>(
			(reinterpret_cast<u32>(pDoubleBuffer->Polys) + POLYBUFFERSIZE - 0x100) & 0x7FFFFFFF);
}

// @MEDIUMTODO
void SpideyMain(void)
{
	DXERR_printf("xxx main\n");
	for (i32 i = 0; i < 0x1000; i++)
	{
		gMainStuff[i] = 0x4B415453;
	}

	gMainStuff[0] = 0x544C4148;

	Init_AtStart(1);
	PCTex_LoadPcIcons();
	GameFMV_PlayMovie(0, 1, 1, 2.5f);
	GameFMV_PlayMovie(1, 1, 1, 1.0f);
	GameFMV_PlayMovie(2, 1, 1, 1.0f);
	GameFMV_PlayMovie(3, 1, 1, 1.0f);

	Init_Cleanup(0);
	gRunCinemaRelated = 0;

	while (gVlanksRelated)
		;

#ifndef MODEL_PREVIEW
	if (gRenderTest & 8)
#else
	if(1)
#endif
	{
		Spool_ClearAllPSXs();
		PCGfx_DoModelPreview();
		Init_Cleanup(0);
	}
	else
	{
	}
}

// @Ok
// @Matching
// @Leak
void* CClass::operator new(size_t size)
{
	void *pnew = Mem_New(size);

	// Ensure size is a multiple of 4.
	size = ( size + 3 ) & ~0x03;

	// Zero all the newly allocated memory
	u32 *p=(u32 *)pnew;
	for (i32 i=0; i<size/4; ++i) *p++=0;

	return pnew;
}

// @Ok
void CClass::operator delete(void *ptr)
{
	Mem_Delete(ptr);
}

// @Ok
CClass::~CClass()
{
}

template<bool b>

struct StaticAssert{};



template<>

struct StaticAssert<true>

{

	static void assert() {}
};

// @Bogus
void compile_time_assertions(){
	StaticAssert<sizeof(CVector)==12>::assert();
	StaticAssert<sizeof(CFriction)==3>::assert();
	//StaticAssert<sizeof(CBit) == 0x38>::assert();

	//StaticAssert<sizeof(CMenu)==0x53C>::assert();

	//StaticAssert<sizeof(CExpandingBox)==52>::assert();

	StaticAssert<sizeof(CSVector)==6>::assert();

	StaticAssert<sizeof(SVector)==6>::assert();

	StaticAssert<sizeof(CQuadBit)==0x84>::assert();

	//StaticAssert<sizeof(CMJ)==0x324>::assert();

	StaticAssert<sizeof(MATRIX)==0x20>::assert();

	StaticAssert<sizeof(u32)==4>::assert();
	StaticAssert<sizeof(u16)==2>::assert();
	StaticAssert<sizeof(u8)==1>::assert();

	StaticAssert<sizeof(i32)==4>::assert();
	StaticAssert<sizeof(i16)==2>::assert();
	StaticAssert<sizeof(i8)==1>::assert();
}

// @Bogus
extern "C" EXPORT int run_assertions(void)
{
	puts("[*] Starting validation");



	validate_CItem();
	validate_CVector();
	validate_CSVector();
	validate_CBit();
	validate_CQuadBit();
	validate_CFT4Bit();
	validate_CFlatBit();
	validate_CBody();
	validate_SVector();
	validate_CSuper();
	validate_CBaddy();
	validate_CMJ();
	validate_CSubmariner();
	validate_CVenom();
	validate_CBlackCat();
	validate_CTorch();
	validate_CHostage();
	validate_CScriptOnlyBaddy();
	validate_CCop();
	validate_CCarnage();
	validate_CChopper();
	validate_CDocOc();
	validate_CJonah();
	validate_CLizard();
	validate_CLizMan();
	validate_CMystFoot();
	validate_CMysterio();
	validate_CSoftSpot();
	validate_CPlatform();
	validate_CRhino();
	validate_CScorpion();
	validate_CPunchOb();
	validate_CSimby();
	validate_CSimbyBase();
	validate_CSpClone();
	validate_CSuperDocOck();
	validate_CThug();
	validate_CTurret();
	validate_MATRIX();
	validate_SMatrix();
	validate_SJoint();
	validate_CRudeWordHitterSpidey();
	validate_CBulletFrag();
	validate_CImpactWeb();
	validate_CDomePiece();
	validate_CDome();
	validate_CDomeRing();
	validate_CWeb();
	validate_CSwinger();
	validate_CTurretBase();
	validate_CDummy();
	validate_CSniperSplat();
	validate_SStateFlags();
	validate_CGPolyLine();
	validate_CCamera();
	validate_CQuat();
	validate_SBlockHeader();
	validate_SHandle();
	validate_CItemFrag();
	validate_SLineInfo();
	validate_STexWibItemInfo();
	validate_CPlayer();
	validate_CSmokeTrail();
	validate_CMessage();
	validate_CTrapWebEffect();
	validate_CMenu();
	validate_SEntry();
	validate_CBullet();
	validate_SLinkInfo();
	validate_CElectrify();
	validate_CSimbySlimeBase();
	validate_CMysterioLaser();
	validate_Font();
	validate_CTurretLaser();
	validate_CLaserFence();
	validate_CGoldFish();
	validate_CPowerUp();
	validate_CSwitch();
	validate_CChain();
	validate_CGLine();
	validate_SlicedImage2();
	validate_Image();
	validate_SControl();
	validate_Bitmap256();
	validate_SPCTexture();
	validate_CPolyLine();
	validate_CSonicBubble();
	validate_CGlow();
	validate_CLinked2EndedBit();
	validate_CRibbonBit();
	validate_CSniperTarget();
	validate_CVenomWrap();
	validate_CSmokeJet();
	validate_CTexturedRibbon();
	validate_CDomeShockWave();
	validate_CMysterioHeadCircle();
	validate_SAnimFrame();
	validate_CFadePalettes();
	validate_CSimpleTexturedRibbon();
	validate_CManipOb();
	validate_SimpleMessage();
	validate_CShellMysterioHeadGlow();
	validate_CWobblyGlow();
	validate_CSimpleAnim();
	validate_CCopPing();
	validate_SHook();
	validate_Spidey_CIcon();
	validate_CEmber();
	validate_CThugPing();
	validate_CAIProc();
	validate_CAIProc_LookAt();
	validate_Texture();
	validate_CRhinoNasalSteam();
	validate_CAIProc_RotY();
	validate_CAIProc_Fall();
	validate_CAIProc_StateSwitchSendMessage();
	validate_CAIProc_MonitorAttack();
	validate_CAIProc_AccZ();
	validate_SMoveToInfo();
	validate_CAIProc_MoveTo();
	validate_CNonRenderedBit();
	validate_SPSXRegion();
	validate_CSimbyShot();
	validate_CVenomElectrified();
	validate_CCarnageElectrified();
	validate_CConstantLaser();
	validate_CShellSymBurn();
	validate_CExpandingBox();
	validate_CL1A3Bomb();
	validate_CMotionBlur();
	validate_SHitInfo();
	validate_SCommandPoint();
	validate_PendingListEntry();
	validate_CSpecialDisplay();
	validate_CSkidMark();
	validate_TextureEntry();
	validate_CShellVenomElectrified();
	validate_CSkinGoo();
	validate_SSkinGooSource();
	validate_SSkinGooSource2();
	validate_SSkinGooParams();
	validate_CShellCarnageElectrified();
	validate_CShellSuperDocOckElectrified();
	validate_CShellRhinoNasalSteam();
	validate_CShellEmber();
	validate_CShellSimbyMeltSplat();
	validate_CShellSimbyFireDeath();
	validate_CShellGoldFish();
	validate_CShellMysterioHeadCircle();
	validate_SpideyIconRelated();
	validate_CGlowFlash();
	validate_SChainData();
	validate_CSearchlight();
	validate_SFlatBitVelocity();
	validate_CMachineGunBullet();
	validate_CChopperMissile();
	validate_CChunkControl();
	validate_SChunkEntry();
	validate_CGouraudRibbon();
	validate_CCopBulletTracer();
	validate_CCombatImpactRing();
	validate_SCamera();
	validate_SRibbonPoint();
	validate_CRhinoWallImpact();
	validate_CFootprint();
	validate_CChunkSmoke();
	validate_CBouncingRock();
	validate_CFlameExplosion();
	validate_CFrag();
	validate_CPixel();
	validate_CFireySpark();
	validate_CSimbyDroplet();
	validate_CSymBurn();
	validate_CBackground();
	validate_CAngrySpark();
	validate_CBitServer();
	validate_CCarnageHitSpark();
	validate_CChunkBit();
	validate_CTextBox();
	validate_CCopLaserPing();
	validate_CDamagedSoftSpotEffect();
	validate_CElectro();
	validate_CElectroLine();
	validate_CFireyExplosion();
	validate_CFlamingImpactWeb();
	validate_CTripWire();
	validate_CSmokeRing();
	validate_CTexturedRibbon();
	validate_SLineSeg();
	validate_CWibbly();
	validate_SSmokeRingRelated();
	validate_Sprite2();
	validate_SBitServerEntry();
	validate_PKR_FILEINFO();
	validate_PKR_FOOTER();
	validate_PKR_DIRINFO();
	validate_LIBPKR_HANDLE();
	validate_NODE_DIRINFO();
	validate_PVRHeader();
	validate_ClutPC();
	validate_PKR_HEADER();
	validate_SGDOpenFile();
	validate_NODE_FILEINFO();
	validate_SSFXBank();
	validate_SMapping();
	validate_SActionMap();
	validate_SSaveGame();
	validate_MEMORY_ALLOC();
	validate_SMessageProg();
	validate_SLevel();
	validate_SMessage();
	validate_DXsound();
	validate_DXContext();
	validate_DXContextEntry();
	validate_SVideoMode();
	validate_DXVideoModeContext();
	validate_DxZBufferContext();
	validate_DXPOLY();
	validate_SFontEntry();
	validate_SDataGlyph();
	validate_POLY_FT4();
	validate_POLY_GT4();
	validate_SPack();
	validate_tag_S_Pal();
	validate_SViewport();
	validate_SDoubleBuffer();
	validate_SDXPolyField();
	validate_SPCTexPixelFormat();
	validate_SPCTexContainer();
	validate_SAccess();
	validate_AnimPacket();
	validate_SCalcBuffer();
	validate_SCheat();
	validate_SButton();
	validate_DDPIXELFORMAT();
	validate_ConvertPSXPaletteToPC();
	validate_BmpHeader();
	validate_Load8BitBMP2();
	validate_CWibbling3DExplosion();
	validate_C3DExplosion();
	validate_CGrenadeWave();
	validate_CGrenadeExplosion();
	validate_CRipple();
	validate_SSection();
	validate_SFringeQuad();
	validate_SModel();
	validate_SMessageData();
	validate_SSfxEntry();
	validate_reloc_mod();
	validate_SReloc();
	validate_SRelocEntry();
	validate_SMovieDetails();
	validate_BINKSUMMARY();
	validate_BINK();
	validate_matrix4x4();
	validate_vector3d();
	validate_vector4d();
	validate_SScore();
	validate_SRecords();
	validate_SRecordRelated();
	validate_SDCCardTime();
	validate_SCardHead();
	validate_SBackupFile();
	validate_SSaveFile();
	validate_SDCCardFullTime();
	validate_SPdPadBig();
	validate_SPdPadSmall();
	validate_tagSVRAMRect();
	validate_SZone();
	validate_DCSkaterModel();
	validate_DCMaterial();
	validate_DCObject();
	validate_DCStrip();
	validate_DCObjectList();
	validate_DCKeyFrame();
	validate_PREManager();
	validate_CSonicRipple();
	validate_Vector();
	validate_SRhinoData();
	validate_SLight();
	validate_CManipObChunk();
	validate_DB_RECT();
	validate_DR_ENV();
	validate_DRAWENV();
	validate_DISPENV();
	validate_SSfxRelated();
	validate_SSfxAsset();
	validate_CRibbon();
	validate_CGlassBit();
	validate_CSmokeGenerator();
	validate_SDXSoundHolder();
	validate_SDxSomething();
	validate_DSBUFFERDESC();
	validate_TwiddleStuff();
	validate_CSmokePuff();
	validate_SRibbonTexture();
	validate_SSimpleRibbonParams();
	validate_CSpark();
	validate_SIndicator();
	validate_POLY_F3();
	validate_CVenomHitSpark();
	validate_SPushOffset();
	validate_SLink();

	puts("[*] Validation done!");

    return FAIL_VALIDATION;
}

// @Bogus
void runtime_assertions()
{
	int result = run_assertions();

	while(result)
		;
}

// @Bogus
void *my_malloc(size_t s)
{
	void* res = malloc(s);

	return res;
}

// @Bogus
void my_free(void* block)
{
	free(block);
}

// @Bogus
int my_atexit(
   void (MY_CDECL *func )( void )
)
{
	return atexit(func);
}

// @Bogus
void *my_realloc(void *m, size_t s)
{
	void* res = realloc(m, s);

	return res;
}

#ifdef _WIN32
// @Bogus
_onexit_t my_onexit(
   _onexit_t function
)
{
	return _onexit(function);
}
#endif

// @Bogus
void *my_new(size_t s)
{
	void* res = ::operator new(s);
	return res;
}

// @Bogus
void *my_calloc(size_t a, size_t b)
{
	return calloc(a, b);
}

// @Bogus
void patch_alloc(void)
{
	PATCH_PUSH_RET(0x0052A227, my_malloc);
	PATCH_PUSH_RET(0x0052A3C0, my_free);
	PATCH_PUSH_RET(0x00529C39, my_atexit);

	PATCH_PUSH_RET(0x0052F250, my_realloc);
#ifdef _WIN32
	PATCH_PUSH_RET(0x00529BBB, my_onexit);
#endif
	PATCH_PUSH_RET(0x00529BA2, my_new);

	PATCH_PUSH_RET(0x0052C044, my_calloc);
}

// @Bogus
static int my_video_player(const char*, i32)
{
	return 1;
}

#ifdef _WIN32
static volatile DWORD gSpideyCompatSeen = 0;
static volatile DWORD gSpideyCompatWidth = 0;
static volatile DWORD gSpideyCompatHeight = 0;
static volatile DWORD gSpideyCompatBpp = 0;
static volatile DWORD gSpideyCompatRefresh = 0;
static volatile DWORD gSpideyCompatFlags = 0;
static volatile long gSpideyCompatFirstResult = 0;
static volatile long gSpideyCompatRetryResult = 0;
static volatile DWORD gSpideyCompatRetryAttempted = 0;

static void SpideyAppendCompatLog(
		DWORD width,
		DWORD height,
		DWORD bpp,
		HRESULT firstResult,
		HRESULT retryResult,
		int retried)
{
	FILE* f = fopen("spidey-decomp-compat.log", "a");
	if (!f)
		return;

	fprintf(
		f,
		"SetDisplayMode %lux%lux%lu first=0x%08lX",
		(unsigned long)width,
		(unsigned long)height,
		(unsigned long)bpp,
		(unsigned long)firstResult);

	if (retried)
	{
		fprintf(
			f,
			" retry_bpp=32 retry=0x%08lX",
			(unsigned long)retryResult);
	}

	fputc('\n', f);
	fflush(f);
	fclose(f);
}

static HRESULT __stdcall SpideyCompatSetDisplayModeHelper(
		LPDIRECTDRAW7 dd,
		DWORD width,
		DWORD height,
		DWORD bpp,
		DWORD refreshRate,
		DWORD flags)
{
	gSpideyCompatSeen = 1;
	gSpideyCompatWidth = width;
	gSpideyCompatHeight = height;
	gSpideyCompatBpp = bpp;
	gSpideyCompatRefresh = refreshRate;
	gSpideyCompatFlags = flags;
	gSpideyCompatFirstResult = 0x7FFFFFFF;
	gSpideyCompatRetryResult = 0x7FFFFFFF;
	gSpideyCompatRetryAttempted = 0;

	if (!dd)
	{
		SpideyAppendCompatLog(
			width,
			height,
			bpp,
			E_POINTER,
			E_POINTER,
			0);
		return E_POINTER;
	}

	HRESULT hr = dd->SetDisplayMode(
		width,
		height,
		bpp,
		refreshRate,
		flags);
	gSpideyCompatFirstResult = (long)hr;

	if (hr == DDERR_UNSUPPORTED && bpp == 16)
	{
		gSpideyCompatRetryAttempted = 1;
		HRESULT retry = dd->SetDisplayMode(
			width,
			height,
			32,
			refreshRate,
			flags);
		gSpideyCompatRetryResult = (long)retry;

		SpideyAppendCompatLog(
			width,
			height,
			bpp,
			hr,
			retry,
			1);

		if (SUCCEEDED(retry))
		{
			// Retail gColorCount. The caller reads this value later while
			// constructing surfaces and Direct3D state.
			*(DWORD*)0x006B78EC = 32;
			return retry;
		}

		return retry;
	}

	SpideyAppendCompatLog(
		width,
		height,
		bpp,
		hr,
		hr,
		0);
	return hr;
}


static int SpideyModeContextContains(
		DDSURFACEDESC2* surfaces,
		int count,
		DWORD width,
		DWORD height,
		DWORD bpp)
{
	for (int i = 0; i < count; ++i)
	{
		if (surfaces[i].dwWidth == width &&
			surfaces[i].dwHeight == height &&
			surfaces[i].ddpfPixelFormat.dwRGBBitCount == bpp)
		{
			return 1;
		}
	}

	return 0;
}

static int SpideyAppendModernMode(
		DWORD width,
		DWORD height,
		DWORD bpp)
{
	int* count =
		(int*)0x006B5998;
	DDSURFACEDESC2* surfaces =
		(DDSURFACEDESC2*)0x006B599C;
	unsigned char* flags =
		(unsigned char*)0x006B789C;

	if (*count < 0 || *count >= 64)
		return 0;

	if (SpideyModeContextContains(
			surfaces,
			*count,
			width,
			height,
			bpp))
	{
		return 0;
	}

	DDSURFACEDESC2* desc =
		&surfaces[*count];

	memset(desc, 0, sizeof(*desc));
	desc->dwSize = sizeof(*desc);
	desc->dwFlags =
		DDSD_CAPS |
		DDSD_WIDTH |
		DDSD_HEIGHT |
		DDSD_PIXELFORMAT;
	desc->dwWidth = width;
	desc->dwHeight = height;
	desc->ddsCaps.dwCaps = DDSCAPS_3DDEVICE;
	desc->ddpfPixelFormat.dwSize =
		sizeof(desc->ddpfPixelFormat);
	desc->ddpfPixelFormat.dwFlags =
		DDPF_RGB;
	desc->ddpfPixelFormat.dwRGBBitCount =
		bpp;

	// Retail mode flags:
	//   bit 0 = valid mode
	//   bit 2 = hardware/accelerated-resolution candidate.
	// Modern modes should participate in the normal accelerated path.
	flags[*count] =
		1 | 4;

	(*count)++;
	return 1;
}

static void SpideyInjectModernVideoModes()
{
	int before =
		*(int*)0x006B5998;
	int added =
		0;
	int saw1440 =
		0;

	DEVMODEA dm;
	memset(&dm, 0, sizeof(dm));
	dm.dmSize = sizeof(dm);

	for (DWORD index = 0;
		 EnumDisplaySettingsA(0, index, &dm);
		 ++index)
	{
		if (dm.dmPelsWidth < 640 ||
			dm.dmPelsHeight < 480)
		{
			memset(&dm, 0, sizeof(dm));
			dm.dmSize = sizeof(dm);
			continue;
		}

		DWORD bpp =
			dm.dmBitsPerPel;

		if (bpp < 32)
		{
			memset(&dm, 0, sizeof(dm));
			dm.dmSize = sizeof(dm);
			continue;
		}

		if (dm.dmPelsWidth == 2560 &&
			dm.dmPelsHeight == 1440)
		{
			saw1440 =
				1;
		}

		added +=
			SpideyAppendModernMode(
				dm.dmPelsWidth,
				dm.dmPelsHeight,
				32);

		if (*(int*)0x006B5998 >= 64)
			break;

		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
	}

	// 2560x1440 is a first-class supported render size for the modern
	// compatibility path even when legacy DirectDraw mode enumeration does
	// not advertise it. In windowed mode this controls the real offscreen
	// render target and viewport, not a post-process upscale.
	if (*(int*)0x006B5998 < 64)
	{
		added +=
			SpideyAppendModernMode(
				2560,
				1440,
				32);
	}

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"modern_modes before=%d after=%d added=%d windows_1440=%d explicit_2560x1440=1\n",
			before,
			*(int*)0x006B5998,
			added,
			saw1440);
		fclose(f);
	}
}


typedef void (__cdecl *SpideyRetailInitDirectDrawFn)(HWND);

static void __cdecl SpideyCompatInitDirectDraw7(
		HWND hwnd)
{
	SpideyRetailInitDirectDrawFn retail =
		(SpideyRetailInitDirectDrawFn)0x004FEDD0;

	retail(hwnd);
	SpideyInjectModernVideoModes();
}

static void SpideyInstallModernModeReinitCompat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	const unsigned long retailInitDirectDraw =
		0x004FEDD0;

	int patched =
		0;

	for (unsigned char* p = textStart;
		 p + 5 <= textEnd;
		 ++p)
	{
		if (p[0] != 0xE8)
			continue;

		long rel =
			*(long*)(p + 1);

		unsigned long target =
			(unsigned long)(p + 5 + rel);

		if (target != retailInitDirectDraw)
			continue;

		long newRel =
			(long)(
				(unsigned char*)&SpideyCompatInitDirectDraw7 -
				(p + 5));

		*(long*)(p + 1) =
			newRel;

		FlushInstructionCache(
			GetCurrentProcess(),
			p,
			5);

		patched++;
	}

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"modern_mode_reinit patched_calls=%d retail_init=0x004FEDD0 wrapper=0x%08lX\n",
			patched,
			(unsigned long)&SpideyCompatInitDirectDraw7);
		fclose(f);
	}
}

static void SpideyRestoreSavedRenderResolution()
{
	DWORD savedWidth =
		*(DWORD*)0x02E096F8;
	DWORD savedHeight =
		*(DWORD*)0x02E0970C;
	DWORD savedBpp =
		*(DWORD*)0x02E098E4;

	if (savedWidth < 512 ||
		savedWidth > 8192 ||
		savedHeight < 384 ||
		savedHeight > 8192)
	{
		return;
	}

	if (savedBpp != 16 &&
		savedBpp != 24 &&
		savedBpp != 32)
	{
		savedBpp =
			32;
	}

	// RealWinMain resets the live render globals to 640x480 after loading
	// the user's settings. Restore the saved values immediately before
	// retail DXINIT_DirectX8 so surface creation and the D3D viewport are
	// actually native to the selected resolution.
	*(DWORD*)0x006B78E4 =
		savedWidth;
	*(DWORD*)0x006B78E8 =
		savedHeight;
	*(DWORD*)0x006B78EC =
		savedBpp;

	// Retail gGameResolutionX/Y mirrors used by gameplay/render scaling.
	*(DWORD*)0x00568154 =
		savedWidth;
	*(DWORD*)0x00568158 =
		savedHeight;

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"restore_saved_resolution %lux%lux%lu live_dx=%lux%lux%lu\n",
			(unsigned long)savedWidth,
			(unsigned long)savedHeight,
			(unsigned long)savedBpp,
			(unsigned long)*(DWORD*)0x006B78E4,
			(unsigned long)*(DWORD*)0x006B78E8,
			(unsigned long)*(DWORD*)0x006B78EC);
		fclose(f);
	}
}

typedef void (__cdecl *SpideyRetailDXINITFn)(
		HWND,
		HINSTANCE,
		u32);

static void __cdecl SpideyCompatDXINITDirectX8(
		HWND hwnd,
		HINSTANCE hInstance,
		u32 options)
{
	SpideyRestoreSavedRenderResolution();

	SpideyRetailDXINITFn retail =
		(SpideyRetailDXINITFn)0x004FDE90;

	retail(
		hwnd,
		hInstance,
		options | 1);

	SpideyInjectModernVideoModes();
}

static void SpideyInstallWindowedDirectDrawCompat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	const unsigned long dxInitAddress =
		0x004FDE90;

	unsigned char* matchedPush =
		0;
	unsigned char* matchedCall =
		0;
	int matchingCalls =
		0;

	for (unsigned char* p = textStart;
		 p + 5 <= textEnd;
		 ++p)
	{
		if (p[0] != 0xE8)
			continue;

		long rel =
			*(long*)(p + 1);

		unsigned long target =
			(unsigned long)(p + 5 + rel);

		if (target != dxInitAddress)
			continue;

		// The retail RealWinMain call is reconstructed as:
		//     DXINIT_DirectX8(hwnd, hInstance, 2);
		//
		// For cdecl, the right-to-left argument setup puts "push 2"
		// somewhere shortly before this direct call. Search only a small
		// bounded window and require a unique candidate.
		unsigned char* windowStart =
			p >= textStart + 20 ? p - 20 : textStart;

		unsigned char* pushTwo =
			0;
		int pushTwoCount =
			0;

		for (unsigned char* q = windowStart;
			 q + 2 <= p;
			 ++q)
		{
			if (q[0] == 0x6A &&
				q[1] == 0x02)
			{
				pushTwo =
					q;
				pushTwoCount++;
			}
		}

		if (pushTwoCount == 1)
		{
			matchedPush =
				pushTwo;
			matchedCall =
				p;
			matchingCalls++;
		}
	}

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (matchingCalls != 1 ||
		!matchedPush ||
		!matchedCall)
	{
		if (f)
		{
			fprintf(
				f,
				"Windowed DirectDraw patch NOT installed: expected 1 DXINIT_DirectX8 call with one nearby push 2, found %d\n",
				matchingCalls);
			fclose(f);
		}

		printf(
			"[!] Windowed DirectDraw compatibility patch skipped: ambiguous DXINIT_DirectX8 call count %d\n",
			matchingCalls);
		return;
	}

	unsigned long callTarget =
		(unsigned long)(
			matchedCall +
			5 +
			*(long*)(matchedCall + 1));

	if (matchedPush[0] != 0x6A ||
		matchedPush[1] != 0x02 ||
		callTarget != dxInitAddress)
	{
		if (f)
		{
			fprintf(
				f,
				"Windowed DirectDraw patch NOT installed: verification failed push=%08lX call=%08lX target=%08lX\n",
				(unsigned long)matchedPush,
				(unsigned long)matchedCall,
				callTarget);
			fclose(f);
		}

		puts(
			"[!] Windowed DirectDraw compatibility patch skipped: verification failed");
		return;
	}

	// Preserve the proven windowed-mode bit and redirect only this verified
	// RealWinMain call through a same-signature wrapper. The wrapper restores
	// the saved render resolution before retail initialization and augments
	// the legacy mode table afterward.
	matchedPush[1] =
		0x03;

	long wrapperRel =
		(long)(
			(unsigned char*)&SpideyCompatDXINITDirectX8 -
			(matchedCall + 5));

	*(long*)(matchedCall + 1) =
		wrapperRel;

	FlushInstructionCache(
		GetCurrentProcess(),
		matchedPush,
		2);
	FlushInstructionCache(
		GetCurrentProcess(),
		matchedCall,
		5);

	if (f)
	{
		fprintf(
			f,
			"Windowed DirectDraw patch installed push_site=0x%08lX call_site=0x%08lX retail_target=0x%08lX wrapper=0x%08lX old_arg=2 new_arg=3\n",
			(unsigned long)matchedPush,
			(unsigned long)matchedCall,
			callTarget,
			(unsigned long)&SpideyCompatDXINITDirectX8);
		fclose(f);
	}

	printf(
		"[*] Windowed DirectDraw compatibility patch: DXINIT wrapper at 0x%08lX\n",
		(unsigned long)matchedCall);
}
#endif


#ifdef _WIN32
static unsigned long gSpideyPresentFrame = 0;

static int SpideyGetCurrentClientScreenRect(
		HWND hwnd,
		RECT* outRect)
{
	if (!hwnd || !outRect)
		return 0;

	RECT client;
	if (!GetClientRect(hwnd, &client))
		return 0;

	POINT tl;
	POINT br;
	tl.x = client.left;
	tl.y = client.top;
	br.x = client.right;
	br.y = client.bottom;

	if (!ClientToScreen(hwnd, &tl) ||
		!ClientToScreen(hwnd, &br))
	{
		return 0;
	}

	outRect->left = tl.x;
	outRect->top = tl.y;
	outRect->right = br.x;
	outRect->bottom = br.y;
	return 1;
}

static void SpideyLogSurfaceState(
		FILE* f,
		const char* label,
		LPDIRECTDRAWSURFACE7 surface,
		int sampleAsPrimary,
		const RECT* primaryRect)
{
	if (!f)
		return;

	if (!surface)
	{
		fprintf(
			f,
			"%s ptr=0x00000000\n",
			label);
		return;
	}

	DDSURFACEDESC2 desc;
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);

	HRESULT descHr =
		surface->GetSurfaceDesc(&desc);
	HRESULT lostHr =
		surface->IsLost();

	fprintf(
		f,
		"%s ptr=0x%08lX desc_hr=0x%08lX lost_hr=0x%08lX",
		label,
		(unsigned long)surface,
		(unsigned long)descHr,
		(unsigned long)lostHr);

	if (SUCCEEDED(descHr))
	{
		fprintf(
			f,
			" width=%lu height=%lu pitch=%ld bpp=%lu caps=0x%08lX",
			(unsigned long)desc.dwWidth,
			(unsigned long)desc.dwHeight,
			(long)desc.lPitch,
			(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
			(unsigned long)desc.ddsCaps.dwCaps);
	}

	HDC dc = 0;
	HRESULT dcHr =
		surface->GetDC(&dc);

	fprintf(
		f,
		" getdc_hr=0x%08lX",
		(unsigned long)dcHr);

	if (SUCCEEDED(dcHr) && dc)
	{
		int xs[3];
		int ys[3];

		if (sampleAsPrimary && primaryRect)
		{
			int width =
				primaryRect->right - primaryRect->left;
			int height =
				primaryRect->bottom - primaryRect->top;

			xs[0] = primaryRect->left + width / 4;
			xs[1] = primaryRect->left + width / 2;
			xs[2] = primaryRect->left + (width * 3) / 4;
			ys[0] = primaryRect->top + height / 4;
			ys[1] = primaryRect->top + height / 2;
			ys[2] = primaryRect->top + (height * 3) / 4;
		}
		else
		{
			int width =
				SUCCEEDED(descHr) ? (int)desc.dwWidth : 640;
			int height =
				SUCCEEDED(descHr) ? (int)desc.dwHeight : 480;

			xs[0] = width / 4;
			xs[1] = width / 2;
			xs[2] = (width * 3) / 4;
			ys[0] = height / 4;
			ys[1] = height / 2;
			ys[2] = (height * 3) / 4;
		}

		unsigned long sampleHash =
			2166136261UL;
		int nonBlack =
			0;

		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				COLORREF pixel =
					GetPixel(
						dc,
						xs[x],
						ys[y]);

				sampleHash ^=
					(unsigned long)pixel;
				sampleHash *=
					16777619UL;

				if (pixel != RGB(0, 0, 0) &&
					pixel != CLR_INVALID)
				{
					nonBlack++;
				}
			}
		}

		fprintf(
			f,
			" sample_hash=0x%08lX nonblack=%d",
			sampleHash,
			nonBlack);

		surface->ReleaseDC(dc);
	}

	fputc('\n', f);
}

static int SpideyCompatPresentSceneToWindow(
		HWND hwnd,
		LPDIRECTDRAWSURFACE7 scene,
		unsigned long frame,
		int shouldLog)
{
	if (!hwnd || !scene)
		return 0;

	RECT clientRect;
	if (!GetClientRect(hwnd, &clientRect))
		return 0;

	const int dstWidth =
		clientRect.right - clientRect.left;
	const int dstHeight =
		clientRect.bottom - clientRect.top;

	if (dstWidth <= 0 || dstHeight <= 0)
		return 0;

	DDSURFACEDESC2 desc;
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);

	HRESULT descHr =
		scene->GetSurfaceDesc(&desc);

	if (FAILED(descHr) ||
		desc.dwWidth == 0 ||
		desc.dwHeight == 0)
	{
		if (shouldLog)
		{
			FILE* f = fopen(
				"spidey-decomp-present.log",
				"a");
			if (f)
			{
				fprintf(
					f,
					"compat_present frame=%lu skipped scene_desc_hr=0x%08lX\n",
					frame,
					(unsigned long)descHr);
				fclose(f);
			}
		}
		return 0;
	}

	HDC sceneDC = 0;
	HRESULT sceneDCHr =
		scene->GetDC(&sceneDC);

	if (FAILED(sceneDCHr) || !sceneDC)
	{
		if (shouldLog)
		{
			FILE* f = fopen(
				"spidey-decomp-present.log",
				"a");
			if (f)
			{
				fprintf(
					f,
					"compat_present frame=%lu skipped scene_getdc_hr=0x%08lX\n",
					frame,
					(unsigned long)sceneDCHr);
				fclose(f);
			}
		}
		return 0;
	}

	HDC windowDC =
		::GetDC(hwnd);

	BOOL copyOk =
		FALSE;
	DWORD copyError =
		0;
	int usedStretch =
		0;

	if (windowDC)
	{
		if (dstWidth == (int)desc.dwWidth &&
			dstHeight == (int)desc.dwHeight)
		{
			copyOk =
				BitBlt(
					windowDC,
					0,
					0,
					dstWidth,
					dstHeight,
					sceneDC,
					0,
					0,
					SRCCOPY);
		}
		else
		{
			usedStretch =
				1;

			SetStretchBltMode(
				windowDC,
				COLORONCOLOR);

			copyOk =
				StretchBlt(
					windowDC,
					0,
					0,
					dstWidth,
					dstHeight,
					sceneDC,
					0,
					0,
					(int)desc.dwWidth,
					(int)desc.dwHeight,
					SRCCOPY);
		}

		if (!copyOk)
			copyError =
				GetLastError();

		GdiFlush();
		::ReleaseDC(
			hwnd,
			windowDC);
	}
	else
	{
		copyError =
			GetLastError();
	}

	scene->ReleaseDC(
		sceneDC);

	if (shouldLog)
	{
		FILE* f = fopen(
			"spidey-decomp-present.log",
			"a");

		if (f)
		{
			fprintf(
				f,
				"compat_present frame=%lu result=%d error=%lu src=%lux%lu dst=%dx%d stretch=%d\n",
				frame,
				copyOk ? 1 : 0,
				(unsigned long)copyError,
				(unsigned long)desc.dwWidth,
				(unsigned long)desc.dwHeight,
				dstWidth,
				dstHeight,
				usedStretch);
			fclose(f);
		}
	}

	return copyOk ? 1 : 0;
}

typedef void (__cdecl *SpideyRetailFlipFn)(void);

static void __cdecl SpideyDiagDXPOLYFlip(void)
{
	const unsigned long frame =
		++gSpideyPresentFrame;

	HWND hwnd =
		*(HWND*)0x006B58D0;
	RECT* storedRect =
		(RECT*)0x006B5958;

	RECT oldRect =
		*storedRect;
	RECT liveRect;
	memset(&liveRect, 0, sizeof(liveRect));

	int haveLiveRect =
		SpideyGetCurrentClientScreenRect(
			hwnd,
			&liveRect);

	int rectCorrected =
		0;

	if (haveLiveRect &&
		liveRect.right > liveRect.left &&
		liveRect.bottom > liveRect.top &&
		(oldRect.left != liveRect.left ||
		 oldRect.top != liveRect.top ||
		 oldRect.right != liveRect.right ||
		 oldRect.bottom != liveRect.bottom))
	{
		*storedRect =
			liveRect;
		rectCorrected =
			1;
	}

	const int shouldLog =
		frame <= 5 ||
		(frame % 120) == 0 ||
		rectCorrected;

	if (shouldLog)
	{
		FILE* f = fopen(
			"spidey-decomp-present.log",
			"a");

		if (f)
		{
			fprintf(
				f,
				"frame=%lu hwnd=0x%08lX option=%lu lowgfx=%lu saved_res=%lux%lux%lu live_res=%lux%lux%lu old_rect=%ld,%ld,%ld,%ld live_rect=%ld,%ld,%ld,%ld have_live=%d corrected=%d\n",
				frame,
				(unsigned long)hwnd,
				(unsigned long)*(DWORD*)0x006B78F4,
				(unsigned long)*(DWORD*)0x006B78F8,
				(unsigned long)*(DWORD*)0x02E096F8,
				(unsigned long)*(DWORD*)0x02E0970C,
				(unsigned long)*(DWORD*)0x02E098E4,
				(unsigned long)*(DWORD*)0x006B78E4,
				(unsigned long)*(DWORD*)0x006B78E8,
				(unsigned long)*(DWORD*)0x006B78EC,
				(long)oldRect.left,
				(long)oldRect.top,
				(long)oldRect.right,
				(long)oldRect.bottom,
				(long)liveRect.left,
				(long)liveRect.top,
				(long)liveRect.right,
				(long)liveRect.bottom,
				haveLiveRect,
				rectCorrected);

			SpideyLogSurfaceState(
				f,
				"scene_pre",
				*(LPDIRECTDRAWSURFACE7*)0x006B7908,
				0,
				0);

			fclose(f);
		}
	}

	SpideyRetailFlipFn retailFlip =
		(SpideyRetailFlipFn)0x00502990;
	retailFlip();

	if (*(DWORD*)0x006B78F4)
	{
		SpideyCompatPresentSceneToWindow(
			hwnd,
			*(LPDIRECTDRAWSURFACE7*)0x006B7908,
			frame,
			shouldLog);
	}

	if (shouldLog)
	{
		FILE* f = fopen(
			"spidey-decomp-present.log",
			"a");

		if (f)
		{
			RECT finalRect =
				*storedRect;

			SpideyLogSurfaceState(
				f,
				"primary_post",
				*(LPDIRECTDRAWSURFACE7*)0x006B7904,
				1,
				&finalRect);

			fprintf(
				f,
				"frame_end=%lu final_rect=%ld,%ld,%ld,%ld\n",
				frame,
				(long)finalRect.left,
				(long)finalRect.top,
				(long)finalRect.right,
				(long)finalRect.bottom);

			fclose(f);
		}
	}
}


static void SpideyInstallMoviePresentCompat()
{
	unsigned char* start =
		(unsigned char*)0x0050B5A0;
	unsigned char* end =
		(unsigned char*)0x0050B790;
	const unsigned long retailFlip =
		0x00502990;

	unsigned char* match =
		0;
	int count =
		0;

	for (unsigned char* p = start;
		 p + 5 <= end;
		 ++p)
	{
		if (p[0] != 0xE8)
			continue;

		long rel =
			*(long*)(p + 1);

		unsigned long target =
			(unsigned long)(p + 5 + rel);

		if (target == retailFlip)
		{
			match =
				p;
			count++;
		}
	}

	FILE* f = fopen(
		"spidey-decomp-present.log",
		"a");

	if (count != 1 || !match)
	{
		if (f)
		{
			fprintf(
				f,
				"movie_present NOT installed expected=1 found=%d range=0x0050B5A0-0x0050B790\n",
				count);
			fclose(f);
		}
		return;
	}

	long newRel =
		(long)(
			(unsigned char*)&SpideyDiagDXPOLYFlip -
			(match + 5));

	*(long*)(match + 1) =
		newRel;

	FlushInstructionCache(
		GetCurrentProcess(),
		match,
		5);

	if (f)
	{
		fprintf(
			f,
			"movie_present installed call_site=0x%08lX retail_target=0x%08lX wrapper=0x%08lX\n",
			(unsigned long)match,
			retailFlip,
			(unsigned long)&SpideyDiagDXPOLYFlip);
		fclose(f);
	}
}

static void SpideyInstallPresentProbe()
{
	unsigned char* site =
		(unsigned char*)0x00502D41;

	const unsigned char expected[5] =
	{
		0xE8, 0x4A, 0xFC, 0xFF, 0xFF
	};

	FILE* f = fopen(
		"spidey-decomp-present.log",
		"a");

	if (memcmp(
			site,
			expected,
			sizeof(expected)) != 0)
	{
		if (f)
		{
			fprintf(
				f,
				"present_probe NOT installed: unexpected bytes at 0x00502D41: %02X %02X %02X %02X %02X\n",
				site[0],
				site[1],
				site[2],
				site[3],
				site[4]);
			fclose(f);
		}
		return;
	}

	unsigned long oldTarget =
		(unsigned long)(
			site +
			5 +
			*(long*)(site + 1));

	if (oldTarget != 0x00502990)
	{
		if (f)
		{
			fprintf(
				f,
				"present_probe NOT installed: target=0x%08lX expected=0x00502990\n",
				oldTarget);
			fclose(f);
		}
		return;
	}

	long rel =
		(long)(
			(unsigned char*)SpideyDiagDXPOLYFlip -
			(site + 5));

	site[0] =
		0xE8;
	*(long*)(site + 1) =
		rel;

	FlushInstructionCache(
		GetCurrentProcess(),
		site,
		5);

	if (f)
	{
		fprintf(
			f,
			"present_probe installed call_site=0x00502D41 retail_target=0x00502990 wrapper=0x%08lX\n",
			(unsigned long)SpideyDiagDXPOLYFlip);
		fclose(f);
	}
}
#endif

#ifdef _WIN32
static const char gSpideyDxKindDI[] = "DI";
static const char gSpideyDxKindDS[] = "DS";
static const char gSpideyDxKindD3D[] = "D3D";

static void SpideyAppendDxErrorWithCaller(
		const char* kind,
		unsigned long callerReturn,
		long error,
		char* file,
		i32 line)
{
	FILE* f = fopen("spidey-decomp-dxerror.log", "a");
	if (!f)
		return;

	unsigned long callSite =
		callerReturn >= 5 ? callerReturn - 5 : callerReturn;

	fprintf(
		f,
		"%s error=0x%08lX file=%s line=%d caller_return=0x%08lX call_site=0x%08lX\n",
		kind,
		(unsigned long)error,
		file ? file : "<null>",
		line,
		callerReturn,
		callSite);

	if (gSpideyCompatSeen)
	{
		fprintf(
			f,
			"compat_state width=%lu height=%lu bpp=%lu refresh=%lu flags=0x%08lX first=0x%08lX retry_attempted=%lu retry=0x%08lX\n",
			(unsigned long)gSpideyCompatWidth,
			(unsigned long)gSpideyCompatHeight,
			(unsigned long)gSpideyCompatBpp,
			(unsigned long)gSpideyCompatRefresh,
			(unsigned long)gSpideyCompatFlags,
			(unsigned long)gSpideyCompatFirstResult,
			(unsigned long)gSpideyCompatRetryAttempted,
			(unsigned long)gSpideyCompatRetryResult);
	}
	else
	{
		fprintf(f, "compat_state not_seen\n");
	}

	__try
	{
		fprintf(
			f,
			"retail_mode_globals width=%lu height=%lu bpp=%lu\n",
			(unsigned long)*(DWORD*)0x006B78E4,
			(unsigned long)*(DWORD*)0x006B78E8,
			(unsigned long)*(DWORD*)0x006B78EC);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		fprintf(f, "retail_mode_globals <unreadable>\n");
	}

	fprintf(f, "code_window_base=0x%08lX\n", callSite - 96);
	fprintf(f, "code_window_bytes=");

	__try
	{
		const unsigned char* p =
			(const unsigned char*)(callSite - 96);

		for (int i = 0; i < 160; ++i)
		{
			fprintf(f, "%02X", p[i]);
			if (i != 159)
				fputc(' ', f);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		fprintf(f, "<unreadable>");
	}

	fputc('\n', f);
	fflush(f);
	fclose(f);
}

__declspec(naked) static void SpideyDiagDisplayDIError(
		long error,
		char* file,
		i32 line)
{
	__asm
	{
		mov eax, [esp]
		push dword ptr [esp+12]
		push dword ptr [esp+12]
		push dword ptr [esp+12]
		push eax
		push offset gSpideyDxKindDI
		call SpideyAppendDxErrorWithCaller
		add esp, 20
		jmp displayDIError
	}
}

__declspec(naked) static void SpideyDiagDisplayDSError(
		long error,
		char* file,
		i32 line)
{
	__asm
	{
		mov eax, [esp]
		push dword ptr [esp+12]
		push dword ptr [esp+12]
		push dword ptr [esp+12]
		push eax
		push offset gSpideyDxKindDS
		call SpideyAppendDxErrorWithCaller
		add esp, 20
		jmp displayDSError
	}
}

__declspec(naked) static void SpideyDiagDisplayD3DError(
		long error,
		char* file,
		i32 line)
{
	__asm
	{
		mov eax, [esp]
		push dword ptr [esp+12]
		push dword ptr [esp+12]
		push dword ptr [esp+12]
		push eax
		push offset gSpideyDxKindD3D
		call SpideyAppendDxErrorWithCaller
		add esp, 20
		jmp displayD3DError
	}
}
#endif


// @Bogus
void game_patches(void)
{
	//PATCH_CALL(0x004707BE, my_video_player);

#ifdef _WIN32
	SpideyInstallWindowedDirectDrawCompat();
	SpideyInstallModernModeReinitCompat();
	SpideyInstallPresentProbe();
	SpideyInstallMoviePresentCompat();

	PATCH_PUSH_RET(0x004FC240, SpideyDiagDisplayDIError);
	PATCH_PUSH_RET(0x004FC630, SpideyDiagDisplayDSError);
	PATCH_PUSH_RET(0x004FC820, SpideyDiagDisplayD3DError);
#endif

	patch_alloc();

	patch_mem();
	patch_utils();
	patch_ps2funcs();

	patch_pkr();
	patch_pcdcMem();
	patch_pack();
	patch_vram();

	patch_CItem();
	patch_CBody();

	patch_spool();
	patch_trig();
	patch_pctex();
	patch_dcfileio();
	patch_PCMovie();

	patch_flash();
	patch_pshell();
	patch_FontTools();
	patch_mess();
	patch_m3dcolij();
	patch_CSuper();
	patch_ps2m3d();
	patch_m3dutils();
	patch_CBit();
	patch_CFT4Bit();
}

// @Bogus
void runtime_patches(void)
{
#ifdef _WIN32
	LPVOID text_start = (void*)0x00401000;

	SIZE_T text_size = 0x0053B000 - (int)text_start;

	DWORD text_protect;
	VirtualProtect(text_start, text_size, PAGE_EXECUTE_READWRITE, &text_protect);

	game_patches();

	DWORD t;
	VirtualProtect(text_start, text_size, text_protect, &t);
#endif
}

#include "runtime_version.h"

#ifndef RUNTIME_VERSION
#define RUNTIME_VERSION "LOCAL"

#endif

#ifndef _WIN32

int main()
{
	compile_time_assertions();
	return run_assertions();
}


#else


#ifdef _WIN32
typedef LONG (CALLBACK *SpideyVectoredHandlerFn)(EXCEPTION_POINTERS*);
typedef PVOID (WINAPI *SpideyAddVectoredExceptionHandlerFn)(
    ULONG firstHandler,
    SpideyVectoredHandlerFn handler);
typedef BOOL (WINAPI *SpideySetThreadStackGuaranteeFn)(
    PULONG stackSizeInBytes);

// Diagnostic only: capture the first access violation observed by the
// process without relying on the game's top-level exception filter.
static LONG CALLBACK SpideyVectoredExceptionHandler(EXCEPTION_POINTERS* info)
{
    if (!info || !info->ExceptionRecord)
        return EXCEPTION_CONTINUE_SEARCH;

    DWORD exceptionCode =
        info->ExceptionRecord->ExceptionCode;

    if (exceptionCode != EXCEPTION_ACCESS_VIOLATION &&
        exceptionCode != 0xC00000FD)
        return EXCEPTION_CONTINUE_SEARCH;

    FILE* f = fopen("spidey-decomp-crash.log", "w");
    if (!f)
        return EXCEPTION_CONTINUE_SEARCH;

    if (exceptionCode == 0xC00000FD)
        fprintf(f, "spidey-decomp stack overflow\n");
    else
        fprintf(f, "spidey-decomp access violation\n");

    fprintf(f, "exception_code=0x%08lX\n",
        exceptionCode);
    fprintf(f, "exception_address=0x%08lX\n",
        (unsigned long)info->ExceptionRecord->ExceptionAddress);

    if (exceptionCode == EXCEPTION_ACCESS_VIOLATION &&
        info->ExceptionRecord->NumberParameters >= 2)
    {
        unsigned long op =
            (unsigned long)info->ExceptionRecord->ExceptionInformation[0];
        unsigned long target =
            (unsigned long)info->ExceptionRecord->ExceptionInformation[1];

        const char* opName = "unknown";
        if (op == 0)
            opName = "read";
        else if (op == 1)
            opName = "write";
        else if (op == 8)
            opName = "execute";

        fprintf(f, "access_operation=%s\n", opName);
        fprintf(f, "access_target=0x%08lX\n", target);
    }

    MEMORY_BASIC_INFORMATION mbi;
    memset(&mbi, 0, sizeof(mbi));

    if (VirtualQuery(
            info->ExceptionRecord->ExceptionAddress,
            &mbi,
            sizeof(mbi)) == sizeof(mbi))
    {
        char modulePath[MAX_PATH];
        modulePath[0] = '\0';

        HMODULE module = (HMODULE)mbi.AllocationBase;
        if (GetModuleFileNameA(module, modulePath, sizeof(modulePath)))
        {
            fprintf(f, "fault_module=%s\n", modulePath);
            fprintf(f, "fault_module_base=0x%08lX\n",
                (unsigned long)module);
            fprintf(f, "fault_module_offset=0x%08lX\n",
                (unsigned long)info->ExceptionRecord->ExceptionAddress -
                (unsigned long)module);
        }
    }

    if (info->ContextRecord)
    {
        CONTEXT* ctx = info->ContextRecord;

#if defined(_M_IX86)
        fprintf(f, "EAX=0x%08lX\n", ctx->Eax);
        fprintf(f, "EBX=0x%08lX\n", ctx->Ebx);
        fprintf(f, "ECX=0x%08lX\n", ctx->Ecx);
        fprintf(f, "EDX=0x%08lX\n", ctx->Edx);
        fprintf(f, "ESI=0x%08lX\n", ctx->Esi);
        fprintf(f, "EDI=0x%08lX\n", ctx->Edi);
        fprintf(f, "EBP=0x%08lX\n", ctx->Ebp);
        fprintf(f, "ESP=0x%08lX\n", ctx->Esp);
        fprintf(f, "EIP=0x%08lX\n", ctx->Eip);
        fprintf(f, "EFLAGS=0x%08lX\n", ctx->EFlags);

        int stackWordCount =
            exceptionCode == 0xC00000FD ? 128 : 32;

        fprintf(f, "stack_dwords=");
        __try
        {
            unsigned long* sp = (unsigned long*)ctx->Esp;
            for (int i = 0; i < stackWordCount; ++i)
            {
                fprintf(f, "%08lX", sp[i]);
                if (i != stackWordCount - 1)
                    fputc(',', f);
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            fprintf(f, "<unreadable>");
        }
        fputc('\n', f);

        fprintf(f, "ebp_return_chain=");
        __try
        {
            unsigned long* frame =
                (unsigned long*)ctx->Ebp;

            for (int i = 0; i < 64 && frame; ++i)
            {
                unsigned long next =
                    frame[0];
                unsigned long ret =
                    frame[1];

                fprintf(f, "%08lX", ret);

                if (i != 63)
                    fputc(',', f);

                if (!next ||
                    next <= (unsigned long)frame ||
                    next - (unsigned long)frame > 0x100000)
                    break;

                frame =
                    (unsigned long*)next;
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            fprintf(f, "<unreadable>");
        }
        fputc('\n', f);
#endif
    }

    fflush(f);
    fclose(f);
    return EXCEPTION_CONTINUE_SEARCH;
}

static void InstallSpideyCrashHandler()
{
    HMODULE kernel = GetModuleHandleA("kernel32.dll");

    if (kernel)
    {
        SpideySetThreadStackGuaranteeFn setGuarantee =
            (SpideySetThreadStackGuaranteeFn)GetProcAddress(
                kernel,
                "SetThreadStackGuarantee");

        if (setGuarantee)
        {
            ULONG requested =
                64 * 1024;
            ULONG originalRequest =
                requested;

            if (setGuarantee(&requested))
            {
                printf(
                    "[*] Reserved %lu bytes for stack-overflow diagnostics\n",
                    (unsigned long)originalRequest);
            }
            else
            {
                puts(
                    "[!] SetThreadStackGuarantee failed; stack-overflow log may be unavailable");
            }
        }

        SpideyAddVectoredExceptionHandlerFn addHandler =
            (SpideyAddVectoredExceptionHandlerFn)GetProcAddress(
                kernel,
                "AddVectoredExceptionHandler");

        if (addHandler)
        {
            addHandler(1, SpideyVectoredExceptionHandler);
            puts("[*] Installed vectored crash handler");
            return;
        }
    }

    SetUnhandledExceptionFilter(SpideyVectoredExceptionHandler);
    puts("[*] Vectored handler unavailable; installed fallback crash handler");
}
#endif

HMODULE bink_dll;

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved ) 
{
	compile_time_assertions();
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:

			if(GetModuleHandle("tobey_validator.exe") != NULL)
			{
				puts("In validator");
				break;
			}

			AllocConsole();
			SetConsoleTitle("spidey-decomp - " RUNTIME_VERSION);
			freopen("CONOUT$", "w", stdout);
			InstallSpideyCrashHandler();

			bink_dll = GetModuleHandleA("binkw32.dll");


			puts("spidey-decomp starting " RUNTIME_VERSION);

			runtime_assertions();
			runtime_patches();

            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
#endif

// @Bogus
void DoAssert(u8 cond, const char* str, ...)
{
	if (!cond)
	{
		char message[1024];
		message[0] = '\0';

		va_list args;
		va_start(args, str);
		_vsnprintf(
			message,
			sizeof(message) - 1,
			str,
			args);
		va_end(args);

		message[sizeof(message) - 1] = '\0';

		puts(message);

		FILE* f = fopen(
			"spidey-decomp-runtime.log",
			"a");

		if (f)
		{
			fprintf(f, "ASSERT: %s\n", message);
			fclose(f);
		}
	}
}
