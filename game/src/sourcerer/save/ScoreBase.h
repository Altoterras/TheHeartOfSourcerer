/***********************************************************************//**
 *	ScoreBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/08/17.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SAVE_SCORE_BASE_H_
#define _SRCR_SAVE_SCORE_BASE_H_

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
 *	スコア
 *
**//*---------------------------------------------------------------------*/
class ScoreBase
{
	//======================================================================
	// 定数
public:
	static const u32	NUM_SCORE_MAX		= 32;

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	スコアのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class ScoreSaveStructure
	{
	public:
		// [  0]
		u16 _scrcnt[NUM_SCORE_MAX];			// [ 64] スコアカウンタ
		// [ 64]
		static const s32 SIZE_THIS = 64;
		ScoreSaveStructure() { TFW_ZERO_MEMORY(this, sizeof(ScoreSaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スコアカウントを得る
	u16 getScoreCount(s32 sc) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void upScore(s32 sc);
	bool updatePsnsScore();
	virtual u32 calcScore() = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ScoreBase(PsnsBase* psnsRef);
	virtual ~ScoreBase() {}
	virtual bool create() { return true; }
	virtual void destroy() {}
	virtual bool serialize(Serializer* ser) const = 0;
	virtual bool deserialize(Serializer* ser) = 0;

	//======================================================================
	// 変数
protected:
	u16 _scrcnt[NUM_SCORE_MAX];	// スコアカウンタ

private:
	PsnsBase* _psnsRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SAVE_SCORE_BASE_H_
