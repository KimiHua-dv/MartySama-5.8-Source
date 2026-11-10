// PythonBackground.cpp: implementation of the CPythonBackground class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../eterlib/CullingManager.h"
#include "../eterlib/Camera.h"
#include "../eterPack/EterPackManager.h"
#include "../gamelib/MapOutDoor.h"
#include "../gamelib/PropertyLoader.h"

#include "PythonBackground.h"
#include "PythonCharacterManager.h"
#include "PythonNetworkStream.h"
#include "PythonMiniMap.h"
#include "PythonSystem.h"
#if defined(ANK_SYSTEM_GRAPHICS)
#include <fstream>
#include <sstream>
#include <string>
#include "../SpeedTreeLib/VertexShaders.h"
#include <random>
#endif

std::string g_strEffectName = "d:/ymir work/effect/etc/direction/direction_land.mse";

#if defined(ANK_SYSTEM_GRAPHICS)
std::chrono::steady_clock::time_point lastExecutionTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point lastExecutionTimeDog;
std::chrono::steady_clock::time_point lastExecutionTimeWolf;
std::chrono::steady_clock::time_point lastExecutionTimeBirds;
std::chrono::steady_clock::time_point lastExecutionTimeBar;
#endif

DWORD CPythonBackground::GetRenderShadowTime()
{
	return m_dwRenderShadowTime;
}

bool CPythonBackground::SetVisiblePart(int eMapOutDoorPart, bool isVisible)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SetVisiblePart(eMapOutDoorPart, isVisible);
	return true;
}

void CPythonBackground::EnableTerrainOnlyForHeight()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.EnableTerrainOnlyForHeight(TRUE);
}

bool CPythonBackground::SetSplatLimit(int iSplatNum)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.SetSplatLimit(iSplatNum);
	return true;
}

void CPythonBackground::CreateCharacterShadowTexture()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.CreateCharacterShadowTexture();
}

void CPythonBackground::ReleaseCharacterShadowTexture()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.ReleaseCharacterShadowTexture();
}

#if defined(ANK_SYSTEM_GRAPHICS)
std::mt19937 rng;
#endif

#if defined(ANK_SYSTEM_GRAPHICS)
void CPythonBackground::RefreshShadowLevel()
{
	SetGraphicsLevel(CPythonSystem::Instance().GetGraphicsLevel());
	if (CPythonSystem::Instance().GetGraphicsLevel() == 0)
	{
		SetShadowQualityLevel(0);
		SetTerrainShadow(0);
		SetTreeShadow(0);
		SetSpecularLevel(0);
		SetBloodLevel(0);
	}
	else
	{
		SetShadowQualityLevel(CPythonSystem::Instance().GetShadowQualityLevel());
		SetTerrainShadow(CPythonSystem::Instance().GetTerrainShadow());
		SetTreeShadow(CPythonSystem::Instance().GetTreeShadow());
		SetRainLevel(CPythonSystem::Instance().GetRainLevel());
		SetSpecularLevel(CPythonSystem::Instance().GetSpecularLevel());
		SetBloodLevel(CPythonSystem::Instance().GetBloodLevel());
		SetAmbianceEffectsLevel(CPythonSystem::Instance().GetAmbianceEffectsLevel());
		CPythonSystem::Instance().GetLEAVES();
	}

	SetGrassType(CPythonSystem::Instance().GetGrassType());
	CPythonSystem::Instance().SetSharpnessPower(CPythonSystem::Instance().GetSharpnessPower());
}

void CPythonBackground::RefreshGraphicsLevel()
{
	SetGraphicsLevel(CPythonSystem::Instance().GetGraphicsLevel());
}


bool CPythonBackground::SetGraphicsLevel(int eLevel)
{
	if (!m_pkMap)
		return false;

	if (m_eGraphicsLevel == eLevel)
		return true;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eGraphicsLevel = eLevel;

	switch (m_eGraphicsLevel)
	{
	case GRAPHICS_OFF:
		Frustum::Instance().graphicsLevel = 0;
		Frustum::Instance().graphicsWaterLevel = 0;
		Frustum::Instance().shadowRender = 0;
		Frustum::Instance().terrainDepthLevel = 0;
		Frustum::Instance().increaseShadowRangeFrustum = 1.0f;
		SetShadowQualityLevel(0);
		SetTreeShadow(0);
		SetBloodLevel(0);
		break;

	case GRAPHICS_MEDIUM:
		Frustum::Instance().graphicsLevel = 2;
		Frustum::Instance().graphicsWaterLevel = 0;
		Frustum::Instance().shadowRender = 1;
		Frustum::Instance().increaseShadowRangeFrustum = 1.0f;
		Frustum::Instance().terrainDepthLevel = 1;
		SetShadowQualityLevel(1);
		SetTreeShadow(1);
		SetBloodLevel(1);
		break;

	case GRAPHICS_ULTRA:
		Frustum::Instance().graphicsLevel = 2;
		Frustum::Instance().graphicsWaterLevel = 2;
		Frustum::Instance().shadowRender = 2;
		Frustum::Instance().increaseShadowRangeFrustum = 10.0f;
		Frustum::Instance().terrainDepthLevel = 1;
		SetShadowQualityLevel(3);
		SetTreeShadow(3);
		SetBloodLevel(1);
		break;
	}

	return true;
}

#if defined(ANK_SYSTEM_GRAPHICS)
ID3DXBuffer* pShaderBuffer;
ID3DXBuffer* pShaderBuffer2;
ID3DXBuffer* pShaderBufferShadowBuilding;
ID3DXBuffer* pErrorBuffer;
ID3DXBuffer* pErrorBuffer2;
const char* PS_RedColor;
const char* PS_RedColor2;
const char* PS_RedColorShadowBuilding;
int numberofloads = 0;
#endif

bool CPythonBackground::SetShadowQualityLevel(int eLevel)
{
#if defined(ANK_SYSTEM_GRAPHICS)
	DWORD dwShaderStart = GetTickCount();
	if (numberofloads == 0)
	{
		// Embed the HLSL shader code as a local C++ multi-line string.
		const char* PS_RedColor =
			"sampler2D sampler0 : register(s0);"
			"sampler2D sampler1 : register(s1);"
			"sampler2D sampler2 : register(s2);"
			"float4 PS_RedColor(float2 texCoord0 : TEXCOORD0, float2 texCoord1 : TEXCOORD1, float2 texCoord2 : TEXCOORD2) : COLOR"
			"{"
			"   float2 center = float2(0.5, 0.5);"
			"   float2 size = float2(0.159689, 0.159689);"
			"   float2 pixelSize = 10.0 / float2(3840.0, 2160.0);"
			"   float4 tex0 = tex2D(sampler0, texCoord0);"
			"   float4 tex1 = float4(0.0, 0.0, 0.0, 0.0);"
			"   for(int dx = -2; dx <= 2; dx++) {"
			"       for(int dy = -2; dy <= 2; dy++) {"
			"           tex1 += tex2D(sampler1, texCoord1 + float2(dx, dy) * pixelSize);"
			"       }"
			"   }"
			"   tex1 /= 31.0;"
			"   float4 tex2 = tex2D(sampler2, texCoord2);"
			"   float4 result = tex0 * tex1 * 6.0;"
			"   result.a = tex0.a * 3;"
			"   return result;"
			"}";

		// Now use this string for your shader compilation
		D3DXCompileShader(PS_RedColor, strlen(PS_RedColor), NULL, NULL, "PS_RedColor", "ps_2_0", 0, &pShaderBuffer, NULL, NULL);
		DWORD dwCompileTime = GetTickCount() - dwShaderStart;
		DWORD dwCreateShaderStart = GetTickCount();
		ms_lpd3dDevice->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pShader);
		DWORD dwCreateShaderTime = GetTickCount() - dwCreateShaderStart;
		pShaderBuffer->Release();

		numberofloads = 1;
	}
