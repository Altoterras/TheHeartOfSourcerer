/***********************************************************************//**
 *	EdevFixedStrTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/19.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _EDEV_BODY_EDEV_FIXED_STR_TBL_H_
#define _EDEV_BODY_EDEV_FIXED_STR_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EdevFixedData.h"

#include "../../tfw/lib/TypeUtils.h"

////////////////////////////////////////////////////////////////////////////
// クラス

EDEV_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定システム文字列テーブル
 *		※	FixedStringTable.ods の定義
 *
**//*---------------------------------------------------------------------*/
class EdevFixedStrTbl
{
	//======================================================================
	// 定数
public:
	enum
	{
		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// 汎用

		SID_YES									= 101,
		SID_NO									= 102,

		SID_SOIL								= 1001,
		SID_WATER								= 1002,
		SID_FIRE								= 1003,
		SID_WIND								= 1004,
		SID_AETHEL								= 1005,
		SID_EL_PLUS								= 1011,

		SID_CHARPARAM_UNI_FULL					= 1021,
		SID_CHARPARAM_ACT_FULL					= 1022,
		SID_CHARPARAM_STB_FULL					= 1023,
		SID_CHARPARAM_LV_FULL					= 1024,
		SID_CHARPARAM_EXP_FULL					= 1025,
		SID_CHARPARAM_ENE_FULL					= 1026,
		SID_CHARPARAM_MAXENE_FULL				= 1027,
		SID_CHARPARAM_ENEDMG_FULL				= 1028,
		SID_CHARPARAM_PHSDMG_FULL				= 1029,
		SID_CHARPARAM_UNI_SHORT					= 1041,
		SID_CHARPARAM_ACT_SHORT					= 1042,
		SID_CHARPARAM_STB_SHORT					= 1043,
		SID_CHARPARAM_LV_SHORT					= 1044,
		SID_CHARPARAM_EXP_SHORT					= 1045,

		SID_EQUIPPLACE_LHAND					= 1101,
		SID_EQUIPPLACE_RHAND					= 1102,
		SID_EQUIPPLACE_ACCSR					= 1103,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// 戦闘

		SID_ENTER_BATTLE						= 2001,
		SID_ENTER_BATTLE_PLURAL					= 2002,
		SID_WIN_BATTLE							= 2011,
		SID_WIN_BATTLE_PLURAL					= 2012,

		SID_ATTACK_FIRST						= 2101,
		SID_OUTSOUMA_FIRST						= 2102,
		SID_SKILL_FIRST							= 2103,
		SID_USE_FIRST							= 2104,
		SID_CANCEL_BEHV							= 2111,

		SID_ENE_DAMAGE							= 2201,
		SID_ENE_DAMAGE_POINTS					= 2202,
		SID_PHYS_INJURY							= 2203,
		SID_PHYS_INJURY_POINTS					= 2204,
		SID_ENE_RECOVER							= 2205,
		SID_ENE_RECOVER_POINTS					= 2206,
		SID_PHYS_RECOVER						= 2207,
		SID_PHYS_RECOVER_POINTS					= 2208,
		SID_NO_DAMAGE							= 2209,

		SID_NO_EFFECTIVE						= 2211,
		SID_MISS_ATTACK							= 2212,

		SID_BECOME_DOWN							= 2301,
		SID_BECOME_ALIVE						= 2302,
		SID_BECOME_ENEMY_CLEARED				= 2303,
		SID_BECOME_HEAVY						= 2311,
		SID_WELL_HEAVY							= 2312,
		SID_BECOME_SLEEP						= 2321,
		SID_WELL_SLEEP							= 2322,
		SID_BECOME_CONFU						= 2323,
		SID_WELL_CONFU							= 2324,
		SID_BECOME_TRANS						= 2325,
		SID_OUT_TRANS							= 2326,

		SID_START_GUARD							= 2401,
		SID_START_DANCING						= 2402,
		SID_START_CHARGE						= 2403,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// フィールド

