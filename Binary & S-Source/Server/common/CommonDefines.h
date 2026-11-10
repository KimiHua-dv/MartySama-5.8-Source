#ifndef __INC_ANKIRA_COMMON_DEFINES_H__
#define __INC_ANKIRA_COMMON_DEFINES_H__
#pragma once

enum eCommonDefines {
	MAP_ALLOW_LIMIT = 32, // 32 default
};

///////////////////////////////////////////////////////////////// SISTEMAS - ADICIONALES -//////////////////////////////////////////////////////////
#define _IMPROVED_PACKET_ENCRYPTION_										//Sistema [Algoritmo de encriptación] Oficial.
	#ifdef _IMPROVED_PACKET_ENCRYPTION_										//Sistema [Algoritmo de encriptación] Oficial.
	#define USE_IMPROVED_PACKET_DECRYPTED_BUFFER							//Sistema [Algoritmo de desencriptación] Oficial.
#endif
#define __UDP_BLOCK__														//Sistema [Algoritmo ip pública] Oficial.
#define ENABLE_NO_MOUNT_CHECK												//Sistema [Restricción de habilidades - Caballo -] Oficial.
#define ENABLE_D_NJGUILD													//Sistema [Gremio - Mazmorras -] Oficial.
#define ENABLE_FULL_NOTICE													//Sistema [Anuncios - Noticias -] Oficial.
#define ENABLE_NEWSTUFF														//Sistema [Verificación de datos] Oficial.
#define ENABLE_PORT_SECURITY												//Sistema [Verificación de administración - GM -] Oficial.
#define ENABLE_BELT_INVENTORY_EX											//Sistema [Inventario de cinturones] Oficial.
#define ENABLE_CMD_WARP_IN_DUNGEON											//Sistema [Teletransportación a cualquier mapa- GM -] Oficial.
// #define ENABLE_ITEM_ATTR_COSTUME											//Sistema [Bonificaciones en atuendos] Oficial.
// #define ENABLE_SEQUENCE_SYSTEM											//Sistema [Protección ataques masivos] Oficial.
#define ENABLE_PLAYER_PER_ACCOUNT5											//Sistema [Estructura de 5 raza] Oficial.
#define ENABLE_DICE_SYSTEM													//Sistema [Apuesta de comercio] Oficial.
#define ENABLE_EXTEND_INVEN_SYSTEM											//Sistema [3-4 inventario] Oficial.
#define ENABLE_MOUNT_COSTUME_SYSTEM											//Sistema [Monturas] Oficial.
#define ENABLE_WEAPON_COSTUME_SYSTEM										//Sistema [Atuendos] Oficial.
#define ENABLE_QUEST_DIE_EVENT												//Sistema [Penalización en quest - Muerte de jugador -] Oficial.
#define ENABLE_QUEST_BOOT_EVENT												//Sistema [Activación de eventos - Programados -] Oficial.
#define ENABLE_QUEST_DND_EVENT												//Sistema [Mejora de misiónes - Ejecución personalizada -] Oficial.
#define ENABLE_PET_SYSTEM_EX												//Sistema [Mascotas de acompañamiento] Oficial.
#define ENABLE_SKILL_FLAG_PARTY												//Sistema [Habilidades para miembros del grupo] Oficial.
#define ENABLE_NO_DSS_QUALIFICATION											//Sistema [Alquímia - Cualificación -] Oficial.
// #define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5								//Sistema [División de cuotas entre 5] Oficial.
#define ENABLE_CHECK_SELL_PRICE												//Sistema [Verificación de precio de venta] Oficial.
#define ENABLE_GOTO_LAG_FIX													//Sistema [Limpieza de visión - Reducción de lag -] Oficial.
#define ENABLE_MOUNT_COSTUME_EX_SYSTEM										//Sistema [Caballo - Guerras pvp bloqueo -] Oficial.
#define ENABLE_QUIVER_SYSTEM												//Sistema [Carcaja de arquero] Oficial.
#define ENABLE_REDUCED_ENTITY_VIEW											//Sistema [Optimización de rendimiento] Oficial.
#define ENABLE_GUILD_TOKEN_AUTH												//Sistema [Verificación de gremio] Oficial.
#define ENABLE_DS_GRADE_MYTH												//Sistema [Alquímia - Mítica -] Oficial.
//#define ENABLE_DB_SQL_LOG													//Sistema [Navicat - Tabla de logs -] Oficial.
#define __PET_SYSTEM__														//Sistema [Mascotas] Oficial.
	#ifdef __PET_SYSTEM__													//Sistema [Mascotas] Oficial.
	#define USE_ACTIVE_PET_SEAL_EFFECT										//Sistema [Mascotas - Sello de activación -] Oficial.
	#define PET_SEAL_ACTIVE_SOCKET_IDX 2									//Sistema [Mascotas - Sello de activación en sockets-] Oficial.
	#define USE_PET_SEAL_ON_LOGIN											//Sistema [Mascotas - Reloguearse con mascota -] Oficial.