#endif



	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eShadowQualityLevel = eLevel;

	switch (eLevel)
	{
	case SHADOW_QUALITY_BAD:
		rkMap.SetShadowTextureSize(1024);
		rkMap.CreateCharacterShadowTexture();
		break;
	case SHADOW_QUALITY_AVERAGE:
		rkMap.SetShadowTextureSize(1500);
		rkMap.CreateCharacterShadowTexture();
		break;
	case SHADOW_QUALITY_GOOD:
		rkMap.SetShadowTextureSize(1750);
		rkMap.CreateCharacterShadowTexture();
		break;
	case SHADOW_QUALITY_ULTRA:
		rkMap.SetShadowTextureSize(2048);
		rkMap.CreateCharacterShadowTexture();
		break;
	}
	return true;
}

bool CPythonBackground::SetTerrainShadow(int eLevel)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eTerrainShadow = eLevel;

	switch (eLevel)
	{
	case TERRAIN_SHADOW_off:
		break;
	case TERRAIN_SHADOW_on:
		break;
	}

	return true;
}

bool CPythonBackground::SetSpecularLevel(int eLevel)
{

	m_eSpecularLevel = eLevel;

	switch (m_eSpecularLevel)
	{
	case SPECULAR_off:
		Frustum::Instance().realtimeSpecular = 0;
		break;

	case SPECULAR_on:
		Frustum::Instance().realtimeSpecular = 1;
		break;
	}

	return true;
}

bool CPythonBackground::SetAmbianceEffectsLevel(int eLevel)
{

	m_eAmbianceEffectsLevel = eLevel;

	switch (m_eAmbianceEffectsLevel)
	{
	case AEFFECTS_off:
		Frustum::Instance().ambianceEffectsLevel = 0;
		break;

	case AEFFECTS_on:
		Frustum::Instance().ambianceEffectsLevel = 1;
		break;
	}

	return true;
}

bool CPythonBackground::SetBloodLevel(int eLevel)
{

	m_eBloodLevel = eLevel;

	switch (m_eBloodLevel)
	{
	case BLOOD_off:
		Frustum::Instance().bloodLevel = 0;
		break;

	case BLOOD_on:
		Frustum::Instance().bloodLevel = 1;
		break;
	}

	return true;
}

bool CPythonBackground::SetTreeShadow(int eLevel)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eTreeShadow = eLevel;

	switch (eLevel)
	{
	case TREE_SHADOW_on:
		Frustum::Instance().treeShadow = 0;
		break;

	case TREE_SHADOW_off:
		Frustum::Instance().treeShadow = 1;
		break;

	case TREE_SHADOW_high:
		Frustum::Instance().treeShadow = 2;
		break;

	case TREE_SHADOW_all:
		Frustum::Instance().treeShadow = 3;
		break;

	}

	return true;
}

bool CPythonBackground::SetGrassType(int eLevel)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eGrassType = eLevel;

	switch (eLevel)
	{
	case GRASS_off:
		Frustum::Instance().realtimeGrass = 0;
		break;

	case GRASS_on:
		Frustum::Instance().realtimeGrass = 1;
		break;
	}

	return true;
}


bool CPythonBackground::SetRainLevel(int eLevel)
{
	if (!m_pkMap)
		return false;

	if (m_eRainLevel == eLevel)
		return true;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eRainLevel = eLevel;

	switch (m_eRainLevel)
	{
	case RAIN_0:
		DisableSnowEnvironment();

		Frustum::Instance().selectRainFrustun = 0;
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(0.5f);
		break;

	case RAIN_1:
		EnableSnowEnvironment();
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(0.6f);
		Frustum::Instance().selectRainFrustun = 8;
		break;

	case RAIN_2:
		EnableSnowEnvironment();
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(0.7f);
		Frustum::Instance().selectRainFrustun = 16;
		break;

	case RAIN_3:
		EnableSnowEnvironment();
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(0.8f);
		Frustum::Instance().selectRainFrustun = 25;
		break;

	case RAIN_4:
		break;
	}


	return true;
}
#else
void CPythonBackground::RefreshShadowLevel()
{
	SetShadowLevel(CPythonSystem::Instance().GetShadowLevel());
}
#endif

bool CPythonBackground::SetShadowLevel(int eLevel)
{
	if (!m_pkMap)
		return false;

	if (m_eShadowLevel == eLevel)
		return true;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eShadowLevel = eLevel;

	switch (m_eShadowLevel)
	{
		case SHADOW_NONE:
			rkMap.SetDrawShadow(false);
			rkMap.SetShadowTextureSize(512);
			break;

		case SHADOW_GROUND:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(false);
#if defined(ANK_SYSTEM_GRAPHICS)
			rkMap.SetShadowTextureSize(2048);
#else
			rkMap.SetShadowTextureSize(512);
#endif
			break;

		case SHADOW_GROUND_AND_SOLO:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
#if defined(ANK_SYSTEM_GRAPHICS)
			rkMap.SetShadowTextureSize(2048);
#else
			rkMap.SetShadowTextureSize(512);
#endif
			break;

		case SHADOW_ALL:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
#if defined(ANK_SYSTEM_GRAPHICS)
			rkMap.SetShadowTextureSize(2048);
#else
			rkMap.SetShadowTextureSize(512);
#endif
			break;

		case SHADOW_ALL_HIGH:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
#if defined(ANK_SYSTEM_GRAPHICS)
			rkMap.SetShadowTextureSize(2048);
#else
			rkMap.SetShadowTextureSize(1024);
#endif
			break;

		case SHADOW_ALL_MAX:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
			rkMap.SetShadowTextureSize(2048);
			break;
	}

	return true;
}

#if defined(ANK_SYSTEM_GRAPHICS)
std::string readShaderFromFile(const char* filename) {
	std::ifstream shaderFile(filename);
	if (!shaderFile.is_open()) {
		return "";
	}

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();

	return shaderStream.str();
}
#endif

void CPythonBackground::SelectViewDistanceNum(int eNum)
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	if (!mc_pcurEnvironmentData)
	{
		TraceError("CPythonBackground::SelectViewDistanceNum(int eNum=%d) mc_pcurEnvironmentData is NULL", eNum);
		return;
	}

	m_eViewDistanceNum = eNum;

	TEnvironmentData * env = ((TEnvironmentData *) mc_pcurEnvironmentData);

	if (env->bReserve)
	{
		env->m_fFogNearDistance = m_ViewDistanceSet[m_eViewDistanceNum].m_fFogStart;
		env->m_fFogFarDistance = m_ViewDistanceSet[m_eViewDistanceNum].m_fFogEnd;
		env->v3SkyBoxScale = m_ViewDistanceSet[m_eViewDistanceNum].m_v3SkyBoxScale;
		rkMap.SetEnvironmentSkyBox();
	}
}

