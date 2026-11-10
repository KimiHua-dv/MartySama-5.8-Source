#pragma once

class CPythonSystem : public CSingleton<CPythonSystem>
{
	public:
		enum EWindow
		{
			WINDOW_STATUS,
			WINDOW_INVENTORY,
			WINDOW_ABILITY,
			WINDOW_SOCIETY,
			WINDOW_JOURNAL,
			WINDOW_COMMAND,

			WINDOW_QUICK,
			WINDOW_GAUGE,
			WINDOW_MINIMAP,
			WINDOW_CHAT,

			WINDOW_MAX_NUM,
		};

		enum
		{
			FREQUENCY_MAX_NUM  = 30,
			RESOLUTION_MAX_NUM = 100
		};

		typedef struct SResolution
		{
			DWORD	width;
			DWORD	height;
			DWORD	bpp;		// bits per pixel (high-color = 16bpp, true-color = 32bpp)

			DWORD	frequency[20];
			BYTE	frequency_count;
		} TResolution;

		typedef struct SWindowStatus
		{
			int		isVisible;
			int		isMinimized;

			int		ixPosition;
			int		iyPosition;
			int		iHeight;
		} TWindowStatus;

		typedef struct SConfig
		{
			DWORD			width;
			DWORD			height;
			DWORD			bpp;
			DWORD			frequency;

			bool			is_software_cursor;
			bool			is_object_culling;
			int				iDistance;
			int				iShadowLevel;

			FLOAT			music_volume;
			BYTE			voice_volume;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
			FLOAT			effects_volume;
#endif
			int				gamma;

			int				isSaveID;
			char			SaveID[20];

			bool			bWindowed;
			bool			bDecompressDDS;
			bool			bNoSoundCard;
			bool			bUseDefaultIME;
			BYTE			bSoftwareTiling;
			bool			bViewChat;
			bool			bAlwaysShowName;
			bool			bShowDamage;
			bool			bShowSalesText;
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
			bool			bShowMobAIFlag;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
			bool			bShowMobLevel;
#endif
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_ENABLE_FOV_OPTION)
			int				iShadowQualityLevel;
			int				iTerrainShadow;
			int				iTreeShadow;
			int				iRainLevel;
			int				iSpecularLevel;
			int				iBloodLevel;
			int				iAmbianceEffectsLevel;
			int				iGraphicsLevel;
			int				iGrassType;
			float			fSharpnessPower;
			float			fLeaves;
#endif
#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
			bool			bShowNightMode;
			bool			bShowSnowMode;
			bool			bShowSnowTextureMode;
			bool			bNightMode;
			bool			bSnowMode;
			bool			bSnowTextureMode;
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
			float			fPrivateShopViewDistance;
#endif
		} TConfig;

	public:
		CPythonSystem();
		virtual ~CPythonSystem();

		void Clear();
		void SetInterfaceHandler(PyObject * poHandler);
		void DestroyInterfaceHandler();

		// Config
		void							SetDefaultConfig();
		bool							LoadConfig();
		bool							SaveConfig();
		void							ApplyConfig();
		void							SetConfig(TConfig * set_config);
		TConfig *						GetConfig();
		void							ChangeSystem();

		// Interface
		bool							LoadInterfaceStatus();
		void							SaveInterfaceStatus();
		bool							isInterfaceConfig();
		const TWindowStatus &			GetWindowStatusReference(int iIndex);

		DWORD							GetWidth();
		DWORD							GetHeight();
		DWORD							GetBPP();
		DWORD							GetFrequency();
		bool							IsSoftwareCursor();
		bool							IsWindowed();
		bool							IsViewChat();
		bool							IsAlwaysShowName();
		bool							IsShowDamage();
		bool							IsShowSalesText();
		bool							IsUseDefaultIME();
		bool							IsNoSoundCard();
		bool							IsAutoTiling();
		bool							IsSoftwareTiling();
		void							SetSoftwareTiling(bool isEnable);
		void							SetViewChatFlag(int iFlag);
		void							SetAlwaysShowNameFlag(int iFlag);
		void							SetShowDamageFlag(int iFlag);
		void							SetShowSalesTextFlag(int iFlag);
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
		bool							IsShowMobAIFlag();
		void							SetShowMobAIFlagFlag(int iFlag);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
		bool							IsShowMobLevel();
		void							SetShowMobLevelFlag(int iFlag);
#endif
#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
		void							SetNightModeOption(int iFlag);
		bool							GetNightModeOption();

		void							SetSnowModeOption(int iFlag);
		bool							GetSnowModeOption();

		void							SetSnowTextureModeOption(int iFlag);
		bool							GetSnowTextureModeOption();

		void							SetNightModeOption(bool bEnable);
		bool							GetNightModeOption() const;

		void							SetSnowModeOption(bool bEnable);
		bool							GetSnowModeOption() const;

		void							SetSnowTextureModeOption(bool bEnable);
		bool							GetSnowTextureModeOption() const;
#endif

		// Window
		void							SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

		// SaveID
		int								IsSaveID();
		const char *					GetSaveID();
		void							SetSaveID(int iValue, const char * c_szSaveID);

		/// Display
		void							GetDisplaySettings();

		int								GetResolutionCount();
		int								GetFrequencyCount(int index);
		bool							GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp);
		bool							GetFrequency(int index, int freq_index, OUT DWORD *frequncy);
		int								GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
		int								GetFrequencyIndex(int res_index, DWORD frequency);
		bool							isViewCulling();

		// Sound
		float							GetMusicVolume();
		int								GetSoundVolume();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
		int								GetEffectsVolume();
#endif
		void							SetMusicVolume(float fVolume);
		void							SetSoundVolumef(float fVolume);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
		void							SetEffectsVolumef(float fVolume);
#endif
		int								GetDistance();
		int								GetShadowLevel();
		void							SetShadowLevel(unsigned int level);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_ENABLE_FOV_OPTION)
		float							GetSharpnessPower();
		void							SetSharpnessPower(float fsharpness);

		int								GetGraphicsLevel();
		void							SetGraphicsLevel(unsigned int level);

		int								GetShadowQualityLevel();
		void							SetShadowQualityLevel(unsigned int level);

		int								GetTerrainShadow();
		void							SetTerrainShadow(unsigned int level);

		int								GetTreeShadow();
		void							SetTreeShadow(unsigned int level);

		int								GetRainLevel();
		void							SetRainLevel(unsigned int level);

		int								GetSpecularLevel();
		void							SetSpecularLevel(unsigned int level);

		int								GetBloodLevel();
		void							SetBloodLevel(unsigned int level);

		int								GetAmbianceEffectsLevel();
		void							SetAmbianceEffectsLevel(unsigned int level);

		int								GetGrassType();
		void							SetGrassType(unsigned int level);

		float							GetLEAVES();
		void							SetLEAVES(const float c_fValue);
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	public:
		float							GetPrivateShopViewDistance() { return m_Config.fPrivateShopViewDistance; }
		void							SetPrivateShopViewDistance(float fDistance) { m_Config.fPrivateShopViewDistance = fDistance; }
#endif

	protected:
		TResolution						m_ResolutionList[RESOLUTION_MAX_NUM];
		int								m_ResolutionCount;

		TConfig							m_Config;
		TConfig							m_OldConfig;

		bool							m_isInterfaceConfig;
		PyObject *						m_poInterfaceHandler;
		TWindowStatus					m_WindowStatus[WINDOW_MAX_NUM];
};

