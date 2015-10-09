/***********************************************************************//**
 *	QsMod.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QSMOD_QS_MOD_H_
#define _SRCR_QUEST_QSMOD_QS_MOD_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class StatusChangeResult;
class Game;
class QuestArticle;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	クエスト毎の専用処理モジュール
 *		※原則的には、可能な限りイベントで実装すべきだが、
 *		　イベントのみで実装が困難な場合はこのモジュールを実装する
 *			実装方法の検討は以下の順で考えるべきである
 *				(1) 既存の EventMatter のみで実装する
 *				　　↓
 *				(2) クエスト専用の QsEmp で実装する
 *				　　↓
 *				(3) 最終手段としてこのモジュールを派生して実装する
 *
**//*---------------------------------------------------------------------*/
class QsMod : public FrameExec
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool checkAchievementCustom() const;
	virtual bool handoverRequisitesCustom() { return false; }
	virtual void rewardCustom() {}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QsMod();
	virtual bool create(QuestArticle* qsaRef);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec) {}	// フレーム制御

	virtual bool begin();
	virtual bool end();

	virtual void notifyChangedMap(bool isDeserializeCase) {}
	virtual void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat) {}

	//======================================================================
	// 変数
protected:
	QuestArticle* _qstaRef;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_QUEST_QSMOD_QS_MOD_H_