void CPythonBackground::SetViewDistanceSet(int eNum, float fFarClip)
{
	if (!m_pkMap)
		return;

	m_ViewDistanceSet[eNum].m_fFogStart		= fFarClip * 0.5f;//0.3333333f;
	m_ViewDistanceSet[eNum].m_fFogEnd		= fFarClip * 0.7f;//0.6666667f;

	float fSkyBoxScale						= fFarClip * 0.6f;//0.5773502f;
	m_ViewDistanceSet[eNum].m_v3SkyBoxScale	= D3DXVECTOR3(fSkyBoxScale, fSkyBoxScale, fSkyBoxScale);
	m_ViewDistanceSet[eNum].m_fFarClip		= fFarClip;

	if (eNum == m_eViewDistanceNum)
		SelectViewDistanceNum(eNum);
}

float CPythonBackground::GetFarClip()
{
	if (!m_pkMap)
		return 50000.0f;

	if (m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip==0.0f)
	{
		TraceError("CPythonBackground::GetFarClip m_eViewDistanceNum=%d", m_eViewDistanceNum);
		m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip=25600.0f;
	}

	return m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip;
}

void CPythonBackground::GetDistanceSetInfo(int * peNum, float * pfStart, float * pfEnd, float * pfFarClip)
{
	if (!m_pkMap)
	{
		*peNum = 4;
		*pfStart= 10000.0f;
		*pfEnd= 15000.0f;
		*pfFarClip = 50000.0f;
		return;
	}
	*peNum = m_eViewDistanceNum;
	*pfStart = m_ViewDistanceSet[m_eViewDistanceNum].m_fFogStart;
	*pfEnd= m_ViewDistanceSet[m_eViewDistanceNum].m_fFogEnd;
	*pfFarClip = m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPythonBackground::CPythonBackground()
{
	m_dwRenderShadowTime=0;
	m_eViewDistanceNum=0;
	m_eViewDistanceNum=0;
	m_eViewDistanceNum=0;
	m_eShadowLevel=SHADOW_NONE;
#if defined(ANK_SYSTEM_GRAPHICS)
	m_eAmbianceEffectsLevel = AEFFECTS_off;
#endif
#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
	m_bXMasShowEvent = false;
	m_bNightMode = true;
	m_bSnowMode = true;
	m_bSnowTextureMode = true;
#endif
	m_dwBaseX=0;
	m_dwBaseY=0;
	m_strMapName="";
	m_iDayMode = DAY_MODE_LIGHT;
	m_iXMasTreeGrade = 0;
	m_bVisibleGuildArea = FALSE;

	SetViewDistanceSet(4, 25600.0f);
	SetViewDistanceSet(3, 25600.0f);
	SetViewDistanceSet(2, 25600.0f);
	SetViewDistanceSet(1, 25600.0f);
	SetViewDistanceSet(0, 25600.0f);
	Initialize();
}

CPythonBackground::~CPythonBackground()
{
	Tracen("CPythonBackground Clear");
}

#define ENABLE_ATLASINFO_FROM_ROOT
void CPythonBackground::Initialize()
{
#ifdef ENABLE_ATLASINFO_FROM_ROOT
	std::string stAtlasInfoFileName("AtlasInfo.txt");
#else
	std::string stAtlasInfoFileName (LocaleService_GetLocalePath());
	stAtlasInfoFileName += "/AtlasInfo.txt";
#endif
	SetAtlasInfoFileName(stAtlasInfoFileName.c_str());
	CMapManager::Initialize();
}

void CPythonBackground::__CreateProperty()
{
	if (CEterPackManager::SEARCH_FILE_FIRST == CEterPackManager::Instance().GetSearchMode() &&
		_access("property", 0) == 0)
	{
		m_PropertyManager.Initialize(NULL);

		CPropertyLoader PropertyLoader;
		PropertyLoader.SetPropertyManager(&m_PropertyManager);
		PropertyLoader.Create("*.*", "Property");
	}
	else
	{
		m_PropertyManager.Initialize("pack/property");
	}
}

//////////////////////////////////////////////////////////////////////
// Normal Functions
//////////////////////////////////////////////////////////////////////

bool CPythonBackground::GetPickingPoint(D3DXVECTOR3 * v3IntersectPt)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetPickingPoint(v3IntersectPt);
}

bool CPythonBackground::GetPickingPointWithRay(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetPickingPointWithRay(rRay, v3IntersectPt);
}

bool CPythonBackground::GetPickingPointWithRayOnlyTerrain(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetPickingPointWithRayOnlyTerrain(rRay, v3IntersectPt);
}

BOOL CPythonBackground::GetLightDirection(D3DXVECTOR3 & rv3LightDirection)
{
	if (!mc_pcurEnvironmentData)
		return FALSE;

	rv3LightDirection.x = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.x;
	rv3LightDirection.y = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.y;
	rv3LightDirection.z = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.z;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CPythonBackground::Destroy()
{
	CMapManager::Destroy();
	m_SnowEnvironment.Destroy();
	m_bVisibleGuildArea = FALSE;
}

void CPythonBackground::Create()
{
	static int s_isCreateProperty=false;

	if (!s_isCreateProperty)
	{
		s_isCreateProperty=true;
		__CreateProperty();
	}

	CMapManager::Create();

	m_SnowEnvironment.Create();
}

struct FGetPortalID
{
	float m_fRequestX, m_fRequestY;
	std::set<int> m_kSet_iPortalID;
	FGetPortalID(float fRequestX, float fRequestY)
	{
		m_fRequestX=fRequestX;
		m_fRequestY=fRequestY;
	}
	void operator () (CGraphicObjectInstance * pObject)
	{
		for (int i = 0; i < PORTAL_ID_MAX_NUM; ++i)
		{
			int iID = pObject->GetPortal(i);
			if (0 == iID)
				break;

			m_kSet_iPortalID.insert(iID);
		}
	}
};

void CPythonBackground::Update(float fCenterX, float fCenterY, float fCenterZ)
{
	if (!IsMapReady())
		return;
#ifdef __PERFORMANCE_CHECKER__
	DWORD t1=ELTimer_GetMSec();
#endif
	UpdateMap(fCenterX, fCenterY, fCenterZ);
#ifdef __PERFORMANCE_CHECKER__
	DWORD t2=ELTimer_GetMSec();
#endif
	UpdateAroundAmbience(fCenterX, fCenterY, fCenterZ);
#ifdef __PERFORMANCE_CHECKER__
	DWORD t3=ELTimer_GetMSec();
#endif
	m_SnowEnvironment.Update(D3DXVECTOR3(fCenterX, -fCenterY, fCenterZ));

#ifdef __PERFORMANCE_CHECKER__
	{
		static FILE* fp=fopen("perf_bg_update.txt", "w");
		if (t3-t1>5)
		{
			fprintf(fp, "BG.Total %d (Time %f)\n", t3-t1, ELTimer_GetMSec()/1000.0f);
			fprintf(fp, "BG.UpdateMap %d\n", t2-t1);
			fprintf(fp, "BG.UpdateAmb %d\n", t3-t2);
			fflush(fp);
		}
	}
#endif

	// Portal Process
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	if (rkMap.IsEnablePortal())
	{
		CCullingManager & rkCullingMgr = CCullingManager::Instance();
		FGetPortalID kGetPortalID(fCenterX, -fCenterY);

		Vector3d aVector3d;
		aVector3d.Set(fCenterX, -fCenterY, fCenterZ);

		Vector3d toTop;
		toTop.Set(0, 0, 25000.0f);

		rkCullingMgr.ForInRay(aVector3d, toTop, &kGetPortalID);

		std::set<int>::iterator itor = kGetPortalID.m_kSet_iPortalID.begin();
		if (!__IsSame(kGetPortalID.m_kSet_iPortalID, m_kSet_iShowingPortalID))
		{
			ClearPortal();
			std::set<int>::iterator itor=kGetPortalID.m_kSet_iPortalID.begin();
			for (; itor!=kGetPortalID.m_kSet_iPortalID.end(); ++itor)
			{
				AddShowingPortalID(*itor);
			}
			RefreshPortal();

			m_kSet_iShowingPortalID = kGetPortalID.m_kSet_iPortalID;
		}
	}

	// Target Effect Process
	{
		std::map<DWORD, DWORD>::iterator itor = m_kMap_dwTargetID_dwChrID.begin();
		for (; itor != m_kMap_dwTargetID_dwChrID.end(); ++itor)
		{
			DWORD dwTargetID = itor->first;
			DWORD dwChrID = itor->second;

			CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwChrID);

			if (!pInstance)
				continue;

			TPixelPosition kPixelPosition;
			pInstance->NEW_GetPixelPosition(&kPixelPosition);

			CreateSpecialEffect(dwTargetID,
								+kPixelPosition.x,
								-kPixelPosition.y,
								+kPixelPosition.z,
								g_strEffectName.c_str());
		}
	}

	// Reserve Target Effect
	{
		std::map<DWORD, SReserveTargetEffect>::iterator itor = m_kMap_dwID_kReserveTargetEffect.begin();
		for (; itor != m_kMap_dwID_kReserveTargetEffect.end();)
		{
			DWORD dwID = itor->first;
			SReserveTargetEffect & rReserveTargetEffect = itor->second;

			float ilx = float(rReserveTargetEffect.ilx);
			float ily = float(rReserveTargetEffect.ily);

			float fHeight = rkMap.GetHeight(ilx, ily);
			if (0.0f == fHeight)
			{
				++itor;
				continue;
			}

			CreateSpecialEffect(dwID, ilx, ily, fHeight, g_strEffectName.c_str());

			itor = m_kMap_dwID_kReserveTargetEffect.erase(itor);
		}
	}
}

