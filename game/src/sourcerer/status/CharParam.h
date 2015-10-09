/***********************************************************************//**
 *	CharParam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_CHAR_PARAM_H_
#define _SRCR_STATUS_CHAR_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class RenderCtx;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	キャラクター パラメータ
 *
**//*---------------------------------------------------------------------*/
class CharParam
{
	//======================================================================
	// 定数
public:
	static const u16 VALUE_DEFAULT	= 100;
	static const u16 VALUE_MAX		= 10000;

	static const u16 EXP_MAX_VOL01	= 8888;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// エネルギー基本値を得る
	inline s32 getEnergy() const { return _energy; }
	// 集中・統一力を得る
	inline u16 getUnity() const { return _unity; }
	// 活動・変換力を得る
	inline u16 getActivity() const { return _activity; }
	// 安定・持続力を得る
	inline u16 getStability() const { return _stability; }
	// 経験値を得る
	inline u16 getExperience() const { return _experience; }
	// レベルを得る
	inline u16 getLevel() const { return _level; }

	// 経験値を設定する
	inline void setExperience(u16 exp) { _experience = exp; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void levelup(const CharParam* cprmNext);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharParam()						{	clear();	}
	CharParam(const CharParam& src)	{	copy(&src);	}

	void copy(const CharParam* src)
	{
		_level = src->_level;
		_experience = src->_experience;
		_energy = src->_energy;
		_unity = src->_unity;
		_activity = src->_activity;
		_stability = src->_stability;
	}
	CharParam& operator =(const CharParam& src) { copy(&src); return *this; }

	void set(u16 level, u16 exp, s32 lfenergy, u16 unity, u16 activity, u16 stability)
	{
		_level = level;
		_experience = exp;
		_energy = lfenergy;
		_unity = unity;
		_activity = activity;
		_stability = stability;
	}

	void clear()
	{
		ASSERT(sizeof(CharParam) == SIZE_THIS);
		_level = 0;
		_experience = 0;
		_energy = 0;
		_unity = VALUE_DEFAULT;
		_activity = VALUE_DEFAULT;
		_stability = VALUE_DEFAULT;
	}

	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

	//======================================================================
	// 変数
private:
	// [  0]
	u16 _level;				// レベル
	u16 _experience;		// 経験値
	u16 _unity;				// 集中・統一力（サットヴァ制御力）
	u16 _activity;			// 活動・変換力（ラジャス制御力）
	u16 _stability;			// 安定・持続度（タマス制御力）
	u8 __pad_0010[2];
	s32 _energy;			// エネルギー基本値
	// [ 16]
	static const u32 SIZE_THIS = 16;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_CHAR_PARAM_H_
