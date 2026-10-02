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

static int gSpideyGameplayUiScalePercent =
	kSpideyDefaultGameplayUiScalePercent;
static int gSpideyPendingGameplayUiScalePercent =
	kSpideyDefaultGameplayUiScalePercent;
static int gSpideyMenuTextScalePercent =
	kSpideyDefaultMenuTextScalePercent;
static int gSpideyPendingMenuTextScalePercent =
	kSpideyDefaultMenuTextScalePercent;

static char gSpideyGameplayUiScaleMenuLabel[64] =
	"Gameplay UI Scale: 125%";
static char gSpideyMenuTextScaleMenuLabel[64] =
	"Menu/Text Scale: 100%";
static char gSpideyPauseGameplayUiScaleMenuLabel[64] =
	"UI Scale [=====-----] 125%";
static char gSpideyPauseMenuTextScaleMenuLabel[64] =
	"Menu Text [===-------] 100%";
static char gSpideyPauseApplyUiScaleLabel[] =
	"Apply UI Scale";
static int gSpideyInLevelDisplayMenuActive = 0;

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

	const int slots =
		10;
	const int range =
		kSpideyUiScaleMaxPercent -
		kSpideyUiScaleMinPercent;
	int filled =
		0;

	if (range > 0)
	{
		filled =
			((percent -
			  kSpideyUiScaleMinPercent) *
			 slots +
			 range / 2) /
			range;
	}

	if (filled < 0)
		filled =
			0;
	if (filled > slots)
		filled =
			slots;

	char bar[11];
	int i;
	for (i = 0;
		 i < slots;
		 ++i)
	{
		bar[i] =
			i < filled ?
				'=' :
				'-';
	}
	bar[slots] =
		0;

	sprintf(
		destination,
		"%s [%s] %d%%",
		prefix,
		bar,
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
		"Menu Text",
		gSpideyPendingMenuTextScalePercent);
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

	gSpideyPendingWindowMode =
		gSpideyWindowMode;
	gSpideyPendingAspectMode =
		gSpideyAspectMode;
	gSpideyPendingGameplayUiScalePercent =
		gSpideyGameplayUiScalePercent;
	gSpideyPendingMenuTextScalePercent =
		gSpideyMenuTextScalePercent;

	SpideyUpdateDisplayModeMenuLabel();
	SpideyUpdateUiScaleMenuLabels();

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"ui_scale_settings load gameplay_percent=%d text_percent=%d range=%d-%d step=%d config=%s\n",
			gSpideyGameplayUiScalePercent,
			gSpideyMenuTextScalePercent,
			kSpideyUiScaleMinPercent,
			kSpideyUiScaleMaxPercent,
			kSpideyUiScaleStepPercent,
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

