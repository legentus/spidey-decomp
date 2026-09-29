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


static void SpideyLogRetailAudioState(const char* stage)
{
	FILE* f = fopen("spidey-decomp-audio.log", "a");
	if (!f)
		return;

	void* pDS = 0;
	void* pPrimary = 0;
	i32 loadedBuffers = 0;
	i32 activeVoices = 0;

	__try
	{
		pDS =
			*(void**)0x006B7920;
		pPrimary =
			*(void**)0x006BBF1C;

		void** buffers =
			(void**)0x006BBAD4;
		for (i32 i = 0; i < 0x80; i++)
		{
			if (buffers[i])
				loadedBuffers++;
		}

		unsigned char* holders =
			(unsigned char*)0x006BBD50;
		for (i32 j = 0; j < 0x20; j++)
		{
			if (*(void**)(holders + j * 0x0C))
				activeVoices++;
		}

		fprintf(
			f,
			"audio_state stage=%s pDS=0x%08lX primary=0x%08lX loaded_buffers=%d active_voices=%d\n",
			stage ? stage : "<null>",
			(unsigned long)pDS,
			(unsigned long)pPrimary,
			loadedBuffers,
			activeVoices);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		fprintf(
			f,
			"audio_state stage=%s <unreadable>\n",
			stage ? stage : "<null>");
	}

	fclose(f);
}

typedef void (__cdecl *SpideyRetailDXINITDirectX8Fn)(
		HWND,
		HINSTANCE,
		unsigned long);

static void __cdecl SpideyDiagDXINITDirectX8(
		HWND hwnd,
		HINSTANCE hInstance,
		unsigned long flags)
{
	SpideyRetailDXINITDirectX8Fn fn =
		(SpideyRetailDXINITDirectX8Fn)0x004FDE90;

	fn(hwnd, hInstance, flags);
	SpideyLogRetailAudioState("after_DXINIT_DirectX8");
}

typedef void (__cdecl *SpideyRetailSFXNameFn)(char*);

static void __cdecl SpideyDiagSFXInit(char* name)
{
	SpideyRetailSFXNameFn fn =
		(SpideyRetailSFXNameFn)0x004718B0;

	fn(name);
	SpideyLogRetailAudioState("after_SFX_Init");
}

static void __cdecl SpideyDiagSFXSpoolInLevel(char* name)
{
	SpideyRetailSFXNameFn fn =
		(SpideyRetailSFXNameFn)0x004719B0;

	fn(name);
	SpideyLogRetailAudioState("after_SFX_SpoolInLevelSFX");
}

static int SpideyRedirectDirectCalls(
		unsigned long oldTarget,
		void* newTarget,
		const char* label)
{
	unsigned char* textStart =
		(unsigned char*)0x00401000;
	unsigned char* textEnd =
		(unsigned char*)0x0053B000;
	i32 count = 0;

	for (unsigned char* p = textStart;
		 p + 5 <= textEnd;
		 ++p)
	{
		if (p[0] != 0xE8)
			continue;

		long oldRel =
			*(long*)(p + 1);

		unsigned long target =
			(unsigned long)(p + 5 + oldRel);

		if (target != oldTarget)
			continue;

		long newRel =
			(long)((unsigned char*)newTarget - (p + 5));

		*(long*)(p + 1) =
			newRel;
		count++;
	}

	FlushInstructionCache(
		GetCurrentProcess(),
		textStart,
		textEnd - textStart);

	FILE* f = fopen("spidey-decomp-audio.log", "a");
	if (f)
	{
		fprintf(
			f,
			"audio_hook label=%s old_target=0x%08lX replacement=0x%08lX direct_calls=%d\n",
			label ? label : "<null>",
			oldTarget,
			(unsigned long)newTarget,
			count);
		fclose(f);
	}

	return count;
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

	// Change RealWinMain's third DXINIT_DirectX8 argument from 2 to 3.
	// DXINIT_DirectX8 stores:
	//     gDxOptionRelated = a3 & 1;
	// so this preserves bit 1 while enabling the game's own windowed
	// DirectDraw initialization path.
	matchedPush[1] =
		0x03;

	long dxInitWrapperRel =
		(long)(
			(unsigned char*)SpideyDiagDXINITDirectX8 -
			(matchedCall + 5));
	*(long*)(matchedCall + 1) =
		dxInitWrapperRel;

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
			(unsigned long)SpideyDiagDXINITDirectX8);
		fclose(f);
	}

	printf(
		"[*] Windowed DirectDraw compatibility patch: DXINIT_DirectX8 arg 2->3 at 0x%08lX\n",
		(unsigned long)matchedPush);
}
#endif

