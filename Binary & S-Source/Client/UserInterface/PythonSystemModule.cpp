#include "StdAfx.h"
#include "PythonSystem.h"

PyObject * systemGetWidth(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetWidth());
}

PyObject * systemGetHeight(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetHeight());
}

PyObject * systemSetInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonSystem::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject * systemDestroyInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::Instance().DestroyInterfaceHandler();
	return Py_BuildNone();
}

PyObject * systemReserveResource(PyObject* poSelf, PyObject* poArgs)
{
	std::set<std::string> ResourceSet;
	CResourceManager::Instance().PushBackgroundLoadingSet(ResourceSet);
	return Py_BuildNone();
}

PyObject * systemisInterfaceConfig(PyObject* poSelf, PyObject* poArgs)
{
	int isInterfaceConfig = CPythonSystem::Instance().isInterfaceConfig();
	return Py_BuildValue("i", isInterfaceConfig);
}

PyObject * systemSaveWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisible))
		return Py_BuildException();

	int iMinimized;
	if (!PyTuple_GetInteger(poArgs, 2, &iMinimized))
		return Py_BuildException();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 3, &ix))
		return Py_BuildException();

	int iy;
	if (!PyTuple_GetInteger(poArgs, 4, &iy))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 5, &iHeight))
		return Py_BuildException();

	CPythonSystem::Instance().SaveWindowStatus(iIndex, iVisible, iMinimized, ix, iy, iHeight);
	return Py_BuildNone();
}

PyObject * systemGetWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const CPythonSystem::TWindowStatus & c_rWindowStatus = CPythonSystem::Instance().GetWindowStatusReference(iIndex);
	return Py_BuildValue("iiiii", c_rWindowStatus.isVisible,
								  c_rWindowStatus.isMinimized,
								  c_rWindowStatus.ixPosition,
								  c_rWindowStatus.iyPosition,
								  c_rWindowStatus.iHeight);
}

PyObject * systemGetConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();

	int iRes = CPythonSystem::Instance().GetResolutionIndex(tmp->width, tmp->height, tmp->bpp);
	int iFrequency = CPythonSystem::Instance().GetFrequencyIndex(iRes, tmp->frequency);

	return Py_BuildValue("iiiiiiii",  iRes,
									  iFrequency,
									  tmp->is_software_cursor,
									  tmp->is_object_culling,
									  tmp->music_volume,
									  tmp->voice_volume,
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
									  tmp->effects_volume,
#endif
									  tmp->gamma,
									  tmp->iDistance);
}

PyObject * systemSetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	char * szSaveID;
	if (!PyTuple_GetString(poArgs, 1, &szSaveID))
		return Py_BuildException();

	CPythonSystem::Instance().SetSaveID(iValue, szSaveID);
	return Py_BuildNone();
}

PyObject * systemisSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int value = CPythonSystem::Instance().IsSaveID();
	return Py_BuildValue("i", value);
}

PyObject * systemGetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	const char * c_szSaveID = CPythonSystem::Instance().GetSaveID();
	return Py_BuildValue("s", c_szSaveID);
}

PyObject * systemGetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetMusicVolume());
}

PyObject * systemGetSoundVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSoundVolume());
}

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
PyObject* systemGetEffectsVolume(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetEffectsVolume());
}
#endif

PyObject * systemSetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject * systemSetSoundVolumef(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetSoundVolumef(fVolume);
	return Py_BuildNone();
}

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
PyObject* systemSetEffectsVolumef(PyObject* poSelf, PyObject* poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetEffectsVolumef(fVolume);
	return Py_BuildNone();
}
#endif

PyObject * systemIsSoftwareCursor(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsSoftwareCursor());
}

PyObject * systemSetViewChatFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetViewChatFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemIsViewChat(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsViewChat());
}

PyObject * systemSetAlwaysShowNameFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetAlwaysShowNameFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowDamageFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowDamageFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowSalesTextFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowSalesTextFlag(iFlag);

	return Py_BuildNone();
}