bool CPythonBackground::__IsSame(std::set<int> & rleft, std::set<int> & rright)
{
	std::set<int>::iterator itor_l;
	std::set<int>::iterator itor_r;

	for (itor_l=rleft.begin(); itor_l!=rleft.end(); ++itor_l)
	{
		if (rright.end() == rright.find(*itor_l))
			return false;
	}

	for (itor_r=rright.begin(); itor_r!=rright.end(); ++itor_r)
	{
		if (rleft.end() == rleft.find(*itor_r))
			return false;
	}

	return true;
}

void CPythonBackground::Render()
{
	if (!IsMapReady())
		return;

	m_SnowEnvironment.Deform();

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.Render();
	if (m_bVisibleGuildArea)
		rkMap.RenderMarkedArea();
}

void CPythonBackground::RenderSnow()
{
	m_SnowEnvironment.Render();
}

void CPythonBackground::RenderPCBlocker()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderPCBlocker();
}

void CPythonBackground::RenderCollision()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderCollision();
}

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_UNDERWATER_CAMERA)
void CPythonBackground::GetUnderwaterCamera()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (!rkMap.IsWaterVisible())
		return;

	CCamera* camera = CCameraManager::Instance().GetCurrentCamera();
	D3DXVECTOR3 cameraEye = camera->GetEye();


	bool underwaterSound = false;
	bool surfaceSound = false;


	if ((cameraEye.z < Frustum::Instance().waterLevel - 5.0f) && Frustum::Instance().checkUnderwaterPosition > 0.5f)
	{
		Frustum::Instance().isUnderwater = 1.0f;
		underwaterSound = true;
		surfaceSound = false;
	}
	else
	{
		Frustum::Instance().isUnderwater = 0.0f;
		underwaterSound = false;
		surfaceSound = true;
	}

	static bool isFirstTime = true;

	if (surfaceSound && !Frustum::Instance().isPlayingSurfaceSound)
	{
		if (!isFirstTime)
		{
			CSoundManager::Instance().StopAllSound3D();
			CSoundManager::Instance().PlayMusic3D(2, cameraEye.x, cameraEye.y, cameraEye.z, "data/sound/waterOut.mp3");
			CSoundManager::Instance().PlayMusic3D(2, cameraEye.x, cameraEye.y, cameraEye.z, "data/sound/waterOut.mp3");
			Frustum::Instance().underwaterCameraTransition = 5.0f;
		}

		Frustum::Instance().isPlayingSurfaceSound = true;
		Frustum::Instance().isPlayingUnderwaterSound = false; // Reset the underwater sound flag
		isFirstTime = false;
	}

	if (underwaterSound && !Frustum::Instance().isPlayingUnderwaterSound)
	{
		CSoundManager::Instance().PlayMusic3D(2, cameraEye.x, cameraEye.y, cameraEye.z, "data/sound/underwater.mp3");
		CSoundManager::Instance().PlayMusic3D(2, cameraEye.x, cameraEye.y, cameraEye.z, "data/sound/underwater.mp3");
		CSoundManager::Instance().PlayMusic3D(2, cameraEye.x, cameraEye.y, cameraEye.z, "data/sound/waterSubmerge.mp3");
		CSoundManager::Instance().PlayMusic3D(2, cameraEye.x, cameraEye.y, cameraEye.z, "data/sound/waterSubmerge.mp3");
		Frustum::Instance().isPlayingUnderwaterSound = true;
		Frustum::Instance().isPlayingSurfaceSound = false; // Reset the surface sound flag
	}


}
#endif

#if defined(ANK_SYSTEM_GRAPHICS)
void CPythonBackground::RenderShadow()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().graphicsLevel > 0)
	{
		if (Frustum::Instance().treeShadow >= 2)
		{

			rkMap.__RenderTerrain_RenderHardwareTransformPatch2();
			rkMap.RenderArea2();
		}
	}
}
#endif

#if defined(ANK_SYSTEM_GRAPHICS)
void CPythonBackground::RenderCharacterShadowToTexture()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	rkMap.SkyUtil();
	CSpeedTreeForestDirectX8::Instance().Render3(Forest_RenderAll);
}

void CPythonBackground::RenderCharacterShadowToTexture2()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().shadowType != 3)
	{
		if (Frustum::Instance().isIndoor == true)
		{
			bool canRender = rkMap.BeginRenderCharacterShadowToTexture2();
			if (canRender)
			{

				rkMap.RenderDungeon();
			}
			rkMap.EndRenderCharacterShadowToTexture2();
		}
	}

}

void CPythonBackground::RenderWaterReflection()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (!rkMap.IsWaterVisible())
		return;

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_UNDERWATER_CAMERA)
	CInstanceBase instance;
	instance.GetUnderwaterCameraPosition();
