/***********************************************************************//**
 *	SoumaScriptKeywords.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_KEYWORDS_H_
#define _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_KEYWORDS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../es/EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsKeyword;

ES_END_NS

SRCR_BEGIN_NS

using namespace es;

/*---------------------------------------------------------------------*//**
 *	ソウマ スクリプト キーワード
 *
**//*---------------------------------------------------------------------*/
class SoumaScriptKeywords
{
	//======================================================================
	// 定数
public:
	// 専用キーワード
	enum Keyword
	{
		// クラス名
		S_CLASS_NS_CLUSTER,						// "Corn"
		S_CLASS_EN_CLUSTER,						// "Cluster"
		S_CLASS_NS_CONSOLE,						// "Console"
		S_CLASS_EN_CONSOLE,						// "Sola"
		S_CLASS_NS_SOUMA,						// "Souma"
		S_CLASS_EN_SOUMA,						// "SourceMagic"
		S_CLASS_NS_UNIT,						// "Mono"
		S_CLASS_EN_UNIT,						// "Unit"
		// 定数アクセサ名
		S_CONSTP_NS_ACCESSORY,					// "asorio"
		S_CONSTP_EN_ACCESSORY,					// "accessory"
		S_CONSTP_NS_LEFT_HAND,					// "hara"
		S_CONSTP_EN_LEFT_HAND,					// "leftHand"
		S_CONSTP_NS_RIGHT_HAND,					// "fura"
		S_CONSTP_EN_RIGHT_HAND,					// "rightHand"
		// 関数アクセサ名
#if 0
		S_FUNCP_NS_ASCEN_AETHER,				// "acenAeth"
		S_FUNCP_EN_ASCEN_AETHER,				// "ascendAether"
		S_FUNCP_NS_ASCEN_ELEMENT,				// "acenElm"
		S_FUNCP_EN_ASCEN_ELEMENT,				// "ascendElement"
		S_FUNCP_NS_ASCEN_FIRE,					// "acenFogo"
		S_FUNCP_EN_ASCEN_FIRE,					// "ascendFire"
		S_FUNCP_NS_ASCEN_SOIL,					// "acenSolo"
		S_FUNCP_EN_ASCEN_SOIL,					// "ascendSoil"
		S_FUNCP_NS_ASCEN_WATER,					// "acenNeru"
		S_FUNCP_EN_ASCEN_WATER,					// "ascendWater"
		S_FUNCP_NS_ASCEN_WIND,					// "acenVan"
		S_FUNCP_EN_ASCEN_WIND,					// "ascendWind"
#endif
		S_FUNCP_NS_CENTERING,					// "edradin"
		S_FUNCP_EN_CENTERING,					// "centering"
		S_FUNCP_NS_CREATE_CLUSTER,				// "enluc"
		S_FUNCP_EN_CREATE_CLUSTER,				// "belight"
		S_FUNCP_NS_CREATE_INTENT,				// "enito"
		S_FUNCP_EN_CREATE_INTENT,				// "enword"
		S_FUNCP_NS_DISSOLVE,					// "disona"
		S_FUNCP_EN_DISSOLVE,					// "dissolve"
		S_FUNCP_NS_ENUM_AROUND_UNITS,			// "enumosmono"
		S_FUNCP_EN_ENUM_AROUND_UNITS,			// "enumAroundUnits"
		S_FUNCP_NS_ENUM_RECOGNIZED_UNITS,		// "enumanekmono"
		S_FUNCP_EN_ENUM_RECOGNIZED_UNITS,		// "enumRecognizedUnits"
		S_FUNCP_NS_ENUM_SPIRIT_KNOWS_UNITS,		// "enumfaesirumono"
		S_FUNCP_ES_ENUM_SPIRIT_KNOWS_UNITS,		// "enumSpiritKnowsUnits"
		S_FUNCP_NS_ENUM_UNITS_BY_NAME,			// "enummonoparesta"
		S_FUNCP_ES_ENUM_UNITS_BY_NAME,			// "enumUnitsByName"
#if 0
		S_FUNCP_NS_DESCEN_AETHER,				// "decenAeth"
		S_FUNCP_EN_DESCEN_AETHER,				// "descendAether"
		S_FUNCP_NS_DESCEN_ATMAN,				// "decenAtoma"
		S_FUNCP_EN_DESCEN_ATMAN,				// "descendAtman"
		S_FUNCP_NS_DESCEN_ELEMENT,				// "decenElm"
		S_FUNCP_EN_DESCEN_ELEMENT,				// "descendElement"
		S_FUNCP_NS_DESCEN_FIRE,					// "decenFogo"
		S_FUNCP_EN_DESCEN_FIRE,					// "descendFire"
		S_FUNCP_NS_DESCEN_WATER,				// "decenNeru"
		S_FUNCP_EN_DESCEN_WATER,				// "descendWater"
		S_FUNCP_NS_DESCEN_WIND,					// "decenVan"
		S_FUNCP_EN_DESCEN_WIND,					// "descendWind"
		S_FUNCP_NS_EVOKE_AETHER,				// "lamAeth"
		S_FUNCP_EN_EVOKE_AETHER,				// "evokeAether"
#endif
		S_FUNCP_NS_EVOKE_ELEMENT,				// "lamelm"
		S_FUNCP_EN_EVOKE_ELEMENT,				// "evokeElement"
		S_FUNCP_NS_EVOKE_ORIGINAL_ELEMENTS,		// "lamprimelm"
		S_FUNCP_EN_EVOKE_ORIGINAL_ELEMENTS,		// "evokeOriginalElements"
		S_FUNCP_NS_EVOKE_PRESET_ELEMENTS,		// "lampresetelm"
		S_FUNCP_EN_EVOKE_PRESET_ELEMENTS,		// "evokePresetElements"
#if 0
		S_FUNCP_NS_EVOKE_FIRE,					// "lamFogo"
		S_FUNCP_EN_EVOKE_FIRE,					// "evokeFire"
		S_FUNCP_NS_EVOKE_SOIL,					// "lamSolo"
		S_FUNCP_EN_EVOKE_SOIL,					// "evokeSoil"
		S_FUNCP_NS_EVOKE_WATER,					// "lamNeru"
		S_FUNCP_EN_EVOKE_WATER,					// "evokeWater"
		S_FUNCP_NS_EVOKE_WIND,					// "lamVan"
		S_FUNCP_EN_EVOKE_WIND,					// "evokeWind"
		S_FUNCP_NS_GET_AETHER_QUANTITY,			// "getAethQuat"
		S_FUNCP_EN_GET_AETHER_QUANTITY,			// "getAetherQuantity"
#endif
		S_FUNCP_NS_GET_CONSDEPTH,				// "getconsnur"
		S_FUNCP_EN_GET_CONSDEPTH,				// "getConsDepth"
		S_FUNCP_NS_GET_CONSOLE,					// "getsola"
		S_FUNCP_EN_GET_CONSOLE,					// "getConsole"
		S_FUNCP_NS_GET_ELEMENT_QUANTITY,		// "getelmquat"
		S_FUNCP_EN_GET_ELEMENT_QUANTITY,		// "getElementQuantity"
		S_FUNCP_NS_GET_ENERGY_DAMAGE,			// "getnadano"
		S_FUNCP_EN_GET_ENERGY_DAMAGE,			// "getEnergyDamage"
		S_FUNCP_NS_GET_EQUIPED_UNIT,			// "getsokuitmono"
		S_FUNCP_EN_GET_EQUIPED_UNIT,			// "getEquipedUnit"
		S_FUNCP_NS_GET_FOCUS_UNITS,				// "getfocmono"
		S_FUNCP_EN_GET_FOCUS_UNITS,				// "getFocusUnits"
		S_FUNCP_NS_GET_LIFE_ENERGY,				// "getcuilna"
		S_FUNCP_EN_GET_LIFE_ENERGY,				// "getLifeEnergy"
		S_FUNCP_NS_GET_MASTER_UNIT,				// "getorthormono"
		S_FUNCP_EN_GET_MASTER_UNIT,				// "getMasterUnit"
		S_FUNCP_NS_GET_MAX_LIFE_ENERGY,			// "getgemcuilna"
		S_FUNCP_EN_GET_MAX_LIFE_ENERGY,			// "getMaxLifeEnergy"
		S_FUNCP_NS_GET_PHYSICAL_DAMAGE,			// "getmonotudano"
		S_FUNCP_EN_GET_PHYSICAL_DAMAGE,			// "getPhysicalDamage"
#if 0
		S_FUNCP_NS_GET_FIRE_QUANTITY,			// "getFogoQuat"
		S_FUNCP_EN_GET_FIRE_QUANTITY,			// "getFireQuantity"
		S_FUNCP_NS_GET_SOIL_QUANTITY,			// "getSoloQuat"
		S_FUNCP_EN_GET_SOIL_QUANTITY,			// "getSoilQuantity"
		S_FUNCP_NS_GET_WATER_QUANTITY,			// "getNeruQuat"
		S_FUNCP_EN_GET_WATER_QUANTITY,			// "getWaterQuantity"
		S_FUNCP_NS_GET_WIND_QUANTITY,			// "getVanQuat"
		S_FUNCP_EN_GET_WIND_QUANTITY,			// "getWindQuantity"
#endif
		S_FUNCP_NS_IN,							// "im"
		S_FUNCP_EN_IN,							// "in"
		S_FUNCP_NS_IS_ENEMY,					// "hagoth"
		S_FUNCP_EN_IS_ENEMY,					// "isEnemy"
		S_FUNCP_NS_IS_FRIEND,					// "hamelon"
		S_FUNCP_EN_IS_FRIEND,					// "isFriend"
		S_FUNCP_NS_IS_ITEM,						// "hamotol"
		S_FUNCP_EN_IS_ITEM,						// "isItem"
		S_FUNCP_NS_IS_MASTER,					// "haorthor"
		S_FUNCP_EN_IS_MASTER,					// "isMaster"
		S_FUNCP_NS_IS_PUT_ITEM,					// "haposumotol"
		S_FUNCP_EN_IS_PUT_ITEM,					// "isPutItem"
		S_FUNCP_NS_MATERIALIZE_CLUSTER,			// "materaizad"
		S_FUNCP_EN_MATERIALIZE_CLUSTER,			// "materialize"
		S_FUNCP_NS_MEDITATE,					// "ia"
		S_FUNCP_EN_MEDITATE,					// "meditate"
		S_FUNCP_NS_OUT,							// "de"
		S_FUNCP_EN_OUT,							// "out"
#if 0
		S_FUNCP_NS_SERVE_CLUSTER,				// "buia"
		S_FUNCP_EN_SERVE_CLUSTER,				// "serve"
#endif
		S_FUNCP_NS_SET_FOCUS_UNIT,				// "setfocmono"
		S_FUNCP_EN_SET_FOCUS_UNIT,				// "setFocusUnit"
		S_FUNCP_NS_SLOW_CHANT,					// "tarudlina"
		S_FUNCP_EN_SLOW_CHANT,					// "slowChant"
		S_FUNCP_NS_SYNTHESIZE,					// "awas"
		S_FUNCP_EN_SYNTHESIZE,					// "synthesize"
		S_FUNCP_NS_WAIT,						// "deri"
		S_FUNCP_EN_WAIT,						// "wait"
		// 引数などで使われる汎用単語
		S_GENEW_NS_SOIL,						// "solo"
		S_GENEW_EN_SOIL,						// "soil"
		S_GENEW_NS_WATER,						// "neru"
		S_GENEW_EN_WATER,						// "water"
		S_GENEW_NS_FIRE,						// "fogo"
		S_GENEW_EN_FIRE,						// "fire"
		S_GENEW_NS_WIND,						// "van"
		S_GENEW_EN_WIND,						// "wind"
		S_GENEW_NS_AETHEL,						// "aeth"
		S_GENEW_EN_AETHEL,						// "aethel"
		S_GENEW_NS_LEFT_HAND,					// "hara"
		S_GENEW_EN_LEFT_HAND,					// "left-hand"
		S_GENEW_NS_RIGHT_HAND,					// "fura"
		S_GENEW_EN_RIGHT_HAND,					// "right-hand"
		S_GENEW_NS_ACCESSORY,					// "asorio"
		S_GENEW_EN_ACCESSORY,					// "accessory"
		S_GENEW_NS_ITEM,						// "motol"
		S_GENEW_EN_ITEM,						// "item"
		S_GENEW_NS_MIDDLE,						// "perin"
		S_GENEW_EN_MIDDLE,						// "middle"
		S_GENEW_NS_MIGHTY,						// "taura"
		S_GENEW_EN_MIGHTY,						// "mighty"
		S_GENEW_NS_POINT,						// "ment"
		S_GENEW_EN_POINT,						// "point"
		S_GENEW_NS_POINTS,						// "ments"
		S_GENEW_EN_POINTS,						// "points"
		S_GENEW_NS_STRONG,						// "bel"
		S_GENEW_EN_STRONG,						// "strong"
		S_GENEW_NS_WEAK,						// "rac"
		S_GENEW_EN_WEAK,						// "weak"
		S_GENEW_NS_OP_PURIFY,					// "prud"
		S_GENEW_EN_OP_PURIFY,					// "purify"
		S_GENEW_NS_OP_CURE,						// "traos"
		S_GENEW_EN_OP_CURE,						// "cure"
		S_GENEW_NS_OP_CATHARSIS,				// "catras"
		S_GENEW_EN_OP_CATHARSIS,				// "catharsis"
		S_GENEW_NS_OP_EXCEED,					// "modati"
		S_GENEW_EN_OP_EXCEED,					// "exceed"
		S_GENEW_NS_OP_WRAP,						// "hamad"
		S_GENEW_EN_OP_WRAP,						// "wrap"
		S_GENEW_NS_OP_BARRIER,					// "baron"
		S_GENEW_EN_OP_BARRIER,					// "barrier"
		S_GENEW_NS_OP_SUFFUSE,					// "enchar"
		S_GENEW_EN_OP_SUFFUSE,					// "suffuse"
		S_GENEW_NS_OP_UNWRAP,					// "nuhamad"
		S_GENEW_EN_OP_UNWRAP,					// "unwrap"
		S_GENEW_NS_OP_UNBARRIER,				// "nubaron"
		S_GENEW_EN_OP_UNBARRIER,				// "unbarrier"
		S_GENEW_NS_OP_UNSUFFUSE,				// "nuenchar"
		S_GENEW_EN_OP_UNSUFFUSE,				// "unsuffuse"
		S_GENEW_NS_OP_ASCEN_S,					// "eruacen"
		S_GENEW_EN_OP_ASCEN_S,					// "single-ascend"
		S_GENEW_NS_OP_ASCEN_A,					// "awolacen"
		S_GENEW_EN_OP_ASCEN_A,					// "all-ascend"
		S_GENEW_NS_OP_DESCEN_S,					// "erudecen"
		S_GENEW_EN_OP_DESCEN_S,					// "single-descend"
		S_GENEW_NS_OP_DESCEN_A,					// "awoldecen"
		S_GENEW_EN_OP_DESCEN_A,					// "all-descend"
		S_GENEW_NS_OP_ATTACK,					// "da"
		S_GENEW_EN_OP_ATTACK,					// "attack"
		S_GENEW_NS_OP_EXC_ATTACK,				// "modati-da"
		S_GENEW_EN_OP_EXC_ATTACK,				// "exceeding-attack"
		S_GENEW_NS_OP_UNW_ATTACK,				// "nuhamad-da"
		S_GENEW_EN_OP_UNW_ATTACK,				// "unwrapping-attack"
		S_GENEW_NS_OP_UNB_ATTACK,				// "nubaron-da"
		S_GENEW_EN_OP_UNB_ATTACK,				// "unbarriering-attack"
		S_GENEW_NS_OP_UNS_ATTACK,				// "nuenchar-da"
		S_GENEW_EN_OP_UNS_ATTACK,				// "unsuffusing-attack"
		S_GENEW_NS_OP_NOP,						// "nop"
		S_GENEW_EN_OP_NOP,						// "nop"
		NUM_S
	};
	/*
			"getalca",			// FUNC_GET_HANDLE
			"setfoc",			// FUNC_SET_FOCUS
	*/

	const static int NUM_LANG = 2;	// SS と JS

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 専用キーワードを得る
	static const EsAtom* getKeyword(s32 kwd);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();

	static bool match(s32 kwd, const CStringBase* str, bool eqnoreCase);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_KEYWORDS_H_
