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
#include "renderer11_legacy_bridge.h"


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

static int gSpideyDpiAware = 0;

typedef BOOL (WINAPI *SpideySetProcessDPIAwareFn)(void);
typedef BOOL (WINAPI *SpideyIsProcessDPIAwareFn)(void);

static void SpideyEnableDpiAwarenessEarly()
{
	HMODULE user32 =
		GetModuleHandleA(
			"user32.dll");

	BOOL setResult =
		FALSE;
	BOOL awareResult =
		FALSE;

	if (user32)
	{
		SpideyIsProcessDPIAwareFn isAware =
			(SpideyIsProcessDPIAwareFn)GetProcAddress(
				user32,
				"IsProcessDPIAware");

		SpideySetProcessDPIAwareFn setAware =
			(SpideySetProcessDPIAwareFn)GetProcAddress(
				user32,
				"SetProcessDPIAware");

		if (isAware)
		{
			awareResult =
				isAware();
		}

		if (!awareResult &&
			setAware)
		{
			setResult =
				setAware();

			if (isAware)
			{
				awareResult =
					isAware();
			}
			else
			{
				awareResult =
					setResult;
			}
		}
	}

	gSpideyDpiAware =
		awareResult ? 1 : 0;

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"dpi_awareness user32=0x%08lX set_result=%d process_aware=%d metrics=%dx%d\n",
			(unsigned long)user32,
			setResult ? 1 : 0,
			gSpideyDpiAware,
			GetSystemMetrics(0),
			GetSystemMetrics(1));
		fclose(f);
	}
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
			// DX11 now owns the visible output, so 2560x1440 must remain
			// selectable in the retail Screen Size row. The old D3D7
			// CreateDevice limitation is handled later by remapping only the
			// hidden legacy backing surface to a known-good physical size.
			saw1440 =
				1;
		}

		if (gSpideyDpiAware &&
			((int)dm.dmPelsWidth > GetSystemMetrics(0) ||
			 (int)dm.dmPelsHeight > GetSystemMetrics(1)))
		{
			memset(&dm, 0, sizeof(dm));
			dm.dmSize = sizeof(dm);
			continue;
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


	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"modern_modes before=%d after=%d added=%d windows_1440=%d ui_2560x1440_exposed=1 legacy_backing_remap=1 dpi_aware=%d metrics=%dx%d\n",
			before,
			*(int*)0x006B5998,
			added,
			saw1440,
			gSpideyDpiAware,
			GetSystemMetrics(0),
			GetSystemMetrics(1));
		fclose(f);
	}
}


static void SpideyKeepBorderlessMonitorWindow(HWND hwnd)
{
	if (!hwnd)
		return;

	// Keep this compatible with the project's original Windows 98-era SDK.
	// The matching toolchain does not expose MonitorFromWindow/MONITORINFO.
	// The retail game already uses GetSystemMetrics(0/1) for its fullscreen
	// popup window, so use the same primary-display dimensions here.
	const int left =
		0;
	const int top =
		0;
	const int width =
		GetSystemMetrics(0);
	const int height =
		GetSystemMetrics(1);

	if (width <= 0 || height <= 0)
		return;

	LONG style =
		GetWindowLongA(
			hwnd,
			GWL_STYLE);

	style &= ~(
		WS_CAPTION |
		WS_THICKFRAME |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX |
		WS_SYSMENU);
	style |=
		WS_POPUP | WS_VISIBLE;

	SetWindowLongA(
		hwnd,
		GWL_STYLE,
		style);

	SetWindowPos(
		hwnd,
		HWND_TOP,
		left,
		top,
		width,
		height,
		SWP_FRAMECHANGED |
		SWP_SHOWWINDOW);

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"borderless_monitor_window rect=%d,%d,%d,%d size=%dx%d source=GetSystemMetrics\n",
			left,
			top,
			left + width,
			top + height,
			width,
			height);
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
	SpideyKeepBorderlessMonitorWindow(hwnd);
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

static unsigned long gSpideySelectedOutputWidth = 640;
static unsigned long gSpideySelectedOutputHeight = 480;
static unsigned long gSpideySelectedOutputBpp = 32;

static void SpideyRestoreSavedRenderResolution()
{
	DWORD requestedWidth =
		*(DWORD*)0x02E096F8;
	DWORD requestedHeight =
		*(DWORD*)0x02E0970C;
	DWORD requestedBpp =
		*(DWORD*)0x02E098E4;

	if (requestedWidth < 512 ||
		requestedWidth > 8192 ||
		requestedHeight < 384 ||
		requestedHeight > 8192)
	{
		return;
	}

	if (requestedBpp != 16 &&
		requestedBpp != 24 &&
		requestedBpp != 32)
	{
		requestedBpp =
			32;
	}

	gSpideySelectedOutputWidth =
		requestedWidth;
	gSpideySelectedOutputHeight =
		requestedHeight;
	gSpideySelectedOutputBpp =
		32;

	DWORD physicalWidth =
		requestedWidth;
	DWORD physicalHeight =
		requestedHeight;
	DWORD physicalBpp =
		requestedBpp;

	int remappedLegacyBacking =
		0;

	// 2560x1440 is now a valid *DX11 output* selection. The retail D3D7
	// device still rejects a 2560x1440 scene surface with
	// DDERR_INVALIDOBJECT, so quarantine only the hidden legacy backing
	// surface. 1920x1440 is runtime-verified on the same machine and keeps
	// the maximum known-good vertical resolution while DX11 renders the
	// requested output independently.
	if (requestedWidth == 2560 &&
		requestedHeight == 1440)
	{
		physicalWidth =
			1920;
		physicalHeight =
			1440;
		physicalBpp =
			32;
		remappedLegacyBacking =
			1;
	}

	// Keep the persisted/user-facing setting exactly as selected. Only the
	// live D3D7 globals receive the compatibility backing dimensions.
	*(DWORD*)0x02E096F8 =
		requestedWidth;
	*(DWORD*)0x02E0970C =
		requestedHeight;
	*(DWORD*)0x02E098E4 =
		32;

	*(DWORD*)0x006B78E4 =
		physicalWidth;
	*(DWORD*)0x006B78E8 =
		physicalHeight;
	*(DWORD*)0x006B78EC =
		physicalBpp;

	// The logical game viewport follows the selected output. Retail frontend
	// code may temporarily switch this back to 640x480 later; gameplay mode
	// restores the selected dimensions through the display-options wrapper.
	*(DWORD*)0x00568154 =
		requestedWidth;
	*(DWORD*)0x00568158 =
		requestedHeight;

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"restore_saved_resolution selected=%lux%lux%lu physical=%lux%lux%lu legacy_backing_remap=%d preserve_selected=1\n",
			(unsigned long)requestedWidth,
			(unsigned long)requestedHeight,
			(unsigned long)gSpideySelectedOutputBpp,
			(unsigned long)physicalWidth,
			(unsigned long)physicalHeight,
			(unsigned long)physicalBpp,
			remappedLegacyBacking);
		fclose(f);
	}
}

static int gSpideyModernAspectEnabled = 1;
static int gSpideyFrontendLegacyMode = 1;
static unsigned long gSpideyModernLogicalWidth = 0;
static unsigned long gSpideyModernLogicalHeight = 0;
static unsigned long gSpideyLegacyPhysicalWidth = 640;
static unsigned long gSpideyLegacyPhysicalHeight = 480;

static void SpideyRefreshModernLogicalResolution()
{
	unsigned long width =
		gSpideySelectedOutputWidth;
	unsigned long height =
		gSpideySelectedOutputHeight;

	if (width < 640 ||
		width > 8192 ||
		height < 480 ||
		height > 8192)
	{
		HWND hwnd =
			*(HWND*)0x006B58D0;

		width =
			0;
		height =
			0;

		if (hwnd)
		{
			RECT client;
			if (GetClientRect(hwnd, &client))
			{
				width =
					(unsigned long)(client.right - client.left);
				height =
					(unsigned long)(client.bottom - client.top);
			}
		}

		if (!width || !height)
		{
			width =
				(unsigned long)GetSystemMetrics(0);
			height =
				(unsigned long)GetSystemMetrics(1);
		}
	}

	if (width < 640 || height < 480)
	{
		width = 640;
		height = 480;
	}

	gSpideyModernLogicalWidth =
		width;
	gSpideyModernLogicalHeight =
		height;
}

static int SpideyUseModernGameplayAspect()
{
	return gSpideyModernAspectEnabled &&
		!gSpideyFrontendLegacyMode &&
		gSpideyModernLogicalWidth >= 640 &&
		gSpideyModernLogicalHeight >= 480;
}

static void SpideyApplyLogicalRenderResolution(
		int useModern,
		const char* reason)
{
	unsigned long width =
		gSpideyLegacyPhysicalWidth;
	unsigned long height =
		gSpideyLegacyPhysicalHeight;

	if (useModern &&
		SpideyUseModernGameplayAspect())
	{
		width =
			gSpideyModernLogicalWidth;
		height =
			gSpideyModernLogicalHeight;
	}

	if (!width || !height)
		return;

	*(DWORD*)0x00568154 =
		width;
	*(DWORD*)0x00568158 =
		height;

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"logical_render_resolution reason=%s modern=%d frontend=%d logical=%lux%lu physical=%lux%lu selected=%lux%lu\n",
			reason ? reason : "unknown",
			useModern ? 1 : 0,
			gSpideyFrontendLegacyMode,
			width,
			height,
			gSpideyLegacyPhysicalWidth,
			gSpideyLegacyPhysicalHeight,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight);
		fclose(f);
	}
}

typedef void (__cdecl *SpideyRetailSetDisplayOptionsFn)(
		u32,
		u32,
		u32,
		i32,
		i32);

static void SpideyInstallRetailD3D7DrawProbe(void);
static void SpideyFlushRetailD3D7DrawProbeFrame(
		unsigned long frame);

static void __cdecl SpideyCompatSetDisplayOptions(
		u32 width,
		u32 height,
		u32 bpp,
		i32 option4,
		i32 option5)
{
	const u32 requestedWidth =
		width;
	const u32 requestedHeight =
		height;
	const u32 requestedBpp =
		bpp;

	const int frontendLegacy =
		*(DWORD*)0x006B78F4 &&
		requestedWidth == 640 &&
		requestedHeight == 480 &&
		requestedBpp == 16 &&
		option4 == 0 &&
		option5 == 4;

	u32 physicalWidth =
		requestedWidth;
	u32 physicalHeight =
		requestedHeight;
	u32 physicalBpp =
		requestedBpp;

	int remappedLegacyBacking =
		0;

	if (!frontendLegacy)
	{
		gSpideySelectedOutputWidth =
			requestedWidth;
		gSpideySelectedOutputHeight =
			requestedHeight;
		gSpideySelectedOutputBpp =
			32;

		// Modern DX11 output is always 32-bit. Keep legacy physical D3D7
		// below the exact 2560x1440 target that is known to fail CreateDevice.
		physicalBpp =
			32;

		if (requestedWidth == 2560 &&
			requestedHeight == 1440)
		{
			physicalWidth =
				1920;
			physicalHeight =
				1440;
			remappedLegacyBacking =
				1;
		}
	}

	SpideyRetailSetDisplayOptionsFn retail =
		(SpideyRetailSetDisplayOptionsFn)0x00500250;

	retail(
		physicalWidth,
		physicalHeight,
		physicalBpp,
		option4,
		option5);

	gSpideyFrontendLegacyMode =
		frontendLegacy ? 1 : 0;

	gSpideyLegacyPhysicalWidth =
		(unsigned long)*(DWORD*)0x006B78E4;
	gSpideyLegacyPhysicalHeight =
		(unsigned long)*(DWORD*)0x006B78E8;

	if (!gSpideyLegacyPhysicalWidth ||
		!gSpideyLegacyPhysicalHeight)
	{
		gSpideyLegacyPhysicalWidth =
			(unsigned long)physicalWidth;
		gSpideyLegacyPhysicalHeight =
			(unsigned long)physicalHeight;
	}

	// Retail is free to touch its saved-resolution fields while rebuilding
	// the D3D7 device. Restore the user-facing output selection afterward.
	*(DWORD*)0x02E096F8 =
		(DWORD)gSpideySelectedOutputWidth;
	*(DWORD*)0x02E0970C =
		(DWORD)gSpideySelectedOutputHeight;
	*(DWORD*)0x02E098E4 =
		(DWORD)gSpideySelectedOutputBpp;

	SpideyInjectModernVideoModes();
	SpideyKeepBorderlessMonitorWindow(
		*(HWND*)0x006B58D0);

	SpideyRefreshModernLogicalResolution();
	SpideyApplyLogicalRenderResolution(
		!gSpideyFrontendLegacyMode,
		gSpideyFrontendLegacyMode ?
			"display_options_frontend" :
			"display_options_gameplay");

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"display_options selected=%lux%lux%lu physical=%lux%lux%lu option4=%d option5=%d frontend_legacy=%d legacy_backing_remap=%d preserve_selected=1\n",
			(unsigned long)gSpideySelectedOutputWidth,
			(unsigned long)gSpideySelectedOutputHeight,
			(unsigned long)gSpideySelectedOutputBpp,
			(unsigned long)physicalWidth,
			(unsigned long)physicalHeight,
			(unsigned long)physicalBpp,
			option4,
			option5,
			frontendLegacy,
			remappedLegacyBacking);
		fclose(f);
	}

	// Display-option changes can destroy/recreate the retail D3D7 device.
	SpideyInstallRetailD3D7DrawProbe();
}