#endif

	rkMap.SetWaterReflectionRenderTarget();
	CCamera* camera = CCameraManager::Instance().GetCurrentCamera();
	D3DXVECTOR3 cameraEye = camera->GetEye();
	D3DXVECTOR3 cameraTarget = camera->GetTarget();
	D3DXVECTOR3 cameraUp = camera->GetUp();

	static float closestWaterHeight = 0.0f;
	float minDistance = 1e10f;

	float playerX = Frustum::Instance().characterPosition.x;
	float playerY = Frustum::Instance().characterPosition.y;
	float playerZ = Frustum::Instance().characterPosition.z;

	auto& waterPoints = Frustum::Instance().waterPoints;

	// Find the closest water point
	for (const auto& point : waterPoints) {
		float waterX, waterY, waterZ;
		std::tie(waterX, waterY, waterZ) = point;

		float distance = std::sqrt((waterX - playerX) * (waterX - playerX) +
			(waterY - playerY) * (waterY - playerY) +
			(waterZ - playerZ) * (waterZ - playerZ));
		if (distance < minDistance) {
			minDistance = distance;
			closestWaterHeight = waterZ;
		}
	}

	Frustum::Instance().waterLevel = closestWaterHeight;

	D3DXVECTOR3 reflectionEye = D3DXVECTOR3(cameraEye.x, cameraEye.y, Frustum::Instance().waterLevel * 2.0f - cameraEye.z);
	D3DXVECTOR3 reflectionTarget = D3DXVECTOR3(cameraTarget.x, cameraTarget.y, Frustum::Instance().waterLevel * 2.0f - cameraTarget.z);
	D3DXVECTOR3 reflectionUp = D3DXVECTOR3(cameraUp.x, cameraUp.y, -cameraUp.z);

	camera->SetViewParams(reflectionEye, reflectionTarget, reflectionUp);

	D3DXMATRIX matLightView, matLightProj;
	STATEMANAGER.SaveTransform(D3DTS_VIEW, &(camera->GetViewMatrix()));
	STATEMANAGER.EnableClipPlane(0, 0.0, 0.0, 1.0, -Frustum::Instance().waterLevel);
	STATEMANAGER.GetTransform(D3DTS_PROJECTION, &matLightProj);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix2(cameraEye, camera->GetViewMatrix(), matLightProj);

	{
		RenderSky();
		RenderCloud();
		CSpeedTreeForestDirectX8::Instance().RenderReflection();
		rkMap.RenderArea();
		rkMap.RenderTerrainReflect();

		SetCharacterDirLight();
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		rkChrMgr.Render();
	}

	{
		STATEMANAGER.PopRenderTarget();
		camera->SetViewParams(cameraEye, cameraTarget, cameraUp);
		STATEMANAGER.RestoreTransform(D3DTS_VIEW);
		STATEMANAGER.DisableClipPlane(0);
	}
}

void CPythonBackground::RenderPuddleReflection()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	rkMap.SetPuddleReflectionRenderTarget();
	CCamera* camera = CCameraManager::Instance().GetCurrentCamera();
	D3DXVECTOR3 cameraEye = camera->GetEye();
	D3DXVECTOR3 cameraTarget = camera->GetTarget();
	D3DXVECTOR3 cameraUp = camera->GetUp();

	Frustum::Instance().puddleLevel = Frustum::Instance().characterPosition.z;

	D3DXVECTOR3 reflectionEye = D3DXVECTOR3(cameraEye.x, cameraEye.y, Frustum::Instance().puddleLevel * 2.0f - cameraEye.z);
	D3DXVECTOR3 reflectionTarget = D3DXVECTOR3(cameraTarget.x, cameraTarget.y, Frustum::Instance().puddleLevel * 2.0f - cameraTarget.z);
	D3DXVECTOR3 reflectionUp = D3DXVECTOR3(cameraUp.x, cameraUp.y, -cameraUp.z);

	camera->SetViewParams(reflectionEye, reflectionTarget, reflectionUp);

	D3DXMATRIX matLightView, matLightProj;
	STATEMANAGER.SaveTransform(D3DTS_VIEW, &(camera->GetViewMatrix()));
	STATEMANAGER.EnableClipPlane(0, 0.0, 0.0, 1.0, -Frustum::Instance().puddleLevel);
	STATEMANAGER.GetTransform(D3DTS_PROJECTION, &matLightProj);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix2(cameraEye, camera->GetViewMatrix(), matLightProj);

	{
		RenderSky();
		RenderCloud();
		CSpeedTreeForestDirectX8::Instance().RenderReflection();
		rkMap.RenderArea();

		SetCharacterDirLight();
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		rkChrMgr.Render();
	}

	{
		STATEMANAGER.PopRenderTarget();
		camera->SetViewParams(cameraEye, cameraTarget, cameraUp);
		STATEMANAGER.RestoreTransform(D3DTS_VIEW);
		STATEMANAGER.DisableClipPlane(0);
	}
}


void CPythonBackground::RenderCharacterShadowToTexture3()
{


	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	if (Frustum::Instance().isIndoor)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().shadowType != 3)
	{
		bool canRender = rkMap.BeginRenderCharacterShadowToTexture3();
		if (canRender)
		{
			CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

			if (Frustum::Instance().treeShadow == 0)
			{
				rkChrMgr.RenderShadowMainInstance();
			}

			if (Frustum::Instance().treeShadow >= 1)
			{
				rkChrMgr.RenderShadowAllInstances();
				RenderShadow(); //buildings
			}

			if (Frustum::Instance().treeShadow == 3)
			{
				if (Frustum::Instance().shadowRender == 2)
				{
					ms_lpd3dDevice->SetPixelShader(rkMap.m_lpShadowMapPixelShaderDepthTree);
					CSpeedTreeForestDirectX8::Instance().Render2(Forest_RenderAll); //tree to shadowmap
				}
			}
		}
		rkMap.EndRenderCharacterShadowToTexture3();
	}

}

void CPythonBackground::RenderGrass()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.RenderGrass();
}

void CPythonBackground::RenderCharacterShadowToTexture4()
{


	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	if (Frustum::Instance().isIndoor)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().shadowType != 3)
	{
		if (Frustum::Instance().shadowType == 2)
		{
			bool canRender = rkMap.BeginRenderCharacterShadowToTexture4();
			if (canRender)
			{

				CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

				if (Frustum::Instance().treeShadow == 0)
				{
					rkChrMgr.RenderShadowMainInstance();
				}

				if (Frustum::Instance().treeShadow >= 1)
				{
					rkChrMgr.RenderShadowAllInstances();
					RenderShadow(); //buildings
				}

				if (Frustum::Instance().treeShadow == 3)
				{
					if (Frustum::Instance().shadowRender == 2)
					{
						ms_lpd3dDevice->SetPixelShader(rkMap.m_lpShadowMapPixelShaderDepthTree);
						CSpeedTreeForestDirectX8::Instance().Render2(Forest_RenderAll); 
					}
				}

				//rkMap.RenderTree(Forest_RenderToShadow);
			}
			rkMap.EndRenderCharacterShadowToTexture4();
		}
		else
		{
			if (Frustum::Instance().treeShadow == 3)
			{
				bool canRender = rkMap.BeginRenderCharacterShadowToTexture4();
				if (canRender)
				{
					if (Frustum::Instance().shadowRender == 2)
					{
						CSpeedTreeForestDirectX8::Instance().Render3(Forest_RenderAll); //tree to shadowmap
					}
				}
				rkMap.EndRenderCharacterShadowToTexture4();
			}
		}
	}


}
#else
void CPythonBackground::RenderCharacterShadowToTexture()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	DWORD t1=ELTimer_GetMSec();

	if (m_eShadowLevel == SHADOW_ALL ||
		m_eShadowLevel == SHADOW_ALL_HIGH ||
		m_eShadowLevel == SHADOW_ALL_MAX ||
		m_eShadowLevel == SHADOW_GROUND_AND_SOLO)
	{
		D3DXMATRIX matWorld;
		STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);

		bool canRender=rkMap.BeginRenderCharacterShadowToTexture();
		if (canRender)
		{
			CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();

			if (m_eShadowLevel == SHADOW_GROUND_AND_SOLO)
				rkChrMgr.RenderShadowMainInstance();
			else
				rkChrMgr.RenderShadowAllInstances();
		}
		rkMap.EndRenderCharacterShadowToTexture();

		STATEMANAGER.SetTransform(D3DTS_WORLD, &matWorld);
	}

	DWORD t2=ELTimer_GetMSec();

	m_dwRenderShadowTime=t2-t1;
}
#endif

