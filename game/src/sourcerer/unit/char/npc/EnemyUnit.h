/***********************************************************************//**
 *	EnemyUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_NPC_ENEMY_UNIT_H_
#define _SRCR_UNIT_CHAR_NPC_ENEMY_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "NpcCharUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;

TFW_END_NS

SRCR_BEGIN_NS

class EnemySpirit;

/*---------------------------------------------------------------------*//**
 *	ヘルパーユニット
 *
**//*---------------------------------------------------------------------*/
class EnemyUnit : public NpcCharUnit
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 自らの（型の）スピリットを得る
	inline EnemySpirit* getThisSpirit() const { return (EnemySpirit*)getSpirit(); }
	// ダウンフレームカウンタ値を得る
	f32 getDownFrameCounter() const { return _fcntDown; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setDetectTarget(Unit* unitTrg);
	void calcTargetPosition();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EnemyUnit();
	virtual ~EnemyUnit();
	virtual bool create(Spirit* sprtRef, u16 chrdid);
//	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	virtual void execWalking(const ExecCtx* ec);
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	virtual void notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat);

protected:

	//======================================================================
	// 変数
protected:

private:
	s32 _cntDetect;				// 対象検出間引きカウンタ
	f32 _fcntDown;				// ダウンフレームカウンタ
	s32 _fcntCharOverlapAvoid;	// キャラ重なり回避カウンタ
	f32 _angleCharOverlapAvoid;	// キャラ重なり回避角度
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_NPC_ENEMY_UNIT_H_