#ifdef _WIN32

struct SpideyXInputGamepad
{
	WORD wButtons;
	BYTE bLeftTrigger;
	BYTE bRightTrigger;
	SHORT sThumbLX;
	SHORT sThumbLY;
	SHORT sThumbRX;
	SHORT sThumbRY;
};

struct SpideyXInputState
{
	DWORD dwPacketNumber;
	SpideyXInputGamepad Gamepad;
};

struct SpideyXInputVibration
{
	WORD wLeftMotorSpeed;
	WORD wRightMotorSpeed;
};

typedef DWORD (WINAPI *SpideyXInputGetStateFn)(
		DWORD,
		SpideyXInputState*);
typedef DWORD (WINAPI *SpideyXInputSetStateFn)(
		DWORD,
		SpideyXInputVibration*);

static HMODULE gSpideyXInputModule = 0;
static SpideyXInputGetStateFn gSpideyXInputGetState = 0;
static SpideyXInputSetStateFn gSpideyXInputSetState = 0;
static DWORD gSpideyXInputUser = 0xFFFFFFFF;
static unsigned char gSpideyXInputButtons[32];
static unsigned char gSpideyXInputWasDown[32];
static WORD gSpideyXInputRumble = 0;
static i32 gSpideyXInputLoggedConnected = 0;

static const WORD SPIDEY_XINPUT_DPAD_UP = 0x0001;
static const WORD SPIDEY_XINPUT_DPAD_DOWN = 0x0002;
static const WORD SPIDEY_XINPUT_DPAD_LEFT = 0x0004;
static const WORD SPIDEY_XINPUT_DPAD_RIGHT = 0x0008;
static const WORD SPIDEY_XINPUT_START = 0x0010;
static const WORD SPIDEY_XINPUT_BACK = 0x0020;
static const WORD SPIDEY_XINPUT_LEFT_THUMB = 0x0040;
static const WORD SPIDEY_XINPUT_RIGHT_THUMB = 0x0080;
static const WORD SPIDEY_XINPUT_LEFT_SHOULDER = 0x0100;
static const WORD SPIDEY_XINPUT_RIGHT_SHOULDER = 0x0200;
static const WORD SPIDEY_XINPUT_A = 0x1000;
static const WORD SPIDEY_XINPUT_B = 0x2000;
static const WORD SPIDEY_XINPUT_X = 0x4000;
static const WORD SPIDEY_XINPUT_Y = 0x8000;

static void SpideyXInputLog(const char* message)
{
	FILE* f = fopen("spidey-decomp-controller.log", "a");
	if (!f)
		return;
	fprintf(f, "%s\n", message ? message : "<null>");
	fclose(f);
}

static int SpideyXInputLoad()
{
	if (gSpideyXInputGetState)
		return 1;

	const char* dlls[] =
	{
		"xinput1_4.dll",
		"xinput1_3.dll",
		"xinput9_1_0.dll"
	};

	for (i32 i = 0; i < 3; i++)
	{
		HMODULE module =
			LoadLibraryA(dlls[i]);
		if (!module)
			continue;

		SpideyXInputGetStateFn getState =
			(SpideyXInputGetStateFn)GetProcAddress(
				module,
				"XInputGetState");

		if (!getState)
		{
			FreeLibrary(module);
			continue;
		}

		gSpideyXInputModule =
			module;
		gSpideyXInputGetState =
			getState;
		gSpideyXInputSetState =
			(SpideyXInputSetStateFn)GetProcAddress(
				module,
				"XInputSetState");

		FILE* f = fopen(
			"spidey-decomp-controller.log",
			"a");
		if (f)
		{
			fprintf(
				f,
				"xinput loaded dll=%s rumble=%d\n",
				dlls[i],
				gSpideyXInputSetState ? 1 : 0);
			fclose(f);
		}
		return 1;
	}

	SpideyXInputLog("xinput unavailable");
	return 0;
}

static int SpideyXInputFindController()
{
	if (!SpideyXInputLoad())
		return 0;

	SpideyXInputState state;
	memset(&state, 0, sizeof(state));

	for (DWORD i = 0; i < 4; i++)
	{
		if (gSpideyXInputGetState(i, &state) == 0)
		{
			gSpideyXInputUser =
				i;
			if (!gSpideyXInputLoggedConnected)
			{
				FILE* f = fopen(
					"spidey-decomp-controller.log",
					"a");
				if (f)
				{
					fprintf(
						f,
						"xinput controller connected user=%lu\n",
						(unsigned long)i);
					fclose(f);
				}
				gSpideyXInputLoggedConnected = 1;
			}
			return 1;
		}
	}

	gSpideyXInputUser =
		0xFFFFFFFF;
	return 0;
}