// @Ok
static void SpideyPauseCommitUiScale()
{
	const int oldGameplay =
		gSpideyGameplayUiScalePercent;
	const int oldText =
		gSpideyMenuTextScalePercent;

	gSpideyGameplayUiScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingGameplayUiScalePercent);
	gSpideyMenuTextScalePercent =
		SpideyClampUiScalePercent(
			gSpideyPendingMenuTextScalePercent);

	SpideySaveModernVideoSettings();
	SpideyUpdateUiScaleMenuLabels();

	FILE* f =
		SpideyOpenConsolidatedLog(
			"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"pause_ui_apply old_gameplay=%d new_gameplay=%d old_text=%d new_text=%d saved=1 live_policy=next_draw_no_device_rebuild\n",
			oldGameplay,
			gSpideyGameplayUiScalePercent,
			oldText,
			gSpideyMenuTextScalePercent);
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

	if (menu &&
		!SpideyPauseMenuHasEntry(
			menu,
			gSpideyPauseApplyUiScaleLabel))
	{
		if (menu->mNumLines <= 37)
		{
			SpideyRetailMenuAddEntryFn retailAdd =
				(SpideyRetailMenuAddEntryFn)0x0043FFF0;

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
				gSpideyPauseApplyUiScaleLabel);

			// Three extra rows: preserve approximately the same visual center.
			menu->mY -=
				(menu->mLineSep * 3) / 2;

			FILE* f =
				SpideyOpenConsolidatedLog(
					"COMPAT");
			if (f)
			{
				fprintf(
					f,
					"pause_ui_controls rows_added=3 rows=%u y=%d line_sep=%d gameplay=%d text=%d retail_display_options_disabled=1 graphical_slider_resources=0 controls=left_right_apply\n",
					(unsigned int)menu->mNumLines,
					menu->mY,
					menu->mLineSep,
					gSpideyPendingGameplayUiScalePercent,
					gSpideyPendingMenuTextScalePercent);
				fclose(f);
			}
		}
	}

	RetailUpdateFn retailUpdate =
		(RetailUpdateFn)0x00440600;
	retailUpdate(
		menu,
		0);

	if (!menu ||
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
				"pause_ui_adjust kind=%s line=%u percent=%d pending_gameplay=%d pending_text=%d\n",
				kind,
				(unsigned int)menu->mLine,
				*percent,
				gSpideyPendingGameplayUiScalePercent,
				gSpideyPendingMenuTextScalePercent);
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

	RetailCheckTriggersFn retail =
		(RetailCheckTriggersFn)0x0050C180;
	const u8 triggered =
		retail(
			mask,
			option2,
			option3);

	if (!triggered)
		return triggered;

	CMenu* menu =
		*(CMenu**)0x005FAED0;
	if (!menu ||
		menu->mLine >= menu->mNumLines ||
		!menu->mEntry[menu->mLine].name)
	{
		return triggered;
	}

	const char* selected =
		menu->mEntry[menu->mLine].name;

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
				"pause_ui_confirm action=apply line=%u rows=%u mask=0x%08lX gameplay=%d text=%d\n",
				(unsigned int)menu->mLine,
				(unsigned int)menu->mNumLines,
				(unsigned long)mask,
				gSpideyGameplayUiScalePercent,
				gSpideyMenuTextScalePercent);
			fclose(f);
		}
		return 0;
	}

	if (!strcmp(
			selected,
			gSpideyPauseGameplayUiScaleMenuLabel) ||
		!strcmp(
			selected,
			gSpideyPauseMenuTextScaleMenuLabel))
	{
		// Synthetic pause rows are adjusted with left/right or the slider.
		// Consume confirm so retail does not dispatch an unknown string.
		return 0;
	}

	return triggered;
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
			"display_menu_mod retail=0x0050D9B0 rows=7 row1=Aspect_Ratio row3=Gameplay_UI_Scale row4=Menu_Text_Scale row5=Display_Mode row6=Apply label=%d resfmt=%d aspectfmt=%d aspectprev=%d aspectnext=%d compatnext=%d compatprev=%d resprev=%d resnext=%d applyentry=%d applyconfirm=%d modeupdate=%d scaledraw=%d pause_inline=1 pause_text_sliders=1 pause_display_hook=0 pause_update=%d pause_confirm=%d range=%d-%d step=%d defaults=%d,%d\n",
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
				"gameplay_ui_alignment source=compass_qpoly seq=%lu policy=bottom_right_compact logical=%lux%lu density=%.6f,%.6f before=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f after=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
				gSpideyCompassQPolyProbeSamples % 3,
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

	const int cartridgeTextInstalled =
		SpideyPatchDirectCall(
			0x00465A83,
			0x00458700,
			(void*)&SpideyCompatCartridgeCountText,
			"cartridge_count_text");

	const unsigned long compassQPolySites[] =
	{
		0x00463D19,
		0x00464035,
		0x00464257
	};
	int compassQPolyCalls =
		0;
	int compassQPolyIndex;
	for (compassQPolyIndex = 0;
		 compassQPolyIndex <
			(int)(sizeof(compassQPolySites) /
			 sizeof(compassQPolySites[0]));
		 ++compassQPolyIndex)
	{
		compassQPolyCalls +=
			SpideyPatchDirectCall(
				compassQPolySites[compassQPolyIndex],
				0x00507910,
				(void*)&SpideyCompatCompassQPoly2D,
				"compass_qpoly");
	}

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
			"gameplay_ui_scale_install frame_target=0x00462C30 frame_calls=%d texture_target=0x00462CD0 texture_calls=%d cartridge_text=%d compass_qpoly=%d health_qpoly=%d,%d,%d health_flat=%d,%d panel_qpoly=%d panel_gouraud=%d panel_flat=%d reference=512x240 baseline_output=640x480 policy=compact_holders_compass_cartridge_gouraud_flat_panel_qpoly_passthrough user_percent=%d\n",
			frameCalls,
			textureCalls,
			cartridgeTextInstalled,
			compassQPolyCalls,
			healthQPolyOne,
			healthQPolyTwo,
			healthQPolyThree,
			healthFlatOne,
			healthFlatTwo,
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

	// 0x0056F3B8 is the retail active-camera pointer used by
	// CPlayer::PutCameraBehind. Read only; Phase 0 camera work must not
	// mutate retail camera state.
	CCamera* camera =
		*(CCamera**)0x0056F3B8;

	if (!camera)
	{
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
		(frame - gSpideyCameraTelemetryLastIntentFrame >= 15);
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

	FILE* f = SpideyOpenConsolidatedLog(
		"COMPAT");
	if (f)
	{
		fprintf(
			f,
			"renderer11_release_exclusive_for_compat reason=%s selected=%lux%lu had_exclusive=%d release_result=%d primary_lost=0x%08lX primary_restore=0x%08lX scene_lost=0x%08lX scene_restore=0x%08lX\n",
			reason ? reason : "unknown",
			gSpideySelectedOutputWidth,
			gSpideySelectedOutputHeight,
			hadExclusive,
			releaseResult,
			(unsigned long)primaryLost,
			(unsigned long)primaryRestore,
			(unsigned long)sceneLost,
			(unsigned long)sceneRestore);
		fclose(f);
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

	FILE* f = SpideyOpenConsolidatedLog(
		"TEXTURE");
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

	FILE* f = SpideyOpenConsolidatedLog(
		"TEXTURE");
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


typedef void (__cdecl *SpideyRetailLogicFn)(void);

static unsigned long gSpideyTimingLogicWindowStart = 0;
static unsigned long gSpideyTimingLogicTicks = 0;
static unsigned long gSpideyTimingPresentWindowStart = 0;
static unsigned long gSpideyTimingPresentFrames = 0;

static void SpideyLogTimingWindow(
		const char* kind,
		unsigned long elapsed,
		unsigned long count)
{
	if (!kind ||
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
	fclose(f);
}

static void __cdecl SpideyCompatLogicTiming()
{
	SpideyRetailLogicFn retail =
		(SpideyRetailLogicFn)0x00455400;

	retail();

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
		SpideyLogTimingWindow(
			"logic",
			elapsed,
			gSpideyTimingLogicTicks);
		gSpideyTimingLogicWindowStart =
			now;
		gSpideyTimingLogicTicks =
			0;
	}
}

static void SpideyRecordPresentTiming()
{
	SpideyTryRebindBinkAudio(
		"frame_safe_point");

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
	}
}

static void SpideyInstallTimingTelemetry()
{
	const int logicInstalled =
		SpideyPatchDirectCall(
			0x00455A8B,
			0x00455400,
			SpideyCompatLogicTiming,
			"gameplay_logic_timing");

	FILE* f = SpideyOpenConsolidatedLog(
		"TIMING");
	if (f)
	{
		fprintf(
			f,
			"timing_install logic=%d call=0x00455A8B retail=0x00455400 engine_vblanks=0x006B4CA0\n",
			logicInstalled);
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
		(frame % 120) == 0 ||
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
	const unsigned long frame =
		++gSpideyPresentFrame;
	SpideyRecordPresentTiming();

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

	if ((frame <= 5 ||
		 (frame % 120) == 0) &&
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

	if (((gSpideyShadowPreviewEnabled &&
		  gSpideyShadowPreviewReady) ||
		 shadowReferenceDelay) &&
		SpideyRenderer11PresentShadow(
			1,
			0))
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

static void __cdecl SpideyCompatCleanup503AF0()
{
	void* object =
		*(void**)0x006BBF1C;

	if (!object)
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
	SpideyInstallTimingTelemetry();
	SpideyInstallPresentProbe();
	SpideyInstallMovieFrameCompat();
	SpideyInstallMoviePresentCompat();
	SpideyInstallMovieStopCompat();
	SpideyInstallRetailInputCompat();
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

		FILE* f = SpideyOpenConsolidatedLog(
		"RUNTIME");

		if (f)
		{
			fprintf(f, "ASSERT: %s\n", message);
			fclose(f);
		}
	}
}