		SID_NOT_FOUND							= 3001,
		SID_GET_SOMETHING						= 3002,
		SID_GET_SOURCE							= 3003,
		SID_ALREADY_HAS_SOURCE					= 3004,

		SID_NOT_HAPPENED						= 3011,

		SID_ERROR_ITEM_FULL						= 3501,
		SID_ERROR_SOURCE_FULL					= 3502,
		SID_ERROR_ERROR_ITEM_THROW				= 3503,

		SID_GET_EXPERIENCE						= 3801,
		SID_GET_EXPERIENCE_EACH					= 3802,
		SID_GET_EXP_CHAIN						= 3803,
		SID_LV_UP								= 3805,
		SID_CHARPARAM_UP_ENERGY					= 3811,
		SID_CHARPARAM_UP_UNI					= 3812,
		SID_CHARPARAM_UP_ACT					= 3813,
		SID_CHARPARAM_UP_STB					= 3814,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// 技

		SID_ISITSUBUTE							= 4101,
		SID_HORN								= 4102,
		SID_SHOCKWAVE							= 4103,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// イベント

		SID_ADD_ONE_FREE_PAGE					= 5101,
		SID_ADD_PLURAL_FREE_PAGES				= 5102,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// ソウマ

		SID_SERVE_SOUMA							= 6001,
		SID_CHANTING_START						= 6002,
		SID_CHANTING_BREAK						= 6003,

		SID_APPEAR_WRAPPER						= 6101,
		SID_DISAPPEAR_WRAPPER					= 6102,
		SID_STREN_WRAPPER						= 6105,
		SID_WEAKN_WRAPPER						= 6106,
		SID_APPEAR_BARIEER						= 6111,
		SID_DISAPPEAR_BARIEER					= 6112,
		SID_STREN_BARIEER						= 6115,
		SID_WEAKN_BARIEER						= 6116,

		SID_SOUMA_ERROR_GAME					= 6300,

		SID_SOUMA_ERROR_LOCAL					= 6500,

		SID_SOUMA_ERROR_SCRIPT					= 6700,
		SID_SOUMA_ESCR_SOURCE_ERROR				= 6701,
		SID_SOUMA_ESCR_SOURCE_ERROR_MSG			= 6702,
		SID_SOUMA_ESCR_SYNTAX_ERROR				= 6703,

		SID_SOUMA_SCRIPT_ERROR_MSG				= 6901,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// クエスト

		SID_QUEST_CONFIRM_CONTRACT				= 7001,
		SID_QUEST_CONFIRM_CANCEL				= 7002,

		SID_QUEST_CONFIRM_HANDOVER				= 7011,

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// メニュー・インターフェイス系

		SID_GUI_BACK							= 8001,		// 戻る
		SID_GUI_ADVANCE							= 8002,		// 進む
		SID_GUI_CLOSE							= 8003,		// 閉じる
		SID_GUI_OPEN							= 8004,		// 開く
		SID_GUI_EDIT							= 8005,		// 編集
		SID_GUI_LOOK							= 8006,		// 見る

		SID_GUI_MENU							= 8011,
		
		SID_GUI_CONFIRM_CLONE					= 8021,
		SID_GUI_CONFIRM_DELETE					= 8022,
		SID_GUI_CONFIRM_REFLECT_CHANGES			= 8023,

		SID_GUI_CONFIRM_CONV_TO_JS				= 8031,
		SID_GUI_CONFIRM_CONV_TO_SS				= 8032,

		SID_GUI_CONFIRM_SKIP					= 8051,
		SID_GUI_CONFIRM_SAVE					= 8052,

		SID_GUI_MENU_SOURCEBOOK					= 8101,
		SID_GUI_MENU_SOURCE_EDIT				= 8102,
		SID_GUI_MENU_SOURCE_REQ_OQL				= 8121,
		SID_GUI_MENU_SOURCE_FREE_SLOT			= 8131,