#ifdef WJ_SHOW_MOB_INFO
#include "PythonCharacterManager.h"
#include "PythonNonPlayer.h"
#include "PythonSystem.h"
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
PyObject * systemIsShowMobAIFlag(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobAIFlag());
}
PyObject * systemSetShowMobAIFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobAIFlagFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it=CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti=CPythonCharacterManager::Instance().CharacterInstanceEnd();
			it!=ti;
			++it)
	{
		CInstanceBase * pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoAiFlagRefresh();
	}
	return Py_BuildNone();
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
PyObject * systemIsShowMobLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobLevel());
}
PyObject * systemSetShowMobLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobLevelFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it=CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti=CPythonCharacterManager::Instance().CharacterInstanceEnd();
			it!=ti;
			++it)
	{
		CInstanceBase * pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoLevelRefresh();
	}
	return Py_BuildNone();
}
#endif
#endif

PyObject * systemIsAlwaysShowName(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAlwaysShowName());
}

PyObject * systemIsShowDamage(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowDamage());
}

PyObject * systemIsShowSalesText(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowSalesText());
}

PyObject * systemSetConfig(PyObject * poSelf, PyObject * poArgs)
{
	int res_index;
	int width;
	int height;
	int bpp;
	int frequency_index;
	int frequency;
	int software_cursor;
	int shadow;
	int object_culling;
	int music_volume;
	int voice_volume;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	int effects_volume;
#endif
	int gamma;
	int distance;

	if (!PyTuple_GetInteger(poArgs, 0, &res_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &software_cursor))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 3, &shadow))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 4, &object_culling))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 5, &music_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 6, &voice_volume))
		return Py_BuildException();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	if (!PyTuple_GetInteger(poArgs, 7, &effects_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 9, &distance))
		return Py_BuildException();
#else
	if (!PyTuple_GetInteger(poArgs, 7, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &distance))
		return Py_BuildException();
#endif

	if (!CPythonSystem::Instance().GetResolution(res_index, (DWORD *) &width, (DWORD *) &height, (DWORD *) &bpp))
		return Py_BuildNone();

	if (!CPythonSystem::Instance().GetFrequency(res_index,frequency_index, (DWORD *) &frequency))
		return Py_BuildNone();

	CPythonSystem::TConfig tmp;

	memcpy(&tmp, CPythonSystem::Instance().GetConfig(), sizeof(tmp));

	tmp.width				= width;
	tmp.height				= height;
	tmp.bpp					= bpp;
	tmp.frequency			= frequency;
	tmp.is_software_cursor	= software_cursor ? true : false;
	tmp.is_object_culling	= object_culling ? true : false;
	tmp.music_volume		= (char) music_volume;
	tmp.voice_volume		= (char) voice_volume;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	tmp.effects_volume		= (char) voice_volume;
#endif
	tmp.gamma				= gamma;
	tmp.iDistance			= distance;

	CPythonSystem::Instance().SetConfig(&tmp);
	return Py_BuildNone();
}

PyObject * systemApplyConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::Instance().ApplyConfig();
	return Py_BuildNone();
}

PyObject * systemSaveConfig(PyObject * poSelf, PyObject * poArgs)
{
	int ret = CPythonSystem::Instance().SaveConfig();
	return Py_BuildValue("i", ret);
}

PyObject * systemGetResolutionCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetResolutionCount());
}

PyObject * systemGetFrequencyCount(PyObject * poSelf, PyObject * poArgs)
{
	int	index;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetFrequencyCount(index));
}

PyObject * systemGetResolution(PyObject * poSelf, PyObject * poArgs)
{
	int	index;
	DWORD width = 0, height = 0, bpp = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonSystem::Instance().GetResolution(index, &width, &height, &bpp);
	return Py_BuildValue("iii", width, height, bpp);
}