inline float Interpolate(float fStart, float fEnd, float fPercent)
{
	return fStart + (fEnd - fStart) * fPercent;
}
struct CollisionChecker
{
	bool isBlocked;
	CInstanceBase* pInstance;
	CollisionChecker(CInstanceBase* pInstance) : pInstance(pInstance), isBlocked(false) {}
	void operator () (CGraphicObjectInstance* pOpponent)
	{
		if (isBlocked)
			return;

		if (!pOpponent)
			return;

		if (pInstance->IsBlockObject(*pOpponent))
			isBlocked=true;
	}
};

struct CollisionAdjustChecker
{
	bool isBlocked;
	CInstanceBase* pInstance;
	CollisionAdjustChecker(CInstanceBase* pInstance) : pInstance(pInstance), isBlocked(false) {}
	void operator () (CGraphicObjectInstance* pOpponent)
	{
		if (!pOpponent)
			return;

		if (pInstance->AvoidObject(*pOpponent))
			isBlocked=true;
	}
};

bool CPythonBackground::CheckAdvancing(CInstanceBase * pInstance)
{
	if (!IsMapReady())
		return true;

	Vector3d center;
	float radius;
	pInstance->GetGraphicThingInstanceRef().GetBoundingSphere(center,radius);

	CCullingManager & rkCullingMgr = CCullingManager::Instance();

	CollisionAdjustChecker kCollisionAdjustChecker(pInstance);
	rkCullingMgr.ForInRange(center, radius, &kCollisionAdjustChecker);
	if (kCollisionAdjustChecker.isBlocked)
	{
		CollisionChecker kCollisionChecker(pInstance);
		rkCullingMgr.ForInRange(center, radius, &kCollisionChecker);
		if (kCollisionChecker.isBlocked)
		{
			pInstance->BlockMovement();
			return true;
		}
		else
		{
			pInstance->NEW_MoveToDestPixelPositionDirection(pInstance->NEW_GetDstPixelPositionRef());
		}
		return false;
	}
	return false;
}

void CPythonBackground::RenderSky()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderSky();
}

void CPythonBackground::RenderCloud()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderCloud();
}

void CPythonBackground::RenderWater()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderWater();
}

void CPythonBackground::RenderEffect()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderEffect();
}

void CPythonBackground::RenderBeforeLensFlare()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderBeforeLensFlare();
}

void CPythonBackground::RenderAfterLensFlare()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderAfterLensFlare();
}

void CPythonBackground::ClearGuildArea()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.ClearGuildArea();
}

void CPythonBackground::RegisterGuildArea(int isx, int isy, int iex, int iey)
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RegisterGuildArea(isx, isy, iex, iey);
}

#if defined(ANK_SYSTEM_GRAPHICS)
void CPythonBackground::SetCharacterDirLight()
{
	if (!IsMapReady())
		return;

	if (!mc_pcurEnvironmentData)
		return;

	if (Frustum::Instance().shadowType == 3)
	{
		D3DLIGHT9 Light;
		Light.Type = D3DLIGHT_DIRECTIONAL;
		Light.Position = D3DXVECTOR3(50.0f, 150.0f, 350.0f);
		Light.Direction = D3DXVECTOR3(-0.15f, -0.3f, -0.9f);
		Light.Diffuse.r = 10.0f;
		Light.Diffuse.g = 10.0f;
		Light.Diffuse.b = 10.0f;
		Light.Diffuse.a = 10.0f;
		Light.Ambient.r = 10.0f;
		Light.Ambient.g = 10.0f;
		Light.Ambient.b = 10.0f;
		Light.Ambient.a = 10.0f;

		STATEMANAGER.SetLight(0, &Light);
	}
}
#else
void CPythonBackground::SetCharacterDirLight()
{
	if (!IsMapReady())
		return;

	if (!mc_pcurEnvironmentData)
		return;

	STATEMANAGER.SetLight(0, &mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_CHARACTER]);
}
#endif

void CPythonBackground::SetBackgroundDirLight()
{
	if (!IsMapReady())
		return;
	if (!mc_pcurEnvironmentData)
		return;

	STATEMANAGER.SetLight(0, &mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND]);
}

void CPythonBackground::GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY)
{
	rGlobalX-=m_dwBaseX;
	rGlobalY-=m_dwBaseY;
}

void CPythonBackground::LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY)
{
	rLocalX+=m_dwBaseX;
	rLocalY+=m_dwBaseY;
}

void CPythonBackground::RegisterDungeonMapName(const char * c_szMapName)
{
	m_kSet_strDungeonMapName.insert(c_szMapName);
}

CPythonBackground::TMapInfo* CPythonBackground::GlobalPositionToMapInfo(DWORD dwGlobalX, DWORD dwGlobalY)
{
	TMapInfoVector::iterator f = std::find_if(m_kVct_kMapInfo.begin(), m_kVct_kMapInfo.end(), FFindWarpMapName(dwGlobalX, dwGlobalY));
	if (f == m_kVct_kMapInfo.end())
		return NULL;

	return &(*f);
}

#if defined(ANK_SYSTEM_GRAPHICS)
bool map1isloaded = false;
#endif

void CPythonBackground::Warp(DWORD dwX, DWORD dwY)
{
	DWORD dwTotalStart = GetTickCount();

	DWORD dwStart = GetTickCount();
	TMapInfo* pkMapInfo = GlobalPositionToMapInfo(dwX, dwY);
	DWORD dwMapInfoTime = GetTickCount() - dwStart;

	if (!pkMapInfo)
	{
		TraceError("NOT_FOUND_GLOBAL_POSITION(%d, %d)", dwX, dwY);
		return;
	}

	dwStart = GetTickCount();
	RefreshShadowLevel();
	DWORD dwShadowTime = GetTickCount() - dwStart;

	TMapInfo& rMapInfo = *pkMapInfo;

	assert((dwX >= rMapInfo.m_dwBaseX) && (dwY >= rMapInfo.m_dwBaseY));

#if defined(ANK_SYSTEM_GRAPHICS)
	Frustum::Instance().grassTexturesInitialized = false;
	Frustum::Instance().grassMatrices.clear();
	Frustum::Instance().grassShadowMatrices.clear();
#endif

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_UNDERWATER_CAMERA)
	Frustum::Instance().waterPoints.clear();
#endif

	dwStart = GetTickCount();

	if (!LoadMap(
		rMapInfo.m_strName,
		float(dwX - rMapInfo.m_dwBaseX),
		float(dwY - rMapInfo.m_dwBaseY),
		0))
	{
		// LOAD_MAP_ERROR_HANDLING
		PostQuitMessage(0);
		// END_OF_LOAD_MAP_ERROR_HANDLING
		return;
	}

	DWORD dwLoadMapTime = GetTickCount() - dwStart;

