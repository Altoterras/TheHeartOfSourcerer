/***********************************************************************//**
 *	Score.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_SAVE_SCORE_H_
#define _SRCR_EPISODE_SAVE_SCORE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../save/ScoreBase.h"

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
class Score : public ScoreBase
{
	//======================================================================
	// 定数
public:
	enum ScoreCount
	{
		SC_NULL,					// (  0) 無効
		SC_NEW_GAME_VOL01,			// (  1) Volume01 ニューゲームした
		SC_RENEW_GAME_VOL01,		// (  2) Volume01 リニューゲームした
		SC_CLEAR_VOL01,				// (  3) Volume01 クリア
		SC_CLEAR_STORY_EVENT,		// (  4) ストーリークリア
		SC_CLEAR_QUEST,				// (  5) クエストクリア
		SC_LOAD,					// (  6) ロードした数
		SC_TALK_KABAL,				// (  7) カバルと会話した（１回目のみ）
		SC_CONFRONT_WIN,			// (  8) 対決勝利
		SC_DEFEAT_ENEMY_BY_WEAPON,	// (  9) 武器によって敵を倒した
		SC_DEFEAT_ENEMY_BY_SOUMA,	// ( 10) ソウマによって敵を倒した
		SC_EFF_ENEMY_HEAVY,			// ( 11) 敵を倦怠化した
		SC_EFF_ENEMY_SLEEP,			// ( 12) 敵を眠らせた
		SC_EFF_ENEMY_CONFU,			// ( 13) 敵を錯乱させた
		SC_EFF_PC_HENLIGHT,			// ( 14) PC が半覚醒になった
		SC_EFF_PC_CURE_DOWN,		// ( 15) PC がダウンから回復させた
		SC_EFF_PC_CURE_HEAVY,		// ( 16) PC が倦怠から回復させた
		SC_EFF_PC_CURE_SLEEP,		// ( 17) PC が睡眠から回復させた
		SC_EFF_PC_CURE_CONFU,		// ( 18) PC が錯乱から回復させた
		NUM_SC						// ( 19)
	};

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	u32 calcScore();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Score(PsnsBase* psnsRef);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

	//======================================================================
	// 変数
public:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_SAVE_SCORE_H_