PyObject * systemGetCurrentResolution(PyObject * poSelf, PyObject *poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();
	return Py_BuildValue("iii", tmp->width, tmp->height, tmp->bpp);
}

PyObject * systemGetFrequency(PyObject * poSelf, PyObject * poArgs)
{
	int	index, frequency_index;
	DWORD frequency = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	CPythonSystem::Instance().GetFrequency(index, frequency_index, &frequency);
	return Py_BuildValue("i", frequency);
}

PyObject * systemGetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowLevel());
}

PyObject * systemSetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level > 0)
		CPythonSystem::Instance().SetShadowLevel(level);

	return Py_BuildNone();
}

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_ENABLE_FOV_OPTION)
PyObject* systemGetGraphicsLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetGraphicsLevel());
}

PyObject* systemSetGraphicsLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	CPythonSystem::Instance().SetGraphicsLevel(level);

	return Py_BuildNone();
}

PyObject* systemGetSpecularLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSpecularLevel());
}

PyObject* systemSetSpecularLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetSpecularLevel(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetBloodLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetBloodLevel());
}

PyObject* systemSetBloodLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetBloodLevel(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetGrassType(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetGrassType());
}

PyObject* systemSetGrassType(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetGrassType(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetAmbianceEffectsLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetAmbianceEffectsLevel());
}

PyObject* systemSetAmbianceEffectsLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetAmbianceEffectsLevel(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetRainLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetRainLevel());
}

PyObject* systemSetRainLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetRainLevel(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetShadowQualityLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowQualityLevel());
}

PyObject* systemSetShadowQualityLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetShadowQualityLevel(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetTerrainShadow(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetTerrainShadow());
}

PyObject* systemSetTerrainShadow(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetTerrainShadow(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetTreeShadow(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetTreeShadow());
}

PyObject* systemSetTreeShadow(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetTreeShadow(iLevel);
	return Py_BuildNone();
}

PyObject* systemSetSharpnessPower(PyObject* poSelf, PyObject* poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();

	CPythonSystem::Instance().SetSharpnessPower(fValue);
	return Py_BuildNone();
}

PyObject* systemGetSharpnessPower(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetSharpnessPower());
}

PyObject* systemSetLeaves(PyObject* poSelf, PyObject* poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();

	CPythonSystem::Instance().SetLEAVES(fValue);
	return Py_BuildNone();
}

PyObject* systemGetLeaves(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetLEAVES());
}
#endif

#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
PyObject* systemSetNightModeOption(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonSystem::Instance().SetNightModeOption(bEnable);
	return Py_BuildNone();
}

PyObject* systemGetNightModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetNightModeOption());
}

PyObject* systemSetSnowModeOption(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonSystem::Instance().SetSnowModeOption(bEnable);
	return Py_BuildNone();
}

PyObject* systemGetSnowModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSnowModeOption());
}

PyObject* systemSetSnowTextureModeOption(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonSystem::Instance().SetSnowTextureModeOption(bEnable);
	return Py_BuildNone();
}

PyObject* systemGetSnowTextureModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSnowTextureModeOption());
}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
PyObject* systemGetPrivateShopViewDistance(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetPrivateShopViewDistance());
}

PyObject* systemSetPrivateShopViewDistance(PyObject* poSelf, PyObject* poArgs)
{
	float fDistance;

	if (!PyTuple_GetFloat(poArgs, 0, &fDistance))
		return Py_BuildException();

	CPythonSystem::Instance().SetPrivateShopViewDistance(fDistance);

	return Py_BuildNone();
}
#endif