static void SpideyInstallDisplayOptionsCompat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	const unsigned long retailSetDisplayOptions =
		0x00500250;

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

		if (target != retailSetDisplayOptions)
			continue;

		long newRel =
			(long)(
				(unsigned char*)&SpideyCompatSetDisplayOptions -
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
			"display_options_compat patched_calls=%d retail=0x00500250 wrapper=0x%08lX\n",
			patched,
			(unsigned long)&SpideyCompatSetDisplayOptions);
		fclose(f);
	}
}

static HMODULE gSpideyRenderer11Module = 0;

typedef unsigned long (__cdecl *SpideyRenderer11GetAbiVersionFn)(void);
typedef const char* (__cdecl *SpideyRenderer11GetBackendNameFn)(void);
typedef int (__cdecl *SpideyRenderer11ProbeFn)(void);
typedef int (__cdecl *SpideyRenderer11InitializeFn)(
		HWND,
		unsigned long,
		unsigned long);
typedef int (__cdecl *SpideyRenderer11ResizeFn)(
		unsigned long,
		unsigned long);
typedef int (__cdecl *SpideyRenderer11PresentPixelsFn)(
		const void*,
		unsigned long,
		unsigned long,
		long,
		int,
		int);
typedef int (__cdecl *SpideyRenderer11PresentHdcFn)(
		HDC,
		unsigned long,
		unsigned long,
		int,
		int);
typedef int (__cdecl *SpideyRenderer11UpdateTextureFn)(
		unsigned long,
		const void*,
		unsigned long,
		unsigned long,
		long,
		unsigned long,
		unsigned long,
		unsigned long,
		unsigned long,
		unsigned long);
typedef int (__cdecl *SpideyRenderer11AssociateTextureHandleFn)(
		unsigned long,
		unsigned long);
typedef long (__cdecl *SpideyRenderer11ResolveTextureHandleFn)(
		unsigned long);
typedef long (__cdecl *SpideyRenderer11UpdateTransientTextureFn)(
		unsigned long,
		const void*,
		unsigned long,
		unsigned long,
		long,
		unsigned long,
		unsigned long,
		unsigned long,
		unsigned long,
		unsigned long);
typedef void (__cdecl *SpideyRenderer11ShadowSetClearFn)(
		unsigned long,
		unsigned long,
		float,
		unsigned long);
typedef int (__cdecl *SpideyRenderer11ShadowSubmitTriangleFanFn)(
		const SpideyRenderer11LegacyShadowVertex*,
		unsigned long,
		const SpideyRenderer11LegacyShadowState*);
typedef int (__cdecl *SpideyRenderer11ShadowEndFrameFn)(
		unsigned long,
		unsigned long,
		unsigned long);
typedef void (__cdecl *SpideyRenderer11ShadowSetContinuousFn)(
		int);
typedef int (__cdecl *SpideyRenderer11PresentShadowFn)(
		int,
		int);
typedef void (__cdecl *SpideyRenderer11ReleaseTextureFn)(
		unsigned long);
typedef void (__cdecl *SpideyRenderer11ReleaseAllTexturesFn)(void);
typedef unsigned long (__cdecl *SpideyRenderer11GetResidentTextureCountFn)(void);
typedef void (__cdecl *SpideyRenderer11ShutdownFn)(void);

static SpideyRenderer11InitializeFn gSpideyRenderer11Initialize = 0;
static SpideyRenderer11ResizeFn gSpideyRenderer11Resize = 0;
static SpideyRenderer11PresentPixelsFn gSpideyRenderer11PresentPixels = 0;
static SpideyRenderer11PresentHdcFn gSpideyRenderer11PresentHdc = 0;
static SpideyRenderer11UpdateTextureFn gSpideyRenderer11UpdateTexture = 0;
static SpideyRenderer11AssociateTextureHandleFn gSpideyRenderer11AssociateTextureHandle = 0;
static SpideyRenderer11ResolveTextureHandleFn gSpideyRenderer11ResolveTextureHandle = 0;
static SpideyRenderer11UpdateTransientTextureFn gSpideyRenderer11UpdateTransientTexture = 0;
static SpideyRenderer11ShadowSetClearFn gSpideyRenderer11ShadowSetClear = 0;
static SpideyRenderer11ShadowSubmitTriangleFanFn gSpideyRenderer11ShadowSubmitTriangleFan = 0;
static SpideyRenderer11ShadowEndFrameFn gSpideyRenderer11ShadowEndFrame = 0;
static SpideyRenderer11ShadowSetContinuousFn gSpideyRenderer11ShadowSetContinuous = 0;
static SpideyRenderer11PresentShadowFn gSpideyRenderer11PresentShadow = 0;
static SpideyRenderer11ReleaseTextureFn gSpideyRenderer11ReleaseTexture = 0;
static SpideyRenderer11ReleaseAllTexturesFn gSpideyRenderer11ReleaseAllTextures = 0;
static SpideyRenderer11GetResidentTextureCountFn gSpideyRenderer11GetResidentTextureCount = 0;
static SpideyRenderer11ShutdownFn gSpideyRenderer11Shutdown = 0;
static int gSpideyRenderer11BridgeReady = 0;
static int gSpideyRenderer11Initialized = 0;
static int gSpideyRenderer11PresentationDisabled = 0;
static int gSpideyRenderer11PixelsDisabled = 0;
static HWND gSpideyRenderer11Window = 0;
static unsigned long gSpideyRenderer11Width = 0;
static unsigned long gSpideyRenderer11Height = 0;

