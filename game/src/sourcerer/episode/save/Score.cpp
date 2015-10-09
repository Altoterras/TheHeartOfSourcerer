/***********************************************************************//**
 *	Score.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Score.h"

// Friends
#include "SaveStructure.h"

// External
#include "../../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Score メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	スコアを計算する
**//*---------------------------------------------------------------------*/
u32 Score::calcScore()
{
	u32 score = 0;
	score += _scrcnt[SC_NEW_GAME_VOL01] * 100;			// Volume01 ニューゲームした
	score += _scrcnt[SC_RENEW_GAME_VOL01] * 1;			// Volume01 リニューゲームした
	score += _scrcnt[SC_CLEAR_VOL01] * 1000;			// Volume01 クリア
	score += _scrcnt[SC_CLEAR_STORY_EVENT] * 100;		// ストーリークリア
	score += _scrcnt[SC_CLEAR_QUEST] * 100;				// クエストクリア
	score += _scrcnt[SC_LOAD] * 1;						// ロードした数
	score += _scrcnt[SC_TALK_KABAL] * 100;				// カバルと会話した（１回目のみ）
	score += _scrcnt[SC_CONFRONT_WIN] * 1;				// 対決勝利
	score += _scrcnt[SC_DEFEAT_ENEMY_BY_WEAPON] * 1;	// ソウマによる対決勝利
	score += _scrcnt[SC_DEFEAT_ENEMY_BY_SOUMA] * 5;		// ソウマによる対決勝利
	score += _scrcnt[SC_EFF_ENEMY_HEAVY] * 1;			// 敵を倦怠化した
	score += _scrcnt[SC_EFF_ENEMY_SLEEP] * 1;			// 敵を眠らせた
	score += _scrcnt[SC_EFF_ENEMY_CONFU] * 1;			// 敵を錯乱させた
	score += _scrcnt[SC_EFF_PC_HENLIGHT] * 3;			// PC が半覚醒になった
	score += _scrcnt[SC_EFF_PC_CURE_DOWN] * 2;			// PC がダウンから回復させた
	score += _scrcnt[SC_EFF_PC_CURE_HEAVY] * 2;			// PC が倦怠から回復させた
	score += _scrcnt[SC_EFF_PC_CURE_SLEEP] * 2;			// PC が睡眠から回復させた
	score += _scrcnt[SC_EFF_PC_CURE_CONFU] * 2;			// PC が錯乱から回復させた
	return score;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Score::Score(PsnsBase* psnsRef)
	: ScoreBase(psnsRef)
{
	ASSERT(NUM_SC <= NUM_SCORE_MAX);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool Score::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(sizeof(ScoreSaveStructure) == ScoreSaveStructure::SIZE_THIS);

		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		TFW_COPY_MEMORY(svst->_scoresvst._scrcnt, _scrcnt, sizeof(u16) * NUM_SCORE_MAX);
	}
	else						// ダイレクトモード
	{
		ser->store(_scrcnt, sizeof(u16) * NUM_SCORE_MAX);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool Score::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(sizeof(ScoreSaveStructure) == ScoreSaveStructure::SIZE_THIS);

		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		TFW_COPY_MEMORY(_scrcnt, svst->_scoresvst._scrcnt, sizeof(u16) * NUM_SCORE_MAX);
	}
	else						// ダイレクトモード
	{
		ser->restore(_scrcnt, sizeof(u16) * NUM_SCORE_MAX);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
