#pragma once

///////////////////////////////////////////////////////////////// SISTEMAS - ADICIONALES -//////////////////////////////////////////////////////////

#define ENABLE_SWITCHBOT_WORLDARD
#define LOCALE_SERVICE_EUROPE										//Sistema [Idioma - Europeo -] Oficial.
#define ENABLE_ENERGY_SYSTEM										//Sistema [Energía] Oficial.
#define ENABLE_DRAGON_SOUL_SYSTEM									//Sistema [Alquímia] Oficial.
	#ifdef ENABLE_DRAGON_SOUL_SYSTEM								//Sistema [Alquímia] Oficial.
	#define ENABLE_NO_DSS_QUALIFICATION								//Sistema [Alquímia - Cualificación -] Oficial.
	#define ENABLE_DS_GRADE_MYTH									//Sistema [Alquímia - Mítica -] Oficial.
#endif
#define ENABLE_NEW_EQUIPMENT_SYSTEM									//Sistema [Inventario de cinturones] Oficial.
#define ENABLE_PACK_GET_CHECK										//Sistema [Reconocimiento de extensiones - .py, .pyc, .txt -] Oficial.
#define ENABLE_PROTOSTRUCT_AUTODETECT								//Sistema [Estructura de protos] Oficial.
#define ENABLE_PLAYER_PER_ACCOUNT5									//Sistema [Estructura de 5 raza] Oficial.
#define ENABLE_LEVEL_IN_TRADE										//Sistema [Ver nivel al comercial] Oficial.
#define ENABLE_DICE_SYSTEM											//Sistema [Apuesta de comercio] Oficial.
#define ENABLE_EXTEND_INVEN_SYSTEM									//Sistema [3-4 inventario] Oficial.
#define ENABLE_DISCORD_RPC											//Sistema [Información del juego en discord] Oficial.
#define ENABLE_PET_SYSTEM_EX										//Sistema [Mascotas de acompañamiento] Oficial.
//#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5							//Sistema [División de cuotas entre 5] Oficial.
#define ENABLE_QUIVER_SYSTEM										//Sistema [Carcaja de arquero] Oficial.
#define ENABLE_INGAME_CONSOLE										//Sistema [Consola dentro del juego] Oficial.
#define ENABLE_LOCALE_COMMON										//Sistema [Nuevos locale_game y locale_interface] Oficial.
#define ENABLE_GUILD_TOKEN_AUTH										//Sistema [Verificación de gremio] Oficial.
#define ENABLE_NEW_EVENT_STRUCT										//Sistema [Estructura mejorada de líneas] Oficial.
	#ifdef ENABLE_NEW_EVENT_STRUCT									//Sistema [Estructura mejorada de líneas] Oficial.
	#define USE_NEW_EVENT_TEXT_AUTO_Y								//Sistema [Estructura mejorada de líneas] Oficial.
#endif
#define ENABLE_WOLFMAN_CHARACTER									//Sistema [5 raza - Lobo -] Oficial.
	#ifdef ENABLE_WOLFMAN_CHARACTER									//Sistema [5 raza - Lobo -] Oficial.
	#define DISABLE_WOLFMAN_ON_CREATE								//Sistema [5 raza - Lobo desactivar -] Oficial.
#endif
#define ENABLE_MOUNT_COSTUME_SYSTEM									//Sistema [Monturas] Oficial.
#define ENABLE_WEAPON_COSTUME_SYSTEM								//Sistema [Atuendos] Oficial.
	#define ENABLE_COSTUME_SYSTEM									//Sistema [Atuendos] Oficial.
	#ifdef ENABLE_WEAPON_COSTUME_SYSTEM								//Sistema [Atuendos] Oficial.
	#define ENABLE_USE_COSTUME_ATTR									//Sistema [Bonificaciones en atuendos] Oficial.