static int SpideyProbeRenderer11Bridge()
{
	if (!gSpideyRenderer11Module)
	{
		gSpideyRenderer11Module =
			LoadLibraryA(
				"spidey_renderer11.dll");
	}

	FILE* f = fopen(
		"spidey-decomp-compat.log",
		"a");

	if (!gSpideyRenderer11Module)
	{
		if (f)
		{
			fprintf(
				f,
				"renderer11_bridge load_failed error=%lu\n",
				(unsigned long)GetLastError());
			fclose(f);
		}
		return 0;
	}

	SpideyRenderer11GetAbiVersionFn getAbi =
		(SpideyRenderer11GetAbiVersionFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_GetAbiVersion");

	SpideyRenderer11GetBackendNameFn getName =
		(SpideyRenderer11GetBackendNameFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_GetBackendName");

	SpideyRenderer11ProbeFn probe =
		(SpideyRenderer11ProbeFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_Probe");

	gSpideyRenderer11Initialize =
		(SpideyRenderer11InitializeFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_Initialize");

	gSpideyRenderer11Resize =
		(SpideyRenderer11ResizeFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_Resize");

	gSpideyRenderer11PresentPixels =
		(SpideyRenderer11PresentPixelsFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_PresentPixels");

	gSpideyRenderer11PresentHdc =
		(SpideyRenderer11PresentHdcFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_PresentHdc");

	gSpideyRenderer11UpdateTexture =
		(SpideyRenderer11UpdateTextureFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_UpdateTexture");

	gSpideyRenderer11AssociateTextureHandle =
		(SpideyRenderer11AssociateTextureHandleFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_AssociateTextureHandle");

	gSpideyRenderer11ResolveTextureHandle =
		(SpideyRenderer11ResolveTextureHandleFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ResolveTextureHandle");

	gSpideyRenderer11UpdateTransientTexture =
		(SpideyRenderer11UpdateTransientTextureFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_UpdateTransientTexture");

	gSpideyRenderer11ShadowSetClear =
		(SpideyRenderer11ShadowSetClearFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ShadowSetClear");

	gSpideyRenderer11ShadowSubmitTriangleFan =
		(SpideyRenderer11ShadowSubmitTriangleFanFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ShadowSubmitTriangleFan");

	gSpideyRenderer11ShadowEndFrame =
		(SpideyRenderer11ShadowEndFrameFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ShadowEndFrame");

	gSpideyRenderer11ShadowSetContinuous =
		(SpideyRenderer11ShadowSetContinuousFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ShadowSetContinuous");

	gSpideyRenderer11PresentShadow =
		(SpideyRenderer11PresentShadowFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_PresentShadow");

	gSpideyRenderer11ReleaseTexture =
		(SpideyRenderer11ReleaseTextureFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ReleaseTexture");

	gSpideyRenderer11ReleaseAllTextures =
		(SpideyRenderer11ReleaseAllTexturesFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_ReleaseAllTextures");

	gSpideyRenderer11GetResidentTextureCount =
		(SpideyRenderer11GetResidentTextureCountFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_GetResidentTextureCount");

	gSpideyRenderer11Shutdown =
		(SpideyRenderer11ShutdownFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_Shutdown");

	if (!getAbi ||
		!getName ||
		!probe ||
		!gSpideyRenderer11Initialize ||
		!gSpideyRenderer11Resize ||
		!gSpideyRenderer11PresentPixels ||
		!gSpideyRenderer11PresentHdc ||
		!gSpideyRenderer11UpdateTexture ||
		!gSpideyRenderer11AssociateTextureHandle ||
		!gSpideyRenderer11ResolveTextureHandle ||
		!gSpideyRenderer11UpdateTransientTexture ||
		!gSpideyRenderer11ShadowSetClear ||
		!gSpideyRenderer11ShadowSubmitTriangleFan ||
		!gSpideyRenderer11ShadowEndFrame ||
		!gSpideyRenderer11ShadowSetContinuous ||
		!gSpideyRenderer11PresentShadow ||
		!gSpideyRenderer11ReleaseTexture ||
		!gSpideyRenderer11ReleaseAllTextures ||
		!gSpideyRenderer11GetResidentTextureCount ||
		!gSpideyRenderer11Shutdown)
	{
		if (f)
		{
			fprintf(
				f,
				"renderer11_bridge exports_missing abi=0x%08lX name=0x%08lX probe=0x%08lX init=0x%08lX resize=0x%08lX present_pixels=0x%08lX present_hdc=0x%08lX update_tex=0x%08lX associate_tex=0x%08lX resolve_tex=0x%08lX transient_tex=0x%08lX shadow_clear=0x%08lX shadow_submit=0x%08lX shadow_end=0x%08lX shadow_continuous=0x%08lX present_shadow=0x%08lX release_tex=0x%08lX release_all=0x%08lX tex_count=0x%08lX shutdown=0x%08lX\n",
				(unsigned long)getAbi,
				(unsigned long)getName,
				(unsigned long)probe,
				(unsigned long)gSpideyRenderer11Initialize,
				(unsigned long)gSpideyRenderer11Resize,
				(unsigned long)gSpideyRenderer11PresentPixels,
				(unsigned long)gSpideyRenderer11PresentHdc,
				(unsigned long)gSpideyRenderer11UpdateTexture,
				(unsigned long)gSpideyRenderer11AssociateTextureHandle,
				(unsigned long)gSpideyRenderer11ResolveTextureHandle,
				(unsigned long)gSpideyRenderer11UpdateTransientTexture,
				(unsigned long)gSpideyRenderer11ShadowSetClear,
				(unsigned long)gSpideyRenderer11ShadowSubmitTriangleFan,
				(unsigned long)gSpideyRenderer11ShadowEndFrame,
				(unsigned long)gSpideyRenderer11ShadowSetContinuous,
				(unsigned long)gSpideyRenderer11PresentShadow,
				(unsigned long)gSpideyRenderer11ReleaseTexture,
				(unsigned long)gSpideyRenderer11ReleaseAllTextures,
				(unsigned long)gSpideyRenderer11GetResidentTextureCount,
				(unsigned long)gSpideyRenderer11Shutdown);
			fclose(f);
		}
		return 0;
	}

	unsigned long abi =
		getAbi();
	const char* name =
		getName();
	int probeResult =
		probe();

	gSpideyRenderer11BridgeReady =
		abi == 6 &&
		probeResult != 0;

	if (f)
	{
		fprintf(
			f,
			"renderer11_bridge loaded module=0x%08lX abi=%lu expected=6 backend=%s probe=%d phase2c2_exports=%d\n",
			(unsigned long)gSpideyRenderer11Module,
			abi,
			name ? name : "unknown",
			probeResult,
			gSpideyRenderer11BridgeReady ? 1 : 0);
		fclose(f);
	}

	return gSpideyRenderer11BridgeReady;
}

static int SpideyEnsureRenderer11Presentation(
		HWND hwnd,
		unsigned long width,
		unsigned long height)
{
	if (!gSpideyRenderer11BridgeReady ||
		gSpideyRenderer11PresentationDisabled ||
		!gSpideyRenderer11Initialize ||
		!gSpideyRenderer11Resize ||
		!gSpideyRenderer11Shutdown ||
		!hwnd ||
		!width ||
		!height)
	{
		return 0;
	}

	if (gSpideyRenderer11Initialized &&
		gSpideyRenderer11Window != hwnd)
	{
		gSpideyRenderer11Shutdown();
		gSpideyRenderer11Initialized = 0;
		gSpideyRenderer11Window = 0;
		gSpideyRenderer11Width = 0;
		gSpideyRenderer11Height = 0;
	}

	if (!gSpideyRenderer11Initialized)
	{
		int initialized =
			gSpideyRenderer11Initialize(
				hwnd,
				width,
				height);

		FILE* f = fopen(
			"spidey-decomp-compat.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"renderer11_phase2 initialize hwnd=0x%08lX size=%lux%lu result=%d\n",
				(unsigned long)hwnd,
				width,
				height,
				initialized);
			fclose(f);
		}

		if (!initialized)
		{
			gSpideyRenderer11PresentationDisabled = 1;
			return 0;
		}

		gSpideyRenderer11Initialized = 1;
		gSpideyRenderer11Window = hwnd;
		gSpideyRenderer11Width = width;
		gSpideyRenderer11Height = height;
		return 1;
	}

	if (gSpideyRenderer11Width != width ||
		gSpideyRenderer11Height != height)
	{
		int resized =
			gSpideyRenderer11Resize(
				width,
				height);

		FILE* f = fopen(
			"spidey-decomp-compat.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"renderer11_phase2 resize old=%lux%lu new=%lux%lu result=%d\n",
				gSpideyRenderer11Width,
				gSpideyRenderer11Height,
				width,
				height,
				resized);
			fclose(f);
		}

		if (!resized)
		{
			gSpideyRenderer11Shutdown();
			gSpideyRenderer11Initialized = 0;
			gSpideyRenderer11PresentationDisabled = 1;
			return 0;
		}

		gSpideyRenderer11Width = width;
		gSpideyRenderer11Height = height;
	}

	return 1;
}

int SpideyRenderer11MirrorLegacyTexture(
		unsigned long textureId,
		void* legacySurface)
{
	if (!gSpideyRenderer11BridgeReady ||
		!gSpideyRenderer11UpdateTexture ||
		!legacySurface)
	{
		return 0;
	}

	// Do not depend on PCTex being initialized after our explicit Phase 2B
	// early-init point. If a retail path creates a texture sooner, bring the
	// DX11 device up lazily from the live game HWND and continue mirroring.
	if (!gSpideyRenderer11Initialized)
	{
		HWND mirrorWindow =
			*(HWND*)0x006B58D0;
		RECT mirrorClient;

		if (!mirrorWindow ||
			!GetClientRect(
				mirrorWindow,
				&mirrorClient))
		{
			return 0;
		}

		unsigned long mirrorWidth =
			(unsigned long)(
				mirrorClient.right -
				mirrorClient.left);
		unsigned long mirrorHeight =
			(unsigned long)(
				mirrorClient.bottom -
				mirrorClient.top);

		if (!mirrorWidth ||
			!mirrorHeight ||
			!SpideyEnsureRenderer11Presentation(
				mirrorWindow,
				mirrorWidth,
				mirrorHeight))
		{
			return 0;
		}

		FILE* lazyLog = fopen(
			"spidey-decomp-compat.log",
			"a");
		if (lazyLog)
		{
			fprintf(
				lazyLog,
				"renderer11_phase2b lazy_texture_initialize hwnd=0x%08lX size=%lux%lu result=1\n",
				(unsigned long)mirrorWindow,
				mirrorWidth,
				mirrorHeight);
			fclose(lazyLog);
		}
	}

	LPDIRECTDRAWSURFACE7 surface =
		(LPDIRECTDRAWSURFACE7)legacySurface;

	DDSURFACEDESC2 desc;
	memset(
		&desc,
		0,
		sizeof(desc));
	desc.dwSize =
		sizeof(desc);

	HRESULT lockHr =
		surface->Lock(
			0,
			&desc,
			DDLOCK_WAIT | DDLOCK_READONLY,
			0);
	int lockRetry =
		0;

	if (FAILED(lockHr))
	{
		memset(
			&desc,
			0,
			sizeof(desc));
		desc.dwSize =
			sizeof(desc);

		lockHr =
			surface->Lock(
				0,
				&desc,
				DDLOCK_WAIT,
				0);
		lockRetry =
			SUCCEEDED(lockHr) ? 1 : 0;
	}

	if (FAILED(lockHr) ||
		!desc.lpSurface ||
		!desc.dwWidth ||
		!desc.dwHeight ||
		!desc.ddpfPixelFormat.dwRGBBitCount)
	{
		FILE* f = fopen(
			"spidey-decomp-texture.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"dx11_mirror id=%lu result=0 lock_hr=0x%08lX ptr=0x%08lX size=%lux%lu bpp=%lu retry=%d\n",
				textureId,
				(unsigned long)lockHr,
				(unsigned long)desc.lpSurface,
				(unsigned long)desc.dwWidth,
				(unsigned long)desc.dwHeight,
				(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
				lockRetry);
			fclose(f);
		}

		if (SUCCEEDED(lockHr))
			surface->Unlock(0);

		return 0;
	}

	int mirrored =
		gSpideyRenderer11UpdateTexture(
			textureId,
			desc.lpSurface,
			(unsigned long)desc.dwWidth,
			(unsigned long)desc.dwHeight,
			(long)desc.lPitch,
			(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
			(unsigned long)desc.ddpfPixelFormat.dwRBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwGBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwBBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwRGBAlphaBitMask);

	surface->Unlock(0);

	FILE* f = fopen(
		"spidey-decomp-texture.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"dx11_mirror id=%lu result=%d size=%lux%lu pitch=%ld bpp=%lu masks=%08lX,%08lX,%08lX,%08lX retry=%d resident=%lu\n",
			textureId,
			mirrored,
			(unsigned long)desc.dwWidth,
			(unsigned long)desc.dwHeight,
			(long)desc.lPitch,
			(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
			(unsigned long)desc.ddpfPixelFormat.dwRBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwGBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwBBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwRGBAlphaBitMask,
			lockRetry,
			SpideyRenderer11GetMirroredTextureCount());
		fclose(f);
	}

	return mirrored;
}

int SpideyRenderer11AssociateLegacyTexture(
		unsigned long textureId,
		void* legacySurface)
{
	if (!gSpideyRenderer11AssociateTextureHandle ||
		!legacySurface)
	{
		return 0;
	}

	int associated =
		gSpideyRenderer11AssociateTextureHandle(
			textureId,
			(unsigned long)legacySurface);

	FILE* f = fopen(
		"spidey-decomp-texture.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"dx11_associate id=%lu handle=0x%08lX result=%d resident=%lu\n",
			textureId,
			(unsigned long)legacySurface,
			associated,
			SpideyRenderer11GetMirroredTextureCount());
		fclose(f);
	}

	return associated;
}

long SpideyRenderer11ResolveLegacyTexture(
		void* legacySurface)
{
	if (!gSpideyRenderer11ResolveTextureHandle ||
		!legacySurface)
	{
		return -1;
	}

	return gSpideyRenderer11ResolveTextureHandle(
			(unsigned long)legacySurface);
}

long SpideyRenderer11MirrorTransientLegacyTexture(
		void* legacySurface)
{
	if (!gSpideyRenderer11UpdateTransientTexture ||
		!legacySurface)
	{
		return -1;
	}

	LPDIRECTDRAWSURFACE7 surface =
		(LPDIRECTDRAWSURFACE7)legacySurface;

	DDSURFACEDESC2 desc;
	memset(
		&desc,
		0,
		sizeof(desc));
	desc.dwSize =
		sizeof(desc);

	HRESULT lockHr =
		surface->Lock(
			0,
			&desc,
			DDLOCK_WAIT | DDLOCK_READONLY,
			0);
	int lockRetry =
		0;

	if (FAILED(lockHr))
	{
		memset(
			&desc,
			0,
			sizeof(desc));
		desc.dwSize =
			sizeof(desc);

		lockHr =
			surface->Lock(
				0,
				&desc,
				DDLOCK_WAIT,
				0);
		lockRetry =
			SUCCEEDED(lockHr) ? 1 : 0;
	}

	if (FAILED(lockHr) ||
		!desc.lpSurface ||
		!desc.dwWidth ||
		!desc.dwHeight ||
		!desc.ddpfPixelFormat.dwRGBBitCount)
	{
		if (SUCCEEDED(lockHr))
			surface->Unlock(0);

		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"transient_mirror handle=0x%08lX result=-1 lock_hr=0x%08lX size=%lux%lu bpp=%lu retry=%d\n",
				(unsigned long)surface,
				(unsigned long)lockHr,
				(unsigned long)desc.dwWidth,
				(unsigned long)desc.dwHeight,
				(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
				lockRetry);
			fclose(f);
		}

		return -1;
	}

	long textureId =
		gSpideyRenderer11UpdateTransientTexture(
			(unsigned long)surface,
			desc.lpSurface,
			(unsigned long)desc.dwWidth,
			(unsigned long)desc.dwHeight,
			(long)desc.lPitch,
			(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
			(unsigned long)desc.ddpfPixelFormat.dwRBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwGBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwBBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwRGBAlphaBitMask);

	surface->Unlock(0);

	FILE* f = fopen(
		"spidey-decomp-draw.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"transient_mirror handle=0x%08lX result=%ld size=%lux%lu pitch=%ld bpp=%lu masks=%08lX,%08lX,%08lX,%08lX retry=%d resident=%lu\n",
			(unsigned long)surface,
			textureId,
			(unsigned long)desc.dwWidth,
			(unsigned long)desc.dwHeight,
			(long)desc.lPitch,
			(unsigned long)desc.ddpfPixelFormat.dwRGBBitCount,
			(unsigned long)desc.ddpfPixelFormat.dwRBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwGBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwBBitMask,
			(unsigned long)desc.ddpfPixelFormat.dwRGBAlphaBitMask,
			lockRetry,
			SpideyRenderer11GetMirroredTextureCount());
		fclose(f);
	}

	return textureId;
}

void SpideyRenderer11ShadowSetClear(
		unsigned long clearFlags,
		unsigned long clearColor,
		float clearDepth,
		unsigned long clearStencil)
{
	if (gSpideyRenderer11ShadowSetClear)
	{
		gSpideyRenderer11ShadowSetClear(
			clearFlags,
			clearColor,
			clearDepth,
			clearStencil);
	}
}

int SpideyRenderer11ShadowSubmitTriangleFan(
		const SpideyRenderer11LegacyShadowVertex* vertices,
		unsigned long vertexCount,
		const SpideyRenderer11LegacyShadowState* state)
{
	if (!gSpideyRenderer11ShadowSubmitTriangleFan)
		return 0;

	return gSpideyRenderer11ShadowSubmitTriangleFan(
			vertices,
			vertexCount,
			state);
}

int SpideyRenderer11ShadowEndFrame(
		unsigned long frame,
		unsigned long sceneWidth,
		unsigned long sceneHeight)
{
	if (!gSpideyRenderer11ShadowEndFrame)
		return 0;

	return gSpideyRenderer11ShadowEndFrame(
			frame,
			sceneWidth,
			sceneHeight);
}

void SpideyRenderer11ShadowSetContinuous(
		int enabled)
{
	if (gSpideyRenderer11ShadowSetContinuous)
		gSpideyRenderer11ShadowSetContinuous(enabled);
}

int SpideyRenderer11PresentShadow(
		int preserveAspect,
		int vsync)
{
	if (!gSpideyRenderer11PresentShadow)
		return 0;

	return gSpideyRenderer11PresentShadow(
			preserveAspect,
			vsync);
}

void SpideyRenderer11ReleaseMirroredTexture(
		unsigned long textureId)
{
	if (gSpideyRenderer11ReleaseTexture)
		gSpideyRenderer11ReleaseTexture(textureId);
}

void SpideyRenderer11ReleaseAllMirroredTextures(void)
{
	if (gSpideyRenderer11ReleaseAllTextures)
		gSpideyRenderer11ReleaseAllTextures();
}

unsigned long SpideyRenderer11GetMirroredTextureCount(void)
{
	if (!gSpideyRenderer11GetResidentTextureCount)
		return 0;

	return gSpideyRenderer11GetResidentTextureCount();
}

typedef HRESULT (WINAPI *SpideyRetailD3D7SetRenderTargetFn)(
		LPDIRECT3DDEVICE7,
		LPDIRECTDRAWSURFACE7,
		DWORD);
typedef HRESULT (WINAPI *SpideyRetailD3D7ClearFn)(
		LPDIRECT3DDEVICE7,
		DWORD,
		LPD3DRECT,
		DWORD,
		D3DCOLOR,
		D3DVALUE,
		DWORD);
typedef HRESULT (WINAPI *SpideyRetailD3D7SetViewportFn)(
		LPDIRECT3DDEVICE7,
		LPD3DVIEWPORT7);
typedef HRESULT (WINAPI *SpideyRetailD3D7SetRenderStateFn)(
		LPDIRECT3DDEVICE7,
		D3DRENDERSTATETYPE,
		DWORD);
typedef HRESULT (WINAPI *SpideyRetailD3D7DrawPrimitiveFn)(
		LPDIRECT3DDEVICE7,
		D3DPRIMITIVETYPE,
		DWORD,
		LPVOID,
		DWORD,
		DWORD);
typedef HRESULT (WINAPI *SpideyRetailD3D7SetTextureFn)(
		LPDIRECT3DDEVICE7,
		DWORD,
		LPDIRECTDRAWSURFACE7);
typedef HRESULT (WINAPI *SpideyRetailD3D7SetTextureStageStateFn)(
		LPDIRECT3DDEVICE7,
		DWORD,
		D3DTEXTURESTAGESTATETYPE,
		DWORD);

struct SpideyRetailTLVertexProbe
{
	f32 x;
	f32 y;
	f32 z;
	f32 rhw;
	DWORD diffuse;
	f32 u;
	f32 v;
};

static SpideyRetailD3D7SetRenderTargetFn gSpideyRetailD3D7SetRenderTargetOriginal = 0;
static SpideyRetailD3D7ClearFn gSpideyRetailD3D7ClearOriginal = 0;
static SpideyRetailD3D7SetViewportFn gSpideyRetailD3D7SetViewportOriginal = 0;
static SpideyRetailD3D7SetRenderStateFn gSpideyRetailD3D7SetRenderStateOriginal = 0;
static SpideyRetailD3D7DrawPrimitiveFn gSpideyRetailD3D7DrawPrimitiveOriginal = 0;
static SpideyRetailD3D7SetTextureFn gSpideyRetailD3D7SetTextureOriginal = 0;
static SpideyRetailD3D7SetTextureStageStateFn gSpideyRetailD3D7SetTextureStageStateOriginal = 0;
static LPDIRECT3DDEVICE7 gSpideyRetailD3D7DrawProbeDevice = 0;
static void** gSpideyRetailD3D7DrawProbeVtable = 0;

static SpideyRenderer11LegacyShadowState gSpideyRetailShadowState;
static int gSpideyRetailShadowStateValid = 0;
static LPDIRECTDRAWSURFACE7 gSpideyRetailShadowRenderTarget = 0;

static LPDIRECTDRAWSURFACE7 gSpideyPendingTransientSurfaces[32];
static unsigned long gSpideyPendingTransientCount = 0;
static unsigned long gSpideyShadowSubmitted = 0;
static unsigned long gSpideyShadowSkipped = 0;
static unsigned long gSpideyShadowOffscreenSkipped = 0;
static unsigned long gSpideyTransientQueued = 0;
static unsigned long gSpideyTransientMirrored = 0;
static unsigned long gSpideyPresentFrame = 0;
static int gSpideyShadowPreviewEnabled = 1;
static int gSpideyShadowPreviewReady = 0;
static int gSpideyShadowPreviewModeSynced = 0;

static unsigned long gSpideyRetailDrawCalls = 0;
static unsigned long gSpideyRetailDrawTextured = 0;
static unsigned long gSpideyRetailDrawMirrored = 0;
static unsigned long gSpideyRetailDrawMissing = 0;
static unsigned long gSpideyRetailDrawTriangleFan = 0;
static unsigned long gSpideyRetailDrawFvf144 = 0;
static unsigned long gSpideyRetailDrawOtherPrimitive = 0;
static unsigned long gSpideyRetailDrawOtherFvf = 0;
static unsigned long gSpideyRetailDrawSampleCount = 0;

static int gSpideyModernRangeValid = 0;
static float gSpideyModernMinX = 0.0f;
static float gSpideyModernMaxX = 0.0f;
static float gSpideyModernMinY = 0.0f;
static float gSpideyModernMaxY = 0.0f;
static unsigned long gSpideyModernVertexCount = 0;
static unsigned long gSpideyModernOutsidePhysicalX = 0;
static unsigned long gSpideyModernOutsidePhysicalY = 0;

static int SpideyIsExecutablePointer(
		void* pointer)
{
	if (!pointer)
		return 0;

	MEMORY_BASIC_INFORMATION mbi;
	memset(
		&mbi,
		0,
		sizeof(mbi));

	if (!VirtualQuery(
			pointer,
			&mbi,
			sizeof(mbi)))
	{
		return 0;
	}

	DWORD protect =
		mbi.Protect &
		0xFF;

	return protect == PAGE_EXECUTE ||
		protect == PAGE_EXECUTE_READ ||
		protect == PAGE_EXECUTE_READWRITE ||
		protect == PAGE_EXECUTE_WRITECOPY;
}

static void SpideyQueueTransientSurface(
		LPDIRECTDRAWSURFACE7 surface)
{
	if (!surface ||
		SpideyRenderer11ResolveLegacyTexture(surface) >= 0)
	{
		return;
	}

	for (unsigned long i = 0;
		 i < gSpideyPendingTransientCount;
		 ++i)
	{
		if (gSpideyPendingTransientSurfaces[i] == surface)
			return;
	}

	if (gSpideyPendingTransientCount >=
		sizeof(gSpideyPendingTransientSurfaces) /
		sizeof(gSpideyPendingTransientSurfaces[0]))
	{
		return;
	}

	surface->AddRef();
	gSpideyPendingTransientSurfaces[
		gSpideyPendingTransientCount++] =
		surface;
	++gSpideyTransientQueued;
}

static void SpideyProcessPendingTransientSurfaces()
{
	for (unsigned long i = 0;
		 i < gSpideyPendingTransientCount;
		 ++i)
	{
		LPDIRECTDRAWSURFACE7 surface =
			gSpideyPendingTransientSurfaces[i];

		if (surface)
		{
			long textureId =
				SpideyRenderer11MirrorTransientLegacyTexture(
					surface);

			if (textureId >= 0)
				++gSpideyTransientMirrored;

			surface->Release();
		}

		gSpideyPendingTransientSurfaces[i] =
			0;
	}

	gSpideyPendingTransientCount =
		0;
}

static void SpideyInitializeRetailShadowState(
		LPDIRECT3DDEVICE7 device)
{
	memset(
		&gSpideyRetailShadowState,
		0,
		sizeof(gSpideyRetailShadowState));

	if (!device)
	{
		gSpideyRetailShadowStateValid =
			0;
		return;
	}

	device->GetRenderState(
		D3DRENDERSTATE_ZENABLE,
		&gSpideyRetailShadowState.zEnable);
	device->GetRenderState(
		D3DRENDERSTATE_ZWRITEENABLE,
		&gSpideyRetailShadowState.zWrite);
	device->GetRenderState(
		D3DRENDERSTATE_ZFUNC,
		&gSpideyRetailShadowState.zFunc);
	device->GetRenderState(
		D3DRENDERSTATE_ALPHABLENDENABLE,
		&gSpideyRetailShadowState.alphaBlendEnable);
	device->GetRenderState(
		D3DRENDERSTATE_SRCBLEND,
		&gSpideyRetailShadowState.srcBlend);
	device->GetRenderState(
		D3DRENDERSTATE_DESTBLEND,
		&gSpideyRetailShadowState.dstBlend);
	device->GetRenderState(
		D3DRENDERSTATE_ALPHATESTENABLE,
		&gSpideyRetailShadowState.alphaTestEnable);
	device->GetRenderState(
		D3DRENDERSTATE_ALPHAREF,
		&gSpideyRetailShadowState.alphaRef);
	device->GetRenderState(
		D3DRENDERSTATE_ALPHAFUNC,
		&gSpideyRetailShadowState.alphaFunc);
	device->GetRenderState(
		D3DRENDERSTATE_FOGENABLE,
		&gSpideyRetailShadowState.fogEnable);
	device->GetRenderState(
		D3DRENDERSTATE_FOGCOLOR,
		&gSpideyRetailShadowState.fogColor);

	device->GetTextureStageState(
		0,
		D3DTSS_COLOROP,
		&gSpideyRetailShadowState.colorOp);
	device->GetTextureStageState(
		0,
		D3DTSS_COLORARG1,
		&gSpideyRetailShadowState.colorArg1);
	device->GetTextureStageState(
		0,
		D3DTSS_COLORARG2,
		&gSpideyRetailShadowState.colorArg2);
	device->GetTextureStageState(
		0,
		D3DTSS_ALPHAOP,
		&gSpideyRetailShadowState.alphaOp);
	device->GetTextureStageState(
		0,
		D3DTSS_ALPHAARG1,
		&gSpideyRetailShadowState.alphaArg1);
	device->GetTextureStageState(
		0,
		D3DTSS_ALPHAARG2,
		&gSpideyRetailShadowState.alphaArg2);
	device->GetTextureStageState(
		0,
		D3DTSS_ADDRESSU,
		&gSpideyRetailShadowState.addressU);
	device->GetTextureStageState(
		0,
		D3DTSS_ADDRESSV,
		&gSpideyRetailShadowState.addressV);
	device->GetTextureStageState(
		0,
		D3DTSS_MAGFILTER,
		&gSpideyRetailShadowState.magFilter);
	device->GetTextureStageState(
		0,
		D3DTSS_MINFILTER,
		&gSpideyRetailShadowState.minFilter);

	D3DVIEWPORT7 viewport;
	memset(
		&viewport,
		0,
		sizeof(viewport));

	if (SUCCEEDED(
			device->GetViewport(
				&viewport)))
	{
		gSpideyRetailShadowState.viewportX =
			viewport.dwX;
		gSpideyRetailShadowState.viewportY =
			viewport.dwY;
		gSpideyRetailShadowState.viewportWidth =
			viewport.dwWidth;
		gSpideyRetailShadowState.viewportHeight =
			viewport.dwHeight;
		gSpideyRetailShadowState.viewportMinZ =
			viewport.dvMinZ;
		gSpideyRetailShadowState.viewportMaxZ =
			viewport.dvMaxZ;
	}

	LPDIRECTDRAWSURFACE7 renderTarget =
		0;

	if (SUCCEEDED(
			device->GetRenderTarget(
				&renderTarget)) &&
		renderTarget)
	{
		gSpideyRetailShadowRenderTarget =
			renderTarget;
		renderTarget->Release();
	}

	LPDIRECTDRAWSURFACE7 texture =
		0;

	if (SUCCEEDED(
			device->GetTexture(
				0,
				&texture)) &&
		texture)
	{
		gSpideyRetailShadowState.textureHandle =
			(unsigned long)texture;
		texture->Release();
	}

	gSpideyRetailShadowStateValid =
		1;
}

static HRESULT WINAPI SpideyShadowD3D7SetRenderTarget(
		LPDIRECT3DDEVICE7 device,
		LPDIRECTDRAWSURFACE7 renderTarget,
		DWORD flags)
{
	if (!gSpideyRetailD3D7SetRenderTargetOriginal)
		return E_FAIL;

	HRESULT hr =
		gSpideyRetailD3D7SetRenderTargetOriginal(
			device,
			renderTarget,
			flags);

	if (SUCCEEDED(hr))
	{
		gSpideyRetailShadowRenderTarget =
			renderTarget;
	}

	return hr;
}

static HRESULT WINAPI SpideyShadowD3D7Clear(
		LPDIRECT3DDEVICE7 device,
		DWORD count,
		LPD3DRECT rects,
		DWORD flags,
		D3DCOLOR color,
		D3DVALUE z,
		DWORD stencil)
{
	if (!gSpideyRetailD3D7ClearOriginal)
		return E_FAIL;

	HRESULT hr =
		gSpideyRetailD3D7ClearOriginal(
			device,
			count,
			rects,
			flags,
			color,
			z,
			stencil);

	if (SUCCEEDED(hr) &&
		count == 0 &&
		gSpideyRetailShadowRenderTarget ==
			*(LPDIRECTDRAWSURFACE7*)0x006B7908)
	{
		SpideyRenderer11ShadowSetClear(
			(unsigned long)flags,
			(unsigned long)color,
			z,
			(unsigned long)stencil);
	}

	return hr;
}

static HRESULT WINAPI SpideyShadowD3D7SetViewport(
		LPDIRECT3DDEVICE7 device,
		LPD3DVIEWPORT7 viewport)
{
	if (!gSpideyRetailD3D7SetViewportOriginal)
		return E_FAIL;

	HRESULT hr =
		gSpideyRetailD3D7SetViewportOriginal(
			device,
			viewport);

	if (SUCCEEDED(hr) &&
		viewport)
	{
		gSpideyRetailShadowState.viewportX =
			viewport->dwX;
		gSpideyRetailShadowState.viewportY =
			viewport->dwY;
		gSpideyRetailShadowState.viewportWidth =
			viewport->dwWidth;
		gSpideyRetailShadowState.viewportHeight =
			viewport->dwHeight;
		gSpideyRetailShadowState.viewportMinZ =
			viewport->dvMinZ;
		gSpideyRetailShadowState.viewportMaxZ =
			viewport->dvMaxZ;
		gSpideyRetailShadowStateValid =
			1;
	}

	return hr;
}

static HRESULT WINAPI SpideyShadowD3D7SetRenderState(
		LPDIRECT3DDEVICE7 device,
		D3DRENDERSTATETYPE state,
		DWORD value)
{
	if (!gSpideyRetailD3D7SetRenderStateOriginal)
		return E_FAIL;

	HRESULT hr =
		gSpideyRetailD3D7SetRenderStateOriginal(
			device,
			state,
			value);

	if (FAILED(hr))
		return hr;

	switch (state)
	{
		case D3DRENDERSTATE_ZENABLE:
			gSpideyRetailShadowState.zEnable = value;
			break;
		case D3DRENDERSTATE_ZWRITEENABLE:
			gSpideyRetailShadowState.zWrite = value;
			break;
		case D3DRENDERSTATE_ZFUNC:
			gSpideyRetailShadowState.zFunc = value;
			break;
		case D3DRENDERSTATE_ALPHABLENDENABLE:
			gSpideyRetailShadowState.alphaBlendEnable = value;
			break;
		case D3DRENDERSTATE_SRCBLEND:
			gSpideyRetailShadowState.srcBlend = value;
			break;
		case D3DRENDERSTATE_DESTBLEND:
			gSpideyRetailShadowState.dstBlend = value;
			break;
		case D3DRENDERSTATE_ALPHATESTENABLE:
			gSpideyRetailShadowState.alphaTestEnable = value;
			break;
		case D3DRENDERSTATE_ALPHAREF:
			gSpideyRetailShadowState.alphaRef = value;
			break;
		case D3DRENDERSTATE_ALPHAFUNC:
			gSpideyRetailShadowState.alphaFunc = value;
			break;
		case D3DRENDERSTATE_FOGENABLE:
			gSpideyRetailShadowState.fogEnable = value;
			break;
		case D3DRENDERSTATE_FOGCOLOR:
			gSpideyRetailShadowState.fogColor = value;
			break;
	}

	return hr;
}

static HRESULT WINAPI SpideyShadowD3D7SetTexture(
		LPDIRECT3DDEVICE7 device,
		DWORD stage,
		LPDIRECTDRAWSURFACE7 texture)
{
	if (!gSpideyRetailD3D7SetTextureOriginal)
		return E_FAIL;

	HRESULT hr =
		gSpideyRetailD3D7SetTextureOriginal(
			device,
			stage,
			texture);

	if (SUCCEEDED(hr) &&
		stage == 0)
	{
		gSpideyRetailShadowState.textureHandle =
			(unsigned long)texture;
	}

	return hr;
}

static HRESULT WINAPI SpideyShadowD3D7SetTextureStageState(
		LPDIRECT3DDEVICE7 device,
		DWORD stage,
		D3DTEXTURESTAGESTATETYPE state,
		DWORD value)
{
	if (!gSpideyRetailD3D7SetTextureStageStateOriginal)
		return E_FAIL;

	HRESULT hr =
		gSpideyRetailD3D7SetTextureStageStateOriginal(
			device,
			stage,
			state,
			value);

	if (FAILED(hr) ||
		stage != 0)
	{
		return hr;
	}

	switch (state)
	{
		case D3DTSS_COLOROP:
			gSpideyRetailShadowState.colorOp = value;
			break;
		case D3DTSS_COLORARG1:
			gSpideyRetailShadowState.colorArg1 = value;
			break;
		case D3DTSS_COLORARG2:
			gSpideyRetailShadowState.colorArg2 = value;
			break;
		case D3DTSS_ALPHAOP:
			gSpideyRetailShadowState.alphaOp = value;
			break;
		case D3DTSS_ALPHAARG1:
			gSpideyRetailShadowState.alphaArg1 = value;
			break;
		case D3DTSS_ALPHAARG2:
			gSpideyRetailShadowState.alphaArg2 = value;
			break;
		case D3DTSS_ADDRESSU:
			gSpideyRetailShadowState.addressU = value;
			break;
		case D3DTSS_ADDRESSV:
			gSpideyRetailShadowState.addressV = value;
			break;
		case D3DTSS_MAGFILTER:
			gSpideyRetailShadowState.magFilter = value;
			break;
		case D3DTSS_MINFILTER:
			gSpideyRetailShadowState.minFilter = value;
			break;
	}

	return hr;
}

static HRESULT WINAPI SpideyProbeD3D7DrawPrimitive(
		LPDIRECT3DDEVICE7 device,
		D3DPRIMITIVETYPE primitiveType,
		DWORD vertexTypeDesc,
		LPVOID vertices,
		DWORD vertexCount,
		DWORD flags)
{
	++gSpideyRetailDrawCalls;

	if (primitiveType == D3DPT_TRIANGLEFAN)
		++gSpideyRetailDrawTriangleFan;
	else
		++gSpideyRetailDrawOtherPrimitive;

	if (vertexTypeDesc == 324)
		++gSpideyRetailDrawFvf144;
	else
		++gSpideyRetailDrawOtherFvf;

	LPDIRECTDRAWSURFACE7 texture =
		gSpideyRetailShadowStateValid ?
		(LPDIRECTDRAWSURFACE7)
			gSpideyRetailShadowState.textureHandle :
		0;
	HRESULT textureHr =
		gSpideyRetailShadowStateValid ?
		S_OK :
		E_FAIL;
	long mirroredTextureId =
		-1;

	if (texture)
	{
		++gSpideyRetailDrawTextured;

		mirroredTextureId =
			SpideyRenderer11ResolveLegacyTexture(
				texture);

		if (mirroredTextureId >= 0)
		{
			++gSpideyRetailDrawMirrored;
		}
		else
		{
			++gSpideyRetailDrawMissing;
			SpideyQueueTransientSurface(
				texture);
		}
	}

	int shadowSubmitted =
		0;

	const unsigned long shadowFrame =
		gSpideyPresentFrame + 1;
	const int captureShadowFrame =
		gSpideyShadowPreviewEnabled ||
		shadowFrame <= 5 ||
		(shadowFrame % 120) == 0;

	const int onMainScene =
		gSpideyRetailShadowRenderTarget ==
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	if (captureShadowFrame &&
		!onMainScene)
	{
		++gSpideyShadowOffscreenSkipped;
	}

	if (onMainScene &&
		vertexTypeDesc == 324 &&
		vertices &&
		vertexCount)
	{
		const SpideyRetailTLVertexProbe* rangeVertices =
			(const SpideyRetailTLVertexProbe*)vertices;

		__try
		{
			for (DWORD rangeIndex = 0;
				 rangeIndex < vertexCount;
				 ++rangeIndex)
			{
				const float x =
					rangeVertices[rangeIndex].x;
				const float y =
					rangeVertices[rangeIndex].y;

				if (!gSpideyModernRangeValid)
				{
					gSpideyModernMinX = x;
					gSpideyModernMaxX = x;
					gSpideyModernMinY = y;
					gSpideyModernMaxY = y;
					gSpideyModernRangeValid = 1;
				}
				else
				{
					if (x < gSpideyModernMinX)
						gSpideyModernMinX = x;
					if (x > gSpideyModernMaxX)
						gSpideyModernMaxX = x;
					if (y < gSpideyModernMinY)
						gSpideyModernMinY = y;
					if (y > gSpideyModernMaxY)
						gSpideyModernMaxY = y;
				}

				++gSpideyModernVertexCount;

				if (x < 0.0f ||
					x > (float)gSpideyLegacyPhysicalWidth)
				{
					++gSpideyModernOutsidePhysicalX;
				}

				if (y < 0.0f ||
					y > (float)gSpideyLegacyPhysicalHeight)
				{
					++gSpideyModernOutsidePhysicalY;
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	if (captureShadowFrame &&
		onMainScene &&
		gSpideyRetailShadowStateValid &&
		primitiveType == D3DPT_TRIANGLEFAN &&
		vertexTypeDesc == 324 &&
		vertices &&
		vertexCount >= 3)
	{
		SpideyRenderer11LegacyShadowState shadowState =
			gSpideyRetailShadowState;

		if (SpideyUseModernGameplayAspect() &&
			gSpideyShadowPreviewEnabled)
		{
			shadowState.viewportX = 0;
			shadowState.viewportY = 0;
			shadowState.viewportWidth =
				gSpideyModernLogicalWidth;
			shadowState.viewportHeight =
				gSpideyModernLogicalHeight;
			shadowState.viewportMinZ = 0.0f;
			shadowState.viewportMaxZ = 1.0f;
		}

		shadowSubmitted =
			SpideyRenderer11ShadowSubmitTriangleFan(
				(const SpideyRenderer11LegacyShadowVertex*)vertices,
				(unsigned long)vertexCount,
				&shadowState);
	}

	if (captureShadowFrame)
	{
		if (shadowSubmitted)
		{
			++gSpideyShadowSubmitted;
		}
		else if (onMainScene)
		{
			++gSpideyShadowSkipped;
		}
	}

	const int unusual =
		primitiveType != D3DPT_TRIANGLEFAN ||
		vertexTypeDesc != 324 ||
		(texture && mirroredTextureId < 0);

	if (gSpideyRetailDrawSampleCount < 16 ||
		unusual)
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");

		if (f)
		{
			int sampledVertex =
				0;
			SpideyRetailTLVertexProbe firstVertex;
			memset(
				&firstVertex,
				0,
				sizeof(firstVertex));

			if (vertices &&
				vertexCount &&
				vertexTypeDesc == 324)
			{
				__try
				{
					firstVertex =
						*(SpideyRetailTLVertexProbe*)vertices;
					sampledVertex =
						1;
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					sampledVertex =
						0;
				}
			}

			fprintf(
				f,
				"draw_sample call=%lu device=0x%08lX primitive=%lu fvf=%lu vertices=0x%08lX count=%lu flags=0x%08lX texture_hr=0x%08lX texture=0x%08lX mirrored_id=%ld",
				gSpideyRetailDrawCalls,
				(unsigned long)device,
				(unsigned long)primitiveType,
				(unsigned long)vertexTypeDesc,
				(unsigned long)vertices,
				(unsigned long)vertexCount,
				(unsigned long)flags,
				(unsigned long)textureHr,
				(unsigned long)texture,
				mirroredTextureId);

			if (sampledVertex)
			{
				fprintf(
					f,
					" v0=%.3f,%.3f,%.6f,%.6f color=0x%08lX uv=%.6f,%.6f",
					firstVertex.x,
					firstVertex.y,
					firstVertex.z,
					firstVertex.rhw,
					(unsigned long)firstVertex.diffuse,
					firstVertex.u,
					firstVertex.v);
			}

			// Snapshot the fixed-function state only for sampled/unusual draws.
			// These getters are observational and leave retail D3D7 untouched.
			DWORD zEnable = 0;
			DWORD zWrite = 0;
			DWORD zFunc = 0;
			DWORD alphaBlend = 0;
			DWORD srcBlend = 0;
			DWORD dstBlend = 0;
			DWORD alphaTest = 0;
			DWORD alphaRef = 0;
			DWORD alphaFunc = 0;
			DWORD fogEnable = 0;
			DWORD fogColor = 0;
			DWORD colorOp = 0;
			DWORD colorArg1 = 0;
			DWORD colorArg2 = 0;
			DWORD alphaOp = 0;
			DWORD alphaArg1 = 0;
			DWORD alphaArg2 = 0;
			DWORD addressU = 0;
			DWORD addressV = 0;
			DWORD magFilter = 0;
			DWORD minFilter = 0;
			D3DVIEWPORT7 viewport;
			memset(
				&viewport,
				0,
				sizeof(viewport));

			if (device)
			{
				device->GetRenderState(D3DRENDERSTATE_ZENABLE, &zEnable);
				device->GetRenderState(D3DRENDERSTATE_ZWRITEENABLE, &zWrite);
				device->GetRenderState(D3DRENDERSTATE_ZFUNC, &zFunc);
				device->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &alphaBlend);
				device->GetRenderState(D3DRENDERSTATE_SRCBLEND, &srcBlend);
				device->GetRenderState(D3DRENDERSTATE_DESTBLEND, &dstBlend);
				device->GetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, &alphaTest);
				device->GetRenderState(D3DRENDERSTATE_ALPHAREF, &alphaRef);
				device->GetRenderState(D3DRENDERSTATE_ALPHAFUNC, &alphaFunc);
				device->GetRenderState(D3DRENDERSTATE_FOGENABLE, &fogEnable);
				device->GetRenderState(D3DRENDERSTATE_FOGCOLOR, &fogColor);

				device->GetTextureStageState(0, D3DTSS_COLOROP, &colorOp);
				device->GetTextureStageState(0, D3DTSS_COLORARG1, &colorArg1);
				device->GetTextureStageState(0, D3DTSS_COLORARG2, &colorArg2);
				device->GetTextureStageState(0, D3DTSS_ALPHAOP, &alphaOp);
				device->GetTextureStageState(0, D3DTSS_ALPHAARG1, &alphaArg1);
				device->GetTextureStageState(0, D3DTSS_ALPHAARG2, &alphaArg2);
				device->GetTextureStageState(0, D3DTSS_ADDRESSU, &addressU);
				device->GetTextureStageState(0, D3DTSS_ADDRESSV, &addressV);
				device->GetTextureStageState(0, D3DTSS_MAGFILTER, &magFilter);
				device->GetTextureStageState(0, D3DTSS_MINFILTER, &minFilter);
				device->GetViewport(&viewport);
			}

			const int cacheMismatch =
				!gSpideyRetailShadowStateValid ||
				gSpideyRetailShadowState.zEnable != zEnable ||
				gSpideyRetailShadowState.zWrite != zWrite ||
				gSpideyRetailShadowState.zFunc != zFunc ||
				gSpideyRetailShadowState.alphaBlendEnable != alphaBlend ||
				gSpideyRetailShadowState.srcBlend != srcBlend ||
				gSpideyRetailShadowState.dstBlend != dstBlend ||
				gSpideyRetailShadowState.alphaTestEnable != alphaTest ||
				gSpideyRetailShadowState.alphaRef != alphaRef ||
				gSpideyRetailShadowState.alphaFunc != alphaFunc ||
				gSpideyRetailShadowState.fogEnable != fogEnable ||
				gSpideyRetailShadowState.fogColor != fogColor ||
				gSpideyRetailShadowState.colorOp != colorOp ||
				gSpideyRetailShadowState.colorArg1 != colorArg1 ||
				gSpideyRetailShadowState.colorArg2 != colorArg2 ||
				gSpideyRetailShadowState.alphaOp != alphaOp ||
				gSpideyRetailShadowState.alphaArg1 != alphaArg1 ||
				gSpideyRetailShadowState.alphaArg2 != alphaArg2 ||
				gSpideyRetailShadowState.addressU != addressU ||
				gSpideyRetailShadowState.addressV != addressV ||
				gSpideyRetailShadowState.magFilter != magFilter ||
				gSpideyRetailShadowState.minFilter != minFilter ||
				gSpideyRetailShadowState.viewportX != viewport.dwX ||
				gSpideyRetailShadowState.viewportY != viewport.dwY ||
				gSpideyRetailShadowState.viewportWidth != viewport.dwWidth ||
				gSpideyRetailShadowState.viewportHeight != viewport.dwHeight ||
				gSpideyRetailShadowState.viewportMinZ != viewport.dvMinZ ||
				gSpideyRetailShadowState.viewportMaxZ != viewport.dvMaxZ;

			fprintf(
				f,
				" state=z:%lu zw:%lu zf:%lu ab:%lu sb:%lu db:%lu at:%lu ar:%lu af:%lu fog:%lu fogc:0x%08lX tex=co:%lu ca1:%lu ca2:%lu ao:%lu aa1:%lu aa2:%lu au:%lu av:%lu mag:%lu min:%lu viewport=%lu,%lu,%lux%lu zrange=%.4f,%.4f cache_mismatch=%d",
				(unsigned long)zEnable,
				(unsigned long)zWrite,
				(unsigned long)zFunc,
				(unsigned long)alphaBlend,
				(unsigned long)srcBlend,
				(unsigned long)dstBlend,
				(unsigned long)alphaTest,
				(unsigned long)alphaRef,
				(unsigned long)alphaFunc,
				(unsigned long)fogEnable,
				(unsigned long)fogColor,
				(unsigned long)colorOp,
				(unsigned long)colorArg1,
				(unsigned long)colorArg2,
				(unsigned long)alphaOp,
				(unsigned long)alphaArg1,
				(unsigned long)alphaArg2,
				(unsigned long)addressU,
				(unsigned long)addressV,
				(unsigned long)magFilter,
				(unsigned long)minFilter,
				(unsigned long)viewport.dwX,
				(unsigned long)viewport.dwY,
				(unsigned long)viewport.dwWidth,
				(unsigned long)viewport.dwHeight,
				viewport.dvMinZ,
				viewport.dvMaxZ,
				cacheMismatch);

			fputc(
				'\n',
				f);
			fclose(f);
		}

		++gSpideyRetailDrawSampleCount;
	}

	if (!gSpideyRetailD3D7DrawPrimitiveOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7DrawPrimitiveOriginal(
			device,
			primitiveType,
			vertexTypeDesc,
			vertices,
			vertexCount,
			flags);
}

static void SpideyResetRetailD3D7DrawProbeFrame()
{
	gSpideyRetailDrawCalls = 0;
	gSpideyRetailDrawTextured = 0;
	gSpideyRetailDrawMirrored = 0;
	gSpideyRetailDrawMissing = 0;
	gSpideyRetailDrawTriangleFan = 0;
	gSpideyRetailDrawFvf144 = 0;
	gSpideyRetailDrawOtherPrimitive = 0;
	gSpideyRetailDrawOtherFvf = 0;
	gSpideyShadowSubmitted = 0;
	gSpideyShadowSkipped = 0;
	gSpideyShadowOffscreenSkipped = 0;
	gSpideyTransientQueued = 0;
	gSpideyTransientMirrored = 0;
	gSpideyModernRangeValid = 0;
	gSpideyModernMinX = 0.0f;
	gSpideyModernMaxX = 0.0f;
	gSpideyModernMinY = 0.0f;
	gSpideyModernMaxY = 0.0f;
	gSpideyModernVertexCount = 0;
	gSpideyModernOutsidePhysicalX = 0;
	gSpideyModernOutsidePhysicalY = 0;
}

static void SpideyFlushRetailD3D7DrawProbeFrame(
		unsigned long frame)
{
	const int shouldLog =
		frame <= 5 ||
		(frame % 120) == 0 ||
		gSpideyRetailDrawMissing != 0 ||
		gSpideyRetailDrawOtherPrimitive != 0 ||
		gSpideyRetailDrawOtherFvf != 0;

	if (shouldLog)
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");

		if (f)
		{
			fprintf(
				f,
				"draw_frame frame=%lu calls=%lu textured=%lu mirrored=%lu missing=%lu triangle_fan=%lu fvf_0x144=%lu other_primitive=%lu other_fvf=%lu shadow_submit=%lu shadow_skip=%lu shadow_offscreen_skip=%lu transient_queued=%lu transient_mirrored=%lu resident=%lu device=0x%08lX modern=%d logical=%lux%lu physical=%lux%lu range_valid=%d xrange=%.3f,%.3f yrange=%.3f,%.3f vertices=%lu outside_physical_x=%lu outside_physical_y=%lu\n",
				frame,
				gSpideyRetailDrawCalls,
				gSpideyRetailDrawTextured,
				gSpideyRetailDrawMirrored,
				gSpideyRetailDrawMissing,
				gSpideyRetailDrawTriangleFan,
				gSpideyRetailDrawFvf144,
				gSpideyRetailDrawOtherPrimitive,
				gSpideyRetailDrawOtherFvf,
				gSpideyShadowSubmitted,
				gSpideyShadowSkipped,
				gSpideyShadowOffscreenSkipped,
				gSpideyTransientQueued,
				gSpideyTransientMirrored,
				SpideyRenderer11GetMirroredTextureCount(),
				(unsigned long)gSpideyRetailD3D7DrawProbeDevice,
				SpideyUseModernGameplayAspect() ? 1 : 0,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				gSpideyLegacyPhysicalWidth,
				gSpideyLegacyPhysicalHeight,
				gSpideyModernRangeValid,
				gSpideyModernMinX,
				gSpideyModernMaxX,
				gSpideyModernMinY,
				gSpideyModernMaxY,
				gSpideyModernVertexCount,
				gSpideyModernOutsidePhysicalX,
				gSpideyModernOutsidePhysicalY);
			fclose(f);
		}
	}

	SpideyResetRetailD3D7DrawProbeFrame();
}

static int SpideyPatchRetailD3D7VtableMethod(
		void** vtable,
		int index,
		void* wrapper,
		void** original,
		const char* name)
{
	if (!vtable ||
		!wrapper ||
		!original)
	{
		return 0;
	}

	void* current =
		0;

	__try
		{
			current =
				vtable[index];
		}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{
			current =
				0;
		}

	if (current == wrapper)
		return *original ? 1 : 0;

	if (!SpideyIsExecutablePointer(
			current))
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"state_hook NOT installed method=%s index=%d current=0x%08lX reason=non_executable\n",
				name ? name : "unknown",
				index,
				(unsigned long)current);
			fclose(f);
		}
		return 0;
	}

	DWORD oldProtect =
		0;

	if (!VirtualProtect(
			&vtable[index],
			sizeof(void*),
			PAGE_EXECUTE_READWRITE,
			&oldProtect))
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"state_hook NOT installed method=%s index=%d reason=virtual_protect error=%lu\n",
				name ? name : "unknown",
				index,
				(unsigned long)GetLastError());
			fclose(f);
		}
		return 0;
	}

	*original =
		current;
	vtable[index] =
		wrapper;

	DWORD ignoredProtect =
		0;
	VirtualProtect(
		&vtable[index],
		sizeof(void*),
		oldProtect,
		&ignoredProtect);

	FlushInstructionCache(
		GetCurrentProcess(),
		&vtable[index],
		sizeof(void*));

	FILE* f = fopen(
		"spidey-decomp-draw.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"state_hook installed method=%s index=%d original=0x%08lX wrapper=0x%08lX\n",
			name ? name : "unknown",
			index,
			(unsigned long)current,
			(unsigned long)wrapper);
		fclose(f);
	}

	return 1;
}

static void SpideyInstallRetailD3D7DrawProbe(void)
{
	LPDIRECT3DDEVICE7 device =
		0;

	__try
		{
			device =
				*(LPDIRECT3DDEVICE7*)0x006B791C;
		}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{
			device =
				0;
		}

	if (!device)
		return;

	D3DDEVICEDESC7 caps;
	memset(
		&caps,
		0,
		sizeof(caps));

	HRESULT capsHr =
		E_FAIL;

	__try
		{
			capsHr =
				device->GetCaps(
					&caps);
		}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{
			capsHr =
				E_FAIL;
		}

	if (FAILED(capsHr))
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"draw_probe NOT installed device=0x%08lX getcaps_hr=0x%08lX\n",
				(unsigned long)device,
				(unsigned long)capsHr);
			fclose(f);
		}
		return;
	}

	void** vtable =
		0;

	__try
		{
			vtable =
				*(void***)device;
		}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{
			vtable =
				0;
		}

	if (!vtable)
		return;

	const int setRenderTargetIndex = 8;
	const int clearIndex = 10;
	const int setViewportIndex = 13;
	const int setRenderStateIndex = 20;
	const int drawPrimitiveIndex = 25;
	const int setTextureIndex = 35;
	const int setTextureStageStateIndex = 37;

	const int alreadyInstalled =
		device == gSpideyRetailD3D7DrawProbeDevice &&
		vtable == gSpideyRetailD3D7DrawProbeVtable &&
		vtable[setRenderTargetIndex] == (void*)&SpideyShadowD3D7SetRenderTarget &&
		vtable[clearIndex] == (void*)&SpideyShadowD3D7Clear &&
		vtable[setViewportIndex] == (void*)&SpideyShadowD3D7SetViewport &&
		vtable[setRenderStateIndex] == (void*)&SpideyShadowD3D7SetRenderState &&
		vtable[drawPrimitiveIndex] == (void*)&SpideyProbeD3D7DrawPrimitive &&
		vtable[setTextureIndex] == (void*)&SpideyShadowD3D7SetTexture &&
		vtable[setTextureStageStateIndex] == (void*)&SpideyShadowD3D7SetTextureStageState;

	if (alreadyInstalled)
		return;

	const int renderTargetOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			setRenderTargetIndex,
			(void*)&SpideyShadowD3D7SetRenderTarget,
			(void**)&gSpideyRetailD3D7SetRenderTargetOriginal,
			"SetRenderTarget");

	const int clearOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			clearIndex,
			(void*)&SpideyShadowD3D7Clear,
			(void**)&gSpideyRetailD3D7ClearOriginal,
			"Clear");

	const int viewportOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			setViewportIndex,
			(void*)&SpideyShadowD3D7SetViewport,
			(void**)&gSpideyRetailD3D7SetViewportOriginal,
			"SetViewport");

	const int renderStateOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			setRenderStateIndex,
			(void*)&SpideyShadowD3D7SetRenderState,
			(void**)&gSpideyRetailD3D7SetRenderStateOriginal,
			"SetRenderState");

	const int drawOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			drawPrimitiveIndex,
			(void*)&SpideyProbeD3D7DrawPrimitive,
			(void**)&gSpideyRetailD3D7DrawPrimitiveOriginal,
			"DrawPrimitive");

	const int textureOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			setTextureIndex,
			(void*)&SpideyShadowD3D7SetTexture,
			(void**)&gSpideyRetailD3D7SetTextureOriginal,
			"SetTexture");

	const int textureStateOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			setTextureStageStateIndex,
			(void*)&SpideyShadowD3D7SetTextureStageState,
			(void**)&gSpideyRetailD3D7SetTextureStageStateOriginal,
			"SetTextureStageState");

	if (!renderTargetOk ||
		!clearOk ||
		!viewportOk ||
		!renderStateOk ||
		!drawOk ||
		!textureOk ||
		!textureStateOk)
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"draw_probe partial device=0x%08lX vtable=0x%08lX target=%d clear=%d viewport=%d renderstate=%d draw=%d texture=%d texstate=%d\n",
				(unsigned long)device,
				(unsigned long)vtable,
				renderTargetOk,
				clearOk,
				viewportOk,
				renderStateOk,
				drawOk,
				textureOk,
				textureStateOk);
			fclose(f);
		}
		return;
	}

	gSpideyRetailD3D7DrawProbeDevice =
		device;
	gSpideyRetailD3D7DrawProbeVtable =
		vtable;

	SpideyInitializeRetailShadowState(
		device);

	FILE* f = fopen(
		"spidey-decomp-draw.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"draw_probe installed device_slot=0x006B791C device=0x%08lX vtable=0x%08lX draw_index=%d getcaps_hr=0x%08lX max_tex=%lux%lu state_hooks=7 shadow_state_valid=%d\n",
			(unsigned long)device,
			(unsigned long)vtable,
			drawPrimitiveIndex,
			(unsigned long)capsHr,
			(unsigned long)caps.dwMaxTextureWidth,
			(unsigned long)caps.dwMaxTextureHeight,
			gSpideyRetailShadowStateValid);
		fclose(f);
	}

	SpideyResetRetailD3D7DrawProbeFrame();
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
	// Phase 0 of the renderer migration: prove the modern x86 DX11 helper
	// DLL can be loaded and can create a hardware D3D11 device. Rendering
	// remains on D3D7 until individual DXPOLY responsibilities are migrated.
	SpideyProbeRenderer11Bridge();

	SpideyRestoreSavedRenderResolution();

	SpideyRetailDXINITFn retail =
		(SpideyRetailDXINITFn)0x004FDE90;

	retail(
		hwnd,
		hInstance,
		options | 1);

	SpideyInjectModernVideoModes();
	SpideyKeepBorderlessMonitorWindow(hwnd);

	// Phase 2B needs the DX11 device alive before PCTex starts creating game
	// textures. Initialize the modern swap chain immediately after the retail
	// DirectDraw/D3D7 init completes instead of waiting for the first Flip.
	RECT clientRect;
	if (GetClientRect(hwnd, &clientRect))
	{
		unsigned long width =
			(unsigned long)(clientRect.right - clientRect.left);
		unsigned long height =
			(unsigned long)(clientRect.bottom - clientRect.top);

		if (width && height)
		{
			int earlyReady =
				SpideyEnsureRenderer11Presentation(
					hwnd,
					width,
					height);

			FILE* f = fopen(
				"spidey-decomp-compat.log",
				"a");
			if (f)
			{
				fprintf(
					f,
					"renderer11_phase2b early_initialize hwnd=0x%08lX size=%lux%lu result=%d\n",
					(unsigned long)hwnd,
					width,
					height,
					earlyReady);
				fclose(f);
			}
		}
	}

	SpideyInstallRetailD3D7DrawProbe();
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
		unsigned long samplePixels[9];
		memset(
			samplePixels,
			0,
			sizeof(samplePixels));

		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				COLORREF pixel =
					GetPixel(
						dc,
						xs[x],
						ys[y]);

				samplePixels[y * 3 + x] =
					(unsigned long)pixel;

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
			" sample_hash=0x%08lX nonblack=%d samples=%06lX,%06lX,%06lX,%06lX,%06lX,%06lX,%06lX,%06lX,%06lX",
			sampleHash,
			nonBlack,
			samplePixels[0],
			samplePixels[1],
			samplePixels[2],
			samplePixels[3],
			samplePixels[4],
			samplePixels[5],
			samplePixels[6],
			samplePixels[7],
			samplePixels[8]);

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

	const int renderer11Ready =
		SpideyEnsureRenderer11Presentation(
			hwnd,
			(unsigned long)dstWidth,
			(unsigned long)dstHeight);

	if (renderer11Ready &&
		gSpideyRenderer11PresentPixels &&
		!gSpideyRenderer11PixelsDisabled)
	{
		DDSURFACEDESC2 lockedDesc;
		memset(
			&lockedDesc,
			0,
			sizeof(lockedDesc));
		lockedDesc.dwSize =
			sizeof(lockedDesc);

		HRESULT lockHr =
			scene->Lock(
				0,
				&lockedDesc,
				DDLOCK_WAIT | DDLOCK_READONLY,
				0);
		int lockRetried =
			0;

		if (FAILED(lockHr))
		{
			memset(
				&lockedDesc,
				0,
				sizeof(lockedDesc));
			lockedDesc.dwSize =
				sizeof(lockedDesc);

			lockHr =
				scene->Lock(
					0,
					&lockedDesc,
					DDLOCK_WAIT,
					0);
			lockRetried =
				SUCCEEDED(lockHr) ? 1 : 0;
		}

		int pixelFormatOk =
			SUCCEEDED(lockHr) &&
			lockedDesc.lpSurface &&
			lockedDesc.lPitch > 0 &&
			lockedDesc.ddpfPixelFormat.dwRGBBitCount == 32 &&
			lockedDesc.ddpfPixelFormat.dwRBitMask == 0x00FF0000 &&
			lockedDesc.ddpfPixelFormat.dwGBitMask == 0x0000FF00 &&
			lockedDesc.ddpfPixelFormat.dwBBitMask == 0x000000FF;

		if (pixelFormatOk)
		{
			int dx11PixelsPresented =
				gSpideyRenderer11PresentPixels(
					lockedDesc.lpSurface,
					(unsigned long)lockedDesc.dwWidth,
					(unsigned long)lockedDesc.dwHeight,
					(long)lockedDesc.lPitch,
					1,
					0);

			scene->Unlock(0);

			if (dx11PixelsPresented)
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
							"compat_present_dx11_pixels frame=%lu result=1 src=%lux%lu pitch=%ld dst=%dx%d aspect_fit=1 lock_retry=%d\n",
							frame,
							(unsigned long)lockedDesc.dwWidth,
							(unsigned long)lockedDesc.dwHeight,
							(long)lockedDesc.lPitch,
							dstWidth,
							dstHeight,
							lockRetried);
						fclose(f);
					}
				}

				return 3;
			}

			gSpideyRenderer11PixelsDisabled =
				1;

			FILE* compat = fopen(
				"spidey-decomp-compat.log",
				"a");
			if (compat)
			{
				fprintf(
					compat,
					"renderer11_phase2 pixel_present_failed disabling_pixels_fallback=dx11_hdc\n");
				fclose(compat);
			}
		}
		else
		{
			if (SUCCEEDED(lockHr))
				scene->Unlock(0);

			if (shouldLog)
			{
				FILE* f = fopen(
					"spidey-decomp-present.log",
					"a");
				if (f)
				{
					fprintf(
						f,
						"compat_present_dx11_pixels frame=%lu skipped lock_hr=0x%08lX ptr=0x%08lX pitch=%ld bpp=%lu r=0x%08lX g=0x%08lX b=0x%08lX fallback=dx11_hdc\n",
						frame,
						(unsigned long)lockHr,
						(unsigned long)lockedDesc.lpSurface,
						(long)lockedDesc.lPitch,
						(unsigned long)lockedDesc.ddpfPixelFormat.dwRGBBitCount,
						(unsigned long)lockedDesc.ddpfPixelFormat.dwRBitMask,
						(unsigned long)lockedDesc.ddpfPixelFormat.dwGBitMask,
						(unsigned long)lockedDesc.ddpfPixelFormat.dwBBitMask);
					fclose(f);
				}
			}
		}
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

	if (renderer11Ready &&
		gSpideyRenderer11PresentHdc)
	{
		int dx11Presented =
			gSpideyRenderer11PresentHdc(
				sceneDC,
				(unsigned long)desc.dwWidth,
				(unsigned long)desc.dwHeight,
				1,
				0);

		if (dx11Presented)
		{
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
						"compat_present_dx11_hdc frame=%lu result=1 src=%lux%lu dst=%dx%d aspect_fit=1\n",
						frame,
						(unsigned long)desc.dwWidth,
						(unsigned long)desc.dwHeight,
						dstWidth,
						dstHeight);
					fclose(f);
				}
			}

			return 2;
		}

		FILE* compat = fopen(
			"spidey-decomp-compat.log",
			"a");
		if (compat)
		{
			fprintf(
				compat,
				"renderer11_phase2 hdc_present_failed disabling_dx11_present_fallback=gdi_hwnd\n");
			fclose(compat);
		}

		if (gSpideyRenderer11Shutdown)
			gSpideyRenderer11Shutdown();

		gSpideyRenderer11Initialized = 0;
		gSpideyRenderer11PresentationDisabled = 1;
	}

	HDC windowDC =
		::GetDC(hwnd);

	BOOL copyOk =
		FALSE;
	DWORD copyError =
		0;
	int usedStretch =
		0;

	int presentX =
		0;
	int presentY =
		0;
	int presentWidth =
		dstWidth;
	int presentHeight =
		dstHeight;
	int aspectFit =
		0;

	if (windowDC)
	{
		// Resolutions are capped far below the 32-bit product range, so keep
		// this old-MSVC-friendly instead of requiring long long syntax.
		const unsigned long srcWide =
			(unsigned long)desc.dwWidth *
			(unsigned long)dstHeight;
		const unsigned long dstWide =
			(unsigned long)dstWidth *
			(unsigned long)desc.dwHeight;

		if (srcWide != dstWide)
		{
			aspectFit =
				1;

			if (srcWide > dstWide)
			{
				presentWidth =
					dstWidth;
				presentHeight =
					(int)(
						(unsigned long)dstWidth *
						(unsigned long)desc.dwHeight /
						(unsigned long)desc.dwWidth);
				presentY =
					(dstHeight - presentHeight) / 2;
			}
			else
			{
				presentHeight =
					dstHeight;
				presentWidth =
					(int)(
						(unsigned long)dstHeight *
						(unsigned long)desc.dwWidth /
						(unsigned long)desc.dwHeight);
				presentX =
					(dstWidth - presentWidth) / 2;
			}

			// Do not clear the entire client before the copy. Doing so creates
			// a visible black frame whenever GDI presents the FillRect before
			// the subsequent StretchBlt. Clear only the actual bars.
			HBRUSH blackBrush =
				(HBRUSH)GetStockObject(BLACK_BRUSH);
			RECT barRect;

			if (presentX > 0)
			{
				barRect.left = 0;
				barRect.top = 0;
				barRect.right = presentX;
				barRect.bottom = dstHeight;
				FillRect(windowDC, &barRect, blackBrush);
			}

			if (presentX + presentWidth < dstWidth)
			{
				barRect.left = presentX + presentWidth;
				barRect.top = 0;
				barRect.right = dstWidth;
				barRect.bottom = dstHeight;
				FillRect(windowDC, &barRect, blackBrush);
			}

			if (presentY > 0)
			{
				barRect.left = presentX;
				barRect.top = 0;
				barRect.right = presentX + presentWidth;
				barRect.bottom = presentY;
				FillRect(windowDC, &barRect, blackBrush);
			}

			if (presentY + presentHeight < dstHeight)
			{
				barRect.left = presentX;
				barRect.top = presentY + presentHeight;
				barRect.right = presentX + presentWidth;
				barRect.bottom = dstHeight;
				FillRect(windowDC, &barRect, blackBrush);
			}
		}

		if (presentWidth == (int)desc.dwWidth &&
			presentHeight == (int)desc.dwHeight)
		{
			copyOk =
				BitBlt(
					windowDC,
					presentX,
					presentY,
					presentWidth,
					presentHeight,
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
					presentX,
					presentY,
					presentWidth,
					presentHeight,
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
				"compat_present frame=%lu result=%d error=%lu src=%lux%lu dst=%dx%d present=%d,%d,%dx%d stretch=%d aspect_fit=%d\n",
				frame,
				copyOk ? 1 : 0,
				(unsigned long)copyError,
				(unsigned long)desc.dwWidth,
				(unsigned long)desc.dwHeight,
				dstWidth,
				dstHeight,
				presentX,
				presentY,
				presentWidth,
				presentHeight,
				usedStretch,
				aspectFit);
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

	int shadowPreviewToggled =
		0;
	int shadowPreviewToggledOn =
		0;
	int shadowReferenceDelay =
		0;

	if (!gSpideyShadowPreviewModeSynced)
	{
		SpideyRenderer11ShadowSetContinuous(
			gSpideyShadowPreviewEnabled);
		gSpideyShadowPreviewModeSynced =
			1;

		FILE* previewLog = fopen(
			"spidey-decomp-present.log",
			"a");
		if (previewLog)
		{
			fprintf(
				previewLog,
				"shadow_default frame=%lu enabled=%d mode=dx11_geometry key=F10_reference_toggle\n",
				frame,
				gSpideyShadowPreviewEnabled);
			fclose(previewLog);
		}
	}

	if (GetAsyncKeyState(VK_F10) & 1)
	{
		gSpideyShadowPreviewEnabled =
			gSpideyShadowPreviewEnabled ? 0 : 1;
		gSpideyShadowPreviewReady =
			0;
		shadowPreviewToggled =
			1;
		shadowPreviewToggledOn =
			gSpideyShadowPreviewEnabled ? 1 : 0;

		if (gSpideyShadowPreviewEnabled)
		{
			SpideyRenderer11ShadowSetContinuous(1);
			SpideyApplyLogicalRenderResolution(
				1,
				"f10_dx11");
		}
		else
		{
			// The frame that just finished drawing still used the modern
			// logical viewport. Keep that completed DX11 frame visible once,
			// switch game projection back to the physical D3D7 aspect for
			// the next frame, then enter reference mode cleanly.
			shadowReferenceDelay =
				1;
			SpideyApplyLogicalRenderResolution(
				0,
				"f10_d3d7_reference");
		}

		FILE* previewLog = fopen(
			"spidey-decomp-present.log",
			"a");
		if (previewLog)
		{
			fprintf(
				previewLog,
				"shadow_preview_toggle frame=%lu enabled=%d key=F10\n",
				frame,
				gSpideyShadowPreviewEnabled);
			fclose(previewLog);
		}
	}

	// DXPOLY_Flip runs after retail EndScene. Transient texture surfaces are
	// no longer actively bound for drawing here, so this is the safe point
	// to lock/mirror them and then replay the queued retail primitive stream
	// into the completely offscreen DX11 shadow target.
	SpideyProcessPendingTransientSurfaces();

	unsigned long shadowWidth =
		(unsigned long)*(DWORD*)0x006B78E4;
	unsigned long shadowHeight =
		(unsigned long)*(DWORD*)0x006B78E8;

	LPDIRECTDRAWSURFACE7 shadowScene =
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	if (shadowScene)
	{
		DDSURFACEDESC2 shadowDesc;
		memset(
			&shadowDesc,
			0,
			sizeof(shadowDesc));
		shadowDesc.dwSize =
			sizeof(shadowDesc);

		if (SUCCEEDED(
				shadowScene->GetSurfaceDesc(
					&shadowDesc)) &&
			shadowDesc.dwWidth &&
			shadowDesc.dwHeight)
		{
			shadowWidth =
				(unsigned long)shadowDesc.dwWidth;
			shadowHeight =
				(unsigned long)shadowDesc.dwHeight;
		}
	}

	if (SpideyUseModernGameplayAspect())
	{
		shadowWidth =
			gSpideyModernLogicalWidth;
		shadowHeight =
			gSpideyModernLogicalHeight;
	}

	int shadowFrameResult =
		0;

	if (shadowWidth &&
		shadowHeight)
	{
		shadowFrameResult =
			SpideyRenderer11ShadowEndFrame(
				frame,
				shadowWidth,
				shadowHeight);
	}

	if (shadowReferenceDelay)
	{
		// EndFrame replayed the just-completed modern frame while continuous
		// capture was still enabled. Disable it only after that safe replay.
		SpideyRenderer11ShadowSetContinuous(0);
	}

	if (!gSpideyShadowPreviewEnabled)
	{
		gSpideyShadowPreviewReady =
			0;
	}
	else if (!shadowPreviewToggledOn &&
		shadowFrameResult)
	{
		// Enabling at this Flip is intentionally a one-frame warmup: the
		// just-finished frame may have been sampled rather than captured
		// continuously. The next frame is fully captured before previewing.
		gSpideyShadowPreviewReady =
			1;
	}

	if ((frame <= 5 ||
		 (frame % 120) == 0) &&
		!shadowFrameResult)
	{
		FILE* f = fopen(
			"spidey-decomp-draw.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"shadow_frame_bridge frame=%lu result=0 target=%lux%lu\n",
				frame,
				shadowWidth,
				shadowHeight);
			fclose(f);
		}
	}

	// The original D3D7 draw path remains authoritative and visible. These
	// counters describe the same completed retail frame that was shadowed.
	SpideyFlushRetailD3D7DrawProbeFrame(
		frame);
	SpideyInstallRetailD3D7DrawProbe();

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
		rectCorrected ||
		shadowPreviewToggled;

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

	const int windowedCompat =
		*(DWORD*)0x006B78F4 ? 1 : 0;

	// In the compatibility/windowed path, retail DXPOLY_Flip blits the
	// scene into the legacy DirectDraw primary before our direct HWND
	// presenter copies the same scene again. Runtime logs show that legacy
	// primary at 1920x1080 while the borderless client is 2560x1440. Letting
	// both presentation paths race can expose stale/foreign primary content
	// between our copies. Use the direct scene->HWND presenter as the sole
	// windowed presentation path; preserve untouched retail Flip behavior
	// for the original non-windowed path.
	int compatPresentPath =
		0;

	if (!windowedCompat)
	{
		retailFlip();
	}
	else if (((gSpideyShadowPreviewEnabled &&
			   gSpideyShadowPreviewReady) ||
			  shadowReferenceDelay) &&
		SpideyRenderer11PresentShadow(
			1,
			0))
	{
		compatPresentPath =
			4;
	}
	else
	{
		compatPresentPath =
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
			fprintf(
				f,
				"present_path frame=%lu windowed=%d retail_flip=%d dx11=%d dx11_shadow=%d dx11_pixels=%d dx11_hdc=%d direct_hwnd=%d shadow_preview=%d shadow_ready=%d compat_result=%d\n",
				frame,
				windowedCompat,
				windowedCompat ? 0 : 1,
				compatPresentPath >= 2 ? 1 : 0,
				compatPresentPath == 4 ? 1 : 0,
				compatPresentPath == 3 ? 1 : 0,
				compatPresentPath == 2 ? 1 : 0,
				compatPresentPath == 1 ? 1 : 0,
				gSpideyShadowPreviewEnabled,
				gSpideyShadowPreviewReady,
				compatPresentPath);
			fclose(f);
		}
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


static void SpideyReleaseRetailMovieSurface(
		const char* reason)
{
	LPDIRECTDRAWSURFACE7* slot =
		(LPDIRECTDRAWSURFACE7*)0x00AC0A3C;
	LPDIRECTDRAWSURFACE7 surface =
		*slot;

	if (!surface)
		return;

	DDSURFACEDESC2 desc;
	memset(&desc, 0, sizeof(desc));
	desc.dwSize =
		sizeof(desc);

	HRESULT descHr =
		surface->GetSurfaceDesc(
			&desc);

	ULONG refs =
		surface->Release();
	*slot =
		0;

	FILE* f = fopen(
		"spidey-decomp-present.log",
		"a");

	if (f)
	{
		fprintf(
			f,
			"movie_surface_release reason=%s surface=0x%08lX desc_hr=0x%08lX size=%lux%lu remaining_refs=%lu\n",
			reason ? reason : "unknown",
			(unsigned long)surface,
			(unsigned long)descHr,
			(unsigned long)desc.dwWidth,
			(unsigned long)desc.dwHeight,
			(unsigned long)refs);
		fclose(f);
	}
}

static void __cdecl SpideyDiagMovieFlip(void)
{
	HBINK movie =
		*(HBINK*)0x00AC0BA4;
	const int finalFrame =
		movie &&
		movie->Frames &&
		movie->FrameNum == movie->Frames;

	SpideyDiagDXPOLYFlip();

	if (finalFrame)
	{
		SpideyReleaseRetailMovieSurface(
			"final_frame");
	}
}

typedef void (__cdecl *SpideyRetailMovieStopFn)(void);

static void __cdecl SpideyCompatMovieStop(void)
{
	SpideyRetailMovieStopFn retailStop =
		(SpideyRetailMovieStopFn)0x0050B790;

	retailStop();

	SpideyReleaseRetailMovieSurface(
		"stop_call");
}

static void SpideyInstallMovieStopCompat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	const unsigned long retailStop =
		0x0050B790;

	int patched =
		0;

	FILE* f = fopen(
		"spidey-decomp-present.log",
		"a");

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

		if (target != retailStop)
			continue;

		long newRel =
			(long)(
				(unsigned char*)&SpideyCompatMovieStop -
				(p + 5));

		*(long*)(p + 1) =
			newRel;

		FlushInstructionCache(
			GetCurrentProcess(),
			p,
			5);

		if (f)
		{
			fprintf(
				f,
				"movie_stop_patch call_site=0x%08lX retail_target=0x0050B790 wrapper=0x%08lX\n",
				(unsigned long)p,
				(unsigned long)&SpideyCompatMovieStop);
		}

		patched++;
	}

	if (f)
	{
		fprintf(
			f,
			"movie_stop_compat patched_calls=%d\n",
			patched);
		fclose(f);
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
			(unsigned char*)&SpideyDiagMovieFlip -
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
			(unsigned long)&SpideyDiagMovieFlip);
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
typedef void (__cdecl *SpideyRetailCleanup503AF0Fn)(void);