#if defined(ANK_SYSTEM_GRAPHICS)
	dwStart = GetTickCount();

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.CreateShadowmapShaders();

	DWORD dwShaderTime = GetTickCount() - dwStart;
#else
	DWORD dwShaderTime = 0;
#endif

	dwStart = GetTickCount();

	CPythonMiniMap::Instance().LoadAtlas();

	DWORD dwAtlasTime = GetTickCount() - dwStart;

	m_dwBaseX = rMapInfo.m_dwBaseX;
	m_dwBaseY = rMapInfo.m_dwBaseY;

	m_strMapName = rMapInfo.m_strName;

	SetXMaxTree(m_iXMasTreeGrade);

	if (m_kSet_strDungeonMapName.end() != m_kSet_strDungeonMapName.find(m_strMapName))
	{
		EnableTerrainOnlyForHeight();

		CMapOutdoor& rkMap = GetMapOutdoorRef();
		rkMap.EnablePortal(TRUE);
	}

	m_kSet_iShowingPortalID.clear();
	m_kMap_dwTargetID_dwChrID.clear();
	m_kMap_dwID_kReserveTargetEffect.clear();

#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
	if (IsSnowMap(m_strMapName))
	{
		if (GetXMasShowEvent())
		{
			if (GetSnowModeOption())
				EnableSnowEnvironment();

			SetSnowTexture(GetSnowTextureModeOption());
		}
	}
	else
	{
		DisableSnowEnvironment();
	}

	if (GetDayMode() != DAY_MODE_LIGHT && GetNightModeOption() && IsBoomMap(m_strMapName))
	{
		RegisterEnvironmentData(DAY_MODE_DARK, "d:/ymir work/environment/cerapus5.msenv");

		const TEnvironmentData* c_pEnvironmenData;

		if (GetEnvironmentData(DAY_MODE_DARK, &c_pEnvironmenData))
			ResetEnvironmentDataPtr(c_pEnvironmenData);
	}
#endif
}
void CPythonBackground::VisibleGuildArea()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.VisibleMarkedArea();

	m_bVisibleGuildArea = TRUE;
}

void CPythonBackground::DisableGuildArea()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.DisableMarkedArea();

	m_bVisibleGuildArea = FALSE;
}

const char * CPythonBackground::GetWarpMapName()
{
	return m_strMapName.c_str();
}

#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
void CPythonBackground::SetXMasShowEvent(bool bMode)
{
	m_bXMasShowEvent = bMode;
}

bool CPythonBackground::GetXMasShowEvent() const
{
	return m_bXMasShowEvent;
}

bool CPythonBackground::IsBoomMap(const std::string& sMapName) const
{
	static const std::vector<std::string> vMapName
	{
		"season1/metin2_map_sungzi_flame_hill_01",
		"season1/metin2_map_sungzi_flame_hill_02",
		"season1/metin2_map_sungzi_flame_hill_03",
		"season1/metin2_map_sungzi_desert_01",
		"season1/metin2_map_sungzi_desert_hill_01",
		"season1/metin2_map_sungzi_desert_hill_02",
		"season1/metin2_map_sungzi_desert_hill_03",
		"season2/metin2_map_empirewar03",
		"metin2_map_devilsCatacomb",
		"metin2_map_Mt_Thunder",
		"metin2_map_n_flame_dungeon_01",
		"metin2_map_n_snow_dungeon_01",
		"metin2_map_spiderdungeon_01",
		"metin2_map_spiderdungeon_02",
		"metin2_map_spiderdungeon_03",
		"metin2_map_deviltower1",
		"metin2_map_dawnmist_dungeon_01",
		"metin2_map_Mt_Th_dungeon_01",
		"metin2_12zi_stage",
		"metin2_map_n_flame_dragon",
		"metin2_map_defensewave",
		"metin2_map_defensewave_port",
		"metin2_map_mists_of_island",
		"metin2_map_miniboss_01",
		"metin2_map_miniboss_02",
		"metin2_map_labyrinth",
		"metin2_map_boss_crack_skipia",
		"metin2_map_boss_crack_flame",
		"metin2_map_boss_crack_snow",
		"metin2_map_boss_crack_dawnmist",
		"metin2_map_boss_awaken_skipia",
		"metin2_map_boss_awaken_flame",
		"metin2_map_boss_awaken_snow",
		"metin2_map_boss_awaken_dawnmist",
		"metin2_map_elemental_01",
		"metin2_map_elemental_02",
		"metin2_map_elemental_03",
		"metin2_map_elemental_04",
		"metin2_map_smhdungeon_01",
		"metin2_map_smhdungeon_02",
		"metin2_map_snake_temple_02",
		"metin2_map_whitedragoncave_01",
		"metin2_map_whitedragoncave_02",
		"metin2_map_whitedragoncave_boss",
		"metin2_map_anglar_dungeon_01",
		"metin2_map_otherworld_01",
		"metin2_map_otherworld_02",
		"metin2_map_otherworld_03",
		"metin2_map_otherworld_04",
		"metin2_map_smhgate_a1",
		"metin2_map_smhgate_b1",
		"metin2_map_smhgate_c1",
		"metin2_map_smhgate_threeway",
		"metin2_map_smhgate_desert",
		"metin2_map_smhgate_flame",
		"metin2_map_smhgate_snow",
		"metin2_map_smhgate_devils",
		"metin2_map_smhgate_dawnmist",
	};

	return std::find(vMapName.begin(), vMapName.end(), sMapName) == vMapName.end();
}

bool CPythonBackground::IsSnowMap(const std::string& sMapName) const
{
	static const std::vector<std::string> vMapName
	{
		"metin2_map_n_flame_01",
		"metin2_map_n_desert_01",
		"season1/metin2_map_sungzi_flame_hill_01",
		"season1/metin2_map_sungzi_flame_hill_02",
		"season1/metin2_map_sungzi_flame_hill_03",
		"season1/metin2_map_sungzi_desert_01",
		"season1/metin2_map_sungzi_desert_hill_01",
		"season1/metin2_map_sungzi_desert_hill_02",
		"season1/metin2_map_sungzi_desert_hill_03",
		"season2/metin2_map_empirewar03",
		"metin2_map_devilsCatacomb",
		"metin2_map_Mt_Thunder",
		"metin2_map_n_flame_dungeon_01",
		"metin2_map_spiderdungeon_01",
		"metin2_map_spiderdungeon_02",
		"metin2_map_spiderdungeon_03",
		"metin2_map_deviltower1",
		"metin2_map_dawnmist_dungeon_01",
		"metin2_map_Mt_Th_dungeon_01",
		"metin2_12zi_stage",
		"metin2_map_n_flame_dragon",
		"metin2_map_defensewave",
		"metin2_map_defensewave_port",
		"metin2_map_mists_of_island",
		"metin2_map_miniboss_01",
		"metin2_map_miniboss_02",
		"metin2_map_labyrinth",
		"metin2_map_boss_crack_skipia",
		"metin2_map_boss_crack_flame",
		"metin2_map_boss_crack_snow",
		"metin2_map_boss_crack_dawnmist",
		"metin2_map_boss_awaken_skipia",
		"metin2_map_boss_awaken_flame",
		"metin2_map_boss_awaken_snow",
		"metin2_map_boss_awaken_dawnmist",
		"metin2_guild_pve",
		"metin2_map_elemental_01",
		"metin2_map_elemental_02",
		"metin2_map_elemental_03",
		"metin2_map_elemental_04",
		"metin2_map_battleroyale",
		"metin2_map_maze_dungeon1",
		"metin2_map_maze_dungeon2",
		"metin2_map_maze_dungeon3",
		"metin2_map_smhdungeon_01",
		"metin2_map_smhdungeon_02",
		"metin2_map_snake_temple_02",
		"metin2_map_whitedragoncave_01",
		"metin2_map_whitedragoncave_02",
		"metin2_map_whitedragoncave_boss",
		"metin2_map_anglar_dungeon_01",
		"metin2_map_otherworld_01",
		"metin2_map_otherworld_02",
		"metin2_map_otherworld_03",
		"metin2_map_otherworld_04",
		"metin2_map_secretdungeon_01",
		"metin2_map_smhgate_a1",
		"metin2_map_smhgate_b1",
		"metin2_map_smhgate_c1",
		"metin2_map_smhgate_threeway",
		"metin2_map_smhgate_desert",
		"metin2_map_smhgate_flame",
		"metin2_map_smhgate_snow",
		"metin2_map_smhgate_devils",
		"metin2_map_smhgate_dawnmist",
	};

	return std::find(vMapName.begin(), vMapName.end(), sMapName) == vMapName.end();
}