#endif
#define ENABLE_ACCE_COSTUME_SYSTEM									//Sistema [Estolas] Oficial.
	#ifdef ENABLE_ACCE_COSTUME_SYSTEM								//Sistema [Estolas] Oficial.
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS					//Sistema [Estolas - Bonificaciones negativas -] Oficial.
#endif
#define ENABLE_MOUSEWHEEL_EVENT										//Sistema [Rueda de ratón - Desplazamiento -] Oficial.
#define ENABLE_MINIMAP_TELEPORT_CLICK								//Sistema [Teletransportar - Minimapa -] Oficial.
#define ENABLE_CHEQUE_SYSTEM										//Sistema [Won] Oficial.
	#ifdef ENABLE_CHEQUE_SYSTEM										//Sistema [Won] Oficial.
	#define DISABLE_CHEQUE_DROP										//Sistema [Won - Caida desactivar-] Oficial.
	#define ENABLE_WON_EXCHANGE_WINDOW								//Sistema [Won - Ventana -] Oficial.
#endif
#define ENABLE_LITTLE_PACK											//Sistema [Extensión de encriptación] Oficial.
#define ENABLE_PENDANT_SYSTEM										//Sistema [Talismanes] Oficial.
#define ENABLE_GLOVE_SYSTEM											//Sistema [Guantes de poder] Oficial.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// SISTEMAS - GRÁFICOS -/////////////////////////////////////////////////////////////
#define WJ_SHOW_MOB_INFO											//Sistema [Información de monstruos] Oficial.
	#ifdef WJ_SHOW_MOB_INFO											//Sistema [Información de monstruos] Oficial.
	#define ENABLE_SHOW_MOBAIFLAG									//Sistema [Información de monstruos - Caracter -] Oficial.
	#define ENABLE_SHOW_MOBLEVEL									//Sistema [Información de monstruos - Nivel -] Oficial.
	#define WJ_SHOW_MOB_INFO_EX										//Sistema [Información de monstruos] Oficial.
#endif
#define ENABLE_RACE_HEIGHT											//Sistema [Altura de npc] Oficial.
#define ENABLE_LVL115_ARMOR_EFFECT									//Sistema [Efectos armaduras nivel 115] Oficial.
#define ENABLE_SLOT_WINDOW_EX										//Sistema [Efectos de objetos de inventario] Oficial.
#define ENABLE_CANSEEHIDDENTHING_FOR_GM								//Sistema [GM - Ver efectos invisibles -] Oficial.
#define ENABLE_TEXT_LEVEL_REFRESH									//Sistema [Actualización de nivel instantaneo] Oficial.
#define ENABLE_MINIMAP_WHITEMARK_CIRCLE								//Sistema [Efecto de circulos - Minimapa y mapa -] Oficial.
#define __ENABLE_STEALTH_FIX__										//Sistema [Camuflaje - Fix efecto -] Oficial.
#define ENABLE_EMOJI_SYSTEM											//Sistema [Efecto de imagenes - Textos -] Oficial.
#define ENABLE_HIGHLIGHT_NEW_ITEM									//Sistema [Efecto al tirar o intercambiar] Oficial.
#define ENABLE_4TH_AFF_SKILL_DESC									//Sistema [Cuarto efecto de habilidades] Oficial.
#define DIRECTX9													//Sistema [Directx9] Oficial.
	#if defined(DIRECTX9)											//Sistema [Directx9] Oficial.
	#define D3DLIGHT8					D3DLIGHT9					//Sistema [Directx9] Oficial.
	#define D3DMATERIAL8				D3DMATERIAL9				//Sistema [Directx9] Oficial.
	#define IDirect3DVertexBuffer8		IDirect3DVertexBuffer9		//Sistema [Directx9] Oficial.
	#define LPDIRECT3DTEXTURE8			LPDIRECT3DTEXTURE9			//Sistema [Directx9] Oficial.
	#define LPDIRECT3DSURFACE8			LPDIRECT3DSURFACE9			//Sistema [Directx9] Oficial.
	#define D3DVIEWPORT8				D3DVIEWPORT9				//Sistema [Directx9] Oficial.
	#define LPDIRECT3DDEVICE8			LPDIRECT3DDEVICE9			//Sistema [Directx9] Oficial.
	#define LPDIRECT3DVERTEXBUFFER8		LPDIRECT3DVERTEXBUFFER9		//Sistema [Directx9] Oficial.
	#define LPDIRECT3DINDEXBUFFER8		LPDIRECT3DINDEXBUFFER9		//Sistema [Directx9] Oficial.
	#define DXLOCKTYPE					(void**)					//Sistema [Directx9] Oficial.
	#define D3DVERTEXELEMENT8			D3DVERTEXELEMENT9			//Sistema [Directx9] Oficial.
	#define LPDIRECT3DBASETEXTURE8		LPDIRECT3DBASETEXTURE9		//Sistema [Directx9] Oficial.
	#define LPDIRECT3DPIXELSHADER8		LPDIRECT3DPIXELSHADER9		//Sistema [Directx9] Oficial.
	#define D3DADAPTER_IDENTIFIER8		D3DADAPTER_IDENTIFIER9		//Sistema [Directx9] Oficial.
	#define IDirect3D8					IDirect3D9					//Sistema [Directx9] Oficial.
	#define IDirect3DDevice8			IDirect3DDevice9			//Sistema [Directx9] Oficial.
	#define D3DCAPS8					D3DCAPS9					//Sistema [Directx9] Oficial.
	#define LPDIRECT3D8					LPDIRECT3D9					//Sistema [Directx9] Oficial.
	#define D3DCAPS2_CANRENDERWINDOWED	DDCAPS2_CANRENDERWINDOWED	//Sistema [Directx9] Oficial.
	#define IDirect3DTexture8			IDirect3DTexture9			//Sistema [Directx9] Oficial.
	#define Direct3DCreate8				Direct3DCreate9				//Sistema [Directx9] Oficial.
	#define IDirect3DSurface8			IDirect3DSurface9			//Sistema [Directx9] Oficial.
	#else
	#define DXLOCKTYPE					(BYTE**)					//Sistema [Directx9] Oficial.