static void __cdecl SpideyCompatCleanup503AF0()
{
	void* object =
		*(void**)0x006BBF1C;

	if (!object)
	{
		FILE* f = fopen(
			"spidey-decomp-compat.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"cleanup_503AF0 skipped null_global=0x006BBF1C\n");
			fclose(f);
		}
		return;
	}

	SpideyRetailCleanup503AF0Fn retail =
		(SpideyRetailCleanup503AF0Fn)0x00503AF0;
	retail();
}

static void SpideyInstallCleanup503AF0Compat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	const unsigned long retailTarget =
		0x00503AF0;
	i32 patched =
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

		if (target != retailTarget)
			continue;

		long newRel =
			(long)(
				(unsigned char*)&SpideyCompatCleanup503AF0 -
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
			"cleanup_503AF0_compat patched_calls=%d retail=0x00503AF0 wrapper=0x%08lX\n",
			patched,
			(unsigned long)&SpideyCompatCleanup503AF0);
		fclose(f);
	}
}

typedef i32 (__cdecl *SpideyRetailPollKeyboardFn)(void);
typedef i32 (__cdecl *SpideyRetailPollMouseFn)(i32*, i32*);

static i32 gSpideyRetailInputForeground = -1;
static unsigned long gSpideyRetailInputSyncCount = 0;