#endif
#define ENABLE_WOLFMAN_CHARACTER											//Sistema [5 raza - Lobo -] Oficial.
#ifdef ENABLE_WOLFMAN_CHARACTER												//Sistema [5 raza - Lobo -] Oficial.
	#define DISABLE_WOLFMAN_ON_CREATE										//Sistema [5 raza - Lobo desactivar -] Oficial.
	#define USE_MOB_BLEEDING_AS_POISON										//Sistema [5 raza - Bonificación de sangrado -] Oficial.
	#define USE_MOB_CLAW_AS_DAGGER											//Sistema [5 raza - Resistencia de garras -] Oficial.
// #define USE_ITEM_BLEEDING_AS_POISON										//Sistema [5 raza - Resistencia de sangrado -] Oficial.
// #define USE_ITEM_CLAW_AS_DAGGER											//Sistema [5 raza - Resistencia de garras -] Oficial.
	#define USE_WOLFMAN_STONES												//Sistema [5 raza - Piedras -] Oficial.
	#define USE_WOLFMAN_BOOKS												//Sistema [5 raza - Libros -] Oficial.
#endif
// #define ENABLE_MAGIC_REDUCTION_SYSTEM									//Sistema [Reducción de resistencia mágica] Oficial.
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM										//Sistema [Reducción de resistencia mágica] Oficial.
// #define USE_MAGIC_REDUCTION_STONES										//Sistema [Reducción de resistencia mágica] Oficial.
#endif
#define DISABLE_STOP_RIDING_WHEN_DIE										//Sistema [Caballo - Revivir con el -] Oficial.
#define ENABLE_ACCE_COSTUME_SYSTEM											//Sistema [Estolas] Oficial.
	#ifdef ENABLE_ACCE_COSTUME_SYSTEM										//Sistema [Estolas] Oficial.
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS							//Sistema [Estolas - Bonificaciones negativas -] Oficial.
#endif
#define ENABLE_HIGHLIGHT_NEW_ITEM											//Sistema [Efecto al tirar o intercambiar] Oficial.
#define ENABLE_KILL_EVENT_FIX												//Sistema [Fix de misiones - 0 exp al matar -] Oficial.
// #define ENABLE_SYSLOG_PACKET_SENT										//Sistema [Depuración] Oficial.
#define ENABLE_EXTEND_ITEM_AWARD											//Sistema [Creación de objetos con bonificaciones] Oficial.
#ifdef ENABLE_EXTEND_ITEM_AWARD												//Sistema [Creación de objetos con bonificaciones] Oficial.
	// #define USE_ITEM_AWARD_CHECK_ATTRIBUTES								//Sistema [Creación de objetos con bonificaciones - Rango limitado -] Oficial.
#endif
#define ENABLE_CHEQUE_SYSTEM												//Sistema [Won] Oficial.
	#ifdef ENABLE_CHEQUE_SYSTEM												//Sistema [Won] Oficial.
	#define ENABLE_SHOP_USE_CHEQUE											//Sistema [Won - Tiendas -] Oficial.
	#define DISABLE_CHEQUE_DROP												//Sistema [Won - Caida desactivar-] Oficial.
	#define ENABLE_WON_EXCHANGE_WINDOW										//Sistema [Won - Ventana -] Oficial.
#endif
#define ENABLE_PENDANT_SYSTEM												//Sistema [Talismanes] Oficial.
#define ENABLE_GLOVE_SYSTEM													//Sistema [Guantes de poder] Oficial
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_SPECIAL_STORAGE
#define WJ_ENABLE_TRADABLE_ICON
#define __PREMIUM_PRIVATE_SHOP__
	#ifdef __PREMIUM_PRIVATE_SHOP__
	#define ENABLE_PRIVATE_SHOP_PREMIUM_TIME
	#define ENABLE_PRIVATE_SHOP_CHEQUE
	#define ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	#define ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	// #define ENABLE_PRIVATE_SHOP_SPECIAL_INV
	// #define ENABLE_PRIVATE_SHOP_EXTEND_INV
	// #define ENABLE_PRIVATE_SHOP_EXTEND_SPECIAL_INV
	#define ENABLE_PRIVATE_SHOP_LOCKED_SLOTS
	// #define ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	// #define ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	//#define ENABLE_PRIVATE_SHOP_APPLY_RANDOM
#endif
#define ENABLE_FLY_MOUNT

#define __BL_TELEPORT__

#define ENABLE_SWITCHBOT_WORLDARD

#define ENABLE_SHOW_CHEST_DROP
#define __SYSERRGOD_SKILL_TRAINING__
#endif