#endif
#define ENABLE_ADVANCED_GAME_OPTIONS								//Sistema [Nueva ventana de opciones] Oficial.
#define ANK_SYSTEM_GRAPHICS
#ifdef ANK_SYSTEM_GRAPHICS
	#define ANKIRA_TRANSPARENCY_BUILDING_FIX
	#define ANKIRA_IMPROVE_ENTITY_FREEZE
	#define ANKIRA_BLOOD_EFFECT
	#define ANKIRA_GPU_LEAF_PLACEMENT
	#define ANKIRA_GPU_TREE_WIND
	#define ANKIRA_TREE_DYNAMIC_LIGHT
	#define ANKIRA_DEATH_CAMERA
	#define ANKIRA_WATER_REFLECTION
	#define ANKIRA_UNDERWATER_CAMERA
	#define ANKIRA_SHARPNESS
	#define ANKIRA_DYNAMIC_LIGHT
	#define ANKIRA_SPECULAR
	#define ANKIRA_ENVIRONMENT_EFFECT_OPTION
	#define ANKIRA_GRAPHICS_SOUND
	#define ENABLE_CPP_PSM		// PlayerSettingsModule Client'e Tasima Modulu (Fast Loading)
#endif
#define ENABLE_SPECIAL_STORAGE
#define WJ_ENABLE_TRADABLE_ICON
#define __BL_CLIP_MASK__
#define ENABLE_RENDER_TARGET
#define ENABLE_PREMIUM_PRIVATE_SHOP
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	#define ENABLE_PRIVATE_SHOP_PREMIUM_TIME
	#define ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	#define ENABLE_PRIVATE_SHOP_CHEQUE
	#define ENABLE_PRIVATE_SHOP_LIMITED_DISTANCE_RENDERING
	#define ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	#define ENABLE_PRIVATE_SHOP_LOCKED_SLOTS
	#define ENABLE_PRIVATE_SHOP_PRICE_SORT
	#define ENABLE_PRIVATE_SHOP_TIME_SORT
	#define ENABLE_PRIVATE_SHOP_DIFFERENT_PRICE_RESTRICTION
	// #define ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	// #define ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	//#define ENABLE_PRIVATE_SHOP_APPLY_RANDOM
#endif
#define ENABLE_FIX_MOBS_LAG
#define ENABLE_FLY_MOUNT
#define ENABLE_SHOW_CHEST_DROP

#define __BL_TELEPORT__
#define __SYSERRGOD_SKILL_TRAINING__
#define ENABLE_METIN_QUEUE
#define METIN_QUEUE_MAX 5
#define __RENDER_TARGET__
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