void CPythonBackground::SetNightModeOption(bool bMode)
{
	m_bNightMode = bMode;
}

bool CPythonBackground::GetNightModeOption() const
{
	return m_bNightMode;
}

void CPythonBackground::SetSnowModeOption(bool bMode)
{
	m_bSnowMode = bMode;
}

bool CPythonBackground::GetSnowModeOption() const
{
	return m_bSnowMode;
}

void CPythonBackground::SetSnowTextureModeOption(bool bMode)
{
	m_bSnowTextureMode = bMode;
}

bool CPythonBackground::GetSnowTextureModeOption() const
{
	return m_bSnowTextureMode;
}

int CPythonBackground::GetDayMode() const
{
	return m_iDayMode;
}

bool CPythonBackground::IsBoomMap()
{
	std::vector<std::string> mapVec{
		"metin2_map_n_flame_01",
			"metin2_map_n_desert_01",
			"metin2_mapb_b1",
			"metin2_map_spiderdungeon_01",
			"metin2_map_spiderdungeon_02",
			"metin2_map_spiderdungeon_03",
			"metin2_map_spiderdungeon_03_dungeon",
			"metin2_map_deviltower1",
			"season1/metin2_map_sungzi_flame_hill_01",
			"season1/metin2_map_sungzi_flame_hill_02",
			"season1/metin2_map_sungzi_flame_hill_03",
			"season1/metin2_map_sungzi_desert_01",
			"season1/metin2_map_sungzi_desert_hill_01",
			"season1/metin2_map_sungzi_desert_hill_02",
			"season1/metin2_map_sungzi_desert_hill_03",
			"season2/metin2_map_empirewar03",
			"metin2_map_devilsCatacomb",
			"metin2_map_Mt_Thunder",
			"metin2_map_n_flame_dungeon_01",
			"metin2_map_dawnmist_dungeon_01",
			"metin2_map_Mt_Th_dungeon_01"
	};

	if (std::find(std::begin(mapVec), std::end(mapVec), m_strMapName.c_str()) != std::end(mapVec))
		return false;

	return true;
}
#endif

void CPythonBackground::ChangeToDay()
{
	m_iDayMode = DAY_MODE_LIGHT;
	SetAmbianceEffectsLevel(0);
}

void CPythonBackground::ChangeToNight()
{
	m_iDayMode = DAY_MODE_DARK;
	SetAmbianceEffectsLevel(1);
}

void CPythonBackground::EnableSnowEnvironment()
{
	m_SnowEnvironment.Enable();
}

void CPythonBackground::DisableSnowEnvironment()
{
	m_SnowEnvironment.Disable();
}

const D3DXVECTOR3 c_v3TreePos = D3DXVECTOR3(76500.0f, -60900.0f, 20215.0f);

void CPythonBackground::SetXMaxTree(int iGrade)
{
	if (!m_pkMap)
		return;

	assert(iGrade >= 0 && iGrade <= 3);
	m_iXMasTreeGrade = iGrade;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	if ("map_n_snowm_01" != m_strMapName)
	{
		rkMap.XMasTree_Destroy();
		return;
	}

	if (0 == iGrade)
	{
		rkMap.XMasTree_Destroy();
		return;
	}

	//////////////////////////////////////////////////////////////////////

	iGrade -= 1;
	iGrade = max(iGrade, 0);
	iGrade = min(iGrade, 2);

	static std::string s_strTreeName[3] = {
		"d:/ymir work/tree/christmastree1.spt",
		"d:/ymir work/tree/christmastree2.spt",
		"d:/ymir work/tree/christmastree3.spt"
	};
	static std::string s_strEffectName[3] = {
		"d:/ymir work/effect/etc/christmas_tree/tree_1s.mse",
		"d:/ymir work/effect/etc/christmas_tree/tree_2s.mse",
		"d:/ymir work/effect/etc/christmas_tree/tree_3s.mse",
	};
	rkMap.XMasTree_Set(c_v3TreePos.x, c_v3TreePos.y, c_v3TreePos.z, s_strTreeName[iGrade].c_str(), s_strEffectName[iGrade].c_str());
}

void CPythonBackground::CreateTargetEffect(DWORD dwID, DWORD dwChrVID)
{
	m_kMap_dwTargetID_dwChrID.emplace(dwID, dwChrVID);
}

void CPythonBackground::CreateTargetEffect(DWORD dwID, long lx, long ly)
{
	if (m_kMap_dwTargetID_dwChrID.end() != m_kMap_dwTargetID_dwChrID.find(dwID))
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	DWORD dwBaseX;
	DWORD dwBaseY;
	rkMap.GetBaseXY(&dwBaseX, &dwBaseY);

	int ilx = +(lx-int(dwBaseX));
	int ily = -(ly-int(dwBaseY));

	float fHeight = rkMap.GetHeight(float(ilx), float(ily));

	if (0.0f == fHeight)
	{
		SReserveTargetEffect ReserveTargetEffect;
		ReserveTargetEffect.ilx = ilx;
		ReserveTargetEffect.ily = ily;
		m_kMap_dwID_kReserveTargetEffect.emplace(dwID, ReserveTargetEffect);
		return;
	}

	CreateSpecialEffect(dwID, ilx, ily, fHeight, g_strEffectName.c_str());
}

void CPythonBackground::DeleteTargetEffect(DWORD dwID)
{
	if (m_kMap_dwID_kReserveTargetEffect.end() != m_kMap_dwID_kReserveTargetEffect.find(dwID))
	{
		m_kMap_dwID_kReserveTargetEffect.erase(dwID);
	}
	if (m_kMap_dwTargetID_dwChrID.end() != m_kMap_dwTargetID_dwChrID.find(dwID))
	{
		m_kMap_dwTargetID_dwChrID.erase(dwID);
	}

	DeleteSpecialEffect(dwID);
}

void CPythonBackground::CreateSpecialEffect(DWORD dwID, float fx, float fy, float fz, const char * c_szFileName)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SpecialEffect_Create(dwID, fx, fy, fz, c_szFileName);
}

void CPythonBackground::DeleteSpecialEffect(DWORD dwID)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SpecialEffect_Delete(dwID);
}