static void SpideyLogRetailInput(
		const char* eventName,
		HRESULT keyboardHr,
		HRESULT mouseHr,
		HRESULT controllerHr)
{
	FILE* f = fopen(
		"spidey-decomp-input.log",
		"a");
	if (!f)
		return;

	fprintf(
		f,
		"retail_input event=%s count=%lu hwnd=0x%08lX foreground=0x%08lX active=0x%08lX focus=0x%08lX keyboard=0x%08lX mouse=0x%08lX controller=0x%08lX\n",
		eventName ? eventName : "unknown",
		++gSpideyRetailInputSyncCount,
		(unsigned long)*(HWND*)0x006B7A60,
		(unsigned long)GetForegroundWindow(),
		(unsigned long)GetActiveWindow(),
		(unsigned long)GetFocus(),
		(unsigned long)keyboardHr,
		(unsigned long)mouseHr,
		(unsigned long)controllerHr);
	fclose(f);
}

static i32 SpideySyncRetailInputForeground(void)
{
	HWND hwnd =
		*(HWND*)0x006B7A60;

	const i32 foreground =
		hwnd &&
		GetForegroundWindow() == hwnd;

	if (foreground == gSpideyRetailInputForeground)
		return foreground;

	gSpideyRetailInputForeground =
		foreground;

	LPDIRECTINPUTDEVICE8A keyboard =
		*(LPDIRECTINPUTDEVICE8A*)0x006B7A5C;
	LPDIRECTINPUTDEVICE8A mouse =
		*(LPDIRECTINPUTDEVICE8A*)0x006B7A64;
	LPDIRECTINPUTDEVICE8A controller =
		*(LPDIRECTINPUTDEVICE8A*)0x006B7A2C;

	// Clear the real retail transition-state arrays on every focus edge so
	// a held/released key from before Alt+Tab cannot poison menu navigation.
	memset(
		(void*)0x006B792C,
		0,
		0x100);
	memset(
		(void*)0x006B7A54,
		0,
		3);
	memset(
		(void*)0x006B7A34,
		0,
		0x20);

	HRESULT keyboardHr =
		DI_OK;
	HRESULT mouseHr =
		DI_OK;
	HRESULT controllerHr =
		DI_OK;

	if (foreground)
	{
		if (keyboard)
			keyboardHr = keyboard->Acquire();
		if (mouse)
			mouseHr = mouse->Acquire();
		if (controller)
			controllerHr = controller->Acquire();

		SpideyLogRetailInput(
			"foreground_acquire",
			keyboardHr,
			mouseHr,
			controllerHr);
	}
	else
	{
		if (keyboard)
			keyboardHr = keyboard->Unacquire();
		if (mouse)
			mouseHr = mouse->Unacquire();
		if (controller)
			controllerHr = controller->Unacquire();

		SpideyLogRetailInput(
			"background_unacquire",
			keyboardHr,
			mouseHr,
			controllerHr);
	}

	return foreground;
}