void initsystemSetting()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetWidth",					systemGetWidth,					METH_VARARGS },
		{ "GetHeight",					systemGetHeight,				METH_VARARGS },

		{ "SetInterfaceHandler",		systemSetInterfaceHandler,		METH_VARARGS },
		{ "DestroyInterfaceHandler",	systemDestroyInterfaceHandler,	METH_VARARGS },
		{ "ReserveResource",			systemReserveResource,			METH_VARARGS },

		{ "isInterfaceConfig",			systemisInterfaceConfig,		METH_VARARGS },
		{ "SaveWindowStatus",			systemSaveWindowStatus,			METH_VARARGS },
		{ "GetWindowStatus",			systemGetWindowStatus,			METH_VARARGS },

		{ "GetResolutionCount",			systemGetResolutionCount,		METH_VARARGS },
		{ "GetFrequencyCount",			systemGetFrequencyCount,		METH_VARARGS },

		{ "GetCurrentResolution",		systemGetCurrentResolution,		METH_VARARGS },

		{ "GetResolution",				systemGetResolution,			METH_VARARGS },
		{ "GetFrequency",				systemGetFrequency,				METH_VARARGS },

		{ "ApplyConfig",				systemApplyConfig,				METH_VARARGS },
		{ "SetConfig",					systemSetConfig,				METH_VARARGS },
		{ "SaveConfig",					systemSaveConfig,				METH_VARARGS },
		{ "GetConfig",					systemGetConfig,				METH_VARARGS },

		{ "SetSaveID",					systemSetSaveID,				METH_VARARGS },
		{ "isSaveID",					systemisSaveID,					METH_VARARGS },
		{ "GetSaveID",					systemGetSaveID,				METH_VARARGS },

		{ "GetMusicVolume",				systemGetMusicVolume,			METH_VARARGS },
		{ "GetSoundVolume",				systemGetSoundVolume,			METH_VARARGS },
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
		{ "GetEffectsVolume",			systemGetEffectsVolume,			METH_VARARGS },
#endif
		{ "SetMusicVolume",				systemSetMusicVolume,			METH_VARARGS },
		{ "SetSoundVolumef",			systemSetSoundVolumef,			METH_VARARGS },
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
		{ "SetEffectsVolumef",			systemSetEffectsVolumef,		METH_VARARGS },
#endif
		{ "IsSoftwareCursor",			systemIsSoftwareCursor,			METH_VARARGS },

		{ "SetViewChatFlag",			systemSetViewChatFlag,			METH_VARARGS },
		{ "IsViewChat",					systemIsViewChat,				METH_VARARGS },

		{ "SetAlwaysShowNameFlag",		systemSetAlwaysShowNameFlag,	METH_VARARGS },
		{ "IsAlwaysShowName",			systemIsAlwaysShowName,			METH_VARARGS },

		{ "SetShowDamageFlag",			systemSetShowDamageFlag,		METH_VARARGS },
		{ "IsShowDamage",				systemIsShowDamage,				METH_VARARGS },

		{ "SetShowSalesTextFlag",		systemSetShowSalesTextFlag,		METH_VARARGS },
		{ "IsShowSalesText",			systemIsShowSalesText,			METH_VARARGS },

		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },

#ifdef WJ_SHOW_MOB_INFO
		{ "IsShowMobAIFlag",			systemIsShowMobAIFlag,			METH_VARARGS },
		{ "SetShowMobAIFlag",			systemSetShowMobAIFlag,			METH_VARARGS },

		{ "IsShowMobLevel",				systemIsShowMobLevel,			METH_VARARGS },
		{ "SetShowMobLevel",			systemSetShowMobLevel,			METH_VARARGS },