static int SpideyScaleXInputAxis(
		SHORT value,
		i32 deadzone)
{
	i32 v =
		(i32)value;

	if (v > -deadzone &&
		v < deadzone)
	{
		return 0;
	}

	if (v > 0)
	{
		i32 scaled =
			(v - deadzone) * 1000 /
			(32767 - deadzone);
		if (scaled > 1000)
			scaled = 1000;
		return scaled;
	}

	i32 magnitude =
		-v;
	i32 scaled =
		(magnitude - deadzone) * 1000 /
		(32768 - deadzone);
	if (scaled > 1000)
		scaled = 1000;
	return -scaled;
}

static void SpideyXInputSetButton(
		i32 index,
		i32 down)
{
	if (index < 0 || index >= 32)
		return;

	if (down)
	{
		gSpideyXInputButtons[index] =
			gSpideyXInputWasDown[index] ?
			0x7F :
			0xFF;
	}
	else
	{
		gSpideyXInputButtons[index] =
			gSpideyXInputWasDown[index] ?
			0x80 :
			0x00;
	}

	gSpideyXInputWasDown[index] =
		down ? 1 : 0;
}

static int SpideyXInputPov(WORD buttons)
{
	i32 up =
		(buttons & SPIDEY_XINPUT_DPAD_UP) != 0;
	i32 down =
		(buttons & SPIDEY_XINPUT_DPAD_DOWN) != 0;
	i32 left =
		(buttons & SPIDEY_XINPUT_DPAD_LEFT) != 0;
	i32 right =
		(buttons & SPIDEY_XINPUT_DPAD_RIGHT) != 0;

	if (up && right)
		return 4500;
	if (right && down)
		return 13500;
	if (down && left)
		return 22500;
	if (left && up)
		return 31500;
	if (up)
		return 0;
	if (right)
		return 9000;
	if (down)
		return 18000;
	if (left)
		return 27000;
	return -1;
}

// Button indices intentionally preserve Spider-Man's retail default mapping:
// 0=X, 1=A, 3=B, 4=Y, 6=LB, 7=RT, 9=RB, 11=Menu.
static i32 __cdecl SpideyXInputSetupController()
{
	memset(
		gSpideyXInputButtons,
		0,
		sizeof(gSpideyXInputButtons));
	memset(
		gSpideyXInputWasDown,
		0,
		sizeof(gSpideyXInputWasDown));

	return SpideyXInputFindController();
}

static i32 __cdecl SpideyXInputPollController(
		i32* x,
		i32* y,
		i32* pov)
{
	if (!gSpideyXInputGetState ||
		gSpideyXInputUser == 0xFFFFFFFF)
	{
		if (!SpideyXInputFindController())
			return 0;
	}

	SpideyXInputState state;
	memset(&state, 0, sizeof(state));

	if (gSpideyXInputGetState(
			gSpideyXInputUser,
			&state) != 0)
	{
		gSpideyXInputUser =
			0xFFFFFFFF;
		return 0;
	}

	const WORD buttons =
		state.Gamepad.wButtons;

	SpideyXInputSetButton(0, (buttons & SPIDEY_XINPUT_X) != 0);
	SpideyXInputSetButton(1, (buttons & SPIDEY_XINPUT_A) != 0);
	SpideyXInputSetButton(2, (buttons & SPIDEY_XINPUT_BACK) != 0);
	SpideyXInputSetButton(3, (buttons & SPIDEY_XINPUT_B) != 0);
	SpideyXInputSetButton(4, (buttons & SPIDEY_XINPUT_Y) != 0);
	SpideyXInputSetButton(5, state.Gamepad.bLeftTrigger > 30);
	SpideyXInputSetButton(6, (buttons & SPIDEY_XINPUT_LEFT_SHOULDER) != 0);
	SpideyXInputSetButton(7, state.Gamepad.bRightTrigger > 30);
	SpideyXInputSetButton(8, (buttons & SPIDEY_XINPUT_LEFT_THUMB) != 0);
	SpideyXInputSetButton(9, (buttons & SPIDEY_XINPUT_RIGHT_SHOULDER) != 0);
	SpideyXInputSetButton(10, (buttons & SPIDEY_XINPUT_RIGHT_THUMB) != 0);
	SpideyXInputSetButton(11, (buttons & SPIDEY_XINPUT_START) != 0);
	SpideyXInputSetButton(12, (buttons & SPIDEY_XINPUT_DPAD_UP) != 0);
	SpideyXInputSetButton(13, (buttons & SPIDEY_XINPUT_DPAD_DOWN) != 0);
	SpideyXInputSetButton(14, (buttons & SPIDEY_XINPUT_DPAD_LEFT) != 0);
	SpideyXInputSetButton(15, (buttons & SPIDEY_XINPUT_DPAD_RIGHT) != 0);

	if (x)
		*x =
			SpideyScaleXInputAxis(
				state.Gamepad.sThumbLX,
				7849);
	if (y)
		*y =
			-SpideyScaleXInputAxis(
				state.Gamepad.sThumbLY,
				7849);
	if (pov)
		*pov =
			SpideyXInputPov(buttons);

	return 1;
}