static i32 __cdecl SpideyCompatRetailPollKeyboard(void)
{
	if (!SpideySyncRetailInputForeground())
		return -1;

	SpideyRetailPollKeyboardFn retail =
		(SpideyRetailPollKeyboardFn)0x00501B80;

	i32 result =
		retail();

	if (result < 0)
	{
		LPDIRECTINPUTDEVICE8A keyboard =
			*(LPDIRECTINPUTDEVICE8A*)0x006B7A5C;
		HRESULT acquireHr =
			keyboard ? keyboard->Acquire() : E_FAIL;

		SpideyLogRetailInput(
			"keyboard_retry",
			acquireHr,
			DI_OK,
			DI_OK);

		if (keyboard &&
			SUCCEEDED(acquireHr))
		{
			result =
				retail();
		}
	}

	return result;
}

static i32 __cdecl SpideyCompatRetailPollMouse(
		i32* pY,
		i32* pX)
{
	if (!SpideySyncRetailInputForeground())
	{
		if (pY)
			*pY = 0;
		if (pX)
			*pX = 0;
		return 0;
	}

	SpideyRetailPollMouseFn retail =
		(SpideyRetailPollMouseFn)0x00501CC0;

	return retail(
		pY,
		pX);
}

static void SpideyInstallRetailInputCompat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;

	const unsigned long retailKeyboard =
		0x00501B80;
	const unsigned long retailMouse =
		0x00501CC0;

	i32 keyboardCalls =
		0;
	i32 mouseCalls =
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

		void* wrapper =
			0;

		if (target == retailKeyboard)
		{
			wrapper =
				(void*)&SpideyCompatRetailPollKeyboard;
			keyboardCalls++;
		}
		else if (target == retailMouse)
		{
			wrapper =
				(void*)&SpideyCompatRetailPollMouse;
			mouseCalls++;
		}
		else
		{
			continue;
		}

		long newRel =
			(long)(
				(unsigned char*)wrapper -
				(p + 5));

		*(long*)(p + 1) =
			newRel;

		FlushInstructionCache(
			GetCurrentProcess(),
			p,
			5);
	}

	FILE* f = fopen(
		"spidey-decomp-input.log",
		"a");
	if (f)
	{
		fprintf(
			f,
			"retail_input_compat installed keyboard_calls=%d mouse_calls=%d keyboard=0x00501B80 mouse=0x00501CC0 hwnd=0x006B7A60\n",
			keyboardCalls,
			mouseCalls);
		fclose(f);
	}

	// Force the first live poll to establish and log foreground state.
	gSpideyRetailInputForeground =
		-1;
}

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
	SpideyInstallDisplayOptionsCompat();
	SpideyInstallPresentProbe();
	SpideyInstallMoviePresentCompat();
	SpideyInstallMovieStopCompat();
	SpideyInstallRetailInputCompat();
	SpideyInstallCleanup503AF0Compat();

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

			// Must happen before the game creates its HWND/DirectDraw objects.
			// Dynamic lookup keeps the matching VC6-era SDK build compatible.
			SpideyEnableDpiAwarenessEarly();

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
