/***********************************************************************//**
 *	TrophyBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/08/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SAVE_TROPHY_BASE_H_
#define _SRCR_SAVE_TROPHY_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;
class PsnsBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	トロフィー
 *
**//*---------------------------------------------------------------------*/
class TrophyBase
{
	//======================================================================
	// 定数
public:
	static const u16	RATE_MAX				= 100;

	// トロフィーエントリー最大数
	static const u32	NUM_TROPHY_MAX			= 64;

private:
	// フラグ
	static const u8		F_CHANGED				= 0x01;	// 変更フラグ

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	トロフィーのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class TrophySaveStructure
	{
	public:
		// [  0]
		u8 _tprate[NUM_TROPHY_MAX];				// [ 64]
		u8 _tpflags[NUM_TROPHY_MAX];			// [ 64]
		u8 _padding0100[64];					// [ 64]
		// [192]
		static const s32 SIZE_THIS = 192;
		TrophySaveStructure() { TFW_ZERO_MEMORY(this, sizeof(TrophySaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// トロフィー達成率を得る
	u8 getTrophyAchievementRate(s32 itp) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void achievementTrophy(s32 itp, u8 rate);
	bool updatePsnsTrophy();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TrophyBase(PsnsBase* psnsRef);
	virtual ~TrophyBase() {}
	virtual bool create() { return true; }
	virtual void destroy() {}
	virtual bool serialize(Serializer* ser) const = 0;
	virtual bool deserialize(Serializer* ser) = 0;

	//======================================================================
	// 変数
protected:
	u8 _tprate[NUM_TROPHY_MAX];		// トロフィーカウンタ
	u8 _tpflags[NUM_TROPHY_MAX];	// トロフィーフラグ

private:
	PsnsBase* _psnsRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SAVE_TROPHY_BASE_H_
