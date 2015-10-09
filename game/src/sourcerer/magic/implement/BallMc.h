/***********************************************************************//**
 *	BallMc.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_IMPLEMENT_BALL_MC_H_
#define _SRCR_MAGIC_IMPLEMENT_BALL_MC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MagicCluster.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ボール
 *
**//*---------------------------------------------------------------------*/
class BallMc : public MagicCluster
{
	//======================================================================
	// 定数
protected:
	static const f32 RADIUS_MAX;
	static const f32 RATIO_RADIUS_TO_SCALE;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 半径を得る
	virtual f32 getRadius() const { return _radius; }
	// 見た目上の半径を得る
	virtual f32 getLookRadius() const { return _radiusPtcl; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void reflectPosture();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BallMc();
	virtual ~BallMc();
	virtual bool create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm);
	virtual void destroy();
	virtual void exec(const ExecCtx* ec);			// フレーム制御

protected:
	virtual void updateEleneFace(const ExecCtx* ec);
	void reflectColor();
	virtual void appearAtrbEffect(const ExecCtx* ec, u32 eeff, u16 ptcldid);
	virtual void calcRadius();
	virtual void calcParticleRadius();

	//======================================================================
	// 変数
protected:
	f32 _radius;		// 半径
	f32 _radiusPtcl;	// 表示ボール半径
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_IMPLEMENT_BALL_MC_H_
