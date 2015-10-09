/***********************************************************************//**
 *	ServedMc.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/17.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_IMPLEMENT_SERVED_MC_H_
#define _SRCR_MAGIC_IMPLEMENT_SERVED_MC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "BallMc.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	投げつける形態のクラスター
 *
**//*---------------------------------------------------------------------*/
class ServedMc : public BallMc
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setInitialVelocity(const Vector3F* vel);
	virtual void accelerate(const Vector3F* acc);
	virtual void reflectPosture();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ServedMc();
	virtual ~ServedMc();
	virtual bool create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm);
	virtual void destroy();
	virtual void exec(const ExecCtx* ec);			// フレーム制御

protected:
	void execExtinction(const ExecCtx* ec);
	bool appearParticles(const MagicClusterCreateParam* mccprm);
	virtual bool collUnit(const ExecCtx* ec, Unit* unit);
	f32 calcMulAlpha();
	void applyParticlePosture();

	//======================================================================
	// 変数
private:
	s32	_numParticle;		// 使用パーティクル数
	Vector3F* _posAfter;	// 残像位置
	f32 _accDefault;		// 基本加速値
	f32 _speedDefault;		// 基本速さ
	bool _leaveOwner;		// 唱者から離れたかどうかのフラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_IMPLEMENT_SERVED_MC_H_