#endif

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_ENABLE_FOV_OPTION)
		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },

		{ "GetGraphicsLevel",			systemGetGraphicsLevel,			METH_VARARGS },
		{ "SetGraphicsLevel",			systemSetGraphicsLevel,			METH_VARARGS },

		{ "GetShadowQualityLevel",		systemGetShadowQualityLevel,	METH_VARARGS },
		{ "SetShadowQualityLevel",		systemSetShadowQualityLevel,	METH_VARARGS },

		{ "GetTerrainShadow",			systemGetTerrainShadow,			METH_VARARGS },
		{ "SetTerrainShadow",			systemSetTerrainShadow,			METH_VARARGS },

		{ "GetTreeShadow",				systemGetTreeShadow,			METH_VARARGS },
		{ "SetTreeShadow",				systemSetTreeShadow,			METH_VARARGS },

		{ "GetRainLevel",				systemGetRainLevel,				METH_VARARGS },
		{ "SetRainLevel",				systemSetRainLevel,				METH_VARARGS },

		{ "GetGrassType",				systemGetGrassType,				METH_VARARGS },
		{ "SetGrassType",				systemSetGrassType,				METH_VARARGS },

		{ "GetSpecularLevel",			systemGetSpecularLevel,			METH_VARARGS },
		{ "SetSpecularLevel",			systemSetSpecularLevel,			METH_VARARGS },

		{ "GetBloodLevel",				systemGetBloodLevel,			METH_VARARGS },
		{ "SetBloodLevel",				systemSetBloodLevel,			METH_VARARGS },

		{ "SetSharpnessPower",			systemSetSharpnessPower,		METH_VARARGS },
		{ "GetSharpnessPower",			systemGetSharpnessPower,		METH_VARARGS },

		{ "GetAmbianceEffectsLevel",	systemGetAmbianceEffectsLevel,	METH_VARARGS },
		{ "SetAmbianceEffectsLevel",	systemSetAmbianceEffectsLevel,	METH_VARARGS },

		{ "SetLEAVES",					systemSetLeaves,				METH_VARARGS },
		{ "GetLEAVES",					systemGetLeaves,				METH_VARARGS },
#endif

#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
		{ "SetNightModeOption",			systemSetNightModeOption,		METH_VARARGS },
		{ "GetNightModeOption",			systemGetNightModeOption,		METH_VARARGS },
		{ "SetSnowModeOption",			systemSetSnowModeOption,		METH_VARARGS },
		{ "GetSnowModeOption",			systemGetSnowModeOption,		METH_VARARGS },
		{ "SetSnowTextureModeOption",	systemSetSnowTextureModeOption,	METH_VARARGS },
		{ "GetSnowTextureModeOption",	systemGetSnowTextureModeOption,	METH_VARARGS },
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		{ "GetPrivateShopViewDistance",				systemGetPrivateShopViewDistance,			METH_VARARGS },
		{ "SetPrivateShopViewDistance",				systemSetPrivateShopViewDistance,			METH_VARARGS },
#endif
		{ NULL,							NULL,							NULL }
	};

	PyObject * poModule = Py_InitModule("systemSetting", s_methods);

	PyModule_AddIntConstant(poModule, "WINDOW_STATUS",		CPythonSystem::WINDOW_STATUS);
	PyModule_AddIntConstant(poModule, "WINDOW_INVENTORY",	CPythonSystem::WINDOW_INVENTORY);
	PyModule_AddIntConstant(poModule, "WINDOW_ABILITY",		CPythonSystem::WINDOW_ABILITY);
	PyModule_AddIntConstant(poModule, "WINDOW_SOCIETY",		CPythonSystem::WINDOW_SOCIETY);
	PyModule_AddIntConstant(poModule, "WINDOW_JOURNAL",		CPythonSystem::WINDOW_JOURNAL);
	PyModule_AddIntConstant(poModule, "WINDOW_COMMAND",		CPythonSystem::WINDOW_COMMAND);

	PyModule_AddIntConstant(poModule, "WINDOW_QUICK",		CPythonSystem::WINDOW_QUICK);
	PyModule_AddIntConstant(poModule, "WINDOW_GAUGE",		CPythonSystem::WINDOW_GAUGE);
	PyModule_AddIntConstant(poModule, "WINDOW_MINIMAP",		CPythonSystem::WINDOW_MINIMAP);
	PyModule_AddIntConstant(poModule, "WINDOW_CHAT",		CPythonSystem::WINDOW_CHAT);
}

