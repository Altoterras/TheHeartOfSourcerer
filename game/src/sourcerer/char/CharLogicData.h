/***********************************************************************//**
 *	CharLogicData.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/28.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CHAR_NPC_LOGIC_DATA_H_
#define _SRCR_CHAR_NPC_LOGIC_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../status/Elene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	NPC ロジックデータ
 *		※	本プロジェクトにおいて、データ構造においても struct でなく、
 *			class を使っているのは、将来的に機能を持たせる可能性がある
 *			点と Java への移植性を考えてのことである．
 *			逆に明示的に struct を使用しているものに関しては、データ構造
 *			に徹するべきであるという主張である．
 *													【2010/06/29 r-kishi】
**//*---------------------------------------------------------------------*/
class CharLogicData
{
	//======================================================================
	// 定数
public:
	enum Param
	{
		PARAM_DEFAULT_ATTACK_BODY_RATE,
		PARAM_NORMAL_REACH,
		PARAM_DETECT_RANGE,
		PARAM_BATTLE_RANGE,
		PARAM_EXP,
		PARAM_DROP_ITEM_ID,
		PARAM_DROP_ITEM_PROBABILITY,
		PARAM_EC_FLAGS,
		PARAM_NEGATIVE_ATRB,
		PARAM_P10,
		PARAM_P11,
		NUM_PARAM
	};

	//======================================================================
	// 定数
public:
	// 敵キャラフラグ（EC フラグ）
	static const u16	ECF_NO_DOWN_VANISH		= 0x0001;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// キャラクタ定義 ID を得る
	inline u16 getCharDefId() const { return _chrdid; }
	// 技確率（0 ～ 128）を得る
	inline u16 getSkillProbability() const { return _probSkill; }
	// 技範囲を得る
	inline f32 getSkillRange() const { return _rangeSkill; }
	// 技五元素原質値を得る
	inline const Elene* getSkillOqlElene() const { return &_eleneOqlSkill; }
	// 技五元素現在値を得る
	inline const Elene* getSkillCurElene() const { return &_eleneCqlSkill; }
	// 汎用パラメータを得る
	inline u16 getParam(Param p) const { ASSERT((0 <= p) && (p < NUM_PARAM)); return _params[p]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharLogicData();
	bool loadFromDataFile(FileBase* file);

	//======================================================================
	// 変数
private:
	u16 _chrdid;				// キャラクタ定義 ID
	u8 _probSkill;				// 技確率
	f32 _rangeSkill;			// 技パラメータ
	Elene _eleneOqlSkill;		// 技五元素原質値
	Elene _eleneCqlSkill;		// 技五元素現在値
	u16 _params[NUM_PARAM];	// 汎用パラメータ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CHAR_NPC_LOGIC_DATA_H_
