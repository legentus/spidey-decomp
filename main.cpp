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
#include "screen.h"
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
#include "m3dzone.h"
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
#include <math.h>
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
#include "physics.h"
#include "PRE.h"
#include "dcfileio.h"
#include "PCMovie.h"
#include "flash.h"
#include "renderer11_legacy_bridge.h"
#include "input11_legacy_bridge.h"


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

// @Ok
static FILE* SpideyOpenConsolidatedLog(
		const char* category)
{
	FILE* f =
		fopen(
			"spidey-decomp.log",
			"a");

	if (!f)
		return 0;

	fprintf(
		f,
		"[%s] ",
		category ?
			category :
			"GENERAL");

	return f;
}

static void SpideyAppendCompatLog(
		DWORD width,
		DWORD height,
		DWORD bpp,
		HRESULT firstResult,
		HRESULT retryResult,
		int retried)
{
	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
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


	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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


static void SpideyApplySelectedWindowStyle(
		HWND hwnd,
		const char* reason);

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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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
	SpideyApplySelectedWindowStyle(
		hwnd,
		"directdraw_init");
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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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

static unsigned long gSpideyPendingOutputWidth = 640;
static unsigned long gSpideyPendingOutputHeight = 480;
static int gSpideyAspectMode = 0;
static int gSpideyPendingAspectMode = 0;

enum SpideyWindowMode
{
	SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE = 0,
	SPIDEY_WINDOW_BORDERLESS = 1,
	SPIDEY_WINDOW_WINDOWED = 2
};

static int gSpideyWindowMode =
	SPIDEY_WINDOW_BORDERLESS;
static int gSpideyPendingWindowMode =
	SPIDEY_WINDOW_BORDERLESS;
static CMenu* gSpideyDisplayMenu = 0;

static const int kSpideyUiScaleMinPercent = 50;
static const int kSpideyUiScaleMaxPercent = 200;
static const int kSpideyUiScaleStepPercent = 5;
static const int kSpideyDefaultGameplayUiScalePercent = 125;
static const int kSpideyDefaultMenuTextScalePercent = 100;
static const int kSpideyCameraSensitivityMinPercent = 25;
static const int kSpideyCameraSensitivityMaxPercent = 200;
static const int kSpideyCameraSensitivityStepPercent = 5;
static const int kSpideyDefaultCameraSensitivityPercent = 100;

static int gSpideyGameplayUiScalePercent =
	kSpideyDefaultGameplayUiScalePercent;
static int gSpideyPendingGameplayUiScalePercent =
	kSpideyDefaultGameplayUiScalePercent;
static int gSpideyMenuTextScalePercent =
	kSpideyDefaultMenuTextScalePercent;
static int gSpideyPendingMenuTextScalePercent =
	kSpideyDefaultMenuTextScalePercent;
static int gSpideyCameraSensitivityPercent =
	kSpideyDefaultCameraSensitivityPercent;
static int gSpideyPendingCameraSensitivityPercent =
	kSpideyDefaultCameraSensitivityPercent;

static char gSpideyGameplayUiScaleMenuLabel[64] =
	"Gameplay UI Scale: 125%";
static char gSpideyMenuTextScaleMenuLabel[64] =
	"Menu/Text Scale: 100%";
static char gSpideyPauseOptionsLabel[] =
	"Options";
static char gSpideyPauseOptionsHeadingLabel[] =
	"Options";
static char gSpideyPauseGameplayUiScaleMenuLabel[64] =
	"UI Scale: 125%";
static char gSpideyPauseMenuTextScaleMenuLabel[64] =
	"Text Scale: 100%";
static char gSpideyPauseCameraSensitivityMenuLabel[64] =
	"Camera Sensitivity: 100%";
static char gSpideyPauseApplyUiScaleLabel[] =
	"Apply Settings";
static char gSpideyPauseBackLabel[] =
	"Back";
static int gSpideyPauseOptionsActive = 0;
static int gSpideyPauseParentSnapshotValid = 0;
static CMenu* gSpideyPauseMenuOwner = 0;
static int gSpideyPauseEnterHeld = 0;
static int gSpideyPauseLastEnterRawState = -1;
// CMenu is validated as 0x53C bytes. Preserve everything after the vtable
// and expanding-box pointer while the same retail pause CMenu is repurposed
// as our custom Options submenu. The compile-time guard prevents a future
// layout change from silently making this snapshot range invalid.
typedef char SpideyPauseCMenuSizeCheck[
	(sizeof(CMenu) == 0x53C) ? 1 : -1];
static unsigned char gSpideyPauseParentState[
	sizeof(CMenu) - 8];
static int gSpideyInLevelDisplayMenuActive = 0;

static void SpideyApplyFrontendTextScale(
		const char* reason);

static const char* const gSpideyAspectLabels[] =
{
	"AUTO",
	"4:3",
	"5:4",
	"16:9",
	"16:10",
	"21:9",
	"32:9"
};
static char gSpideyAspectRatioMenuLabel[] =
	"Aspect Ratio";
static const char* const gSpideyWindowModeLabels[] =
{
	"Fullscreen Exclusive",
	"Borderless",
	"Windowed"
};
static char gSpideyDisplayModeMenuLabel[96] =
	"Display Mode: Borderless";
static char gSpideyDisplayApplyMenuLabel[] =
	"Apply";
static char gSpideyModernVideoIniPath[MAX_PATH];

static const char* SpideyGetModernVideoIniPath()
{
	if (gSpideyModernVideoIniPath[0])
		return gSpideyModernVideoIniPath;

	DWORD length =
		GetModuleFileNameA(
			0,
			gSpideyModernVideoIniPath,
			MAX_PATH);

	if (!length ||
		length >= MAX_PATH)
	{
		strcpy(
			gSpideyModernVideoIniPath,
			".\\spidey-modern-video.ini");
		return gSpideyModernVideoIniPath;
	}

	char* slash =
		strrchr(
			gSpideyModernVideoIniPath,
			'\\');
	if (!slash)
	{
		slash =
			strrchr(
				gSpideyModernVideoIniPath,
				'/');
	}

	if (slash)
	{
		++slash;
		*slash =
			0;

		const char* fileName =
			"spidey-modern-video.ini";

		if (strlen(gSpideyModernVideoIniPath) +
			strlen(fileName) <
			MAX_PATH)
		{
			strcat(
				gSpideyModernVideoIniPath,
				fileName);
		}
	}
	else
	{
		strcpy(
			gSpideyModernVideoIniPath,
			".\\spidey-modern-video.ini");
	}

	return gSpideyModernVideoIniPath;
}

// @Ok
static int SpideyClampUiScalePercent(
		int percent)
{
	if (percent < kSpideyUiScaleMinPercent)
		return kSpideyUiScaleMinPercent;
	if (percent > kSpideyUiScaleMaxPercent)
		return kSpideyUiScaleMaxPercent;
	return percent;
}

// @Ok
static int SpideyClampCameraSensitivityPercent(
		int percent)
{
	if (percent <
		kSpideyCameraSensitivityMinPercent)
	{
		return kSpideyCameraSensitivityMinPercent;
	}

	if (percent >
		kSpideyCameraSensitivityMaxPercent)
	{
		return kSpideyCameraSensitivityMaxPercent;
	}

	return percent;
}

// @Ok
static int SpideyUiScalePercentToSliderValue(
		int percent)
{
	percent =
		SpideyClampUiScalePercent(
			percent);

	const int range =
		kSpideyUiScaleMaxPercent -
		kSpideyUiScaleMinPercent;

	if (range <= 0)
		return 0;

	return ((percent -
		kSpideyUiScaleMinPercent) * 256) /
		range;
}

// @Ok
static void SpideyBuildPauseUiScaleLabel(
		char* destination,
		const char* prefix,
		int percent)
{
	if (!destination ||
		!prefix)
	{
		return;
	}

	percent =
		SpideyClampUiScalePercent(
			percent);

	sprintf(
		destination,
		"%s: %d%%",
		prefix,
		percent);
}

// @Ok
static void SpideyUpdateUiScaleMenuLabels()
{
	gSpideyPendingGameplayUiScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingGameplayUiScalePercent);
	gSpideyPendingMenuTextScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingMenuTextScalePercent);
	gSpideyPendingCameraSensitivityPercent =
		SpideyClampCameraSensitivityPercent(
			gSpideyPendingCameraSensitivityPercent);

	sprintf(
		gSpideyGameplayUiScaleMenuLabel,
		"Gameplay UI Scale: %d%%",
		gSpideyPendingGameplayUiScalePercent);
	sprintf(
		gSpideyMenuTextScaleMenuLabel,
		"Menu/Text Scale: %d%%",
		gSpideyPendingMenuTextScalePercent);

	SpideyBuildPauseUiScaleLabel(
		gSpideyPauseGameplayUiScaleMenuLabel,
		"UI Scale",
		gSpideyPendingGameplayUiScalePercent);
	SpideyBuildPauseUiScaleLabel(
		gSpideyPauseMenuTextScaleMenuLabel,
		"Text Scale",
		gSpideyPendingMenuTextScalePercent);

	sprintf(
		gSpideyPauseCameraSensitivityMenuLabel,
		"Camera Sensitivity: %d%%",
		gSpideyPendingCameraSensitivityPercent);
}

static void SpideyUpdateDisplayModeMenuLabel()
{
	if (gSpideyPendingWindowMode <
			SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE ||
		gSpideyPendingWindowMode >
			SPIDEY_WINDOW_WINDOWED)
	{
		gSpideyPendingWindowMode =
			SPIDEY_WINDOW_BORDERLESS;
	}

	sprintf(
		gSpideyDisplayModeMenuLabel,
		"Display Mode: %s",
		gSpideyWindowModeLabels[
			gSpideyPendingWindowMode]);
}

static void SpideyApplySelectedWindowStyle(
		HWND hwnd,
		const char* reason)
{
	if (!hwnd)
		return;

	int mode =
		gSpideyWindowMode;
	if (mode < SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE ||
		mode > SPIDEY_WINDOW_WINDOWED)
	{
		mode =
			SPIDEY_WINDOW_BORDERLESS;
	}

	const int screenWidth =
		GetSystemMetrics(0);
	const int screenHeight =
		GetSystemMetrics(1);

	LONG style =
		0;
	int left =
		0;
	int top =
		0;
	int outerWidth =
		(int)gSpideySelectedOutputWidth;
	int outerHeight =
		(int)gSpideySelectedOutputHeight;

	if (mode ==
		SPIDEY_WINDOW_WINDOWED)
	{
		style =
			WS_OVERLAPPEDWINDOW |
			WS_VISIBLE;

		RECT rect;
		rect.left =
			0;
		rect.top =
			0;
		rect.right =
			outerWidth;
		rect.bottom =
			outerHeight;

		if (AdjustWindowRect(
				&rect,
				style,
				FALSE))
		{
			outerWidth =
				rect.right -
				rect.left;
			outerHeight =
				rect.bottom -
				rect.top;
		}

		if (screenWidth > 0)
			left =
				(screenWidth - outerWidth) / 2;
		if (screenHeight > 0)
			top =
				(screenHeight - outerHeight) / 2;
	}
	else
	{
		style =
			WS_POPUP |
			WS_VISIBLE;

		if (mode ==
				SPIDEY_WINDOW_BORDERLESS &&
			screenWidth > 0 &&
			screenHeight > 0)
		{
			outerWidth =
				screenWidth;
			outerHeight =
				screenHeight;
		}
	}

	SetWindowLongA(
		hwnd,
		GWL_STYLE,
		style);

	SetWindowPos(
		hwnd,
		HWND_TOP,
		left,
		top,
		outerWidth,
		outerHeight,
		SWP_FRAMECHANGED |
		SWP_SHOWWINDOW);

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"window_style reason=%s mode=%d label=%s rect=%d,%d,%dx%d selected=%lux%lu\n",
			reason ? reason : "unknown",
			mode,
			gSpideyWindowModeLabels[mode],
			left,
			top,
			outerWidth,
			outerHeight,
			gSpideySelectedOutputWidth,
			gSpideySelectedOutputHeight);
		fclose(f);
	}
}

static void SpideyApplyRendererWindowMode(
		const char* reason);
static void SpideyReleaseRendererExclusiveForCompatRebuild(
		const char* reason);

struct SpideyAudioDeviceInfo
{
	GUID guid;
	int hasGuid;
	char name[128];
};

static const int kSpideyMaxAudioDevices =
	32;
static SpideyAudioDeviceInfo gSpideyAudioDevices[kSpideyMaxAudioDevices];
static int gSpideyAudioDeviceCount =
	0;
static int gSpideySelectedAudioDevice =
	0;
static int gSpideyAudioSettingsLoaded =
	0;
static char gSpideyModernAudioIniPath[MAX_PATH];
static LPDIRECTSOUND8 gSpideyRetainedBinkDirectSound =
	0;

typedef HRESULT (WINAPI *SpideyRealDirectSoundCreate8Fn)(
		LPCGUID,
		LPDIRECTSOUND8*,
		LPUNKNOWN);
typedef HRESULT (WINAPI *SpideyRealDirectSoundEnumerateAFn)(
		LPDSENUMCALLBACKA,
		LPVOID);

static SpideyRealDirectSoundCreate8Fn gSpideyRealDirectSoundCreate8 =
	0;
static SpideyRealDirectSoundEnumerateAFn gSpideyRealDirectSoundEnumerateA =
	0;
static HMODULE gSpideyDirectSoundModule =
	0;

static const char* SpideyGetModernAudioIniPath()
{
	if (gSpideyModernAudioIniPath[0])
		return gSpideyModernAudioIniPath;

	DWORD length =
		GetModuleFileNameA(
			0,
			gSpideyModernAudioIniPath,
			MAX_PATH);

	if (!length ||
		length >= MAX_PATH)
	{
		strcpy(
			gSpideyModernAudioIniPath,
			".\\spidey-modern-audio.ini");
		return gSpideyModernAudioIniPath;
	}

	char* slash =
		strrchr(
			gSpideyModernAudioIniPath,
			'\\');
	if (!slash)
	{
		slash =
			strrchr(
				gSpideyModernAudioIniPath,
				'/');
	}

	if (slash)
	{
		++slash;
		*slash =
			0;

		const char* fileName =
			"spidey-modern-audio.ini";

		if (strlen(gSpideyModernAudioIniPath) +
			strlen(fileName) <
			MAX_PATH)
		{
			strcat(
				gSpideyModernAudioIniPath,
				fileName);
		}
	}
	else
	{
		strcpy(
			gSpideyModernAudioIniPath,
			".\\spidey-modern-audio.ini");
	}

	return gSpideyModernAudioIniPath;
}

static int SpideyResolveDirectSoundExports()
{
	if (gSpideyRealDirectSoundCreate8 &&
		gSpideyRealDirectSoundEnumerateA)
	{
		return 1;
	}

	if (!gSpideyDirectSoundModule)
	{
		gSpideyDirectSoundModule =
			GetModuleHandleA(
				"dsound.dll");
		if (!gSpideyDirectSoundModule)
		{
			gSpideyDirectSoundModule =
				LoadLibraryA(
					"dsound.dll");
		}
	}

	if (!gSpideyDirectSoundModule)
		return 0;

	gSpideyRealDirectSoundCreate8 =
		(SpideyRealDirectSoundCreate8Fn)GetProcAddress(
			gSpideyDirectSoundModule,
			"DirectSoundCreate8");
	gSpideyRealDirectSoundEnumerateA =
		(SpideyRealDirectSoundEnumerateAFn)GetProcAddress(
			gSpideyDirectSoundModule,
			"DirectSoundEnumerateA");

	return gSpideyRealDirectSoundCreate8 &&
		gSpideyRealDirectSoundEnumerateA;
}

static BOOL CALLBACK SpideyAudioEnumerateCallback(
		LPGUID guid,
		LPCSTR description,
		LPCSTR module,
		LPVOID context)
{
	(void)module;
	(void)context;

	// DirectSoundEnumerate reports the primary/default driver with a NULL
	// GUID. We author our own stable row 0 for that policy.
	if (!guid)
		return TRUE;

	if (gSpideyAudioDeviceCount >=
		kSpideyMaxAudioDevices)
	{
		return FALSE;
	}

	SpideyAudioDeviceInfo* info =
		&gSpideyAudioDevices[gSpideyAudioDeviceCount];

	memset(
		info,
		0,
		sizeof(*info));
	info->guid =
		*guid;
	info->hasGuid =
		1;

	if (description &&
		description[0])
	{
		strncpy(
			info->name,
			description,
			sizeof(info->name) - 1);
		info->name[sizeof(info->name) - 1] =
			0;
	}
	else
	{
		strcpy(
			info->name,
			"DirectSound Device");
	}

	++gSpideyAudioDeviceCount;
	return TRUE;
}

static void SpideyRefreshAudioDevices()
{
	memset(
		gSpideyAudioDevices,
		0,
		sizeof(gSpideyAudioDevices));

	gSpideyAudioDeviceCount =
		1;
	gSpideyAudioDevices[0].hasGuid =
		0;
	strcpy(
		gSpideyAudioDevices[0].name,
		"(System Default)");

	if (!SpideyResolveDirectSoundExports())
		return;

	gSpideyRealDirectSoundEnumerateA(
		SpideyAudioEnumerateCallback,
		0);
}

static void SpideyAudioGuidToString(
		const GUID* guid,
		char* value,
		int valueSize)
{
	if (!value ||
		valueSize < 40)
	{
		return;
	}

	if (!guid)
	{
		strcpy(
			value,
			"default");
		return;
	}

	sprintf(
		value,
		"{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		(unsigned long)guid->Data1,
		(unsigned int)guid->Data2,
		(unsigned int)guid->Data3,
		(unsigned int)guid->Data4[0],
		(unsigned int)guid->Data4[1],
		(unsigned int)guid->Data4[2],
		(unsigned int)guid->Data4[3],
		(unsigned int)guid->Data4[4],
		(unsigned int)guid->Data4[5],
		(unsigned int)guid->Data4[6],
		(unsigned int)guid->Data4[7]);
}

static int SpideyAudioGuidFromString(
		const char* value,
		GUID* guid)
{
	if (!value ||
		!guid)
	{
		return 0;
	}

	unsigned long data1 =
		0;
	unsigned int data2 =
		0;
	unsigned int data3 =
		0;
	unsigned int data4[8];

	memset(
		data4,
		0,
		sizeof(data4));

	const int fields =
		sscanf(
			value,
			"{%8lx-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x}",
			&data1,
			&data2,
			&data3,
			&data4[0],
			&data4[1],
			&data4[2],
			&data4[3],
			&data4[4],
			&data4[5],
			&data4[6],
			&data4[7]);

	if (fields != 11)
		return 0;

	guid->Data1 =
		(DWORD)data1;
	guid->Data2 =
		(WORD)data2;
	guid->Data3 =
		(WORD)data3;

	for (int i = 0;
		 i < 8;
		 ++i)
	{
		guid->Data4[i] =
			(BYTE)data4[i];
	}

	return 1;
}

static void SpideySaveAudioSettings()
{
	char value[64];

	if (gSpideySelectedAudioDevice <= 0 ||
		gSpideySelectedAudioDevice >=
			gSpideyAudioDeviceCount)
	{
		strcpy(
			value,
			"default");
	}
	else
	{
		SpideyAudioGuidToString(
			&gSpideyAudioDevices[gSpideySelectedAudioDevice].guid,
			value,
			sizeof(value));
	}

	WritePrivateProfileStringA(
		"Audio",
		"OutputGuid",
		value,
		SpideyGetModernAudioIniPath());
}

static void SpideyLoadAudioSettings()
{
	if (!gSpideyAudioDeviceCount)
		SpideyRefreshAudioDevices();

	char value[64];
	memset(
		value,
		0,
		sizeof(value));

	GetPrivateProfileStringA(
		"Audio",
		"OutputGuid",
		"default",
		value,
		sizeof(value),
		SpideyGetModernAudioIniPath());

	gSpideySelectedAudioDevice =
		0;

	if (_stricmp(
			value,
			"default") != 0)
	{
		GUID configured;
		memset(
			&configured,
			0,
			sizeof(configured));

		if (SpideyAudioGuidFromString(
				value,
				&configured))
		{
			for (int i = 1;
				 i < gSpideyAudioDeviceCount;
				 ++i)
			{
				if (!memcmp(
					&configured,
					&gSpideyAudioDevices[i].guid,
					sizeof(GUID)))
				{
					gSpideySelectedAudioDevice =
						i;
					break;
				}
			}
		}
	}

	gSpideyAudioSettingsLoaded =
		1;
}

static const SpideyAudioDeviceInfo* SpideyGetSelectedAudioDevice()
{
	if (!gSpideyAudioSettingsLoaded)
	{
		SpideyLoadAudioSettings();
	}

	if (gSpideySelectedAudioDevice < 0 ||
		gSpideySelectedAudioDevice >=
			gSpideyAudioDeviceCount)
	{
		gSpideySelectedAudioDevice =
			0;
	}

	return &gSpideyAudioDevices[gSpideySelectedAudioDevice];
}

static HRESULT WINAPI SpideyCompatDirectSoundCreate8(
		LPCGUID requestedGuid,
		LPDIRECTSOUND8* directSound,
		LPUNKNOWN outer)
{
	if (!SpideyResolveDirectSoundExports())
		return E_FAIL;

	if (!gSpideyAudioDeviceCount)
		SpideyRefreshAudioDevices();
	if (!gSpideyAudioSettingsLoaded)
		SpideyLoadAudioSettings();

	const SpideyAudioDeviceInfo* selected =
		SpideyGetSelectedAudioDevice();
	LPCGUID effectiveGuid =
		selected && selected->hasGuid ?
			&selected->guid :
			0;

	const HRESULT hr =
		gSpideyRealDirectSoundCreate8(
			effectiveGuid,
			directSound,
			outer);

	FILE* f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		fprintf(
			f,
			"audio_create requested=0x%08lX selection=%d name=%s policy=%s result=0x%08lX\n",
			(unsigned long)requestedGuid,
			gSpideySelectedAudioDevice,
			selected ? selected->name : "(System Default)",
			effectiveGuid ? "manual_guid" : "system_default",
			(unsigned long)hr);
		fclose(f);
	}

	return hr;
}

static void SpideyInstallAudioDeviceCompat()
{
	const unsigned char expectedThunk[6] =
	{
		0xFF, 0x25, 0x24, 0xB0, 0x53, 0x00
	};
	unsigned char* thunk =
		(unsigned char*)0x00517A70;

	SpideyRefreshAudioDevices();
	SpideyLoadAudioSettings();

	int patched =
		0;
	if (!memcmp(
		thunk,
		expectedThunk,
		sizeof(expectedThunk)))
	{
		PATCH_PUSH_RET(
			0x00517A70,
			SpideyCompatDirectSoundCreate8);
		patched =
			1;
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		fprintf(
			f,
			"audio_device_compat patched=%d thunk=0x00517A70 devices=%d selected=%d config=%s\n",
			patched,
			gSpideyAudioDeviceCount,
			gSpideySelectedAudioDevice,
			SpideyGetModernAudioIniPath());

		for (int i = 0;
			 i < gSpideyAudioDeviceCount;
			 ++i)
		{
			char guidValue[64];
			SpideyAudioGuidToString(
				gSpideyAudioDevices[i].hasGuid ?
					&gSpideyAudioDevices[i].guid :
					0,
				guidValue,
				sizeof(guidValue));
			fprintf(
				f,
				"audio_device index=%d name=%s guid=%s default=%d\n",
				i,
				gSpideyAudioDevices[i].name,
				guidValue,
				i == 0 ? 1 : 0);
		}
		fclose(f);
	}
}

static float SpideyGetAspectScalar(
		int mode,
		unsigned long width,
		unsigned long height)
{
	switch (mode)
	{
		case 1:
			return 1.0f;
		case 2:
			return 1.06667f;
		case 3:
			return 0.75f;
		case 4:
			return 0.83333f;
		case 5:
			return 0.57143f;
		case 6:
			return 0.375f;
		default:
			break;
	}

	if (width < 1 || height < 1)
		return 1.0f;

	return (4.0f * (float)height) /
		(3.0f * (float)width);
}

static float SpideyGetSelectedAspectScalar()
{
	unsigned long width =
		gSpideySelectedOutputWidth;
	unsigned long height =
		gSpideySelectedOutputHeight;

	if (width < 1 || height < 1)
	{
		width =
			(unsigned long)*(DWORD*)0x02E096F8;
		height =
			(unsigned long)*(DWORD*)0x02E0970C;
	}

	return SpideyGetAspectScalar(
		gSpideyAspectMode,
		width,
		height);
}

static void SpideyLogAspectSetting(
		const char* reason)
{
	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (!f)
		return;

	fprintf(
		f,
		"display_aspect reason=%s mode=%d label=%s scalar=%.6f selected=%lux%lu pending=%lux%lu pending_aspect=%s scalar_addr=0x00550064\n",
		reason ? reason : "unknown",
		gSpideyAspectMode,
		gSpideyAspectLabels[gSpideyAspectMode],
		(double)*(float*)0x00550064,
		gSpideySelectedOutputWidth,
		gSpideySelectedOutputHeight,
		gSpideyPendingOutputWidth,
		gSpideyPendingOutputHeight,
		gSpideyAspectLabels[gSpideyPendingAspectMode]);
	fclose(f);
}

static void SpideyApplySelectedAspect(
		const char* reason)
{
	if (gSpideyAspectMode < 0 ||
		gSpideyAspectMode >=
			(int)(sizeof(gSpideyAspectLabels) /
				  sizeof(gSpideyAspectLabels[0])))
	{
		gSpideyAspectMode =
			0;
	}

	*(float*)0x00550064 =
		SpideyGetSelectedAspectScalar();

	SpideyLogAspectSetting(
		reason);
}

static void SpideySaveModernVideoSettings()
{
	char value[16];
	sprintf(
		value,
		"%d",
		gSpideyAspectMode);

	WritePrivateProfileStringA(
		"Video",
		"AspectMode",
		value,
		SpideyGetModernVideoIniPath());

	sprintf(
		value,
		"%d",
		gSpideyWindowMode);

	WritePrivateProfileStringA(
		"Video",
		"WindowMode",
		value,
		SpideyGetModernVideoIniPath());

	sprintf(
		value,
		"%d",
		gSpideyGameplayUiScalePercent);
	WritePrivateProfileStringA(
		"Video",
		"GameplayUIScalePercent",
		value,
		SpideyGetModernVideoIniPath());

	sprintf(
		value,
		"%d",
		gSpideyMenuTextScalePercent);
	WritePrivateProfileStringA(
		"Video",
		"MenuTextScalePercent",
		value,
		SpideyGetModernVideoIniPath());

	sprintf(
		value,
		"%d",
		gSpideyCameraSensitivityPercent);
	WritePrivateProfileStringA(
		"Controls",
		"CameraSensitivityPercent",
		value,
		SpideyGetModernVideoIniPath());
}

static void SpideyLoadModernVideoSettings()
{
	gSpideyAspectMode =
		GetPrivateProfileIntA(
			"Video",
			"AspectMode",
			0,
			SpideyGetModernVideoIniPath());

	if (gSpideyAspectMode < 0 ||
		gSpideyAspectMode >=
			(int)(sizeof(gSpideyAspectLabels) /
				  sizeof(gSpideyAspectLabels[0])))
	{
		gSpideyAspectMode =
			0;
	}

	gSpideyWindowMode =
		GetPrivateProfileIntA(
			"Video",
			"WindowMode",
			SPIDEY_WINDOW_BORDERLESS,
			SpideyGetModernVideoIniPath());

	if (gSpideyWindowMode <
			SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE ||
		gSpideyWindowMode >
			SPIDEY_WINDOW_WINDOWED)
	{
		gSpideyWindowMode =
			SPIDEY_WINDOW_BORDERLESS;
	}

	gSpideyGameplayUiScalePercent =
		SpideyClampUiScalePercent(
			GetPrivateProfileIntA(
				"Video",
				"GameplayUIScalePercent",
				kSpideyDefaultGameplayUiScalePercent,
				SpideyGetModernVideoIniPath()));
	gSpideyMenuTextScalePercent =
		SpideyClampUiScalePercent(
			GetPrivateProfileIntA(
				"Video",
				"MenuTextScalePercent",
				kSpideyDefaultMenuTextScalePercent,
				SpideyGetModernVideoIniPath()));
	gSpideyCameraSensitivityPercent =
		SpideyClampCameraSensitivityPercent(
			GetPrivateProfileIntA(
				"Controls",
				"CameraSensitivityPercent",
				kSpideyDefaultCameraSensitivityPercent,
				SpideyGetModernVideoIniPath()));

	gSpideyPendingWindowMode =
		gSpideyWindowMode;
	gSpideyPendingAspectMode =
		gSpideyAspectMode;
	gSpideyPendingGameplayUiScalePercent =
		gSpideyGameplayUiScalePercent;
	gSpideyPendingMenuTextScalePercent =
		gSpideyMenuTextScalePercent;
	gSpideyPendingCameraSensitivityPercent =
		gSpideyCameraSensitivityPercent;

	SpideyUpdateDisplayModeMenuLabel();
	SpideyUpdateUiScaleMenuLabels();

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"ui_scale_settings load gameplay_percent=%d text_percent=%d range=%d-%d step=%d camera_sensitivity=%d camera_range=%d-%d camera_step=%d config=%s\n",
			gSpideyGameplayUiScalePercent,
			gSpideyMenuTextScalePercent,
			kSpideyUiScaleMinPercent,
			kSpideyUiScaleMaxPercent,
			kSpideyUiScaleStepPercent,
			gSpideyCameraSensitivityPercent,
			kSpideyCameraSensitivityMinPercent,
			kSpideyCameraSensitivityMaxPercent,
			kSpideyCameraSensitivityStepPercent,
			SpideyGetModernVideoIniPath());
		fclose(f);
	}
}

static void SpideyResetPendingDisplaySettings(
		const char* reason)
{
	gSpideyPendingOutputWidth =
		gSpideySelectedOutputWidth;
	gSpideyPendingOutputHeight =
		gSpideySelectedOutputHeight;
	gSpideyPendingAspectMode =
		gSpideyAspectMode;
	gSpideyPendingWindowMode =
		gSpideyWindowMode;
	gSpideyPendingGameplayUiScalePercent =
		gSpideyGameplayUiScalePercent;
	gSpideyPendingMenuTextScalePercent =
		gSpideyMenuTextScalePercent;
	gSpideyPendingCameraSensitivityPercent =
		gSpideyCameraSensitivityPercent;
	SpideyUpdateDisplayModeMenuLabel();
	SpideyUpdateUiScaleMenuLabels();

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_pending_reset reason=%s selected=%lux%lu aspect=%s window_mode=%s gameplay_ui=%d text=%d\n",
			reason ? reason : "unknown",
			gSpideyPendingOutputWidth,
			gSpideyPendingOutputHeight,
			gSpideyAspectLabels[gSpideyPendingAspectMode],
			gSpideyWindowModeLabels[gSpideyPendingWindowMode],
			gSpideyPendingGameplayUiScalePercent,
			gSpideyPendingMenuTextScalePercent);
		fclose(f);
	}
}

static int __cdecl SpideyFormatPendingResolution(
		char* dst,
		const char*,
		u32,
		u32)
{
	if (!dst)
		return 0;

	return sprintf(
		dst,
		"%lux%lu",
		gSpideyPendingOutputWidth,
		gSpideyPendingOutputHeight);
}

static int __cdecl SpideyFormatAspectRatioValue(
		char* dst,
		const char*,
		int)
{
	if (!dst)
		return 0;

	const char* label =
		gSpideyAspectLabels[gSpideyPendingAspectMode];

	strcpy(
		dst,
		label);

	return (int)strlen(label);
}

static u32 __cdecl SpideyDisplayAspectPrev(
		u32 currentBpp)
{
	const int count =
		(int)(sizeof(gSpideyAspectLabels) /
			  sizeof(gSpideyAspectLabels[0]));

	gSpideyPendingAspectMode--;
	if (gSpideyPendingAspectMode < 0)
		gSpideyPendingAspectMode =
			count - 1;

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_pending_aspect direction=prev value=%s committed=%s\n",
			gSpideyAspectLabels[gSpideyPendingAspectMode],
			gSpideyAspectLabels[gSpideyAspectMode]);
		fclose(f);
	}

	return currentBpp;
}

static u32 __cdecl SpideyDisplayAspectNext(
		u32 currentBpp)
{
	const int count =
		(int)(sizeof(gSpideyAspectLabels) /
			  sizeof(gSpideyAspectLabels[0]));

	gSpideyPendingAspectMode++;
	if (gSpideyPendingAspectMode >= count)
		gSpideyPendingAspectMode =
			0;

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_pending_aspect direction=next value=%s committed=%s\n",
			gSpideyAspectLabels[gSpideyPendingAspectMode],
			gSpideyAspectLabels[gSpideyAspectMode]);
		fclose(f);
	}

	return currentBpp;
}

typedef u8 (__cdecl *SpideyRetailResolutionStepFn)(
		u32*,
		u32*,
		u32,
		i32,
		bool);

static u8 SpideyStepPendingResolution(
		unsigned long retailAddress,
		const char* direction,
		u32,
		i32,
		bool)
{
	u32 width =
		(u32)gSpideyPendingOutputWidth;
	u32 height =
		(u32)gSpideyPendingOutputHeight;

	SpideyRetailResolutionStepFn retail =
		(SpideyRetailResolutionStepFn)retailAddress;

	u8 result =
		retail(
			&width,
			&height,
			32,
			0,
			false);

	if (result)
	{
		gSpideyPendingOutputWidth =
			width;
		gSpideyPendingOutputHeight =
			height;
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_pending_resolution direction=%s result=%d value=%lux%lu committed=%lux%lu\n",
			direction ? direction : "unknown",
			result ? 1 : 0,
			gSpideyPendingOutputWidth,
			gSpideyPendingOutputHeight,
			gSpideySelectedOutputWidth,
			gSpideySelectedOutputHeight);
		fclose(f);
	}

	return result;
}

static u8 __cdecl SpideyDisplayPendingPrevResolution(
		u32*,
		u32*,
		u32 bpp,
		i32 option,
		bool exact)
{
	return SpideyStepPendingResolution(
		0x00500F40,
		"prev",
		bpp,
		option,
		exact);
}

static u8 __cdecl SpideyDisplayPendingNextResolution(
		u32*,
		u32*,
		u32 bpp,
		i32 option,
		bool exact)
{
	return SpideyStepPendingResolution(
		0x00500E20,
		"next",
		bpp,
		option,
		exact);
}

static u8 __cdecl SpideyDisplayAspectResolutionNoop(
		u32*,
		u32*,
		u32,
		i32,
		bool)
{
	// Retail used to change resolution after changing color depth. Row 1 is
	// now Aspect Ratio, so that compatibility search must not touch Screen Size.
	return 1;
}

// MSVC6 does not accept an explicit __thiscall function-pointer typedef.
// Use an ABI-compatible __fastcall declaration instead: menu -> ECX,
// unused dummy -> EDX, and the retail AddEntry label remains on the stack.
typedef void (__fastcall *SpideyRetailMenuAddEntryFn)(
		CMenu*,
		void*,
		const char*);

// @Ok
static void __fastcall SpideyDisplayAddBrightnessAndApply(
		CMenu* menu,
		void*,
		const char* brightnessLabel)
{
	SpideyRetailMenuAddEntryFn retailAdd =
		(SpideyRetailMenuAddEntryFn)0x0043FFF0;

	// Let AddEntry measure the longest possible mode label once so cycling
	// to Fullscreen Exclusive cannot overflow a box sized for Borderless.
	strcpy(
		gSpideyDisplayModeMenuLabel,
		"Display Mode: Fullscreen Exclusive");

	retailAdd(
		menu,
		0,
		brightnessLabel);
	retailAdd(
		menu,
		0,
		gSpideyGameplayUiScaleMenuLabel);
	retailAdd(
		menu,
		0,
		gSpideyMenuTextScaleMenuLabel);
	retailAdd(
		menu,
		0,
		gSpideyDisplayModeMenuLabel);
	retailAdd(
		menu,
		0,
		gSpideyDisplayApplyMenuLabel);

	if (menu &&
		menu->mNumLines >= 7)
	{
		menu->mY -=
			menu->mLineSep;
	}

	SpideyUpdateDisplayModeMenuLabel();
	SpideyUpdateUiScaleMenuLabels();

	gSpideyDisplayMenu =
		menu;

	SpideyResetPendingDisplaySettings(
		"menu_open");
}

typedef void (__fastcall *SpideyRetailMenuGetEntryXYFn)(
		CMenu*,
		void*,
		const char*,
		int*,
		int*);
typedef void (__cdecl *SpideyDisplaySliderDrawFn)(
		int,
		int,
		int,
		int);
typedef int (__cdecl *SpideyDisplaySliderMouseFn)(
		int,
		int,
		int);

// @Ok
static int SpideyGetDisplayScaleSliderY(
		CMenu* menu,
		const char* label,
		int* y)
{
	if (!menu ||
		!label ||
		!y)
	{
		return 0;
	}

	int x =
		0;
	int entryY =
		0;
	SpideyRetailMenuGetEntryXYFn getEntryXY =
		(SpideyRetailMenuGetEntryXYFn)0x00440110;
	getEntryXY(
		menu,
		0,
		label,
		&x,
		&entryY);

	*y =
		entryY;
	return 1;
}

// @Ok
static void __fastcall SpideyDisplayMenuDisplay(
		CMenu* menu,
		void*)
{
	typedef void (__fastcall *RetailDisplayFn)(
			CMenu*,
			void*);

	RetailDisplayFn retailDisplay =
		(RetailDisplayFn)0x004401B0;
	retailDisplay(
		menu,
		0);

	if (!menu)
		return;

	SpideyDisplaySliderDrawFn drawSlider =
		(SpideyDisplaySliderDrawFn)0x00498060;

	const int sliderX =
		305;
	int y =
		0;

	if (SpideyGetDisplayScaleSliderY(
			menu,
			gSpideyGameplayUiScaleMenuLabel,
			&y))
	{
		drawSlider(
			sliderX,
			y,
			menu->mLine == 3 ? 1 : 0,
			SpideyUiScalePercentToSliderValue(
				gSpideyPendingGameplayUiScalePercent));
	}

	if (SpideyGetDisplayScaleSliderY(
			menu,
			gSpideyMenuTextScaleMenuLabel,
			&y))
	{
		drawSlider(
			sliderX,
			y,
			menu->mLine == 4 ? 1 : 0,
			SpideyUiScalePercentToSliderValue(
				gSpideyPendingMenuTextScalePercent));
	}
}

// @Ok
static int SpideyStepPendingUiScale(
		int row,
		int* percent,
		const char* kind,
		int delta)
{
	if (!percent ||
		!kind ||
		!delta)
	{
		return 0;
	}

	const int before =
		*percent;
	*percent =
		SpideyClampUiScalePercent(
			*percent +
			delta * kSpideyUiScaleStepPercent);

	if (*percent ==
		before)
	{
		return 0;
	}

	SpideyUpdateUiScaleMenuLabels();

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_pending_ui_scale kind=%s row=%d direction=%s percent=%d committed_gameplay=%d committed_text=%d\n",
			kind,
			row,
			delta > 0 ? "next" : "prev",
			*percent,
			gSpideyGameplayUiScalePercent,
			gSpideyMenuTextScalePercent);
		fclose(f);
	}

	return 1;
}

// @Ok
static int SpideyStepPendingCameraSensitivity(
		int row,
		int delta)
{
	if (!delta)
		return 0;

	const int before =
		gSpideyPendingCameraSensitivityPercent;
	gSpideyPendingCameraSensitivityPercent =
		SpideyClampCameraSensitivityPercent(
			gSpideyPendingCameraSensitivityPercent +
			delta *
				kSpideyCameraSensitivityStepPercent);

	if (gSpideyPendingCameraSensitivityPercent ==
		before)
	{
		return 0;
	}

	SpideyUpdateUiScaleMenuLabels();

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_pending_camera_sensitivity row=%d direction=%s percent=%d committed=%d\n",
			row,
			delta > 0 ? "next" : "prev",
			gSpideyPendingCameraSensitivityPercent,
			gSpideyCameraSensitivityPercent);
		fclose(f);
	}

	return 1;
}

// @Ok
static void __fastcall SpideyDisplayMenuUpdate(
		CMenu* menu,
		void*)
{
	typedef void (__fastcall *RetailUpdateFn)(
			CMenu*,
			void*);
	typedef u8 (__cdecl *CheckTriggersFn)(
			u32,
			i32,
			i32);

	RetailUpdateFn retailUpdate =
		(RetailUpdateFn)0x00440600;
	retailUpdate(
		menu,
		0);

	if (!menu)
		return;

	const int row =
		(int)menu->mLine;

	if (row != 3 &&
		row != 4 &&
		row != 5)
	{
		return;
	}

	CheckTriggersFn checkTriggers =
		(CheckTriggersFn)0x0050C180;

	int delta =
		0;
	if (checkTriggers(
			0x00008008,
			1,
			1))
	{
		delta =
			1;
	}
	else if (checkTriggers(
			0x00004004,
			1,
			1))
	{
		delta =
			-1;
	}

	if (row == 3 ||
		row == 4)
	{
		int y =
			0;
		const char* label =
			row == 3 ?
				gSpideyGameplayUiScaleMenuLabel :
				gSpideyMenuTextScaleMenuLabel;
		int* percent =
			row == 3 ?
				&gSpideyPendingGameplayUiScalePercent :
				&gSpideyPendingMenuTextScalePercent;
		const char* kind =
			row == 3 ?
				"gameplay_ui" :
				"menu_text";

		if (!delta &&
			SpideyGetDisplayScaleSliderY(
				menu,
				label,
				&y))
		{
			SpideyDisplaySliderMouseFn sliderMouse =
				(SpideyDisplaySliderMouseFn)0x00497F80;
			delta =
				sliderMouse(
					305,
					y,
					SpideyUiScalePercentToSliderValue(
						*percent));
		}

		if (delta)
		{
			SpideyStepPendingUiScale(
				row,
				percent,
				kind,
				delta);
		}
		return;
	}

	if (!delta)
		return;

	gSpideyPendingWindowMode +=
		delta;

	if (gSpideyPendingWindowMode >
			SPIDEY_WINDOW_WINDOWED)
	{
		gSpideyPendingWindowMode =
			SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE;
	}
	else if (gSpideyPendingWindowMode <
			 SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE)
	{
		gSpideyPendingWindowMode =
			SPIDEY_WINDOW_WINDOWED;
	}

	SpideyUpdateDisplayModeMenuLabel();

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_pending_window_mode direction=%s mode=%d label=%s committed=%s\n",
			delta > 0 ?
				"next" :
				"prev",
			gSpideyPendingWindowMode,
			gSpideyWindowModeLabels[
				gSpideyPendingWindowMode],
			gSpideyWindowModeLabels[
				gSpideyWindowMode]);
		fclose(f);
	}
}

// @Ok
static int SpideyPauseMenuHasEntry(
		CMenu* menu,
		const char* label)
{
	if (!menu ||
		!label)
	{
		return 0;
	}

	int entryIndex;
	for (entryIndex = 0;
		 entryIndex < (int)menu->mNumLines;
		 ++entryIndex)
	{
		if (menu->mEntry[entryIndex].name &&
			!strcmp(
				menu->mEntry[entryIndex].name,
				label))
		{
			return 1;
		}
	}

	return 0;
}

typedef void (__fastcall *SpideyRetailMenuSetLineFn)(
		CMenu*,
		void*,
		char);
typedef void (__fastcall *SpideyRetailMenuZoomFn)(
		CMenu*,
		void*,
		int);

// Rebuild the live expanding box from the CMenu's current rows instead of
// carrying a box sized for the previous menu shape. Retail CMenu::Zoom kills
// the old box and derives the replacement height from GetMenuHeight(), so
// future custom rows grow/shrink the container without another hard-coded
// height adjustment.
static int SpideyPauseRefreshMenuBox(
		CMenu* menu,
		const char* reason)
{
	if (!menu)
		return 0;

	const int zoomType =
		(int)menu->mZoomBoxType;

	if (zoomType < 0 ||
		zoomType > 2)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_menu_box_refresh reason=%s refreshed=0 mode=in_place reason_detail=zoom_type zoom_type=%d rows=%u y=%d line_sep=%d\n",
				reason ? reason : "unknown",
				zoomType,
				(unsigned int)menu->mNumLines,
				menu->mY,
				menu->mLineSep);
			fclose(f);
		}
		return 0;
	}

	CExpandingBox* box =
		menu->ptr_to;
	if (!box)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_menu_box_refresh reason=%s refreshed=0 mode=in_place reason_detail=no_box zoom_type=%d rows=%u y=%d line_sep=%d\n",
				reason ? reason : "unknown",
				zoomType,
				(unsigned int)menu->mNumLines,
				menu->mY,
				menu->mLineSep);
			fclose(f);
		}
		return 0;
	}

	int targetX =
		0;
	int targetY =
		0;
	int targetWidth =
		0;
	int targetHeight =
		0;
	const int menuHeight =
		menu->GetMenuHeight();

	if (zoomType == 0)
	{
		targetX =
			0;
		targetY =
			menu->mY - 18;
		targetWidth =
			512;
		targetHeight =
			menuHeight + 27;
	}
	else
	{
		int yInset =
			12;
		int heightInset =
			17;

		if (Utils_CompareStrings(
				Mess_GetCurrentFont(),
				"sp_fnt03.fnt"))
		{
			yInset =
				10;
			heightInset =
				14;
		}

		targetX =
			menu->mX - 5;
		targetY =
			menu->mY - yInset;
		targetWidth =
			(int)menu->menu_width + 12;
		targetHeight =
			menuHeight + heightInset;
	}

	int oldX =
		0;
	int oldY =
		0;
	int oldWidth =
		0;
	int oldHeight =
		0;
	int refreshed =
		0;
	int writeFault =
		0;

	// CMenu::Zoom kills and reallocates ptr_to. Doing that from the pause
	// update/confirm call chain can invalidate a heap-owned box while retail
	// code is still in the same menu frame. The box already stores both its
	// current and target rectangle, so update the target in place instead.
	// Clamp only shrinking current dimensions; growth can continue through
	// the retail expanding-box animation on subsequent frames.
	__try
	{
		oldX =
			box->field_1C;
		oldY =
			box->field_20;
		oldWidth =
			box->field_C;
		oldHeight =
			box->field_10;

		box->field_1C =
			targetX;
		box->field_20 =
			targetY;
		box->field_C =
			targetWidth;
		box->field_10 =
			targetHeight;

		if (box->field_4 >
			targetWidth)
		{
			box->field_4 =
				targetWidth;
		}

		if (box->field_8 >
			targetHeight)
		{
			box->field_8 =
				targetHeight;
		}

		refreshed =
			1;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		writeFault =
			1;
		refreshed =
			0;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_menu_box_refresh reason=%s refreshed=%d mode=in_place reason_detail=%s zoom_type=%d rows=%u y=%d line_sep=%d box=0x%08lX old_rect=%d,%d,%d,%d target_rect=%d,%d,%d,%d menu_height=%d\n",
			reason ? reason : "unknown",
			refreshed,
			writeFault ? "write_fault" : "ok",
			zoomType,
			(unsigned int)menu->mNumLines,
			menu->mY,
			menu->mLineSep,
			(unsigned long)box,
			oldX,
			oldY,
			oldWidth,
			oldHeight,
			targetX,
			targetY,
			targetWidth,
			targetHeight,
			menuHeight);
		fclose(f);
	}

	return refreshed;
}

// @Ok
static void SpideyPauseResetPendingToCommitted(
		const char* reason)
{
	gSpideyPendingGameplayUiScalePercent =
		gSpideyGameplayUiScalePercent;
	gSpideyPendingMenuTextScalePercent =
		gSpideyMenuTextScalePercent;
	gSpideyPendingCameraSensitivityPercent =
		gSpideyCameraSensitivityPercent;
	SpideyUpdateUiScaleMenuLabels();

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_options_pending_reset reason=%s gameplay=%d text=%d camera_sensitivity=%d\n",
			reason ? reason : "unknown",
			gSpideyPendingGameplayUiScalePercent,
			gSpideyPendingMenuTextScalePercent,
			gSpideyPendingCameraSensitivityPercent);
		fclose(f);
	}
}

// @Ok
static void SpideyPauseAbandonOptionsState(
		const char* reason)
{
	SpideyPauseResetPendingToCommitted(
		reason ? reason : "abandon");

	gSpideyPauseOptionsActive =
		0;
	gSpideyPauseParentSnapshotValid =
		0;
	gSpideyPauseMenuOwner =
		0;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_options_state action=abandon reason=%s\n",
			reason ? reason : "unknown");
		fclose(f);
	}
}

// @Ok
static int SpideyPauseEnterOptions(
		CMenu* menu)
{
	if (!menu ||
		gSpideyPauseOptionsActive)
	{
		return 0;
	}

	// Keep the parent menu's exact retail state so Back can restore it
	// without reconstructing or invoking the retail Options menu.
	memcpy(
		gSpideyPauseParentState,
		((unsigned char*)menu) + 8,
		sizeof(gSpideyPauseParentState));
	gSpideyPauseParentSnapshotValid =
		1;
	gSpideyPauseMenuOwner =
		menu;

	SpideyPauseResetPendingToCommitted(
		"enter_options");

	const int parentY =
		menu->mY;

	// Reuse the same live CMenu object. The expanding-box pointer at +4 is
	// not part of the parent snapshot because it owns live heap state. After
	// the row shape is rebuilt below, retail CMenu::Zoom recreates that box
	// from the current row count/spacing.
	menu->menu_width =
		0;
	menu->mCursorLine =
		0;
	menu->mNumLines =
		0;
	menu->field_32 =
		0;
	menu->field_1B =
		(unsigned char)-1;
	// Retail pause-menu presentation effectively consumes row 0 as the
	// submenu heading/first display slot. Keep an explicit disabled heading
	// there so both adjustable scale rows begin on normal selectable rows.
	menu->mY =
		parentY;

	int entryIndex;
	for (entryIndex = 0;
		 entryIndex < 6;
		 ++entryIndex)
	{
		menu->mEntry[entryIndex].what =
			0;
		menu->mEntry[entryIndex].unk_b =
			1;
		menu->mEntry[entryIndex].unk_a =
			0;
	}

	SpideyRetailMenuAddEntryFn retailAdd =
		(SpideyRetailMenuAddEntryFn)0x0043FFF0;
	retailAdd(
		menu,
		0,
		gSpideyPauseOptionsHeadingLabel);
	retailAdd(
		menu,
		0,
		gSpideyPauseGameplayUiScaleMenuLabel);
	retailAdd(
		menu,
		0,
		gSpideyPauseMenuTextScaleMenuLabel);
	retailAdd(
		menu,
		0,
		gSpideyPauseCameraSensitivityMenuLabel);
	retailAdd(
		menu,
		0,
		gSpideyPauseApplyUiScaleLabel);
	retailAdd(
		menu,
		0,
		gSpideyPauseBackLabel);

	// EntryEnable(false) in retail maps to what=1. Mark the heading directly
	// so up/down navigation skips it while it remains visible as a title.
	menu->mEntry[0].what =
		1;

	SpideyRetailMenuSetLineFn setLine =
		(SpideyRetailMenuSetLineFn)0x0043FF80;
	setLine(
		menu,
		0,
		1);

	SpideyPauseRefreshMenuBox(
		menu,
		"enter_options");

	gSpideyPauseOptionsActive =
		1;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_options_state action=enter menu=0x%08lX rows=%u line=%u cursor=%u y=%d line_sep=%d heading_disabled=%d gameplay=%d text=%d camera_sensitivity=%d\n",
			(unsigned long)menu,
			(unsigned int)menu->mNumLines,
			(unsigned int)menu->mLine,
			(unsigned int)menu->mCursorLine,
			menu->mY,
			menu->mLineSep,
			menu->mEntry[0].what ? 1 : 0,
			gSpideyPendingGameplayUiScalePercent,
			gSpideyPendingMenuTextScalePercent,
			gSpideyPendingCameraSensitivityPercent);
		fclose(f);
	}

	return 1;
}

// @Ok
static int SpideyPauseRestoreParent(
		CMenu* menu,
		const char* reason,
		int discardPending)
{
	if (!menu ||
		!gSpideyPauseParentSnapshotValid ||
		menu != gSpideyPauseMenuOwner)
	{
		SpideyPauseAbandonOptionsState(
			"restore_invalid_owner");
		return 0;
	}

	if (discardPending)
	{
		SpideyPauseResetPendingToCommitted(
			reason ? reason : "back");
	}

	memcpy(
		((unsigned char*)menu) + 8,
		gSpideyPauseParentState,
		sizeof(gSpideyPauseParentState));

	// The snapshot restores the parent rows/style but intentionally not the
	// live expanding-box pointer. Rebuild the box from the restored menu so
	// Back returns to the parent's exact dynamic height as well.
	SpideyPauseRefreshMenuBox(
		menu,
		reason ? reason : "restore_parent");

	gSpideyPauseOptionsActive =
		0;
	gSpideyPauseParentSnapshotValid =
		0;
	gSpideyPauseMenuOwner =
		menu;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_options_state action=restore reason=%s rows=%u line=%u gameplay=%d text=%d camera_sensitivity=%d\n",
			reason ? reason : "unknown",
			(unsigned int)menu->mNumLines,
			(unsigned int)menu->mLine,
			gSpideyGameplayUiScalePercent,
			gSpideyMenuTextScalePercent,
			gSpideyCameraSensitivityPercent);
		fclose(f);
	}

	return 1;
}

// @Ok
static void SpideyPauseCommitUiScale()
{
	const int oldGameplay =
		gSpideyGameplayUiScalePercent;
	const int oldText =
		gSpideyMenuTextScalePercent;
	const int oldCameraSensitivity =
		gSpideyCameraSensitivityPercent;

	gSpideyGameplayUiScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingGameplayUiScalePercent);
	gSpideyMenuTextScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingMenuTextScalePercent);
	gSpideyCameraSensitivityPercent =
		SpideyClampCameraSensitivityPercent(
			gSpideyPendingCameraSensitivityPercent);

	SpideySaveModernVideoSettings();
	SpideyUpdateUiScaleMenuLabels();
	SpideyApplyFrontendTextScale(
		"pause_options_apply");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_ui_apply old_gameplay=%d new_gameplay=%d old_text=%d new_text=%d old_camera_sensitivity=%d new_camera_sensitivity=%d saved=1 live_gameplay=next_draw live_text=reapplied_no_device_rebuild live_camera=next_input\n",
			oldGameplay,
			gSpideyGameplayUiScalePercent,
			oldText,
			gSpideyMenuTextScalePercent,
			oldCameraSensitivity,
			gSpideyCameraSensitivityPercent);
		fclose(f);
	}
}

// @Ok
static void __fastcall SpideyPauseMenuUpdate(
		CMenu* menu,
		void*)
{
	typedef void (__fastcall *RetailUpdateFn)(
			CMenu*,
			void*);
	typedef u8 (__cdecl *RetailCheckTriggersFn)(
			u32,
			i32,
			i32);

	if (!menu)
		return;

	// A pause close, level transition or retail menu rebuild can reuse the
	// same address with different rows. Never carry submenu state across it.
	if (gSpideyPauseOptionsActive)
	{
		const int ownerChanged =
			menu != gSpideyPauseMenuOwner;
		const int submenuShapeLost =
			!ownerChanged &&
			(menu->mNumLines != 6 ||
			 !SpideyPauseMenuHasEntry(
				menu,
				gSpideyPauseOptionsHeadingLabel) ||
			 !SpideyPauseMenuHasEntry(
				menu,
				gSpideyPauseGameplayUiScaleMenuLabel) ||
			 !SpideyPauseMenuHasEntry(
				menu,
				gSpideyPauseMenuTextScaleMenuLabel) ||
			 !SpideyPauseMenuHasEntry(
				menu,
				gSpideyPauseCameraSensitivityMenuLabel) ||
			 !SpideyPauseMenuHasEntry(
				menu,
				gSpideyPauseApplyUiScaleLabel) ||
			 !SpideyPauseMenuHasEntry(
				menu,
				gSpideyPauseBackLabel));

		if (ownerChanged ||
			submenuShapeLost)
		{
			SpideyPauseAbandonOptionsState(
				ownerChanged ?
					"menu_pointer_changed" :
					"menu_rebuilt");
		}
	}

	gSpideyPauseMenuOwner =
		menu;

	if (!gSpideyPauseOptionsActive &&
		!SpideyPauseMenuHasEntry(
			menu,
			gSpideyPauseOptionsLabel))
	{
		if (menu->mNumLines < 40)
		{
			SpideyRetailMenuAddEntryFn retailAdd =
				(SpideyRetailMenuAddEntryFn)0x0043FFF0;
			const unsigned int oldRows =
				(unsigned int)menu->mNumLines;
			const unsigned int oldLine =
				(unsigned int)menu->mLine;
			const char* oldLastLabel =
				oldRows ?
					menu->mEntry[oldRows - 1].name :
					0;

			retailAdd(
				menu,
				0,
				gSpideyPauseOptionsLabel);

			int optionsRow =
				(int)oldRows;
			int quitRow =
				-1;

			// Retail keeps Quit as the final pause row. AddEntry can only append,
			// so move our new Options entry one slot upward and keep the previous
			// final retail row at the bottom. Move the complete SEntry so all
			// colors/scales/flags travel with their original labels.
			if (oldRows > 0)
			{
				SEntry optionsEntry =
					menu->mEntry[oldRows];
				menu->mEntry[oldRows] =
					menu->mEntry[oldRows - 1];
				menu->mEntry[oldRows - 1] =
					optionsEntry;

				optionsRow =
					(int)oldRows - 1;
				quitRow =
					(int)oldRows;

				// If the player happened to be highlighting the old final row
				// while the pause menu was first decorated, follow that retail row
				// to its new bottom index instead of moving selection to Options.
				if (oldLine == oldRows - 1)
				{
					SpideyRetailMenuSetLineFn setLine =
						(SpideyRetailMenuSetLineFn)0x0043FF80;
					setLine(
						menu,
						0,
						(char)oldRows);
				}
			}

			// One added row: preserve approximately the retail visual center.
			menu->mY -=
				menu->mLineSep / 2;

			// The original pause box was authored before our Options row
			// existed. Rebuild it from the now-current parent row list so Quit
			// remains inside the container and later row additions scale too.
			SpideyPauseRefreshMenuBox(
				menu,
				"add_options_parent");

			FILE* f =
				SpideyOpenConsolidatedLog(
					"COMPAT");
			if (f)
			{
				fprintf(
					f,
					"pause_options_entry rows_added=1 rows=%u options_row=%d quit_row=%d previous_last=%s y=%d line_sep=%d parent_only=1 retail_options_invoked=0\n",
					(unsigned int)menu->mNumLines,
					optionsRow,
					quitRow,
					oldLastLabel ? oldLastLabel : "(none)",
					menu->mY,
					menu->mLineSep);
				fclose(f);
			}
		}
		else
		{
			FILE* f =
				SpideyOpenConsolidatedLog(
					"COMPAT");
			if (f)
			{
				fprintf(
					f,
					"pause_options_entry rows_added=0 reason=capacity rows=%u\n",
					(unsigned int)menu->mNumLines);
				fclose(f);
			}
		}
	}

	RetailUpdateFn retailUpdate =
		(RetailUpdateFn)0x00440600;
	retailUpdate(
		menu,
		0);

	if (!gSpideyPauseOptionsActive ||
		menu != gSpideyPauseMenuOwner ||
		menu->mLine >= menu->mNumLines ||
		!menu->mEntry[menu->mLine].name)
	{
		return;
	}

	const char* selected =
		menu->mEntry[menu->mLine].name;
	int* percent =
		0;
	const char* kind =
		0;

	if (!strcmp(
			selected,
			gSpideyPauseGameplayUiScaleMenuLabel))
	{
		percent =
			&gSpideyPendingGameplayUiScalePercent;
		kind =
			"gameplay_ui";
	}
	else if (!strcmp(
			selected,
			gSpideyPauseMenuTextScaleMenuLabel))
	{
		percent =
			&gSpideyPendingMenuTextScalePercent;
		kind =
			"menu_text";
	}
	else if (!strcmp(
			selected,
			gSpideyPauseCameraSensitivityMenuLabel))
	{
		percent =
			&gSpideyPendingCameraSensitivityPercent;
		kind =
			"camera_sensitivity";
	}
	else
	{
		return;
	}

	RetailCheckTriggersFn checkTriggers =
		(RetailCheckTriggersFn)0x0050C180;

	int delta =
		0;
	if (checkTriggers(
			0x00008008,
			1,
			1))
	{
		delta =
			1;
	}
	else if (checkTriggers(
			0x00004004,
			1,
			1))
	{
		delta =
			-1;
	}

	if (delta)
	{
		const int changed =
			percent ==
				&gSpideyPendingCameraSensitivityPercent ?
				SpideyStepPendingCameraSensitivity(
					(int)menu->mLine,
					delta) :
				SpideyStepPendingUiScale(
					(int)menu->mLine,
					percent,
					kind,
					delta);

		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_options_adjust kind=%s line=%u direction=%s changed=%d percent=%d pending_gameplay=%d pending_text=%d pending_camera_sensitivity=%d\n",
				kind,
				(unsigned int)menu->mLine,
				delta > 0 ? "next" : "prev",
				changed,
				*percent,
				gSpideyPendingGameplayUiScalePercent,
				gSpideyPendingMenuTextScalePercent,
				gSpideyPendingCameraSensitivityPercent);
			fclose(f);
		}
	}
}

// @Ok
static u8 __cdecl SpideyPauseConfirmTrigger(
		u32 mask,
		i32 option2,
		i32 option3)
{
	typedef u8 (__cdecl *RetailCheckTriggersFn)(
			u32,
			i32,
			i32);
	typedef u8 (__cdecl *RetailGetKeyStateFn)(
			u8);

	RetailCheckTriggersFn retail =
		(RetailCheckTriggersFn)0x0050C180;
	RetailGetKeyStateFn getKeyState =
		(RetailGetKeyStateFn)0x00501CB0;
	u8 triggered =
		retail(
			mask,
			option2,
			option3);
	int keyboardEnterTriggered =
		0;
	const int enterRawState =
		(int)getKeyState(
			0x1C);
	const int enterDown =
		(enterRawState & 0x7F) ?
			1 :
			0;

	keyboardEnterTriggered =
		enterDown &&
		!gSpideyPauseEnterHeld;
	gSpideyPauseEnterHeld =
		enterDown;

	CMenu* menu =
		gSpideyPauseMenuOwner;
	const char* selected =
		0;
	if (menu &&
		menu->mLine < menu->mNumLines &&
		menu->mEntry[menu->mLine].name)
	{
		selected =
			menu->mEntry[menu->mLine].name;
	}

	if (selected &&
		(gSpideyPauseOptionsActive ||
		 !strcmp(
			 selected,
			 gSpideyPauseOptionsLabel)) &&
		enterRawState !=
			gSpideyPauseLastEnterRawState)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_enter_state raw=0x%02X down=%d edge=%d held=%d line=%u selected=%s\n",
				(unsigned int)(enterRawState & 0xFF),
				enterDown,
				keyboardEnterTriggered,
				gSpideyPauseEnterHeld,
				(unsigned int)menu->mLine,
				selected);
			fclose(f);
		}
		gSpideyPauseLastEnterRawState =
			enterRawState;
	}
	else if (!selected ||
			 (!gSpideyPauseOptionsActive &&
			  strcmp(
				  selected,
				  gSpideyPauseOptionsLabel)))
	{
		gSpideyPauseLastEnterRawState =
			-1;
	}

	// The pause call patched at 0x00441606 services the mouse-oriented
	// trigger path (runtime-observed mask 0x00000100). Calling
	// PCSHELL_CheckTriggers again for keyboard Enter is too late here:
	// its mask-0x10 branch is gated by retail's global one-shot latch at
	// 0x00AC1238. Read the game's raw DirectInput key-state byte instead.
	// Retail stores 0xFF for a fresh press, 0x7F while still held after the
	// next poll, 0x80 for release, and 0x00 for idle. The low seven bits
	// therefore give a stable physical-down state even if another keyboard
	// poll already converted the fresh 0xFF edge into 0x7F. Our own latch
	// turns that down state back into one activation per press.
	if (!triggered &&
		keyboardEnterTriggered &&
		selected &&
		(gSpideyPauseOptionsActive ||
		 !strcmp(
			 selected,
			 gSpideyPauseOptionsLabel)))
	{
		triggered =
			1;
	}

	if (!triggered)
		return triggered;

	if (!menu ||
		!selected)
	{
		return triggered;
	}

	if (!gSpideyPauseOptionsActive &&
		!strcmp(
			selected,
			gSpideyPauseOptionsLabel))
	{
		const int entered =
			SpideyPauseEnterOptions(
				menu);

		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_options_confirm action=open entered=%d mask=0x%08lX source=%s\n",
				entered,
				(unsigned long)mask,
				keyboardEnterTriggered ?
					"raw_directinput_enter_edge" :
					"retail_call_mask");
			fclose(f);
		}
		return 0;
	}

	if (!gSpideyPauseOptionsActive)
		return triggered;

	if (!strcmp(
			selected,
			gSpideyPauseApplyUiScaleLabel))
	{
		SpideyPauseCommitUiScale();

		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_options_confirm action=apply line=%u rows=%u mask=0x%08lX source=%s gameplay=%d text=%d camera_sensitivity=%d\n",
				(unsigned int)menu->mLine,
				(unsigned int)menu->mNumLines,
				(unsigned long)mask,
				keyboardEnterTriggered ?
					"raw_directinput_enter_edge" :
					"retail_call_mask",
				gSpideyGameplayUiScalePercent,
				gSpideyMenuTextScalePercent,
				gSpideyCameraSensitivityPercent);
			fclose(f);
		}
		return 0;
	}

	if (!strcmp(
			selected,
			gSpideyPauseBackLabel))
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"pause_options_confirm action=back line=%u rows=%u mask=0x%08lX source=%s\n",
				(unsigned int)menu->mLine,
				(unsigned int)menu->mNumLines,
				(unsigned long)mask,
				keyboardEnterTriggered ?
					"raw_directinput_enter_edge" :
					"retail_call_mask");
			fclose(f);
		}

		SpideyPauseRestoreParent(
			menu,
			"back",
			1);
		return 0;
	}

	if (!strcmp(
			selected,
			gSpideyPauseGameplayUiScaleMenuLabel) ||
		!strcmp(
			selected,
			gSpideyPauseMenuTextScaleMenuLabel) ||
		!strcmp(
			selected,
			gSpideyPauseCameraSensitivityMenuLabel))
	{
		// Scale rows are adjusted only with left/right. Confirm is consumed
		// so retail never dispatches our custom labels as pause commands.
		return 0;
	}

	// While our submenu owns the CMenu, never allow an unexpected custom row
	// to fall through into the retail pause dispatcher.
	return 0;
}

// @Ok
static void __cdecl SpideyDisplayConfirmOrApply(
		u32,
		u32,
		u32,
		i32,
		i32);

static int SpideyPatchDirectCall(
		unsigned long callAddress,
		unsigned long expectedTarget,
		void* replacement,
		const char* name)
{
	unsigned char* call =
		(unsigned char*)callAddress;

	if (!call ||
		call[0] != 0xE8)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"display_menu_patch name=%s installed=0 reason=opcode address=0x%08lX\n",
				name ? name : "unknown",
				callAddress);
			fclose(f);
		}
		return 0;
	}

	long oldRel =
		*(long*)(call + 1);
	unsigned long oldTarget =
		(unsigned long)(call + 5 + oldRel);

	if (oldTarget != expectedTarget)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"display_menu_patch name=%s installed=0 reason=target address=0x%08lX expected=0x%08lX actual=0x%08lX\n",
				name ? name : "unknown",
				callAddress,
				expectedTarget,
				oldTarget);
			fclose(f);
		}
		return 0;
	}

	DWORD oldProtect =
		0;
	if (!VirtualProtect(
			call,
			5,
			PAGE_EXECUTE_READWRITE,
			&oldProtect))
	{
		return 0;
	}

	*(long*)(call + 1) =
		(long)(
			(unsigned char*)replacement -
			(call + 5));

	DWORD ignoredProtect =
		0;
	VirtualProtect(
		call,
		5,
		oldProtect,
		&ignoredProtect);

	FlushInstructionCache(
		GetCurrentProcess(),
		call,
		5);

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_menu_patch name=%s installed=1 address=0x%08lX retail=0x%08lX wrapper=0x%08lX\n",
			name ? name : "unknown",
			callAddress,
			expectedTarget,
			(unsigned long)replacement);
		fclose(f);
	}

	return 1;
}

static int SpideyPatchDirectCallsToTargetInRange(
		unsigned long startAddress,
		unsigned long endAddress,
		unsigned long expectedTarget,
		void* replacement,
		const char* name)
{
	if (!startAddress ||
		endAddress <= startAddress ||
		!expectedTarget ||
		!replacement)
	{
		return 0;
	}

	int installed =
		0;

	for (unsigned long address = startAddress;
		 address + 5 <= endAddress;
		 ++address)
	{
		unsigned char* call =
			(unsigned char*)address;
		if (call[0] != 0xE8)
			continue;

		const long rel =
			*(long*)(call + 1);
		const unsigned long target =
			(unsigned long)(
				call +
				5 +
				rel);
		if (target != expectedTarget)
			continue;

		if (SpideyPatchDirectCall(
				address,
				expectedTarget,
				replacement,
				name))
		{
			++installed;
			address +=
				4;
		}
	}

	return installed;
}

static int SpideyPatchBytes(
		unsigned long address,
		const unsigned char* expected,
		const unsigned char* replacement,
		unsigned long size,
		const char* name)
{
	unsigned char* target =
		(unsigned char*)address;

	if (!target ||
		!expected ||
		!replacement ||
		!size)
	{
		return 0;
	}

	if (memcmp(
			target,
			expected,
			size) != 0)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"byte_patch name=%s installed=0 reason=expected_bytes address=0x%08lX size=%lu actual=",
				name ? name : "unknown",
				address,
				size);
			for (unsigned long i = 0;
				 i < size;
				 ++i)
			{
				fprintf(
					f,
					"%02X",
					(unsigned int)target[i]);
			}
			fputc(
				'\n',
				f);
			fclose(f);
		}
		return 0;
	}

	DWORD oldProtect =
		0;
	if (!VirtualProtect(
			target,
			size,
			PAGE_EXECUTE_READWRITE,
			&oldProtect))
	{
		return 0;
	}

	memcpy(
		target,
		replacement,
		size);

	DWORD ignoredProtect =
		0;
	VirtualProtect(
		target,
		size,
		oldProtect,
		&ignoredProtect);

	FlushInstructionCache(
		GetCurrentProcess(),
		target,
		size);

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
				"byte_patch name=%s installed=1 address=0x%08lX size=%lu\n",
				name ? name : "unknown",
				address,
				size);
		fclose(f);
	}

	return 1;
}

static unsigned long gSpideyZipSpecialMoveHalfSteps = 0;
static unsigned long gSpideyZipSpecialMoveLogs = 0;

static CVector* __fastcall SpideyZipSpecialMoveAdd60(
		CVector* position,
		void*,
		const CVector& velocity)
{
	if (!position)
		return position;

	CPlayer* player =
		reinterpret_cast<CPlayer*>(
			reinterpret_cast<unsigned char*>(position) -
			8);

	if (player &&
		player->field_E1C ==
			0x40000 &&
		player->field_80 ==
			1 &&
		(player->mAnim == 270 ||
		 player->mAnim == 271))
	{
		const long beforeX =
			position->vx;
		const long beforeY =
			position->vy;
		const long beforeZ =
			position->vz;

		position->vx +=
			velocity.vx >> 1;
		position->vy +=
			velocity.vy >> 1;
		position->vz +=
			velocity.vz >> 1;
		++gSpideyZipSpecialMoveHalfSteps;

		if (gSpideyZipSpecialMoveLogs < 64)
		{
			FILE* f =
				SpideyOpenConsolidatedLog(
					"TIMING");
			if (f)
			{
				fprintf(
					f,
					"web_zip_move_halfstep sample=%lu tick=%ld anim=%u frame=%d field80=%ld before=%ld,%ld,%ld velocity=%ld,%ld,%ld after=%ld,%ld,%ld\n",
					gSpideyZipSpecialMoveHalfSteps,
					(long)*(volatile long*)0x006B4CA8,
					(unsigned int)player->mAnim,
					(int)player->mFrame,
					(long)player->field_80,
					beforeX,
					beforeY,
					beforeZ,
					(long)velocity.vx,
					(long)velocity.vy,
					(long)velocity.vz,
					(long)position->vx,
					(long)position->vy,
					(long)position->vz);
				fclose(f);
			}
			++gSpideyZipSpecialMoveLogs;
		}

		return position;
	}

	typedef CVector* (__fastcall *SpideyRetailVectorAddFn)(
		CVector*,
		void*,
		const CVector&);
	SpideyRetailVectorAddFn retail =
		(SpideyRetailVectorAddFn)0x004E7590;

	return retail(
		position,
		0,
		velocity);
}

// Native-60 player compatibility without replacing retail player physics.
//
// Retail authored its movement quantum around the common field_80 == 2 case.
// The master timer now dispatches one canonical update every 16/17 ms, so
// field_80 == 1 must represent half of that old movement quantum. The force
// hook below replaces only the two friction CALL sites. Four threshold byte
// changes make the generic retail elapsed-vblank math handle a one-tick update
// through its general path while leaving the zero-tick retail shortcut intact.
//
// Web-zip's special no-collision branch is different: retail first adds the
// whole velocity, then returns for field_80 <= 2. Sending field_80==1 through
// its catch-up path cancels that displacement completely. A dedicated call
// hook therefore halves only that initial zip vector add and leaves retail's
// original comparison/return structure untouched.
//
// Collision, grounding, landing, platform and cutscene code remains retail.
//
// Each affected retail branch is:
//
//     cmp field_80, 2
//     jle direct_path
//
// Change only the immediate threshold from 2 to 0. field_80 is asserted
// non-negative by CBody::EveryFrame, so:
//   0 -> original direct path;
//   1 -> general expression, producing the native-60 half-step;
//   2 -> general expression with a zero extra term, reproducing the direct
//        retail value;
//   >2 -> the original catch-up expression, unchanged.
//
// The movement expression is:
//
//     v + (v >> 1) * (field_80 - 2)
//
// which becomes v - (v >> 1) for field_80 == 1. This differs from a plain
// arithmetic v >> 1 by at most one fixed-point unit for odd values.
static void SpideyInstallPlayerPhysics60Compat()
{
	const int normalFrictionInstalled =
		SpideyPatchDirectCall(
			0x00466D84,
			0x004E76B0,
			(void*)&SpideyPhysicsFriction60,
			"timing_player_friction_normal");
	const int crawlFrictionInstalled =
		SpideyPatchDirectCall(
			0x0046801D,
			0x004E76B0,
			(void*)&SpideyPhysicsFriction60,
			"timing_player_friction_crawl");

	const unsigned char twoTickThreshold[] = { 0x02 };
	const unsigned char zeroTickThreshold[] = { 0x00 };

	// Special no-collision animation displacement used by web-zip.
	// Retail first does mPos += mVel and then returns immediately when
	// field_80 <= 2. The old 2->0 threshold patch was wrong: at field_80==1
	// it fell through into the catch-up term with (field_80-2)==-1, which
	// subtracted the same velocity and produced zero net movement.
	//
	// Keep retail's original cmp field_80,2 / JLE intact. Replace only the
	// initial vector-add call so a native-60 one-tick update consumes half of
	// the authored retail velocity; all other elapsed-tick values use the
	// original operator+= unchanged.
	const int specialMoveInstalled =
		SpideyPatchDirectCall(
			0x00466DBC,
			0x004E7590,
			(void*)&SpideyZipSpecialMoveAdd60,
			"timing_player_zip_special_move_halfstep");

	// Main normal-physics displacement.
	const int normalMoveInstalled =
		SpideyPatchBytes(
			0x00466E22,
			twoTickThreshold,
			zeroTickThreshold,
			sizeof(twoTickThreshold),
			"timing_player_move_halfstep");

	// After collision, the retail general path rescales the per-tick move
	// back into velocity. For field_80 == 1 this is (move << 1) / 1.
	const int normalVelocityInstalled =
		SpideyPatchBytes(
			0x00467592,
			twoTickThreshold,
			zeroTickThreshold,
			sizeof(twoTickThreshold),
			"timing_player_velocity_restore_halfstep");

	// Vertical fall displacement uses the same authored elapsed-vblank form.
	const int normalFallInstalled =
		SpideyPatchBytes(
			0x004677ED,
			twoTickThreshold,
			zeroTickThreshold,
			sizeof(twoTickThreshold),
			"timing_player_fall_halfstep");

	// Crawling uses the same field_80 <= 2/full-displacement shortcut.
	const int crawlMoveInstalled =
		SpideyPatchBytes(
			0x00468056,
			twoTickThreshold,
			zeroTickThreshold,
			sizeof(twoTickThreshold),
			"timing_player_crawl_move_halfstep");

	FILE* f = SpideyOpenConsolidatedLog("TIMING");
	if (f)
	{
		fprintf(
			f,
			"player_physics_60_install normal_friction=%d crawl_friction=%d special_move=%d normal_move=%d velocity_restore=%d fall=%d crawl_move=%d retail_friction=0x%08lX policy=retail_collision_halfstep_force_displacement zip_special=call_0x00466DBC_half_velocity_when_field80_1 generic_thresholds=2_to_0 rounding=general_retail_path_max_1_fixed_unit\n",
			normalFrictionInstalled,
			crawlFrictionInstalled,
			specialMoveInstalled,
			normalMoveInstalled,
			normalVelocityInstalled,
			normalFallInstalled,
			crawlMoveInstalled,
			0x004E76B0UL);
		fclose(f);
	}
}

// High-FPS compatibility: retail Mysterio laser liveness is a one-update
// handshake. CMysterioLaser::SetPos writes byte +0x44 = 1, while virtual Move
// at 0x0045BAC0 kills the bit if that byte was not refreshed since the
// immediately previous Move and then clears it. That makes the beam depend on
// producer/consumer call cadence instead of elapsed time.
//
// Reuse the same byte without changing the retail object layout:
//   0   = never/refreshed too long ago
//   1   = fresh marker written by retail SetPos
//   2..255 = encoded gTimerRelated tick modulo 254
// A fresh marker is converted to a timestamp on the next Move. The beam may
// then survive for three canonical 60-Hz ticks (50 ms), matching the minimum
// 20-Hz authored cadence documented for the problematic Mysterio sequence.
// If SetPos stops refreshing it, the fourth elapsed tick kills it normally.
static const unsigned long kSpideyMysterioLaserTickModulo = 254;
static const unsigned long kSpideyMysterioLaserGraceTicks = 3;

typedef void (__fastcall *SpideyRetailCBitDieFn)(
		void*,
		void*);

static void __fastcall SpideyMysterioLaserMoveHighFps(
		void* laser,
		void*)
{
	if (!laser)
		return;

	unsigned char* refresh =
		(unsigned char*)laser +
			0x44;
	const unsigned long now =
		(unsigned long)*(volatile long*)0x006B4CA8;

	if (*refresh == 1)
	{
		*refresh =
			(unsigned char)(
				(now %
					kSpideyMysterioLaserTickModulo) +
				2);
		return;
	}

	if (*refresh >= 2)
	{
		const unsigned long stored =
			(unsigned long)(*refresh - 2);
		const unsigned long current =
			now %
				kSpideyMysterioLaserTickModulo;
		const unsigned long elapsed =
			(current +
				kSpideyMysterioLaserTickModulo -
				stored) %
			kSpideyMysterioLaserTickModulo;

		if (elapsed <=
			kSpideyMysterioLaserGraceTicks)
		{
			return;
		}
	}

	*refresh =
		0;

	SpideyRetailCBitDieFn die =
		(SpideyRetailCBitDieFn)0x00408930;
	die(
		laser,
		0);
}

static int SpideyIsMysterioBossActive();

typedef int (__fastcall *SpideyRetailSoftSpotHitFn)(
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
			(int)*(volatile unsigned char*)(
				(unsigned char*)player +
				0x8F8);

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
			"mysterio_softspot_hit call=%lu spot=0x%08lX part=%d flags=0x%02X destructive_bit=%d damage=%u hp=%d->%d player_web_mode=%d result=%d policy=telemetry_only_retail_hit_unchanged\n",
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
			"mysterio_softspot_hit_install installed=%d vtable=0x0053BB88 slot=0x0053BB94 retail=0x0045F940 wrapper=0x%08lX retail_damage_gate=SHitInfo.field_0_bit_0x04\n",
			gSpideySoftSpotHitInstalled,
			replacement);
		fclose(log);
	}

	return gSpideySoftSpotHitInstalled;
}

typedef void (__fastcall *SpideyRetailMysterioFireBoobiesFn)(
		CMysterio*,
		void*);

typedef int (__fastcall *SpideyRetailMysterioLaserSetPosFn)(
		CMysterioLaser*,
		void*,
		const CVector*,
		const CSVector*);

typedef int (__fastcall *SpideyRetailYawTowardsFn)(
		CBaddy*,
		void*,
		int,
		int);

static unsigned long gSpideyMysterioYawTowardsCalls =
	0;
static unsigned long gSpideyMysterioYawTowardsRetailCalls =
	0;
static unsigned long gSpideyMysterioYawTowardsHeldCalls =
	0;
static unsigned long gSpideyMysterioYawTowardsPhaseResets =
	0;
static unsigned long gSpideyMysterioYawTowardsPhase =
	0;
static unsigned long gSpideyMysterioYawTowardsLastTick =
	0;
static CBaddy* gSpideyMysterioYawTowardsBoss =
	0;
static int gSpideyMysterioYawTowardsPhaseValid =
	0;
static int gSpideyMysterioYawTowardsInstalled =
	0;

static int SpideyGetWrappedYawError(
		CBaddy* baddy,
		int targetYaw)
{
	if (!baddy)
		return 0;

	const int currentYaw =
		(int)*(volatile short*)(
			(unsigned char*)baddy +
			0x16);
	int error =
		targetYaw -
		currentYaw;

	if (error <
		-0x800)
	{
		error +=
			0x1000;
	}
	else if (error >
		0x800)
	{
		error -=
			0x1000;
	}

	return error;
}

// FireBoobies creates a CAIProc_LookAt whose retail Execute calls
// CBaddy::YawTowards once per AI update. YawTowards applies its full
// proportional turn step per call and does not scale that step by field_80.
// The true 20-FPS Mysterio reference therefore advances this controller once
// per 50 ms, while native 60 Hz otherwise converges about three times as
// often. Keep LookAt itself alive at 60 Hz, but sample only its Mysterio
// state-6 turn step on one canonical phase out of three.
static int __fastcall SpideyMysterioYawTowards20Hz(
		CBaddy* baddy,
		void*,
		int targetYaw,
		int turnFactor)
{
	SpideyRetailYawTowardsFn retail =
		(SpideyRetailYawTowardsFn)0x004030C0;

	++gSpideyMysterioYawTowardsCalls;

	if (!baddy)
	{
		++gSpideyMysterioYawTowardsRetailCalls;
		return retail(
			baddy,
			0,
			targetYaw,
			turnFactor);
	}

	int itemType =
		0;
	CBaddy* boss =
		0;
	int state =
		-1;

	__try
	{
		itemType =
			*(volatile int*)0x0060F654;
		boss =
			*(CBaddy* volatile*)0x0060F788;
		state =
			*(volatile int*)(
				(unsigned char*)baddy +
				0x31C);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		itemType =
			0;
		boss =
			0;
		state =
			-1;
	}

	if (itemType !=
			311 ||
		boss !=
			baddy ||
		state !=
			6)
	{
		if (gSpideyMysterioYawTowardsBoss ==
			baddy)
		{
			gSpideyMysterioYawTowardsPhaseValid =
				0;
			gSpideyMysterioYawTowardsBoss =
				0;
		}

		++gSpideyMysterioYawTowardsRetailCalls;
		return retail(
			baddy,
			0,
			targetYaw,
			turnFactor);
	}

	const unsigned long now =
		(unsigned long)*(volatile long*)0x006B4CA8;
	int resetPhase =
		!gSpideyMysterioYawTowardsPhaseValid ||
		gSpideyMysterioYawTowardsBoss !=
			baddy;

	if (!resetPhase)
	{
		const unsigned long elapsed =
			now -
			gSpideyMysterioYawTowardsLastTick;

		if (now <
				gSpideyMysterioYawTowardsLastTick ||
			elapsed >
				3)
		{
			resetPhase =
				1;
		}
	}

	if (resetPhase)
	{
		gSpideyMysterioYawTowardsPhase =
			now %
			3;
		gSpideyMysterioYawTowardsBoss =
			baddy;
		gSpideyMysterioYawTowardsPhaseValid =
			1;
		++gSpideyMysterioYawTowardsPhaseResets;
	}

	gSpideyMysterioYawTowardsLastTick =
		now;

	if ((now %
			3) ==
		gSpideyMysterioYawTowardsPhase)
	{
		++gSpideyMysterioYawTowardsRetailCalls;
		return retail(
			baddy,
			0,
			targetYaw,
			turnFactor);
	}

	++gSpideyMysterioYawTowardsHeldCalls;
	return SpideyGetWrappedYawError(
		baddy,
		targetYaw);
}

static void SpideyLogMysterioYawTowardsStats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"mysterio_yawtowards_20hz_stats installed=%d calls=%lu retail_calls=%lu held_calls=%lu phase_resets=%lu callsite=0x00401528 retail=0x004030C0 policy=lookat_alive_60hz_yaw_step_20hz_fireboobies_state6\n",
		gSpideyMysterioYawTowardsInstalled,
		gSpideyMysterioYawTowardsCalls,
		gSpideyMysterioYawTowardsRetailCalls,
		gSpideyMysterioYawTowardsHeldCalls,
		gSpideyMysterioYawTowardsPhaseResets);
	fclose(f);
}

struct SpideyMysterioLaserSetPosGate
{
	CMysterioLaser* laser;
	long lastTick;
	int valid;
	long visualX;
	long visualY;
	long visualZ;
	int visualValid;
};

static SpideyMysterioLaserSetPosGate gSpideyMysterioLaserSetPosGate[2] =
{
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }
};

static unsigned long gSpideyMysterioLaserSetPosCalls =
	0;
static unsigned long gSpideyMysterioLaserSetPosRetailCalls =
	0;
static unsigned long gSpideyMysterioLaserSetPosHeldCalls =
	0;
static unsigned long gSpideyMysterioLaserSetPosMaxElapsed =
	0;
static unsigned long gSpideyMysterioLaserVisualFollowCalls =
	0;
static unsigned long gSpideyMysterioLaserVisualFollowPoints =
	0;
static int gSpideyMysterioLaserSetPosInstalled =
	0;

static SpideyMysterioLaserSetPosGate*
SpideyGetMysterioLaserSetPosGate(
		CMysterioLaser* laser)
{
	for (int i = 0; i < 2; ++i)
	{
		if (gSpideyMysterioLaserSetPosGate[i].laser ==
			laser)
		{
			return &gSpideyMysterioLaserSetPosGate[i];
		}
	}

	for (int freeIndex = 0; freeIndex < 2; ++freeIndex)
	{
		if (!gSpideyMysterioLaserSetPosGate[freeIndex].laser)
		{
			gSpideyMysterioLaserSetPosGate[freeIndex].laser =
				laser;
			gSpideyMysterioLaserSetPosGate[freeIndex].lastTick =
				0;
			gSpideyMysterioLaserSetPosGate[freeIndex].valid =
				0;
			gSpideyMysterioLaserSetPosGate[freeIndex].visualValid =
				0;
			return &gSpideyMysterioLaserSetPosGate[freeIndex];
		}
	}

	// Mysterio owns at most two concurrent beam objects. If a pointer is
	// replaced between attacks, recycle the oldest slot deterministically.
	int slot =
		gSpideyMysterioLaserSetPosGate[0].lastTick <=
			gSpideyMysterioLaserSetPosGate[1].lastTick ?
			0 :
			1;
	gSpideyMysterioLaserSetPosGate[slot].laser =
		laser;
	gSpideyMysterioLaserSetPosGate[slot].lastTick =
		0;
	gSpideyMysterioLaserSetPosGate[slot].valid =
		0;
	gSpideyMysterioLaserSetPosGate[slot].visualValid =
		0;
	return &gSpideyMysterioLaserSetPosGate[slot];
}


static void SpideyRememberMysterioLaserVisualPosition(
		SpideyMysterioLaserSetPosGate* gate,
		const CVector* position)
{
	if (!gate || !position)
		return;

	gate->visualX = position->vx;
	gate->visualY = position->vy;
	gate->visualZ = position->vz;
	gate->visualValid = 1;
}

static void SpideyFollowMysterioLaserEmitter60Hz(
		CMysterioLaser* laser,
		SpideyMysterioLaserSetPosGate* gate,
		const CVector* position)
{
	if (!laser ||
		!gate ||
		!position ||
		!gate->visualValid)
	{
		SpideyRememberMysterioLaserVisualPosition(
			gate,
			position);
		return;
	}

	const long deltaX =
		position->vx - gate->visualX;
	const long deltaY =
		position->vy - gate->visualY;
	const long deltaZ =
		position->vz - gate->visualZ;

	if (!deltaX &&
		!deltaY &&
		!deltaZ)
	{
		SpideyRememberMysterioLaserVisualPosition(
			gate,
			position);
		return;
	}

	CGouraudRibbon** ribbons =
		(CGouraudRibbon**)((unsigned char*)laser + 0x3C);
	unsigned long translatedPoints = 0;

	for (int ribbonIndex = 0;
		ribbonIndex < 2;
		++ribbonIndex)
	{
		CGouraudRibbon* ribbon =
			ribbons[ribbonIndex];
		if (!ribbon ||
			ribbon->mNumPoints < 2 ||
			ribbon->mNumPoints > 32 ||
			!ribbon->mpPoints)
		{
			continue;
		}

		const int denominator =
			ribbon->mNumPoints - 1;

		for (int pointIndex = 0;
			pointIndex < ribbon->mNumPoints;
			++pointIndex)
		{
			const int weight =
				denominator - pointIndex;

			ribbon->mpPoints[pointIndex].Pos.vx +=
				(deltaX * weight) / denominator;
			ribbon->mpPoints[pointIndex].Pos.vy +=
				(deltaY * weight) / denominator;
			ribbon->mpPoints[pointIndex].Pos.vz +=
				(deltaZ * weight) / denominator;
			++translatedPoints;
		}
	}

	if (translatedPoints)
	{
		++gSpideyMysterioLaserVisualFollowCalls;
		gSpideyMysterioLaserVisualFollowPoints +=
			translatedPoints;

		if (gSpideyMysterioLaserVisualFollowCalls <= 32)
		{
			FILE* log =
				SpideyOpenConsolidatedLog(
					"TIMING");
			if (log)
			{
				fprintf(
					log,
					"mysterio_laser_visual_follow call=%lu delta=%ld,%ld,%ld points=%lu policy=20hz_sim_60hz_emitter_follow\n",
					gSpideyMysterioLaserVisualFollowCalls,
					deltaX,
					deltaY,
					deltaZ,
					translatedPoints);
				fclose(log);
			}
		}
	}

	SpideyRememberMysterioLaserVisualPosition(
		gate,
		position);
}

static int __fastcall SpideyMysterioLaserSetPos20Hz(
		CMysterioLaser* laser,
		void*,
		const CVector* position,
		const CSVector* rotation)
{
	SpideyRetailMysterioLaserSetPosFn retail =
		(SpideyRetailMysterioLaserSetPosFn)0x0045B5E0;

	++gSpideyMysterioLaserSetPosCalls;

	if (!laser ||
		!SpideyIsMysterioBossActive())
	{
		return retail(
			laser,
			0,
			position,
			rotation);
	}

	SpideyMysterioLaserSetPosGate* gate =
		SpideyGetMysterioLaserSetPosGate(
			laser);
	const long now =
		*(volatile long*)0x006B4CA8;

	if (!gate->valid)
	{
		gate->lastTick =
			now;
		gate->valid =
			1;

		++gSpideyMysterioLaserSetPosRetailCalls;
		const int result = retail(
			laser,
			0,
			position,
			rotation);
		SpideyRememberMysterioLaserVisualPosition(
			gate,
			position);
		return result;
	}

	int elapsed =
		(int)(
			now -
			gate->lastTick);
	if (elapsed < 0 ||
		elapsed > 30)
	{
		// New attack / wrap / stale slot: resynchronize immediately.
		elapsed =
			3;
	}

	if (elapsed <
		3)
	{
		++gSpideyMysterioLaserSetPosHeldCalls;
		// Keep authored 20-Hz beam simulation/collision, but move the already
		// built ribbon geometry with the animated chest emitter every 60-Hz frame.
		SpideyFollowMysterioLaserEmitter60Hz(
			laser,
			gate,
			position);
		return 0;
	}

	gate->lastTick =
		now;

	++gSpideyMysterioLaserSetPosRetailCalls;
	if ((unsigned long)elapsed >
		gSpideyMysterioLaserSetPosMaxElapsed)
	{
		gSpideyMysterioLaserSetPosMaxElapsed =
			(unsigned long)elapsed;
	}

	const int result = retail(
		laser,
		0,
		position,
		rotation);
	SpideyRememberMysterioLaserVisualPosition(
		gate,
		position);
	return result;
}

static void SpideyLogMysterioLaserSetPosStats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"mysterio_laser_setpos_20hz_stats installed=%d calls=%lu retail_calls=%lu held_calls=%lu max_elapsed=%lu visual_follow_calls=%lu visual_follow_points=%lu callsites=0x0045D3AB,0x0045D44E retail=0x0045B5E0 policy=20hz_sim_60hz_emitter_follow_fireboobies_ai_60hz\\n",
		gSpideyMysterioLaserSetPosInstalled,
		gSpideyMysterioLaserSetPosCalls,
		gSpideyMysterioLaserSetPosRetailCalls,
		gSpideyMysterioLaserSetPosHeldCalls,
		gSpideyMysterioLaserSetPosMaxElapsed,
		gSpideyMysterioLaserVisualFollowCalls,
		gSpideyMysterioLaserVisualFollowPoints);
	fclose(f);
}

// @Ok
// FireBoobies telemetry remains pass-through. The state machine itself must run
// every 60-Hz Logic update; only the beam SetPos sampling is authored-cadence.
// CMysterio::AI itself remains native 60 Hz so damage/state/object upkeep is
// never starved.
static unsigned long gSpideyMysterioFireBoobiesCalls =
	0;

static void __fastcall SpideyMysterioFireBoobiesTelemetry(
		CMysterio* mysterio,
		void*)
{
	SpideyRetailMysterioFireBoobiesFn retail =
		(SpideyRetailMysterioFireBoobiesFn)0x0045D200;

	++gSpideyMysterioFireBoobiesCalls;

	const unsigned long now =
		(unsigned long)*(volatile long*)0x006B4CA8;
	const int stateBefore =
		mysterio ?
			(int)mysterio->field_31C.bothFlags :
			-1;
	const int substateBefore =
		mysterio ?
			mysterio->dumbAssPad :
			-1;
	const int elapsed =
		mysterio ?
			(int)mysterio->field_80 :
			0;

	retail(
		mysterio,
		0);

	const int stateAfter =
		mysterio ?
			(int)mysterio->field_31C.bothFlags :
			-1;
	const int substateAfter =
		mysterio ?
			mysterio->dumbAssPad :
			-1;

	if (gSpideyMysterioFireBoobiesCalls <=
		512)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (log)
		{
			fprintf(
				log,
				"mysterio_laser_attack event=retail_passthrough call=%lu tick=%lu state=%d->%d substate=%d->%d field80=%d boss_active=%d policy=fireboobies_60hz_setpos_20hz\\n",
				gSpideyMysterioFireBoobiesCalls,
				now,
				stateBefore,
				stateAfter,
				substateBefore,
				substateAfter,
				elapsed,
				SpideyIsMysterioBossActive());
			fclose(log);
		}
	}
}

static void SpideyLogHighFpsRetailBytes(
		const char* label,
		unsigned long address,
		unsigned long size)
{
	if (!label ||
		!address ||
		!size)
	{
		return;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	int valid =
		1;
	const unsigned long chunkSize =
		128;

	__try
	{
		const unsigned char* bytes =
			(const unsigned char*)address;

		for (unsigned long offset = 0;
			 offset < size;
			 offset += chunkSize)
		{
			unsigned long count =
				size - offset;
			if (count >
				chunkSize)
			{
				count =
					chunkSize;
			}

			fprintf(
				f,
				"high_fps_re_bytes label=%s address=0x%08lX size=%lu offset=0x%04lX count=%lu hex=",
				label,
				address,
				size,
				offset,
				count);

			for (unsigned long i = 0;
				 i < count;
				 ++i)
			{
				fprintf(
					f,
					"%02X",
					(unsigned int)bytes[
						offset +
						i]);
			}

			fputc(
				'\n',
				f);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		valid =
			0;
	}

	fprintf(
		f,
		"high_fps_re_bytes_done label=%s address=0x%08lX size=%lu valid=%d\n",
		label,
		address,
		size,
		valid);
	fclose(f);
}

static void SpideyLogRetailFieldXrefs(
		const char* label,
		unsigned long start,
		unsigned long size,
		unsigned long fieldOffset)
{
	if (!label ||
		!start ||
		size < 4)
	{
		return;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	unsigned long matches =
		0;
	int valid =
		1;

	__try
	{
		const unsigned char* bytes =
			(const unsigned char*)start;
		const unsigned char pattern[4] =
		{
			(unsigned char)(fieldOffset & 0xFF),
			(unsigned char)((fieldOffset >> 8) & 0xFF),
			(unsigned char)((fieldOffset >> 16) & 0xFF),
			(unsigned char)((fieldOffset >> 24) & 0xFF)
		};

		for (unsigned long i = 0;
			 i + 4 <= size;
			 ++i)
		{
			if (bytes[i] != pattern[0] ||
				bytes[i + 1] != pattern[1] ||
				bytes[i + 2] != pattern[2] ||
				bytes[i + 3] != pattern[3])
			{
				continue;
			}

			++matches;
			const unsigned long before =
				i < 64 ? i : 64;
			unsigned long after =
				size - i;
			if (after > 128)
				after = 128;
			const unsigned long contextStart =
				i - before;
			const unsigned long contextSize =
				before + after;

			fprintf(
				f,
				"high_fps_field_xref label=%s field=0x%04lX match=%lu address=0x%08lX context_start=0x%08lX context_size=%lu hex=",
				label,
				fieldOffset,
				matches,
				start + i,
				start + contextStart,
				contextSize);

			for (unsigned long j = 0;
				 j < contextSize;
				 ++j)
			{
				fprintf(
					f,
					"%02X",
					(unsigned int)bytes[
						contextStart + j]);
			}
			fputc('\n', f);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		valid =
			0;
	}

	fprintf(
		f,
		"high_fps_field_xref_done label=%s field=0x%04lX range=0x%08lX+0x%lX matches=%lu valid=%d\n",
		label,
		fieldOffset,
		start,
		size,
		matches,
		valid);
	fclose(f);
}

typedef int (__cdecl *SpideyRetailTrigGetLevelIdFn)();

static int SpideyRetailGetLevelId()
{
	SpideyRetailTrigGetLevelIdFn fn =
		(SpideyRetailTrigGetLevelIdFn)0x004DE770;
	return fn();
}

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

// Mysterio laser-attack cadence compatibility.
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



struct SpideyChaseSchedulerStats
{
	unsigned long logicCalls;
	unsigned long logicDelta0;
	unsigned long logicDelta1;
	unsigned long logicDelta2;
	unsigned long logicDelta3Plus;
	unsigned long logicUpdaterActive;
	unsigned long presentCalls;
	unsigned long presentDelta0;
	unsigned long presentDelta1;
	unsigned long presentDelta2;
	unsigned long presentDelta3Plus;
	unsigned long presentUpdaterActive;
	unsigned long presentDelta2Run;
	unsigned long presentDelta2MaxRun;
	long lastLogicVblank;
	long lastPresentVblank;
	int logicVblankValid;
	int presentVblankValid;
};

static SpideyChaseSchedulerStats gSpideyChaseSchedulerStats;

static int SpideyRetailFrameUpdaterActive()
{
	return
		*(volatile long*)0x005FAE98 != 0 ||
		*(volatile long*)0x0060CFB0 != 0;
}

static void SpideyRecordChaseLogicScheduler()
{
	if (SpideyRetailGetLevelId() != 0x501)
		return;

	SpideyChaseSchedulerStats* stats =
		&gSpideyChaseSchedulerStats;
	const long current =
		*(volatile long*)0x006B4CA0;

	++stats->logicCalls;
	if (SpideyRetailFrameUpdaterActive())
		++stats->logicUpdaterActive;

	if (stats->logicVblankValid)
	{
		const long delta =
			current - stats->lastLogicVblank;
		if (delta <= 0)
			++stats->logicDelta0;
		else if (delta == 1)
			++stats->logicDelta1;
		else if (delta == 2)
			++stats->logicDelta2;
		else
			++stats->logicDelta3Plus;
	}
	else
	{
		stats->logicVblankValid = 1;
	}

	stats->lastLogicVblank =
		current;
}

static void SpideyRecordChasePresentScheduler()
{
	if (SpideyRetailGetLevelId() != 0x501)
		return;

	SpideyChaseSchedulerStats* stats =
		&gSpideyChaseSchedulerStats;
	const long current =
		*(volatile long*)0x006B4CA0;

	++stats->presentCalls;
	if (SpideyRetailFrameUpdaterActive())
		++stats->presentUpdaterActive;

	if (stats->presentVblankValid)
	{
		const long delta =
			current - stats->lastPresentVblank;
		if (delta <= 0)
		{
			++stats->presentDelta0;
			stats->presentDelta2Run = 0;
		}
		else if (delta == 1)
		{
			++stats->presentDelta1;
			stats->presentDelta2Run = 0;
		}
		else if (delta == 2)
		{
			++stats->presentDelta2;
			++stats->presentDelta2Run;
			if (stats->presentDelta2Run >
				stats->presentDelta2MaxRun)
			{
				stats->presentDelta2MaxRun =
					stats->presentDelta2Run;
			}
		}
		else
		{
			++stats->presentDelta3Plus;
			stats->presentDelta2Run = 0;
		}
	}
	else
	{
		stats->presentVblankValid = 1;
	}

	stats->lastPresentVblank =
		current;
}

static void SpideyLogChaseSchedulerStats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	const SpideyChaseSchedulerStats* stats =
		&gSpideyChaseSchedulerStats;
	fprintf(
		f,
		"chase_scheduler_stats logic_calls=%lu logic_delta0=%lu logic_delta1=%lu logic_delta2=%lu logic_delta3plus=%lu logic_updater_active=%lu present_calls=%lu present_delta0=%lu present_delta1=%lu present_delta2=%lu present_delta3plus=%lu present_updater_active=%lu present_delta2_max_run=%lu retail_level=0x501 updater1=0x005FAE98 updater2=0x0060CFB0 vblanks=0x006B4CA0 policy=in_memory_only\\n",
		stats->logicCalls,
		stats->logicDelta0,
		stats->logicDelta1,
		stats->logicDelta2,
		stats->logicDelta3Plus,
		stats->logicUpdaterActive,
		stats->presentCalls,
		stats->presentDelta0,
		stats->presentDelta1,
		stats->presentDelta2,
		stats->presentDelta3Plus,
		stats->presentUpdaterActive,
		stats->presentDelta2MaxRun);
	fclose(f);
}

static void SpideyCaptureRetailScheduler()
{
	SpideyLogHighFpsRetailBytes(
		"Logic_Block",
		0x00455400,
		0x1A0);
	SpideyLogHighFpsRetailBytes(
		"Display_Block",
		0x004555A0,
		0x430);
	SpideyLogHighFpsRetailBytes(
		"PlayAway_Block",
		0x004559D0,
		0x2C0);
	SpideyLogHighFpsRetailBytes(
		"SpideyMain_Block",
		0x00455C90,
		0x610);

	const unsigned long ranges[][2] =
	{
		{ 0x00455400, 0x1A0 },
		{ 0x004555A0, 0x430 },
		{ 0x004559D0, 0x2C0 },
		{ 0x00455C90, 0x610 }
	};
	const char* labels[] =
	{
		"Logic",
		"Display",
		"PlayAway",
		"SpideyMain"
	};

	for (int i = 0; i < 4; ++i)
	{
		char label[96];

		sprintf(
			label,
			"%s_FirstFrameUpdater",
			labels[i]);
		SpideyLogRetailFieldXrefs(
			label,
			ranges[i][0],
			ranges[i][1],
			0x005FAE98);

		sprintf(
			label,
			"%s_SecondFrameUpdater",
			labels[i]);
		SpideyLogRetailFieldXrefs(
			label,
			ranges[i][0],
			ranges[i][1],
			0x0060CFB0);

		sprintf(
			label,
			"%s_Vblanks",
			labels[i]);
		SpideyLogRetailFieldXrefs(
			label,
			ranges[i][0],
			ranges[i][1],
			0x006B4CA0);

		sprintf(
			label,
			"%s_TimerRelated",
			labels[i]);
		SpideyLogRetailFieldXrefs(
			label,
			ranges[i][0],
			ranges[i][1],
			0x006B4CA8);
	}
}

// Chase Venom scripted steering compatibility.
//
// Runtime now proves the level and in-engine cutscene itself are running at
// the intended 60-Hz cadence. The remaining failure is the authored player
// route: CPlayer::SynthesizeAnalogueInput recalculates its target-steering
// output every Logic call. At 60 Hz that feedback loop reacts three times as
// often as a 20-Hz-authored Chase Venom sequence.
//
// Keep rendering, physics, animation and ordinary gameplay at 60 Hz. Only the
// Chase Venom synthesized-input producer is sampled at a 20-Hz equivalent:
// execute retail synth once per 3 canonical ticks, advance its internal timers
// by the full elapsed tick count, and hold the last synthesized axes on the
// intervening 60-Hz updates. ReadAnalogueInput still consumes those held axes
// every frame, so movement remains visually smooth.
//
// The same level-scoped ReadAnalogueInput wrapper also restores the proven
// raw field_8F0 input ramp to its 20-Hz real-time rate while synthesized input
// is active. That ramp change alone was previously insufficient, but it is
// part of faithfully reproducing the scripted control producer's authored
// cadence while leaving manual controls untouched.
typedef void (__fastcall *SpideyRetailPlayerSynthInputFn)(
		CPlayer*,
		void*);
typedef void (__fastcall *SpideyRetailReadAnalogueInputFn)(
		CPlayer*,
		void*);

#define SPIDEY_CHASE_SYNTH_TRACE_CAPACITY 2048

struct SpideyChaseSynthTraceSample
{
	unsigned long tick;
	int elapsed;
	int field80Before;
	int scriptClock;
	unsigned int scriptActive;
	unsigned int synthMode;
	int axisX;
	int axisY;
	int ramp;
	unsigned long state;
	int posX;
	int posY;
	int posZ;
	int angleY;
	int headingTraceValid;
	int cameraHeading;
	int cameraMode;
	int cameraInterpTicks;
	unsigned int cameraNextShotPulsesSet;
	unsigned int cameraNextShotPulses;
	int inputBasisHeading;
	int desiredRelativeHeading;
	int desiredWorldHeading;
	unsigned int wall;
	unsigned int ceiling;
	unsigned long collision;
	int groundGrace;
	unsigned long workerMaskBefore;
	unsigned long workerMaskAfter;
	int headBeforeType;
	int headBeforeSize;
	int headBefore2;
	int headBefore3;
	int headAfterType;
	int headAfterSize;
	int headAfter2;
	int headAfter3;
};

static SpideyChaseSynthTraceSample
	gSpideyChaseSynthTrace[
		SPIDEY_CHASE_SYNTH_TRACE_CAPACITY];
static unsigned long gSpideyChaseSynthTraceCount = 0;
static unsigned long gSpideyChaseSynthTraceDropped = 0;

static CPlayer* gSpideyChaseSynthPlayer = 0;
static long gSpideyChaseSynthLastTick = 0;
static int gSpideyChaseSynthTickValid = 0;
static int gSpideyChaseSynthAccumulatedTicks = 0;
static signed char gSpideyChaseSynthHeldX = 0;
static signed char gSpideyChaseSynthHeldY = 0;
static int gSpideyChaseSynthHeldValid = 0;
static int gSpideyChaseSynthFreshThisCall = 0;
static int gSpideyChaseSynthHeldWorldHeading = 0;
static int gSpideyChaseSynthHeldWorldHeadingValid = 0;
static unsigned long gSpideyChaseHeadingSamples = 0;
static unsigned long gSpideyChaseHeadingCorrections = 0;
static unsigned long gSpideyChaseHeadingMaxPreCorrectionDrift = 0;
static unsigned long gSpideyChaseSynthCalls = 0;
static unsigned long gSpideyChaseSynthActiveCalls = 0;
static unsigned long gSpideyChaseSynthRetailUpdates = 0;
static unsigned long gSpideyChaseSynthHeldCalls = 0;
static unsigned long gSpideyChaseSynthMaxElapsed = 0;
static unsigned long gSpideyChaseType3HeldLatchCalls = 0;
static unsigned long gSpideyChaseType3HeldLatchWrites = 0;
static unsigned long gSpideyChaseType3HeldLatchDynamic = 0;
static unsigned long gSpideyChaseType3HeldLatchDirectional = 0;

// L5A1 scripted Wait-chain state used by the through-building Chase.
//
// Retail trigger graph:
//   TRGP_Wait05 -> node 70  -> starts node 71's long scripted sequence and
//                             enables TRGP_Wait06 through node 336.
//   TRGP_Wait06 -> node 298 -> advances the world/script state again.
//
// Their checksums are the actual CRCs of the authored trigger names and are
// present in L5A1_G.psx.  Unlike SCommandPoint::Executed (cleared by Logic
// every update), NumPulsesSet/NumPulses persist after opcode 134 + opcode 3:
//   completed stage == NumPulsesSet != 0 && NumPulses == 0.
//
// The black-wall failure happens while Wait05 is already complete, Wait06 is
// expected next, and the long type-3/code-10 movement is still driving the
// player.  Recover Wait06 only in that exact persistent stage.
static const unsigned long kSpideyChaseWait05Checksum =
	0xF24C5EF1UL;
static const int kSpideyChaseWait05Node = 70;
static const unsigned long kSpideyChaseWait06Checksum =
	0x6B450F4BUL;
static const int kSpideyChaseWait06Node = 298;
static CPlayer* gSpideyChaseBuildingEntryPlayer = 0;
static int gSpideyChaseBuildingEntryRecovered = 0;
static int gSpideyChaseBuildingEntryBlockedSamples = 0;
static unsigned long gSpideyChaseBuildingEntryChecks = 0;
static unsigned long gSpideyChaseBuildingEntryBlockedMatches = 0;
static unsigned long gSpideyChaseBuildingEntryNaturalSeen = 0;
static unsigned long gSpideyChaseBuildingEntryRecoveryAttempts = 0;
static unsigned long gSpideyChaseBuildingEntryRecoveryFires = 0;

static int gSpideyChaseSynthInstalled = 0;

// True authored-cadence player-AI compatibility.
//
// CPlayer::AI @ 0x004C65C0 performs ordinary per-frame housekeeping and then
// calls the callback stored at player+0x554.  CPlayer_CPlayer writes retail
// SpideyAI0 (0x004B13F0) into that slot at 0x004BA2AF/0x004BA2B5.
//
// The previous native-60 compatibility kept SpideyAI0 itself at 60 Hz and
// corrected selected subsystems (synth sample/hold, analogue ramp, movement
// half-steps).  That preserves elapsed time but not authored update order:
// turning, friction, collision, surface-state, trigger sweeps and other
// feedback systems are evaluated three times per 50 ms instead of once.
//
// During synthesized Chase control only, run the retail callback once per
// three canonical 60-Hz ticks with field_80 set to the accumulated elapsed
// ticks.  CPlayer::AI housekeeping, CSuper animation, rendering, cameras and
// every other body remain on the normal 60-Hz simulation cadence.
typedef void (__cdecl *SpideyRetailPlayerAIFn)(
	CPlayer*);

static CPlayer* gSpideyChasePlayerAI20Player = 0;
static long gSpideyChasePlayerAI20LastTick = 0;
static int gSpideyChasePlayerAI20TickValid = 0;
static int gSpideyChasePlayerAI20AccumulatedTicks = 0;
static unsigned long gSpideyChasePlayerAI20Calls = 0;
static unsigned long gSpideyChasePlayerAI20RetailCalls = 0;
static unsigned long gSpideyChasePlayerAI20HeldCalls = 0;
static unsigned long gSpideyChasePlayerAI20MaxElapsed = 0;
static int gSpideyChasePlayerAI20Installed = 0;

static void SpideyResetChasePlayerAI20State(
		CPlayer* player)
{
	gSpideyChasePlayerAI20Player =
		player;
	gSpideyChasePlayerAI20LastTick =
		0;
	gSpideyChasePlayerAI20TickValid =
		0;
	gSpideyChasePlayerAI20AccumulatedTicks =
		0;
}

static void __cdecl SpideyChasePlayerAI20Hz(
		CPlayer* player)
{
	SpideyRetailPlayerAIFn retail =
		(SpideyRetailPlayerAIFn)
		0x004B13F0;

	++gSpideyChasePlayerAI20Calls;

	if (!player ||
		SpideyRetailGetLevelId() != 0x501 ||
		!player->field_1AC)
	{
		if (gSpideyChasePlayerAI20Player != player ||
			gSpideyChasePlayerAI20TickValid)
		{
			SpideyResetChasePlayerAI20State(
				player);
		}

		retail(
			player);
		return;
	}

	if (gSpideyChasePlayerAI20Player != player)
	{
		SpideyResetChasePlayerAI20State(
			player);
	}

	const long currentTick =
		*(volatile long*)0x006B4CA8;

	if (!gSpideyChasePlayerAI20TickValid)
	{
		gSpideyChasePlayerAI20LastTick =
			currentTick;
		gSpideyChasePlayerAI20TickValid =
			1;

		int initialElapsed =
			player->field_80;
		if (initialElapsed < 0)
			initialElapsed = 0;
		if (initialElapsed > 6)
			initialElapsed = 6;
		gSpideyChasePlayerAI20AccumulatedTicks =
			initialElapsed;
	}
	else
	{
		int elapsed =
			(int)(
				currentTick -
				gSpideyChasePlayerAI20LastTick);
		gSpideyChasePlayerAI20LastTick =
			currentTick;

		if (elapsed < 0)
			elapsed = 0;
		if (elapsed > 6)
			elapsed = 6;

		gSpideyChasePlayerAI20AccumulatedTicks +=
			elapsed;
	}

	if (gSpideyChasePlayerAI20AccumulatedTicks < 3)
	{
		++gSpideyChasePlayerAI20HeldCalls;
		return;
	}

	int simElapsed =
		gSpideyChasePlayerAI20AccumulatedTicks;
	if (simElapsed < 1)
		simElapsed = 1;
	if (simElapsed > 6)
		simElapsed = 6;

	const int originalField80 =
		player->field_80;
	player->field_80 =
		simElapsed;

	retail(
		player);

	player->field_80 =
		originalField80;
	gSpideyChasePlayerAI20AccumulatedTicks =
		0;

	++gSpideyChasePlayerAI20RetailCalls;
	if ((unsigned long)simElapsed >
		gSpideyChasePlayerAI20MaxElapsed)
	{
		gSpideyChasePlayerAI20MaxElapsed =
			(unsigned long)simElapsed;
	}
}

static int SpideyInstallChasePlayerAI20HzCompat()
{
	// Patch only the immediate callback written by CPlayer_CPlayer:
	//   C7 86 54 05 00 00 F0 13 4B 00
	//                     ^ immediate at 0x004BA2B5
	const unsigned long original =
		0x004B13F0UL;
	const unsigned long replacement =
		(unsigned long)
		(void*)&SpideyChasePlayerAI20Hz;

	gSpideyChasePlayerAI20Installed =
		SpideyPatchBytes(
			0x004BA2B5,
			(const unsigned char*)&original,
			(const unsigned char*)&replacement,
			sizeof(original),
			"timing_chase_player_ai_20hz_callback");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"chase_player_ai_20hz_install installed=%d constructor_immediate=0x004BA2B5 retail=0x004B13F0 wrapper=0x%08lX policy=cplayer_housekeeping_60hz_scripted_spideyai0_20hz_field80_3\\n",
			gSpideyChasePlayerAI20Installed,
			replacement);
		fclose(f);
	}

	return gSpideyChasePlayerAI20Installed;
}

static void SpideyLogChasePlayerAI20Stats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"chase_player_ai_20hz_stats installed=%d calls=%lu retail_calls=%lu held_calls=%lu max_elapsed=%lu level=0x501 policy=cplayer_housekeeping_60hz_scripted_spideyai0_20hz_field80_accumulated downstream_trigger_recovery=disabled\\n",
		gSpideyChasePlayerAI20Installed,
		gSpideyChasePlayerAI20Calls,
		gSpideyChasePlayerAI20RetailCalls,
		gSpideyChasePlayerAI20HeldCalls,
		gSpideyChasePlayerAI20MaxElapsed);
	fclose(f);
}

// Pair the scripted player cadence with the active retail camera cadence.
//
// Type-3 synthesized codes 8..11 are literal camera-relative directions.
// In particular Chase code 10 writes E2D=-127.  CheckForwards later turns
// that into a world-space desired heading using the camera transform heading
// at CCamera+0x23A.  Therefore a 20-Hz SpideyAI0 still does not reproduce the
// 20-FPS path if CCamera_AI continues solving the camera three times per 50 ms.
//
// The active camera's vtable is 0x0053B4BC and its AI slot (+8) is
// 0x0053B4C4 -> retail CCamera_AI @ 0x00417CB0.
//
// During L5A1 synthesized player control only, run the active camera AI once
// per three canonical ticks with accumulated field_80 (normally 3).  Other
// cameras and all ordinary gameplay call retail every update.
typedef void (__fastcall *SpideyRetailCameraAIFn)(
	CCamera*,
	void*);

static CCamera* gSpideyChaseCameraAI20Camera = 0;
static long gSpideyChaseCameraAI20LastTick = 0;
static int gSpideyChaseCameraAI20TickValid = 0;
static int gSpideyChaseCameraAI20AccumulatedTicks = 0;
static unsigned long gSpideyChaseCameraAI20Calls = 0;
static unsigned long gSpideyChaseCameraAI20RetailCalls = 0;
static unsigned long gSpideyChaseCameraAI20HeldCalls = 0;
static unsigned long gSpideyChaseCameraAI20MaxElapsed = 0;
static int gSpideyChaseCameraAI20Installed = 0;

static void SpideyResetChaseCameraAI20State(
		CCamera* camera)
{
	gSpideyChaseCameraAI20Camera =
		camera;
	gSpideyChaseCameraAI20LastTick =
		0;
	gSpideyChaseCameraAI20TickValid =
		0;
	gSpideyChaseCameraAI20AccumulatedTicks =
		0;
}

static void __fastcall SpideyChaseCameraAI20Hz(
		CCamera* camera,
		void*)
{
	SpideyRetailCameraAIFn retail =
		(SpideyRetailCameraAIFn)
		0x00417CB0;

	++gSpideyChaseCameraAI20Calls;

	CPlayer* player =
		*(CPlayer**)0x006A9038;
	CCamera* activeCamera =
		*(CCamera**)0x0056F3B8;

	const int chaseScripted =
		camera &&
		camera == activeCamera &&
		player &&
		SpideyRetailGetLevelId() == 0x501 &&
		player->field_1AC;

	if (!chaseScripted)
	{
		if (gSpideyChaseCameraAI20Camera != camera ||
			gSpideyChaseCameraAI20TickValid)
		{
			SpideyResetChaseCameraAI20State(
				camera);
		}

		retail(
			camera,
			0);
		return;
	}

	if (gSpideyChaseCameraAI20Camera != camera)
	{
		SpideyResetChaseCameraAI20State(
			camera);
	}

	const long currentTick =
		*(volatile long*)0x006B4CA8;

	if (!gSpideyChaseCameraAI20TickValid)
	{
		gSpideyChaseCameraAI20LastTick =
			currentTick;
		gSpideyChaseCameraAI20TickValid =
			1;

		int initialElapsed =
			camera->field_80;
		if (initialElapsed < 0)
			initialElapsed = 0;
		if (initialElapsed > 6)
			initialElapsed = 6;
		gSpideyChaseCameraAI20AccumulatedTicks =
			initialElapsed;
	}
	else
	{
		int elapsed =
			(int)(
				currentTick -
				gSpideyChaseCameraAI20LastTick);
		gSpideyChaseCameraAI20LastTick =
			currentTick;

		if (elapsed < 0)
			elapsed = 0;
		if (elapsed > 6)
			elapsed = 6;

		gSpideyChaseCameraAI20AccumulatedTicks +=
			elapsed;
	}

	if (gSpideyChaseCameraAI20AccumulatedTicks < 3)
	{
		++gSpideyChaseCameraAI20HeldCalls;
		return;
	}

	int simElapsed =
		gSpideyChaseCameraAI20AccumulatedTicks;
	if (simElapsed < 1)
		simElapsed = 1;
	if (simElapsed > 6)
		simElapsed = 6;

	const int originalField80 =
		camera->field_80;
	camera->field_80 =
		simElapsed;

	retail(
		camera,
		0);

	camera->field_80 =
		originalField80;
	gSpideyChaseCameraAI20AccumulatedTicks =
		0;

	++gSpideyChaseCameraAI20RetailCalls;
	if ((unsigned long)simElapsed >
		gSpideyChaseCameraAI20MaxElapsed)
	{
		gSpideyChaseCameraAI20MaxElapsed =
			(unsigned long)simElapsed;
	}
}

static int SpideyInstallChaseCameraAI20HzCompat()
{
	const unsigned long original =
		0x00417CB0UL;
	const unsigned long replacement =
		(unsigned long)
		(void*)&SpideyChaseCameraAI20Hz;

	gSpideyChaseCameraAI20Installed =
		SpideyPatchBytes(
			0x0053B4C4,
			(const unsigned char*)&original,
			(const unsigned char*)&replacement,
			sizeof(original),
			"timing_chase_camera_ai_20hz_vtable");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"chase_camera_ai_20hz_install installed=%d vtable_slot=0x0053B4C4 retail=0x00417CB0 wrapper=0x%08lX policy=active_camera_20hz_only_during_l5a1_synthesized_player_control\\n",
			gSpideyChaseCameraAI20Installed,
			replacement);
		fclose(f);
	}

	return gSpideyChaseCameraAI20Installed;
}

static void SpideyLogChaseCameraAI20Stats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"chase_camera_ai_20hz_stats installed=%d calls=%lu retail_calls=%lu held_calls=%lu max_elapsed=%lu level=0x501 policy=paired_scripted_player_and_active_camera_20hz\\n",
		gSpideyChaseCameraAI20Installed,
		gSpideyChaseCameraAI20Calls,
		gSpideyChaseCameraAI20RetailCalls,
		gSpideyChaseCameraAI20HeldCalls,
		gSpideyChaseCameraAI20MaxElapsed);
	fclose(f);
}

// Chase Venom world-actor / script-controller cadence compatibility.
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

static CPlayer* gSpideyChaseRampPlayer = 0;
static int gSpideyChaseRampRemainder = 0;
static unsigned long gSpideyChaseRampCalls = 0;
static unsigned long gSpideyChaseRampCorrections = 0;
static unsigned long gSpideyChaseRampUnexpected = 0;
static int gSpideyChaseRampInstalled = 0;

static unsigned long SpideyChaseReadWorkerTypeMask(
		CPlayer* player)
{
	if (!player)
		return 0;

	unsigned long mask =
		0;

	__try
	{
		unsigned char* raw =
			(unsigned char*)player;
		int* block =
			*(int**)(raw + 0x1BC);

		for (int nodes = 0;
			 block && nodes < 32;
			 ++nodes)
		{
			const int type =
				block[0];
			const int size =
				block[1];

			if (type >= 0 &&
				type < 32)
			{
				mask |=
					1UL <<
					type;
			}

			if (size < 2 ||
				size > 32)
			{
				break;
			}

			int* next =
				(int*)block[
					size -
					1];
			if (next == block)
				break;
			block =
				next;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return mask;
	}

	return mask;
}

static void SpideyChaseReadWorkerHead(
		CPlayer* player,
		int* type,
		int* size,
		int* data2,
		int* data3)
{
	if (type)
		*type = -1;
	if (size)
		*size = 0;
	if (data2)
		*data2 = 0;
	if (data3)
		*data3 = 0;

	if (!player)
		return;

	__try
	{
		unsigned char* raw =
			(unsigned char*)player;
		int* block =
			*(int**)(raw + 0x1BC);
		if (!block)
			return;

		if (type)
			*type = block[0];
		if (size)
			*size = block[1];
		if (data2 &&
			block[1] > 2)
		{
			*data2 = block[2];
		}
		if (data3 &&
			block[1] > 3)
		{
			*data3 = block[3];
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

// Recover the authored Wait05 -> Wait06 transition only when the persistent
// trigger state proves Wait05 completed and Wait06 did not.
//
// SCommandPoint::Executed is deliberately not used here: retail Logic clears
// it every logic update.  NumPulsesSet/NumPulses are the persistent evidence
// left by this level's opcode-134/opcode-3 trigger stages.
static void SpideyChaseRecoverBuildingEntryIfMissed(
		CPlayer* player)
{
	if (!player)
		return;

	++gSpideyChaseBuildingEntryChecks;

	if (gSpideyChaseBuildingEntryPlayer !=
		player)
	{
		gSpideyChaseBuildingEntryPlayer =
			player;
		gSpideyChaseBuildingEntryRecovered =
			0;
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
	}

	if (SpideyRetailGetLevelId() !=
		0x501)
	{
		gSpideyChaseBuildingEntryRecovered =
			0;
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
		return;
	}

	if (!player->field_1AC ||
		gSpideyChaseBuildingEntryRecovered)
	{
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
		return;
	}

	SCommandPoint* wait05 =
		GetCommandPoint(
			kSpideyChaseWait05Node);
	SCommandPoint* wait06 =
		GetCommandPoint(
			kSpideyChaseWait06Node);

	if (!wait05 ||
		!wait06 ||
		wait05->Checksum !=
			kSpideyChaseWait05Checksum ||
		wait06->Checksum !=
			kSpideyChaseWait06Checksum)
	{
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
		return;
	}

	const int wait05Complete =
		wait05->NumPulsesSet &&
		wait05->NumPulses == 0;
	const int wait06Complete =
		wait06->NumPulsesSet &&
		wait06->NumPulses == 0;

	// If retail naturally completed Wait06, there is nothing to recover.
	if (wait06Complete)
	{
		++gSpideyChaseBuildingEntryNaturalSeen;
		gSpideyChaseBuildingEntryRecovered =
			1;
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
		return;
	}

	// The long through-building command is only a valid Wait06 recovery site
	// after Wait05 has actually completed.
	if (!wait05Complete)
	{
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
		return;
	}

	int headType = -1;
	int headSize = 0;
	int headCode = 0;
	int headTimer = 0;
	SpideyChaseReadWorkerHead(
		player,
		&headType,
		&headSize,
		&headCode,
		&headTimer);

	const int inBuildingEntryCluster =
		player->mPos.vx >= 73500000 &&
		player->mPos.vx <= 76000000 &&
		player->mPos.vy >= -2000000 &&
		player->mPos.vy <= 3000000 &&
		player->mPos.vz >= 14500000 &&
		player->mPos.vz <= 17500000;

	const int blockedOnAuthoredWorker =
		(player->mCollision & 1) &&
		headType == 3 &&
		headSize > 3 &&
		headCode == 10 &&
		inBuildingEntryCluster;

	if (!blockedOnAuthoredWorker)
	{
		gSpideyChaseBuildingEntryBlockedSamples =
			0;
		return;
	}

	++gSpideyChaseBuildingEntryBlockedMatches;
	++gSpideyChaseBuildingEntryBlockedSamples;

	// Require two consecutive observed wall-collision samples.  A transient
	// brush against nearby geometry is not enough to advance the Wait chain.
	if (gSpideyChaseBuildingEntryBlockedSamples <
		2)
	{
		return;
	}

	++gSpideyChaseBuildingEntryRecoveryAttempts;

	unsigned long itemChecksum = 0;
	unsigned long itemFlags = 0;
	int itemRegion = -1;
	int itemModel = -1;
	unsigned long faceFlags = 0;

	__try
	{
		CItem* item =
			player->mLineInfo.pItem;
		if (item)
		{
			itemFlags =
				(unsigned long)item->mFlags;
			itemRegion =
				(int)item->mRegion;
			itemModel =
				(int)item->mModel;
			itemChecksum =
				(unsigned long)
				Spool_GetModelChecksum(
					item);
		}
		if (player->mLineInfo.pFace)
		{
			faceFlags =
				(unsigned long)
				player->mLineInfo.pFace[3];
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		itemChecksum = 0;
		itemFlags = 0;
		itemRegion = -1;
		itemModel = -1;
		faceFlags = 0;
	}

	SCommandPoint* fired =
		Trig_TriggerCommandPoint(
			kSpideyChaseWait06Checksum,
			true);

	if (fired)
	{
		++gSpideyChaseBuildingEntryRecoveryFires;
		gSpideyChaseBuildingEntryRecovered =
			1;

		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"chase_building_entry_recovery fired=1 stage=wait05_to_wait06 node=%d checksum=0x%08lX wait05_pulses_set=%u wait05_pulses=%u wait06_pulses_set=%u wait06_pulses=%u pos=%d,%d,%d state=0x%08lX collision=0x%04X head=%d,%d,%d,%d item_checksum=0x%08lX item_region=%d item_model=%d item_flags=0x%08lX face_flags=0x%08lX policy=advance_next_authored_wait_stage_no_noclip_no_teleport\\n",
				kSpideyChaseWait06Node,
				kSpideyChaseWait06Checksum,
				(unsigned int)wait05->NumPulsesSet,
				(unsigned int)wait05->NumPulses,
				(unsigned int)wait06->NumPulsesSet,
				(unsigned int)wait06->NumPulses,
				player->mPos.vx,
				player->mPos.vy,
				player->mPos.vz,
				(unsigned long)player->field_E1C,
				(unsigned int)player->mCollision,
				headType,
				headSize,
				headCode,
				headTimer,
				itemChecksum,
				itemRegion,
				itemModel,
				itemFlags,
				faceFlags);
			fclose(f);
		}
	}
	else
	{
		// A same-frame natural collision may already have consumed Wait06.
		// Re-check the persistent pulse state before allowing another attempt.
		wait06 =
			GetCommandPoint(
				kSpideyChaseWait06Node);
		if (wait06 &&
			wait06->NumPulsesSet &&
			wait06->NumPulses == 0)
		{
			++gSpideyChaseBuildingEntryNaturalSeen;
			gSpideyChaseBuildingEntryRecovered =
				1;
		}
		else
		{
			gSpideyChaseBuildingEntryBlockedSamples =
				1;
		}
	}
}

// Type-3 synth workers have per-call side effects in addition to their
// analogue axes. Retail 0x004BCC2D dispatches worker->data2 as follows:
//
//   0..7,16..19: assert player[0x1C0 + code*0x10] and, on the first
//                 assertion, player[offset + 1].
//   8:             player[0x240] = 1, E2E = -127
//   9:             player[0x250] = 1, E2E = +127
//   10:            player[0x260] = 1, E2D = -127
//   11:            player[0x270] = 1, E2D = +127
//
// The 20-Hz sample/hold wrapper already preserves the final E2D/E2E result.
// On the two held 60-Hz frames, however, skipping retail synth entirely also
// skipped these latches while the rest of CPlayer still updated at 60 Hz.
// Reassert only the latch side effects here; do not touch the held axes, so
// final worker-order precedence remains exactly what the fresh retail sample
// produced.
static void SpideyChaseReassertHeldType3Latches(
		CPlayer* player)
{
	if (!player)
		return;

	__try
	{
		unsigned char* raw =
			(unsigned char*)player;
		int* block =
			*(int**)(raw + 0x1BC);
		int foundType3 =
			0;

		for (int nodes = 0;
			 block && nodes < 32;
			 ++nodes)
		{
			const int type =
				block[0];
			const int size =
				block[1];

			if (type == 3 &&
				size > 3)
			{
				const int code =
					block[2];
				int wrote =
					0;

				foundType3 =
					1;

				if ((code >= 0 &&
					 code <= 7) ||
					(code >= 16 &&
					 code <= 19))
				{
					const int offset =
						0x1C0 +
						(code << 4);

					if (!raw[offset])
					{
						raw[
							offset +
							1] =
							1;
					}
					raw[offset] =
						1;
					++gSpideyChaseType3HeldLatchDynamic;
					wrote =
						1;
				}
				else if (code == 8)
				{
					raw[0x240] =
						1;
					++gSpideyChaseType3HeldLatchDirectional;
					wrote =
						1;
				}
				else if (code == 9)
				{
					raw[0x250] =
						1;
					++gSpideyChaseType3HeldLatchDirectional;
					wrote =
						1;
				}
				else if (code == 10)
				{
					raw[0x260] =
						1;
					++gSpideyChaseType3HeldLatchDirectional;
					wrote =
						1;
				}
				else if (code == 11)
				{
					raw[0x270] =
						1;
					++gSpideyChaseType3HeldLatchDirectional;
					wrote =
						1;
				}

				if (wrote)
				{
					++gSpideyChaseType3HeldLatchWrites;
				}
			}

			if (size < 2 ||
				size > 32)
			{
				break;
			}

			int* next =
				(int*)block[
					size -
					1];
			if (next == block)
				break;
			block =
				next;
		}

		if (foundType3)
		{
			++gSpideyChaseType3HeldLatchCalls;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void SpideyRecordChaseSynthTrace(
		CPlayer* player,
		unsigned long tick,
		int elapsed,
		int field80Before,
		int headBeforeType,
		int headBeforeSize,
		int headBefore2,
		int headBefore3)
{
	if (!player)
		return;

	if (gSpideyChaseSynthTraceCount >=
		SPIDEY_CHASE_SYNTH_TRACE_CAPACITY)
	{
		++gSpideyChaseSynthTraceDropped;
		return;
	}

	SpideyChaseSynthTraceSample* sample =
		&gSpideyChaseSynthTrace[
			gSpideyChaseSynthTraceCount];
	memset(
		sample,
		0,
		sizeof(*sample));

	sample->tick =
		tick;
	sample->elapsed =
		elapsed;
	sample->field80Before =
		field80Before;
	sample->workerMaskBefore =
		SpideyChaseReadWorkerTypeMask(
			player);
	sample->headBeforeType =
		headBeforeType;
	sample->headBeforeSize =
		headBeforeSize;
	sample->headBefore2 =
		headBefore2;
	sample->headBefore3 =
		headBefore3;

	__try
	{
		unsigned char* raw =
			(unsigned char*)player;
		sample->scriptClock =
			*(int*)(raw + 0x1B0);
		sample->scriptActive =
			(unsigned int)*(raw + 0x1B4);
		sample->synthMode =
			(unsigned int)*(raw + 0x1AC);
		sample->axisX =
			(int)(signed char)*(raw + 0xE2D);
		sample->axisY =
			(int)(signed char)*(raw + 0xE2E);
		sample->ramp =
			*(int*)(raw + 0x8F0);
		sample->state =
			(unsigned long)*(unsigned long*)(raw + 0xE1C);
		sample->posX =
			player->mPos.vx;
		sample->posY =
			player->mPos.vy;
		sample->posZ =
			player->mPos.vz;
		sample->angleY =
			(int)player->mAngles.vy;
		sample->wall =
			(unsigned int)*(raw + 0x8E8);
		sample->ceiling =
			(unsigned int)*(raw + 0x8E9);
		sample->collision =
			(unsigned long)player->mCollision;
		sample->groundGrace =
			player->field_EA4;

		CCamera* traceCamera =
			*(CCamera**)0x0056F3B8;
		if (traceCamera)
		{
			sample->cameraMode =
				(int)traceCamera->mCameraMode;
			sample->cameraInterpTicks =
				(int)traceCamera->field_2BC;
		}

		SCommandPoint* nextCameraShot =
			GetCommandPoint(74);
		if (nextCameraShot)
		{
			sample->cameraNextShotPulsesSet =
				(unsigned int)nextCameraShot->NumPulsesSet;
			sample->cameraNextShotPulses =
				(unsigned int)nextCameraShot->NumPulses;
		}

		sample->workerMaskAfter =
			SpideyChaseReadWorkerTypeMask(
				player);
		SpideyChaseReadWorkerHead(
			player,
			&sample->headAfterType,
			&sample->headAfterSize,
			&sample->headAfter2,
			&sample->headAfter3);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	++gSpideyChaseSynthTraceCount;
}

static unsigned long gSpideyScriptMotionTraceSamples =
	0;
static unsigned long gSpideyScriptMotionTraceDropped =
	0;
static unsigned long gSpideyScriptMotionLastMask =
	0xFFFFFFFFUL;
static int gSpideyScriptMotionLastHead =
	-999;
static int gSpideyScriptMotionLastState =
	-1;
static int gSpideyScriptMotionLastAnim =
	-1;
static int gSpideyScriptMotionLastCrawl =
	-1;

static void SpideyLogScriptMotionSnapshot(
		const char* phase,
		CPlayer* player)
{
	if (!player ||
		!player->field_1AC)
	{
		return;
	}

	CCamera* camera =
		*(CCamera**)0x0056F3B8;
	if (!camera ||
		camera->mCameraMode !=
			CAMERAMODE_DEMO)
	{
		return;
	}

	unsigned long workerMask =
		SpideyChaseReadWorkerTypeMask(
			player);
	int headType = -1;
	int headSize = 0;
	int head2 = 0;
	int head3 = 0;
	SpideyChaseReadWorkerHead(
		player,
		&headType,
		&headSize,
		&head2,
		&head3);

	const int state =
		(int)player->field_E1C;
	const int anim =
		(int)player->mAnim;
	const int crawl =
		(int)player->field_AD4;

	const int changed =
		workerMask !=
			gSpideyScriptMotionLastMask ||
		headType !=
			gSpideyScriptMotionLastHead ||
		state !=
			gSpideyScriptMotionLastState ||
		anim !=
			gSpideyScriptMotionLastAnim ||
		crawl !=
			gSpideyScriptMotionLastCrawl;

	const unsigned long tick =
		(unsigned long)
		*(volatile long*)0x006B4CA8;

	if (!changed &&
		(tick % 6UL) != 0)
	{
		return;
	}

	if (gSpideyScriptMotionTraceSamples >=
		256)
	{
		++gSpideyScriptMotionTraceDropped;
		return;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"script_motion phase=%s sample=%lu tick=%lu camera_mode=%d active=%d mask=0x%08lX head=%d,%d,%d,%d state=0x%08X crawl=%d wall=%d ceiling=%d anim=%d frame=%d frac=%d finished=%d field80=%d pos=%ld,%ld,%ld vel=%ld,%ld,%ld acc=%ld,%ld,%ld collision=0x%04X\n",
			phase ? phase : "unknown",
			gSpideyScriptMotionTraceSamples,
			tick,
			(int)camera->mCameraMode,
			(int)player->field_1AC,
			workerMask,
			headType,
			headSize,
			head2,
			head3,
			(unsigned int)state,
			crawl,
			(int)player->field_8E8,
			(int)player->field_8E9,
			anim,
			(int)player->mFrame,
			(int)player->mFrameFrac,
			(int)player->mAnimFinished,
			(int)player->field_80,
			(long)player->mPos.vx,
			(long)player->mPos.vy,
			(long)player->mPos.vz,
			(long)player->mVel.vx,
			(long)player->mVel.vy,
			(long)player->mVel.vz,
			(long)player->mAcc.vx,
			(long)player->mAcc.vy,
			(long)player->mAcc.vz,
			(unsigned int)player->mCollision);
		fclose(f);
	}

	++gSpideyScriptMotionTraceSamples;
	gSpideyScriptMotionLastMask =
		workerMask;
	gSpideyScriptMotionLastHead =
		headType;
	gSpideyScriptMotionLastState =
		state;
	gSpideyScriptMotionLastAnim =
		anim;
	gSpideyScriptMotionLastCrawl =
		crawl;
}

static void SpideyResetChaseSynthState(
		CPlayer* player)
{
	gSpideyChaseSynthPlayer =
		player;
	gSpideyChaseSynthLastTick =
		0;
	gSpideyChaseSynthTickValid =
		0;
	gSpideyChaseSynthAccumulatedTicks =
		0;
	gSpideyChaseSynthHeldX =
		0;
	gSpideyChaseSynthHeldY =
		0;
	gSpideyChaseSynthHeldValid =
		0;
	gSpideyChaseSynthFreshThisCall =
		0;
	gSpideyChaseSynthHeldWorldHeading =
		0;
	gSpideyChaseSynthHeldWorldHeadingValid =
		0;
}

static void __fastcall SpideyChaseVenomSynth20Hz(
		CPlayer* player,
		void*)
{
	SpideyRetailPlayerSynthInputFn retail =
		(SpideyRetailPlayerSynthInputFn)0x004BC300;

	++gSpideyChaseSynthCalls;
	gSpideyChaseSynthFreshThisCall =
		0;

	SpideyLogScriptMotionSnapshot(
		"pre",
		player);

	if (!player ||
		SpideyRetailGetLevelId() != 0x501 ||
		!player->field_1AC)
	{
		if (gSpideyChaseSynthPlayer != player ||
			gSpideyChaseSynthTickValid)
		{
			SpideyResetChaseSynthState(
				player);
		}
		retail(
			player,
			0);
		SpideyLogScriptMotionSnapshot(
			"post",
			player);
		return;
	}

	++gSpideyChaseSynthActiveCalls;

	if (gSpideyChaseSynthPlayer != player)
	{
		SpideyResetChaseSynthState(
			player);
	}

	// Do not synthesize downstream trigger recovery while the authored-cadence
	// player-AI experiment is active.  The purpose of this candidate is to let
	// the natural 20-Hz player trajectory cross the retail trigger faces.
	const long currentTick =
		*(volatile long*)0x006B4CA8;
	int elapsedSinceCall =
		0;

	if (!gSpideyChaseSynthTickValid)
	{
		gSpideyChaseSynthLastTick =
			currentTick;
		gSpideyChaseSynthTickValid =
			1;
	}
	else
	{
		elapsedSinceCall =
			(int)(
				currentTick -
				gSpideyChaseSynthLastTick);
		gSpideyChaseSynthLastTick =
			currentTick;

		if (elapsedSinceCall < 0)
			elapsedSinceCall = 0;
		if (elapsedSinceCall > 6)
			elapsedSinceCall = 6;

		gSpideyChaseSynthAccumulatedTicks +=
			elapsedSinceCall;
	}

	const int firstUpdate =
		!gSpideyChaseSynthHeldValid;

	if (!firstUpdate &&
		gSpideyChaseSynthAccumulatedTicks < 3)
	{
		unsigned char* raw =
			(unsigned char*)player;
		*(signed char*)(raw + 0xE2D) =
			gSpideyChaseSynthHeldX;
		*(signed char*)(raw + 0xE2E) =
			gSpideyChaseSynthHeldY;
		SpideyChaseReassertHeldType3Latches(
			player);
		++gSpideyChaseSynthHeldCalls;
		return;
	}

	int synthElapsed =
		firstUpdate ?
			player->field_80 :
			gSpideyChaseSynthAccumulatedTicks;
	if (synthElapsed < 1)
		synthElapsed = 1;
	if (synthElapsed > 6)
		synthElapsed = 6;

	int headBeforeType = -1;
	int headBeforeSize = 0;
	int headBefore2 = 0;
	int headBefore3 = 0;
	SpideyChaseReadWorkerHead(
		player,
		&headBeforeType,
		&headBeforeSize,
		&headBefore2,
		&headBefore3);

	const int originalField80 =
		player->field_80;
	player->field_80 =
		synthElapsed;

	retail(
		player,
		0);

	player->field_80 =
		originalField80;
	gSpideyChaseSynthAccumulatedTicks =
		0;
	gSpideyChaseSynthHeldX =
		*(signed char*)(
			((unsigned char*)player) +
			0xE2D);
	gSpideyChaseSynthHeldY =
		*(signed char*)(
			((unsigned char*)player) +
			0xE2E);
	gSpideyChaseSynthHeldValid =
		1;
	gSpideyChaseSynthFreshThisCall =
		1;
	++gSpideyChaseSynthRetailUpdates;

	if ((unsigned long)synthElapsed >
		gSpideyChaseSynthMaxElapsed)
	{
		gSpideyChaseSynthMaxElapsed =
			(unsigned long)synthElapsed;
	}

	SpideyRecordChaseSynthTrace(
		player,
		(unsigned long)currentTick,
		synthElapsed,
		originalField80,
		headBeforeType,
		headBeforeSize,
		headBefore2,
		headBefore3);
}

static int SpideyChaseReadCameraTransformHeading(
		int* heading)
{
	if (heading)
		*heading = 0;

	__try
	{
		unsigned char* camera =
			*(unsigned char**)0x0056F3B8;
		if (!camera)
			return 0;

		if (heading)
		{
			*heading =
				(int)*(unsigned short*)(
					camera +
					0x23A) &
				0x0FFF;
		}
		return 1;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
}

static unsigned long SpideyChaseHeadingDistance12(
		int a,
		int b)
{
	int delta =
		(a - b) &
		0x0FFF;
	if (delta > 0x800)
		delta =
			0x1000 -
			delta;
	return (unsigned long)delta;
}

// Retail synthesized movement ultimately becomes camera-relative analogue
// axes. Route worker type 2 explicitly generates them from CCamera+0x23A, and
// the other direction-producing script workers feed the same ReadAnalogueInput
// path. CheckForwards then adds the current CCamera+0x23A again on normal
// ground movement.
//
// Therefore holding only the old axes while the native-60 camera keeps moving
// is not equivalent to the original 20-Hz controller. Preserve the effective
// sampled world steering heading for ANY active synthesized analogue movement.
// On held 60-Hz frames, rewrite E32 so CheckForwards sees the same world
// heading even if the camera moved.
static void SpideyChaseStabilizeHeldWorldHeading(
		CPlayer* player)
{
	if (!player ||
		!player->field_1AC)
	{
		gSpideyChaseSynthHeldWorldHeadingValid =
			0;
		return;
	}

	unsigned char* raw =
		(unsigned char*)player;
	const int axesActive =
		*(signed char*)(raw + 0xE2D) != 0 ||
		*(signed char*)(raw + 0xE2E) != 0;

	if (!axesActive)
	{
		gSpideyChaseSynthHeldWorldHeadingValid =
			0;
		return;
	}

	int cameraHeading =
		0;
	if (!SpideyChaseReadCameraTransformHeading(
			&cameraHeading))
	{
		gSpideyChaseSynthHeldWorldHeadingValid =
			0;
		return;
	}

	const int relativeHeading =
		(int)*(unsigned short*)(
			raw +
			0xE32) &
		0x0FFF;
	const int inputBasisHeading =
		(int)*(unsigned short*)(
			raw +
			0xE34) &
		0x0FFF;
	const int worldHeading =
		player->field_8E8 ?
			relativeHeading :
			(relativeHeading +
			 cameraHeading) &
				0x0FFF;

	if (gSpideyChaseSynthFreshThisCall ||
		!gSpideyChaseSynthHeldWorldHeadingValid)
	{
		gSpideyChaseSynthHeldWorldHeading =
			worldHeading;
		gSpideyChaseSynthHeldWorldHeadingValid =
			1;
		++gSpideyChaseHeadingSamples;

		if (gSpideyChaseSynthTraceCount > 0)
		{
			SpideyChaseSynthTraceSample* sample =
				&gSpideyChaseSynthTrace[
					gSpideyChaseSynthTraceCount -
					1];
			sample->headingTraceValid =
				1;
			sample->cameraHeading =
				cameraHeading;
			sample->inputBasisHeading =
				inputBasisHeading;
			sample->desiredRelativeHeading =
				relativeHeading;
			sample->desiredWorldHeading =
				worldHeading;
		}
		return;
	}

	const unsigned long drift =
		SpideyChaseHeadingDistance12(
			worldHeading,
			gSpideyChaseSynthHeldWorldHeading);
	if (drift >
		gSpideyChaseHeadingMaxPreCorrectionDrift)
	{
		gSpideyChaseHeadingMaxPreCorrectionDrift =
			drift;
	}

	int correctedRelative =
		gSpideyChaseSynthHeldWorldHeading;
	if (!player->field_8E8)
	{
		correctedRelative =
			(correctedRelative -
			 cameraHeading) &
			0x0FFF;
	}

	*(unsigned short*)(
		raw +
		0xE32) =
		(unsigned short)correctedRelative;
	++gSpideyChaseHeadingCorrections;
}

static void __fastcall SpideyChaseVenomReadAnalogue20HzRamp(
		CPlayer* player,
		void*)
{
	SpideyRetailReadAnalogueInputFn retail =
		(SpideyRetailReadAnalogueInputFn)0x004BD510;

	++gSpideyChaseRampCalls;

	if (!player ||
		SpideyRetailGetLevelId() != 0x501 ||
		!player->field_1AC)
	{
		if (gSpideyChaseRampPlayer != player)
		{
			gSpideyChaseRampPlayer =
				player;
		}
		gSpideyChaseRampRemainder =
			0;
		retail(
			player,
			0);
		return;
	}

	if (gSpideyChaseRampPlayer != player)
	{
		gSpideyChaseRampPlayer =
			player;
		gSpideyChaseRampRemainder =
			0;
	}

	unsigned char* raw =
		(unsigned char*)player;
	const int oldRamp =
		*(int*)(raw + 0x8F0);
	int elapsedTicks =
		player->field_80;
	if (elapsedTicks < 0)
		elapsedTicks = 0;
	if (elapsedTicks > 6)
		elapsedTicks = 6;

	retail(
		player,
		0);

	SpideyChaseStabilizeHeldWorldHeading(
		player);

	const int axesActive =
		*(signed char*)(raw + 0xE2D) != 0 ||
		*(signed char*)(raw + 0xE2E) != 0;
	if (!axesActive)
	{
		gSpideyChaseRampRemainder =
			0;
		return;
	}

	const int retailRamp =
		*(int*)(raw + 0x8F0);
	int expectedRetail =
		oldRamp + 0x20;
	if (expectedRetail > 0x100)
		expectedRetail = 0x100;

	if (oldRamp < 0 ||
		oldRamp > 0x100 ||
		retailRamp != expectedRetail)
	{
		++gSpideyChaseRampUnexpected;
		gSpideyChaseRampRemainder =
			0;
		return;
	}

	const int numerator =
		(0x20 * elapsedTicks) +
		gSpideyChaseRampRemainder;
	const int increment =
		numerator / 3;
	gSpideyChaseRampRemainder =
		numerator % 3;

	int correctedRamp =
		oldRamp + increment;
	if (correctedRamp > 0x100)
		correctedRamp = 0x100;
	*(int*)(raw + 0x8F0) =
		correctedRamp;
	++gSpideyChaseRampCorrections;
}

static void SpideyLogChaseSynthStats()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"chase_synth_20hz_stats synth_installed=%d synth_calls=%lu active_calls=%lu retail_updates=%lu held_calls=%lu max_elapsed=%lu type3_latch_calls=%lu type3_latch_writes=%lu type3_latch_dynamic=%lu type3_latch_directional=%lu heading_samples=%lu heading_corrections=%lu heading_max_pre_correction_drift=%lu building_entry_checks=%lu building_entry_blocked_matches=%lu building_entry_natural_seen=%lu building_entry_recovery_attempts=%lu building_entry_recovery_fires=%lu ramp_installed=%d ramp_calls=%lu ramp_corrections=%lu ramp_unexpected=%lu trace_samples=%lu trace_dropped=%lu level=0x501 render_physics=60hz synth_sample_hold=20hz cadence_ticks=3 building_entry_policy=recover_wait05_to_wait06_from_persistent_pulse_state_no_noclip_no_teleport\\n",
		gSpideyChaseSynthInstalled,
		gSpideyChaseSynthCalls,
		gSpideyChaseSynthActiveCalls,
		gSpideyChaseSynthRetailUpdates,
		gSpideyChaseSynthHeldCalls,
		gSpideyChaseSynthMaxElapsed,
		gSpideyChaseType3HeldLatchCalls,
		gSpideyChaseType3HeldLatchWrites,
		gSpideyChaseType3HeldLatchDynamic,
		gSpideyChaseType3HeldLatchDirectional,
		gSpideyChaseHeadingSamples,
		gSpideyChaseHeadingCorrections,
		gSpideyChaseHeadingMaxPreCorrectionDrift,
		gSpideyChaseBuildingEntryChecks,
		gSpideyChaseBuildingEntryBlockedMatches,
		gSpideyChaseBuildingEntryNaturalSeen,
		gSpideyChaseBuildingEntryRecoveryAttempts,
		gSpideyChaseBuildingEntryRecoveryFires,
		gSpideyChaseRampInstalled,
		gSpideyChaseRampCalls,
		gSpideyChaseRampCorrections,
		gSpideyChaseRampUnexpected,
		gSpideyChaseSynthTraceCount,
		gSpideyChaseSynthTraceDropped);
	fclose(f);
}

static void SpideyDumpChaseSynthTrace()
{
	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	for (unsigned long i = 0;
		 i < gSpideyChaseSynthTraceCount;
		 ++i)
	{
		const SpideyChaseSynthTraceSample* sample =
			&gSpideyChaseSynthTrace[i];
		fprintf(
			f,
			"chase_synth_trace i=%lu tick=%lu elapsed=%d field80=%d synth=%u script_active=%u script_clock=%d axes=%d,%d ramp=%d state=0x%08lX pos=%d,%d,%d angle_y=%d heading_valid=%d camera_heading=%d camera_mode=%d camera_interp=%d camera74_pulses_set=%u camera74_pulses=%u input_basis_e34=%d desired_relative_e32=%d desired_world=%d wall=%u ceiling=%u collision=0x%08lX ground_grace=%d worker_mask_before=0x%08lX worker_mask_after=0x%08lX head_before=%d,%d,%d,%d head_after=%d,%d,%d,%d\\n",
			i,
			sample->tick,
			sample->elapsed,
			sample->field80Before,
			sample->synthMode,
			sample->scriptActive,
			sample->scriptClock,
			sample->axisX,
			sample->axisY,
			sample->ramp,
			sample->state,
			sample->posX,
			sample->posY,
			sample->posZ,
			sample->angleY,
			sample->headingTraceValid,
			sample->cameraHeading,
			sample->cameraMode,
			sample->cameraInterpTicks,
			sample->cameraNextShotPulsesSet,
			sample->cameraNextShotPulses,
			sample->inputBasisHeading,
			sample->desiredRelativeHeading,
			sample->desiredWorldHeading,
			sample->wall,
			sample->ceiling,
			sample->collision,
			sample->groundGrace,
			sample->workerMaskBefore,
			sample->workerMaskAfter,
			sample->headBeforeType,
			sample->headBeforeSize,
			sample->headBefore2,
			sample->headBefore3,
			sample->headAfterType,
			sample->headAfterSize,
			sample->headAfter2,
			sample->headAfter3);
	}

	fclose(f);
}

static int SpideyInstallChaseSynth20HzCompat()
{
	const int synthInstalled =
		SpideyPatchDirectCall(
			0x004BD572,
			0x004BC300,
			(void*)&SpideyChaseVenomSynth20Hz,
			"chase_venom_synth_sample_hold");
	const int rampInstalled =
		SpideyPatchDirectCallsToTargetInRange(
			0x00401000,
			0x0053B000,
			0x004BD510,
			(void*)&SpideyChaseVenomReadAnalogue20HzRamp,
			"chase_venom_read_analogue_ramp");

	gSpideyChaseSynthInstalled =
		synthInstalled;
	gSpideyChaseRampInstalled =
		rampInstalled;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"chase_synth_20hz_install synth=%d synth_call=0x004BD572 synth_retail=0x004BC300 ramp_calls=%d ramp_retail=0x004BD510 level=0x501 policy=60hz_render_physics_20hz_scripted_control_sample_hold_camera_compensated_world_heading cadence_ticks=3 manual_input=untouched\\n",
			synthInstalled,
			rampInstalled);
		fclose(f);
	}

	return
		synthInstalled &&
		rampInstalled > 0;
}

typedef CBody* (__cdecl *SpideyRetailTrigCreateObjectFn)(
		i32);
typedef void (__fastcall *SpideyRetailScriptOnlyAIFn)(
		CScriptOnlyBaddy*,
		void*);
typedef i16* (__fastcall *SpideyRetailSwitchSynthInputFn)(
		CPlayer*,
		void*,
		i16*);

static unsigned long gSpideyL1A3CreateTraceCalls = 0;
static unsigned long gSpideyL1A3ScriptOnlyAiCalls = 0;
static unsigned long gSpideyL1A3ScriptOnlyAiStored = 0;
static unsigned long gSpideyL1A3SynthSwitchCalls = 0;
static int gSpideyL1A3ScriptOnlyInstalled = 0;

static int SpideyIsL1A3StartupControllerNode(
		int node)
{
	return node == 66 ||
		node == 132 ||
		node == 134 ||
		node == 165 ||
		node == 214 ||
		node == 395;
}

static CBody* __cdecl SpideyTraceL1A3TrigCreateObject(
		i32 nodeIndex)
{
	SpideyRetailTrigCreateObjectFn retail =
		(SpideyRetailTrigCreateObjectFn)0x004DEE70;

	CBody* result =
		retail(
			nodeIndex);

	if (SpideyRetailGetLevelId() == 0x103 &&
		(SpideyIsL1A3StartupControllerNode(
			nodeIndex) ||
		 nodeIndex == 301))
	{
		++gSpideyL1A3CreateTraceCalls;

		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"l1a3_startup event=create_object call=%lu tick=%ld node=%d result=0x%08lX item_type=%d expected_role=%s\n",
				gSpideyL1A3CreateTraceCalls,
				(long)*(volatile long*)0x006B4CA8,
				nodeIndex,
				(unsigned long)result,
				result ?
					(int)*(i16*)(
						(unsigned char*)result +
						0x38) :
					-1,
				nodeIndex == 301 ?
					"powerup" :
					"scriptonly_controller");
			fclose(f);
		}
	}

	return result;
}

static void __fastcall SpideyTraceL1A3ScriptOnlyAI(
		CScriptOnlyBaddy* baddy,
		void*)
{
	SpideyRetailScriptOnlyAIFn retail =
		(SpideyRetailScriptOnlyAIFn)0x00407840;

	++gSpideyL1A3ScriptOnlyAiCalls;

	if (!baddy ||
		SpideyRetailGetLevelId() != 0x103)
	{
		retail(
			baddy,
			0);
		return;
	}

	int node = -1;
	int active = 0;
	int delay = 0;
	int special = 0;
	int raw238 = 0;
	unsigned int command = 0xFFFF;
	i16* script = 0;

	__try
	{
		unsigned char* raw =
			(unsigned char*)baddy;
		node =
			(int)*(i16*)(raw + 0x0DE);
		active =
			(int)*(unsigned char*)(raw + 0x20C);
		delay =
			*(i32*)(raw + 0x230);
		special =
			(int)*(unsigned char*)(raw + 0x234);
		raw238 =
			*(i32*)(raw + 0x238);
		script =
			*(i16**)(raw + 0x24C);
		if (script)
		{
			command =
				(unsigned int)(
					*(u16*)script);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		node = -1;
	}

	if (SpideyIsL1A3StartupControllerNode(
			node) &&
		gSpideyL1A3ScriptOnlyAiStored < 512)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"l1a3_startup event=scriptonly_ai phase=pre call=%lu sample=%lu tick=%ld node=%d this=0x%08lX field80=%ld active=%d delay=%d special=%d raw238=%d script=0x%08lX command=0x%04X\n",
				gSpideyL1A3ScriptOnlyAiCalls,
				gSpideyL1A3ScriptOnlyAiStored,
				(long)*(volatile long*)0x006B4CA8,
				node,
				(unsigned long)baddy,
				(long)baddy->field_80,
				active,
				delay,
				special,
				raw238,
				(unsigned long)script,
				command);
			fclose(f);
			++gSpideyL1A3ScriptOnlyAiStored;
		}
	}

	// Do not inspect baddy after retail AI returns. Terminal 0x4100 can mark
	// the controller dead and its lifetime is owned by the retail list code.
	retail(
		baddy,
		0);
}

static i16* __fastcall SpideyTraceL1A3SwitchToSynthesizedInput(
		CPlayer* player,
		void*,
		i16* program)
{
	SpideyRetailSwitchSynthInputFn retail =
		(SpideyRetailSwitchSynthInputFn)0x004BC1A0;

	++gSpideyL1A3SynthSwitchCalls;

	int words[16];
	for (int i = 0;
		 i < 16;
		 ++i)
	{
		words[i] =
			-1;
	}

	if (program)
	{
		__try
		{
			for (int i = 0;
				 i < 16;
				 ++i)
			{
				words[i] =
					(int)(u16)program[i];
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	const int isL1A3 =
		SpideyRetailGetLevelId() == 0x103;

	if (isL1A3)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"l1a3_startup event=synth_switch phase=pre call=%lu tick=%ld player=0x%08lX program=0x%08lX words=%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X state=0x%08lX active=%d clock=%d parse=%d workers=0x%08lX aim=%u crawl=%u\n",
				gSpideyL1A3SynthSwitchCalls,
				(long)*(volatile long*)0x006B4CA8,
				(unsigned long)player,
				(unsigned long)program,
				words[0] & 0xFFFF,
				words[1] & 0xFFFF,
				words[2] & 0xFFFF,
				words[3] & 0xFFFF,
				words[4] & 0xFFFF,
				words[5] & 0xFFFF,
				words[6] & 0xFFFF,
				words[7] & 0xFFFF,
				words[8] & 0xFFFF,
				words[9] & 0xFFFF,
				words[10] & 0xFFFF,
				words[11] & 0xFFFF,
				words[12] & 0xFFFF,
				words[13] & 0xFFFF,
				words[14] & 0xFFFF,
				words[15] & 0xFFFF,
				player ?
					(unsigned long)player->field_E1C :
					0,
				player ?
					(int)player->field_1AC :
					-1,
				player ?
					*(i32*)((unsigned char*)player + 0x1B0) :
					-1,
				player ?
					(int)*(unsigned char*)((unsigned char*)player + 0x1B4) :
					-1,
				player ?
					(unsigned long)player->field_1BC :
					0,
				player ?
					(unsigned int)player->field_8EA :
					0,
				player ?
					(unsigned int)player->field_AD4 :
					0);
			fclose(f);
		}
	}

	i16* result =
		retail(
			player,
			0,
			program);

	if (isL1A3 &&
		player)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"l1a3_startup event=synth_switch phase=post call=%lu tick=%ld result=0x%08lX state=0x%08lX active=%d clock=%d parse=%d script=0x%08lX workers=0x%08lX aim=%u crawl=%u\n",
				gSpideyL1A3SynthSwitchCalls,
				(long)*(volatile long*)0x006B4CA8,
				(unsigned long)result,
				(unsigned long)player->field_E1C,
				(int)player->field_1AC,
				*(i32*)((unsigned char*)player + 0x1B0),
				(int)*(unsigned char*)((unsigned char*)player + 0x1B4),
				(unsigned long)*(i16**)((unsigned char*)player + 0x1B8),
				(unsigned long)player->field_1BC,
				(unsigned int)player->field_8EA,
				(unsigned int)player->field_AD4);
			fclose(f);
		}
	}

	return result;
}

static int SpideyInstallL1A3StartupTelemetry()
{
	const int createObjectInstalled =
		SpideyPatchDirectCall(
			0x004DFC7B,
			0x004DEE70,
			(void*)&SpideyTraceL1A3TrigCreateObject,
			"l1a3_startup_create_object_trace");

	const int synthSwitchInstalled =
		SpideyPatchDirectCall(
			0x004E1499,
			0x004BC1A0,
			(void*)&SpideyTraceL1A3SwitchToSynthesizedInput,
			"l1a3_startup_synth_switch_trace");

	void** vtable =
		(void**)0x0053B2E8;
	const unsigned long expectedAI =
		0x00407840;
	const unsigned long foundAI =
		(unsigned long)vtable[2];

	int scriptOnlyInstalled =
		0;
	if (foundAI ==
		expectedAI)
	{
		DWORD oldProtect =
			0;
		if (VirtualProtect(
				&vtable[2],
				sizeof(void*),
				PAGE_EXECUTE_READWRITE,
				&oldProtect))
		{
			vtable[2] =
				(void*)&SpideyTraceL1A3ScriptOnlyAI;

			DWORD ignoredProtect =
				0;
			VirtualProtect(
				&vtable[2],
				sizeof(void*),
				oldProtect,
				&ignoredProtect);
			FlushInstructionCache(
				GetCurrentProcess(),
				&vtable[2],
				sizeof(void*));
			scriptOnlyInstalled =
				1;
		}
	}

	gSpideyL1A3ScriptOnlyInstalled =
		scriptOnlyInstalled;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"l1a3_startup_trace_install create=%d create_call=0x004DFC7B create_retail=0x004DEE70 scriptonly=%d vtable=0x0053B2E8 ai_slot=2 ai_retail=0x%08lX ai_found=0x%08lX synth=%d synth_call=0x004E1499 synth_retail=0x004BC1A0 level=0x103 policy=retail_behavior_unchanged_trace_node214_to_node234_c7\n",
			createObjectInstalled,
			scriptOnlyInstalled,
			expectedAI,
			foundAI,
			synthSwitchInstalled);
		fclose(f);
	}

	return createObjectInstalled &&
		scriptOnlyInstalled &&
		synthSwitchInstalled;
}

typedef void (__fastcall *SpideyRetailScorpionAIFn)(
		CScorpion*,
		void*);

static unsigned long gSpideyScorpionAiCalls =
	0;
static unsigned long gSpideyScorpionAiLogged =
	0;
static int gSpideyScorpionAiInstalled =
	0;
static CScorpion* gSpideyScorpionAiLastObject =
	0;
static int gSpideyScorpionAiLastState =
	0x7FFFFFFF;
static int gSpideyScorpionAiLastAnim =
	-1;
static int gSpideyScorpionAiLastHealth =
	0x7FFFFFFF;

static void SpideyLogScorpionAiSnapshot(
		const char* phase,
		CScorpion* scorpion,
		unsigned long call,
		unsigned long tick)
{
	if (!scorpion)
		return;

	const int state =
		scorpion->field_31C.bothFlags;
	const int stateAux =
		scorpion->dumbAssPad;
	const int anim =
		(int)scorpion->mAnim;
	const int frame =
		(int)scorpion->mFrame;
	const int finished =
		(int)scorpion->mAnimFinished;
	const int health =
		(int)scorpion->mHealth;
	const int field80 =
		(int)scorpion->field_80;
	const int field1F8 =
		(int)scorpion->field_1F8;
	const int fieldBD8 =
		(int)scorpion->field_BD8;
	const int fieldBF8 =
		(int)scorpion->field_BF8;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	fprintf(
		f,
		"scorpion_ai phase=%s call=%lu tick=%lu this=0x%08lX state=0x%08X state_aux=0x%08X anim=%d frame=%d finished=%d health=%d field80=%d field1F8=%d fieldBD8=%d fieldBF8=%d target_handle=%08lX:%08lX pos=%ld,%ld,%ld\n",
		phase ? phase : "unknown",
		call,
		tick,
		(unsigned long)scorpion,
		(unsigned int)state,
		(unsigned int)stateAux,
		anim,
		frame,
		finished,
		health,
		field80,
		field1F8,
		fieldBD8,
		fieldBF8,
		(unsigned long)scorpion->hCurrentTarget.pWhatever,
		(unsigned long)scorpion->hCurrentTarget.Id,
		(long)scorpion->mPos.vx,
		(long)scorpion->mPos.vy,
		(long)scorpion->mPos.vz);
	fclose(f);
	++gSpideyScorpionAiLogged;
}

static void __fastcall SpideyScorpionAITelemetry(
		CScorpion* scorpion,
		void*)
{
	SpideyRetailScorpionAIFn retail =
		(SpideyRetailScorpionAIFn)0x00488590;

	++gSpideyScorpionAiCalls;
	const unsigned long call =
		gSpideyScorpionAiCalls;
	const unsigned long tick =
		(unsigned long)*(volatile long*)0x006B4CA8;

	if (!scorpion)
	{
		retail(
			scorpion,
			0);
		return;
	}

	const int stateBefore =
		scorpion->field_31C.bothFlags;
	const int animBefore =
		(int)scorpion->mAnim;
	const int healthBefore =
		(int)scorpion->mHealth;

	const int changedBefore =
		gSpideyScorpionAiLastObject !=
			scorpion ||
		gSpideyScorpionAiLastState !=
			stateBefore ||
		gSpideyScorpionAiLastAnim !=
			animBefore ||
		gSpideyScorpionAiLastHealth !=
			healthBefore;

	if (changedBefore ||
		(call % 30UL) == 0)
	{
		SpideyLogScorpionAiSnapshot(
			"pre",
			scorpion,
			call,
			tick);
	}

	gSpideyScorpionAiLastObject =
		scorpion;
	gSpideyScorpionAiLastState =
		stateBefore;
	gSpideyScorpionAiLastAnim =
		animBefore;
	gSpideyScorpionAiLastHealth =
		healthBefore;

	// Do not inspect the object after retail AI returns. Some death/teardown
	// paths may invalidate the object during AI; the next surviving AI call
	// will naturally expose any state transition without making telemetry a
	// lifetime hazard.
	retail(
		scorpion,
		0);
}

static int SpideyInstallScorpionAITelemetry()
{
	void** vtable =
		(void**)0x0053BE2C;
	const unsigned long expectedAI =
		0x00488590;
	const unsigned long foundAI =
		(unsigned long)vtable[2];

	int installed =
		0;

	if (foundAI ==
		expectedAI)
	{
		DWORD oldProtect =
			0;
		if (VirtualProtect(
				&vtable[2],
				sizeof(void*),
				PAGE_EXECUTE_READWRITE,
				&oldProtect))
		{
			vtable[2] =
				(void*)&SpideyScorpionAITelemetry;

			DWORD ignoredProtect =
				0;
			VirtualProtect(
				&vtable[2],
				sizeof(void*),
				oldProtect,
				&ignoredProtect);
			FlushInstructionCache(
				GetCurrentProcess(),
				&vtable[2],
				sizeof(void*));
			installed =
				1;
		}
	}

	gSpideyScorpionAiInstalled =
		installed;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"scorpion_ai_telemetry_install installed=%d vtable=0x0053BE2C slot=2 expected=0x%08lX found=0x%08lX wrapper=0x%08lX policy=retail_ai_unchanged_precall_transition_and_periodic_trace\n",
			installed,
			expectedAI,
			foundAI,
			(unsigned long)&SpideyScorpionAITelemetry);
		fclose(f);
	}

	return installed;
}

static void SpideyInstallHighFpsTimingCompat()
{
	void** mysterioLaserVtable =
		(void**)0x0053BB34;
	const unsigned long expectedDestructor =
		0x0045B540;
	const unsigned long expectedMove =
		0x0045BAC0;

	int mysterioLaserInstalled =
		0;
	const int l1a3StartupTelemetryInstalled =
		SpideyInstallL1A3StartupTelemetry();
	const int scorpionAiTelemetryInstalled =
		SpideyInstallScorpionAITelemetry();
	const int mysterioSoftSpotHitTelemetryInstalled =
		SpideyInstallMysterioSoftSpotHitTelemetry();
	const int mysterioYawTowardsInstalled =
		SpideyPatchDirectCall(
			0x00401528,
			0x004030C0,
			(void*)&SpideyMysterioYawTowards20Hz,
			"mysterio_fireboobies_lookat_yaw_20hz");
	gSpideyMysterioYawTowardsInstalled =
		mysterioYawTowardsInstalled;
	const int mysterioLaserAttackTelemetryInstalled =
		SpideyPatchDirectCall(
			0x0045F489,
			0x0045D200,
			(void*)&SpideyMysterioFireBoobiesTelemetry,
			"mysterio_laser_attack_telemetry");

	const int mysterioLaserSetPosLeftInstalled =
		SpideyPatchDirectCall(
			0x0045D3AB,
			0x0045B5E0,
			(void*)&SpideyMysterioLaserSetPos20Hz,
			"mysterio_laser_setpos_left_20hz");
	const int mysterioLaserSetPosRightInstalled =
		SpideyPatchDirectCall(
			0x0045D44E,
			0x0045B5E0,
			(void*)&SpideyMysterioLaserSetPos20Hz,
			"mysterio_laser_setpos_right_20hz");
	gSpideyMysterioLaserSetPosInstalled =
		mysterioLaserSetPosLeftInstalled &&
		mysterioLaserSetPosRightInstalled;
	const unsigned long foundDestructor =
		(unsigned long)mysterioLaserVtable[0];
	const unsigned long foundMove =
		(unsigned long)mysterioLaserVtable[1];

	if (foundDestructor ==
			expectedDestructor &&
		foundMove ==
			expectedMove)
	{
		DWORD oldProtect =
			0;
		if (VirtualProtect(
				&mysterioLaserVtable[1],
				sizeof(void*),
				PAGE_EXECUTE_READWRITE,
				&oldProtect))
		{
			mysterioLaserVtable[1] =
				(void*)&SpideyMysterioLaserMoveHighFps;

			DWORD ignoredProtect =
				0;
			VirtualProtect(
				&mysterioLaserVtable[1],
				sizeof(void*),
				oldProtect,
				&ignoredProtect);
			FlushInstructionCache(
				GetCurrentProcess(),
				&mysterioLaserVtable[1],
				sizeof(void*));

			mysterioLaserInstalled =
				1;
		}
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"high_fps_compat mysterio_laser=%d l1a3_startup_telemetry=%d scorpion_ai_telemetry=%d softspot_hit_telemetry=%d attack_telemetry=%d attack_call=0x0045F489 attack_retail=0x0045D200 vtable=0x0053BB34 destructor_expected=0x%08lX destructor_found=0x%08lX move_expected=0x%08lX move_found=0x%08lX clock=gTimerRelated_60hz grace_ticks=%lu grace_ms=50 marker_offset=0x44 policy=elapsed_tick_liveness_plus_setpos_20hz_sampling\n",
			mysterioLaserInstalled,
			l1a3StartupTelemetryInstalled,
			scorpionAiTelemetryInstalled,
			mysterioSoftSpotHitTelemetryInstalled,
			mysterioLaserAttackTelemetryInstalled,
			expectedDestructor,
			foundDestructor,
			expectedMove,
			foundMove,
			kSpideyMysterioLaserGraceTicks);
		fclose(f);
	}

	// One startup-only xref batch for the remaining Chase steering basis.
	// Retail type-2 synth uses camera+0x23A, ReadAnalogueInput writes E32
	// from E34, and CheckForwards consumes E32. Recover every E32/E34 and
	// camera-heading displacement in SpideyAI0 so the next runtime tells us
	// exactly who owns E34 and whether its update order is another 60-Hz
	// dependency. This is static byte scanning only; no per-frame I/O.
	SpideyLogRetailFieldXrefs(
		"SpideyAI0_E32",
		0x004B13F0,
		0x73A0,
		0x0E32);
	SpideyLogRetailFieldXrefs(
		"SpideyAI0_E34",
		0x004B13F0,
		0x73A0,
		0x0E34);
	SpideyLogRetailFieldXrefs(
		"SpideyAI0_CameraHeading23A",
		0x004B13F0,
		0x73A0,
		0x023A);

	SpideyInstallChasePlayerAI20HzCompat();
	SpideyInstallChaseCameraAI20HzCompat();
	SpideyInstallChaseBaddyAI20HzCompat();
	SpideyInstallChaseSynth20HzCompat();
}

static unsigned long gSpideyModernAimMovementCalls = 0;
static unsigned long gSpideyModernAimLookaroundCalls = 0;
static int gSpideyModernAimLastAxisX = 0x7FFFFFFF;
static int gSpideyModernAimLastAxisY = 0x7FFFFFFF;
static unsigned long gSpideyModernAimLastMoveState = 0xFFFFFFFFUL;
static int gSpideyModernAimLastMoveResult = -1;
static CVector gSpideyModernAimLastBodyPos;
static int gSpideyModernAimLastBodyPosValid = 0;

// Manual aim has to remain logically active for reticle/web/camera behavior,
// but runtime proves field_8EA also causes a later per-frame locomotion reset.
// While movement is actually held, mask field_8EA across the ordinary player
// locomotion state machine and keep an out-of-band "effective aim" state for
// our modern wrappers. Restore as soon as movement or the aim control is
// released.
static CPlayer* gSpideyModernAimLocomotionMaskedPlayer = 0;
static CPlayer* gSpideyModernAimZipReleaseLatchPlayer = 0;
static unsigned char gSpideyModernAimLocomotionSavedState = 0;
static unsigned long gSpideyModernAimLocomotionMaskCount = 0;
static unsigned long gSpideyModernAimLocomotionRestoreCount = 0;

static int SpideyModernAimIsEffectivelyActive(
		CPlayer* player)
{
	if (!player ||
		SpideyIsMysterioBossActive() ||
		gSpideyModernAimZipReleaseLatchPlayer ==
			player)
	{
		return 0;
	}

	if (player->field_8EA)
		return 1;

	return
		gSpideyModernAimLocomotionMaskedPlayer ==
			player &&
		gSpideyModernAimLocomotionSavedState != 0;
}

static void SpideyModernAimRestoreLocomotionState()
{
	CPlayer* player =
		gSpideyModernAimLocomotionMaskedPlayer;
	if (!player)
		return;

	__try
	{
		player->field_8EA =
			gSpideyModernAimLocomotionSavedState;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	gSpideyModernAimLocomotionMaskedPlayer =
		0;
	gSpideyModernAimLocomotionSavedState =
		0;
	++gSpideyModernAimLocomotionRestoreCount;
}

static void SpideyModernAimValidateLocomotionMaskAtFrameEnd()
{
	CPlayer* player =
		gSpideyModernAimLocomotionMaskedPlayer;
	if (!player)
		return;

	int keepMasked =
		0;

	__try
	{
		unsigned char* input =
			(unsigned char*)player->field_E0C;
		const int aimHeld =
			input &&
			input[0x40] != 0;
		const int movementHeld =
			player->field_E2D != 0 ||
			player->field_E2E != 0;
		CPlayer* currentPlayer =
			*(CPlayer**)0x006A9038;
		CCamera* camera =
			*(CCamera**)0x0056F3B8;
		const int ordinaryGameplayCamera =
			camera &&
			camera->mCameraMode ==
				CAMERAMODE_DEMO;

		keepMasked =
			aimHeld &&
			movementHeld &&
			currentPlayer == player &&
			ordinaryGameplayCamera;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		keepMasked =
			0;
	}

	if (!keepMasked)
	{
		SpideyModernAimRestoreLocomotionState();
	}
}

static void SpideyModernAimValidateZipReleaseLatchAtFrameEnd()
{
	CPlayer* player =
		gSpideyModernAimZipReleaseLatchPlayer;
	if (!player)
		return;

	int keepLatch =
		0;
	__try
	{
		unsigned char* input =
			(unsigned char*)player->field_E0C;
		CPlayer* currentPlayer =
			*(CPlayer**)0x006A9038;
		keepLatch =
			currentPlayer == player &&
			input &&
			input[0x40] != 0;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		keepLatch =
			0;
	}

	if (!keepLatch)
	{
		gSpideyModernAimZipReleaseLatchPlayer =
			0;
	}
}

static void SpideyModernAimDropForZip(
		CPlayer* player)
{
	if (!player)
		return;

	if (gSpideyModernAimLocomotionMaskedPlayer ==
		player)
	{
		// Do not call the normal restore helper here: that would restore the
		// saved retail aim flag to 1. A successful aimed zip deliberately
		// exits aim before entering the 0x40000 travel state.
		gSpideyModernAimLocomotionMaskedPlayer =
			0;
		gSpideyModernAimLocomotionSavedState =
			0;
		++gSpideyModernAimLocomotionRestoreCount;
	}

	player->field_8EA =
		0;
	player->field_DE4 =
		0;
	Screen_TargetOn(
		false);
	gSpideyModernAimZipReleaseLatchPlayer =
		player;
	gSpideyModernAimLastBodyPosValid =
		0;
}

typedef int (__fastcall *SpideyRetailCheckForwardsFn)(
		CPlayer*,
		void*,
		int);
typedef void (__fastcall *SpideyRetailSetupLookaroundCameraFn)(
		CPlayer*,
		void*);
typedef void (__fastcall *SpideyRetailEnterLookaroundModeFn)(
		CPlayer*,
		void*);

static unsigned long gSpideyModernAimEnterRetailCalls = 0;
static unsigned long gSpideyModernAimEnterSuppressedCalls = 0;
static unsigned long gSpideyModernAimRawFlagReclearCount = 0;

static void __fastcall SpideyModernAimEnterLookaroundMode(
		CPlayer* player,
		void*)
{
	SpideyRetailEnterLookaroundModeFn retail =
		(SpideyRetailEnterLookaroundModeFn)0x004C3580;

	if (player &&
		gSpideyModernAimZipReleaseLatchPlayer ==
			player)
	{
		// A camera-directed zip exits manual aim. Keep retail lookaround from
		// immediately re-entering while the physical Aim control is still
		// held; the frame-end latch clears as soon as Aim is released.
		++gSpideyModernAimEnterSuppressedCalls;
		return;
	}

	if (player &&
		gSpideyModernAimLocomotionMaskedPlayer ==
			player &&
		gSpideyModernAimLocomotionSavedState != 0)
	{
		// The raw aim flag is intentionally hidden from locomotion while the
		// modern sidecar continues to own effective manual aim. Do not let
		// held-input retail logic repeatedly re-enter lookaround mode: the
		// retail entry routine writes field_8EA=1 and reinitializes the stand
		// pose, which the 93d633 runtime exposed as the movement vibration.
		if (player->field_8EA)
		{
			player->field_8EA =
				0;
			++gSpideyModernAimRawFlagReclearCount;
		}

		++gSpideyModernAimEnterSuppressedCalls;
		return;
	}

	++gSpideyModernAimEnterRetailCalls;
	retail(
		player,
		0);
}

static int __fastcall SpideyModernAimCheckForwards(
		CPlayer* player,
		void*,
		int allowTurn)
{
	SpideyRetailCheckForwardsFn retail =
		(SpideyRetailCheckForwardsFn)0x004BF8A0;

	if (!player)
	{
		return retail(
			player,
			0,
			allowTurn);
	}

	unsigned char* input =
		(unsigned char*)player->field_E0C;
	unsigned char savedAimControl =
		0;
	int patchedAimControl =
		0;

	if (input)
	{
		__try
		{
			savedAimControl =
				input[0x40];
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			savedAimControl =
				0;
		}
	}

	const int effectiveAim =
		SpideyModernAimIsEffectivelyActive(
			player);

	if (!effectiveAim)
	{
		return retail(
			player,
			0,
			allowTurn);
	}

	const int movementHeld =
		player->field_E2D != 0 ||
		player->field_E2E != 0;

	if (movementHeld &&
		gSpideyModernAimLocomotionMaskedPlayer ==
			player &&
		gSpideyModernAimLocomotionSavedState != 0 &&
		player->field_8EA)
	{
		player->field_8EA =
			0;
		++gSpideyModernAimRawFlagReclearCount;
	}

	// If a prior movement mask is still active but either control was
	// released, restore retail manual-aim state before continuing so the
	// normal lookaround-exit path can observe it.
	if (gSpideyModernAimLocomotionMaskedPlayer ==
			player &&
		(!savedAimControl ||
		 !movementHeld))
	{
		SpideyModernAimRestoreLocomotionState();
	}

	// The canonical CheckForwards has an earlier held-aim-control gate:
	//   if (input[0x40] && (field_E1C & 1)) return 0;
	// Hide that button only while movement is evaluated.
	if (input)
	{
		__try
		{
			input[0x40] =
				0;
			patchedAimControl =
				1;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			patchedAimControl =
				0;
		}
	}

	// Runtime shows CheckForwards can enter run state 0x10 successfully, but
	// the following frame resets straight back to stand while field_8EA is
	// still visible to the normal locomotion state machine. Keep the retail
	// aim flag masked across frames only while aim + movement are held.
	if (savedAimControl &&
		movementHeld &&
		!gSpideyModernAimLocomotionMaskedPlayer)
	{
		gSpideyModernAimLocomotionSavedState =
			player->field_8EA ?
				player->field_8EA :
				1;
		player->field_8EA =
			0;
		gSpideyModernAimLocomotionMaskedPlayer =
			player;
		++gSpideyModernAimLocomotionMaskCount;
	}

	int result =
		0;

	__try
	{
		result =
			retail(
				player,
				0,
				allowTurn);
	}
	__finally
	{
		if (patchedAimControl)
		{
			input[0x40] =
				savedAimControl;
		}
	}

	++gSpideyModernAimMovementCalls;

	const int axisX =
		(int)player->field_E2D;
	const int axisY =
		(int)player->field_E2E;
	const unsigned long moveState =
		(unsigned long)player->field_E1C;
	const int movementChanged =
		axisX != gSpideyModernAimLastAxisX ||
		axisY != gSpideyModernAimLastAxisY ||
		moveState != gSpideyModernAimLastMoveState ||
		result != gSpideyModernAimLastMoveResult;

	if (gSpideyModernAimMovementCalls <= 6 ||
		movementChanged ||
		(gSpideyModernAimMovementCalls % 60) == 0)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_manual_aim event=movement call=%lu aim_control=%u axes=%d,%d state=0x%08lX result=%d allow_turn=%d body_pos=%d,%d,%d body_vel=%d,%d,%d anim=%u collision=0x%08lX aim_state=%d actual_aim_state=%u locomotion_mask=%d mask_count=%lu restore_count=%lu enter_retail=%lu enter_suppressed=%lu raw_reclear=%lu wall=%u ceiling=%u ignore_input=%d ground_grace=%d\n",
				gSpideyModernAimMovementCalls,
				(unsigned int)savedAimControl,
				axisX,
				axisY,
				moveState,
				result,
				allowTurn,
				player->mPos.vx,
				player->mPos.vy,
				player->mPos.vz,
				player->mVel.vx,
				player->mVel.vy,
				player->mVel.vz,
				(unsigned int)player->mAnim,
				(unsigned long)player->mCollision,
				SpideyModernAimIsEffectivelyActive(
					player),
				(unsigned int)player->field_8EA,
				gSpideyModernAimLocomotionMaskedPlayer ==
					player ? 1 : 0,
				gSpideyModernAimLocomotionMaskCount,
				gSpideyModernAimLocomotionRestoreCount,
				gSpideyModernAimEnterRetailCalls,
				gSpideyModernAimEnterSuppressedCalls,
				gSpideyModernAimRawFlagReclearCount,
				(unsigned int)player->field_8E8,
				(unsigned int)player->field_8E9,
				(int)player->field_E18,
				(int)player->field_EA4);
			fclose(f);
		}
	}

	gSpideyModernAimLastAxisX =
		axisX;
	gSpideyModernAimLastAxisY =
		axisY;
	gSpideyModernAimLastMoveState =
		moveState;
	gSpideyModernAimLastMoveResult =
		result;

	return result;
}

// Manual aim TPS framing. World +Y points downward in this game, so a
// negative Y offset places the camera/reticle focus above Spider-Man.
// 96 world units is intentionally modest: enough to put the reticle over the
// character instead of through his body without introducing a shoulder bias.
static const int kSpideyManualAimFocusHeightUnits = 96;
static const int kSpideyManualAimFocusHeight =
	kSpideyManualAimFocusHeightUnits *
	4096;

static CVector SpideyModernAimFramedFocus(
		CPlayer* player,
		CCamera* camera)
{
	CVector focus;

	if (player)
	{
		focus =
			player->mPos;
		focus.vy -=
			kSpideyManualAimFocusHeight;
	}
	else if (camera)
	{
		focus =
			camera->field_144;
	}
	else
	{
		focus.vx =
			0;
		focus.vy =
			0;
		focus.vz =
			0;
	}

	return focus;
}

static int SpideyModernAimApplyCameraPoint(
		CPlayer* player,
		CCamera* camera)
{
	if (!player ||
		!camera ||
		!SpideyModernAimIsEffectivelyActive(
			player) ||
		camera->mCameraMode !=
			CAMERAMODE_DEMO)
	{
		return 0;
	}

	// Use the same framed target as the final camera orientation: a point
	// slightly above Spider-Man. This makes the projected reticle live above
	// the character while mouse/right-stick still rotate the real orbit camera.
	const CVector framedFocus =
		SpideyModernAimFramedFocus(
			player,
			camera);
	const int dx =
		framedFocus.vx -
		camera->mPos.vx;
	const int dy =
		framedFocus.vy -
		camera->mPos.vy;
	const int dz =
		framedFocus.vz -
		camera->mPos.vz;

	if (!dx &&
		!dy &&
		!dz)
	{
		return 0;
	}

	const int rayScale =
		8;

	// Hip-fire is now runtime-validated using this exact unmodified visible
	// camera ray. Keep manual aim on the same ray too. The previous third-pass
	// X/Y reflection while leaving Z forward created a non-collinear point,
	// which explains the inverted/off-screen reticle behavior seen at runtime.
	player->field_DC0.vx =
		camera->mPos.vx +
		dx * rayScale;
	player->field_DC0.vy =
		camera->mPos.vy +
		dy * rayScale;
	player->field_DC0.vz =
		camera->mPos.vz +
		dz * rayScale;
	player->field_DE4 =
		1;

	return 1;
}

static void __fastcall SpideyModernAimSetupLookaroundCamera(
		CPlayer* player,
		void*)
{
	SpideyRetailSetupLookaroundCameraFn retail =
		(SpideyRetailSetupLookaroundCameraFn)0x004C38A0;

	if (!player)
	{
		retail(
			player,
			0);
		return;
	}

	CCamera* camera =
		*(CCamera**)0x0056F3B8;
	const int modernAim =
		SpideyModernAimIsEffectivelyActive(
			player) &&
		camera &&
		camera->mCameraMode ==
			CAMERAMODE_DEMO;

	if (!modernAim)
	{
		retail(
			player,
			0);
		return;
	}

	// Do NOT run retail SetupLookaroundCamera in modern manual aim.
	//
	// Runtime proved that it still owns the legacy lookaround accumulators
	// and pose/joint steering: WASD continued moving the old cursor while
	// normal locomotion simultaneously tried to run, leaving Spider-Man
	// twisted in place. The modern path already owns the gameplay camera and
	// reticle world point, so running the legacy lookaround controller only
	// creates a second controller fighting movement.
	//
	// Keep field_8EA intact so enter/exit/fire code still sees manual aim.
	// field_DC0 + field_DE4 are the pieces RenderLookaroundReticle needs.
	const int applied =
		SpideyModernAimApplyCameraPoint(
			player,
			camera);

	++gSpideyModernAimLookaroundCalls;

	int bodyDeltaX =
		0;
	int bodyDeltaY =
		0;
	int bodyDeltaZ =
		0;
	if (gSpideyModernAimLastBodyPosValid)
	{
		bodyDeltaX =
			player->mPos.vx -
			gSpideyModernAimLastBodyPos.vx;
		bodyDeltaY =
			player->mPos.vy -
			gSpideyModernAimLastBodyPos.vy;
		bodyDeltaZ =
			player->mPos.vz -
			gSpideyModernAimLastBodyPos.vz;
	}

	gSpideyModernAimLastBodyPos =
		player->mPos;
	gSpideyModernAimLastBodyPosValid =
		1;

	const int movementHeld =
		player->field_E2D ||
		player->field_E2E;
	const CVector framedFocus =
		SpideyModernAimFramedFocus(
			player,
			camera);
	if (gSpideyModernAimLookaroundCalls <= 12 ||
		(gSpideyModernAimLookaroundCalls % 60) == 0 ||
		(movementHeld &&
		 (gSpideyModernAimLookaroundCalls % 30) == 0))
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_manual_aim event=reticle call=%lu applied=%d retail_setup=0 camera=0x%08lX mode=%d axes=%d,%d aim_point=%d,%d,%d camera_pos=%d,%d,%d camera_focus=%d,%d,%d framed_focus=%d,%d,%d framing_up_units=%d body_pos=%d,%d,%d body_delta=%d,%d,%d body_vel=%d,%d,%d state=0x%08lX anim=%u collision=0x%08lX aim_state=%d actual_aim_state=%u locomotion_mask=%d wall=%u ceiling=%u ignore_input=%d ground_grace=%d\n",
				gSpideyModernAimLookaroundCalls,
				applied,
				(unsigned long)camera,
				(int)camera->mCameraMode,
				(int)player->field_E2D,
				(int)player->field_E2E,
				player->field_DC0.vx,
				player->field_DC0.vy,
				player->field_DC0.vz,
				camera->mPos.vx,
				camera->mPos.vy,
				camera->mPos.vz,
				camera->field_144.vx,
				camera->field_144.vy,
				camera->field_144.vz,
				framedFocus.vx,
				framedFocus.vy,
				framedFocus.vz,
				kSpideyManualAimFocusHeightUnits,
				player->mPos.vx,
				player->mPos.vy,
				player->mPos.vz,
				bodyDeltaX,
				bodyDeltaY,
				bodyDeltaZ,
				player->mVel.vx,
				player->mVel.vy,
				player->mVel.vz,
				(unsigned long)player->field_E1C,
				(unsigned int)player->mAnim,
				(unsigned long)player->mCollision,
				SpideyModernAimIsEffectivelyActive(
					player),
				(unsigned int)player->field_8EA,
				gSpideyModernAimLocomotionMaskedPlayer ==
					player ? 1 : 0,
				(unsigned int)player->field_8E8,
				(unsigned int)player->field_8E9,
				(int)player->field_E18,
				(int)player->field_EA4);
			fclose(f);
		}
	}
}

static void SpideyInstallModernManualAimCompat()
{
	static const unsigned char enterExpected[2] =
	{
		0x6A,
		0x07
	};
	static const unsigned char enterReplacement[2] =
	{
		0x6A,
		0x03
	};

	// CPlayer::CheckForwards:
	//   cmp byte ptr [esi+0x8EA], 0
	//   jne 0x004BFA0A
	static const unsigned char forwardsExpected[6] =
	{
		0x0F,
		0x85,
		0x3F,
		0x01,
		0x00,
		0x00
	};
	static const unsigned char forwardsReplacement[6] =
	{
		0x90,
		0x90,
		0x90,
		0x90,
		0x90,
		0x90
	};

	const int cameraInstalled =
		SpideyPatchBytes(
			0x004C370B,
			enterExpected,
			enterReplacement,
			sizeof(enterExpected),
			"modern_manual_aim_camera_mode");
	const int movementAimGateInstalled =
		SpideyPatchBytes(
			0x004BF8C5,
			forwardsExpected,
			forwardsReplacement,
			sizeof(forwardsExpected),
			"modern_manual_aim_check_forwards");
	const int movementControlInstalled =
		SpideyPatchDirectCall(
			0x004B231A,
			0x004BF8A0,
			(void*)&SpideyModernAimCheckForwards,
			"modern_manual_aim_movement_control");
	const int reticleInstalled =
		SpideyPatchDirectCall(
			0x004B8673,
			0x004C38A0,
			(void*)&SpideyModernAimSetupLookaroundCamera,
			"modern_manual_aim_reticle");
	const int enterReentryCallsInstalled =
		SpideyPatchDirectCallsToTargetInRange(
			0x004B0000,
			0x004B9000,
			0x004C3580,
			(void*)&SpideyModernAimEnterLookaroundMode,
			"modern_manual_aim_enter_guard");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"CAMERA");
	if (f)
	{
		fprintf(
			f,
				"modern_manual_aim_install camera_mode=%d enter_mode_site=0x004C370B retail_mode=7 modern_mode=3 movement_aim_gate=%d movement_control=%d movement_call=0x004B231A reticle=%d reticle_call=0x004B8673 enter_reentry_calls=%d enter_target=0x004C3580 aim_control=input_plus_0x40 movement_axes=E2D_E2E reticle_source=framed_tps_camera_ray locomotion_mask=field_8EA_while_aim_plus_move effective_aim_sidecar=1 frame_end_release_guard=1\n",
				cameraInstalled,
				movementAimGateInstalled,
				movementControlInstalled,
				reticleInstalled,
				enterReentryCallsInstalled);
		fclose(f);
	}
}

static char gSpideyAudioOutputMenuLabel[160];

typedef void (__cdecl *SpideyRetailShutdownDirectSoundFn)(void);
typedef void (__cdecl *SpideyRetailDxSoundInitFn)(void);
typedef void (__cdecl *SpideyRetailSpoolMenuSfxFn)(const char*);
typedef void (__fastcall *SpideyRetailMenuUpdateFn)(
		CMenu*,
		void*);
typedef u8 (__cdecl *SpideyRetailShellCheckTriggersFn)(
		u32,
		i32,
		i32);

static void SpideyCompactAudioDeviceName(
		const char* source,
		char* destination,
		int destinationSize)
{
	if (!destination ||
		destinationSize <= 0)
	{
		return;
	}

	destination[0] =
		0;

	if (!source ||
		!source[0])
	{
		source =
			"System Default";
	}

	char normalized[96];
	normalized[0] =
		0;

	const char* inner =
		source;
	int sourceLength =
		(int)strlen(source);

	if (sourceLength > 10 &&
		!strncmp(
			source,
			"Speakers (",
			10) &&
		source[sourceLength - 1] == ')')
	{
		int innerLength =
			sourceLength - 11;
		if (innerLength >
			(int)sizeof(normalized) - 1)
		{
			innerLength =
				(int)sizeof(normalized) - 1;
		}

		memcpy(
			normalized,
			source + 10,
			innerLength);
		normalized[innerLength] =
			0;
		inner =
			normalized;
	}
	else if (sourceLength > 18 &&
			 !strncmp(
				source,
				"Virtual Speakers (",
				18) &&
			 source[sourceLength - 1] == ')')
	{
		int innerLength =
			sourceLength - 19;
		if (innerLength >
			(int)sizeof(normalized) - 1)
		{
			innerLength =
				(int)sizeof(normalized) - 1;
		}

		memcpy(
			normalized,
			source + 18,
			innerLength);
		normalized[innerLength] =
			0;
		inner =
			normalized;
	}

	const int maxVisible =
		20;
	const int innerLength =
		(int)strlen(inner);

	if (innerLength <= maxVisible)
	{
		strncpy(
			destination,
			inner,
			destinationSize - 1);
		destination[destinationSize - 1] =
			0;
		return;
	}

	const int headLength =
		10;
	const int tailLength =
		7;

	if (destinationSize <
		headLength +
		3 +
		tailLength +
		1)
	{
		strncpy(
			destination,
			inner,
			destinationSize - 1);
		destination[destinationSize - 1] =
			0;
		return;
	}

	memcpy(
		destination,
		inner,
		headLength);
	memcpy(
		destination + headLength,
		"...",
		3);
	memcpy(
		destination + headLength + 3,
		inner + innerLength - tailLength,
		tailLength);
	destination[
		headLength +
		3 +
		tailLength] =
		0;
}

static void SpideyUpdateAudioOutputMenuLabel()
{
	const SpideyAudioDeviceInfo* selected =
		SpideyGetSelectedAudioDevice();
	const char* name =
		selected && selected->name[0] ?
			selected->name :
			"(System Default)";

	char compactName[48];
	SpideyCompactAudioDeviceName(
		name,
		compactName,
		sizeof(compactName));

	sprintf(
		gSpideyAudioOutputMenuLabel,
		"Output: %s",
		compactName);
}

static int SpideyCreateSelectedDirectSound(
		LPDIRECTSOUND8* outDirectSound)
{
	if (!outDirectSound)
		return 0;

	*outDirectSound =
		0;

	LPDIRECTSOUND8 directSound =
		0;
	HRESULT hr =
		SpideyCompatDirectSoundCreate8(
			0,
			&directSound,
			0);

	if (FAILED(hr) ||
		!directSound)
	{
		return 0;
	}

	HWND hwnd =
		*(HWND*)0x006B58D0;
	hr =
		directSound->SetCooperativeLevel(
			hwnd,
			DSSCL_EXCLUSIVE);

	if (FAILED(hr))
	{
		directSound->Release();
		return 0;
	}

	DSCAPS* caps =
		(DSCAPS*)0x006B5870;
	memset(
		caps,
		0,
		sizeof(*caps));
	caps->dwSize =
		sizeof(*caps);

	hr =
		directSound->GetCaps(
			caps);
	if (FAILED(hr))
	{
		directSound->Release();
		memset(
			caps,
			0,
			sizeof(*caps));
		return 0;
	}

	*outDirectSound =
		directSound;
	return 1;
}

static void SpideyTryRebindBinkAudio(
		const char* reason)
{
	if (!gSpideyRetainedBinkDirectSound)
		return;

	LPDIRECTSOUND8 currentDirectSound =
		*(LPDIRECTSOUND8*)0x006B7920;
	if (!currentDirectSound)
		return;

	void* activeBink =
		*(void**)0x00AC0BA4;
	if (activeBink)
		return;

	// PCMOVIE_Init only binds Bink's sound system once. No Bink movie is
	// active now, so clear the one-shot flag and let retail bind the current
	// G_PDS safely before releasing the old retained DirectSound object.
	*(unsigned char*)0x00AC0BA0 =
		0;

	typedef void (__cdecl *SpideyRetailPCMovieInitFn)(void);
	SpideyRetailPCMovieInitFn movieInit =
		(SpideyRetailPCMovieInitFn)0x0050B0F0;
	movieInit();

	gSpideyRetainedBinkDirectSound->Release();
	gSpideyRetainedBinkDirectSound =
		0;

	FILE* f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		fprintf(
			f,
			"audio_bink_rebind reason=%s runtime_directsound=0x%08lX bink_inited=%u bink_handle=0x%08lX retained_released=1\n",
			reason ? reason : "unknown",
			(unsigned long)*(LPDIRECTSOUND8*)0x006B7920,
			(unsigned int)*(unsigned char*)0x00AC0BA0,
			(unsigned long)*(void**)0x00AC0BA4);
		fclose(f);
	}
}

static int SpideyRestartDirectSoundForShell(
		int selectedIndex,
		const char* reason)
{
	if (selectedIndex < 0 ||
		selectedIndex >=
			gSpideyAudioDeviceCount)
	{
		selectedIndex =
			0;
	}

	const int previousSelectedAudioDevice =
		gSpideySelectedAudioDevice;
	gSpideySelectedAudioDevice =
		selectedIndex;

	LPDIRECTSOUND8* directSoundSlot =
		(LPDIRECTSOUND8*)0x006B7920;
	LPDIRECTSOUND8 previousDirectSound =
		*directSoundSlot;

	// Keep a temporary rollback reference regardless of Bink state. Retail
	// shutdown releases the game's ownership, but a failed new endpoint
	// create must be able to restore the previous working device.
	if (previousDirectSound)
		previousDirectSound->AddRef();

	// If Bink has already been initialized, it may still hold the current
	// DirectSound object as a raw backend pointer. Retain that object before
	// retail releases its game-owned reference so live endpoint switching
	// cannot invalidate Bink underneath an active movie.
	if (*directSoundSlot &&
		*(unsigned char*)0x00AC0BA0 &&
		!gSpideyRetainedBinkDirectSound)
	{
		(*directSoundSlot)->AddRef();
		gSpideyRetainedBinkDirectSound =
			*directSoundSlot;
	}

	if (*directSoundSlot)
	{
		SpideyRetailShutdownDirectSoundFn shutdownSound =
			(SpideyRetailShutdownDirectSoundFn)0x005000F0;
		shutdownSound();
	}

	LPDIRECTSOUND8 directSound =
		0;
	int created =
		SpideyCreateSelectedDirectSound(
			&directSound);
	int fellBack =
		0;

	if (!created &&
		gSpideySelectedAudioDevice != 0)
	{
		gSpideySelectedAudioDevice =
			0;
		fellBack =
			1;
		created =
			SpideyCreateSelectedDirectSound(
				&directSound);
	}

	int restoredPrevious =
		0;

	if (!created &&
		previousDirectSound)
	{
		gSpideySelectedAudioDevice =
			previousSelectedAudioDevice;
		previousDirectSound->AddRef();
		directSound =
			previousDirectSound;
		created =
			1;
		restoredPrevious =
			1;
	}

	if (created &&
		directSound)
	{
		*directSoundSlot =
			directSound;

		SpideyRetailDxSoundInitFn soundInit =
			(SpideyRetailDxSoundInitFn)0x005039F0;
		SpideyRetailSpoolMenuSfxFn spoolMenu =
			(SpideyRetailSpoolMenuSfxFn)0x004719B0;

		soundInit();
		spoolMenu(
			"menu");
	}

	if (previousDirectSound)
	{
		previousDirectSound->Release();
		previousDirectSound =
			0;
	}

	SpideySaveAudioSettings();
	SpideyUpdateAudioOutputMenuLabel();
	SpideyTryRebindBinkAudio(
		"device_switch");

	FILE* f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		const SpideyAudioDeviceInfo* selected =
			SpideyGetSelectedAudioDevice();
		fprintf(
			f,
			"audio_restart reason=%s requested_index=%d selected_index=%d name=%s created=%d fallback_default=%d restored_previous=%d shell_only=1 retained_bink_ds=0x%08lX active_bink=0x%08lX apply=live\n",
			reason ? reason : "unknown",
			selectedIndex,
			gSpideySelectedAudioDevice,
			selected ? selected->name : "(System Default)",
			created,
			fellBack,
			restoredPrevious,
			(unsigned long)gSpideyRetainedBinkDirectSound,
			(unsigned long)*(void**)0x00AC0BA4);
		fclose(f);
	}

	return created;
}

static void __fastcall SpideyAudioAddOutputDevice(
		CMenu* menu,
		void*,
		const char* fifthLabel)
{
	SpideyRetailMenuAddEntryFn retailAdd =
		(SpideyRetailMenuAddEntryFn)0x0043FFF0;

	// Refresh on every Audio-menu open so devices connected since launch are
	// visible. Re-resolve the persisted GUID against the new enumeration.
	SpideyRefreshAudioDevices();
	gSpideyAudioSettingsLoaded =
		0;
	SpideyLoadAudioSettings();
	SpideyUpdateAudioOutputMenuLabel();

	retailAdd(
		menu,
		0,
		fifthLabel);
	retailAdd(
		menu,
		0,
		gSpideyAudioOutputMenuLabel);

	// The retail screen was authored for five rows. Keep all six rows within
	// the same vertical composition by shifting the CMenu text one line up.
	// Matching wrappers below shift slider graphics AND slider hit regions by
	// the exact same amount, so all Audio controls remain aligned.
	if (menu &&
		menu->mNumLines >= 6)
	{
		menu->mY -=
			menu->mLineSep;
	}
}

typedef void (__cdecl *SpideyRetailDrawSliderFn)(
		int,
		int,
		int,
		int);
typedef int (__cdecl *SpideyRetailSliderMouseLogicFn)(
		int,
		int,
		int);

static const int gSpideyAudioExtraRowShift =
	20;

static void __cdecl SpideyAudioDrawSliderShifted(
		int x,
		int y,
		int selected,
		int value)
{
	SpideyRetailDrawSliderFn retail =
		(SpideyRetailDrawSliderFn)0x00498060;
	retail(
		x,
		y - gSpideyAudioExtraRowShift,
		selected,
		value);
}

static int __cdecl SpideyAudioSliderMouseLogicShifted(
		int x,
		int y,
		int value)
{
	SpideyRetailSliderMouseLogicFn retail =
		(SpideyRetailSliderMouseLogicFn)0x00497F80;
	return retail(
		x,
		y - gSpideyAudioExtraRowShift,
		value);
}

typedef int (__cdecl *SpideyRetailMessDrawTextFn)(
		int,
		int,
		const char*,
		int,
		u32);

static int __cdecl SpideyAudioDrawStereoValueShifted(
		int x,
		int y,
		const char* text,
		int option,
		u32 flags)
{
	SpideyRetailMessDrawTextFn retail =
		(SpideyRetailMessDrawTextFn)0x00458700;
	return retail(
		x,
		y - gSpideyAudioExtraRowShift,
		text,
		option,
		flags);
}

static void __fastcall SpideyAudioMenuUpdate(
		CMenu* menu,
		void*)
{
	SpideyRetailMenuUpdateFn retailUpdate =
		(SpideyRetailMenuUpdateFn)0x00440600;
	retailUpdate(
		menu,
		0);

	if (!menu ||
		menu->mLine != 5 ||
		gSpideyAudioDeviceCount < 1)
	{
		return;
	}

	SpideyRetailShellCheckTriggersFn checkTriggers =
		(SpideyRetailShellCheckTriggersFn)0x0050C180;

	int delta =
		0;

	// These are the exact left/right trigger masks used by the retail
	// Shell_SFXMusic loop for its existing sliders.
	if (checkTriggers(
			0x00008008,
			0,
			0))
	{
		delta =
			1;
	}
	else if (checkTriggers(
			0x00004004,
			0,
			0))
	{
		delta =
			-1;
	}

	if (!delta)
		return;

	int next =
		gSpideySelectedAudioDevice +
		delta;

	if (next < 0)
		next =
			gSpideyAudioDeviceCount - 1;
	if (next >=
		gSpideyAudioDeviceCount)
	{
		next =
			0;
	}

	if (next ==
		gSpideySelectedAudioDevice)
	{
		return;
	}

	SpideyRestartDirectSoundForShell(
		next,
		delta > 0 ?
			"audio_menu_next" :
			"audio_menu_prev");
}

typedef void (__cdecl *SpideyRetailSetBootSoundModeFn)(
		bool);

static void __cdecl SpideyAudioSetStereoModeCompat(
		bool stereo)
{
	FILE* f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		fprintf(
			f,
			"audio_mode_change phase=before requested_stereo=%d boot_mode=%u runtime_directsound=0x%08lX bink_inited=%u bink_handle=0x%08lX\n",
			stereo ? 1 : 0,
			(unsigned int)*(unsigned char*)0x0061919D,
			(unsigned long)*(LPDIRECTSOUND8*)0x006B7920,
			(unsigned int)*(unsigned char*)0x00AC0BA0,
			(unsigned long)*(void**)0x00AC0BA4);
		fclose(f);
	}

	SpideyRetailSetBootSoundModeFn retail =
		(SpideyRetailSetBootSoundModeFn)0x00472AA0;
	retail(
		stereo);

	f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		fprintf(
			f,
			"audio_mode_change phase=after requested_stereo=%d boot_mode=%u runtime_directsound=0x%08lX bink_inited=%u bink_handle=0x%08lX\n",
			stereo ? 1 : 0,
			(unsigned int)*(unsigned char*)0x0061919D,
			(unsigned long)*(LPDIRECTSOUND8*)0x006B7920,
			(unsigned int)*(unsigned char*)0x00AC0BA0,
			(unsigned long)*(void**)0x00AC0BA4);
		fclose(f);
	}
}

static void SpideyInstallAudioMenuCompat()
{
	const int addEntryInstalled =
		SpideyPatchDirectCall(
			0x0049788B,
			0x0043FFF0,
			(void*)&SpideyAudioAddOutputDevice,
			"audio_output_entry");

	const int updateInstalled =
		SpideyPatchDirectCall(
			0x00497B57,
			0x00440600,
			(void*)&SpideyAudioMenuUpdate,
			"audio_output_update");

	const int stereoModeInstalled =
		SpideyPatchDirectCall(
			0x00497DD5,
			0x00472AA0,
			(void*)&SpideyAudioSetStereoModeCompat,
			"audio_stereo_mode_telemetry");

	const int sliderOneInstalled =
		SpideyPatchDirectCall(
			0x00497978,
			0x00498060,
			(void*)&SpideyAudioDrawSliderShifted,
			"audio_slider_1_shift");
	const int sliderTwoInstalled =
		SpideyPatchDirectCall(
			0x00497998,
			0x00498060,
			(void*)&SpideyAudioDrawSliderShifted,
			"audio_slider_2_shift");
	const int sliderThreeInstalled =
		SpideyPatchDirectCall(
			0x004979B8,
			0x00498060,
			(void*)&SpideyAudioDrawSliderShifted,
			"audio_slider_3_shift");
	const int sliderMouseInstalled =
		SpideyPatchDirectCall(
			0x00497BE9,
			0x00497F80,
			(void*)&SpideyAudioSliderMouseLogicShifted,
			"audio_slider_mouse_shift");
	const int stereoValueInstalled =
		SpideyPatchDirectCall(
			0x00497A1B,
			0x00458700,
			(void*)&SpideyAudioDrawStereoValueShifted,
			"audio_stereo_value_shift");

	FILE* f = SpideyOpenConsolidatedLog(
		"AUDIO");
	if (f)
	{
		fprintf(
			f,
			"audio_menu_mod retail=0x004977D0 rows=6 output_row=5 add_entry=%d update=%d stereo_telemetry=%d controls=retail_left_right device_apply=live retained_bink_backend=1 full_layout_shift=%d slider_draws=%d,%d,%d slider_mouse=%d stereo_value=%d compact_label=1\n",
			addEntryInstalled,
			updateInstalled,
			stereoModeInstalled,
			gSpideyAudioExtraRowShift,
			sliderOneInstalled,
			sliderTwoInstalled,
			sliderThreeInstalled,
			sliderMouseInstalled,
			stereoValueInstalled);
		fclose(f);
	}
}

static void SpideyInstallDisplayAspectCompat()
{
	SpideyLoadModernVideoSettings();

	int labelInstalled =
		0;

	const char** rowOneLabel =
		(const char**)0x0054BBD4;

	DWORD oldProtect =
		0;
	if (VirtualProtect(
			rowOneLabel,
			sizeof(*rowOneLabel),
			PAGE_READWRITE,
			&oldProtect))
	{
		*rowOneLabel =
			gSpideyAspectRatioMenuLabel;

		DWORD ignoredProtect =
			0;
		VirtualProtect(
			rowOneLabel,
			sizeof(*rowOneLabel),
			oldProtect,
			&ignoredProtect);

		labelInstalled =
			1;
	}

	const int resolutionFormatInstalled =
		SpideyPatchDirectCall(
			0x0050DB56,
			0x00529F90,
			(void*)&SpideyFormatPendingResolution,
			"resolution_format_pending");

	const int aspectFormatInstalled =
		SpideyPatchDirectCall(
			0x0050DBBB,
			0x00529F90,
			(void*)&SpideyFormatAspectRatioValue,
			"aspect_format_pending");

	const int aspectPrevInstalled =
		SpideyPatchDirectCall(
			0x0050DDAB,
			0x005010C0,
			(void*)&SpideyDisplayAspectPrev,
			"aspect_prev_pending");

	const int aspectNextInstalled =
		SpideyPatchDirectCall(
			0x0050DDCE,
			0x00501060,
			(void*)&SpideyDisplayAspectNext,
			"aspect_next_pending");

	const int aspectResolutionNextDisabled =
		SpideyPatchDirectCall(
			0x0050DDFB,
			0x00500E20,
			(void*)&SpideyDisplayAspectResolutionNoop,
			"aspect_resolution_next_disabled");

	const int aspectResolutionPrevDisabled =
		SpideyPatchDirectCall(
			0x0050DE1F,
			0x00500F40,
			(void*)&SpideyDisplayAspectResolutionNoop,
			"aspect_resolution_prev_disabled");

	const int resolutionPrevInstalled =
		SpideyPatchDirectCall(
			0x0050DE71,
			0x00500F40,
			(void*)&SpideyDisplayPendingPrevResolution,
			"resolution_prev_pending");

	const int resolutionNextInstalled =
		SpideyPatchDirectCall(
			0x0050DE88,
			0x00500E20,
			(void*)&SpideyDisplayPendingNextResolution,
			"resolution_next_pending");

	const int applyEntryInstalled =
		SpideyPatchDirectCall(
			0x0050DA72,
			0x0043FFF0,
			(void*)&SpideyDisplayAddBrightnessAndApply,
			"apply_entry");

	const int applyConfirmInstalled =
		SpideyPatchDirectCall(
			0x0050DCF8,
			0x00500250,
			(void*)&SpideyDisplayConfirmOrApply,
			"apply_confirm");

	const int menuUpdateInstalled =
		SpideyPatchDirectCall(
			0x0050DCA0,
			0x00440600,
			(void*)&SpideyDisplayMenuUpdate,
			"display_mode_update");

	const int menuDisplayInstalled =
		SpideyPatchDirectCall(
			0x0050DC26,
			0x004401B0,
			(void*)&SpideyDisplayMenuDisplay,
			"display_scale_slider_draw");

	const int pauseUiUpdateInstalled =
		SpideyPatchDirectCall(
			0x004415F8,
			0x00440600,
			(void*)&SpideyPauseMenuUpdate,
			"pause_ui_update");

	const int pauseConfirmInstalled =
		SpideyPatchDirectCall(
			0x00441606,
			0x0050C180,
			(void*)&SpideyPauseConfirmTrigger,
			"pause_display_confirm");

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_menu_mod retail=0x0050D9B0 rows=7 row1=Aspect_Ratio row3=Gameplay_UI_Scale row4=Menu_Text_Scale row5=Display_Mode row6=Apply label=%d resfmt=%d aspectfmt=%d aspectprev=%d aspectnext=%d compatnext=%d compatprev=%d resprev=%d resnext=%d applyentry=%d applyconfirm=%d modeupdate=%d scaledraw=%d pause_custom_options=1 pause_parent_rows_added=1 pause_parent_insert=before_last pause_submenu_rows=6 pause_retail_options_invoked=0 pause_update=%d pause_confirm=%d pause_keyboard_source=raw_directinput_dik_0x1c range=%d-%d step=%d defaults=%d,%d\n",
			labelInstalled,
			resolutionFormatInstalled,
			aspectFormatInstalled,
			aspectPrevInstalled,
			aspectNextInstalled,
			aspectResolutionNextDisabled,
			aspectResolutionPrevDisabled,
			resolutionPrevInstalled,
			resolutionNextInstalled,
			applyEntryInstalled,
			applyConfirmInstalled,
			menuUpdateInstalled,
			menuDisplayInstalled,
			pauseUiUpdateInstalled,
			pauseConfirmInstalled,
			kSpideyUiScaleMinPercent,
			kSpideyUiScaleMaxPercent,
			kSpideyUiScaleStepPercent,
			kSpideyDefaultGameplayUiScalePercent,
			kSpideyDefaultMenuTextScalePercent);
		fclose(f);
	}
}

static void SpideyRestoreSavedRenderResolution()
{
	// Load aspect + window-mode policy before the early DirectX/renderer
	// bootstrap so a persisted Windowed or Exclusive choice is not
	// overwritten by the old borderless-only startup behavior.
	SpideyLoadModernVideoSettings();

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

	SpideyApplySelectedAspect(
		"restore_saved_resolution");

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

	// The logical game/frontend viewport follows the selected output. Any
	// lower-resolution D3D7 surface is compatibility-producer plumbing only
	// and must not become the frontend layout coordinate basis.
	*(DWORD*)0x00568154 =
		requestedWidth;
	*(DWORD*)0x00568158 =
		requestedHeight;

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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
// Independent of DirectDraw mode/resolution requests. This mirrors whether
// the retail PShell frontend lifecycle is actually active.
static int gSpideyFrontendUiActive = 0;
static int gSpideyShadowPreviewEnabled = 1;
static unsigned long gSpideyModernLogicalWidth = 0;
static unsigned long gSpideyModernLogicalHeight = 0;
static unsigned long gSpideyLegacyPhysicalWidth = 640;
static unsigned long gSpideyLegacyPhysicalHeight = 480;

typedef void (__cdecl *SpideyRetailSetMouseBoundsFn)(
		i32,
		i32,
		i32,
		i32);
typedef void (__cdecl *SpideyRetailSetMousePositionFn)(
		i32,
		i32);
typedef void (__cdecl *SpideyRetailGetMousePositionFn)(
		i32*,
		i32*);

// @Ok
static int SpideyGetFrontendMouseDomains(
		int* pClientWidth,
		int* pClientHeight,
		int* pShellWidth,
		int* pShellHeight)
{
	int clientWidth =
		0;
	int clientHeight =
		0;

	HWND hwnd =
		*(HWND*)0x006B58D0;

	if (hwnd)
	{
		RECT client;
		if (GetClientRect(
				hwnd,
				&client))
		{
			clientWidth =
				client.right -
				client.left;
			clientHeight =
				client.bottom -
				client.top;
		}
	}

	if (clientWidth < 64 ||
		clientHeight < 64)
	{
		clientWidth =
			(int)gSpideySelectedOutputWidth;
		clientHeight =
			(int)gSpideySelectedOutputHeight;
	}

	if (clientWidth < 64 ||
		clientHeight < 64)
	{
		clientWidth =
			(int)gSpideyLegacyPhysicalWidth;
		clientHeight =
			(int)gSpideyLegacyPhysicalHeight;
	}

	// PCSHELL_CoordsPCtoDC divides PC mouse coordinates by the retail
	// DirectX canvas (gDxResolutionX/Y). In this executable those live at
	// the same width/height globals used by the D3D7 compatibility backing.
	// Therefore GetMousePosition must return this *retail PC canvas* domain,
	// not the modern DX11 logical canvas. Mapping to modern logical here and
	// then letting PCSHELL convert again caused the upper-left "invisible
	// box" (a second scale-down on both axes).
	int shellWidth =
		(int)*(DWORD*)0x006B78E4;
	int shellHeight =
		(int)*(DWORD*)0x006B78E8;

	if (shellWidth < 64 ||
		shellHeight < 64)
	{
		shellWidth =
			clientWidth;
		shellHeight =
			clientHeight;
	}

	if (clientWidth < 64 ||
		clientHeight < 64 ||
		shellWidth < 64 ||
		shellHeight < 64)
	{
		return 0;
	}

	if (pClientWidth)
		*pClientWidth =
			clientWidth;
	if (pClientHeight)
		*pClientHeight =
			clientHeight;
	if (pShellWidth)
		*pShellWidth =
			shellWidth;
	if (pShellHeight)
		*pShellHeight =
			shellHeight;

	return 1;
}

// @Ok
static void SpideyMapFrontendMouseToLogical(
		i32 rawX,
		i32 rawY,
		i32* pShellX,
		i32* pShellY)
{
	int clientWidth =
		0;
	int clientHeight =
		0;
	int shellWidth =
		0;
	int shellHeight =
		0;

	if (!SpideyGetFrontendMouseDomains(
			&clientWidth,
			&clientHeight,
			&shellWidth,
			&shellHeight))
	{
		if (pShellX)
			*pShellX =
				rawX;
		if (pShellY)
			*pShellY =
				rawY;
		return;
	}

	// Raw relative mouse state spans the whole HWND client. Convert exactly
	// once into the PC pixel canvas expected by PCSHELL_CoordsPCtoDC.
	if (pShellX)
	{
		*pShellX =
			(i32)(
				((long)rawX *
				 (long)shellWidth) /
				(long)clientWidth);
	}
	if (pShellY)
	{
		*pShellY =
			(i32)(
				((long)rawY *
				 (long)shellHeight) /
				(long)clientHeight);
	}
}

// @Ok
static void __cdecl SpideyCompatGetMousePosition(
		i32* pX,
		i32* pY)
{
	SpideyMapFrontendMouseToLogical(
		*(i32*)0x00AC0900,
		*(i32*)0x00AC0904,
		pX,
		pY);
}

// @Ok
static void SpideySyncFrontendMouseBounds(
		const char* reason)
{
	if (!gSpideyFrontendLegacyMode)
		return;

	int clientWidth =
		0;
	int clientHeight =
		0;
	int shellWidth =
		0;
	int shellHeight =
		0;

	if (!SpideyGetFrontendMouseDomains(
			&clientWidth,
			&clientHeight,
			&shellWidth,
			&shellHeight))
	{
		return;
	}

	i32 maxX =
		clientWidth - 32;
	i32 maxY =
		clientHeight - 32;

	if (maxX < 0)
		maxX = 0;
	if (maxY < 0)
		maxY = 0;

	SpideyRetailSetMouseBoundsFn retailSetBounds =
		(SpideyRetailSetMouseBoundsFn)0x0050A6B0;
	SpideyRetailSetMousePositionFn retailSetPosition =
		(SpideyRetailSetMousePositionFn)0x0050A700;

	// Work in the raw virtual-cursor domain here. The public GetMousePosition
	// hook maps this into the retail PC canvas expected by PCSHELL before its
	// normal PC->512x240 conversion for cursor drawing and hit testing.
	i32 mouseX =
		*(i32*)0x00AC0900;
	i32 mouseY =
		*(i32*)0x00AC0904;

	retailSetBounds(
		0,
		0,
		maxX,
		maxY);

	int recentered =
		0;

	if (mouseX < 0 ||
		mouseX > maxX ||
		mouseY < 0 ||
		mouseY > maxY)
	{
		mouseX =
			maxX / 2;
		mouseY =
			maxY / 2;
		recentered =
			1;
	}

	retailSetPosition(
		mouseX,
		mouseY);

	i32 mappedX =
		0;
	i32 mappedY =
		0;
	SpideyMapFrontendMouseToLogical(
		mouseX,
		mouseY,
		&mappedX,
		&mappedY);

	FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");
	if (f)
	{
		fprintf(
			f,
			"retail_input event=frontend_bounds_sync reason=%s client=%dx%d bounds=0,0,%d,%d raw_position=%d,%d shell_position=%d,%d recentered=%d shell_canvas=%dx%d modern_logical=%lux%lu basis=client_to_retail_pc_canvas\n",
			reason ? reason : "unknown",
			clientWidth,
			clientHeight,
			maxX,
			maxY,
			mouseX,
			mouseY,
			mappedX,
			mappedY,
			recentered,
			shellWidth,
			shellHeight,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight);
		fclose(f);
	}
}

// @Ok
static i32 SpideyMouseCanvasWidth()
{
	i32 width =
		*(i32*)0x006B78E4;
	if (width < 1)
		width = 640;
	return width;
}

// @Ok
static i32 SpideyMouseCanvasHeight()
{
	i32 height =
		*(i32*)0x006B78E8;
	if (height < 1)
		height = 480;
	return height;
}

// @Ok
static void __cdecl SpideyCompatGetMouseHotspotPosition(
		i32* pX,
		i32* pY)
{
	i32 shellX =
		0;
	i32 shellY =
		0;

	SpideyMapFrontendMouseToLogical(
		*(i32*)0x00AC0900,
		*(i32*)0x00AC0904,
		&shellX,
		&shellY);

	const i32 width =
		SpideyMouseCanvasWidth();
	const i32 height =
		SpideyMouseCanvasHeight();

	if (pX)
	{
		*pX =
			shellX +
			(*(i32*)0x00AC0A04 * width) /
				640;
	}

	if (pY)
	{
		*pY =
			shellY +
			(*(i32*)0x00AC0A08 * height) /
				480;
	}
}

// @Ok
static i32 __cdecl SpideyCompatIsMouseOver(
		i32 left,
		i32 top,
		i32 right,
		i32 bottom)
{
	i32 mouseX =
		0;
	i32 mouseY =
		0;

	SpideyCompatGetMouseHotspotPosition(
		&mouseX,
		&mouseY);

	// Preserve the retail strict-boundary behavior exactly. The only change
	// is that the hotspot now uses the same live PC-pixel canvas as
	// PCSHELL_CoordsDCtoPC instead of stale gameplay logical dimensions.
	return mouseX > left &&
		mouseX < right &&
		mouseY > top &&
		mouseY < bottom;
}

// @Ok
static void SpideyInstallMouseCoordinateCompat()
{
	// PCINPUT_GetMousePosition feeds the visible shell cursor. Hook it as
	// well as hotspot/hit-testing so all three paths use the same mapping.
	PATCH_PUSH_RET(
		0x0050A750,
		SpideyCompatGetMousePosition);

	const unsigned char expectedMouseOver[6] =
	{
		0x8B, 0x0D, 0x54, 0x81, 0x56, 0x00
	};
	const unsigned char expectedHotspot[6] =
	{
		0x8B, 0x0D, 0x54, 0x81, 0x56, 0x00
	};

	unsigned char* mouseOver =
		(unsigned char*)0x0050A820;
	unsigned char* hotspot =
		(unsigned char*)0x0050A770;

	int mouseOverPatched =
		0;
	int hotspotPatched =
		0;

	if (!memcmp(
			mouseOver,
			expectedMouseOver,
			sizeof(expectedMouseOver)))
	{
		PATCH_PUSH_RET(
			0x0050A820,
			SpideyCompatIsMouseOver);
		mouseOverPatched =
			1;
	}

	if (!memcmp(
			hotspot,
			expectedHotspot,
			sizeof(expectedHotspot)))
	{
		PATCH_PUSH_RET(
			0x0050A770,
			SpideyCompatGetMouseHotspotPosition);
		hotspotPatched =
			1;
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");
	if (f)
	{
		fprintf(
			f,
			"mouse_coordinate_compat position=1 mouse_over=%d hotspot=%d position_addr=0x0050A750 mouse_over_addr=0x0050A820 hotspot_addr=0x0050A770 canvas=%dx%d gameplay=%lux%lu basis=client_to_retail_pc_canvas\n",
			mouseOverPatched,
			hotspotPatched,
			SpideyMouseCanvasWidth(),
			SpideyMouseCanvasHeight(),
			(unsigned long)*(DWORD*)0x00568154,
			(unsigned long)*(DWORD*)0x00568158);
		fclose(f);
	}
}

static void SpideyFitLogicalCanvasToSelectedAspect(
		unsigned long* pWidth,
		unsigned long* pHeight)
{
	if (!pWidth ||
		!pHeight ||
		!*pWidth ||
		!*pHeight ||
		gSpideyAspectMode == 0)
	{
		return;
	}

	unsigned long aspectNumerator =
		0;
	unsigned long aspectDenominator =
		0;

	switch (gSpideyAspectMode)
	{
		case 1:
			aspectNumerator = 4;
			aspectDenominator = 3;
			break;
		case 2:
			aspectNumerator = 5;
			aspectDenominator = 4;
			break;
		case 3:
			aspectNumerator = 16;
			aspectDenominator = 9;
			break;
		case 4:
			aspectNumerator = 16;
			aspectDenominator = 10;
			break;
		case 5:
			aspectNumerator = 21;
			aspectDenominator = 9;
			break;
		case 6:
			aspectNumerator = 32;
			aspectDenominator = 9;
			break;
		default:
			return;
	}

	const double outputAspect =
		(double)*pWidth /
		(double)*pHeight;
	const double targetAspect =
		(double)aspectNumerator /
		(double)aspectDenominator;

	if (outputAspect > targetAspect)
	{
		unsigned long fittedWidth =
			(unsigned long)(
				(double)*pHeight *
				targetAspect +
				0.5);

		if (fittedWidth >= 320)
			*pWidth =
				fittedWidth;
	}
	else if (outputAspect < targetAspect)
	{
		unsigned long fittedHeight =
			(unsigned long)(
				(double)*pWidth /
				targetAspect +
				0.5);

		if (fittedHeight >= 240)
			*pHeight =
				fittedHeight;
	}
}

static int gSpideyRequestedFrontendTextScale =
	256;
static int gSpideyLastFrontendTextRequested =
	-1;
static int gSpideyLastFrontendTextEffective =
	-1;
static unsigned long gSpideyLastFrontendTextHeight =
	0;
static int gSpideyLastFrontendTextMode =
	-1;

// @Ok
static int SpideyGetResolutionAwareTextScale(
		int requestedScale)
{
	if (requestedScale <= 0)
		return requestedScale;

	long scaled =
		requestedScale;

	if (gSpideyShadowPreviewEnabled &&
		gSpideyModernLogicalHeight > 480)
	{
		scaled =
			((long)requestedScale * 480L) /
			(long)gSpideyModernLogicalHeight;

		const int minimumScale =
			requestedScale < 64 ?
				requestedScale :
				64;

		if (scaled < minimumScale)
			scaled =
				minimumScale;
	}

	scaled =
		(scaled *
		 (long)gSpideyMenuTextScalePercent +
		 50L) /
		100L;

	if (scaled < 1L)
		scaled =
		1L;
	if (scaled > 65535L)
		scaled =
		65535L;

	return (int)scaled;
}

// @Ok
static void SpideyApplyFrontendTextScale(
		const char* reason)
{
	const int effective =
		SpideyGetResolutionAwareTextScale(
			gSpideyRequestedFrontendTextScale);

	*(u16*)0x0060D5A4 =
		(u16)effective;

	const int frontend =
		(gSpideyFrontendUiActive ||
		 gSpideyFrontendLegacyMode) ? 1 : 0;

	if (gSpideyLastFrontendTextRequested !=
			gSpideyRequestedFrontendTextScale ||
		gSpideyLastFrontendTextEffective !=
			effective ||
		gSpideyLastFrontendTextHeight !=
			gSpideyModernLogicalHeight ||
		gSpideyLastFrontendTextMode !=
			frontend)
	{
		FILE* log = SpideyOpenConsolidatedLog(
		"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"ui_text_scale reason=%s requested=%d effective=%d frontend=%d logical=%lux%lu reference_height=480 user_percent=%d scope=frontend_gameplay_pause\n",
				reason ? reason : "unknown",
				gSpideyRequestedFrontendTextScale,
				effective,
				frontend,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				gSpideyMenuTextScalePercent);
			fclose(log);
		}

		gSpideyLastFrontendTextRequested =
			gSpideyRequestedFrontendTextScale;
		gSpideyLastFrontendTextEffective =
			effective;
		gSpideyLastFrontendTextHeight =
			gSpideyModernLogicalHeight;
		gSpideyLastFrontendTextMode =
			frontend;
	}
}

// @Ok
static void __cdecl SpideyCompatMessSetScale(
		i32 requestedScale)
{
	gSpideyRequestedFrontendTextScale =
		requestedScale;
	SpideyApplyFrontendTextScale(
		"mess_set_scale");
}

// @Ok
static void SpideyInstallFrontendTextScaleCompat()
{
	// patch_mess installs the reconstructed retail-compatible setter first;
	// this final entrypoint override keeps its exact one-argument ABI while
	// applying modern frontend resolution policy.
	PATCH_PUSH_RET(
		0x00458620,
		SpideyCompatMessSetScale);

	FILE* log = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (log)
	{
		fprintf(
			log,
			"ui_text_scale_install retail=0x00458620 wrapper=0x%08lX baseline=640x480 mode=resolution_aware scope=frontend_gameplay_pause\n",
			(unsigned long)&SpideyCompatMessSetScale);
		fclose(log);
	}
}

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

	// Screen Size is the physical/output selection. Aspect Ratio defines the
	// largest non-stretched content canvas that fits inside that output.
	// 16:9 at 2560x1440 remains 2560x1440; 4:3 becomes 1920x1440 and is
	// pillarboxed by the existing DX11 aspect-preserving presenter.
	SpideyFitLogicalCanvasToSelectedAspect(
		&width,
		&height);

	gSpideyModernLogicalWidth =
		width;
	gSpideyModernLogicalHeight =
		height;
}

static int SpideyUseModernOutputAspect()
{
	return gSpideyModernAspectEnabled &&
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
		SpideyUseModernOutputAspect())
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

	// Mess_SetScale may have been called before this transition established
	// the new frontend/logical size. Re-evaluate the last retail-requested
	// scale now so level -> menu transitions immediately get the right size.
	SpideyApplyFrontendTextScale(
		reason ? reason : "logical_resolution");

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"logical_render_resolution reason=%s modern=%d frontend=%d logical=%lux%lu physical=%lux%lu selected=%lux%lu content=%lux%lu aspect=%s\n",
			reason ? reason : "unknown",
			useModern ? 1 : 0,
			gSpideyFrontendLegacyMode,
			width,
			height,
			gSpideyLegacyPhysicalWidth,
			gSpideyLegacyPhysicalHeight,
			gSpideySelectedOutputWidth,
			gSpideySelectedOutputHeight,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight,
			gSpideyAspectLabels[gSpideyAspectMode]);
		fclose(f);
	}
}


typedef void (__cdecl *SpideyRetailPShellLifecycleFn)(void);

// @Ok
static void SpideySetFrontendUiActive(
		int active,
		const char* reason)
{
	gSpideyFrontendUiActive =
		active ? 1 : 0;
	gSpideyFrontendLegacyMode =
		active ? 1 : 0;

	SpideyRefreshModernLogicalResolution();

	if (active)
	{
		SpideyApplyLogicalRenderResolution(
			1,
			reason ?
				reason :
				"shell_frontend_active");
		SpideySyncFrontendMouseBounds(
			reason ?
				reason :
				"shell_frontend_active");
	}
	else
	{
		// Gameplay, pause menus, mission text and HUD text use the same
		// modern-resolution typography policy as the frontend. Re-evaluate
		// the last retail-requested scale here instead of restoring the
		// original 640x480-sized glyph scale.
		SpideyApplyFrontendTextScale(
			reason ?
				reason :
				"shell_frontend_inactive");
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"frontend_lifecycle reason=%s active=%d logical=%lux%lu selected=%lux%lu\n",
			reason ? reason : "unknown",
			gSpideyFrontendUiActive,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight,
			gSpideySelectedOutputWidth,
			gSpideySelectedOutputHeight);
		fclose(f);
	}
}

// @Ok
static void __cdecl SpideyCompatPShellInitialise()
{
	// Set frontend ownership before retail initialization, because
	// PShell_Initialise itself calls PShell_NormalFont/Mess_SetScale.
	SpideySetFrontendUiActive(
		1,
		"pshell_initialise_pre");

	SpideyRetailPShellLifecycleFn retail =
		(SpideyRetailPShellLifecycleFn)0x0048D790;
	retail();

	// Reassert after initialization in case a retail display rebuild occurred
	// inside the shell startup sequence.
	SpideySetFrontendUiActive(
		1,
		"pshell_initialise_post");
}

// @Ok
static void __cdecl SpideyCompatPShellCleanup()
{
	SpideyRetailPShellLifecycleFn retail =
		(SpideyRetailPShellLifecycleFn)0x0048D880;
	retail();

	SpideySetFrontendUiActive(
		0,
		"pshell_cleanup_post");
}

// @Ok
static int SpideyPatchAllRetailDirectCalls(
		unsigned long retailTarget,
		void* replacement)
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	int patched =
		0;

	for (unsigned char* p = textStart;
		 p + 5 <= textEnd;
		 ++p)
	{
		if (p[0] != 0xE8)
			continue;

		const long rel =
			*(long*)(p + 1);
		const unsigned long target =
			(unsigned long)(p + 5 + rel);

		if (target != retailTarget)
			continue;

		DWORD oldProtect =
			0;
		if (!VirtualProtect(
				p,
				5,
				PAGE_EXECUTE_READWRITE,
				&oldProtect))
		{
			continue;
		}

		*(long*)(p + 1) =
			(long)(
				(unsigned char*)replacement -
				(p + 5));

		DWORD ignoredProtect =
			0;
		VirtualProtect(
			p,
			5,
			oldProtect,
			&ignoredProtect);
		FlushInstructionCache(
			GetCurrentProcess(),
			p,
			5);
		++patched;
	}

	return patched;
}

// @Ok
static void SpideyInstallFrontendLifecycleCompat()
{
	const int initialiseCalls =
		SpideyPatchAllRetailDirectCalls(
			0x0048D790,
			(void*)&SpideyCompatPShellInitialise);
	const int cleanupCalls =
		SpideyPatchAllRetailDirectCalls(
			0x0048D880,
			(void*)&SpideyCompatPShellCleanup);

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"frontend_lifecycle_install initialise_target=0x0048D790 initialise_calls=%d cleanup_target=0x0048D880 cleanup_calls=%d\n",
			initialiseCalls,
			cleanupCalls);
		fclose(f);
	}
}

typedef void (__cdecl *SpideyRetailPanelSetCoordsFn)(
		i32,
		i32,
		POLY_FT4*,
		void*,
		i32,
		i32);

static unsigned long gSpideyGameplayUiScaledPolys =
	0;
static unsigned long gSpideyGameplayUiScaleSamples =
	0;
static unsigned long gSpideyGameplayUiFillScaledDraws =
	0;
static unsigned long gSpideyGameplayUiFillScaleSamples =
	0;
static unsigned long gSpideyPanelGouraudProbeSamples =
	0;
static unsigned long gSpideyPanelFlatProbeSamples =
	0;

// @Ok
static void SpideyGetGameplayUiDensity(
		float* densityX,
		float* densityY)
{
	if (!densityX ||
		!densityY)
	{
		return;
	}

	*densityX =
		1.0f;
	*densityY =
		1.0f;

	if (!gSpideyShadowPreviewEnabled ||
		gSpideyModernLogicalWidth <= 640 ||
		gSpideyModernLogicalHeight <= 480)
	{
		return;
	}

	const float userScale =
		(float)gSpideyGameplayUiScalePercent /
		100.0f;

	*densityX =
		(640.0f /
		 (float)gSpideyModernLogicalWidth) *
		userScale;
	*densityY =
		(480.0f /
		 (float)gSpideyModernLogicalHeight) *
		userScale;
}

// @Ok
static short SpideyScaleGameplayUiCoord(
		short value,
		float anchor,
		float density)
{
	const float scaled =
		anchor +
		((float)value - anchor) *
		density;

	if (scaled >= 32767.0f)
		return 32767;
	if (scaled <= -32768.0f)
		return -32768;

	return (short)(
		scaled >= 0.0f ?
			scaled + 0.5f :
			scaled - 0.5f);
}

// @Ok
static float SpideyChooseGameplayUiAnchor(
		short a,
		short b,
		short c,
		short d,
		float extent)
{
	short minimum =
		a;
	short maximum =
		a;

	if (b < minimum)
		minimum = b;
	if (c < minimum)
		minimum = c;
	if (d < minimum)
		minimum = d;

	if (b > maximum)
		maximum = b;
	if (c > maximum)
		maximum = c;
	if (d > maximum)
		maximum = d;

	const float center =
		((float)minimum +
		 (float)maximum) *
		0.5f;

	if (center < extent * 0.40f)
		return 0.0f;

	if (center > extent * 0.60f)
		return extent;

	return extent * 0.5f;
}

// @Ok
static void SpideyCompactGameplayUiPoly(
		POLY_FT4* poly,
		const char* source)
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
		SpideyChooseGameplayUiAnchor(
			poly->x0,
			poly->x1,
			poly->x2,
			poly->x3,
			512.0f);
	const float anchorY =
		SpideyChooseGameplayUiAnchor(
			poly->y0,
			poly->y1,
			poly->y2,
			poly->y3,
			240.0f);

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

	++gSpideyGameplayUiScaledPolys;

	if (gSpideyGameplayUiScaleSamples < 48)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"gameplay_ui_scale source=%s logical=%lux%lu density=%.6f,%.6f user_percent=%d anchor=%.1f,%.1f before=%d,%d,%d,%d,%d,%d,%d,%d after=%d,%d,%d,%d,%d,%d,%d,%d live_after=%.2f,%.2f,%.2f,%.2f count=%lu\n",
				source ? source : "unknown",
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				(double)densityX,
				(double)densityY,
				gSpideyGameplayUiScalePercent,
				(double)anchorX,
				(double)anchorY,
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
				(double)poly->x0 *
					(double)gSpideyModernLogicalWidth / 512.0,
				(double)poly->y0 *
					(double)gSpideyModernLogicalHeight / 240.0,
				(double)poly->x3 *
					(double)gSpideyModernLogicalWidth / 512.0,
				(double)poly->y3 *
					(double)gSpideyModernLogicalHeight / 240.0,
				gSpideyGameplayUiScaledPolys);
			fclose(log);
		}

		++gSpideyGameplayUiScaleSamples;
	}
}

// @Ok
static void __cdecl SpideyCompatPanelSetCoordsFrame(
		i32 x,
		i32 y,
		POLY_FT4* poly,
		void* frame,
		i32 width,
		i32 height)
{
	SpideyRetailPanelSetCoordsFn retail =
		(SpideyRetailPanelSetCoordsFn)0x00462C30;

	retail(
		x,
		y,
		poly,
		frame,
		width,
		height);

	SpideyCompactGameplayUiPoly(
		poly,
		"anim_frame");
}

// @Ok
static void __cdecl SpideyCompatPanelSetCoordsTexture(
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
				"venom_chase_bar_scale level=0x501 policy=shared_top_center_anchor logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%d,%d,%d,%d,%d,%d,%d,%d after=%d,%d,%d,%d,%d,%d,%d,%d count=%lu\n",
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

typedef void (__cdecl *SpideyRetailQPoly2DFn)(
		float, float, float, float, u32,
		float, float, float, float, u32,
		float, float, float, float, u32,
		float, float, float, float, u32,
		float);

typedef void (__cdecl *SpideyRetailFlatUiPolyFn)(
		float,
		i32,
		i32,
		i32,
		i32,
		u8,
		u8,
		u8,
		i32,
		i32);

// @Ok
static float SpideyChooseGameplayUiFloatAnchor(
		float a,
		float b,
		float c,
		float d,
		float extent)
{
	float minimum =
		a;
	float maximum =
		a;

	if (b < minimum)
		minimum = b;
	if (c < minimum)
		minimum = c;
	if (d < minimum)
		minimum = d;

	if (b > maximum)
		maximum = b;
	if (c > maximum)
		maximum = c;
	if (d > maximum)
		maximum = d;

	const float center =
		(minimum + maximum) *
		0.5f;

	if (center < extent * 0.40f)
		return 0.0f;
	if (center > extent * 0.60f)
		return extent;
	return extent * 0.5f;
}

// @Ok
static float SpideyScaleGameplayUiFloatCoord(
		float value,
		float anchor,
		float density)
{
	return anchor +
		(value - anchor) *
		density;
}

// @Ok
static int SpideyRoundGameplayUiCoord(
		float value)
{
	return (int)(
		value >= 0.0f ?
			value + 0.5f :
			value - 0.5f);
}

typedef int (__cdecl *SpideyRetailMessDrawTextFn)(
		i32,
		i32,
		const char*,
		i32,
		u32);

static unsigned long gSpideyCartridgeTextProbeSamples =
	0;
static unsigned long gSpideyCompassQPolyProbeSamples =
	0;

// @Ok
static int SpideyGetGameplayHudTextScale(
		int requestedScale)
{
	if (requestedScale <= 0)
		return requestedScale;

	long scaled =
		requestedScale;

	if (gSpideyShadowPreviewEnabled &&
		gSpideyModernLogicalHeight > 480)
	{
		scaled =
			((long)requestedScale * 480L) /
			(long)gSpideyModernLogicalHeight;

		const int minimumScale =
			requestedScale < 64 ?
				requestedScale :
				64;

		if (scaled < minimumScale)
			scaled =
				minimumScale;
	}

	scaled =
		(scaled *
		 (long)gSpideyGameplayUiScalePercent +
		 50L) /
		100L;

	if (scaled < 1L)
		scaled =
			1L;
	if (scaled > 65535L)
		scaled =
			65535L;

	return (int)scaled;
}

// @Ok
static int __cdecl SpideyCompatCartridgeCountText(
		i32 x,
		i32 y,
		const char* text,
		i32 option4,
		u32 option5)
{
	const int beforeX =
		x;
	const int beforeY =
		y;

	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	if (!gSpideyFrontendUiActive &&
		(densityX < 0.9995f ||
		 densityX > 1.0005f ||
		 densityY < 0.9995f ||
		 densityY > 1.0005f))
	{
		x =
			SpideyRoundGameplayUiCoord(
				(float)x *
				densityX);
		y =
			SpideyRoundGameplayUiCoord(
				(float)y *
				densityY);
	}

	const u16 savedScale =
		*(u16*)0x0060D5A4;
	const int hudScale =
		SpideyGetGameplayHudTextScale(
			gSpideyRequestedFrontendTextScale);
	*(u16*)0x0060D5A4 =
		(u16)hudScale;

	if (gSpideyCartridgeTextProbeSamples < 24)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"gameplay_ui_alignment source=cartridge_text policy=top_left_compact before=%d,%d after=%d,%d density=%.6f,%.6f requested_scale=%d hud_scale=%d saved_text_scale=%u gameplay_percent=%d text_percent=%d text=%s\n",
				beforeX,
				beforeY,
				x,
				y,
				(double)densityX,
				(double)densityY,
				gSpideyRequestedFrontendTextScale,
				hudScale,
				(unsigned int)savedScale,
				gSpideyGameplayUiScalePercent,
				gSpideyMenuTextScalePercent,
				text ? text : "<null>");
			fclose(log);
		}
		++gSpideyCartridgeTextProbeSamples;
	}

	SpideyRetailMessDrawTextFn retail =
		(SpideyRetailMessDrawTextFn)0x00458700;
	const int result =
		retail(
			x,
			y,
			text,
			option4,
			option5);

	*(u16*)0x0060D5A4 =
		savedScale;

	return result;
}

// @Ok
static void __cdecl SpideyCompatCompassQPoly2D(
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
	const float beforeX0 =
		x0;
	const float beforeY0 =
		y0;
	const float beforeX1 =
		x1;
	const float beforeY1 =
		y1;
	const float beforeX2 =
		x2;
	const float beforeY2 =
		y2;
	const float beforeX3 =
		x3;
	const float beforeY3 =
		y3;

	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	if (!gSpideyFrontendUiActive &&
		(densityX < 0.9995f ||
		 densityX > 1.0005f ||
		 densityY < 0.9995f ||
		 densityY > 1.0005f))
	{
		const float anchorX =
			(float)gSpideyModernLogicalWidth;
		const float anchorY =
			(float)gSpideyModernLogicalHeight;

		x0 =
			SpideyScaleGameplayUiFloatCoord(
				x0,
				anchorX,
				densityX);
		x1 =
			SpideyScaleGameplayUiFloatCoord(
				x1,
				anchorX,
				densityX);
		x2 =
			SpideyScaleGameplayUiFloatCoord(
				x2,
				anchorX,
				densityX);
		x3 =
			SpideyScaleGameplayUiFloatCoord(
				x3,
				anchorX,
				densityX);
		y0 =
			SpideyScaleGameplayUiFloatCoord(
				y0,
				anchorY,
				densityY);
		y1 =
			SpideyScaleGameplayUiFloatCoord(
				y1,
				anchorY,
				densityY);
		y2 =
			SpideyScaleGameplayUiFloatCoord(
				y2,
				anchorY,
				densityY);
		y3 =
			SpideyScaleGameplayUiFloatCoord(
				y3,
				anchorY,
				densityY);
	}

	if (gSpideyCompassQPolyProbeSamples < 48)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"gameplay_ui_alignment source=compass_arrow_qpoly sample=%lu call=0x00463D19 policy=bottom_right_compact logical=%lux%lu density=%.6f,%.6f before=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f after=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
				gSpideyCompassQPolyProbeSamples,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				(double)densityX,
				(double)densityY,
				(double)beforeX0,
				(double)beforeY0,
				(double)beforeX1,
				(double)beforeY1,
				(double)beforeX2,
				(double)beforeY2,
				(double)beforeX3,
				(double)beforeY3,
				(double)x0,
				(double)y0,
				(double)x1,
				(double)y1,
				(double)x2,
				(double)y2,
				(double)x3,
				(double)y3);
			fclose(log);
		}
		++gSpideyCompassQPolyProbeSamples;
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
static void __cdecl SpideyCompatHealthBarQPoly2D(
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
	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	const int shouldScale =
		!gSpideyFrontendUiActive &&
		(densityX < 0.9995f ||
		 densityX > 1.0005f ||
		 densityY < 0.9995f ||
		 densityY > 1.0005f);

	float beforeX0 =
		x0;
	float beforeY0 =
		y0;
	float beforeX1 =
		x1;
	float beforeY1 =
		y1;
	float beforeX2 =
		x2;
	float beforeY2 =
		y2;
	float beforeX3 =
		x3;
	float beforeY3 =
		y3;

	if (shouldScale)
	{
		float extentX =
			(float)*(DWORD*)0x00568154;
		float extentY =
			(float)*(DWORD*)0x00568158;

		if (extentX < 640.0f)
			extentX =
				(float)gSpideyModernLogicalWidth;
		if (extentY < 480.0f)
			extentY =
				(float)gSpideyModernLogicalHeight;

		const float anchorX =
			SpideyChooseGameplayUiFloatAnchor(
				x0,
				x1,
				x2,
				x3,
				extentX);
		const float anchorY =
			SpideyChooseGameplayUiFloatAnchor(
				y0,
				y1,
				y2,
				y3,
				extentY);

		x0 =
			SpideyScaleGameplayUiFloatCoord(
				x0,
				anchorX,
				densityX);
		x1 =
			SpideyScaleGameplayUiFloatCoord(
				x1,
				anchorX,
				densityX);
		x2 =
			SpideyScaleGameplayUiFloatCoord(
				x2,
				anchorX,
				densityX);
		x3 =
			SpideyScaleGameplayUiFloatCoord(
				x3,
				anchorX,
				densityX);
		y0 =
			SpideyScaleGameplayUiFloatCoord(
				y0,
				anchorY,
				densityY);
		y1 =
			SpideyScaleGameplayUiFloatCoord(
				y1,
				anchorY,
				densityY);
		y2 =
			SpideyScaleGameplayUiFloatCoord(
				y2,
				anchorY,
				densityY);
		y3 =
			SpideyScaleGameplayUiFloatCoord(
				y3,
				anchorY,
				densityY);

		++gSpideyGameplayUiFillScaledDraws;

		if (gSpideyGameplayUiFillScaleSamples < 12)
		{
			FILE* log =
				SpideyOpenConsolidatedLog(
					"COMPAT");
			if (log)
			{
				fprintf(
					log,
					"gameplay_ui_fill_scale source=qpoly logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f after=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f count=%lu\n",
					gSpideyModernLogicalWidth,
					gSpideyModernLogicalHeight,
					(double)densityX,
					(double)densityY,
					gSpideyGameplayUiScalePercent,
					(double)beforeX0,
					(double)beforeY0,
					(double)beforeX1,
					(double)beforeY1,
					(double)beforeX2,
					(double)beforeY2,
					(double)beforeX3,
					(double)beforeY3,
					(double)x0,
					(double)y0,
					(double)x1,
					(double)y1,
					(double)x2,
					(double)y2,
					(double)x3,
					(double)y3,
					gSpideyGameplayUiFillScaledDraws);
				fclose(log);
			}
			++gSpideyGameplayUiFillScaleSamples;
		}
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

static unsigned long gSpideyPanelQPolyProbeSamples =
	0;

// @Ok
static void __cdecl SpideyCompatPanelQPoly2D(
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
	if (gSpideyPanelQPolyProbeSamples < 48)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"gameplay_ui_alignment source=panel_qpoly policy=passthrough seq=%lu logical=%lux%lu coords=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f reason=coordinates_already_match_post_holder_live_space\n",
				gSpideyPanelQPolyProbeSamples % 6,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				(double)x0,
				(double)y0,
				(double)x1,
				(double)y1,
				(double)x2,
				(double)y2,
				(double)x3,
				(double)y3);
			fclose(log);
		}
		++gSpideyPanelQPolyProbeSamples;
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

typedef void (__cdecl *SpideyRetailGouraudUiPolyFn)(
		float,
		i32,
		i32,
		i32,
		i32,
		u32,
		u32,
		u32,
		u32,
		i32);

// @Ok
static void __cdecl SpideyCompatPanelGouraudPoly(
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
	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	const int beforeX =
		x;
	const int beforeY =
		y;
	const int beforeWidth =
		width;
	const int beforeHeight =
		height;

	if (!gSpideyFrontendUiActive &&
		(densityX < 0.9995f ||
		 densityX > 1.0005f ||
		 densityY < 0.9995f ||
		 densityY > 1.0005f))
	{
		const float right =
			(float)x +
			(float)width;
		const float bottom =
			(float)y +
			(float)height;
		const float anchorX =
			SpideyChooseGameplayUiFloatAnchor(
				(float)x,
				right,
				(float)x,
				right,
				512.0f);
		const float anchorY =
			SpideyChooseGameplayUiFloatAnchor(
				(float)y,
				(float)y,
				bottom,
				bottom,
				240.0f);

		const int scaledLeft =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					(float)x,
					anchorX,
					densityX));
		const int scaledRight =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					right,
					anchorX,
					densityX));
		const int scaledTop =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					(float)y,
					anchorY,
					densityY));
		const int scaledBottom =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					bottom,
					anchorY,
					densityY));

		x =
			scaledLeft;
		y =
			scaledTop;
		width =
			scaledRight -
			scaledLeft;
		height =
			scaledBottom -
			scaledTop;

		++gSpideyGameplayUiFillScaledDraws;
	}

	if (gSpideyPanelGouraudProbeSamples < 48)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"gameplay_ui_alignment source=panel_gouraud seq=%lu logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%d,%d,%d,%d after=%d,%d,%d,%d live_after=%.2f,%.2f,%.2f,%.2f\n",
				gSpideyPanelGouraudProbeSamples % 3,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				(double)densityX,
				(double)densityY,
				gSpideyGameplayUiScalePercent,
				beforeX,
				beforeY,
				beforeWidth,
				beforeHeight,
				x,
				y,
				width,
				height,
				(double)x *
					(double)gSpideyModernLogicalWidth / 512.0,
				(double)y *
					(double)gSpideyModernLogicalHeight / 240.0,
				(double)(x + width) *
					(double)gSpideyModernLogicalWidth / 512.0,
				(double)(y + height) *
					(double)gSpideyModernLogicalHeight / 240.0);
			fclose(log);
		}
		++gSpideyPanelGouraudProbeSamples;
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
static void __cdecl SpideyCompatPanelFlatPoly(
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
	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	const int beforeX =
		x;
	const int beforeY =
		y;
	const int beforeWidth =
		width;
	const int beforeHeight =
		height;

	if (!gSpideyFrontendUiActive &&
		(densityX < 0.9995f ||
		 densityX > 1.0005f ||
		 densityY < 0.9995f ||
		 densityY > 1.0005f))
	{
		const float right =
			(float)x +
			(float)width;
		const float bottom =
			(float)y +
			(float)height;
		const float anchorX =
			SpideyChooseGameplayUiFloatAnchor(
				(float)x,
				right,
				(float)x,
				right,
				512.0f);
		const float anchorY =
			SpideyChooseGameplayUiFloatAnchor(
				(float)y,
				(float)y,
				bottom,
				bottom,
				240.0f);

		const int scaledLeft =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					(float)x,
					anchorX,
					densityX));
		const int scaledRight =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					right,
					anchorX,
					densityX));
		const int scaledTop =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					(float)y,
					anchorY,
					densityY));
		const int scaledBottom =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					bottom,
					anchorY,
					densityY));

		x =
			scaledLeft;
		y =
			scaledTop;
		width =
			scaledRight -
			scaledLeft;
		height =
			scaledBottom -
			scaledTop;

		++gSpideyGameplayUiFillScaledDraws;
	}

	if (gSpideyPanelFlatProbeSamples < 48)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"gameplay_ui_alignment source=panel_flat seq=%lu logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%d,%d,%d,%d after=%d,%d,%d,%d live_after=%.2f,%.2f,%.2f,%.2f\n",
				gSpideyPanelFlatProbeSamples % 3,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				(double)densityX,
				(double)densityY,
				gSpideyGameplayUiScalePercent,
				beforeX,
				beforeY,
				beforeWidth,
				beforeHeight,
				x,
				y,
				width,
				height,
				(double)x *
					(double)gSpideyModernLogicalWidth / 512.0,
				(double)y *
					(double)gSpideyModernLogicalHeight / 240.0,
				(double)(x + width) *
					(double)gSpideyModernLogicalWidth / 512.0,
				(double)(y + height) *
					(double)gSpideyModernLogicalHeight / 240.0);
			fclose(log);
		}
		++gSpideyPanelFlatProbeSamples;
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
static void __cdecl SpideyCompatHealthBarFlatPoly(
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
	float densityX =
		1.0f;
	float densityY =
		1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	const int shouldScale =
		!gSpideyFrontendUiActive &&
		(densityX < 0.9995f ||
		 densityX > 1.0005f ||
		 densityY < 0.9995f ||
		 densityY > 1.0005f);

	const int beforeX =
		x;
	const int beforeY =
		y;
	const int beforeWidth =
		width;
	const int beforeHeight =
		height;

	if (shouldScale)
	{
		const float right =
			(float)x +
			(float)width;
		const float bottom =
			(float)y +
			(float)height;
		const float anchorX =
			SpideyChooseGameplayUiFloatAnchor(
				(float)x,
				right,
				(float)x,
				right,
				512.0f);
		const float anchorY =
			SpideyChooseGameplayUiFloatAnchor(
				(float)y,
				(float)y,
				bottom,
				bottom,
				240.0f);

		const int scaledLeft =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					(float)x,
					anchorX,
					densityX));
		const int scaledRight =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					right,
					anchorX,
					densityX));
		const int scaledTop =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					(float)y,
					anchorY,
					densityY));
		const int scaledBottom =
			SpideyRoundGameplayUiCoord(
				SpideyScaleGameplayUiFloatCoord(
					bottom,
					anchorY,
					densityY));

		x =
			scaledLeft;
		y =
			scaledTop;
		width =
			scaledRight -
			scaledLeft;
		height =
			scaledBottom -
			scaledTop;

		++gSpideyGameplayUiFillScaledDraws;

		if (gSpideyGameplayUiFillScaleSamples < 12)
		{
			FILE* log =
				SpideyOpenConsolidatedLog(
					"COMPAT");
			if (log)
			{
				fprintf(
					log,
					"gameplay_ui_fill_scale source=flat logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%d,%d,%d,%d after=%d,%d,%d,%d count=%lu\n",
					gSpideyModernLogicalWidth,
					gSpideyModernLogicalHeight,
					(double)densityX,
					(double)densityY,
					gSpideyGameplayUiScalePercent,
					beforeX,
					beforeY,
					beforeWidth,
					beforeHeight,
					x,
					y,
					width,
					height,
					gSpideyGameplayUiFillScaledDraws);
				fclose(log);
			}
			++gSpideyGameplayUiFillScaleSamples;
		}
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

static unsigned long gSpideyMysterioBossUiScaledDraws =
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
			"mysterio_health_alignment source=%s sample=%lu logical=%lux%lu density_user=%d rect=%.2f,%.2f,%.2f,%.2f live_if_512x240=%.2f,%.2f,%.2f,%.2f\n",
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


// @Ok
static void SpideyCompactMysterioBossHolderPoly(
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

	float densityX = 1.0f;
	float densityY = 1.0f;
	SpideyGetGameplayUiDensity(
		&densityX,
		&densityY);

	if (densityX >= 1.0f &&
		densityY >= 1.0f)
	{
		return;
	}

	const float anchorX = 512.0f;
	const float anchorY = 0.0f;
	poly->x0 = SpideyScaleGameplayUiCoord(poly->x0, anchorX, densityX);
	poly->x1 = SpideyScaleGameplayUiCoord(poly->x1, anchorX, densityX);
	poly->x2 = SpideyScaleGameplayUiCoord(poly->x2, anchorX, densityX);
	poly->x3 = SpideyScaleGameplayUiCoord(poly->x3, anchorX, densityX);
	poly->y0 = SpideyScaleGameplayUiCoord(poly->y0, anchorY, densityY);
	poly->y1 = SpideyScaleGameplayUiCoord(poly->y1, anchorY, densityY);
	poly->y2 = SpideyScaleGameplayUiCoord(poly->y2, anchorY, densityY);
	poly->y3 = SpideyScaleGameplayUiCoord(poly->y3, anchorY, densityY);
}

static void __cdecl SpideyCompatMysterioBossHolderTexture(
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
	SpideyCompactMysterioBossHolderPoly(
		poly);

	if (SpideyIsMysterioBossActive() &&
		poly)
	{
		SpideyLogMysterioHealthRect(
			"holder_texture_shared_top_right",
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
	SpideyRetailPanelSetCoordsFn retail =
		(SpideyRetailPanelSetCoordsFn)0x00462C30;
	retail(
		x,
		y,
		poly,
		frame,
		width,
		height);
	SpideyCompactMysterioBossHolderPoly(
		poly);

	if (SpideyIsMysterioBossActive() &&
		poly)
	{
		SpideyLogMysterioHealthRect(
			"holder_frame_shared_top_right",
			(float)poly->x0,
			(float)poly->y0,
			(float)poly->x3,
			(float)poly->y3);
	}
}

static unsigned long gSpideyScorpionChaseBarScaleSamples =
	0;
static unsigned long gSpideyScorpionChaseBarScaledPolys =
	0;

// Race to the Bugle's Scorpion/Jonah progress meter is one authored
// top-right composite. Its three holder pieces must share the same anchor;
// choosing an anchor independently per piece tears the meter apart at modern
// resolutions, exactly like the earlier Mysterio boss holder failure.
static void SpideyCompactScorpionChaseBarPoly(
		POLY_FT4* poly,
		const char* source)
{
	if (!poly ||
		gSpideyFrontendUiActive ||
		!gSpideyShadowPreviewEnabled ||
		gSpideyModernLogicalWidth <= 640 ||
		gSpideyModernLogicalHeight <= 480)
	{
		return;
	}

	float densityX = 1.0f;
	float densityY = 1.0f;
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

	const float anchorX = 512.0f;
	const float anchorY = 0.0f;

	poly->x0 = SpideyScaleGameplayUiCoord(poly->x0, anchorX, densityX);
	poly->x1 = SpideyScaleGameplayUiCoord(poly->x1, anchorX, densityX);
	poly->x2 = SpideyScaleGameplayUiCoord(poly->x2, anchorX, densityX);
	poly->x3 = SpideyScaleGameplayUiCoord(poly->x3, anchorX, densityX);
	poly->y0 = SpideyScaleGameplayUiCoord(poly->y0, anchorY, densityY);
	poly->y1 = SpideyScaleGameplayUiCoord(poly->y1, anchorY, densityY);
	poly->y2 = SpideyScaleGameplayUiCoord(poly->y2, anchorY, densityY);
	poly->y3 = SpideyScaleGameplayUiCoord(poly->y3, anchorY, densityY);

	++gSpideyScorpionChaseBarScaledPolys;

	if (gSpideyScorpionChaseBarScaleSamples < 48)
	{
		FILE* log =
			SpideyOpenConsolidatedLog(
				"COMPAT");
		if (log)
		{
			fprintf(
				log,
				"scorpion_chase_bar_scale source=%s item_type=%d policy=shared_top_right_anchor logical=%lux%lu density=%.6f,%.6f user_percent=%d before=%d,%d,%d,%d,%d,%d,%d,%d after=%d,%d,%d,%d,%d,%d,%d,%d count=%lu\n",
				source ? source : "unknown",
				*(volatile int*)0x0060F654,
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
				gSpideyScorpionChaseBarScaledPolys);
			fclose(log);
		}

		++gSpideyScorpionChaseBarScaleSamples;
	}
}

static void __cdecl SpideyCompatScorpionChaseBarTexture(
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
	SpideyCompactScorpionChaseBarPoly(
		poly,
		"texture");
}

static void __cdecl SpideyCompatScorpionChaseBarFrame(
		i32 x,
		i32 y,
		POLY_FT4* poly,
		void* frame,
		i32 width,
		i32 height)
{
	SpideyRetailPanelSetCoordsFn retail =
		(SpideyRetailPanelSetCoordsFn)0x00462C30;
	retail(
		x,
		y,
		poly,
		frame,
		width,
		height);
	SpideyCompactScorpionChaseBarPoly(
		poly,
		"frame");
}

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
		SpideyLogMysterioHealthRect(
			"fill_qpoly_live_passthrough",
			x0,
			y0,
			x3,
			y3);
		// Mysterio has already converted these QPoly vertices into live pixels.
		// Do not run them through the authored-space health-bar compactor twice.
		((SpideyRetailQPoly2DFn)0x00507910)(
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
		SpideyLogMysterioHealthRect(
			"fill_flat_authored_before_compact",
			(float)x,
			(float)y,
			(float)(x + width),
			(float)(y + height));
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
		SpideyLogMysterioHealthRect(
			"fill_gouraud_authored_before_compact",
			(float)x,
			(float)y,
			(float)(x + width),
			(float)(y + height));
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
	const int frameCalls =
		SpideyPatchAllRetailDirectCalls(
			0x00462C30,
			(void*)&SpideyCompatPanelSetCoordsFrame);
	const int textureCalls =
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

	const int scorpionChaseTextureOne =
		SpideyPatchDirectCall(
			0x004651CF,
			(unsigned long)(void*)&SpideyCompatPanelSetCoordsTexture,
			(void*)&SpideyCompatScorpionChaseBarTexture,
			"scorpion_chase_bar_texture_1");
	const int scorpionChaseTextureTwo =
		SpideyPatchDirectCall(
			0x004653E3,
			(unsigned long)(void*)&SpideyCompatPanelSetCoordsTexture,
			(void*)&SpideyCompatScorpionChaseBarTexture,
			"scorpion_chase_bar_texture_2");
	const int scorpionChaseFrame =
		SpideyPatchDirectCall(
			0x004655F2,
			(unsigned long)(void*)&SpideyCompatPanelSetCoordsFrame,
			(void*)&SpideyCompatScorpionChaseBarFrame,
			"scorpion_chase_bar_frame");

	const unsigned long venomChaseBarCoordSites[] =
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

	const int cartridgeTextInstalled =
		SpideyPatchDirectCall(
			0x00465A83,
			0x00458700,
			(void*)&SpideyCompatCartridgeCountText,
			"cartridge_count_text");

	const int compassArrowQPolyInstalled =
		SpideyPatchDirectCall(
			0x00463D19,
			0x00507910,
			(void*)&SpideyCompatCompassQPoly2D,
			"compass_arrow_qpoly");

	const int healthQPolyOne =
		SpideyPatchDirectCall(
			0x004644E3,
			0x00507910,
			(void*)&SpideyCompatHealthBarQPoly2D,
			"health_fill_qpoly_1");
	const int healthQPolyTwo =
		SpideyPatchDirectCall(
			0x00464707,
			0x00507910,
			(void*)&SpideyCompatHealthBarQPoly2D,
			"health_fill_qpoly_2");
	const int healthQPolyThree =
		SpideyPatchDirectCall(
			0x00464936,
			0x00507910,
			(void*)&SpideyCompatHealthBarQPoly2D,
			"health_fill_qpoly_3");
	const int healthFlatOne =
		SpideyPatchDirectCall(
			0x0046497D,
			0x00462D60,
			(void*)&SpideyCompatHealthBarFlatPoly,
			"health_fill_flat_1");
	const int healthFlatTwo =
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
	{
		0x00465D08,
		0x00465F46,
		0x00466176,
		0x004663AA,
		0x004665D1,
		0x004667F6
	};
	int panelQPolyCalls =
		0;
	for (int qpolyIndex = 0;
		 qpolyIndex < (int)(sizeof(panelQPolySites) / sizeof(panelQPolySites[0]));
		 ++qpolyIndex)
	{
		panelQPolyCalls +=
			SpideyPatchDirectCall(
				panelQPolySites[qpolyIndex],
				0x00507910,
				(void*)&SpideyCompatPanelQPoly2D,
				"panel_fill_qpoly");
	}

	const unsigned long panelGouraudSites[] =
	{
		0x0046687B,
		0x00466931,
		0x004669C7
	};
	int panelGouraudCalls =
		0;
	for (int gouraudIndex = 0;
		 gouraudIndex < (int)(sizeof(panelGouraudSites) / sizeof(panelGouraudSites[0]));
		 ++gouraudIndex)
	{
		panelGouraudCalls +=
			SpideyPatchDirectCall(
				panelGouraudSites[gouraudIndex],
				0x00462FB0,
				(void*)&SpideyCompatPanelGouraudPoly,
				"panel_fill_gouraud");
	}

	const unsigned long panelFlatSites[] =
	{
		0x004668D1,
		0x00466A1B,
		0x00466A65
	};
	int panelFlatCalls =
		0;
	for (int flatIndex = 0;
		 flatIndex < (int)(sizeof(panelFlatSites) / sizeof(panelFlatSites[0]));
		 ++flatIndex)
	{
		panelFlatCalls +=
			SpideyPatchDirectCall(
				panelFlatSites[flatIndex],
				0x00462D60,
				(void*)&SpideyCompatPanelFlatPoly,
				"panel_fill_flat");
	}

	FILE* log =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (log)
	{
		fprintf(
			log,
			"gameplay_ui_scale_install frame_target=0x00462C30 frame_calls=%d texture_target=0x00462CD0 texture_calls=%d venom_chase_bar_calls=%d venom_chase_bar_policy=level_0x501_shared_top_center_anchor scorpion_chase_holders=%d,%d,%d scorpion_chase_policy=item_310_shared_top_right_anchor cartridge_text=%d compass_arrow_qpoly=%d compass_live_qpoly_passthrough=2 health_qpoly=%d,%d,%d health_flat=%d,%d mysterio_boss_fill=qpoly:%d,flat:%d,gouraud:%d,%d mysterio_holders=texture:%d,frame:%d mysterio_boss_type=311 panel_qpoly=%d panel_gouraud=%d panel_flat=%d reference=512x240 baseline_output=640x480 policy=compact_holders_compass_arrow_only_cartridge_gouraud_flat_panel_qpoly_passthrough user_percent=%d\n",
			frameCalls,
			textureCalls,
			venomChaseBarCoordCalls,
			scorpionChaseTextureOne,
			scorpionChaseTextureTwo,
			scorpionChaseFrame,
			cartridgeTextInstalled,
			compassArrowQPolyInstalled,
			healthQPolyOne,
			healthQPolyTwo,
			healthQPolyThree,
			healthFlatOne,
			healthFlatTwo,
			mysterioBossQPolyCalls,
			mysterioBossFlatCall,
			mysterioBossGouraudOne,
			mysterioBossGouraudTwo,
			mysterioHolderTextureCall,
			mysterioHolderFrameCall,
			panelQPolyCalls,
			panelGouraudCalls,
			panelFlatCalls,
			gSpideyGameplayUiScalePercent);
		fclose(log);
	}
}



typedef void (__cdecl *SpideyRetailLoadCullBasisFn)(
		const short*);
typedef int (__cdecl *SpideyRetailM3dSqrtFn)(
		int);

static void SpideyNormalizeCullPlane(
		short* destination,
		long x,
		long y,
		long z)
{
	if (!destination)
		return;

	const long lengthSquared =
		x * x +
		y * y +
		z * z;

	if (lengthSquared <= 0)
	{
		destination[0] = 0;
		destination[1] = 0;
		destination[2] = 0;
		return;
	}

	SpideyRetailM3dSqrtFn retailSqrt =
		(SpideyRetailM3dSqrtFn)0x0046D430;
	const int length =
		retailSqrt(
			(int)lengthSquared);

	if (length <= 0)
	{
		destination[0] = 0;
		destination[1] = 0;
		destination[2] = 0;
		return;
	}

	destination[0] =
		(short)((x * 4096L) / length);
	destination[1] =
		(short)((y * 4096L) / length);
	destination[2] =
		(short)((z * 4096L) / length);
}

static int gSpideyHorPlusCullLogged =
	0;

static void __cdecl SpideyCompatLoadCullBasis(
		const short* source)
{
	SpideyRetailLoadCullBasisFn retail =
		(SpideyRetailLoadCullBasisFn)0x0046D810;

	if (!source ||
		!gSpideyShadowPreviewEnabled ||
		gSpideyFrontendLegacyMode ||
		!SpideyUseModernOutputAspect())
	{
		retail(
			source);
		return;
	}

	float aspectScalar =
		*(float*)0x00550064;

	if (aspectScalar < 0.25f ||
		aspectScalar > 2.0f)
	{
		aspectScalar =
			1.0f;
	}

	const long aspectFixed =
		(long)(
			aspectScalar * 4096.0f +
			0.5f);

	// M3d_RenderSetup produces six normalized frustum-plane normals as two
	// 3x3 matrices. This source is the second matrix. Row 0 is the opposing
	// vertical plane; rows 1 and 2 are the mirrored left/right side planes.
	// Their sum is the camera-forward component and their difference is the
	// camera-right component. Scale only that right component by the same
	// aspect scalar used by retail projection, then renormalize to the
	// 4096-length fixed-point convention expected by the sphere culler.
	short adjusted[9];
	int component;

	for (component = 0;
		 component < 9;
		 ++component)
	{
		adjusted[component] =
			source[component];
	}

	long leftRaw[3];
	long rightRaw[3];

	for (component = 0;
		 component < 3;
		 ++component)
	{
		const long left =
			(long)source[3 + component];
		const long right =
			(long)source[6 + component];
		const long sum =
			left + right;
		const long difference =
			left - right;
		const long scaledDifference =
			(difference * aspectFixed) >>
				12;

		leftRaw[component] =
			sum + scaledDifference;
		rightRaw[component] =
			sum - scaledDifference;
	}

	SpideyNormalizeCullPlane(
		&adjusted[3],
		leftRaw[0],
		leftRaw[1],
		leftRaw[2]);
	SpideyNormalizeCullPlane(
		&adjusted[6],
		rightRaw[0],
		rightRaw[1],
		rightRaw[2]);

	if (!gSpideyHorPlusCullLogged)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"horplus_cull scalar=%.6f source_left=%d,%d,%d source_right=%d,%d,%d adjusted_left=%d,%d,%d adjusted_right=%d,%d,%d call=0x004739CB retail=0x0046D810\n",
				(double)aspectScalar,
				(int)source[3],
				(int)source[4],
				(int)source[5],
				(int)source[6],
				(int)source[7],
				(int)source[8],
				(int)adjusted[3],
				(int)adjusted[4],
				(int)adjusted[5],
				(int)adjusted[6],
				(int)adjusted[7],
				(int)adjusted[8]);
			fclose(f);
		}
		gSpideyHorPlusCullLogged =
			1;
	}

	retail(
		adjusted);
}

static void SpideyInstallHorPlusCullCompat()
{
	const int installed =
		SpideyPatchDirectCall(
			0x004739CB,
			0x0046D810,
			SpideyCompatLoadCullBasis,
			"horplus_cull_basis");

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"horplus_cull_install installed=%d call=0x004739CB retail=0x0046D810 scalar=%.6f\n",
			installed,
			(double)*(float*)0x00550064);
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

	const int frontendLegacyRequest =
		requestedWidth == 640 &&
		requestedHeight == 480 &&
		requestedBpp == 16 &&
		option4 == 0;

	// The exact 640x480x16 shell request is still a useful fallback signal
	// during very early shell entry, but modern-resolution rebuilds while a
	// menu is active must never revoke frontend ownership.
	if (frontendLegacyRequest)
		gSpideyFrontendUiActive =
			1;

	const int frontendActive =
		gSpideyFrontendUiActive ? 1 : 0;

	// The compatibility producer must never be torn down/rebuilt while DXGI
	// owns the display exclusively. Release exclusive unconditionally if it
	// is currently active; the selected target mode is reacquired only after
	// the replacement compatibility objects have their interception hooks.
	SpideyReleaseRendererExclusiveForCompatRebuild(
		"display_options_pre_retail");

	u32 physicalWidth =
		requestedWidth;
	u32 physicalHeight =
		requestedHeight;
	u32 physicalBpp =
		requestedBpp;

	int remappedLegacyBacking =
		0;

	int seededSelectedOutput =
		0;

	// Retail calls DXINIT_SetDisplayOptions internally for gameplay/shell
	// transitions. Those calls describe only the compatibility producer and
	// must never replace the modern user-facing DX11 output selected through
	// our Display -> Apply path. Seed from retail only as an emergency when
	// no valid modern selection exists yet.
	if (!frontendActive &&
		(gSpideySelectedOutputWidth < 640 ||
		 gSpideySelectedOutputHeight < 480))
	{
		gSpideySelectedOutputWidth =
			requestedWidth;
		gSpideySelectedOutputHeight =
			requestedHeight;
		gSpideySelectedOutputBpp =
			32;
		seededSelectedOutput =
			1;
	}

	// The old shell asks retail DirectDraw to fall back to 640x480x16 on
	// frontend entry. DX11 now owns the visible output, so do not let that
	// request become the active frontend canvas. Keep the user's selected
	// modern dimensions authoritative and use only a hidden D3D7-compatible
	// producer surface where the legacy device requires one.
	if (frontendLegacyRequest &&
		gSpideySelectedOutputWidth >= 640 &&
		gSpideySelectedOutputHeight >= 480)
	{
		physicalWidth =
			(u32)gSpideySelectedOutputWidth;
		physicalHeight =
			(u32)gSpideySelectedOutputHeight;
		physicalBpp =
			32;
		remappedLegacyBacking =
			1;
	}
	else
	{
		physicalBpp =
			32;
	}

	// Runtime-grounded: retail D3D7 rejects a 2560x1440 scene surface.
	// Keep that limitation quarantined to the hidden producer only.
	if (physicalWidth == 2560 &&
		physicalHeight == 1440)
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

	SpideyRetailSetDisplayOptionsFn retail =
		(SpideyRetailSetDisplayOptionsFn)0x00500250;

	retail(
		physicalWidth,
		physicalHeight,
		physicalBpp,
		option4,
		option5);

	gSpideyFrontendLegacyMode =
		frontendActive ? 1 : 0;

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

	SpideyApplySelectedAspect(
		gSpideyFrontendLegacyMode ?
			"display_options_frontend" :
			"display_options_gameplay");

	SpideyInjectModernVideoModes();

	SpideyApplySelectedWindowStyle(
		*(HWND*)0x006B58D0,
		"display_options");

	// Retail may have replaced the Direct3D7 device and main surfaces. Hook
	// those replacement compatibility objects *before* DXGI is allowed to
	// reacquire exclusive display ownership.
	SpideyInstallRetailD3D7DrawProbe();

	SpideyApplyRendererWindowMode(
		gSpideyWindowMode ==
			SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE ?
			"display_options_enter_exclusive" :
			"display_options_leave_exclusive");

	SpideyRefreshModernLogicalResolution();
	SpideyApplyLogicalRenderResolution(
		gSpideyShadowPreviewEnabled ? 1 : 0,
		gSpideyFrontendLegacyMode ?
			"display_options_frontend" :
			"display_options_gameplay");

	// Retail PCSHELL_Initialize only establishes mouse bounds while its
	// cursor sprite is first created. A gameplay -> frontend device switch
	// can therefore leave the mouse clamped to the old gameplay backing.
	// Rebind the retail mouse domain to the active frontend D3D7 canvas.
	SpideySyncFrontendMouseBounds(
		"display_options_transition");

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_options requested=%lux%lux%lu selected=%lux%lux%lu physical=%lux%lux%lu option4=%d option5=%d frontend_active=%d frontend_legacy_request=%d legacy_backing_remap=%d selection_seeded=%d preserve_selected=1\n",
			(unsigned long)requestedWidth,
			(unsigned long)requestedHeight,
			(unsigned long)requestedBpp,
			(unsigned long)gSpideySelectedOutputWidth,
			(unsigned long)gSpideySelectedOutputHeight,
			(unsigned long)gSpideySelectedOutputBpp,
			(unsigned long)physicalWidth,
			(unsigned long)physicalHeight,
			(unsigned long)physicalBpp,
			option4,
			option5,
			frontendActive,
			frontendLegacyRequest,
			remappedLegacyBacking,
			seededSelectedOutput);
		fclose(f);
	}

}


typedef void (__cdecl *SpideyRetailSaveSettingsFn)(void);

static void __cdecl SpideyDisplayConfirmOrApply(
		u32 liveWidth,
		u32 liveHeight,
		u32 liveBpp,
		i32 option4,
		i32 option5)
{
	const int onApply =
		gSpideyDisplayMenu &&
		gSpideyDisplayMenu->mNumLines >= 7 &&
		gSpideyDisplayMenu->mLine == 6;

	if (!onApply)
	{
		FILE* ignored = SpideyOpenConsolidatedLog(
			"COMPAT");
		if (ignored)
		{
			fprintf(
				ignored,
				"display_apply ignored line=%d pending=%lux%lu aspect=%s gameplay_ui=%d text=%d committed=%lux%lu aspect=%s gameplay_ui=%d text=%d\n",
				gSpideyDisplayMenu ?
					(int)gSpideyDisplayMenu->mLine :
					-1,
				gSpideyPendingOutputWidth,
				gSpideyPendingOutputHeight,
				gSpideyAspectLabels[gSpideyPendingAspectMode],
				gSpideyPendingGameplayUiScalePercent,
				gSpideyPendingMenuTextScalePercent,
				gSpideySelectedOutputWidth,
				gSpideySelectedOutputHeight,
				gSpideyAspectLabels[gSpideyAspectMode],
				gSpideyGameplayUiScalePercent,
				gSpideyMenuTextScalePercent);
			fclose(ignored);
		}
		return;
	}

	if (gSpideyPendingOutputWidth < 640 ||
		gSpideyPendingOutputWidth > 8192 ||
		gSpideyPendingOutputHeight < 480 ||
		gSpideyPendingOutputHeight > 8192)
	{
		SpideyResetPendingDisplaySettings(
			"apply_invalid_pending");
		return;
	}

	if (gSpideyPendingAspectMode < 0 ||
		gSpideyPendingAspectMode >=
			(int)(sizeof(gSpideyAspectLabels) /
				  sizeof(gSpideyAspectLabels[0])))
	{
		gSpideyPendingAspectMode =
			gSpideyAspectMode;
	}

	gSpideyPendingGameplayUiScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingGameplayUiScalePercent);
	gSpideyPendingMenuTextScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingMenuTextScalePercent);

	const int displayChanged =
		gSpideyPendingOutputWidth !=
			gSpideySelectedOutputWidth ||
		gSpideyPendingOutputHeight !=
			gSpideySelectedOutputHeight ||
		gSpideyPendingAspectMode !=
			gSpideyAspectMode ||
		gSpideyPendingWindowMode !=
			gSpideyWindowMode;
	const int uiScaleChanged =
		gSpideyPendingGameplayUiScalePercent !=
			gSpideyGameplayUiScalePercent ||
		gSpideyPendingMenuTextScalePercent !=
			gSpideyMenuTextScalePercent;

	gSpideySelectedOutputWidth =
		gSpideyPendingOutputWidth;
	gSpideySelectedOutputHeight =
		gSpideyPendingOutputHeight;
	gSpideySelectedOutputBpp =
		32;
	gSpideyAspectMode =
		gSpideyPendingAspectMode;
	gSpideyWindowMode =
		gSpideyPendingWindowMode;
	gSpideyGameplayUiScalePercent =
		gSpideyPendingGameplayUiScalePercent;
	gSpideyMenuTextScalePercent =
		gSpideyPendingMenuTextScalePercent;

	*(DWORD*)0x02E096F8 =
		(DWORD)gSpideySelectedOutputWidth;
	*(DWORD*)0x02E0970C =
		(DWORD)gSpideySelectedOutputHeight;
	*(DWORD*)0x02E098E4 =
		(DWORD)gSpideySelectedOutputBpp;

	SpideySaveModernVideoSettings();
	SpideyApplySelectedAspect(
		"display_menu_apply_commit");

	const int liveFrontend =
		(gSpideyFrontendUiActive ||
		 gSpideyFrontendLegacyMode) ? 1 : 0;

	if (displayChanged)
	{
		SpideyCompatSetDisplayOptions(
			(u32)gSpideySelectedOutputWidth,
			(u32)gSpideySelectedOutputHeight,
			32,
			option4,
			option5);

		if (liveFrontend)
		{
			gSpideyFrontendUiActive =
				1;
			gSpideyFrontendLegacyMode =
				1;
			SpideyRefreshModernLogicalResolution();
			SpideyApplyLogicalRenderResolution(
				1,
				"display_apply_frontend_restore");
			SpideySyncFrontendMouseBounds(
				"display_apply_frontend_restore");
		}

		SpideyApplySelectedWindowStyle(
			*(HWND*)0x006B58D0,
			"display_apply");
		SpideyApplyRendererWindowMode(
			"display_apply");
	}
	else
	{
		SpideyRefreshModernLogicalResolution();
	}

	SpideyApplyFrontendTextScale(
		displayChanged ?
			"display_apply_post_rebuild" :
			"display_apply_ui_only");

	SpideyRetailSaveSettingsFn retailSave =
		(SpideyRetailSaveSettingsFn)0x00515850;
	retailSave();

	SpideyResetPendingDisplaySettings(
		"apply_complete");

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"display_apply committed=1 selected=%lux%lux%lu aspect=%s scalar=%.6f window_mode=%s gameplay_ui=%d text=%d display_changed=%d ui_changed=%d in_level=%d live_before=%lux%lux%lu frontend=%d brightness=%d saved_now=1\n",
			gSpideySelectedOutputWidth,
			gSpideySelectedOutputHeight,
			gSpideySelectedOutputBpp,
			gSpideyAspectLabels[gSpideyAspectMode],
			(double)*(float*)0x00550064,
			gSpideyWindowModeLabels[gSpideyWindowMode],
			gSpideyGameplayUiScalePercent,
			gSpideyMenuTextScalePercent,
			displayChanged,
			uiScaleChanged,
			gSpideyInLevelDisplayMenuActive,
			(unsigned long)liveWidth,
			(unsigned long)liveHeight,
			(unsigned long)liveBpp,
			liveFrontend,
			option5);
		fclose(f);
	}
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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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

static HMODULE gSpideyInput11Module = 0;

typedef unsigned long (__cdecl *SpideyInput11GetAbiVersionFn)(void);
typedef const char* (__cdecl *SpideyInput11GetBackendNameFn)(void);
typedef int (__cdecl *SpideyInput11ProbeFn)(void);
typedef int (__cdecl *SpideyInput11PollFn)(
		SpideyInput11LegacyState*,
		unsigned long);
typedef int (__cdecl *SpideyInput11SetVibrationFn)(
		float,
		float);
typedef void (__cdecl *SpideyInput11ShutdownFn)(void);

static SpideyInput11PollFn gSpideyInput11Poll = 0;
static SpideyInput11SetVibrationFn gSpideyInput11SetVibration = 0;
static SpideyInput11ShutdownFn gSpideyInput11Shutdown = 0;
static int gSpideyInput11BridgeReady = 0;
static SpideyInput11LegacyState gSpideyInput11State;
static int gSpideyInput11LastConnected = -1;
static unsigned long gSpideyInput11LastUser = 0xFFFFFFFFUL;
static int gSpideyRetailActionMapLogged = 0;

// Raw relative mouse motion is captured later by the existing retail
// DXINPUT_PollMouse compatibility wrapper, but the passive camera sampler is
// defined earlier in this translation unit. Keep the shared observation state
// here so the matching/VC6-era proxy sees declarations before first use.
static i32 gSpideyRawMouseDeltaX = 0;
static i32 gSpideyRawMouseDeltaY = 0;
static unsigned long gSpideyRawMousePollCount = 0;

static void SpideyLogRetailActionMap()
{
	if (gSpideyRetailActionMapLogged)
		return;

	// Original processControllerScreen/initActionMaps use 11 SActionMap
	// records at 0x00568690, stride 0x1C:
	// +0x00 action bit, +0x04 inline 16-byte label,
	// +0x14 keyboard mapping, +0x18 controller mapping.
	const unsigned char* base =
		(const unsigned char*)0x00568690;
	FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");

	if (!f)
		return;

	int valid =
		1;

	__try
	{
		for (int i = 0;
			 i < 11;
			 ++i)
		{
			const unsigned char* entry =
				base +
				i * 0x1C;
			const unsigned long action =
				*(const unsigned long*)(entry + 0x00);
			char label[17];
			memset(
				label,
				0,
				sizeof(label));
			memcpy(
				label,
				entry + 0x04,
				16);
			label[16] =
				0;

			const unsigned long keyboard =
				*(const unsigned long*)(entry + 0x14);
			const unsigned long controller =
				*(const unsigned long*)(entry + 0x18);

			fprintf(
				f,
				"retail_action_map index=%d action=0x%04lX label=%s keyboard=0x%08lX controller=0x%08lX passive=1\n",
				i,
				action,
				label,
				keyboard,
				controller);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		valid =
			0;
		fprintf(
			f,
				"retail_action_map event=read_fault base=0x00568690 passive=1\n");
	}

	fclose(f);

	if (valid)
	{
		gSpideyRetailActionMapLogged =
			1;
	}
}

static int SpideyProbeInput11Bridge()
{
	if (!gSpideyInput11Module)
	{
		gSpideyInput11Module =
			LoadLibraryA(
				"spidey_input11.dll");
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");

	if (!gSpideyInput11Module)
	{
		if (f)
		{
			fprintf(
				f,
				"input11_bridge load_failed error=%lu\n",
				(unsigned long)GetLastError());
			fclose(f);
		}
		return 0;
	}

	SpideyInput11GetAbiVersionFn getAbi =
		(SpideyInput11GetAbiVersionFn)GetProcAddress(
			gSpideyInput11Module,
			"SpideyInput11_GetAbiVersion");
	SpideyInput11GetBackendNameFn getName =
		(SpideyInput11GetBackendNameFn)GetProcAddress(
			gSpideyInput11Module,
			"SpideyInput11_GetBackendName");
	SpideyInput11ProbeFn probe =
		(SpideyInput11ProbeFn)GetProcAddress(
			gSpideyInput11Module,
			"SpideyInput11_Probe");

	gSpideyInput11Poll =
		(SpideyInput11PollFn)GetProcAddress(
			gSpideyInput11Module,
			"SpideyInput11_Poll");
	gSpideyInput11SetVibration =
		(SpideyInput11SetVibrationFn)GetProcAddress(
			gSpideyInput11Module,
			"SpideyInput11_SetVibration");
	gSpideyInput11Shutdown =
		(SpideyInput11ShutdownFn)GetProcAddress(
			gSpideyInput11Module,
			"SpideyInput11_Shutdown");

	if (!getAbi ||
		!getName ||
		!probe ||
		!gSpideyInput11Poll ||
		!gSpideyInput11SetVibration ||
		!gSpideyInput11Shutdown)
	{
		if (f)
		{
			fprintf(
				f,
				"input11_bridge exports_missing abi=0x%08lX name=0x%08lX probe=0x%08lX poll=0x%08lX vibration=0x%08lX shutdown=0x%08lX\n",
				(unsigned long)getAbi,
				(unsigned long)getName,
				(unsigned long)probe,
				(unsigned long)gSpideyInput11Poll,
				(unsigned long)gSpideyInput11SetVibration,
				(unsigned long)gSpideyInput11Shutdown);
			fclose(f);
		}
		return 0;
	}

	const unsigned long abi =
		getAbi();
	const char* name =
		getName();
	const int probeResult =
		probe();

	gSpideyInput11BridgeReady =
		abi == 1 &&
		probeResult != 0;

	if (f)
	{
		fprintf(
			f,
				"input11_bridge loaded module=0x%08lX abi=%lu expected=1 backend=%s probe=%d passive=1\n",
				(unsigned long)gSpideyInput11Module,
				abi,
				name ? name : "unknown",
				probeResult);
		fclose(f);
	}

	return gSpideyInput11BridgeReady;
}

int SpideyInput11PassivePoll(
		unsigned long frame)
{
	// Dump the original PC action descriptors once from retail memory.
	// This is independent of controller-helper readiness and is read-only.
	SpideyLogRetailActionMap();

	if (!gSpideyInput11BridgeReady ||
		!gSpideyInput11Poll)
	{
		return 0;
	}

	SpideyInput11LegacyState state;
	memset(
		&state,
		0,
		sizeof(state));

	const int pollResult =
		gSpideyInput11Poll(
			&state,
			sizeof(state));

	if (!pollResult)
		return 0;

	gSpideyInput11State =
		state;

	const int connected =
		state.connected ? 1 : 0;
	const int transition =
		gSpideyInput11LastConnected != connected ||
		(connected &&
		 gSpideyInput11LastUser != state.userIndex);
	const int periodic =
		connected &&
		(frame <= 5 ||
		 (frame % 300) == 0);

	if (transition ||
		periodic)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");
		if (f)
		{
			fprintf(
				f,
				"input11_state frame=%lu connected=%d family=%lu user=%lu packet=%lu buttons=0x%08lX move=%.4f,%.4f camera=%.4f,%.4f triggers=%.4f,%.4f sequence=%lu passive=1 legacy_analog_raw=%u,%u,%u,%u legacy_analog=%d,%d,%d,%d\n",
				frame,
				connected,
				state.deviceFamily,
				state.userIndex,
				state.packetNumber,
				state.buttons,
				state.moveX,
				state.moveY,
				state.cameraX,
				state.cameraY,
				state.leftTrigger,
				state.rightTrigger,
				state.sequence,
				(unsigned int)gSControl[0].RawAnalogueMoveForwardsBackwards,
				(unsigned int)gSControl[0].RawAnalogueMoveLeftRight,
				(unsigned int)gSControl[0].RawAnalogueAimForwardsBackwards,
				(unsigned int)gSControl[0].RawAnalogueAimLeftRight,
				(int)gSControl[0].AnalogueMoveForwardsBackwards,
				(int)gSControl[0].AnalogueMoveLeftRight,
				(int)gSControl[0].AnalogueAimForwardsBackwards,
				(int)gSControl[0].AnalogueAimLeftRight);
			fclose(f);
		}
	}

	gSpideyInput11LastConnected =
		connected;
	gSpideyInput11LastUser =
		state.userIndex;

	return 1;
}

int SpideyInput11IsConnected()
{
	return gSpideyInput11BridgeReady &&
		gSpideyInput11State.connected != 0;
}

const SpideyInput11LegacyState* SpideyInput11GetState()
{
	return &gSpideyInput11State;
}

static CCamera* gSpideyCameraTelemetryLastCamera = 0;
static int gSpideyCameraTelemetryLastMode = -9999;
static i32 gSpideyFrameMouseDeltaX = 0;
static i32 gSpideyFrameMouseDeltaY = 0;
static unsigned long gSpideyFrameMousePollCount = 0;
static unsigned long gSpideyCameraTelemetryLastIntentFrame = 0;

// Stage-A modern camera ownership. Retail still owns every camera mode except
// mode 3 (ordinary gameplay in this PC build), and even mode 3 remains retail
// until the player provides explicit mouse/right-stick camera intent.
static CCamera* gSpideyModernCameraOwner = 0;
static int gSpideyModernCameraActive = 0;
static int gSpideyModernCameraYaw = 0;
static int gSpideyModernCameraPitch = 0;
static int gSpideyModernCameraRadius = 0;
static int gSpideyModernCameraYDistance = -150;
static unsigned long gSpideyModernCameraInputSequence = 0;
static unsigned long gSpideyModernCameraLastConsumedSequence = 0xFFFFFFFFUL;
static unsigned long gSpideyModernCameraLastLogSequence = 0;

// During manual aim the ordinary mode-3 camera may still use Spider-Man as
// its orbit pivot, but the final view target must be independent from that
// pivot. Otherwise the center ray always passes through Spider-Man. Seed a
// free view from the current retail camera direction and move that view with
// mouse/right stick while keeping retail mode-3 position/collision generation.
static int gSpideyManualAimViewActive = 0;
static CPlayer* gSpideyManualAimViewPlayer = 0;
static int gSpideyManualAimViewBaseYaw = 0;
static int gSpideyManualAimViewBasePitch = 0;
static int gSpideyManualAimViewYawOffset = 0;
static int gSpideyManualAimViewPitchOffset = 0;
static unsigned long gSpideyManualAimViewLastLogSequence = 0;

static const int kSpideyManualAimMaxYawOffset = 768;
static const int kSpideyManualAimMaxPitchOffset = 1024;
static const double kSpideyAngleUnitsPerRadian =
	651.8986469044033;
static const double kSpideyRadiansPerAngleUnit =
	0.0015339807878856412;

static int SpideyManualAimClamp(
		int value,
		int minimum,
		int maximum)
{
	if (value < minimum)
		return minimum;
	if (value > maximum)
		return maximum;
	return value;
}

static int SpideyManualAimRadiansToUnits(
		double radians)
{
	const double units =
		radians *
		kSpideyAngleUnitsPerRadian;

	if (units >= 0.0)
		return (int)(units + 0.5);

	return (int)(units - 0.5);
}

static void SpideyManualAimSeedView(
		CPlayer* player,
		CCamera* camera)
{
	if (!player ||
		!camera)
	{
		gSpideyManualAimViewActive =
			0;
		gSpideyManualAimViewPlayer =
			0;
		return;
	}

	const double dx =
		(double)camera->field_144.vx -
		(double)camera->mPos.vx;
	const double dy =
		(double)camera->field_144.vy -
		(double)camera->mPos.vy;
	const double dz =
		(double)camera->field_144.vz -
		(double)camera->mPos.vz;
	const double horizontal =
		sqrt(
			dx * dx +
			dz * dz);

	if (horizontal < 1.0)
	{
		gSpideyManualAimViewBaseYaw =
			(int)camera->field_236 &
			0x0FFF;
		gSpideyManualAimViewBasePitch =
			0;
	}
	else
	{
		gSpideyManualAimViewBaseYaw =
			SpideyManualAimRadiansToUnits(
				atan2(
					-dx,
					-dz)) &
			0x0FFF;
		gSpideyManualAimViewBasePitch =
			SpideyManualAimRadiansToUnits(
				atan2(
					dy,
					horizontal));
	}

	gSpideyManualAimViewYawOffset =
		0;
	gSpideyManualAimViewPitchOffset =
		0;
	gSpideyManualAimViewActive =
		1;
	gSpideyManualAimViewPlayer =
		player;
	gSpideyManualAimViewLastLogSequence =
		0;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"CAMERA");
	if (f)
	{
		fprintf(
			f,
				"modern_manual_camera event=acquire player=0x%08lX camera=0x%08lX base_yaw=%d base_pitch=%d focus=%d,%d,%d camera_pos=%d,%d,%d max_offset=%d,%d\n",
				(unsigned long)player,
				(unsigned long)camera,
				gSpideyManualAimViewBaseYaw,
				gSpideyManualAimViewBasePitch,
				camera->field_144.vx,
				camera->field_144.vy,
				camera->field_144.vz,
				camera->mPos.vx,
				camera->mPos.vy,
				camera->mPos.vz,
				kSpideyManualAimMaxYawOffset,
				kSpideyManualAimMaxPitchOffset);
		fclose(f);
	}
}

static void SpideyManualAimReleaseView(
		const char* reason)
{
	if (!gSpideyManualAimViewActive)
		return;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"CAMERA");
	if (f)
	{
		fprintf(
			f,
				"modern_manual_camera event=release reason=%s player=0x%08lX base_yaw=%d base_pitch=%d offset=%d,%d\n",
				reason ? reason : "unknown",
				(unsigned long)gSpideyManualAimViewPlayer,
				gSpideyManualAimViewBaseYaw,
				gSpideyManualAimViewBasePitch,
				gSpideyManualAimViewYawOffset,
				gSpideyManualAimViewPitchOffset);
		fclose(f);
	}

	gSpideyManualAimViewActive =
		0;
	gSpideyManualAimViewPlayer =
		0;
	gSpideyManualAimViewYawOffset =
		0;
	gSpideyManualAimViewPitchOffset =
		0;
	gSpideyManualAimViewLastLogSequence =
		0;
}

static const int kSpideyModernCameraMouseYawScale = 3;
static const int kSpideyModernCameraMousePitchScale = 2;
static const int kSpideyModernCameraStickYawPerFrame = 32;
static const int kSpideyModernCameraStickPitchPerFrame = 7;
static const int kSpideyModernCameraMaxPitch = 1024;
static const int kSpideyModernCameraCollisionMarginUnits = 24;
static const int kSpideyModernCameraCollisionStartInsetUnits = 16;

static const char* SpideyCameraModeName(
		int mode)
{
	switch (mode)
	{
		case CAMERAMODE_NOTHING: return "NOTHING";
		case CAMERAMODE_NORMAL: return "NORMAL";
		case CAMERAMODE_NO_BIG_AIR: return "NO_BIG_AIR";
		case CAMERAMODE_DEMO: return "DEMO";
		case CAMERAMODE_START: return "START";
		case CAMERAMODE_FAR: return "FAR";
		case CAMERAMODE_OVERHEAD: return "OVERHEAD";
		case CAMERAMODE_FRONT: return "FRONT";
		case CAMERAMODE_IDLE: return "IDLE";
		case CAMERAMODE_FLYING: return "FLYING";
		case CAMERAMODE_FUNKYFLYING: return "FUNKYFLYING";
		case CAMERAMODE_ROLLERCOASTER: return "ROLLERCOASTER";
		case CAMERAMODE_PAN: return "PAN";
		case CAMERAMODE_ITSYLOOKDOWN: return "ITSYLOOKDOWN";
		case CAMERAMODE_ITSYLOOKUP: return "ITSYLOOKUP";
		case CAMERAMODE_LOOSE: return "LOOSE";
		case CAMERAMODE_USER: return "USER";
		case CAMERAMODE_LOOKAROUND: return "LOOKAROUND";
		case CAMERAMODE_UPSIDETEST: return "UPSIDETEST";
		case CAMERAMODE_BOSSBEAST: return "BOSSBEAST";
		case CAMERAMODE_BOSSWAR: return "BOSSWAR";
		case CAMERAMODE_BOSSTANK: return "BOSSTANK";
		case CAMERAMODE_DEBUG: return "DEBUG";
		case CAMERAMODE_COMPETITIONINTRO: return "COMPETITIONINTRO";
		default: return "UNKNOWN";
	}
}

static int SpideyModernCameraSignedAngle(
		int angle)
{
	angle &=
		0x0FFF;
	if (angle > 2048)
		angle -= 4096;
	return angle;
}

static int SpideyClampModernCameraPitch(
		int pitch)
{
	if (pitch <
		-kSpideyModernCameraMaxPitch)
	{
		return -kSpideyModernCameraMaxPitch;
	}

	if (pitch >
		kSpideyModernCameraMaxPitch)
	{
		return kSpideyModernCameraMaxPitch;
	}

	return pitch;
}

static int SpideyRoundCameraDouble(
		double value)
{
	if (value >= 0.0)
		return (int)(value + 0.5);
	return (int)(value - 0.5);
}

static unsigned long gSpideyModernCameraCollisionChecks = 0;
static unsigned long gSpideyModernCameraCollisionHits = 0;
static unsigned long gSpideyModernCameraCollisionLogs = 0;

static int SpideyModernCameraClipToWorld(
		CCamera* camera,
		CPlayer* player)
{
	if (!camera)
		return 0;

	CVector start =
		camera->field_144;

	// Begin slightly inside the playable side of Spider-Man's current
	// surface. This prevents a floor/ceiling/wall contact from starting the
	// camera ray exactly on the plane that is supposed to constrain it.
	if (player)
	{
		start.vx +=
			player->field_C84.vx *
			kSpideyModernCameraCollisionStartInsetUnits;
		start.vy +=
			player->field_C84.vy *
			kSpideyModernCameraCollisionStartInsetUnits;
		start.vz +=
			player->field_C84.vz *
			kSpideyModernCameraCollisionStartInsetUnits;
	}

	const CVector desired =
		camera->mPos;

	if (start.vx == desired.vx &&
		start.vy == desired.vy &&
		start.vz == desired.vz)
	{
		return 0;
	}

	SLineInfo lineInfo;
	lineInfo.StartCoords =
		start;
	lineInfo.EndCoords =
		desired;

	typedef void (__cdecl *SpideyRetailInitLineInfoFn)(
		SLineInfo*);
	typedef void (__cdecl *SpideyRetailZoneLineFn)(
		SLineInfo*,
		i32);

	SpideyRetailInitLineInfoFn initLine =
		(SpideyRetailInitLineInfoFn)0x004524C0;
	SpideyRetailZoneLineFn lineToWorld =
		(SpideyRetailZoneLineFn)0x004549A0;

	initLine(
		&lineInfo);
	lineInfo.RecordTriggerZoneHits =
		0;
	lineToWorld(
		&lineInfo,
		1);
	++gSpideyModernCameraCollisionChecks;

	if (!lineInfo.pItem)
		return 0;

	const double dx =
		(double)lineInfo.Position.vx -
		(double)start.vx;
	const double dy =
		(double)lineInfo.Position.vy -
		(double)start.vy;
	const double dz =
		(double)lineInfo.Position.vz -
		(double)start.vz;
	const double length =
		sqrt(
			dx * dx +
			dy * dy +
			dz * dz);

	if (length <= 1.0)
	{
		camera->mPos =
			start;
	}
	else
	{
		const double margin =
			(double)(
				kSpideyModernCameraCollisionMarginUnits *
				4096);
		double keptLength =
			length -
			margin;
		if (keptLength < 0.0)
			keptLength = 0.0;

		const double scale =
			keptLength /
			length;

		camera->mPos.vx =
			start.vx +
			SpideyRoundCameraDouble(
				dx * scale);
		camera->mPos.vy =
			start.vy +
			SpideyRoundCameraDouble(
				dy * scale);
		camera->mPos.vz =
			start.vz +
			SpideyRoundCameraDouble(
				dz * scale);
	}

	++gSpideyModernCameraCollisionHits;

	if (gSpideyModernCameraCollisionLogs < 64)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_camera_collision hit=%lu check=%lu pitch=%d start=%d,%d,%d desired=%d,%d,%d contact=%d,%d,%d clipped=%d,%d,%d distance=%ld margin_units=%d player_surface=%d,%d,%d\n",
				gSpideyModernCameraCollisionHits,
				gSpideyModernCameraCollisionChecks,
				gSpideyModernCameraPitch,
				start.vx,
				start.vy,
				start.vz,
				desired.vx,
				desired.vy,
				desired.vz,
				lineInfo.Position.vx,
				lineInfo.Position.vy,
				lineInfo.Position.vz,
				camera->mPos.vx,
				camera->mPos.vy,
				camera->mPos.vz,
				(long)lineInfo.Distance,
				kSpideyModernCameraCollisionMarginUnits,
				player ? player->field_C84.vx : 0,
				player ? player->field_C84.vy : 0,
				player ? player->field_C84.vz : 0);
			fclose(f);
		}
		++gSpideyModernCameraCollisionLogs;
	}

	return 1;
}

static int SpideyModernCameraHasIntent(
		int mouseX,
		int mouseY,
		const SpideyInput11LegacyState* input)
{
	if (mouseX ||
		mouseY)
	{
		return 1;
	}

	if (!input ||
		!input->connected)
	{
		return 0;
	}

	return
		input->cameraX > 0.05f ||
		input->cameraX < -0.05f ||
		input->cameraY > 0.05f ||
		input->cameraY < -0.05f;
}

static void SpideyModernCameraRelease(
		const char* reason,
		CCamera* camera,
		int mode)
{
	if (!gSpideyModernCameraActive &&
		!gSpideyModernCameraOwner)
	{
		return;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"CAMERA");
	if (f)
	{
		fprintf(
			f,
			"modern_camera event=release reason=%s camera=0x%08lX mode=%d yaw=%d pitch=%d radius=%d y_dist=%d\n",
			reason ? reason : "unknown",
			(unsigned long)camera,
			mode,
			gSpideyModernCameraYaw,
			gSpideyModernCameraPitch,
			gSpideyModernCameraRadius,
			gSpideyModernCameraYDistance);
		fclose(f);
	}

	gSpideyModernCameraOwner =
		0;
	gSpideyModernCameraActive =
		0;
	gSpideyModernCameraLastConsumedSequence =
		0xFFFFFFFFUL;
	gSpideyModernCameraLastLogSequence =
		0;

	// A scripted/cinematic camera takeover must not carry a stale manual-aim
	// free-view basis or a masked retail aim flag back into ordinary gameplay.
	if (gSpideyManualAimViewActive)
	{
		SpideyManualAimReleaseView(
			reason ? reason : "camera_release");
	}

	SpideyModernAimRestoreLocomotionState();
}

typedef void (__fastcall *SpideyRetailMode3CameraFn)(
		CCamera*,
		void*);

// Stage A: keep retail's normal mode-3 camera generator and everything after
// it (collision/orientation/shake/final publish), but replace the yaw and
// vertical orbit inputs immediately before CM_Normal consumes them.
static void __fastcall SpideyModernMode3Camera(
		CCamera* camera,
		void*)
{
	SpideyRetailMode3CameraFn retail =
		(SpideyRetailMode3CameraFn)0x00418E00;

	if (!camera)
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
	{
		if (gSpideyManualAimViewActive)
		{
			SpideyManualAimReleaseView(
				"mode3_wrapper_inactive");
		}
		SpideyModernAimRestoreLocomotionState();

		retail(
			camera,
			0);
		return;
	}

	const SpideyInput11LegacyState* input =
		SpideyInput11GetState();

	CPlayer* manualAimPlayer =
		0;
	__try
	{
		manualAimPlayer =
			*(CPlayer**)0x006A9038;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		manualAimPlayer =
			0;
	}

	const int manualAim =
		manualAimPlayer &&
		SpideyModernAimIsEffectivelyActive(
			manualAimPlayer);

	// Synthesized/scripted player control is authored against the retail camera
	// transform. Several synth worker opcodes are literal camera-relative stick
	// directions (for example type-3 code 10 = left). Modern mode-3 ownership
	// must therefore yield completely while field_1AC is active, otherwise a
	// user/free-look camera heading silently changes the scripted world route.
	if (manualAimPlayer &&
		manualAimPlayer->field_1AC)
	{
		SpideyModernCameraRelease(
			"scripted_player_input",
			camera,
			camera->mCameraMode);
		retail(
			camera,
			0);
		return;
	}

	// Modern manual aim now shares the ordinary mode-3 orbit camera instead of
	// accumulating an independent free-view yaw/pitch. A third-person shooter
	// should have one view orientation: mouse/right-stick rotates the actual
	// camera, and the reticle/web ray follows that camera.
	if (gSpideyManualAimViewActive)
	{
		SpideyManualAimReleaseView(
			"unified_tps_camera");
	}

	const int newInputFrame =
		gSpideyModernCameraLastConsumedSequence !=
			gSpideyModernCameraInputSequence;
	const int mouseX =
		newInputFrame ?
			gSpideyFrameMouseDeltaX :
			0;
	const int mouseY =
		newInputFrame ?
			gSpideyFrameMouseDeltaY :
			0;
	const float stickX =
		newInputFrame &&
		input &&
		input->connected ?
			input->cameraX :
			0.0f;
	const float stickY =
		newInputFrame &&
		input &&
		input->connected ?
			input->cameraY :
			0.0f;
	const int hasIntent =
		newInputFrame &&
		SpideyModernCameraHasIntent(
			mouseX,
			mouseY,
			input);

	if (gSpideyModernCameraOwner !=
		camera)
	{
		gSpideyModernCameraOwner =
			camera;
		gSpideyModernCameraActive =
			0;
		gSpideyModernCameraLastConsumedSequence =
			0xFFFFFFFFUL;
	}

	if (!gSpideyModernCameraActive &&
		hasIntent)
	{
		gSpideyModernCameraYaw =
			(int)camera->field_236 &
			0x0FFF;

		const int seedXZDistance =
			*(int*)0x00548860;
		const int seedYDistance =
			*(int*)0x00548864;
		gSpideyModernCameraRadius =
			M3dMaths_SquareRoot0(
				seedXZDistance *
					seedXZDistance +
				seedYDistance *
					seedYDistance);
		if (gSpideyModernCameraRadius < 1)
			gSpideyModernCameraRadius = 1;

		gSpideyModernCameraPitch =
			SpideyClampModernCameraPitch(
				SpideyModernCameraSignedAngle(
					ratan2(
						-seedYDistance,
						seedXZDistance)));
		gSpideyModernCameraYDistance =
			seedYDistance;
		gSpideyModernCameraActive =
			1;
		gSpideyModernCameraLastLogSequence =
			0;

		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_camera event=acquire camera=0x%08lX mode=3 seed_yaw=%d seed_pitch=%d seed_radius=%d seed_y_dist=%d mouse=%d,%d stick=%.4f,%.4f ownership=mode3_only\n",
				(unsigned long)camera,
				gSpideyModernCameraYaw,
				gSpideyModernCameraPitch,
				gSpideyModernCameraRadius,
				gSpideyModernCameraYDistance,
				mouseX,
				mouseY,
				(double)stickX,
				(double)stickY);
			fclose(f);
		}
	}

	if (newInputFrame)
	{
		gSpideyModernCameraLastConsumedSequence =
			gSpideyModernCameraInputSequence;
	}

	if (!gSpideyModernCameraActive)
	{
		retail(
			camera,
			0);
		return;
	}

	if (newInputFrame)
	{
		const int sensitivity =
			SpideyClampCameraSensitivityPercent(
				gSpideyCameraSensitivityPercent);
		const int yawDelta =
			(mouseX *
			 kSpideyModernCameraMouseYawScale *
			 sensitivity) /
				100 +
			(int)(
				stickX *
				(float)kSpideyModernCameraStickYawPerFrame *
				(float)sensitivity /
				100.0f);
		const int pitchDelta =
			((-mouseY) *
			 kSpideyModernCameraMousePitchScale *
			 sensitivity) /
				100 +
			(int)(
				stickY *
				(float)kSpideyModernCameraStickPitchPerFrame *
				(float)sensitivity /
				100.0f);

		// Unified TPS policy: aim never diverts look input into a second cursor
		// camera. Rotate the real mode-3 orbit in exactly the same way whether
		// manual aim is held or not.
		gSpideyModernCameraYaw =
			(gSpideyModernCameraYaw +
			 yawDelta) &
			0x0FFF;
		// The old implementation changed a Y offset and derived pitch from
		// that value, which could never reach a true vertical view. Preserve
		// the existing input direction while storing pitch explicitly:
		// increasing the old Y distance decreased the derived vertical angle.
		gSpideyModernCameraPitch =
			SpideyClampModernCameraPitch(
				gSpideyModernCameraPitch -
					pitchDelta);
	}

	const double pitchRadians =
		(double)gSpideyModernCameraPitch *
		kSpideyRadiansPerAngleUnit;
	int xzDistance =
		SpideyRoundCameraDouble(
			cos(pitchRadians) *
			(double)gSpideyModernCameraRadius);
	if (xzDistance < 1)
		xzDistance = 1;

	const int yDistance =
		SpideyRoundCameraDouble(
			-sin(pitchRadians) *
			(double)gSpideyModernCameraRadius);
	const int radialDistance =
		gSpideyModernCameraRadius;
	const int verticalAngle =
		gSpideyModernCameraPitch &
		0x0FFF;

	gSpideyModernCameraYDistance =
		yDistance;

	camera->field_236 =
		(i16)(
			gSpideyModernCameraYaw &
			0x0FFF);

	// CM_Normal reads 0x548858/0x54885C directly. Keep the companion Y
	// distance coherent as well so the retail collision/orientation stage
	// after CM_Normal sees the same modern vertical orbit request.
	*(int*)0x00548864 =
		yDistance;
	*(int*)0x00548860 =
		xzDistance;
	*(int*)0x0054885C =
		radialDistance;
	*(int*)0x00548858 =
		verticalAngle;

	const int requestedYaw =
		gSpideyModernCameraYaw;

	retail(
		camera,
		0);

	const int retailResultYaw =
		(int)camera->field_236 &
		0x0FFF;

	// Do not rewrite field_144 after CM_Normal in manual aim. Retail mode-3 now
	// owns the actual orbit/focus, and the manual reticle/web path consumes that
	// resulting camera ray. This keeps camera movement and aim in one space.

	const int shouldLog =
		hasIntent &&
		(gSpideyModernCameraLastLogSequence == 0 ||
		 gSpideyModernCameraInputSequence -
			gSpideyModernCameraLastLogSequence >= 60);

	if (manualAim &&
		shouldLog)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_manual_camera event=tps_orbit input_seq=%lu yaw=%d retail_yaw=%d y_dist=%d camera_pos=%d,%d,%d camera_focus=%d,%d,%d mouse=%d,%d stick=%.4f,%.4f reticle_policy=camera_ray\n",
				gSpideyModernCameraInputSequence,
				requestedYaw,
				retailResultYaw,
				yDistance,
				camera->mPos.vx,
				camera->mPos.vy,
				camera->mPos.vz,
				camera->field_144.vx,
				camera->field_144.vy,
				camera->field_144.vz,
				mouseX,
				mouseY,
				(double)stickX,
				(double)stickY);
			fclose(f);
		}
	}

	if (shouldLog ||
		retailResultYaw !=
			requestedYaw)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_camera event=update camera=0x%08lX mode=3 input_seq=%lu yaw=%d retail_yaw=%d y_dist=%d xz_dist=%d vertical_angle=%d radius=%d mouse=%d,%d stick=%.4f,%.4f sensitivity=%d retail_overrode_yaw=%d manual_aim=%d manual_tps_unified=1 manual_view_active=%d manual_offset=%d,%d\n",
				(unsigned long)camera,
				gSpideyModernCameraInputSequence,
				requestedYaw,
				retailResultYaw,
				yDistance,
				xzDistance,
				verticalAngle,
				radialDistance,
				mouseX,
				mouseY,
				(double)stickX,
				(double)stickY,
				gSpideyCameraSensitivityPercent,
				retailResultYaw !=
					requestedYaw ? 1 : 0,
				manualAim,
				gSpideyManualAimViewActive,
				gSpideyManualAimViewYawOffset,
				gSpideyManualAimViewPitchOffset);
			fclose(f);
		}

		if (shouldLog)
		{
			gSpideyModernCameraLastLogSequence =
				gSpideyModernCameraInputSequence;
		}
	}
}

typedef void (__fastcall *SpideyRetailLoadIntoMikeCameraFn)(
		CCamera*,
		void*);

static unsigned long gSpideyManualAimPublishCalls = 0;
static unsigned long gSpideyManualAimPublishApplied = 0;

static void __fastcall SpideyModernAimLoadIntoMikeCamera(
		CCamera* camera,
		void*)
{
	SpideyRetailLoadIntoMikeCameraFn retail =
		(SpideyRetailLoadIntoMikeCameraFn)0x00416A20;

	if (!camera ||
		!gSpideyManualAimViewActive ||
		!gSpideyManualAimViewPlayer ||
		camera->mCameraMode !=
			CAMERAMODE_DEMO ||
		!SpideyModernAimIsEffectivelyActive(
			gSpideyManualAimViewPlayer))
	{
		retail(
			camera,
			0);
		return;
	}

	++gSpideyManualAimPublishCalls;

	// CM_Normal and the shared 0x00416B10 post-process have already generated
	// the retail position/collision state by this point. Only replace the
	// orientation used for the final gMikeCamera publish. This is deliberately
	// temporary so the next frame's retail camera interpolation remains intact.
	CQuat savedOrientation =
		camera->field_214;
	const i16 savedTransformHeading =
		camera->field_23A;

	SVECTOR aim;
	aim.vx =
		0;
	aim.vy =
		0;
	aim.vz =
		0;
	Utils_CalcAim(
		(CSVector*)&aim,
		&camera->mPos,
		&camera->field_144);

	MATRIX manualTransform;
	M3dMaths_RotMatrixYXZ(
		&aim,
		&manualTransform);
	MToQ(
		manualTransform,
		camera->field_214);

	retail(
		camera,
		0);

	const i16 publishedTransformHeading =
		camera->field_23A;

	camera->field_214 =
		savedOrientation;
	camera->field_23A =
		savedTransformHeading;

	++gSpideyManualAimPublishApplied;

	if (gSpideyManualAimPublishApplied <= 6 ||
		(gSpideyManualAimPublishApplied % 60) == 0)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"modern_manual_camera event=publish call=%lu applied=%lu camera=0x%08lX aim_angles=%d,%d,%d pos=%d,%d,%d focus=%d,%d,%d published_heading=%d restored_heading=%d\n",
				gSpideyManualAimPublishCalls,
				gSpideyManualAimPublishApplied,
				(unsigned long)camera,
				(int)aim.vx,
				(int)aim.vy,
				(int)aim.vz,
				camera->mPos.vx,
				camera->mPos.vy,
				camera->mPos.vz,
				camera->field_144.vx,
				camera->field_144.vy,
				camera->field_144.vz,
				(int)publishedTransformHeading,
				(int)savedTransformHeading);
			fclose(f);
		}
	}
}

typedef void (__fastcall *SpideyRetailCameraPostprocessFn)(
		CCamera*,
		void*);

static unsigned long gSpideyManualAimFramingCalls = 0;

static void __fastcall SpideyModernAimCameraPostprocess(
		CCamera* camera,
		void*)
{
	SpideyRetailCameraPostprocessFn retail =
		(SpideyRetailCameraPostprocessFn)0x00416B10;

	if (!camera)
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
		0;
	__try
	{
		player =
			*(CPlayer**)0x006A9038;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		player =
			0;
	}

	const int manualAim =
		player &&
		camera->mCameraMode ==
			CAMERAMODE_DEMO &&
		SpideyModernAimIsEffectivelyActive(
			player);

	if (manualAim)
	{
		// CM_Normal has already chosen the actual orbit position and completed
		// its mode-specific work. Move only the look target upward before the
		// shared retail post-process computes collision/orientation. Leaving
		// this framed focus in field_144 also keeps the reticle/web ray in the
		// same camera space for the rest of the frame.
		camera->field_144 =
			SpideyModernAimFramedFocus(
				player,
				camera);
	}

	retail(
		camera,
		0);

	// MoveToDesiredPos is the retail routine wrapped here. It can change
	// camera->mPos after mode-3 has generated its desired orbit position.
	// Clamp *after* that final move so the rendered camera itself cannot cross
	// a wall/floor/ceiling. The caller immediately runs Utils_CalcAim with
	// camera->mPos + field_144, so retail orientation is rebuilt from this
	// clipped position before publish.
	if (gSpideyModernCameraActive &&
		camera->mCameraMode ==
			CAMERAMODE_DEMO)
	{
		SpideyModernCameraClipToWorld(
			camera,
			player);
	}

	if (manualAim)
	{
		// SetupLookaroundCamera runs earlier in SpideyAI0, before this frame's
		// camera orbit/post-process. Its field_DC0 value is therefore one
		// camera update stale during fast look input. Rebuild the aim point
		// here from the final current-frame camera/framed focus so the rendered
		// reticle cannot chase the camera by one frame.
		const int postCameraReticleApplied =
			SpideyModernAimApplyCameraPoint(
				player,
				camera);

		++gSpideyManualAimFramingCalls;

		if (gSpideyManualAimFramingCalls <= 6 ||
			(gSpideyManualAimFramingCalls % 60) == 0)
		{
			FILE* f =
				SpideyOpenConsolidatedLog(
					"CAMERA");
			if (f)
			{
				fprintf(
					f,
					"modern_manual_camera event=framing call=%lu camera=0x%08lX focus=%d,%d,%d body=%d,%d,%d framing_up_units=%d heading=%d transform_heading=%d post_camera_reticle=%d reticle_point=%d,%d,%d\n",
					gSpideyManualAimFramingCalls,
					(unsigned long)camera,
					camera->field_144.vx,
					camera->field_144.vy,
					camera->field_144.vz,
					player->mPos.vx,
					player->mPos.vy,
					player->mPos.vz,
					kSpideyManualAimFocusHeightUnits,
					(int)camera->field_236,
					(int)camera->field_23A,
					postCameraReticleApplied,
					player->field_DC0.vx,
					player->field_DC0.vy,
					player->field_DC0.vz);
				fclose(f);
			}
		}
	}
}

static void SpideyInstallModernCameraCompat()
{
	const int installed =
		SpideyPatchDirectCall(
			0x00418414,
			0x00418E00,
			(void*)&SpideyModernMode3Camera,
			"modern_camera_mode3");
	const int manualFramingInstalled =
		SpideyPatchDirectCall(
			0x00418458,
			0x00416B10,
			(void*)&SpideyModernAimCameraPostprocess,
			"modern_manual_camera_framing");
	// Unified TPS manual aim no longer needs a second final-orientation shim.
	// Leave the retail LoadIntoMikeCamera call untouched so the same mode-3
	// orbit camera that moved above is exactly what rendering receives.
	const int manualPublishInstalled =
		0;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"CAMERA");
	if (f)
	{
		fprintf(
			f,
			"modern_camera_install installed=%d call=0x00418414 retail_mode3=0x00418E00 ownership=mode3_only activation=input_intent mouse=relative_directinput stick=input11_right sensitivity_percent=%d sensitivity_range=%d-%d pitch_limit_units=%d pitch_limit_degrees=90 collision=world_arm_clip collision_margin_units=%d collision_start_inset_units=%d manual_aim_free_view=0 manual_tps_unified=1 manual_yaw_offset_limit=%d manual_pitch_offset_limit=%d manual_focus=framed_above_body manual_framing=%d manual_framing_call=0x00418458 framing_up_units=%d manual_publish=%d manual_publish_call=retail_untouched_0x0041865F retail_publish=0x00416A20\n",
			installed,
			gSpideyCameraSensitivityPercent,
			kSpideyCameraSensitivityMinPercent,
			kSpideyCameraSensitivityMaxPercent,
			kSpideyModernCameraMaxPitch,
			kSpideyModernCameraCollisionMarginUnits,
			kSpideyModernCameraCollisionStartInsetUnits,
			kSpideyManualAimMaxYawOffset,
			kSpideyManualAimMaxPitchOffset,
			manualFramingInstalled,
			kSpideyManualAimFocusHeightUnits,
			manualPublishInstalled);
		fclose(f);
	}
}


typedef void (__cdecl *SpideyRetailDisplayQuadBitListFn)(
		void**);
typedef void (__cdecl *SpideyRetailSetRotMatrixFn)(
		MATRIX*);
typedef void (__cdecl *SpideyRetailMatrix4x4MulFn)(
		float*,
		const float*,
		const float*);

typedef void (__cdecl *SpideyRetailZeroGteTranslationFn)(void);

static unsigned long gSpideyQuadBitCameraRestoreCalls =
	0;
static unsigned long gSpideyQuadBitDcxMismatchCalls =
	0;
static unsigned long gSpideyQuadBitGteTranslationNonZeroCalls = 0;
static unsigned long gSpideyQuadBitGteTranslationProbeSamples = 0;
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
	SpideyRetailZeroGteTranslationFn zeroGteTranslation =
		(SpideyRetailZeroGteTranslationFn)0x0046E460;
	SpideyRetailDisplayQuadBitListFn retail =
		(SpideyRetailDisplayQuadBitListFn)0x004097E0;

	MATRIX* activeCameraView =
		(MATRIX*)0x0056F224;
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

	const int quadBitPreTransX = *(volatile int*)0x00610B34;
	const int quadBitPreTransY = *(volatile int*)0x00610B38;
	const int quadBitPreTransZ = *(volatile int*)0x00610B3C;
	if (quadBitPreTransX || quadBitPreTransY || quadBitPreTransZ)
		++gSpideyQuadBitGteTranslationNonZeroCalls;

	if (gSpideyQuadBitGteTranslationProbeSamples < 64 &&
		(quadBitPreTransX || quadBitPreTransY || quadBitPreTransZ ||
		 gSpideyQuadBitGteTranslationProbeSamples < 8))
	{
		FILE* log = SpideyOpenConsolidatedLog("DRAW");
		if (log)
		{
			fprintf(
				log,
				"quadbit_gte_state sample=%lu restore_call=%lu pre_trans=%d,%d,%d camera=%d,%d,%d list=0x%08lX nonzero_total=%lu action=zero_before_retail\n",
				gSpideyQuadBitGteTranslationProbeSamples,
				gSpideyQuadBitCameraRestoreCalls,
				quadBitPreTransX,
				quadBitPreTransY,
				quadBitPreTransZ,
				*(volatile int*)0x0056F1B4,
				*(volatile int*)0x0056F1B8,
				*(volatile int*)0x0056F1BC,
				(unsigned long)(list ? *list : 0),
				gSpideyQuadBitGteTranslationNonZeroCalls);
			fclose(log);
		}
		++gSpideyQuadBitGteTranslationProbeSamples;
	}

	setRotMatrix(
		activeCameraView);
	zeroGteTranslation();
	retail(
		list);
}


static void SpideyInstallQuadBitCameraAnchorCompat()
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
	unsigned char* pushOpcode =
		(unsigned char*)0x004081D4;
	unsigned long* displayPointer =
		(unsigned long*)0x004081D5;
	const unsigned long expectedRetail =
		0x004097E0;
	int installed =
		0;
	const char* reason =
		"ok";

	if (*pushOpcode != 0x68)
	{
		reason =
			"opcode";
	}
	else if (*displayPointer !=
			expectedRetail)
	{
		reason =
			"target";
	}
	else
	{
		*displayPointer =
			(unsigned long)&SpideyDisplayQuadBitListCameraAnchored;

		FlushInstructionCache(
			GetCurrentProcess(),
			pushOpcode,
			5);
		installed =
			1;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"DRAW");
	if (f)
	{
		fprintf(
			f,
			"quadbit_camera_anchor installed=%d registration_push=0x004081D4 retail_display=0x004097E0 wrapper=0x%08lX camera_view=0x0056F224 gte_set_rot=0x0046D7B0 gte_zero_trans=0x0046E460 horplus_calls=%d,%d horplus_qpoly_wrapper=0x%08lX reason=%s\n",
			installed,
			(unsigned long)&SpideyDisplayQuadBitListCameraAnchored,
			horPlusCallOne,
			horPlusCallTwo,
			(unsigned long)&SpideyQuadBitQPoly3DHorPlus,
			reason);
		fclose(f);
	}
}


typedef CBody* (__fastcall *SpideyRetailSelectTargetBaddyFn)(
		CPlayer*,
		void*,
		int,
		int,
		int,
		int);
typedef void (__cdecl *SpideyRetailQToMFn)(
		CQuat*,
		MATRIX*);
typedef int (__cdecl *SpideyRetailLineOfSightFn)(
		CVector*,
		CVector*,
		CVector*,
		int);

static CBody* gSpideyCameraWebTargetLastTarget = 0;
static unsigned long gSpideyCameraWebTargetCalls = 0;
static unsigned long gSpideyCameraModernScanCalls = 0;
static unsigned long gSpideyCameraCheckWebShotCalls = 0;

// Retail SelectTargetBaddy scores each candidate after transforming the
// player->candidate vector through player + 0x89C. For web auto-aim only,
// temporarily provide the active render camera's rotation matrix there.
// Candidate eligibility, range weighting, LOS and final target selection all
// remain inside the untouched retail scorer.
static CBody* SpideyCameraSelectModernTarget(
		CPlayer* player,
		CCamera* camera,
		int rangeArg,
		int coneArg,
		SpideyRetailLineOfSightFn lineOfSight,
		int* outListNodes,
		int* outEligibleCount,
		int* outCandidateCount,
		double* outScore)
{
	if (outListNodes)
		*outListNodes =
			0;
	if (outEligibleCount)
		*outEligibleCount =
			0;
	if (outCandidateCount)
		*outCandidateCount =
			0;
	if (outScore)
		*outScore =
			0.0;

	if (!player ||
		!camera ||
		!lineOfSight)
	{
		return 0;
	}

	const double forwardX =
		(double)camera->field_144.vx -
		(double)camera->mPos.vx;
	const double forwardY =
		(double)camera->field_144.vy -
		(double)camera->mPos.vy;
	const double forwardZ =
		(double)camera->field_144.vz -
		(double)camera->mPos.vz;
	const double forwardLengthSquared =
		forwardX * forwardX +
		forwardY * forwardY +
		forwardZ * forwardZ;

	if (forwardLengthSquared <=
		1.0)
	{
		return 0;
	}

	int range =
		rangeArg;
	if (range <= 0)
		range =
			2048;

	double cone =
		(double)coneArg /
		4096.0;
	if (cone <= 0.0 ||
		cone > 1.0)
	{
		cone =
			2896.0 /
			4096.0;
	}
	const double coneSquared =
		cone * cone;
	const double rangeSquared =
		(double)range *
		(double)range;

	CBody* best =
		0;
	double bestScore =
		-1.0;
	double bestPlayerDistanceSquared =
		0.0;
	int listNodes =
		0;
	int eligible =
		0;
	int candidates =
		0;

	__try
	{
		// Canonical SelectTargetBaddy @ 0x004C8410 starts from the body-list
		// head stored at 0x0056E990 and advances through CItem::mNextItem
		// (+0x20). G_MECHLIST is the player/mech list at 0x006A9038 and is
		// not the retail auto-aim candidate universe.
		CBody* candidate =
			*(CBody**)0x0056E990;

		// Fail closed on a corrupted list rather than walking forever.
		for (int guard = 0;
			 candidate &&
			 guard < 1024;
			 ++guard)
		{
			CBody* next =
				(CBody*)candidate->mNextItem;
			++listNodes;

			if (candidate != player &&
				candidate->mRMinor != 0 &&
				(candidate->mCBodyFlags &
				 CBODY_TARGETTABLE) &&
				!(candidate->mCBodyFlags &
				  CBODY_ZOMBIE))
			{
				++eligible;

				const double playerDx =
					((double)candidate->mPos.vx -
					 (double)player->mPos.vx) /
					4096.0;
				const double playerDy =
					((double)candidate->mPos.vy -
					 (double)player->mPos.vy) /
					4096.0;
				const double playerDz =
					((double)candidate->mPos.vz -
					 (double)player->mPos.vz) /
					4096.0;
				const double playerDistanceSquared =
					playerDx * playerDx +
					playerDy * playerDy +
					playerDz * playerDz;

				if (playerDistanceSquared <
					rangeSquared)
				{
					const double cameraDx =
						(double)candidate->mPos.vx -
						(double)camera->mPos.vx;
					const double cameraDy =
						(double)candidate->mPos.vy -
						(double)camera->mPos.vy;
					const double cameraDz =
						(double)candidate->mPos.vz -
						(double)camera->mPos.vz;
					const double cameraLengthSquared =
						cameraDx * cameraDx +
						cameraDy * cameraDy +
						cameraDz * cameraDz;

					if (cameraLengthSquared >
						1.0)
					{
						const double dot =
							forwardX * cameraDx +
							forwardY * cameraDy +
							forwardZ * cameraDz;

						if (dot > 0.0)
						{
							const double score =
								(dot * dot) /
								(forwardLengthSquared *
								 cameraLengthSquared);

							if (score >=
								coneSquared)
							{
								++candidates;

								if (lineOfSight(
										&player->mPos,
										&candidate->mPos,
										0,
										0))
								{
									if (!best ||
										score >
											bestScore +
											0.000001 ||
										(score >=
											bestScore -
											0.000001 &&
										 playerDistanceSquared <
											bestPlayerDistanceSquared))
									{
										best =
											candidate;
										bestScore =
											score;
										bestPlayerDistanceSquared =
											playerDistanceSquared;
									}
								}
							}
						}
					}
				}
			}

			candidate =
				next;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		best =
			0;
		bestScore =
			-1.0;
	}

	if (outListNodes)
		*outListNodes =
			listNodes;
	if (outEligibleCount)
		*outEligibleCount =
			eligible;
	if (outCandidateCount)
		*outCandidateCount =
			candidates;
	if (outScore &&
		best)
	{
		*outScore =
			bestScore;
	}

	return best;
}

// Retail SelectTargetBaddy remains available as a compatibility fallback for
// unusual targetable bodies. Normal mode-3 enemy auto-aim now uses the visible
// camera ray directly, with the same basic retail target-table/zombie/range/LOS
// eligibility rules. This removes the player-origin/camera-origin impedance
// mismatch that caused a centered target to appear and disappear on adjacent
// selector calls.
static CBody* SpideyCameraSelectTargetBaddyCommon(
		CPlayer* player,
		int arg1,
		int arg2,
		int arg3,
		int arg4,
		const char* callSource)
{
	SpideyRetailSelectTargetBaddyFn retail =
		(SpideyRetailSelectTargetBaddyFn)0x004C8410;
	SpideyRetailQToMFn retailQToM =
		(SpideyRetailQToMFn)0x0047C7F0;
	SpideyRetailLineOfSightFn retailLineOfSight =
		(SpideyRetailLineOfSightFn)0x004E67A0;

	if (!player)
	{
		return retail(
			player,
			0,
			arg1,
			arg2,
			arg3,
			arg4);
	}

	CCamera* camera =
		*(CCamera**)0x0056F3B8;
	const int useCamera =
		camera &&
		camera->mCameraMode ==
			CAMERAMODE_DEMO;

	CBody* target =
		0;
	const char* selectionSource =
		"retail_player_transform";
	int cameraOriginCandidate =
		0;
	int cameraOriginPlayerLos =
		0;
	int cameraScanListNodes =
		0;
	int cameraScanEligible =
		0;
	int cameraScanCandidates =
		0;
	double cameraScanScore =
		0.0;

	if (!useCamera)
	{
		target =
			retail(
				player,
				0,
				arg1,
				arg2,
				arg3,
				arg4);
	}
	else
	{
		target =
			SpideyCameraSelectModernTarget(
				player,
				camera,
				arg1,
				arg2,
				retailLineOfSight,
				&cameraScanListNodes,
				&cameraScanEligible,
				&cameraScanCandidates,
				&cameraScanScore);

		if (target)
		{
			selectionSource =
				"modern_camera_scan";
			cameraOriginPlayerLos =
				1;
		}
		else
		{
			MATRIX playerTargetMatrix;
			MATRIX cameraTargetMatrix;
			CVector playerPosition;

			memcpy(
				&playerTargetMatrix,
				&player->field_89C,
				sizeof(playerTargetMatrix));
			playerPosition =
				player->mPos;

			retailQToM(
				&camera->field_214,
				&cameraTargetMatrix);

			cameraTargetMatrix.m[2][0] =
				-cameraTargetMatrix.m[2][0];
			cameraTargetMatrix.m[2][1] =
				-cameraTargetMatrix.m[2][1];
			cameraTargetMatrix.m[2][2] =
				-cameraTargetMatrix.m[2][2];

			memcpy(
				&player->field_89C,
				&cameraTargetMatrix,
				sizeof(cameraTargetMatrix));

			__try
			{
				player->mPos =
					camera->mPos;

				target =
					retail(
						player,
						0,
						arg1,
						arg2,
						arg3,
						arg4);

				cameraOriginCandidate =
					target ? 1 : 0;
			}
			__finally
			{
				player->mPos =
					playerPosition;
				memcpy(
					&player->field_89C,
					&playerTargetMatrix,
					sizeof(playerTargetMatrix));
			}

			if (target)
			{
				cameraOriginPlayerLos =
					retailLineOfSight(
						&player->mPos,
						&target->mPos,
						0,
						0) ?
						1 :
						0;

				if (!cameraOriginPlayerLos)
				{
					target =
						0;
				}
			}

			if (target)
			{
				selectionSource =
					"render_camera_origin_fallback";
			}
			else
			{
				memcpy(
					&player->field_89C,
					&cameraTargetMatrix,
					sizeof(cameraTargetMatrix));

				__try
				{
					target =
						retail(
							player,
							0,
							arg1,
							arg2,
							arg3,
							arg4);
				}
				__finally
				{
					memcpy(
						&player->field_89C,
						&playerTargetMatrix,
						sizeof(playerTargetMatrix));
				}

				selectionSource =
					target ?
						"render_camera_orientation_fallback" :
						"render_camera_no_target";
			}
		}
	}

	++gSpideyCameraWebTargetCalls;

	if (callSource &&
		strcmp(
			callSource,
			"check_web_shot") == 0)
	{
		++gSpideyCameraCheckWebShotCalls;
	}

	++gSpideyCameraModernScanCalls;

	if (target !=
			gSpideyCameraWebTargetLastTarget ||
		gSpideyCameraWebTargetCalls <= 4 ||
		(useCamera &&
		 (gSpideyCameraModernScanCalls %
		  120) == 0))
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"camera_web_target event=select call=%lu path=%s source=%s camera=0x%08lX mode=%d modern_active=%d camera_heading=%d target=0x%08lX camera_scan_nodes=%d camera_scan_eligible=%d camera_scan_candidates=%d camera_scan_score=%.6f camera_origin_candidate=%d player_los=%d args=%d,%d,%d,%d\n",
				gSpideyCameraWebTargetCalls,
				callSource ? callSource : "unknown",
				selectionSource,
				(unsigned long)camera,
				camera ?
					(int)camera->mCameraMode :
					-1,
				gSpideyModernCameraActive,
				camera ?
					((int)camera->field_23A &
					 0x0FFF) :
					-1,
				(unsigned long)target,
				cameraScanListNodes,
				cameraScanEligible,
				cameraScanCandidates,
				cameraScanScore,
				cameraOriginCandidate,
				cameraOriginPlayerLos,
				arg1,
				arg2,
				arg3,
				arg4);
			fclose(f);
		}

		gSpideyCameraWebTargetLastTarget =
			target;
	}

	return target;
}

static CBody* __fastcall SpideyCameraSelectTargetBaddyAutoAim(
		CPlayer* player,
		void*,
		int arg1,
		int arg2,
		int arg3,
		int arg4)
{
	return SpideyCameraSelectTargetBaddyCommon(
		player,
		arg1,
		arg2,
		arg3,
		arg4,
		"select_auto_aim");
}

static CBody* __fastcall SpideyCameraSelectTargetBaddyCheckWebShot(
		CPlayer* player,
		void*,
		int arg1,
		int arg2,
		int arg3,
		int arg4)
{
	return SpideyCameraSelectTargetBaddyCommon(
		player,
		arg1,
		arg2,
		arg3,
		arg4,
		"check_web_shot");
}

static void SpideyInstallCameraWebTargetingCompat()
{
	const int autoAimInstalled =
		SpideyPatchDirectCall(
			0x004C5B2F,
			0x004C8410,
			(void*)&SpideyCameraSelectTargetBaddyAutoAim,
			"camera_web_autoaim");
	const int checkWebShotInstalled =
		SpideyPatchDirectCall(
			0x004C09E2,
			0x004C8410,
			(void*)&SpideyCameraSelectTargetBaddyCheckWebShot,
			"camera_web_check_web_shot");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"CAMERA");
	if (f)
	{
		fprintf(
			f,
			"camera_web_target_install autoaim=%d autoaim_call=0x004C5B2F check_web_shot=%d check_web_shot_call=0x004C09E2 retail_select=0x004C8410 retail_qtom=0x0047C7F0 primary=modern_camera_scan retail_list=0x0056E990 filters=targettable_non_zombie_real_range_player_los cone=arg2_over_4096 scope=select_auto_aim_and_check_web_shot fallback=retail_camera_origin_then_orientation\n",
			autoAimInstalled,
			checkWebShotInstalled);
		fclose(f);
	}
}

static void SpideyShadowWorldSpaceProbeActor(
		CBody* body,
		const char* label,
		unsigned long frame)
{
	if (!body)
		return;

	int valid =
		0;
	int isSuper =
		0;
	int regionIndex =
		-1;
	int numParts =
		0;
	int modelVertices =
		0;
	int modelNormals =
		0;
	int modelFaces =
		0;
	int modelFlags =
		0;
	int sampleX =
		0;
	int sampleY =
		0;
	int sampleZ =
		0;
	int poseM00 =
		0;
	int poseM11 =
		0;
	int poseM22 =
		0;
	int poseTx =
		0;
	int poseTy =
		0;
	int poseTz =
		0;
	int bodyPosX =
		0;
	int bodyPosY =
		0;
	int bodyPosZ =
		0;
	int shadowPosX =
		0;
	int shadowPosY =
		0;
	int shadowPosZ =
		0;
	int shadowNormalX =
		0;
	int shadowNormalY =
		0;
	int shadowNormalZ =
		0;
	unsigned int shadowScale =
		0;
	unsigned long regionPtr =
		0;
	unsigned long modelPtr =
		0;
	unsigned long decompressedPtr =
		0;
	unsigned long posePtr =
		0;
	unsigned long selectedPosePtr =
		0;
	char regionName[10];
	memset(
		regionName,
		0,
		sizeof(regionName));

	__try
	{
		bodyPosX =
			body->mPos.vx;
		bodyPosY =
			body->mPos.vy;
		bodyPosZ =
			body->mPos.vz;
		shadowPosX =
			body->mShadowPos.vx;
		shadowPosY =
			body->mShadowPos.vy;
		shadowPosZ =
			body->mShadowPos.vz;
		shadowNormalX =
			(int)body->mShadowNormal.vx;
		shadowNormalY =
			(int)body->mShadowNormal.vy;
		shadowNormalZ =
			(int)body->mShadowNormal.vz;
		shadowScale =
			(unsigned int)body->mShadowScale;

		regionIndex =
			(int)body->mRegion;
		if (regionIndex < 0 ||
			regionIndex >= MAXPSX)
		{
			__leave;
		}

		SPSXRegion* region =
			&G_PSXREGION[regionIndex];
		regionPtr =
			(unsigned long)region;
		isSuper =
			region->IsSuper ? 1 : 0;

		memcpy(
			regionName,
			region->Filename,
			9);
		regionName[9] =
			0;

		if (!isSuper ||
			!region->ppModels ||
			region->NumParts == 0 ||
			region->NumParts > 96)
		{
			__leave;
		}

		CSuper* super =
			(CSuper*)body;
		numParts =
			(int)region->NumParts;
		decompressedPtr =
			(unsigned long)super->mpDecompressedFrame;
		posePtr =
			(unsigned long)super->mpPoseBuffer;

		SModel* model =
			region->ppModels[0];
		if (!model)
		{
			__leave;
		}

		modelPtr =
			(unsigned long)model;
		modelVertices =
			(int)model->NumVertices;
		modelNormals =
			(int)model->NumNormals;
		modelFaces =
			(int)model->NumFaces;
		modelFlags =
			(int)model->Flags;

		if (modelVertices <= 0 ||
			modelVertices > 4096 ||
			modelFaces <= 0 ||
			modelFaces > 8192)
		{
			__leave;
		}

		SVECTOR* localVertices =
			(SVECTOR*)&model->Vertices;
		sampleX =
			(int)localVertices[0].vx;
		sampleY =
			(int)localVertices[0].vy;
		sampleZ =
			(int)localVertices[0].vz;

		SMatrix* pose =
			super->mpPoseBuffer ?
				super->mpPoseBuffer :
				super->mpDecompressedFrame;
		if (pose)
		{
			selectedPosePtr =
				(unsigned long)pose;
			poseM00 =
				(int)pose[0].m[0][0];
			poseM11 =
				(int)pose[0].m[1][1];
			poseM22 =
				(int)pose[0].m[2][2];
			poseTx =
				(int)pose[0].t[0];
			poseTy =
				(int)pose[0].t[1];
			poseTz =
				(int)pose[0].t[2];
		}

		valid =
			1;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		valid =
			0;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"SHADOW");
	if (f)
	{
		fprintf(
			f,
			"world_space_probe frame=%lu label=%s body=0x%08lX valid=%d region=%d region_ptr=0x%08lX name=%s super=%d parts=%d model0=0x%08lX verts=%d normals=%d faces=%d model_flags=0x%04X local_v0=%d,%d,%d decompressed=0x%08lX pose=0x%08lX selected_pose=0x%08lX pose_diag=%d,%d,%d pose_t=%d,%d,%d body_pos=%d,%d,%d shadow_pos=%d,%d,%d shadow_normal=%d,%d,%d shadow_scale=%u\n",
			frame,
			label ? label : "unknown",
			(unsigned long)body,
			valid,
			regionIndex,
			regionPtr,
			regionName[0] ?
				regionName :
				"(none)",
			isSuper,
			numParts,
			modelPtr,
			modelVertices,
			modelNormals,
			modelFaces,
			modelFlags & 0xFFFF,
			sampleX,
			sampleY,
			sampleZ,
			decompressedPtr,
			posePtr,
			selectedPosePtr,
			poseM00,
			poseM11,
			poseM22,
			poseTx,
			poseTy,
			poseTz,
			bodyPosX,
			bodyPosY,
			bodyPosZ,
			shadowPosX,
			shadowPosY,
			shadowPosZ,
			shadowNormalX,
			shadowNormalY,
			shadowNormalZ,
			shadowScale);
		fclose(f);
	}
}

static void SpideyShadowWorldSpaceProbe(
		unsigned long frame)
{
	if (frame < 60 ||
		(frame % 300) != 0)
	{
		return;
	}

	CBody* mech =
		0;
	__try
	{
		mech =
			G_MECHLIST;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		mech =
			0;
	}

	SpideyShadowWorldSpaceProbeActor(
		mech,
		"mech_head",
		frame);

	CBody* target =
		gSpideyCameraWebTargetLastTarget;
	if (target &&
		target != mech)
	{
		SpideyShadowWorldSpaceProbeActor(
			target,
			"web_target",
			frame);
	}
}


static void SpideyCameraPassivePoll(
		unsigned long frame)
{
	// Snapshot and clear relative mouse motion once per completed frame.
	// The input wrapper may be called more than once before Flip, so accumulate
	// there and consume here.
	gSpideyFrameMouseDeltaX =
		gSpideyRawMouseDeltaX;
	gSpideyFrameMouseDeltaY =
		gSpideyRawMouseDeltaY;
	gSpideyFrameMousePollCount =
		gSpideyRawMousePollCount;
	gSpideyRawMouseDeltaX =
		0;
	gSpideyRawMouseDeltaY =
		0;
	gSpideyRawMousePollCount =
		0;
	++gSpideyModernCameraInputSequence;

	// 0x0056F3B8 is the retail active-camera pointer used by
	// CPlayer::PutCameraBehind. This sampler remains read-only; active modern
	// camera mutation happens only in the mode-3 wrapper above.
	CCamera* camera =
		*(CCamera**)0x0056F3B8;

	if (!camera)
	{
		if (gSpideyModernCameraActive ||
			gSpideyModernCameraOwner)
		{
			SpideyModernCameraRelease(
				"camera_detached",
				0,
				-1);
		}

		if (gSpideyCameraTelemetryLastCamera)
		{
			FILE* f = SpideyOpenConsolidatedLog(
		"CAMERA");
			if (f)
			{
				fprintf(
					f,
					"camera_state frame=%lu camera=0x00000000 event=detached passive=1\n",
					frame);
				fclose(f);
			}
		}

		gSpideyCameraTelemetryLastCamera =
			0;
		gSpideyCameraTelemetryLastMode =
			-9999;
		return;
	}

	int mode = -1;
	int pushedMode = -1;
	int x = 0;
	int y = 0;
	int z = 0;
	int focusX = 0;
	int focusY = 0;
	int focusZ = 0;
	int heading = 0;
	int transformHeading = 0;
	int zoom = 0;
	int collisionRayLR = 0;
	int collisionRayBack = 0;
	int xzDistance = 0;
	int yDistance = 0;
	int valid = 0;

	__try
	{
		unsigned char* bytes =
			(unsigned char*)camera;

		x = *(int*)(bytes + 0x008);
		y = *(int*)(bytes + 0x00C);
		z = *(int*)(bytes + 0x010);
		focusX = *(int*)(bytes + 0x144);
		focusY = *(int*)(bytes + 0x148);
		focusZ = *(int*)(bytes + 0x14C);
		zoom = *(int*)(bytes + 0x170);
		heading =
			(int)(*(short*)(bytes + 0x236)) &
			0x0FFF;
		transformHeading =
			(int)(*(short*)(bytes + 0x23A)) &
			0x0FFF;
		collisionRayLR =
			*(int*)(bytes + 0x264);
		collisionRayBack =
			*(int*)(bytes + 0x268);
		pushedMode =
			*(int*)(bytes + 0x280);
		mode =
			*(int*)(bytes + 0x2A0);

		// Retail GetCamXZDistance/GetCamYDistance read these exact globals.
		xzDistance =
			(int)*(short*)0x00548860;
		yDistance =
			(int)*(short*)0x00548864;

		valid =
			1;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		valid =
			0;
	}

	if (!valid)
	{
		if (camera != gSpideyCameraTelemetryLastCamera)
		{
			FILE* f = SpideyOpenConsolidatedLog(
		"CAMERA");
			if (f)
			{
				fprintf(
					f,
					"camera_state frame=%lu camera=0x%08lX event=read_fault passive=1\n",
					frame,
					(unsigned long)camera);
				fclose(f);
			}
		}

		gSpideyCameraTelemetryLastCamera =
			camera;
		return;
	}

	if (gSpideyModernCameraActive &&
		(camera != gSpideyModernCameraOwner ||
		 mode != CAMERAMODE_DEMO))
	{
		SpideyModernCameraRelease(
			camera != gSpideyModernCameraOwner ?
				"camera_changed" :
				"retail_mode_changed",
			camera,
			mode);
	}

	const int cameraChanged =
		camera != gSpideyCameraTelemetryLastCamera;
	const int modeChanged =
		mode != gSpideyCameraTelemetryLastMode;
	const SpideyInput11LegacyState* input =
		SpideyInput11GetState();
	const int modernController =
		input &&
		input->connected;
	const int mouseIntent =
		gSpideyFrameMouseDeltaX != 0 ||
		gSpideyFrameMouseDeltaY != 0;
	const int stickIntent =
		input &&
		(input->cameraX > 0.05f ||
		 input->cameraX < -0.05f ||
		 input->cameraY > 0.05f ||
		 input->cameraY < -0.05f);
	const int intentSample =
		(mouseIntent ||
		 stickIntent) &&
		(frame - gSpideyCameraTelemetryLastIntentFrame >= 60);
	const int periodic =
		frame <= 5 ||
		(frame % (modernController ? 60 : 300)) == 0;

	if (cameraChanged ||
		modeChanged ||
		intentSample ||
		periodic)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"CAMERA");
		if (f)
		{
			fprintf(
				f,
				"camera_state frame=%lu camera=0x%08lX event=%s mode=%d mode_name=%s pushed_mode=%d heading=%d transform_heading=%d pos=%d,%d,%d focus=%d,%d,%d xz_dist=%d y_dist=%d zoom=%d collision_rays=%d,%d input_connected=%d input_camera=%.4f,%.4f input_mouse=%d,%d mouse_polls=%lu passive=1\n",
				frame,
				(unsigned long)camera,
				cameraChanged ? "camera_change" :
					modeChanged ? "mode_change" :
					intentSample ? "input_intent" :
					"periodic",
				mode,
				SpideyCameraModeName(mode),
				pushedMode,
				heading,
				transformHeading,
				x,
				y,
				z,
				focusX,
				focusY,
				focusZ,
				xzDistance,
				yDistance,
				zoom,
				collisionRayLR,
				collisionRayBack,
				modernController,
				input ? input->cameraX : 0.0f,
				input ? input->cameraY : 0.0f,
				gSpideyFrameMouseDeltaX,
				gSpideyFrameMouseDeltaY,
				gSpideyFrameMousePollCount);
			fclose(f);
		}
	}

	if (intentSample)
	{
		gSpideyCameraTelemetryLastIntentFrame =
			frame;
	}

	gSpideyCameraTelemetryLastCamera =
		camera;
	gSpideyCameraTelemetryLastMode =
		mode;

	SpideyShadowWorldSpaceProbe(
		frame);
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
typedef int (__cdecl *SpideyRenderer11SetFullscreenStateFn)(
		int,
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
static SpideyRenderer11SetFullscreenStateFn gSpideyRenderer11SetFullscreenState = 0;
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
static int gSpideyRenderer11AppliedWindowMode = -1;
static unsigned long gSpideyRenderer11AppliedModeWidth = 0;
static unsigned long gSpideyRenderer11AppliedModeHeight = 0;
// A persisted Exclusive setting must not seize the display before the first
// fully replayable DX11 frame exists. Keep DXGI windowed through warmup, then
// enter true exclusive at the first authoritative shadow frame.
static int gSpideyRenderer11ExclusiveDeferred = 1;

static void SpideyApplyRendererWindowMode(
		const char* reason)
{
	if (!gSpideyRenderer11Initialized ||
		!gSpideyRenderer11SetFullscreenState)
	{
		return;
	}

	const int wantsExclusive =
		gSpideyWindowMode ==
			SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE ?
			1 :
			0;
	const int exclusive =
		wantsExclusive &&
		!gSpideyRenderer11ExclusiveDeferred ?
			1 :
			0;

	const unsigned long width =
		gSpideySelectedOutputWidth;
	const unsigned long height =
		gSpideySelectedOutputHeight;

	if (!gSpideyRenderer11ExclusiveDeferred &&
		gSpideyRenderer11AppliedWindowMode ==
			gSpideyWindowMode &&
		gSpideyRenderer11AppliedModeWidth ==
			width &&
		gSpideyRenderer11AppliedModeHeight ==
			height)
	{
		return;
	}

	const int result =
		gSpideyRenderer11SetFullscreenState(
			exclusive,
			width,
			height);

	if (result)
	{
		if (!wantsExclusive ||
			exclusive)
		{
			gSpideyRenderer11AppliedWindowMode =
				gSpideyWindowMode;
			gSpideyRenderer11AppliedModeWidth =
				width;
			gSpideyRenderer11AppliedModeHeight =
				height;
		}
		else
		{
			// Deliberately leave the requested mode unapplied so the first
			// authoritative DX11 frame retries this transition.
			gSpideyRenderer11AppliedWindowMode =
				-1;
			gSpideyRenderer11AppliedModeWidth =
				0;
			gSpideyRenderer11AppliedModeHeight =
				0;
		}
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"renderer11_window_mode reason=%s mode=%d label=%s exclusive=%d requested_exclusive=%d deferred=%d selected=%lux%lu result=%d\n",
			reason ? reason : "unknown",
			gSpideyWindowMode,
			gSpideyWindowModeLabels[
				gSpideyWindowMode],
			exclusive,
			wantsExclusive,
			gSpideyRenderer11ExclusiveDeferred,
			width,
			height,
			result);
		fclose(f);
	}
}

// @Ok
// @Ok
static void SpideyReleaseRendererExclusiveForCompatRebuild(
		const char* reason)
{
	const int hadExclusive =
		gSpideyRenderer11Initialized &&
		gSpideyRenderer11SetFullscreenState &&
		gSpideyRenderer11AppliedWindowMode ==
			SPIDEY_WINDOW_FULLSCREEN_EXCLUSIVE;

	int releaseResult =
		1;

	if (hadExclusive)
	{
		releaseResult =
			gSpideyRenderer11SetFullscreenState(
				0,
				gSpideySelectedOutputWidth,
				gSpideySelectedOutputHeight);

		if (releaseResult)
		{
			gSpideyRenderer11AppliedWindowMode =
				-1;
			gSpideyRenderer11AppliedModeWidth =
				0;
			gSpideyRenderer11AppliedModeHeight =
				0;
		}
	}

	// Compatibility producers can encounter surfaces that were invalidated
	// by an earlier DXGI-exclusive interval even after the current visible
	// mode has become Windowed/Borderless. Always repair those surfaces
	// before handing execution back to legacy movie/rebuild code.
	HRESULT primaryLost =
		S_OK;
	HRESULT sceneLost =
		S_OK;
	HRESULT primaryRestore =
		S_OK;
	HRESULT sceneRestore =
		S_OK;

	LPDIRECTDRAWSURFACE7 primary =
		*(LPDIRECTDRAWSURFACE7*)0x006B7904;
	LPDIRECTDRAWSURFACE7 scene =
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	if (primary)
	{
		primaryLost =
			primary->IsLost();
		if (primaryLost ==
			DDERR_SURFACELOST)
		{
			primaryRestore =
				primary->Restore();
		}
	}

	if (scene)
	{
		sceneLost =
			scene->IsLost();
		if (sceneLost ==
			DDERR_SURFACELOST)
		{
			sceneRestore =
				scene->Restore();
		}
	}

	const int movieFrameReason =
		reason &&
		strcmp(
			reason,
			"movie_frame") ==
			0;
	static unsigned long movieFrameProbeCalls =
		0;
	int shouldLog =
		1;

	if (movieFrameReason)
	{
		++movieFrameProbeCalls;
		shouldLog =
			movieFrameProbeCalls <=
				4 ||
			(movieFrameProbeCalls %
				3000) ==
				0 ||
			hadExclusive ||
			primaryLost ==
				DDERR_SURFACELOST ||
			sceneLost ==
				DDERR_SURFACELOST ||
			FAILED(primaryRestore) ||
			FAILED(sceneRestore);
	}

	if (shouldLog)
	{
		FILE* f = SpideyOpenConsolidatedLog(
			"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"renderer11_release_exclusive_for_compat reason=%s selected=%lux%lu had_exclusive=%d release_result=%d primary_lost=0x%08lX primary_restore=0x%08lX scene_lost=0x%08lX scene_restore=0x%08lX movie_probe_calls=%lu\n",
				reason ? reason : "unknown",
				gSpideySelectedOutputWidth,
				gSpideySelectedOutputHeight,
				hadExclusive,
				releaseResult,
				(unsigned long)primaryLost,
				(unsigned long)primaryRestore,
				(unsigned long)sceneLost,
				(unsigned long)sceneRestore,
				movieFrameProbeCalls);
			fclose(f);
		}
	}
}

static int SpideyProbeRenderer11Bridge()
{
	if (!gSpideyRenderer11Module)
	{
		gSpideyRenderer11Module =
			LoadLibraryA(
				"spidey_renderer11.dll");
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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

	gSpideyRenderer11SetFullscreenState =
		(SpideyRenderer11SetFullscreenStateFn)GetProcAddress(
			gSpideyRenderer11Module,
			"SpideyRenderer11_SetFullscreenState");

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
		!gSpideyRenderer11SetFullscreenState ||
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
				"renderer11_bridge exports_missing abi=0x%08lX name=0x%08lX probe=0x%08lX init=0x%08lX resize=0x%08lX fullscreen=0x%08lX present_pixels=0x%08lX present_hdc=0x%08lX update_tex=0x%08lX associate_tex=0x%08lX resolve_tex=0x%08lX transient_tex=0x%08lX shadow_clear=0x%08lX shadow_submit=0x%08lX shadow_end=0x%08lX shadow_continuous=0x%08lX present_shadow=0x%08lX release_tex=0x%08lX release_all=0x%08lX tex_count=0x%08lX shutdown=0x%08lX\n",
				(unsigned long)getAbi,
				(unsigned long)getName,
				(unsigned long)probe,
				(unsigned long)gSpideyRenderer11Initialize,
				(unsigned long)gSpideyRenderer11Resize,
				(unsigned long)gSpideyRenderer11SetFullscreenState,
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
		abi == 8 &&
		probeResult != 0;

	if (f)
	{
		fprintf(
			f,
			"renderer11_bridge loaded module=0x%08lX abi=%lu expected=8 backend=%s probe=%d phase2c2_exports=%d\n",
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
		gSpideyRenderer11AppliedWindowMode = -1;
		gSpideyRenderer11AppliedModeWidth = 0;
		gSpideyRenderer11AppliedModeHeight = 0;
	}

	if (!gSpideyRenderer11Initialized)
	{
		int initialized =
			gSpideyRenderer11Initialize(
				hwnd,
				width,
				height);

		FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
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
		gSpideyRenderer11AppliedWindowMode = -1;
		SpideyApplySelectedWindowStyle(
			hwnd,
			"renderer11_initialize");
		SpideyApplyRendererWindowMode(
			"renderer11_initialize");
		return 1;
	}

	if (gSpideyRenderer11Width != width ||
		gSpideyRenderer11Height != height)
	{
		int resized =
			gSpideyRenderer11Resize(
				width,
				height);

		FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
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
		gSpideyRenderer11AppliedModeWidth = 0;
		gSpideyRenderer11AppliedModeHeight = 0;
		SpideyApplyRendererWindowMode(
			"renderer11_resize");
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

		FILE* lazyLog = SpideyOpenConsolidatedLog(
		"COMPAT");
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
		FILE* f = SpideyOpenConsolidatedLog(
		"TEXTURE");
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

	// Successful texture mirroring is a normal streaming path. Do not open,
	// write and close the consolidated log for every upload; failures above
	// remain logged and resident totals are available from draw diagnostics.
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

	// Association succeeds for essentially every streamed texture. Logging
	// each success synchronously perturbs the very frame pacing being measured.
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

		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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

	FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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

typedef HRESULT (WINAPI *SpideyRetailD3D7SceneFn)(
		LPDIRECT3DDEVICE7);
typedef HRESULT (WINAPI *SpideyRetailD3D7SurfaceBltFn)(
		LPDIRECTDRAWSURFACE7,
		LPRECT,
		LPDIRECTDRAWSURFACE7,
		LPRECT,
		DWORD,
		LPDDBLTFX);
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

static SpideyRetailD3D7SceneFn gSpideyRetailD3D7BeginSceneOriginal = 0;
static SpideyRetailD3D7SceneFn gSpideyRetailD3D7EndSceneOriginal = 0;
static SpideyRetailD3D7SurfaceBltFn gSpideyRetailD3D7SurfaceBltOriginal = 0;
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

typedef void (__fastcall *SpideyRetailFireWebFn)(
		CPlayer*,
		void*,
		bool,
		i32,
		CVector*,
		bool,
		CSVector*);

static unsigned long gSpideyFireWebCalls = 0;
static unsigned long gSpideyLastFireWebFrame = 0;

static void __fastcall SpideyTimingFireWeb(
		CPlayer* player,
		void*,
		bool a1,
		i32 a2,
		CVector* a3,
		bool a4,
		CSVector* a5)
{
	SpideyRetailFireWebFn retail =
		(SpideyRetailFireWebFn)0x004C5DD0;

	++gSpideyFireWebCalls;
	gSpideyLastFireWebFrame =
		gSpideyPresentFrame;

	retail(
		player,
		0,
		a1,
		a2,
		a3,
		a4,
		a5);
}
static int gSpideyShadowPreviewReady = 0;
static int gSpideyShadowPreviewModeSynced = 0;

// Phase 3E diagnostic: prove the visible DX11 replay can survive without
// executing the matching D3D7 main-scene DrawPrimitive. This is deliberately
// opt-in and conservative: F9 suppresses only draws already accepted by DX11
// with a texture that is already mirrored (or no texture). Offscreen draws,
// unsupported states, unresolved textures and the F10 D3D7 reference path
// always continue through retail D3D7.
static int gSpideyD3D7MainDrawSuppressionEnabled = 0;
static unsigned long gSpideyD3D7MainDrawSuppressed = 0;
static unsigned long gSpideyD3D7MainDrawFallback = 0;

// DX11-authoritative renderer migration. D3D7 remains alive only as a
// compatibility object provider while unsupported/offscreen paths are ported.
// Main-scene presentation and every main draw accepted by DX11 must no longer
// depend on a valid DirectDraw display surface.
static int gSpideyDx11AuthoritativeRendering = 1;
static int gSpideyDx11VirtualSceneActive = 0;
static int gSpideyD3D7FallbackSceneActive = 0;
static unsigned long gSpideyD3D7BeginSceneSuppressed = 0;
static unsigned long gSpideyD3D7EndSceneSuppressed = 0;
static unsigned long gSpideyD3D7MainClearSuppressed = 0;
static unsigned long gSpideyD3D7MainStateSuppressed = 0;
static unsigned long gSpideyD3D7MainBltSuppressed = 0;
static unsigned long gSpideyD3D7FallbackSceneBegins = 0;

// @Ok
static int SpideyDx11AuthoritativeActive()
{
	return
		gSpideyDx11AuthoritativeRendering &&
		gSpideyRenderer11Initialized &&
		gSpideyShadowPreviewEnabled &&
		gSpideyShadowPreviewReady;
}

// @Ok
static int SpideyDx11OnMainScene()
{
	return
		gSpideyRetailShadowRenderTarget &&
		gSpideyRetailShadowRenderTarget ==
			*(LPDIRECTDRAWSURFACE7*)0x006B7908;
}

static unsigned long gSpideyRetailDrawCalls = 0;
static unsigned long gSpideyRetailDrawTextured = 0;
static unsigned long gSpideyRetailDrawMirrored = 0;
static unsigned long gSpideyRetailDrawMissing = 0;
static unsigned long gSpideyRetailDrawTriangleFan = 0;
static unsigned long gSpideyRetailDrawFvf144 = 0;
static unsigned long gSpideyRetailDrawOtherPrimitive = 0;
static unsigned long gSpideyRetailDrawOtherFvf = 0;
static unsigned long gSpideyRetailDrawSampleCount = 0;
static unsigned long gSpideyRetailDraw2D = 0;
static unsigned long gSpideyRetailDraw3D = 0;

static const unsigned long SPIDEY_2D_POLY_TAG_CAPACITY = 32768;
static void* gSpidey2DPolyTags[SPIDEY_2D_POLY_TAG_CAPACITY];
static unsigned long gSpidey2DPolyTagged = 0;

typedef void (__cdecl *SpideyRetailDXPOLYDrawPolyFn)(
		void*,
		int,
		int,
		float);

static unsigned long Spidey2DPolyHash(
		void* poly)
{
	return (
		((unsigned long)poly >> 4) ^
		((unsigned long)poly >> 13)) &
		(SPIDEY_2D_POLY_TAG_CAPACITY - 1);
}

static void SpideyTag2DPoly(
		void* poly)
{
	if (!poly)
		return;

	unsigned long slot =
		Spidey2DPolyHash(
			poly);

	for (unsigned long probe = 0;
		 probe < SPIDEY_2D_POLY_TAG_CAPACITY;
		 ++probe)
	{
		void*& entry =
			gSpidey2DPolyTags[slot];

		if (!entry)
		{
			entry =
				poly;
			++gSpidey2DPolyTagged;
			return;
		}

		if (entry == poly)
			return;

		slot =
			(slot + 1) &
			(SPIDEY_2D_POLY_TAG_CAPACITY - 1);
	}
}

static int SpideyIsTagged2DVertices(
		const void* vertices)
{
	if (!vertices)
		return 0;

	void* poly =
		(void*)(
			(const unsigned char*)vertices -
			0x10);

	unsigned long slot =
		Spidey2DPolyHash(
			poly);

	for (unsigned long probe = 0;
		 probe < SPIDEY_2D_POLY_TAG_CAPACITY;
		 ++probe)
	{
		void* entry =
			gSpidey2DPolyTags[slot];

		if (!entry)
			return 0;

		if (entry == poly)
			return 1;

		slot =
			(slot + 1) &
			(SPIDEY_2D_POLY_TAG_CAPACITY - 1);
	}

	return 0;
}

static void __cdecl SpideyCompatDXPOLYDraw2D(
		void* poly,
		int sortSlot,
		int blendMode,
		float maxDepth)
{
	SpideyTag2DPoly(
		poly);

	SpideyRetailDXPOLYDrawPolyFn retail =
		(SpideyRetailDXPOLYDrawPolyFn)0x00503100;

	retail(
		poly,
		sortSlot,
		blendMode,
		maxDepth);
}

static void SpideyInstall2DPolyProvenanceCompat()
{
	const int quadInstalled =
		SpideyPatchDirectCall(
			0x005078F0,
			0x00503100,
			SpideyCompatDXPOLYDraw2D,
			"drawquad2d_provenance");
	const int qpolyInstalled =
		SpideyPatchDirectCall(
			0x00507D83,
			0x00503100,
			SpideyCompatDXPOLYDraw2D,
			"drawqpoly2d_provenance");

	FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
	if (f)
	{
		fprintf(
			f,
			"draw_provenance installed=%d quad_call=0x005078F0 qpoly_call=0x00507D83 retail=0x00503100 sidecar_capacity=%lu\n",
			quadInstalled &&
				qpolyInstalled ?
				1 :
				0,
			SPIDEY_2D_POLY_TAG_CAPACITY);
		fclose(f);
	}
}


// The retail PC timer requests a fixed 16 ms periodic multimedia timer.
// 16 ms is 62.5 Hz, while TimerCallback converts elapsed milliseconds into a
// 60 Hz virtual-vblank clock. Runtime cadence telemetry now proves the beat:
// one ~32 ms present almost exactly every 24 frames / 0.4 s.
//
// Keep the retail TimerCallback, pause state, fractional accumulator and
// MyVSync work intact. The normal native-60 compatibility dispatcher uses a
// 1 ms WinMM heartbeat and dispatches the untouched retail callback at 60 Hz.
//
// The temporary full-engine 20-FPS Chase reference experiment is complete:
// it proved the retail route works when the whole engine receives ~50 ms
// timer callbacks. The live policy is restored to native 60-Hz timer delivery:
//
//   deadline_ms(n) = floor(n * 1000 / 60) + 1
//
// Chase-specific authored-cadence compatibility now lives at narrower
// subsystem seams (scripted player, active camera and BaddyList) instead of
// slowing the entire engine.
typedef void (CALLBACK *SpideyRetailTimerCallbackFn)(
		UINT,
		UINT,
		DWORD,
		DWORD,
		DWORD);
typedef UINT (WINAPI *SpideyTimeSetEventFn)(
		UINT,
		UINT,
		SpideyRetailTimerCallbackFn,
		DWORD,
		UINT);
typedef UINT (WINAPI *SpideyTimeKillEventFn)(
		UINT);
typedef UINT (WINAPI *SpideyTimePeriodFn)(
		UINT);
typedef DWORD (WINAPI *SpideyTimeGetTimeFn)(
		void);

struct SpideyRetailTimerInfoCompat
{
	UINT uTimerID;
	UINT field_4;
	UINT uPeriod;
	UINT field_C;
};

static SpideyTimeSetEventFn gSpideyOriginalTimeSetEvent = 0;
static SpideyTimeKillEventFn gSpideyOriginalTimeKillEvent = 0;
static SpideyTimePeriodFn gSpideyTimeBeginPeriod = 0;
static SpideyTimePeriodFn gSpideyTimeEndPeriod = 0;
static SpideyTimeGetTimeFn gSpideyTimeGetTime = 0;

static volatile LONG gSpideyPacingTimerActive = 0;
static UINT gSpideyPacingSyntheticTimerId = 0x5A17;
static UINT gSpideyPacingRealTimerId = 0;
static SpideyRetailTimerCallbackFn gSpideyPacingRetailCallback = 0;
static DWORD gSpideyPacingRetailUser = 0;
static UINT gSpideyPacingRetailFlags = 0;
static unsigned long gSpideyPacingVirtualTick = 0;
static unsigned long gSpideyPacingVirtualTotalMs = 0;
static unsigned long gSpideyPacingSourceCallbackCount = 0;
static unsigned long gSpideyPacingSourceMs = 0;
static unsigned long gSpideyPacingSourceStartMs = 0;
static unsigned long gSpideyPacingCallbackCount = 0;
static unsigned long gSpideyPacingPausedCallbackCount = 0;
static unsigned long gSpideyPacingUnexpectedVblankDelta = 0;
static unsigned long gSpideyPacingLastIntervalMs = 0;
static int gSpideyPacingBeginPeriodOne = 0;

// Native-60 retail timer delivery: one canonical vblank per active callback.
// Mysterio's authored attack cadence is handled locally in CMysterio::AI.
static const unsigned long kSpideyPacingDiagnosticHz = 60UL;
static const unsigned long kSpideyPacingExpectedVblanksPerCallback = 1UL;

static int SpideyPatchMainImport(
		const char* dllName,
		const char* functionName,
		void* replacement,
		void** original)
{
	if (!dllName ||
		!functionName ||
		!replacement ||
		!original)
	{
		return 0;
	}

	unsigned char* base =
		(unsigned char*)GetModuleHandleA(
			0);
	if (!base)
		return 0;

	IMAGE_DOS_HEADER* dos =
		(IMAGE_DOS_HEADER*)base;
	if (dos->e_magic !=
		IMAGE_DOS_SIGNATURE)
	{
		return 0;
	}

	IMAGE_NT_HEADERS* nt =
		(IMAGE_NT_HEADERS*)(
			base +
			dos->e_lfanew);
	if (nt->Signature !=
		IMAGE_NT_SIGNATURE)
	{
		return 0;
	}

	const IMAGE_DATA_DIRECTORY& imports =
		nt->OptionalHeader.DataDirectory[
			IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (!imports.VirtualAddress)
		return 0;

	IMAGE_IMPORT_DESCRIPTOR* descriptor =
		(IMAGE_IMPORT_DESCRIPTOR*)(
			base +
			imports.VirtualAddress);

	for (;
		 descriptor->Name;
		 ++descriptor)
	{
		const char* importedDll =
			(const char*)(
				base +
				descriptor->Name);
		if (_stricmp(
				importedDll,
				dllName) != 0)
		{
			continue;
		}

		// Fail closed for a bound import without a name thunk. Treating the
		// resolved FirstThunk function pointers as IMAGE_IMPORT_BY_NAME RVAs
		// would be unsafe.
		if (!descriptor->OriginalFirstThunk)
		{
			return 0;
		}

		IMAGE_THUNK_DATA* nameThunk =
			(IMAGE_THUNK_DATA*)(
				base +
				descriptor->OriginalFirstThunk);
		IMAGE_THUNK_DATA* addressThunk =
			(IMAGE_THUNK_DATA*)(
				base +
				descriptor->FirstThunk);

		for (;
			 nameThunk->u1.AddressOfData;
			 ++nameThunk,
			 ++addressThunk)
		{
			if (IMAGE_SNAP_BY_ORDINAL(
					nameThunk->u1.Ordinal))
			{
				continue;
			}

			// VC6's Platform SDK declares AddressOfData as a pointer-typed
			// union member, whereas newer headers commonly expose an integer RVA.
			// Copy the raw 32-bit thunk value so this source compiles correctly
			// against both header generations.
			DWORD importNameRva =
				0;
			memcpy(
				&importNameRva,
				&nameThunk->u1.AddressOfData,
				sizeof(importNameRva));

			IMAGE_IMPORT_BY_NAME* importName =
				(IMAGE_IMPORT_BY_NAME*)(
					base +
					importNameRva);
			if (strcmp(
					(const char*)importName->Name,
					functionName) != 0)
			{
				continue;
			}

			DWORD oldProtect =
				0;
			if (!VirtualProtect(
					&addressThunk->u1.Function,
					sizeof(addressThunk->u1.Function),
					PAGE_READWRITE,
					&oldProtect))
			{
				return 0;
			}

			// Likewise, VC6 types Function as a pointer member. Avoid a
			// header-version-dependent assignment by copying the raw Win32
			// pointer bits in and out of the thunk slot.
			memcpy(
				original,
				&addressThunk->u1.Function,
				sizeof(void*));
			memcpy(
				&addressThunk->u1.Function,
				&replacement,
				sizeof(replacement));

			DWORD ignoredProtect =
				0;
			VirtualProtect(
				&addressThunk->u1.Function,
				sizeof(addressThunk->u1.Function),
				oldProtect,
				&ignoredProtect);
			FlushInstructionCache(
				GetCurrentProcess(),
				&addressThunk->u1.Function,
				sizeof(addressThunk->u1.Function));

			return 1;
		}
	}

	return 0;
}

static void CALLBACK SpideyPacingTimerThunk(
		UINT timerId,
		UINT message,
		DWORD,
		DWORD param1,
		DWORD param2)
{
	if (!gSpideyPacingTimerActive ||
		!gSpideyPacingRetailCallback)
	{
		return;
	}

	++gSpideyPacingSourceCallbackCount;

	const unsigned long nowMs =
		gSpideyTimeGetTime ?
			(unsigned long)gSpideyTimeGetTime() :
			(unsigned long)GetTickCount();
	gSpideyPacingSourceMs =
		nowMs -
		gSpideyPacingSourceStartMs;

	const unsigned long nextVirtualTick =
		gSpideyPacingVirtualTick +
		1;
	const unsigned long targetTotalMs =
		(unsigned long)(
			((nextVirtualTick * 1000UL) /
			 kSpideyPacingDiagnosticHz) +
			1UL);

	if (gSpideyPacingSourceMs <
		targetTotalMs)
	{
		return;
	}

	UINT interval =
		(UINT)(
			targetTotalMs -
			gSpideyPacingVirtualTotalMs);
	if (interval < 1)
		interval = 1;
	if (interval > 20)
		interval = 20;

	// Advance the delivery schedule regardless of retail pause state. Retail's
	// original 16 ms periodic timer kept firing while paused too; TimerCallback
	// simply ignored those callbacks. Keeping schedule phase independent from
	// Vblanks avoids a resume-time catch-up burst.
	gSpideyPacingVirtualTick =
		nextVirtualTick;
	gSpideyPacingVirtualTotalMs =
		targetTotalMs;
	gSpideyPacingLastIntervalMs =
		interval;

	SpideyRetailTimerInfoCompat* timerInfo =
		(SpideyRetailTimerInfoCompat*)
			gSpideyPacingRetailUser;
	if (timerInfo)
	{
		__try
		{
			timerInfo->field_4 =
				interval;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	const unsigned long beforeVblanks =
		(unsigned long)*(volatile long*)0x006B4CA0;

	gSpideyPacingRetailCallback(
		gSpideyPacingSyntheticTimerId,
		message,
		gSpideyPacingRetailUser,
		param1,
		param2);

	const unsigned long afterVblanks =
		(unsigned long)*(volatile long*)0x006B4CA0;
	const unsigned long deltaVblanks =
		afterVblanks -
		beforeVblanks;

	++gSpideyPacingCallbackCount;

	if (deltaVblanks == 0)
	{
		++gSpideyPacingPausedCallbackCount;
	}
	else if (deltaVblanks !=
		kSpideyPacingExpectedVblanksPerCallback)
	{
		++gSpideyPacingUnexpectedVblankDelta;
	}

	(void)timerId;
}

static UINT WINAPI SpideyCompatTimeSetEvent(
		UINT delay,
		UINT resolution,
		SpideyRetailTimerCallbackFn callback,
		DWORD user,
		UINT flags)
{
	if (!gSpideyOriginalTimeSetEvent)
		return 0;

	const unsigned long callbackAddress =
		(unsigned long)callback;
	const int looksLikeRetailPcTimer =
		delay == 16 &&
		resolution == 16 &&
		callback &&
		callbackAddress >= 0x00401000 &&
		callbackAddress < 0x0053B000 &&
		(flags & 1) != 0;

	if (!looksLikeRetailPcTimer ||
		gSpideyPacingTimerActive)
	{
		return gSpideyOriginalTimeSetEvent(
			delay,
			resolution,
			callback,
			user,
			flags);
	}

	gSpideyPacingRetailCallback =
		callback;
	gSpideyPacingRetailUser =
		user;
	gSpideyPacingRetailFlags =
		flags;
	gSpideyPacingVirtualTick =
		0;
	gSpideyPacingVirtualTotalMs =
		0;
	gSpideyPacingSourceCallbackCount =
		0;
	gSpideyPacingSourceMs =
		0;
	gSpideyPacingSourceStartMs =
		gSpideyTimeGetTime ?
			(unsigned long)gSpideyTimeGetTime() :
			(unsigned long)GetTickCount();
	gSpideyPacingCallbackCount =
		0;
	gSpideyPacingPausedCallbackCount =
		0;
	gSpideyPacingUnexpectedVblankDelta =
		0;
	gSpideyPacingLastIntervalMs =
		0;

	if (gSpideyTimeBeginPeriod &&
		gSpideyTimeBeginPeriod(1) == 0)
	{
		gSpideyPacingBeginPeriodOne =
			1;
	}

	InterlockedExchange(
		(LONG*)&gSpideyPacingTimerActive,
		1);

	const UINT sourcePeriodMs =
		1;

	gSpideyPacingRealTimerId =
		gSpideyOriginalTimeSetEvent(
			sourcePeriodMs,
			1,
			SpideyPacingTimerThunk,
			0,
			1);

	if (!gSpideyPacingRealTimerId)
	{
		InterlockedExchange(
			(LONG*)&gSpideyPacingTimerActive,
			0);

		if (gSpideyPacingBeginPeriodOne &&
			gSpideyTimeEndPeriod)
		{
			gSpideyTimeEndPeriod(1);
			gSpideyPacingBeginPeriodOne =
				0;
		}

		gSpideyPacingRetailCallback =
			0;
		gSpideyPacingRetailUser =
			0;

		return gSpideyOriginalTimeSetEvent(
			delay,
			resolution,
			callback,
			user,
			flags);
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"timer_pacing event=intercept retail_delay=%u retail_resolution=%u retail_flags=0x%08X callback=0x%08lX user=0x%08lX synthetic_id=%u source_period_ms=%u first_delivery_target_ms=17 target_hz=60 expected_vblanks_per_callback=1 policy=periodic_1ms_dispatch_16_17ms_60hz source_clock=%s retail_callback_preserved=1\n",
			delay,
			resolution,
			flags,
			callbackAddress,
			(unsigned long)user,
			gSpideyPacingSyntheticTimerId,
			sourcePeriodMs,
			gSpideyTimeGetTime ?
				"timeGetTime" :
				"GetTickCount");
		fclose(f);
	}

	return gSpideyPacingSyntheticTimerId;
}

static UINT WINAPI SpideyCompatTimeKillEvent(
		UINT timerId)
{
	if (timerId ==
			gSpideyPacingSyntheticTimerId &&
		gSpideyPacingTimerActive)
	{
		InterlockedExchange(
			(LONG*)&gSpideyPacingTimerActive,
			0);

		const UINT realTimerId =
			gSpideyPacingRealTimerId;
		gSpideyPacingRealTimerId =
			0;

		UINT result =
			0;
		if (realTimerId &&
			gSpideyOriginalTimeKillEvent)
		{
			result =
				gSpideyOriginalTimeKillEvent(
					realTimerId);
		}

		if (gSpideyPacingBeginPeriodOne &&
			gSpideyTimeEndPeriod)
		{
			gSpideyTimeEndPeriod(1);
			gSpideyPacingBeginPeriodOne =
				0;
		}

		SpideyLogMysterioLaserSetPosStats();
		SpideyLogMysterioYawTowardsStats();
		SpideyLogChasePlayerAI20Stats();
		SpideyLogChaseCameraAI20Stats();
		SpideyLogChaseBaddyAI20Stats();
		SpideyLogChaseSynthStats();
		SpideyDumpChaseSynthTrace();
		SpideyLogChaseSchedulerStats();

		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"timer_pacing event=kill source_callbacks=%lu dispatched_callbacks=%lu virtual_ticks=%lu paused_callbacks=%lu unexpected_vblank_delta=%lu last_interval_ms=%lu result=%u\n",
				gSpideyPacingSourceCallbackCount,
				gSpideyPacingCallbackCount,
				gSpideyPacingVirtualTick,
				gSpideyPacingPausedCallbackCount,
				gSpideyPacingUnexpectedVblankDelta,
				gSpideyPacingLastIntervalMs,
				result);
			fclose(f);
		}

		gSpideyPacingRetailCallback =
			0;
		gSpideyPacingRetailUser =
			0;

		return result;
	}

	if (gSpideyOriginalTimeKillEvent)
	{
		return gSpideyOriginalTimeKillEvent(
			timerId);
	}

	return 0;
}

static int SpideyInstallModernTimerPacing()
{
	HMODULE winmm =
		GetModuleHandleA(
			"winmm.dll");
	if (!winmm)
	{
		winmm =
			LoadLibraryA(
				"winmm.dll");
	}

	if (winmm)
	{
		gSpideyTimeBeginPeriod =
			(SpideyTimePeriodFn)GetProcAddress(
				winmm,
				"timeBeginPeriod");
		gSpideyTimeEndPeriod =
			(SpideyTimePeriodFn)GetProcAddress(
				winmm,
				"timeEndPeriod");
		gSpideyTimeGetTime =
			(SpideyTimeGetTimeFn)GetProcAddress(
				winmm,
				"timeGetTime");
	}

	void* originalSet =
		0;
	void* originalKill =
		0;

	// Cleanup must be interceptable before we are allowed to hand PCTIMER_Init
	// a synthetic timer ID. If timeKillEvent cannot be hooked, do not hook
	// timeSetEvent at all; retail timing remains completely untouched.
	const int killInstalled =
		SpideyPatchMainImport(
			"WINMM.dll",
			"timeKillEvent",
			(void*)&SpideyCompatTimeKillEvent,
			&originalKill);

	if (killInstalled)
	{
		gSpideyOriginalTimeKillEvent =
			(SpideyTimeKillEventFn)originalKill;
	}

	const int setInstalled =
		killInstalled ?
			SpideyPatchMainImport(
				"WINMM.dll",
				"timeSetEvent",
				(void*)&SpideyCompatTimeSetEvent,
				&originalSet) :
			0;

	if (setInstalled)
	{
		gSpideyOriginalTimeSetEvent =
			(SpideyTimeSetEventFn)originalSet;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"timer_pacing_install set_event=%d kill_event=%d original_set=0x%08lX original_kill=0x%08lX begin_period=0x%08lX end_period=0x%08lX time_get_time=0x%08lX retail_match=16ms_periodic_main_exe target_hz=60 expected_vblanks_per_callback=1 policy=periodic_1ms_source_dispatch_16_17ms_60hz install_order=kill_then_set atomic_cleanup=1 fallback=retail\n",
			setInstalled,
			killInstalled,
			(unsigned long)gSpideyOriginalTimeSetEvent,
			(unsigned long)gSpideyOriginalTimeKillEvent,
			(unsigned long)gSpideyTimeBeginPeriod,
			(unsigned long)gSpideyTimeEndPeriod,
			(unsigned long)gSpideyTimeGetTime);
		fclose(f);
	}

	return
		setInstalled &&
		killInstalled &&
		gSpideyOriginalTimeSetEvent &&
		gSpideyOriginalTimeKillEvent;
}

typedef void (__cdecl *SpideyRetailLogicFn)(void);

static unsigned long gSpideyTimingLogicWindowStart = 0;
static unsigned long gSpideyTimingLogicTicks = 0;
static unsigned long gSpideyTimingPresentWindowStart = 0;
static unsigned long gSpideyTimingPresentFrames = 0;

// Keep frame-pacing diagnostics entirely in memory on the hot path. The
// existing once-per-second timing line publishes the aggregate, so this adds
// no extra per-frame file I/O while letting one runtime distinguish renderer
// stalls from the retail multimedia-timer/vblank cadence.
static LARGE_INTEGER gSpideyTimingPerfFrequency;
static LARGE_INTEGER gSpideyTimingLastPresentCounter;
static unsigned long gSpideyTimingPresentIntervals = 0;
static unsigned long gSpideyTimingPresentOver20Ms = 0;
static unsigned long gSpideyTimingPresentOver25Ms = 0;
static unsigned long gSpideyTimingPresentOver30Ms = 0;
static unsigned long gSpideyTimingPresentOver50Ms = 0;
static unsigned long gSpideyTimingPresentMaxIntervalUs = 0;
static int gSpideyTimingPresentVblankValid = 0;
static long gSpideyTimingLastPresentVblank = 0;
static unsigned long gSpideyTimingPresentVblankSame = 0;
static unsigned long gSpideyTimingPresentVblankOne = 0;
static unsigned long gSpideyTimingPresentVblankMulti = 0;
static unsigned long gSpideyTimingPresentVblankMaxDelta = 0;

// Previous completed DXPOLY flip-wrapper phase timings. SpideyRecordPresentTiming
// runs at the *next* wrapper entry, so a slow inter-present interval can be
// decomposed into prior presenter work vs time spent elsewhere in the frame.
static unsigned long gSpideyTimingLastPresentWorkUs = 0;
static unsigned long gSpideyTimingLastRecordTimingUs = 0;
static unsigned long gSpideyTimingLastTransientUs = 0;
static unsigned long gSpideyTimingLastShadowEndUs = 0;
static unsigned long gSpideyTimingLastDrawProbeUs = 0;
static unsigned long gSpideyTimingLastPresentShadowUs = 0;
static unsigned long gSpideyTimingLastOtherPresentUs = 0;

// Accumulate gameplay-logic work that occurs between presenter entries so a
// slow inter-present interval can distinguish retail logic from diagnostic
// logging and from the rest of the game/update/render path.
static unsigned long gSpideyTimingOutsideLogicRetailUs = 0;
static unsigned long gSpideyTimingOutsideLogicTelemetryUs = 0;
static unsigned long gSpideyTimingOutsideLogicCalls = 0;

// The phase probe proved that synchronous once-per-second timing writes can
// stall the game thread for hundreds of milliseconds. Keep the in-memory
// counters, but make file publication an explicit diagnostic opt-in.
static int gSpideyTimingFileTelemetryEnabled = 0;

static unsigned long SpideyTimingElapsedUs(
		const LARGE_INTEGER* start,
		const LARGE_INTEGER* end)
{
	if (!start ||
		!end ||
		!gSpideyTimingPerfFrequency.QuadPart)
	{
		return 0;
	}

	const LONGLONG ticks =
		end->QuadPart -
		start->QuadPart;
	if (ticks <= 0)
		return 0;

	return
		(unsigned long)(
			(ticks * (LONGLONG)1000000) /
			gSpideyTimingPerfFrequency.QuadPart);
}

#define SPIDEY_TIMING_SLOW_EVENT_CAPACITY 16
#define SPIDEY_TIMING_SLOW_EVENT_THRESHOLD_US 18000

struct SpideyTimingSlowEvent
{
	unsigned long frame;
	unsigned long intervalUs;
	unsigned long webTargetCalls;
	unsigned long checkWebShotCalls;
	unsigned long fireWebCalls;
	unsigned long lastFireWebFrame;
	unsigned long presentWorkUs;
	unsigned long outsidePresentUs;
	unsigned long recordTimingUs;
	unsigned long transientUs;
	unsigned long shadowEndUs;
	unsigned long drawProbeUs;
	unsigned long presentShadowUs;
	unsigned long otherPresentUs;
	unsigned long logicRetailUs;
	unsigned long logicTelemetryUs;
	unsigned long logicCalls;
	unsigned long outsideNonLogicUs;
};

static SpideyTimingSlowEvent
	gSpideyTimingSlowEvents[SPIDEY_TIMING_SLOW_EVENT_CAPACITY];
static unsigned long gSpideyTimingSlowEventCount = 0;
static unsigned long gSpideyTimingSlowEventStored = 0;

static void SpideyLogTimingWindow(
		const char* kind,
		unsigned long elapsed,
		unsigned long count)
{
	if (!gSpideyTimingFileTelemetryEnabled ||
		!kind ||
		!elapsed)
	{
		return;
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"TIMING");
	if (!f)
		return;

	const double hz =
		((double)count * 1000.0) /
		(double)elapsed;

	if (kind[0] == 'p')
	{
		fprintf(
			f,
			"timing_%s elapsed_ms=%lu count=%lu hz=%.3f frontend=%d vblanks=%ld present_frame=%lu logical=%lux%lu physical=%lux%lu cadence_intervals=%lu over20ms=%lu over25ms=%lu over30ms=%lu over50ms=%lu max_interval_us=%lu vblank_same=%lu vblank_one=%lu vblank_multi=%lu vblank_max_delta=%lu slow_event_count=%lu slow_event_stored=%lu timer_active=%ld timer_callbacks=%lu timer_virtual_ticks=%lu timer_paused_callbacks=%lu timer_unexpected_delta=%lu timer_last_interval_ms=%lu timer_source_callbacks=%lu timer_source_ms=%lu fire_web_calls=%lu last_fire_frame=%lu\n",
			kind,
			elapsed,
			count,
			hz,
			gSpideyFrontendLegacyMode,
			(long)*(volatile long*)0x006B4CA0,
			gSpideyPresentFrame,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight,
			gSpideyLegacyPhysicalWidth,
			gSpideyLegacyPhysicalHeight,
			gSpideyTimingPresentIntervals,
			gSpideyTimingPresentOver20Ms,
			gSpideyTimingPresentOver25Ms,
			gSpideyTimingPresentOver30Ms,
			gSpideyTimingPresentOver50Ms,
			gSpideyTimingPresentMaxIntervalUs,
			gSpideyTimingPresentVblankSame,
			gSpideyTimingPresentVblankOne,
			gSpideyTimingPresentVblankMulti,
			gSpideyTimingPresentVblankMaxDelta,
			gSpideyTimingSlowEventCount,
			gSpideyTimingSlowEventStored,
			(long)gSpideyPacingTimerActive,
			gSpideyPacingCallbackCount,
			gSpideyPacingVirtualTick,
			gSpideyPacingPausedCallbackCount,
			gSpideyPacingUnexpectedVblankDelta,
			gSpideyPacingLastIntervalMs,
			gSpideyPacingSourceCallbackCount,
			gSpideyPacingSourceMs,
			gSpideyFireWebCalls,
			gSpideyLastFireWebFrame);

		if (gSpideyTimingSlowEventStored)
		{
			fprintf(
				f,
				"[TIMING] slow_present_events threshold_us=%u",
				SPIDEY_TIMING_SLOW_EVENT_THRESHOLD_US);

			for (unsigned long i = 0;
				 i < gSpideyTimingSlowEventStored;
				 ++i)
			{
				const SpideyTimingSlowEvent& event =
					gSpideyTimingSlowEvents[i];
				fprintf(
					f,
					" frame=%lu interval_us=%lu web_calls=%lu check_web_shot_calls=%lu fire_web_calls=%lu last_fire_frame=%lu present_work_us=%lu outside_present_us=%lu record_timing_us=%lu transient_us=%lu shadow_end_us=%lu draw_probe_us=%lu present_shadow_us=%lu other_present_us=%lu logic_retail_us=%lu logic_telemetry_us=%lu logic_calls=%lu outside_nonlogic_us=%lu",
					event.frame,
					event.intervalUs,
					event.webTargetCalls,
					event.checkWebShotCalls,
					event.fireWebCalls,
					event.lastFireWebFrame,
					event.presentWorkUs,
					event.outsidePresentUs,
					event.recordTimingUs,
					event.transientUs,
					event.shadowEndUs,
					event.drawProbeUs,
					event.presentShadowUs,
					event.otherPresentUs,
					event.logicRetailUs,
					event.logicTelemetryUs,
					event.logicCalls,
					event.outsideNonLogicUs);
			}

			fputc(
				'\n',
				f);
		}
	}
	else
	{
		fprintf(
			f,
			"timing_%s elapsed_ms=%lu count=%lu hz=%.3f frontend=%d vblanks=%ld present_frame=%lu logical=%lux%lu physical=%lux%lu\n",
			kind,
			elapsed,
			count,
			hz,
			gSpideyFrontendLegacyMode,
			(long)*(volatile long*)0x006B4CA0,
			gSpideyPresentFrame,
			gSpideyModernLogicalWidth,
			gSpideyModernLogicalHeight,
			gSpideyLegacyPhysicalWidth,
			gSpideyLegacyPhysicalHeight);
	}
	fclose(f);
}

struct SpideyZipButtonRecordSnapshot
{
	unsigned int held;
	unsigned int pressed;
	unsigned int rapidPress;
	long heldTicks;
	long releasedTicks;
	long transitionTicks;
};

static void SpideyReadZipButtonRecord(
		unsigned char* input,
		unsigned long offset,
		SpideyZipButtonRecordSnapshot* out)
{
	if (!out)
		return;

	memset(
		out,
		0,
		sizeof(*out));

	if (!input)
		return;

	__try
	{
		unsigned char* record =
			input +
			offset;
		out->held =
			(unsigned int)record[0];
		out->pressed =
			(unsigned int)record[1];
		out->rapidPress =
			(unsigned int)record[2];
		out->heldTicks =
			*(long*)(record + 4);
		out->releasedTicks =
			*(long*)(record + 8);
		out->transitionTicks =
			*(long*)(record + 0x0C);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		memset(
			out,
			0,
			sizeof(*out));
	}
}

typedef u8 (FASTCALL *SpideyRetailZipCheckFn)(
		CPlayer*,
		void*);
typedef u8 (FASTCALL *SpideyRetailZipAvailabilityFn)(
		CPlayer*,
		void*,
		SLineInfo*,
		i32);

static unsigned long gSpideyZipR1Calls = 0;
static unsigned long gSpideyZipR2Calls = 0;
static unsigned long gSpideyZipR1Success = 0;
static unsigned long gSpideyZipR2Success = 0;
static unsigned long gSpideyZipTraceStored = 0;
static unsigned long gSpideyZipTraceDropped = 0;
static unsigned long gSpideyZipR1LastSignature = 0xFFFFFFFFUL;
static unsigned long gSpideyZipR2LastSignature = 0xFFFFFFFFUL;
static unsigned long gSpideyZipAvailabilityCalls = 0;
static unsigned long gSpideyZipAvailabilityStored = 0;
static unsigned long gSpideyZipStaleAimClears = 0;
static unsigned long gSpideyZipStaleAimSuccess = 0;
static unsigned long gSpideyZipStaleAimRestores = 0;
static unsigned long gSpideyZipStaleAimLogs = 0;
static unsigned long gSpideyZipModernAimBlocks = 0;
static unsigned long gSpideyZipModernAimBlockLogs = 0;

static unsigned long gSpideyModernAimedZipAttempts = 0;
static unsigned long gSpideyModernAimedZipCameraHits = 0;
static unsigned long gSpideyModernAimedZipSuccess = 0;
static unsigned long gSpideyModernAimedZipLogs = 0;
static CPlayer* gSpideyModernAimedZipValidationPlayer = 0;

static u8 SpideyTryModernAimedR1Zip(
		CPlayer* player,
		SpideyRetailZipCheckFn retail,
		const SpideyZipButtonRecordSnapshot& aimButton,
		const SpideyZipButtonRecordSnapshot& zipButton,
		int* handled)
{
	if (handled)
		*handled = 0;

	if (!player ||
		!retail ||
		!aimButton.held ||
		!zipButton.held ||
		!SpideyModernAimIsEffectivelyActive(
			player))
	{
		return 0;
	}

	CCamera* camera =
		*(CCamera**)0x0056F3B8;
	if (!camera ||
		camera->mCameraMode !=
			CAMERAMODE_DEMO)
	{
		return 0;
	}

	if (handled)
		*handled = 1;
	++gSpideyModernAimedZipAttempts;

	// Refresh the center-camera ray from the current camera/focus pair. This
	// is the same ray that drives the visible manual-aim reticle.
	SpideyModernAimApplyCameraPoint(
		player,
		camera);

	SLineInfo cameraLine;
	cameraLine.StartCoords =
		camera->mPos;
	cameraLine.EndCoords =
		player->field_DC0;

	typedef void (__cdecl *SpideyRetailInitLineInfoFn)(
		SLineInfo*);
	typedef void (__cdecl *SpideyRetailZoneLineFn)(
		SLineInfo*,
		i32);

	SpideyRetailInitLineInfoFn initLine =
		(SpideyRetailInitLineInfoFn)0x004524C0;
	SpideyRetailZoneLineFn lineToWorld =
		(SpideyRetailZoneLineFn)0x004549A0;

	initLine(
		&cameraLine);
	cameraLine.RecordTriggerZoneHits =
		0;
	lineToWorld(
		&cameraLine,
		1);

	if (!cameraLine.pItem)
	{
		if (gSpideyModernAimedZipLogs < 96)
		{
			FILE* f =
				SpideyOpenConsolidatedLog(
					"TIMING");
			if (f)
			{
				fprintf(
					f,
					"web_zip_aimed event=no_camera_hit attempt=%lu tick=%ld camera=%ld,%ld,%ld ray_end=%ld,%ld,%ld body=%ld,%ld,%ld\n",
					gSpideyModernAimedZipAttempts,
					(long)*(volatile long*)0x006B4CA8,
					(long)camera->mPos.vx,
					(long)camera->mPos.vy,
					(long)camera->mPos.vz,
					(long)player->field_DC0.vx,
					(long)player->field_DC0.vy,
					(long)player->field_DC0.vz,
					(long)player->mPos.vx,
					(long)player->mPos.vy,
					(long)player->mPos.vz);
				fclose(f);
			}
			++gSpideyModernAimedZipLogs;
		}
		return 0;
	}

	++gSpideyModernAimedZipCameraHits;

	const double dx =
		(double)cameraLine.Position.vx -
		(double)player->mPos.vx;
	const double dy =
		(double)cameraLine.Position.vy -
		(double)player->mPos.vy;
	const double dz =
		(double)cameraLine.Position.vz -
		(double)player->mPos.vz;
	const double length =
		sqrt(
			dx * dx +
			dy * dy +
			dz * dz);

	if (length < 4096.0)
	{
		return 0;
	}

	CVector aimedDirection;
	aimedDirection.vx =
		SpideyRoundCameraDouble(
			dx * 4096.0 /
			length);
	aimedDirection.vy =
		SpideyRoundCameraDouble(
			dy * 4096.0 /
			length);
	aimedDirection.vz =
		SpideyRoundCameraDouble(
			dz * 4096.0 /
			length);

	const CVector savedSurfaceDirection =
		player->field_C84;
	const unsigned char savedAimState =
		player->field_8EA;

	// Retail R1 normally searches along Spider-Man's current surface-normal
	// basis and rejects while lookaround owns field_8EA. For modern aimed
	// zip, substitute only the search direction and clear that one gate.
	// Retail still performs its own body->surface raycast, range/face checks,
	// web creation, target/normal capture and transition to state 0x40000.
	player->field_C84 =
		aimedDirection;
	player->field_8EA =
		0;
	gSpideyModernAimedZipValidationPlayer =
		player;

	u8 result =
		0;
	__try
	{
		result =
			retail(
				player,
				0);
	}
	__finally
	{
		gSpideyModernAimedZipValidationPlayer =
			0;
		player->field_C84 =
			savedSurfaceDirection;

		if (!result)
		{
			player->field_8EA =
				savedAimState;
		}
	}

	if (result)
	{
		++gSpideyModernAimedZipSuccess;
		SpideyModernAimDropForZip(
			player);
	}

	if (gSpideyModernAimedZipLogs < 96)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"web_zip_aimed event=retail_r1 attempt=%lu camera_hits=%lu success=%lu tick=%ld result=%u camera_hit=%ld,%ld,%ld camera_hit_distance=%ld aimed_dir=%ld,%ld,%ld retail_target=%ld,%ld,%ld retail_normal=%ld,%ld,%ld saved_surface=%ld,%ld,%ld state=0x%08lX anim=%u\n",
				gSpideyModernAimedZipAttempts,
				gSpideyModernAimedZipCameraHits,
				gSpideyModernAimedZipSuccess,
				(long)*(volatile long*)0x006B4CA8,
				(unsigned int)result,
				(long)cameraLine.Position.vx,
				(long)cameraLine.Position.vy,
				(long)cameraLine.Position.vz,
				(long)cameraLine.Distance,
				(long)aimedDirection.vx,
				(long)aimedDirection.vy,
				(long)aimedDirection.vz,
				(long)player->field_DC0.vx,
				(long)player->field_DC0.vy,
				(long)player->field_DC0.vz,
				(long)player->field_DA0.vx,
				(long)player->field_DA0.vy,
				(long)player->field_DA0.vz,
				(long)savedSurfaceDirection.vx,
				(long)savedSurfaceDirection.vy,
				(long)savedSurfaceDirection.vz,
				(unsigned long)player->field_E1C,
				(unsigned int)player->mAnim);
			fclose(f);
		}
		++gSpideyModernAimedZipLogs;
	}

	return result;
}

static int SpideyZipBlockedByModernAimLocomotion(
		CPlayer* player,
		const SpideyZipButtonRecordSnapshot& aimButton)
{
	if (!player ||
		!aimButton.held ||
		player->field_8EA ||
		gSpideyModernAimLocomotionMaskedPlayer !=
			player)
	{
		return 0;
	}

	// Retail never permits R1/R2 zip while lookaround/manual aim owns the
	// player: field_8EA is its first rejection gate. Our modern aimed-
	// locomotion compatibility temporarily masks field_8EA so movement can
	// proceed without dropping the modern reticle/camera. Preserve retail's
	// semantic gate explicitly during that masked window, otherwise a held
	// Aim + Zipline can start the old surface-normal, collision-free zip
	// path even though the user is aiming somewhere entirely different.
	++gSpideyZipModernAimBlocks;

	if (gSpideyZipModernAimBlockLogs < 64)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"web_zip_compat event=block_modern_aim_zip count=%lu tick=%ld state=0x%08lX anim=%u aim_held=%u masked_player=1 axes=%d,%d\n",
				gSpideyZipModernAimBlocks,
				(long)*(volatile long*)0x006B4CA8,
				(unsigned long)player->field_E1C,
				(unsigned int)player->mAnim,
				aimButton.held,
				(int)player->field_E2D,
				(int)player->field_E2E);
			fclose(f);
		}
		++gSpideyZipModernAimBlockLogs;
	}

	return 1;
}

static int SpideyZipClearStaleModernAimGate(
		CPlayer* player,
		const SpideyZipButtonRecordSnapshot& aimButton)
{
	if (!player ||
		!player->field_8EA ||
		aimButton.held)
	{
		return 0;
	}

	// Retail lookaround uses camera mode 7 while field_8EA is active.
	// Our modern manual-aim compatibility deliberately keeps the visible
	// third-person camera in mode 3 instead. If the raw retail aim bit
	// survives after the actual aim control has been released, untouched
	// R1/R2 zip code rejects immediately on field_8EA before it can raycast.
	//
	// Only treat that modern-only combination as stale. Do not interfere
	// with a live modern-aim sidecar or with genuine retail lookaround.
	CCamera* camera =
		*(CCamera**)0x0056F3B8;

	if (!camera ||
		camera->mCameraMode !=
			CAMERAMODE_DEMO ||
		gSpideyModernAimLocomotionMaskedPlayer ==
			player)
	{
		return 0;
	}

	player->field_8EA =
		0;
	++gSpideyZipStaleAimClears;

	if (gSpideyZipStaleAimLogs < 64)
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"TIMING");
		if (f)
		{
			fprintf(
				f,
				"web_zip_compat event=clear_stale_modern_aim count=%lu tick=%ld state=0x%08lX anim=%u camera_mode=%d aim_held=%u modern_camera_active=%d\n",
				gSpideyZipStaleAimClears,
				(long)*(volatile long*)0x006B4CA8,
				(unsigned long)player->field_E1C,
				(unsigned int)player->mAnim,
				(int)camera->mCameraMode,
				aimButton.held,
				gSpideyModernCameraActive);
			fclose(f);
		}
		++gSpideyZipStaleAimLogs;
	}

	return 1;
}

static void SpideyZipRestoreStaleModernAimGate(
		CPlayer* player,
		int cleared,
		u8 retailResult)
{
	if (!player ||
		!cleared)
	{
		return;
	}

	if (retailResult)
	{
		// Retail began zip from a state where field_8EA was expected to be
		// zero. Preserve that canonical state for the subsequent 0x40000
		// animation/physics path.
		++gSpideyZipStaleAimSuccess;

		if (gSpideyZipStaleAimLogs < 64)
		{
			FILE* f =
				SpideyOpenConsolidatedLog(
					"TIMING");
			if (f)
			{
				fprintf(
					f,
					"web_zip_compat event=stale_aim_zip_success success=%lu tick=%ld state=0x%08lX anim=%u\n",
					gSpideyZipStaleAimSuccess,
					(long)*(volatile long*)0x006B4CA8,
					(unsigned long)player->field_E1C,
					(unsigned int)player->mAnim);
				fclose(f);
			}
			++gSpideyZipStaleAimLogs;
		}

		return;
	}

	player->field_8EA =
		1;
	++gSpideyZipStaleAimRestores;
}

static unsigned long SpideyBuildZipGateSignature(
		CPlayer* player,
		int r2,
		const SpideyZipButtonRecordSnapshot& zipButton,
		const SpideyZipButtonRecordSnapshot& r2Button)
{
	if (!player)
		return 0;

	unsigned long signature =
		(unsigned long)player->field_8EA |
		((player->mHeldObject ? 1UL : 0UL) << 1) |
		((unsigned long)(*((unsigned char*)player + 0x550) != 0) << 2) |
		((unsigned long)(zipButton.held != 0) << 3) |
		((unsigned long)(zipButton.pressed != 0) << 4) |
		((unsigned long)(player->field_AD4 != 0) << 5) |
		((unsigned long)(player->field_8E8 != 0) << 6) |
		((unsigned long)(player->field_8E9 != 0) << 7);

	if (r2)
	{
		signature |=
			((unsigned long)(r2Button.held != 0) << 8) |
			((unsigned long)(*(volatile unsigned char*)0x0060CFC7 != 0) << 9) |
			((unsigned long)(player->field_1AC != 0) << 10) |
			((unsigned long)(*((unsigned char*)player + 0xE8D) != 0) << 11);
	}

	signature ^=
		((unsigned long)player->field_E1C << 12);

	return signature;
}

static void SpideyLogZipCheck(
		const char* kind,
		CPlayer* player,
		unsigned long callNumber,
		u8 result,
		unsigned long stateBefore,
		const SpideyZipButtonRecordSnapshot& aimButton,
		const SpideyZipButtonRecordSnapshot& zipButton,
		const SpideyZipButtonRecordSnapshot& r2Button,
		const SpideyZipButtonRecordSnapshot& extraButton,
		unsigned long signature,
		unsigned long* lastSignature)
{
	if (!player ||
		!lastSignature)
	{
		return;
	}

	const unsigned long tick =
		(unsigned long)
		*(volatile long*)0x006B4CA8;

	const int changed =
		signature !=
			*lastSignature;

	const int inputInteresting =
		zipButton.held ||
		zipButton.pressed ||
		zipButton.rapidPress ||
		r2Button.held ||
		r2Button.pressed ||
		extraButton.held ||
		extraButton.pressed;

	if (callNumber > 12 &&
		!changed &&
		!inputInteresting &&
		!result &&
		(tick % 60UL) != 0)
	{
		return;
	}

	*lastSignature =
		signature;

	if (gSpideyZipTraceStored >=
		2048)
	{
		++gSpideyZipTraceDropped;
		return;
	}

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (!f)
		return;

	unsigned char raw550 = 0;
	unsigned char rawE8D = 0;
	__try
	{
		raw550 =
			*((unsigned char*)player + 0x550);
		rawE8D =
			*((unsigned char*)player + 0xE8D);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		raw550 = 0;
		rawE8D = 0;
	}

	fprintf(
		f,
		"web_zip_check kind=%s call=%lu tick=%lu result=%u state_before=0x%08lX state_after=0x%08lX anim=%u frame=%d finished=%u aim_flag=%u held_object=%d gate_550=%u ignore_input=%ld crawl=%u wall=%u ceiling=%u e8d=%u script=%u global_cfc7=%u input40=%u,%u,%u,%ld,%ld,%ld input60=%u,%u,%u,%ld,%ld,%ld input70=%u,%u,%u,%ld,%ld,%ld input100=%u,%u,%u,%ld,%ld,%ld axes=%d,%d pos=%ld,%ld,%ld vel=%ld,%ld,%ld\n",
		kind ? kind : "unknown",
		callNumber,
		tick,
		(unsigned int)result,
		stateBefore,
		(unsigned long)player->field_E1C,
		(unsigned int)player->mAnim,
		(int)player->mFrame,
		(unsigned int)player->mAnimFinished,
		(unsigned int)player->field_8EA,
		player->mHeldObject ? 1 : 0,
		(unsigned int)raw550,
		(long)player->field_E18,
		(unsigned int)player->field_AD4,
		(unsigned int)player->field_8E8,
		(unsigned int)player->field_8E9,
		(unsigned int)rawE8D,
		(unsigned int)player->field_1AC,
		(unsigned int)*(volatile unsigned char*)0x0060CFC7,
		aimButton.held,
		aimButton.pressed,
		aimButton.rapidPress,
		aimButton.heldTicks,
		aimButton.releasedTicks,
		aimButton.transitionTicks,
		zipButton.held,
		zipButton.pressed,
		zipButton.rapidPress,
		zipButton.heldTicks,
		zipButton.releasedTicks,
		zipButton.transitionTicks,
		r2Button.held,
		r2Button.pressed,
		r2Button.rapidPress,
		r2Button.heldTicks,
		r2Button.releasedTicks,
		r2Button.transitionTicks,
		extraButton.held,
		extraButton.pressed,
		extraButton.rapidPress,
		extraButton.heldTicks,
		extraButton.releasedTicks,
		extraButton.transitionTicks,
		(int)player->field_E2D,
		(int)player->field_E2E,
		(long)player->mPos.vx,
		(long)player->mPos.vy,
		(long)player->mPos.vz,
		(long)player->mVel.vx,
		(long)player->mVel.vy,
		(long)player->mVel.vz);
	fclose(f);

	++gSpideyZipTraceStored;
}

static u8 __fastcall SpideyTraceR1ZipCheck(
		CPlayer* player,
		void*)
{
	SpideyRetailZipCheckFn retail =
		(SpideyRetailZipCheckFn)0x004C0EE0;
	++gSpideyZipR1Calls;

	if (!player)
		return retail(
			player,
			0);

	unsigned char* input =
		(unsigned char*)player->field_E0C;
	SpideyZipButtonRecordSnapshot aimButton;
	SpideyZipButtonRecordSnapshot zipButton;
	SpideyZipButtonRecordSnapshot r2Button;
	SpideyZipButtonRecordSnapshot extraButton;
	SpideyReadZipButtonRecord(
		input,
		0x40,
		&aimButton);
	SpideyReadZipButtonRecord(
		input,
		0x60,
		&zipButton);
	SpideyReadZipButtonRecord(
		input,
		0x70,
		&r2Button);
	SpideyReadZipButtonRecord(
		input,
		0x100,
		&extraButton);

	const unsigned long stateBefore =
		(unsigned long)player->field_E1C;
	const unsigned long signature =
		SpideyBuildZipGateSignature(
			player,
			0,
			zipButton,
			r2Button);

	u8 result =
		0;
	int aimedHandled =
		0;

	result =
		SpideyTryModernAimedR1Zip(
			player,
			retail,
			aimButton,
			zipButton,
			&aimedHandled);

	if (!aimedHandled)
	{
		const int staleAimCleared =
			SpideyZipClearStaleModernAimGate(
				player,
				aimButton);

		result =
			retail(
				player,
				0);

		SpideyZipRestoreStaleModernAimGate(
			player,
			staleAimCleared,
			result);
	}

	if (result)
	{
		++gSpideyZipR1Success;
		// RenderLookaroundReticle is gated by field_DE4, not by the retail
		// aim-state bit. Clear the target marker on every successful zip so a
		// stale manual-aim reticle cannot be carried to the new zip target.
		player->field_DE4 =
			0;
		Screen_TargetOn(
			false);
	}

	SpideyLogZipCheck(
		"r1",
		player,
		gSpideyZipR1Calls,
		result,
		stateBefore,
		aimButton,
		zipButton,
		r2Button,
		extraButton,
		signature,
		&gSpideyZipR1LastSignature);

	return result;
}

static u8 __fastcall SpideyTraceR2ZipCheck(
		CPlayer* player,
		void*)
{
	SpideyRetailZipCheckFn retail =
		(SpideyRetailZipCheckFn)0x004C1460;
	++gSpideyZipR2Calls;

	if (!player)
		return retail(
			player,
			0);

	unsigned char* input =
		(unsigned char*)player->field_E0C;
	SpideyZipButtonRecordSnapshot aimButton;
	SpideyZipButtonRecordSnapshot zipButton;
	SpideyZipButtonRecordSnapshot r2Button;
	SpideyZipButtonRecordSnapshot extraButton;
	SpideyReadZipButtonRecord(
		input,
		0x40,
		&aimButton);
	SpideyReadZipButtonRecord(
		input,
		0x60,
		&zipButton);
	SpideyReadZipButtonRecord(
		input,
		0x70,
		&r2Button);
	SpideyReadZipButtonRecord(
		input,
		0x100,
		&extraButton);

	const unsigned long stateBefore =
		(unsigned long)player->field_E1C;
	const unsigned long signature =
		SpideyBuildZipGateSignature(
			player,
			1,
			zipButton,
			r2Button);

	u8 result =
		0;
	const int blockedByModernAim =
		SpideyZipBlockedByModernAimLocomotion(
			player,
			aimButton);

	if (!blockedByModernAim)
	{
		const int staleAimCleared =
			SpideyZipClearStaleModernAimGate(
				player,
				aimButton);

		result =
			retail(
				player,
				0);

		SpideyZipRestoreStaleModernAimGate(
			player,
			staleAimCleared,
			result);
	}

	if (result)
	{
		++gSpideyZipR2Success;
		player->field_DE4 =
			0;
		Screen_TargetOn(
			false);
	}

	SpideyLogZipCheck(
		"r2",
		player,
		gSpideyZipR2Calls,
		result,
		stateBefore,
		aimButton,
		zipButton,
		r2Button,
		extraButton,
		signature,
		&gSpideyZipR2LastSignature);

	return result;
}

static u8 __fastcall SpideyTraceZipAvailability(
		CPlayer* player,
		void*,
		SLineInfo* lineInfo,
		i32 maxDistance)
{
	SpideyRetailZipAvailabilityFn retail =
		(SpideyRetailZipAvailabilityFn)0x004C30D0;
	++gSpideyZipAvailabilityCalls;

	u8 result =
		retail(
			player,
			0,
			lineInfo,
			maxDistance);

	long faceFlags = 0;
	if (lineInfo)
	{
		__try
		{
			if (lineInfo->pFace)
			{
				faceFlags =
					(long)lineInfo->pFace[3];
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			faceFlags = 0;
		}
	}

	const long minDistance =
		player &&
		player->field_E1C == 4 ?
			8 :
			16;
	const int aimedCameraValidation =
		player &&
		lineInfo &&
		gSpideyModernAimedZipValidationPlayer ==
			player;

	if (aimedCameraValidation)
	{
		// The retail orientation test is defined relative to Spider-Man's
		// current crawl/surface basis because legacy R1 can only search along
		// that basis. Aimed zip already acquired this candidate through the
		// center-camera ray, so retain the retail range and non-zippable-face
		// rules while replacing only that obsolete orientation constraint.
		result =
			lineInfo->Distance >
				minDistance &&
			lineInfo->Distance <
				maxDistance &&
			!(faceFlags & 0x40000) ?
				1 :
				0;
	}

	if (!player ||
		!lineInfo ||
		gSpideyZipAvailabilityStored >=
			512)
	{
		return result;
	}

	const char* reason =
		result ?
			(aimedCameraValidation ?
				"accepted_aimed_camera" :
				"accepted") :
		lineInfo->Distance <= minDistance ?
			"distance_low" :
		lineInfo->Distance >= maxDistance ?
			"distance_high" :
		(faceFlags & 0x40000) ?
			"face_no_zip" :
			(aimedCameraValidation ?
				"aimed_camera_rejected" :
				"orientation_or_other");

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"web_zip_availability call=%lu tick=%ld result=%u reason=%s state=0x%08lX distance=%ld min=%ld max=%ld face_flags=0x%08lX hit_item=%d position=%ld,%ld,%ld normal=%d,%d,%d player_pos=%ld,%ld,%ld\n",
			gSpideyZipAvailabilityCalls,
			(long)*(volatile long*)0x006B4CA8,
			(unsigned int)result,
			reason,
			(unsigned long)player->field_E1C,
			(long)lineInfo->Distance,
			minDistance,
			(long)maxDistance,
			(unsigned long)faceFlags,
			lineInfo->pItem ? 1 : 0,
			(long)lineInfo->Position.vx,
			(long)lineInfo->Position.vy,
			(long)lineInfo->Position.vz,
			(int)lineInfo->Normal.vx,
			(int)lineInfo->Normal.vy,
			(int)lineInfo->Normal.vz,
			(long)player->mPos.vx,
			(long)player->mPos.vy,
			(long)player->mPos.vz);
		fclose(f);
		++gSpideyZipAvailabilityStored;
	}

	return result;
}

static unsigned long gSpideyPlayerStateTraceStored = 0;
static unsigned long gSpideyPlayerStateTraceDropped = 0;
static unsigned long gSpideyPlayerStateTraceLastState = 0xFFFFFFFFUL;
static int gSpideyPlayerStateTraceLastAnim = -1;
static int gSpideyPlayerStateTraceLastCrawl = -1;
static int gSpideyPlayerStateTraceLastAim = -1;
static CPlayer* gSpideyPlayerStateTraceLastPlayer = 0;

static void SpideyTracePlayerStateAfterLogic()
{
	CPlayer* player =
		*(CPlayer**)0x006A9038;
	if (!player)
	{
		gSpideyPlayerStateTraceLastPlayer =
			0;
		return;
	}

	if (gSpideyPlayerStateTraceLastPlayer !=
		player)
	{
		gSpideyPlayerStateTraceLastPlayer =
			player;
		gSpideyPlayerStateTraceLastState =
			0xFFFFFFFFUL;
		gSpideyPlayerStateTraceLastAnim =
			-1;
		gSpideyPlayerStateTraceLastCrawl =
			-1;
		gSpideyPlayerStateTraceLastAim =
			-1;
	}

	const unsigned long tick =
		(unsigned long)
		*(volatile long*)0x006B4CA8;
	const unsigned long state =
		(unsigned long)player->field_E1C;
	const int anim =
		(int)player->mAnim;
	const int crawl =
		(int)player->field_AD4;
	const int aim =
		(int)player->field_8EA;

	const int changed =
		state !=
			gSpideyPlayerStateTraceLastState ||
		anim !=
			gSpideyPlayerStateTraceLastAnim ||
		crawl !=
			gSpideyPlayerStateTraceLastCrawl ||
		aim !=
			gSpideyPlayerStateTraceLastAim;

	if (!changed &&
		(tick % 30UL) != 0)
	{
		return;
	}

	gSpideyPlayerStateTraceLastState =
		state;
	gSpideyPlayerStateTraceLastAnim =
		anim;
	gSpideyPlayerStateTraceLastCrawl =
		crawl;
	gSpideyPlayerStateTraceLastAim =
		aim;

	if (gSpideyPlayerStateTraceStored >=
		1024)
	{
		++gSpideyPlayerStateTraceDropped;
		return;
	}

	unsigned char* input =
		(unsigned char*)player->field_E0C;
	SpideyZipButtonRecordSnapshot jumpButton;
	SpideyZipButtonRecordSnapshot aimButton;
	SpideyZipButtonRecordSnapshot zipButton;
	SpideyZipButtonRecordSnapshot webButton;
	SpideyZipButtonRecordSnapshot extraButton;
	SpideyReadZipButtonRecord(
		input,
		0x30,
		&jumpButton);
	SpideyReadZipButtonRecord(
		input,
		0x40,
		&aimButton);
	SpideyReadZipButtonRecord(
		input,
		0x60,
		&zipButton);
	SpideyReadZipButtonRecord(
		input,
		0x70,
		&webButton);
	SpideyReadZipButtonRecord(
		input,
		0x100,
		&extraButton);

	CCamera* camera =
		*(CCamera**)0x0056F3B8;

	FILE* f =
		SpideyOpenConsolidatedLog(
			"TIMING");
	if (f)
	{
		fprintf(
			f,
			"player_state_trace sample=%lu tick=%lu camera_mode=%d state=0x%08lX anim=%u frame=%d frac=%d finished=%u crawl=%u aim=%u wall=%u ceiling=%u ignore_input=%ld script=%u held_object=%d input30=%u,%u input40=%u,%u input60=%u,%u input70=%u,%u input100=%u,%u axes=%d,%d collision=0x%08lX pos=%ld,%ld,%ld vel=%ld,%ld,%ld acc=%ld,%ld,%ld\n",
			gSpideyPlayerStateTraceStored,
			tick,
			camera ?
				(int)camera->mCameraMode :
				-1,
			state,
			(unsigned int)player->mAnim,
			(int)player->mFrame,
			(int)player->mFrameFrac,
			(unsigned int)player->mAnimFinished,
			(unsigned int)player->field_AD4,
			(unsigned int)player->field_8EA,
			(unsigned int)player->field_8E8,
			(unsigned int)player->field_8E9,
			(long)player->field_E18,
			(unsigned int)player->field_1AC,
			player->mHeldObject ? 1 : 0,
			jumpButton.held,
			jumpButton.pressed,
			aimButton.held,
			aimButton.pressed,
			zipButton.held,
			zipButton.pressed,
			webButton.held,
			webButton.pressed,
			extraButton.held,
			extraButton.pressed,
			(int)player->field_E2D,
			(int)player->field_E2E,
			(unsigned long)player->mCollision,
			(long)player->mPos.vx,
			(long)player->mPos.vy,
			(long)player->mPos.vz,
			(long)player->mVel.vx,
			(long)player->mVel.vy,
			(long)player->mVel.vz,
			(long)player->mAcc.vx,
			(long)player->mAcc.vy,
			(long)player->mAcc.vz);
		fclose(f);
		++gSpideyPlayerStateTraceStored;
	}
}

static void __cdecl SpideyCompatLogicTiming()
{
	SpideyRetailLogicFn retail =
		(SpideyRetailLogicFn)0x00455400;

	SpideyRecordChaseLogicScheduler();

	LARGE_INTEGER retailStart;
	LARGE_INTEGER retailEnd;
	QueryPerformanceCounter(
		&retailStart);
	retail();
	QueryPerformanceCounter(
		&retailEnd);

	SpideyTracePlayerStateAfterLogic();

	gSpideyTimingOutsideLogicRetailUs +=
		SpideyTimingElapsedUs(
			&retailStart,
			&retailEnd);
	++gSpideyTimingOutsideLogicCalls;

	const unsigned long now =
		(unsigned long)GetTickCount();

	if (!gSpideyTimingLogicWindowStart)
	{
		gSpideyTimingLogicWindowStart =
			now;
		gSpideyTimingLogicTicks =
			0;
	}

	++gSpideyTimingLogicTicks;

	const unsigned long elapsed =
		now -
		gSpideyTimingLogicWindowStart;

	if (elapsed >= 1000)
	{
		LARGE_INTEGER telemetryStart;
		LARGE_INTEGER telemetryEnd;
		QueryPerformanceCounter(
			&telemetryStart);
		SpideyLogTimingWindow(
			"logic",
			elapsed,
			gSpideyTimingLogicTicks);
		QueryPerformanceCounter(
			&telemetryEnd);
		gSpideyTimingOutsideLogicTelemetryUs +=
			SpideyTimingElapsedUs(
				&telemetryStart,
				&telemetryEnd);

		gSpideyTimingLogicWindowStart =
			now;
		gSpideyTimingLogicTicks =
			0;
	}
}

static void SpideyRecordPresentTiming()
{
	SpideyRecordChasePresentScheduler();

	// These accumulators cover exactly the work since the previous presenter
	// entry. Snapshot/reset them before doing any current presenter work.
	const unsigned long outsideLogicRetailUs =
		gSpideyTimingOutsideLogicRetailUs;
	const unsigned long outsideLogicTelemetryUs =
		gSpideyTimingOutsideLogicTelemetryUs;
	const unsigned long outsideLogicCalls =
		gSpideyTimingOutsideLogicCalls;
	gSpideyTimingOutsideLogicRetailUs =
		0;
	gSpideyTimingOutsideLogicTelemetryUs =
		0;
	gSpideyTimingOutsideLogicCalls =
		0;

	SpideyModernAimValidateLocomotionMaskAtFrameEnd();
	SpideyModernAimValidateZipReleaseLatchAtFrameEnd();

	SpideyTryRebindBinkAudio(
		"frame_safe_point");

	if (!gSpideyTimingPerfFrequency.QuadPart)
	{
		QueryPerformanceFrequency(
			&gSpideyTimingPerfFrequency);
	}

	if (gSpideyTimingPerfFrequency.QuadPart)
	{
		LARGE_INTEGER presentCounter;
		if (QueryPerformanceCounter(
				&presentCounter))
		{
			if (gSpideyTimingLastPresentCounter.QuadPart)
			{
				const LONGLONG deltaTicks =
					presentCounter.QuadPart -
					gSpideyTimingLastPresentCounter.QuadPart;
				const unsigned long deltaUs =
					(unsigned long)(
						(deltaTicks * (LONGLONG)1000000) /
						gSpideyTimingPerfFrequency.QuadPart);

				++gSpideyTimingPresentIntervals;
				if (deltaUs > 20000)
					++gSpideyTimingPresentOver20Ms;
				if (deltaUs > 25000)
					++gSpideyTimingPresentOver25Ms;

				if (!gSpideyFrontendLegacyMode &&
					deltaUs >
						SPIDEY_TIMING_SLOW_EVENT_THRESHOLD_US)
				{
					++gSpideyTimingSlowEventCount;

					if (gSpideyTimingSlowEventStored <
						SPIDEY_TIMING_SLOW_EVENT_CAPACITY)
					{
						SpideyTimingSlowEvent& event =
							gSpideyTimingSlowEvents[
								gSpideyTimingSlowEventStored++];
						event.frame =
							gSpideyPresentFrame;
						event.intervalUs =
							deltaUs;
						event.webTargetCalls =
							gSpideyCameraWebTargetCalls;
						event.checkWebShotCalls =
							gSpideyCameraCheckWebShotCalls;
						event.fireWebCalls =
							gSpideyFireWebCalls;
						event.lastFireWebFrame =
							gSpideyLastFireWebFrame;
						event.presentWorkUs =
							gSpideyTimingLastPresentWorkUs;
						event.outsidePresentUs =
							deltaUs >
								gSpideyTimingLastPresentWorkUs ?
								deltaUs -
									gSpideyTimingLastPresentWorkUs :
								0;
						event.recordTimingUs =
							gSpideyTimingLastRecordTimingUs;
						event.transientUs =
							gSpideyTimingLastTransientUs;
						event.shadowEndUs =
							gSpideyTimingLastShadowEndUs;
						event.drawProbeUs =
							gSpideyTimingLastDrawProbeUs;
						event.presentShadowUs =
							gSpideyTimingLastPresentShadowUs;
						event.otherPresentUs =
							gSpideyTimingLastOtherPresentUs;
						event.logicRetailUs =
							outsideLogicRetailUs;
						event.logicTelemetryUs =
							outsideLogicTelemetryUs;
						event.logicCalls =
							outsideLogicCalls;
						const unsigned long logicAccountedUs =
							outsideLogicRetailUs +
							outsideLogicTelemetryUs;
						event.outsideNonLogicUs =
							event.outsidePresentUs >
								logicAccountedUs ?
								event.outsidePresentUs -
									logicAccountedUs :
								0;
					}
				}
				if (deltaUs > 30000)
					++gSpideyTimingPresentOver30Ms;
				if (deltaUs > 50000)
					++gSpideyTimingPresentOver50Ms;
				if (deltaUs >
						gSpideyTimingPresentMaxIntervalUs)
				{
					gSpideyTimingPresentMaxIntervalUs =
						deltaUs;
				}
			}

			gSpideyTimingLastPresentCounter =
				presentCounter;
		}
	}

	const long currentVblanks =
		*(volatile long*)0x006B4CA0;
	if (gSpideyTimingPresentVblankValid)
	{
		const long deltaVblanks =
			currentVblanks -
			gSpideyTimingLastPresentVblank;

		if (deltaVblanks <= 0)
		{
			++gSpideyTimingPresentVblankSame;
		}
		else if (deltaVblanks == 1)
		{
			++gSpideyTimingPresentVblankOne;
		}
		else
		{
			++gSpideyTimingPresentVblankMulti;
			if ((unsigned long)deltaVblanks >
					gSpideyTimingPresentVblankMaxDelta)
			{
				gSpideyTimingPresentVblankMaxDelta =
					(unsigned long)deltaVblanks;
			}
		}
	}
	else
	{
		gSpideyTimingPresentVblankValid =
			1;
	}
	gSpideyTimingLastPresentVblank =
		currentVblanks;

	const unsigned long now =
		(unsigned long)GetTickCount();

	if (!gSpideyTimingPresentWindowStart)
	{
		gSpideyTimingPresentWindowStart =
			now;
		gSpideyTimingPresentFrames =
			0;
	}

	++gSpideyTimingPresentFrames;

	const unsigned long elapsed =
		now -
		gSpideyTimingPresentWindowStart;

	if (elapsed >= 1000)
	{
		SpideyLogTimingWindow(
			"present",
			elapsed,
			gSpideyTimingPresentFrames);
		gSpideyTimingPresentWindowStart =
			now;
		gSpideyTimingPresentFrames =
			0;
		gSpideyTimingPresentIntervals =
			0;
		gSpideyTimingPresentOver20Ms =
			0;
		gSpideyTimingPresentOver25Ms =
			0;
		gSpideyTimingPresentOver30Ms =
			0;
		gSpideyTimingPresentOver50Ms =
			0;
		gSpideyTimingPresentMaxIntervalUs =
			0;
		gSpideyTimingPresentVblankSame =
			0;
		gSpideyTimingPresentVblankOne =
			0;
		gSpideyTimingPresentVblankMulti =
			0;
		gSpideyTimingPresentVblankMaxDelta =
			0;
		gSpideyTimingSlowEventCount =
			0;
		gSpideyTimingSlowEventStored =
			0;
	}
}

static void SpideyInstallTimingTelemetry()
{
	char timingTelemetryValue[8];
	memset(
		timingTelemetryValue,
		0,
		sizeof(timingTelemetryValue));
	const DWORD timingTelemetryLength =
		GetEnvironmentVariableA(
			"SPIDEY_TIMING_FILE_TELEMETRY",
			timingTelemetryValue,
			sizeof(timingTelemetryValue));
	gSpideyTimingFileTelemetryEnabled =
		timingTelemetryLength > 0 &&
		timingTelemetryValue[0] != '0' ?
			1 :
			0;

	const int timerPacingInstalled =
		SpideyInstallModernTimerPacing();

	const int logicInstalled =
		SpideyPatchDirectCall(
			0x00455A8B,
			0x00455400,
			SpideyCompatLogicTiming,
			"gameplay_logic_timing");
	const int fireWebHooks =
		SpideyPatchDirectCallsToTargetInRange(
			0x00401000,
			0x0053B000,
			0x004C5DD0,
			(void*)&SpideyTimingFireWeb,
			"timing_fire_web");

	const int zipR1Hooks =
		SpideyPatchDirectCallsToTargetInRange(
			0x004B13F0,
			0x004B8790,
			0x004C0EE0,
			(void*)&SpideyTraceR1ZipCheck,
			"timing_web_zip_r1_trace");
	const int zipR2Hooks =
		SpideyPatchDirectCallsToTargetInRange(
			0x004B13F0,
			0x004B8790,
			0x004C1460,
			(void*)&SpideyTraceR2ZipCheck,
			"timing_web_zip_r2_trace");
	const int zipAvailabilityR1 =
		SpideyPatchDirectCall(
			0x004C104F,
			0x004C30D0,
			(void*)&SpideyTraceZipAvailability,
			"timing_web_zip_availability_r1");
	const int zipAvailabilityR2 =
		SpideyPatchDirectCall(
			0x004C165C,
			0x004C30D0,
			(void*)&SpideyTraceZipAvailability,
			"timing_web_zip_availability_r2");

	FILE* f = SpideyOpenConsolidatedLog(
		"TIMING");
	if (f)
	{
		fprintf(
			f,
			"timing_install logic=%d call=0x00455A8B retail=0x00455400 engine_vblanks=0x006B4CA0 modern_timer_pacing=%d fire_web_hooks=%d fire_web_target=0x004C5DD0 slow_threshold_us=%u file_telemetry=%d opt_in_env=SPIDEY_TIMING_FILE_TELEMETRY\n",
			logicInstalled,
			timerPacingInstalled,
			fireWebHooks,
			SPIDEY_TIMING_SLOW_EVENT_THRESHOLD_US,
			gSpideyTimingFileTelemetryEnabled);
		fprintf(
			f,
			"web_zip_trace_install r1_hooks=%d r1_target=0x004C0EE0 r2_hooks=%d r2_target=0x004C1460 availability_r1=%d availability_r2=%d availability_target=0x004C30D0 player_state_trace=post_logic input_base=0x00661100 zip_held=0x00661160 aim_held=0x00661140 r2_held=0x00661170 extra_held=0x00661200\n",
			zipR1Hooks,
			zipR2Hooks,
			zipAvailabilityR1,
			zipAvailabilityR2);
		fclose(f);
	}
}

static int gSpideyModernRangeValid = 0;
static float gSpideyModernMinX = 0.0f;
static float gSpideyModernMaxX = 0.0f;
static float gSpideyModernMinY = 0.0f;
static float gSpideyModernMaxY = 0.0f;
static unsigned long gSpideyModernVertexCount = 0;
static unsigned long gSpideyModernOutsidePhysicalX = 0;
static unsigned long gSpideyModernOutsidePhysicalY = 0;

static int gSpidey2DRangeValid = 0;
static float gSpidey2DMinX = 0.0f;
static float gSpidey2DMaxX = 0.0f;
static float gSpidey2DMinY = 0.0f;
static float gSpidey2DMaxY = 0.0f;
static int gSpidey3DRangeValid = 0;
static float gSpidey3DMinX = 0.0f;
static float gSpidey3DMaxX = 0.0f;
static float gSpidey3DMinY = 0.0f;
static float gSpidey3DMaxY = 0.0f;

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

// @Ok
static int SpideyEnsureD3D7FallbackScene(
		LPDIRECT3DDEVICE7 device)
{
	if (!SpideyDx11AuthoritativeActive() ||
		!gSpideyDx11VirtualSceneActive ||
		gSpideyD3D7FallbackSceneActive)
	{
		return 1;
	}

	if (!gSpideyRetailD3D7BeginSceneOriginal ||
		!device)
	{
		return 0;
	}

	const HRESULT hr =
		gSpideyRetailD3D7BeginSceneOriginal(
			device);
	if (FAILED(hr))
		return 0;

	gSpideyD3D7FallbackSceneActive =
		1;
	++gSpideyD3D7FallbackSceneBegins;
	return 1;
}

// @Ok
static HRESULT WINAPI SpideyCompatD3D7BeginScene(
		LPDIRECT3DDEVICE7 device)
{
	if (SpideyDx11AuthoritativeActive())
	{
		gSpideyDx11VirtualSceneActive =
			1;
		gSpideyD3D7FallbackSceneActive =
			0;
		++gSpideyD3D7BeginSceneSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7BeginSceneOriginal)
		return E_FAIL;

	const HRESULT hr =
		gSpideyRetailD3D7BeginSceneOriginal(
			device);
	if (SUCCEEDED(hr))
		gSpideyD3D7FallbackSceneActive = 1;
	return hr;
}

// @Ok
static HRESULT WINAPI SpideyCompatD3D7EndScene(
		LPDIRECT3DDEVICE7 device)
{
	if (SpideyDx11AuthoritativeActive() &&
		gSpideyDx11VirtualSceneActive)
	{
		HRESULT hr =
			S_OK;

		if (gSpideyD3D7FallbackSceneActive)
		{
			if (!gSpideyRetailD3D7EndSceneOriginal)
				hr = E_FAIL;
			else
				hr =
					gSpideyRetailD3D7EndSceneOriginal(
						device);
		}
		else
		{
			++gSpideyD3D7EndSceneSuppressed;
		}

		gSpideyDx11VirtualSceneActive =
			0;
		gSpideyD3D7FallbackSceneActive =
			0;
		return hr;
	}

	if (!gSpideyRetailD3D7EndSceneOriginal)
		return E_FAIL;

	const HRESULT hr =
		gSpideyRetailD3D7EndSceneOriginal(
			device);
	gSpideyD3D7FallbackSceneActive =
		0;
	return hr;
}

// @Ok
static HRESULT WINAPI SpideyCompatD3D7SurfaceBlt(
		LPDIRECTDRAWSURFACE7 destination,
		LPRECT destinationRect,
		LPDIRECTDRAWSURFACE7 source,
		LPRECT sourceRect,
		DWORD flags,
		LPDDBLTFX effects)
{
	const LPDIRECTDRAWSURFACE7 primary =
		*(LPDIRECTDRAWSURFACE7*)0x006B7904;
	const LPDIRECTDRAWSURFACE7 scene =
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	if (SpideyDx11AuthoritativeActive() &&
		(destination == primary ||
		 destination == scene))
	{
		++gSpideyD3D7MainBltSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7SurfaceBltOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7SurfaceBltOriginal(
		destination,
		destinationRect,
		source,
		sourceRect,
		flags,
		effects);
}

// @Ok
static int SpideyReplayCachedD3D7State(
		LPDIRECT3DDEVICE7 device)
{
	if (!device ||
		!gSpideyRetailShadowStateValid)
	{
		return 0;
	}

	int ok =
		1;

	if (gSpideyRetailD3D7SetViewportOriginal)
	{
		D3DVIEWPORT7 viewport;
		memset(
			&viewport,
			0,
			sizeof(viewport));
		viewport.dwX =
			gSpideyRetailShadowState.viewportX;
		viewport.dwY =
			gSpideyRetailShadowState.viewportY;
		viewport.dwWidth =
			gSpideyRetailShadowState.viewportWidth;
		viewport.dwHeight =
			gSpideyRetailShadowState.viewportHeight;
		viewport.dvMinZ =
			gSpideyRetailShadowState.viewportMinZ;
		viewport.dvMaxZ =
			gSpideyRetailShadowState.viewportMaxZ;

		if (FAILED(
				gSpideyRetailD3D7SetViewportOriginal(
					device,
					&viewport)))
		{
			ok = 0;
		}
	}

	if (gSpideyRetailD3D7SetRenderStateOriginal)
	{
		const D3DRENDERSTATETYPE renderStates[] =
		{
			D3DRENDERSTATE_ZENABLE,
			D3DRENDERSTATE_ZWRITEENABLE,
			D3DRENDERSTATE_ZFUNC,
			D3DRENDERSTATE_ALPHABLENDENABLE,
			D3DRENDERSTATE_SRCBLEND,
			D3DRENDERSTATE_DESTBLEND,
			D3DRENDERSTATE_ALPHATESTENABLE,
			D3DRENDERSTATE_ALPHAREF,
			D3DRENDERSTATE_ALPHAFUNC,
			D3DRENDERSTATE_FOGENABLE,
			D3DRENDERSTATE_FOGCOLOR
		};
		const DWORD renderValues[] =
		{
			gSpideyRetailShadowState.zEnable,
			gSpideyRetailShadowState.zWrite,
			gSpideyRetailShadowState.zFunc,
			gSpideyRetailShadowState.alphaBlendEnable,
			gSpideyRetailShadowState.srcBlend,
			gSpideyRetailShadowState.dstBlend,
			gSpideyRetailShadowState.alphaTestEnable,
			gSpideyRetailShadowState.alphaRef,
			gSpideyRetailShadowState.alphaFunc,
			gSpideyRetailShadowState.fogEnable,
			gSpideyRetailShadowState.fogColor
		};

		for (int i = 0;
			 i < (int)(sizeof(renderStates) /
					 sizeof(renderStates[0]));
			 ++i)
		{
			if (FAILED(
					gSpideyRetailD3D7SetRenderStateOriginal(
						device,
						renderStates[i],
						renderValues[i])))
			{
				ok = 0;
			}
		}
	}

	if (gSpideyRetailD3D7SetTextureStageStateOriginal)
	{
		const D3DTEXTURESTAGESTATETYPE textureStates[] =
		{
			D3DTSS_COLOROP,
			D3DTSS_COLORARG1,
			D3DTSS_COLORARG2,
			D3DTSS_ALPHAOP,
			D3DTSS_ALPHAARG1,
			D3DTSS_ALPHAARG2,
			D3DTSS_ADDRESSU,
			D3DTSS_ADDRESSV,
			D3DTSS_MAGFILTER,
			D3DTSS_MINFILTER
		};
		const DWORD textureValues[] =
		{
			gSpideyRetailShadowState.colorOp,
			gSpideyRetailShadowState.colorArg1,
			gSpideyRetailShadowState.colorArg2,
			gSpideyRetailShadowState.alphaOp,
			gSpideyRetailShadowState.alphaArg1,
			gSpideyRetailShadowState.alphaArg2,
			gSpideyRetailShadowState.addressU,
			gSpideyRetailShadowState.addressV,
			gSpideyRetailShadowState.magFilter,
			gSpideyRetailShadowState.minFilter
		};

		for (int i = 0;
			 i < (int)(sizeof(textureStates) /
					 sizeof(textureStates[0]));
			 ++i)
		{
			if (FAILED(
					gSpideyRetailD3D7SetTextureStageStateOriginal(
						device,
						0,
						textureStates[i],
						textureValues[i])))
			{
				ok = 0;
			}
		}
	}

	if (gSpideyRetailD3D7SetTextureOriginal)
	{
		if (FAILED(
				gSpideyRetailD3D7SetTextureOriginal(
					device,
					0,
					(LPDIRECTDRAWSURFACE7)
						gSpideyRetailShadowState.textureHandle)))
		{
			ok = 0;
		}
	}

	return ok;
}

static HRESULT WINAPI SpideyShadowD3D7SetRenderTarget(
		LPDIRECT3DDEVICE7 device,
		LPDIRECTDRAWSURFACE7 renderTarget,
		DWORD flags)
{
	const LPDIRECTDRAWSURFACE7 mainScene =
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	// Shadow/DX11 state owns the main target. Do not bind the legacy display
	// surface after DX11 has become authoritative; a DXGI mode switch may
	// legitimately leave that DirectDraw surface lost.
	if (SpideyDx11AuthoritativeActive() &&
		renderTarget == mainScene)
	{
		gSpideyRetailShadowRenderTarget =
			renderTarget;
		return S_OK;
	}

	// If a genuinely offscreen path appears after a virtual main-scene
	// BeginScene, start D3D7 lazily only for that compatibility work.
	const int fallbackWasActive =
		gSpideyD3D7FallbackSceneActive;

	if (SpideyDx11AuthoritativeActive() &&
		gSpideyDx11VirtualSceneActive &&
		renderTarget != mainScene &&
		!SpideyEnsureD3D7FallbackScene(
			device))
	{
		return E_FAIL;
	}

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

		if (SpideyDx11AuthoritativeActive() &&
			renderTarget != mainScene &&
			!fallbackWasActive &&
			gSpideyD3D7FallbackSceneActive)
		{
			SpideyReplayCachedD3D7State(
				device);
		}
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
	const int onMainScene =
		SpideyDx11OnMainScene();

	if (count == 0 &&
		onMainScene)
	{
		SpideyRenderer11ShadowSetClear(
			(unsigned long)flags,
			(unsigned long)color,
			z,
			(unsigned long)stencil);
	}

	if (SpideyDx11AuthoritativeActive() &&
		onMainScene)
	{
		++gSpideyD3D7MainClearSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7ClearOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7ClearOriginal(
		device,
		count,
		rects,
		flags,
		color,
		z,
		stencil);
}

static HRESULT WINAPI SpideyShadowD3D7SetViewport(
		LPDIRECT3DDEVICE7 device,
		LPD3DVIEWPORT7 viewport)
{
	if (viewport)
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

	if (SpideyDx11AuthoritativeActive() &&
		SpideyDx11OnMainScene())
	{
		++gSpideyD3D7MainStateSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7SetViewportOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7SetViewportOriginal(
		device,
		viewport);
}

static HRESULT WINAPI SpideyShadowD3D7SetRenderState(
		LPDIRECT3DDEVICE7 device,
		D3DRENDERSTATETYPE state,
		DWORD value)
{
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

	gSpideyRetailShadowStateValid =
		1;

	if (SpideyDx11AuthoritativeActive() &&
		SpideyDx11OnMainScene())
	{
		++gSpideyD3D7MainStateSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7SetRenderStateOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7SetRenderStateOriginal(
		device,
		state,
		value);
}

static HRESULT WINAPI SpideyShadowD3D7SetTexture(
		LPDIRECT3DDEVICE7 device,
		DWORD stage,
		LPDIRECTDRAWSURFACE7 texture)
{
	if (stage == 0)
	{
		gSpideyRetailShadowState.textureHandle =
			(unsigned long)texture;
		gSpideyRetailShadowStateValid =
			1;
	}

	if (SpideyDx11AuthoritativeActive() &&
		SpideyDx11OnMainScene())
	{
		++gSpideyD3D7MainStateSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7SetTextureOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7SetTextureOriginal(
		device,
		stage,
		texture);
}

static HRESULT WINAPI SpideyShadowD3D7SetTextureStageState(
		LPDIRECT3DDEVICE7 device,
		DWORD stage,
		D3DTEXTURESTAGESTATETYPE state,
		DWORD value)
{
	if (stage == 0)
	{
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

		gSpideyRetailShadowStateValid =
			1;
	}

	if (SpideyDx11AuthoritativeActive() &&
		SpideyDx11OnMainScene())
	{
		++gSpideyD3D7MainStateSuppressed;
		return S_OK;
	}

	if (!gSpideyRetailD3D7SetTextureStageStateOriginal)
		return E_FAIL;

	return gSpideyRetailD3D7SetTextureStageStateOriginal(
		device,
		stage,
		state,
		value);
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
		gSpideyDx11AuthoritativeRendering ||
		gSpideyShadowPreviewEnabled ||
		shadowFrame <= 5 ||
		(shadowFrame % 120) == 0;

	const int onMainScene =
		gSpideyRetailShadowRenderTarget ==
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	const int drawIs2D =
		vertexTypeDesc == 324 &&
		vertices &&
		SpideyIsTagged2DVertices(
			vertices);

	if (onMainScene &&
		vertexTypeDesc == 324)
	{
		if (drawIs2D)
			++gSpideyRetailDraw2D;
		else
			++gSpideyRetailDraw3D;
	}

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

				int* classRangeValid =
					drawIs2D ?
						&gSpidey2DRangeValid :
						&gSpidey3DRangeValid;
				float* classMinX =
					drawIs2D ?
						&gSpidey2DMinX :
						&gSpidey3DMinX;
				float* classMaxX =
					drawIs2D ?
						&gSpidey2DMaxX :
						&gSpidey3DMaxX;
				float* classMinY =
					drawIs2D ?
						&gSpidey2DMinY :
						&gSpidey3DMinY;
				float* classMaxY =
					drawIs2D ?
						&gSpidey2DMaxY :
						&gSpidey3DMaxY;

				if (!*classRangeValid)
				{
					*classRangeValid = 1;
					*classMinX = x;
					*classMaxX = x;
					*classMinY = y;
					*classMaxY = y;
				}
				else
				{
					if (x < *classMinX)
						*classMinX = x;
					if (x > *classMaxX)
						*classMaxX = x;
					if (y < *classMinY)
						*classMinY = y;
					if (y > *classMaxY)
						*classMaxY = y;
				}

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
		shadowState.drawClass =
			drawIs2D ?
				1UL :
				0UL;

		if (SpideyUseModernOutputAspect() &&
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
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");

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
				"draw_sample call=%lu device=0x%08lX primitive=%lu fvf=%lu vertices=0x%08lX count=%lu flags=0x%08lX texture_hr=0x%08lX texture=0x%08lX mirrored_id=%ld class=%s",
				gSpideyRetailDrawCalls,
				(unsigned long)device,
				(unsigned long)primitiveType,
				(unsigned long)vertexTypeDesc,
				(unsigned long)vertices,
				(unsigned long)vertexCount,
				(unsigned long)flags,
				(unsigned long)textureHr,
				(unsigned long)texture,
				mirroredTextureId,
				drawIs2D ?
					"2d" :
					"3d");

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

	// Diagnostic D3D7-producer removal trial. Never suppress unless the
	// currently visible DX11 path is fully active and this exact main-scene
	// draw was accepted by DX11. Require an already-resident texture rather
	// than relying on end-of-frame transient recovery so the trial fails
	// closed: any uncertain draw still executes on D3D7.
	const int textureReadyForSuppression =
		!texture ||
		mirroredTextureId >= 0;
	const int suppressRetailMainDraw =
		(SpideyDx11AuthoritativeActive() ||
		 gSpideyD3D7MainDrawSuppressionEnabled) &&
		gSpideyShadowPreviewEnabled &&
		gSpideyShadowPreviewReady &&
		onMainScene &&
		shadowSubmitted &&
		textureReadyForSuppression;

	if (suppressRetailMainDraw)
	{
		++gSpideyD3D7MainDrawSuppressed;
		return S_OK;
	}

	if ((SpideyDx11AuthoritativeActive() ||
		 gSpideyD3D7MainDrawSuppressionEnabled) &&
		onMainScene)
	{
		++gSpideyD3D7MainDrawFallback;
	}

	if (SpideyDx11AuthoritativeActive() &&
		gSpideyDx11VirtualSceneActive &&
		!SpideyEnsureD3D7FallbackScene(
			device))
	{
		return E_FAIL;
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
	gSpideyRetailDraw2D = 0;
	gSpideyRetailDraw3D = 0;
	gSpidey2DPolyTagged = 0;
	memset(
		gSpidey2DPolyTags,
		0,
		sizeof(gSpidey2DPolyTags));
	gSpideyShadowSubmitted = 0;
	gSpideyShadowSkipped = 0;
	gSpideyShadowOffscreenSkipped = 0;
	gSpideyTransientQueued = 0;
	gSpideyTransientMirrored = 0;
	gSpideyD3D7MainDrawSuppressed = 0;
	gSpideyD3D7MainDrawFallback = 0;
	gSpideyD3D7BeginSceneSuppressed = 0;
	gSpideyD3D7EndSceneSuppressed = 0;
	gSpideyD3D7MainClearSuppressed = 0;
	gSpideyD3D7MainStateSuppressed = 0;
	gSpideyD3D7MainBltSuppressed = 0;
	gSpideyD3D7FallbackSceneBegins = 0;
	gSpideyModernRangeValid = 0;
	gSpideyModernMinX = 0.0f;
	gSpideyModernMaxX = 0.0f;
	gSpideyModernMinY = 0.0f;
	gSpideyModernMaxY = 0.0f;
	gSpideyModernVertexCount = 0;
	gSpideyModernOutsidePhysicalX = 0;
	gSpideyModernOutsidePhysicalY = 0;
	gSpidey2DRangeValid = 0;
	gSpidey2DMinX = 0.0f;
	gSpidey2DMaxX = 0.0f;
	gSpidey2DMinY = 0.0f;
	gSpidey2DMaxY = 0.0f;
	gSpidey3DRangeValid = 0;
	gSpidey3DMinX = 0.0f;
	gSpidey3DMaxX = 0.0f;
	gSpidey3DMinY = 0.0f;
	gSpidey3DMaxY = 0.0f;
}

static void SpideyFlushRetailD3D7DrawProbeFrame(
		unsigned long frame)
{
	const int shouldLog =
		frame <= 5 ||
		gSpideyRetailDrawMissing != 0 ||
		gSpideyRetailDrawOtherPrimitive != 0 ||
		gSpideyRetailDrawOtherFvf != 0;

	if (shouldLog)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");

		if (f)
		{
			fprintf(
				f,
				"draw_frame frame=%lu calls=%lu textured=%lu mirrored=%lu missing=%lu triangle_fan=%lu fvf_0x144=%lu other_primitive=%lu other_fvf=%lu class_2d=%lu class_3d=%lu tagged_2d=%lu shadow_submit=%lu shadow_skip=%lu shadow_offscreen_skip=%lu transient_queued=%lu transient_mirrored=%lu dx11_authoritative=%d d3d7_suppressed=%lu d3d7_fallback=%lu begin_suppressed=%lu end_suppressed=%lu clear_suppressed=%lu state_suppressed=%lu blt_suppressed=%lu fallback_scene_begins=%lu resident=%lu device=0x%08lX modern=%d logical=%lux%lu physical=%lux%lu range_valid=%d xrange=%.3f,%.3f yrange=%.3f,%.3f class2d_valid=%d class2d_x=%.3f,%.3f class2d_y=%.3f,%.3f class3d_valid=%d class3d_x=%.3f,%.3f class3d_y=%.3f,%.3f vertices=%lu outside_physical_x=%lu outside_physical_y=%lu\n",
				frame,
				gSpideyRetailDrawCalls,
				gSpideyRetailDrawTextured,
				gSpideyRetailDrawMirrored,
				gSpideyRetailDrawMissing,
				gSpideyRetailDrawTriangleFan,
				gSpideyRetailDrawFvf144,
				gSpideyRetailDrawOtherPrimitive,
				gSpideyRetailDrawOtherFvf,
				gSpideyRetailDraw2D,
				gSpideyRetailDraw3D,
				gSpidey2DPolyTagged,
				gSpideyShadowSubmitted,
				gSpideyShadowSkipped,
				gSpideyShadowOffscreenSkipped,
				gSpideyTransientQueued,
				gSpideyTransientMirrored,
				SpideyDx11AuthoritativeActive() ? 1 : 0,
				gSpideyD3D7MainDrawSuppressed,
				gSpideyD3D7MainDrawFallback,
				gSpideyD3D7BeginSceneSuppressed,
				gSpideyD3D7EndSceneSuppressed,
				gSpideyD3D7MainClearSuppressed,
				gSpideyD3D7MainStateSuppressed,
				gSpideyD3D7MainBltSuppressed,
				gSpideyD3D7FallbackSceneBegins,
				SpideyRenderer11GetMirroredTextureCount(),
				(unsigned long)gSpideyRetailD3D7DrawProbeDevice,
				SpideyUseModernOutputAspect() ? 1 : 0,
				gSpideyModernLogicalWidth,
				gSpideyModernLogicalHeight,
				gSpideyLegacyPhysicalWidth,
				gSpideyLegacyPhysicalHeight,
				gSpideyModernRangeValid,
				gSpideyModernMinX,
				gSpideyModernMaxX,
				gSpideyModernMinY,
				gSpideyModernMaxY,
				gSpidey2DRangeValid,
				gSpidey2DMinX,
				gSpidey2DMaxX,
				gSpidey2DMinY,
				gSpidey2DMaxY,
				gSpidey3DRangeValid,
				gSpidey3DMinX,
				gSpidey3DMaxX,
				gSpidey3DMinY,
				gSpidey3DMaxY,
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
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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

	FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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

// @Ok
static int SpideyInstallRetailD3D7SurfaceCompat()
{
	const int bltIndex = 5;
	LPDIRECTDRAWSURFACE7 surfaces[2];
	surfaces[0] =
		*(LPDIRECTDRAWSURFACE7*)0x006B7904;
	surfaces[1] =
		*(LPDIRECTDRAWSURFACE7*)0x006B7908;

	int patched =
		0;

	for (int i = 0;
		 i < 2;
		 ++i)
	{
		LPDIRECTDRAWSURFACE7 surface =
			surfaces[i];
		if (!surface)
			continue;

		void** vtable =
			0;
		__try
		{
			vtable =
				*(void***)surface;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			vtable =
				0;
		}

		if (!vtable)
			continue;

		void* current =
			vtable[bltIndex];
		if (current ==
			(void*)&SpideyCompatD3D7SurfaceBlt)
		{
			++patched;
			continue;
		}

		// IDirectDrawSurface7 vtable slot 5 is Blt. Both retail main
		// surfaces use the same implementation in the supported executable.
		// Refuse to chain a second, unknown implementation into the shared
		// trampoline rather than risking recursion.
		if (gSpideyRetailD3D7SurfaceBltOriginal &&
			current !=
				(void*)gSpideyRetailD3D7SurfaceBltOriginal)
		{
			FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
			if (f)
			{
				fprintf(
					f,
					"surface_hook NOT installed surface=0x%08lX vtable=0x%08lX blt=0x%08lX expected=0x%08lX\n",
					(unsigned long)surface,
					(unsigned long)vtable,
					(unsigned long)current,
					(unsigned long)gSpideyRetailD3D7SurfaceBltOriginal);
				fclose(f);
			}
			continue;
		}

		DWORD oldProtect =
			0;
		if (!VirtualProtect(
				&vtable[bltIndex],
				sizeof(void*),
				PAGE_EXECUTE_READWRITE,
				&oldProtect))
		{
			continue;
		}

		if (!gSpideyRetailD3D7SurfaceBltOriginal)
		{
			gSpideyRetailD3D7SurfaceBltOriginal =
				(SpideyRetailD3D7SurfaceBltFn)current;
		}

		vtable[bltIndex] =
			(void*)&SpideyCompatD3D7SurfaceBlt;

		DWORD ignoredProtect =
			0;
		VirtualProtect(
			&vtable[bltIndex],
			sizeof(void*),
			oldProtect,
			&ignoredProtect);
		FlushInstructionCache(
			GetCurrentProcess(),
			&vtable[bltIndex],
			sizeof(void*));

		++patched;

		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
		if (f)
		{
			fprintf(
				f,
				"surface_hook installed surface=0x%08lX vtable=0x%08lX method=Blt index=5 original=0x%08lX wrapper=0x%08lX\n",
				(unsigned long)surface,
				(unsigned long)vtable,
				(unsigned long)gSpideyRetailD3D7SurfaceBltOriginal,
				(unsigned long)&SpideyCompatD3D7SurfaceBlt);
			fclose(f);
		}
	}

	return patched;
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
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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

	const int beginSceneIndex = 5;
	const int endSceneIndex = 6;
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
		vtable[beginSceneIndex] == (void*)&SpideyCompatD3D7BeginScene &&
		vtable[endSceneIndex] == (void*)&SpideyCompatD3D7EndScene &&
		vtable[setRenderTargetIndex] == (void*)&SpideyShadowD3D7SetRenderTarget &&
		vtable[clearIndex] == (void*)&SpideyShadowD3D7Clear &&
		vtable[setViewportIndex] == (void*)&SpideyShadowD3D7SetViewport &&
		vtable[setRenderStateIndex] == (void*)&SpideyShadowD3D7SetRenderState &&
		vtable[drawPrimitiveIndex] == (void*)&SpideyProbeD3D7DrawPrimitive &&
		vtable[setTextureIndex] == (void*)&SpideyShadowD3D7SetTexture &&
		vtable[setTextureStageStateIndex] == (void*)&SpideyShadowD3D7SetTextureStageState;

	if (alreadyInstalled)
	{
		SpideyInstallRetailD3D7SurfaceCompat();
		return;
	}

	const int beginSceneOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			beginSceneIndex,
			(void*)&SpideyCompatD3D7BeginScene,
			(void**)&gSpideyRetailD3D7BeginSceneOriginal,
			"BeginScene");

	const int endSceneOk =
		SpideyPatchRetailD3D7VtableMethod(
			vtable,
			endSceneIndex,
			(void*)&SpideyCompatD3D7EndScene,
			(void**)&gSpideyRetailD3D7EndSceneOriginal,
			"EndScene");

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

	const int surfaceHooks =
		SpideyInstallRetailD3D7SurfaceCompat();

	if (!beginSceneOk ||
		!endSceneOk ||
		!renderTargetOk ||
		!clearOk ||
		!viewportOk ||
		!renderStateOk ||
		!drawOk ||
		!textureOk ||
		!textureStateOk)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
		if (f)
		{
			fprintf(
				f,
				"draw_probe partial device=0x%08lX vtable=0x%08lX begin=%d end=%d target=%d clear=%d viewport=%d renderstate=%d draw=%d texture=%d texstate=%d surface_blt=%d\n",
				(unsigned long)device,
				(unsigned long)vtable,
				beginSceneOk,
				endSceneOk,
				renderTargetOk,
				clearOk,
				viewportOk,
				renderStateOk,
				drawOk,
				textureOk,
				textureStateOk,
				surfaceHooks);
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

	FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
	if (f)
	{
		fprintf(
			f,
			"draw_probe installed device_slot=0x006B791C device=0x%08lX vtable=0x%08lX draw_index=%d getcaps_hr=0x%08lX max_tex=%lux%lu state_hooks=9 surface_blt=%d dx11_authoritative=1 shadow_state_valid=%d\n",
			(unsigned long)device,
			(unsigned long)vtable,
			drawPrimitiveIndex,
			(unsigned long)capsHr,
			(unsigned long)caps.dwMaxTextureWidth,
			(unsigned long)caps.dwMaxTextureHeight,
			surfaceHooks,
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
	// Modern helpers are separate VS2022 Win32 DLLs behind legacy-safe C
	// ABIs. Input stays passive here: probe/poll telemetry only, with no
	// gameplay action injection until the helper has runtime proof.
	SpideyProbeInput11Bridge();

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
	SpideyApplySelectedWindowStyle(
		hwnd,
		"early_directx_init");

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

			FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");

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

static int SpideyDiagnosticSurfaceReadbackEnabled()
{
	static int enabled =
		-1;

	if (enabled >= 0)
		return enabled;

	char value[16];
	memset(
		value,
		0,
		sizeof(value));

	const DWORD length =
		GetEnvironmentVariableA(
			"SPIDEY_DIAG_SURFACE_READBACK",
			value,
			sizeof(value));

	enabled =
		length > 0 &&
		value[0] != '0' ?
			1 :
			0;

	return enabled;
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

	if (!SpideyDiagnosticSurfaceReadbackEnabled())
	{
		fprintf(
			f,
			" pixel_sample=disabled");
		fputc(
			'\n',
			f);
		return;
	}

	// GetDC + GetPixel against live DirectDraw surfaces can synchronize the
	// CPU with the compatibility renderer. Keep this old validation path
	// opt-in instead of stalling production gameplay every telemetry interval.
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
			FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
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
					FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
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

			FILE* compat = SpideyOpenConsolidatedLog(
		"COMPAT");
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
				FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
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
			FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
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
				FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
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

		FILE* compat = SpideyOpenConsolidatedLog(
		"COMPAT");
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
		FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

// @Ok
static int SpideyRetailMovieBlocksDx11Takeover()
{
	return
		*(void**)0x00AC0BA4 ||
		*(void**)0x00AC0A3C ?
			1 :
			0;
}

typedef void (__cdecl *SpideyRetailFlipFn)(void);

static void __cdecl SpideyDiagDXPOLYFlip(void)
{
	LARGE_INTEGER presentWorkStart;
	LARGE_INTEGER presentWorkEnd;
	LARGE_INTEGER phaseStart;
	LARGE_INTEGER phaseEnd;
	QueryPerformanceCounter(
		&presentWorkStart);

	unsigned long recordTimingUs =
		0;
	unsigned long transientUs =
		0;
	unsigned long shadowEndUs =
		0;
	unsigned long drawProbeUs =
		0;
	unsigned long presentShadowUs =
		0;

	const unsigned long frame =
		++gSpideyPresentFrame;

	QueryPerformanceCounter(
		&phaseStart);
	SpideyRecordPresentTiming();
	QueryPerformanceCounter(
		&phaseEnd);
	recordTimingUs =
		SpideyTimingElapsedUs(
			&phaseStart,
			&phaseEnd);

	// Modern input Phase 0 is observation-only. Poll once per completed game
	// frame so connection/axis telemetry is available without changing retail
	// action state or controller behavior.
	SpideyInput11PassivePoll(
		frame);
	SpideyCameraPassivePoll(
		frame);

	int shadowPreviewToggled =
		0;
	int shadowPreviewToggledOn =
		0;
	int shadowReferenceDelay =
		0;
	int d3d7SuppressionToggled =
		0;

	if (!gSpideyShadowPreviewModeSynced)
	{
		SpideyRenderer11ShadowSetContinuous(
			gSpideyShadowPreviewEnabled);
		gSpideyShadowPreviewModeSynced =
			1;

		FILE* previewLog = SpideyOpenConsolidatedLog(
		"PRESENT");
		if (previewLog)
		{
			fprintf(
				previewLog,
				"shadow_default frame=%lu enabled=%d mode=dx11_authoritative debug_reference_toggle=disabled\n",
				frame,
				gSpideyShadowPreviewEnabled);
			fclose(previewLog);
		}
	}

	// F9/F10 used to switch between the DX11 preview and a D3D7 reference
	// path. DX11 is now authoritative, so falling back to the legacy display
	// surface is intentionally disabled. Keep the local transition flags at
	// zero for the existing telemetry/presentation bookkeeping.

	// DXPOLY_Flip runs after retail EndScene. Transient texture surfaces are
	// no longer actively bound for drawing here, so this is the safe point
	// to lock/mirror them and then replay the queued retail primitive stream
	// into the completely offscreen DX11 shadow target.
	QueryPerformanceCounter(
		&phaseStart);
	SpideyProcessPendingTransientSurfaces();
	QueryPerformanceCounter(
		&phaseEnd);
	transientUs =
		SpideyTimingElapsedUs(
			&phaseStart,
			&phaseEnd);

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

	if (SpideyUseModernOutputAspect())
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
		QueryPerformanceCounter(
			&phaseStart);
		shadowFrameResult =
			SpideyRenderer11ShadowEndFrame(
				frame,
				shadowWidth,
				shadowHeight);
		QueryPerformanceCounter(
			&phaseEnd);
		shadowEndUs =
			SpideyTimingElapsedUs(
				&phaseStart,
				&phaseEnd);
	}

	if (shadowReferenceDelay)
	{
		// EndFrame replayed the just-completed modern frame while continuous
		// capture was still enabled. Disable it only after that safe replay.
		SpideyRenderer11ShadowSetContinuous(0);
	}

	const int retailMovieBlocksTakeover =
		SpideyRetailMovieBlocksDx11Takeover();

	if (!gSpideyShadowPreviewEnabled ||
		retailMovieBlocksTakeover)
	{
		gSpideyShadowPreviewReady =
			0;
	}
	else if (!shadowPreviewToggledOn &&
		shadowFrameResult)
	{
		// ShadowEndFrame now reports success only for a complete non-empty
		// replay. Retail Bink/DirectDraw movie surfaces remain compatibility
		// producers until the movie ends, so they also block takeover.
		gSpideyShadowPreviewReady =
			1;
	}

	static int lastMovieTakeoverBlock =
		-1;
	if (frame <= 5 ||
		retailMovieBlocksTakeover !=
			lastMovieTakeoverBlock)
	{
		FILE* gateLog = SpideyOpenConsolidatedLog(
		"PRESENT");
		if (gateLog)
		{
			fprintf(
				gateLog,
				"dx11_takeover_gate frame=%lu shadow_result=%d movie_blocks=%d movie=0x%08lX movie_surface=0x%08lX ready=%d deferred=%d\n",
				frame,
				shadowFrameResult,
				retailMovieBlocksTakeover,
				(unsigned long)*(void**)0x00AC0BA4,
				(unsigned long)*(void**)0x00AC0A3C,
				gSpideyShadowPreviewReady,
				gSpideyRenderer11ExclusiveDeferred);
			fclose(gateLog);
		}

		lastMovieTakeoverBlock =
			retailMovieBlocksTakeover;
	}

	if (gSpideyShadowPreviewReady &&
		gSpideyRenderer11ExclusiveDeferred)
	{
		gSpideyRenderer11ExclusiveDeferred =
			0;
		SpideyApplyRendererWindowMode(
			"dx11_authoritative_ready");
	}

	if (frame <= 5 &&
		!shadowFrameResult)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"DRAW");
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

	// Retail D3D7 remains the compatibility/reference producer, while the
	// default visible path replays the same completed primitive stream in DX11.
	// These counters describe the just-completed retail source frame.
	QueryPerformanceCounter(
		&phaseStart);
	SpideyFlushRetailD3D7DrawProbeFrame(
		frame);
	SpideyInstallRetailD3D7DrawProbe();
	QueryPerformanceCounter(
		&phaseEnd);
	drawProbeUs =
		SpideyTimingElapsedUs(
			&phaseStart,
			&phaseEnd);

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
		rectCorrected ||
		shadowPreviewToggled ||
		d3d7SuppressionToggled;

	if (shouldLog)
	{
		FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

	// DX11 is now the sole visible presenter whenever the authoritative
	// replay is ready. This applies equally to windowed, borderless and true
	// DXGI exclusive modes. Retail Flip remains only as a pre-authoritative
	// fallback while the DX11 replay warms up.
	int compatPresentPath =
		0;
	const int dx11Authoritative =
		SpideyDx11AuthoritativeActive();

	int shadowPresentResult =
		0;
	if ((gSpideyShadowPreviewEnabled &&
		 gSpideyShadowPreviewReady) ||
		shadowReferenceDelay)
	{
		QueryPerformanceCounter(
			&phaseStart);
		shadowPresentResult =
			SpideyRenderer11PresentShadow(
				1,
				0);
		QueryPerformanceCounter(
			&phaseEnd);
		presentShadowUs =
			SpideyTimingElapsedUs(
				&phaseStart,
				&phaseEnd);
	}

	if (shadowPresentResult)
	{
		compatPresentPath =
			4;
	}
	else if (dx11Authoritative)
	{
		// Never fall back to a potentially lost DirectDraw display surface
		// after DX11 has taken ownership. Keep the frame alive and make the
		// failure visible in telemetry instead.
		compatPresentPath =
			-4;
	}
	else if (!windowedCompat)
	{
		retailFlip();
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
		FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
		if (f)
		{
			fprintf(
				f,
				"present_path frame=%lu windowed=%d retail_flip=%d dx11=%d dx11_shadow=%d dx11_pixels=%d dx11_hdc=%d direct_hwnd=%d shadow_preview=%d shadow_ready=%d compat_result=%d\n",
				frame,
				windowedCompat,
				(!windowedCompat && !dx11Authoritative &&
				 compatPresentPath == 0) ? 1 : 0,
				(compatPresentPath >= 2 ||
				 compatPresentPath == -4) ? 1 : 0,
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
		FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

	QueryPerformanceCounter(
		&presentWorkEnd);
	const unsigned long presentWorkUs =
		SpideyTimingElapsedUs(
			&presentWorkStart,
			&presentWorkEnd);
	const unsigned long measuredPhaseUs =
		recordTimingUs +
		transientUs +
		shadowEndUs +
		drawProbeUs +
		presentShadowUs;

	gSpideyTimingLastPresentWorkUs =
		presentWorkUs;
	gSpideyTimingLastRecordTimingUs =
		recordTimingUs;
	gSpideyTimingLastTransientUs =
		transientUs;
	gSpideyTimingLastShadowEndUs =
		shadowEndUs;
	gSpideyTimingLastDrawProbeUs =
		drawProbeUs;
	gSpideyTimingLastPresentShadowUs =
		presentShadowUs;
	gSpideyTimingLastOtherPresentUs =
		presentWorkUs >
			measuredPhaseUs ?
			presentWorkUs -
				measuredPhaseUs :
			0;
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

	FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

	FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

typedef int (__cdecl *SpideyRetailNextMovieFrameFn)(void);

// @Ok
static int __cdecl SpideyCompatNextMovieFrame()
{
	// Retail Bink still renders through a lockable DirectDraw surface and
	// blits that surface into g_pDDS_Scene before DXPOLY_Flip. Never let
	// DXGI exclusive ownership invalidate those compatibility surfaces.
	SpideyReleaseRendererExclusiveForCompatRebuild(
		"movie_frame");
	gSpideyRenderer11ExclusiveDeferred =
		1;
	gSpideyShadowPreviewReady =
		0;

	SpideyRetailNextMovieFrameFn retail =
		(SpideyRetailNextMovieFrameFn)0x0050B5A0;
	return retail();
}

// @Ok
static void SpideyInstallMovieFrameCompat()
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	const unsigned long retailNextMovieFrame =
		0x0050B5A0;

	int patched =
		0;

	for (unsigned char* p = textStart;
		 p + 5 <= textEnd;
		 ++p)
	{
		if (p[0] != 0xE8)
			continue;

		const long rel =
			*(long*)(p + 1);
		const unsigned long target =
			(unsigned long)(p + 5 + rel);

		if (target != retailNextMovieFrame)
			continue;

		const long newRel =
			(long)(
				(unsigned char*)&SpideyCompatNextMovieFrame -
				(p + 5));

		*(long*)(p + 1) =
			newRel;
		FlushInstructionCache(
			GetCurrentProcess(),
			p,
			5);
		++patched;
	}

	FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");
	if (f)
	{
		fprintf(
			f,
			"movie_frame_compat patched_calls=%d retail=0x0050B5A0 wrapper=0x%08lX\n",
			patched,
			(unsigned long)&SpideyCompatNextMovieFrame);
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

	FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

	FILE* f = SpideyOpenConsolidatedLog(
		"PRESENT");

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

static unsigned long gSpideyCleanup503AF0LostRecoveries =
	0;
static unsigned long gSpideyCleanup503AF0RestoreFailures =
	0;

static void __cdecl SpideyCompatCleanup503AF0()
{
	LPDIRECTSOUNDBUFFER primary =
		*(LPDIRECTSOUNDBUFFER*)0x006BBF1C;

	if (!primary)
	{
		FILE* f = SpideyOpenConsolidatedLog(
			"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"cleanup_503AF0 skipped null_global=0x006BBF1C\n");
			fclose(f);
		}
		return;
	}

	DWORD status =
		0;
	HRESULT statusHr =
		primary->GetStatus(
			&status);
	const int lost =
		statusHr ==
			DSERR_BUFFERLOST ||
		(SUCCEEDED(statusHr) &&
			(status &
				DSBSTATUS_BUFFERLOST) !=
			0);

	HRESULT restoreHr =
		S_OK;
	unsigned long restoreAttempts =
		0;

	if (lost)
	{
		// Alt-tab can legitimately invalidate the primary DirectSound buffer.
		// Retail DXSOUND_ShutDown calls Stop() and treats DSERR_BUFFERLOST as
		// fatal, eventually reaching CRT _exit. Restore before entering retail
		// so its unchanged Stop/Unload sequence can complete normally.
		do
		{
			++restoreAttempts;
			restoreHr =
				primary->Restore();

			if (restoreHr ==
				DSERR_BUFFERLOST)
			{
				Sleep(10);
			}
		}
		while (restoreHr ==
				DSERR_BUFFERLOST &&
			restoreAttempts <
				8);

		if (SUCCEEDED(restoreHr))
			++gSpideyCleanup503AF0LostRecoveries;
		else
			++gSpideyCleanup503AF0RestoreFailures;

		FILE* f = SpideyOpenConsolidatedLog(
			"COMPAT");
		if (f)
		{
			fprintf(
				f,
				"cleanup_503AF0 buffer_lost status_hr=0x%08lX status=0x%08lX restore_hr=0x%08lX attempts=%lu recoveries=%lu failures=%lu foreground=0x%08lX game_hwnd=0x%08lX\n",
				(unsigned long)statusHr,
				(unsigned long)status,
				(unsigned long)restoreHr,
				restoreAttempts,
				gSpideyCleanup503AF0LostRecoveries,
				gSpideyCleanup503AF0RestoreFailures,
				(unsigned long)GetForegroundWindow(),
				(unsigned long)*(HWND*)0x006B7A60);
			fclose(f);
		}

		// If focus/priority has not returned yet, do not enter retail's fatal
		// Stop()->_exit path. A later cleanup call can retry restoration.
		if (FAILED(restoreHr))
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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
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
	FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");
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

static unsigned long gSpideyBackgroundPumpCalls =
	0;
static unsigned long gSpideyBackgroundPumpMessages =
	0;

// @Ok
static void SpideyPumpBackgroundWindowMessages(
		HWND hwnd)
{
	if (!hwnd)
		return;

	MSG message;
	int pumped =
		0;

	while (pumped < 64 &&
		PeekMessageA(
			&message,
			hwnd,
			0,
			0,
			PM_REMOVE))
	{
		TranslateMessage(
			&message);
		DispatchMessageA(
			&message);
		++pumped;
	}

	++gSpideyBackgroundPumpCalls;
	gSpideyBackgroundPumpMessages +=
		(unsigned long)pumped;

	if (pumped &&
		(gSpideyBackgroundPumpCalls <= 8 ||
		 (gSpideyBackgroundPumpCalls % 300) == 0))
	{
		FILE* f =
			SpideyOpenConsolidatedLog(
				"INPUT");
		if (f)
		{
			fprintf(
				f,
				"background_message_pump calls=%lu pumped=%d total_messages=%lu hwnd=0x%08lX foreground=0x%08lX\n",
				gSpideyBackgroundPumpCalls,
				pumped,
				gSpideyBackgroundPumpMessages,
				(unsigned long)hwnd,
				(unsigned long)GetForegroundWindow());
			fclose(f);
		}
	}
}

// @Ok
static i32 SpideySyncRetailInputForeground(void)
{
	HWND hwnd =
		*(HWND*)0x006B7A60;

	const i32 foreground =
		hwnd &&
		GetForegroundWindow() == hwnd;

	if (foreground == gSpideyRetailInputForeground)
	{
		if (!foreground)
			SpideyPumpBackgroundWindowMessages(
				hwnd);
		return foreground;
	}

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

		SpideyPumpBackgroundWindowMessages(
			hwnd);
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

	const i32 result =
		retail(
			pY,
			pX);

	if (result)
	{
		// Despite the historical local parameter names, retail
		// PCINPUT_UpdateMouse applies the first DXINPUT_PollMouse output to
		// gMouseX and the second output to gMouseY. Preserve the retail call
		// untouched and only mirror those relative deltas for camera telemetry.
		if (pY)
			gSpideyRawMouseDeltaX +=
				*pY;
		if (pX)
			gSpideyRawMouseDeltaY +=
				*pX;
		++gSpideyRawMousePollCount;
	}

	return result;
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

	FILE* f = SpideyOpenConsolidatedLog(
		"INPUT");
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
	FILE* f = SpideyOpenConsolidatedLog(
		"DXERROR");
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
	SpideyInstallAudioDeviceCompat();
	SpideyInstallAudioMenuCompat();
	SpideyInstallModernModeReinitCompat();
	// Claim the Display Options menu's Enter/Apply call before the generic
	// SetDisplayOptions scan rewrites the remaining retail call sites.
	SpideyInstallDisplayAspectCompat();
	SpideyInstallDisplayOptionsCompat();
	SpideyInstallHorPlusCullCompat();
	SpideyInstall2DPolyProvenanceCompat();
	SpideyInstallHighFpsTimingCompat();
	SpideyInstallPlayerPhysics60Compat();
	SpideyInstallTimingTelemetry();
	SpideyInstallPresentProbe();
	SpideyInstallMovieFrameCompat();
	SpideyInstallMoviePresentCompat();
	SpideyInstallMovieStopCompat();
	SpideyInstallRetailInputCompat();
	SpideyInstallModernCameraCompat();
	SpideyInstallModernManualAimCompat();
	SpideyInstallCameraWebTargetingCompat();
	SpideyInstallQuadBitCameraAnchorCompat();
	SpideyInstallMouseCoordinateCompat();
	SpideyInstallFrontendLifecycleCompat();
	SpideyInstallGameplayUiScaleCompat();
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
	patch_ai();
	// Keep the reconstructed player-physics source available for RE, but do
	// not install it globally yet. Retail DoPhysics/crawling/swinging owns the
	// live runtime until the native-60 corrections are reduced to narrow,
	// retail-preserving hooks.

	patch_spool();
	patch_trig();
	patch_pctex();
	patch_dcfileio();
	patch_PCMovie();

	patch_flash();
	patch_pshell();
	patch_FontTools();
	patch_mess();
#ifdef _WIN32
	SpideyInstallFrontendTextScaleCompat();
#endif
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

    FILE* f =
        SpideyOpenConsolidatedLog(
            "CRASH");
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
		{
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

			// Stamp the actually loaded proxy revision into the consolidated
			// runtime log as well as the console title. This makes direct
			// RUN_GAME / Play Current Build sessions self-identifying even
			// when they are launched without the update/test PowerShell wrapper.
			FILE* runtimeVersionLog =
				SpideyOpenConsolidatedLog(
					"RUNTIME");
			if (runtimeVersionLog)
			{
				fprintf(
					runtimeVersionLog,
					"runtime_revision=%s\n",
					RUNTIME_VERSION);
				fclose(runtimeVersionLog);
			}

			runtime_assertions();
			runtime_patches();

            break;
		}

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

		FILE* f = SpideyOpenConsolidatedLog(
		"RUNTIME");

		if (f)
		{
			fprintf(f, "ASSERT: %s\n", message);
			fclose(f);
		}
	}
}
