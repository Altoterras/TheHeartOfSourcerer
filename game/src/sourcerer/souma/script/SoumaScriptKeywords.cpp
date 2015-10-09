/***********************************************************************//**
 *	SoumaScriptKeywords.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SoumaScriptKeywords.h"

// Friends
#include "../../es/EsAtom.h"
#include "../../es/EsContext.h"
#include "../../es/EsKeywordSet.h"
#include "../../es/EsObject.h"
#include "../../es/EsScopeProperty.h"
#include "../../es/EsSysClasses.h"
#include "../../es/EsSysPrototypes.h"
#include "../../es/EsValue.h"

// External

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SoumaScriptKeywords メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	専用キーワードを得る
**//*---------------------------------------------------------------------*/
const EsAtom* SoumaScriptKeywords::getKeyword(s32 kwd)
{
	static const EsAtom* kwarrPrivate[] =
	{
		// クラス名
		EsKeywordSet::newRegisterdAtom("Corn"),								// S_CLASS_NS_CLUSTER
		EsKeywordSet::newRegisterdAtom("Cluster"),							// S_CLASS_EN_CLUSTER
		EsKeywordSet::newRegisterdAtom("Console"),							// S_CLASS_NS_CONSOLE
		EsKeywordSet::newRegisterdAtom("Sola"),								// S_CLASS_EN_CONSOLE
		EsKeywordSet::newRegisterdAtom("Souma"),							// S_CLASS_NS_SOUMA
		EsKeywordSet::newRegisterdAtom("SourceMagic"),						// S_CLASS_EN_SOUMA
		EsKeywordSet::newRegisterdAtom("Mono"),								// S_CLASS_NS_UNIT
		EsKeywordSet::newRegisterdAtom("Unit"),								// S_CLASS_EN_UNIT
		// 定数アクセサ名
		EsKeywordSet::newRegisterdAtom("asorio"),							// S_CONSTP_NS_ACCESSORY
		EsKeywordSet::newRegisterdAtom("accessory"),						// S_CONSTP_EN_ACCESSORY
		EsKeywordSet::newRegisterdAtom("hara"),								// S_CONSTP_NS_LEFT_HAND
		EsKeywordSet::newRegisterdAtom("leftHand"),							// S_CONSTP_EN_LEFT_HAND
		EsKeywordSet::newRegisterdAtom("fura"),								// S_CONSTP_NS_RIGHT_HAND
		EsKeywordSet::newRegisterdAtom("rightHand"),						// S_CONSTP_EN_RIGHT_HAND
		// 関数アクセサ名
#if 0
		EsKeywordSet::newRegisterdAtom("acenaeth"),							// S_FUNCP_NS_ASCEN_AETHER
		EsKeywordSet::newRegisterdAtom("ascendAether"),						// S_FUNCP_EN_ASCEN_AETHER
		EsKeywordSet::newRegisterdAtom("acenelm"),							// S_FUNCP_NS_ASCEN_ELEMENT
		EsKeywordSet::newRegisterdAtom("ascendElement"),					// S_FUNCP_EN_ASCEN_ELEMENT
		EsKeywordSet::newRegisterdAtom("acenfogo"),							// S_FUNCP_NS_ASCEN_FIRE
		EsKeywordSet::newRegisterdAtom("ascendFire"),						// S_FUNCP_EN_ASCEN_FIRE
		EsKeywordSet::newRegisterdAtom("acensolo"),							// S_FUNCP_NS_ASCEN_SOIL
		EsKeywordSet::newRegisterdAtom("ascendSoil"),						// S_FUNCP_EN_ASCEN_SOIL
		EsKeywordSet::newRegisterdAtom("acenneru"),							// S_FUNCP_NS_ASCEN_WATER
		EsKeywordSet::newRegisterdAtom("ascendWater"),						// S_FUNCP_EN_ASCEN_WATER
		EsKeywordSet::newRegisterdAtom("acenvan"),							// S_FUNCP_NS_ASCEN_WIND
		EsKeywordSet::newRegisterdAtom("ascendWind"),						// S_FUNCP_EN_ASCEN_WIND
#endif
		EsKeywordSet::newRegisterdAtom("edradin"),							// S_FUNCP_NS_CENTERING
		EsKeywordSet::newRegisterdAtom("centering"),						// S_FUNCP_EN_CENTERING
		EsKeywordSet::newRegisterdAtom("enluc"),							// S_FUNCP_NS_CREATE_CLUSTER
		EsKeywordSet::newRegisterdAtom("belight"),							// S_FUNCP_EN_CREATE_CLUSTER
		EsKeywordSet::newRegisterdAtom("enwadna"),							// S_FUNCP_NS_CREATE_INTENT
		EsKeywordSet::newRegisterdAtom("enword"),							// S_FUNCP_EN_CREATE_INTENT
		EsKeywordSet::newRegisterdAtom("disona"),							// S_FUNCP_NS_DISSOLVE
		EsKeywordSet::newRegisterdAtom("dissolve"),							// S_FUNCP_EN_DISSOLVE
		EsKeywordSet::newRegisterdAtom("enumosmono"),						// S_FUNCP_NS_ENUM_AROUND_UNITS
		EsKeywordSet::newRegisterdAtom("enumAroundUnits"),					// S_FUNCP_EN_ENUM_AROUND_UNITS
		EsKeywordSet::newRegisterdAtom("enumanekmono"),						// S_FUNCP_NS_ENUM_RECOGNIZED_UNITS
		EsKeywordSet::newRegisterdAtom("enumRecognizedUnits"),				// S_FUNCP_EN_ENUM_RECOGNIZED_UNITS
		EsKeywordSet::newRegisterdAtom("enumfaesirumono"),					// S_FUNCP_NS_ENUM_SPIRIT_KNOWS_UNITS
		EsKeywordSet::newRegisterdAtom("enumSpiritKnowsUnits"),				// S_FUNCP_ES_ENUM_SPIRIT_KNOWS_UNITS
		EsKeywordSet::newRegisterdAtom("enummonoparesta"),					// S_FUNCP_NS_ENUM_UNITS_BY_NAME
		EsKeywordSet::newRegisterdAtom("enumUnitsByName"),					// S_FUNCP_ES_ENUM_UNITS_BY_NAME
#if 0
		EsKeywordSet::newRegisterdAtom("decenaeth"),						// S_FUNCP_NS_DESCEN_AETHER
		EsKeywordSet::newRegisterdAtom("descendAether"),					// S_FUNCP_EN_DESCEN_AETHER
		EsKeywordSet::newRegisterdAtom("decenatoma"),						// S_FUNCP_NS_DESCEN_ATMAN
		EsKeywordSet::newRegisterdAtom("descendAtman"),						// S_FUNCP_EN_DESCEN_ATMAN
		EsKeywordSet::newRegisterdAtom("decenelm"),							// S_FUNCP_NS_DESCEN_ELEMENT
		EsKeywordSet::newRegisterdAtom("descendElement"),					// S_FUNCP_EN_DESCEN_ELEMENT
		EsKeywordSet::newRegisterdAtom("decenfogo"),						// S_FUNCP_NS_DESCEN_FIRE
		EsKeywordSet::newRegisterdAtom("descendFire"),						// S_FUNCP_EN_DESCEN_FIRE
		EsKeywordSet::newRegisterdAtom("decenneru"),						// S_FUNCP_NS_DESCEN_WATER
		EsKeywordSet::newRegisterdAtom("descendWater"),						// S_FUNCP_EN_DESCEN_WATER
		EsKeywordSet::newRegisterdAtom("decenvan"),							// S_FUNCP_NS_DESCEN_WIND
		EsKeywordSet::newRegisterdAtom("descendWind"),						// S_FUNCP_EN_DESCEN_WIND
		EsKeywordSet::newRegisterdAtom("lamaeth"),							// S_FUNCP_NS_EVOKE_AETHER
		EsKeywordSet::newRegisterdAtom("evokeAether"),						// S_FUNCP_EN_EVOKE_AETHER
#endif
		EsKeywordSet::newRegisterdAtom("lamelm"),							// S_FUNCP_NS_EVOKE_ELEMENT
		EsKeywordSet::newRegisterdAtom("evokeElement"),						// S_FUNCP_EN_EVOKE_ELEMENT
		EsKeywordSet::newRegisterdAtom("lamprimelm"),						// S_FUNCP_NS_EVOKE_ORIGINAL_ELEMENTS
		EsKeywordSet::newRegisterdAtom("evokeOriginalElements"),			// S_FUNCP_EN_EVOKE_ORIGINAL_ELEMENTS
		EsKeywordSet::newRegisterdAtom("lampresetelm"),						// S_FUNCP_NS_EVOKE_PRESET_ELEMENTS
		EsKeywordSet::newRegisterdAtom("evokePresetElements"),				// S_FUNCP_EN_EVOKE_PRESET_ELEMENTS
#if 0
		EsKeywordSet::newRegisterdAtom("lamfogo"),							// S_FUNCP_NS_EVOKE_FIRE
		EsKeywordSet::newRegisterdAtom("evokeFire"),						// S_FUNCP_EN_EVOKE_FIRE
		EsKeywordSet::newRegisterdAtom("lamsolo"),							// S_FUNCP_NS_EVOKE_SOIL
		EsKeywordSet::newRegisterdAtom("evokeSoil"),						// S_FUNCP_EN_EVOKE_SOIL
		EsKeywordSet::newRegisterdAtom("lamneru"),							// S_FUNCP_NS_EVOKE_WATER
		EsKeywordSet::newRegisterdAtom("evokeWater"),						// S_FUNCP_EN_EVOKE_WATER
		EsKeywordSet::newRegisterdAtom("lamvan"),							// S_FUNCP_NS_EVOKE_WIND
		EsKeywordSet::newRegisterdAtom("evokeWind"),						// S_FUNCP_EN_EVOKE_WIND
		EsKeywordSet::newRegisterdAtom("getaethquat"),						// S_FUNCP_NS_GET_AETHER_QUANTITY
		EsKeywordSet::newRegisterdAtom("getAetherQuantity"),				// S_FUNCP_EN_GET_AETHER_QUANTITY
#endif
		EsKeywordSet::newRegisterdAtom("getconsnur"),						// S_FUNCP_NS_GET_CONSDEPTH
		EsKeywordSet::newRegisterdAtom("getConsDepth"),						// S_FUNCP_EN_GET_CONSDEPTH
		EsKeywordSet::newRegisterdAtom("getsola"),							// S_FUNCP_NS_GET_CONSOLE
		EsKeywordSet::newRegisterdAtom("getConsole"),						// S_FUNCP_EN_GET_CONSOLE
		EsKeywordSet::newRegisterdAtom("getelmquat"),						// S_FUNCP_NS_GET_ELEMENT_QUANTITY
		EsKeywordSet::newRegisterdAtom("getElementQuantity"),				// S_FUNCP_EN_GET_ELEMENT_QUANTITY
		EsKeywordSet::newRegisterdAtom("getnadano"),						// S_FUNCP_NS_GET_ENERGY_DAMAGE
		EsKeywordSet::newRegisterdAtom("getEnergyDamage"),					// S_FUNCP_EN_GET_ENERGY_DAMAGE
		EsKeywordSet::newRegisterdAtom("getsokuitmono"),					// S_FUNCP_NS_GET_EQUIPED_UNIT
		EsKeywordSet::newRegisterdAtom("getEquipedUnit"),					// S_FUNCP_EN_GET_EQUIPED_UNIT
		EsKeywordSet::newRegisterdAtom("getfocmono"),						// S_FUNCP_NS_GET_FOCUS_UNITS
		EsKeywordSet::newRegisterdAtom("getFocusUnits"),					// S_FUNCP_EN_GET_FOCUS_UNITS
		EsKeywordSet::newRegisterdAtom("getcuilna"),						// S_FUNCP_NS_GET_LIFE_ENERGY
		EsKeywordSet::newRegisterdAtom("getLifeEnergy"),					// S_FUNCP_EN_GET_LIFE_ENERGY
		EsKeywordSet::newRegisterdAtom("getorthormono"),					// S_FUNCP_NS_GET_MASTER_UNIT
		EsKeywordSet::newRegisterdAtom("getMasterUnit"),					// S_FUNCP_EN_GET_MASTER_UNIT
		EsKeywordSet::newRegisterdAtom("getgemcuilna"),						// S_FUNCP_NS_GET_MAX_LIFE_ENERGY
		EsKeywordSet::newRegisterdAtom("getMaxLifeEnergy"),					// S_FUNCP_EN_GET_MAX_LIFE_ENERGY
		EsKeywordSet::newRegisterdAtom("getmonotudano"),					// S_FUNCP_NS_GET_PHYSICAL_DAMAGE
		EsKeywordSet::newRegisterdAtom("getPhysicalDamage"),				// S_FUNCP_EN_GET_PHYSICAL_DAMAGE
#if 0
		EsKeywordSet::newRegisterdAtom("getfogoquat"),						// S_FUNCP_NS_GET_FIRE_QUANTITY
		EsKeywordSet::newRegisterdAtom("getFireQuantity"),					// S_FUNCP_EN_GET_FIRE_QUANTITY
		EsKeywordSet::newRegisterdAtom("getsoloquat"),						// S_FUNCP_NS_GET_SOIL_QUANTITY
		EsKeywordSet::newRegisterdAtom("getSoilQuantity"),					// S_FUNCP_EN_GET_SOIL_QUANTITY
		EsKeywordSet::newRegisterdAtom("getneruquat"),						// S_FUNCP_NS_GET_WATER_QUANTITY
		EsKeywordSet::newRegisterdAtom("getWaterQuantity"),					// S_FUNCP_EN_GET_WATER_QUANTITY
		EsKeywordSet::newRegisterdAtom("getvanquat"),						// S_FUNCP_NS_GET_WIND_QUANTITY
		EsKeywordSet::newRegisterdAtom("getWindQuantity"),					// S_FUNCP_EN_GET_WIND_QUANTITY
#endif
		EsKeywordSet::newRegisterdAtom("im"),								// S_FUNCP_NS_IN
		EsKeywordSet::newRegisterdAtom("in"),								// S_FUNCP_EN_IN
		EsKeywordSet::newRegisterdAtom("hagoth"),							// S_FUNCP_NS_IS_ENEMY
		EsKeywordSet::newRegisterdAtom("isEnemy"),							// S_FUNCP_EN_IS_ENEMY
		EsKeywordSet::newRegisterdAtom("hamelon"),							// S_FUNCP_NS_IS_FRIEND
		EsKeywordSet::newRegisterdAtom("isFriend"),							// S_FUNCP_EN_IS_FRIEND
		EsKeywordSet::newRegisterdAtom("hamotol"),							// S_FUNCP_NS_IS_ITEM
		EsKeywordSet::newRegisterdAtom("isItem"),							// S_FUNCP_EN_IS_ITEM
		EsKeywordSet::newRegisterdAtom("haorthor"),							// S_FUNCP_NS_IS_MASTER
		EsKeywordSet::newRegisterdAtom("isMaster"),							// S_FUNCP_EN_IS_MASTER
		EsKeywordSet::newRegisterdAtom("haposumotol"),						// S_FUNCP_NS_IS_PUT_ITEM
		EsKeywordSet::newRegisterdAtom("isPutItem"),						// S_FUNCP_EN_IS_PUT_ITEM
		EsKeywordSet::newRegisterdAtom("materaizad"),						// S_FUNCP_NS_MATERIALIZE_CLUSTER
		EsKeywordSet::newRegisterdAtom("materialize"),						// S_FUNCP_EN_MATERIALIZE_CLUSTER
		EsKeywordSet::newRegisterdAtom("ia"),								// S_FUNCP_NS_MEDITATE
		EsKeywordSet::newRegisterdAtom("meditate"),							// S_FUNCP_EN_MEDITATE
		EsKeywordSet::newRegisterdAtom("de"),								// S_FUNCP_NS_OUT
		EsKeywordSet::newRegisterdAtom("out"),								// S_FUNCP_EN_OUT
#if 0
		EsKeywordSet::newRegisterdAtom("buia"),								// S_FUNCP_NS_SERVE_CLUSTER
		EsKeywordSet::newRegisterdAtom("serve"),							// S_FUNCP_EN_SERVE_CLUSTER
#endif
		EsKeywordSet::newRegisterdAtom("setfocmono"),						// S_FUNCP_NS_SET_FOCUS_UNIT
		EsKeywordSet::newRegisterdAtom("setFocusUnit"),						// S_FUNCP_EN_SET_FOCUS_UNIT
		EsKeywordSet::newRegisterdAtom("tarudlina"),						// S_FUNCP_NS_SLOW_CHANT
		EsKeywordSet::newRegisterdAtom("slowChant"),						// S_FUNCP_EN_SLOW_CHANT
		EsKeywordSet::newRegisterdAtom("awaraizad"),						// S_FUNCP_NS_SYNTHESIZE
		EsKeywordSet::newRegisterdAtom("synthesize"),						// S_FUNCP_EN_SYNTHESIZE
		EsKeywordSet::newRegisterdAtom("deri"),								// S_FUNCP_NS_WAIT
		EsKeywordSet::newRegisterdAtom("wait"),								// S_FUNCP_EN_WAIT

		EsKeywordSet::newRegisterdAtom("solo"),								// S_GENEW_NS_SOIL
		EsKeywordSet::newRegisterdAtom("soil"),								// S_GENEW_EN_SOIL
		EsKeywordSet::newRegisterdAtom("neru"),								// S_GENEW_NS_WATER
		EsKeywordSet::newRegisterdAtom("water"),							// S_GENEW_EN_WATER
		EsKeywordSet::newRegisterdAtom("fogo"),								// S_GENEW_NS_FIRE
		EsKeywordSet::newRegisterdAtom("fire"),								// S_GENEW_EN_FIRE
		EsKeywordSet::newRegisterdAtom("van"),								// S_GENEW_NS_WIND
		EsKeywordSet::newRegisterdAtom("wind"),								// S_GENEW_EN_WIND
		EsKeywordSet::newRegisterdAtom("aeth"),								// S_GENEW_NS_AETHEL
		EsKeywordSet::newRegisterdAtom("aethel"),							// S_GENEW_EN_AETHEL
		EsKeywordSet::newRegisterdAtom("hara", true),						// S_GENEW_NS_LEFT_HAND
		EsKeywordSet::newRegisterdAtom("left-hand", true),					// S_GENEW_EN_LEFT_HAND
		EsKeywordSet::newRegisterdAtom("fura", true),						// S_GENEW_NS_RIGHT_HAND
		EsKeywordSet::newRegisterdAtom("right-hand", true),					// S_GENEW_EN_RIGHT_HAND
		EsKeywordSet::newRegisterdAtom("asorio", true),						// S_GENEW_NS_ACCESSORY
		EsKeywordSet::newRegisterdAtom("accessory", true),					// S_GENEW_EN_ACCESSORY
		EsKeywordSet::newRegisterdAtom("motol"),							// S_GENEW_NS_ITEM
		EsKeywordSet::newRegisterdAtom("item"),								// S_GENEW_EN_ITEM
		EsKeywordSet::newRegisterdAtom("perin"),							// S_GENEW_NS_MIDDLE
		EsKeywordSet::newRegisterdAtom("middle"),							// S_GENEW_EN_MIDDLE
		EsKeywordSet::newRegisterdAtom("taura"),							// S_GENEW_NS_MIGHTY
		EsKeywordSet::newRegisterdAtom("mighty"),							// S_GENEW_EN_MIGHTY
		EsKeywordSet::newRegisterdAtom("ment"),								// S_GENEW_NS_POINT
		EsKeywordSet::newRegisterdAtom("point"),							// S_GENEW_EN_POINT
		EsKeywordSet::newRegisterdAtom("ments"),							// S_GENEW_NS_POINTS
		EsKeywordSet::newRegisterdAtom("points"),							// S_GENEW_EN_POINTS
		EsKeywordSet::newRegisterdAtom("bel"),								// S_GENEW_NS_STRONG
		EsKeywordSet::newRegisterdAtom("strong"),							// S_GENEW_EN_STRONG
		EsKeywordSet::newRegisterdAtom("rac"),								// S_GENEW_NS_WEAK
		EsKeywordSet::newRegisterdAtom("weak"),								// S_GENEW_EN_WEAK
		EsKeywordSet::newRegisterdAtom("prud"),								// S_GENEW_NS_OP_PURIFY
		EsKeywordSet::newRegisterdAtom("purify"),							// S_GENEW_EN_OP_PURIFY
		EsKeywordSet::newRegisterdAtom("traos"),							// S_GENEW_NS_OP_CURE
		EsKeywordSet::newRegisterdAtom("cure"),								// S_GENEW_EN_OP_CURE
		EsKeywordSet::newRegisterdAtom("catras"),							// S_GENEW_NS_OP_CATHARSIS
		EsKeywordSet::newRegisterdAtom("catharsis"),						// S_GENEW_EN_OP_CATHARSIS
		EsKeywordSet::newRegisterdAtom("modati"),							// S_GENEW_NS_OP_EXCEED
		EsKeywordSet::newRegisterdAtom("exceed"),							// S_GENEW_EN_OP_EXCEED
		EsKeywordSet::newRegisterdAtom("hamad"),							// S_GENEW_NS_OP_WRAP
		EsKeywordSet::newRegisterdAtom("wrap"),								// S_GENEW_EN_OP_WRAP
		EsKeywordSet::newRegisterdAtom("baron"),							// S_GENEW_NS_OP_BARRIER
		EsKeywordSet::newRegisterdAtom("barrier"),							// S_GENEW_EN_OP_BARRIER
		EsKeywordSet::newRegisterdAtom("enchar"),							// S_GENEW_NS_OP_SUFFUSE
		EsKeywordSet::newRegisterdAtom("suffuse"),							// S_GENEW_EN_OP_SUFFUSE
		EsKeywordSet::newRegisterdAtom("nuhamad"),							// S_GENEW_NS_OP_UNWRAP
		EsKeywordSet::newRegisterdAtom("unwrap"),							// S_GENEW_EN_OP_UNWRAP
		EsKeywordSet::newRegisterdAtom("nubaron"),							// S_GENEW_NS_OP_UNBARRIER
		EsKeywordSet::newRegisterdAtom("unbarrier"),						// S_GENEW_EN_OP_UNBARRIER
		EsKeywordSet::newRegisterdAtom("nuenchar"),							// S_GENEW_NS_OP_UNSUFFUSE
		EsKeywordSet::newRegisterdAtom("unsuffuse"),						// S_GENEW_EN_OP_UNSUFFUSE
		EsKeywordSet::newRegisterdAtom("eruacen"),							// S_GENEW_NS_OP_ASCEN_S
		EsKeywordSet::newRegisterdAtom("single-ascend"),					// S_GENEW_EN_OP_ASCEN_S
		EsKeywordSet::newRegisterdAtom("awolacen"),							// S_GENEW_NS_OP_ASCEN_A
		EsKeywordSet::newRegisterdAtom("all-ascend"),						// S_GENEW_EN_OP_ASCEN_A
		EsKeywordSet::newRegisterdAtom("erudecen"),							// S_GENEW_NS_OP_DESCEN_S
		EsKeywordSet::newRegisterdAtom("single-descend"),					// S_GENEW_EN_OP_DESCEN_S
		EsKeywordSet::newRegisterdAtom("awoldecen"),						// S_GENEW_NS_OP_DESCEN_A
		EsKeywordSet::newRegisterdAtom("all-descend"),						// S_GENEW_EN_OP_DESCEN_A
		EsKeywordSet::newRegisterdAtom("da"),								// S_GENEW_NS_OP_ATTACK
		EsKeywordSet::newRegisterdAtom("attack"),							// S_GENEW_EN_OP_ATTACK
		EsKeywordSet::newRegisterdAtom("modati-da"),						// S_GENEW_NS_OP_EXC_ATTACK
		EsKeywordSet::newRegisterdAtom("exceeding-attack"),					// S_GENEW_EN_OP_EXC_ATTACK
		EsKeywordSet::newRegisterdAtom("nuhamad-da"),						// S_GENEW_NS_OP_UNW_ATTACK
		EsKeywordSet::newRegisterdAtom("unwrapping-attack"),				// S_GENEW_EN_OP_UNW_ATTACK
		EsKeywordSet::newRegisterdAtom("nubaron-da"),						// S_GENEW_NS_OP_UNB_ATTACK
		EsKeywordSet::newRegisterdAtom("unbarriering-attack"),				// S_GENEW_EN_OP_UNB_ATTACK
		EsKeywordSet::newRegisterdAtom("nuenchar-da"),						// S_GENEW_NS_OP_UNS_ATTACK
		EsKeywordSet::newRegisterdAtom("unsuffusing-attack"),				// S_GENEW_EN_OP_UNS_ATTACK
		EsKeywordSet::newRegisterdAtom("nof"),								// S_GENEW_NS_OP_NOP
		EsKeywordSet::newRegisterdAtom("nop"),								// S_GENEW_EN_OP_NOP
	};
	ASSERT(SoumaScriptKeywords::NUM_S == (sizeof(kwarrPrivate) / sizeof(const EsAtom*)));

	if((kwd < 0) || (NUM_S <= kwd))
	{
		ASSERT((0 <= kwd) && (kwd < NUM_S));
		return 0L;
	}

	return kwarrPrivate[kwd];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool SoumaScriptKeywords::init()
{
	for(int i = 0; i < NUM_S; i++)
	{
		EsKeywordSet::registerAtom(getKeyword(i));
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	一致判定
**//*---------------------------------------------------------------------*/
bool SoumaScriptKeywords::match(s32 kwd, const CStringBase* str, bool eqnoreCase)
{
	if(kwd < 0) { return false; }
	if(kwd >= NUM_S) { return false; }

	if(eqnoreCase)
	{
		for(int il = 0; il < NUM_LANG; il++)
		{
			if(getKeyword(kwd + il)->getString()->equalsIgnoreCase(str)) { return true; }
		}
	}
	else
	{
		for(int il = 0; il < NUM_LANG; il++)
		{
			if(getKeyword(kwd + il)->getString()->equals(str)) { return true; }
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