static u8 __cdecl SpideyXInputGetControllerButtonState(u8 index)
{
	if (index >= 32)
		return 0;
	return gSpideyXInputButtons[index];
}

static i32 __cdecl SpideyXInputGetNumControllerButtons()
{
	return 16;
}

static i32 __cdecl SpideyXInputSetupForceFeedback(
		i32 magnitude,
		float)
{
	if (magnitude < 0)
		magnitude = -magnitude;
	if (magnitude > 10000)
		magnitude = 10000;

	gSpideyXInputRumble =
		(WORD)(
			(unsigned long)magnitude *
			65535UL /
			10000UL);
	return gSpideyXInputSetState != 0;
}

static i32 __cdecl SpideyXInputStartForceFeedback()
{
	if (!gSpideyXInputSetState ||
		gSpideyXInputUser == 0xFFFFFFFF)
		return 0;

	SpideyXInputVibration vibration;
	vibration.wLeftMotorSpeed =
		gSpideyXInputRumble;
	vibration.wRightMotorSpeed =
		gSpideyXInputRumble;

	return gSpideyXInputSetState(
		gSpideyXInputUser,
		&vibration) == 0;
}

static i32 __cdecl SpideyXInputStopForceFeedback()
{
	if (!gSpideyXInputSetState ||
		gSpideyXInputUser == 0xFFFFFFFF)
		return 0;

	SpideyXInputVibration vibration;
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;

	return gSpideyXInputSetState(
		gSpideyXInputUser,
		&vibration) == 0;
}

static const char* SpideyXboxButtonName(i32 button)
{
	switch (button)
	{
		case 0: return "X";
		case 1: return "A";
		case 2: return "View";
		case 3: return "B";
		case 4: return "Y";
		case 5: return "LT";
		case 6: return "LB";
		case 7: return "RT";
		case 8: return "LS";
		case 9: return "RB";
		case 10: return "RS";
		case 11: return "Menu";
		case 12: return "D-Pad Up";
		case 13: return "D-Pad Down";
		case 14: return "D-Pad Left";
		case 15: return "D-Pad Right";
	}
	return 0;
}

static i32 __cdecl SpideyXboxFormatButtonName(
		char* dest,
		const char* format,
		i32 button)
{
	const char* name =
		SpideyXboxButtonName(button);

	if (dest && name)
	{
		strcpy(dest, name);
		return strlen(dest);
	}

	return sprintf(
		dest,
		format,
		button);
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

	SpideyRedirectDirectCalls(
		0x004718B0,
		(void*)SpideyDiagSFXInit,
		"SFX_Init");
	SpideyRedirectDirectCalls(
		0x004719B0,
		(void*)SpideyDiagSFXSpoolInLevel,
		"SFX_SpoolInLevelSFX");

	PATCH_PUSH_RET(0x004FC240, SpideyDiagDisplayDIError);
	PATCH_PUSH_RET(0x004FC630, SpideyDiagDisplayDSError);
	PATCH_PUSH_RET(0x004FC820, SpideyDiagDisplayD3DError);

	PATCH_PUSH_RET(0x00501890, SpideyXInputSetupController);
	PATCH_PUSH_RET(0x00501E50, SpideyXInputPollController);
	PATCH_PUSH_RET(0x00501FB0, SpideyXInputGetControllerButtonState);
	PATCH_PUSH_RET(0x00502210, SpideyXInputGetNumControllerButtons);
	PATCH_PUSH_RET(0x00501FC0, SpideyXInputSetupForceFeedback);
	PATCH_PUSH_RET(0x005021A0, SpideyXInputStartForceFeedback);
	PATCH_PUSH_RET(0x005021E0, SpideyXInputStopForceFeedback);

	// Retail initActionMaps: replace only the sprintf("button %i") call.
	PATCH_CALL(0x0050D28C, SpideyXboxFormatButtonName);
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