		SID_GUI_MENU_ITEM_LIST					= 8201,

		SID_GUI_MENU_CHAR_STAT					= 8301,

		SID_GUI_MENU_EQUIP						= 8401,
		SID_GUI_EQUIP_NOTHING					= 8411,		// （なし）
		SID_GUI_EQUIP_CHANGE					= 8421,		// 変更

		SID_GUI_MENU_ABILITY					= 8501,
		SID_GUI_ABILITY_CREATABLE_OQL			= 8511,
		SID_GUI_ABILITY_EFFECTIVE_ENT			= 8512,

		SID_GUI_MENU_QUEST						= 8601,
		SID_GUI_MENU_QUEST_NAME					= 8611,
		SID_GUI_MENU_QUEST_REQUISITE			= 8612,
		SID_GUI_MENU_QUEST_REWARD				= 8613,
		SID_GUI_MENU_QUEST_CLIENT				= 8614,
		SID_GUI_MENU_QUEST_DIFFICULTY			= 8615,
		SID_GUI_MENU_QUEST_ACCEPTED				= 8621,
		SID_GUI_MENU_QUEST_ACHIEVED				= 8622,
		SID_GUI_MENU_QUEST_FINISHED				= 8623,
		SID_GUI_MENU_QUEST_CANCELED				= 8624,
		SID_GUI_MENU_QUEST_NO_DATA				= 8631,

		SID_GUI_MENU_SAVE						= 8701,
		SID_GUI_SAVE_FILE_1_AND_TIME			= 8711,
		SID_GUI_SAVE_FILE_2_AND_TIME			= 8712,
		SID_GUI_SAVE_FILE_1_TIME_AND_SCORE		= 8713,
		SID_GUI_SAVE_FILE_2_TIME_AND_SCORE		= 8714,
		SID_GUI_SAVE_DONE						= 8721,
		SID_GUI_CURRENT_PLAY_TIME				= 8731,

		SID_GUI_TITLE_CONFIRM_TRANS_SAVES_TO	= 8801,
		SID_GUI_TITLE_CONFIRM_TRANS_SAVES_FROM	= 8802,
		SID_GUI_TITLE_TRANS_SAVES_DONE			= 8803,
		SID_GUI_TITLE_REQUIRE_PAID_VER_111		= 8804,
		SID_GUI_TITLE_TRANS_SAVES_FAILED		= 8805,

		SID_GUI_MENU_OPTIONS					= 8901,
		SID_GUI_MENU_OPT_DEFAULT_SOUMA_LANG		= 8911,
		SID_GUI_MENU_CONFIRM_CHANGE_SOUMA_LANG	= 8912,
		SID_GUI_MENU_OPTFLAGS_SS				= 8913,
		SID_GUI_MENU_OPTFLAGS_JS				= 8914,
		SID_GUI_MENU_OPT_CHANT_WINDOW_MODE		= 8921,
		SID_GUI_MENU_OPTFLAGS_SPELL				= 8922,
		SID_GUI_MENU_OPTFLAGS_RAW				= 8923,

		SID_GUI_NEXT_STORY_EVENT				= 9101,
		SID_GUI_NEXT_SUB_EVENT					= 9102,
		SID_GUI_NEXT_QUEST_START_EVENT			= 9103,
		SID_GUI_NEXT_QUEST_END_EVENT			= 9104,
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	// 文字列を得る
	static inline const CvString* getString(u16 strid)
	{
		#if defined(_DEBUG)
			const StringTable* strtblSys = EdevFixedData::getFixedSystemStringTable();
			if(strtblSys == 0)
			{
				return TypeUtils::getEmptyString();
			}
			const CvString* str = strtblSys->getString(strid);
			if(str == 0)
			{
				return TypeUtils::getEmptyString();
			}
			return str;
		#else
			return FixedData::getFixedSystemStringTable()->getString(strid);
		#endif
	}
};

EDEV_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _EDEV_BODY_EDEV_FIXED_STR_TBL_H_
