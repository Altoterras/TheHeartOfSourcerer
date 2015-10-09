/***********************************************************************//**
 *	ExplosionMc.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/17.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_IMPLEMENT_EXPLOSION_MC_H_
#define _SRCR_MAGIC_IMPLEMENT_EXPLOSION_MC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MagicCluster.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	爆発クラスター
 *
**//*---------------------------------------------------------------------*/
class ExplosionMc : public MagicCluster
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 半径を得る
	f32 getRadius() const { return _radiusCur; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ExplosionMc();
	~ExplosionMc();
	bool create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm);
	void destroy();
	void exec(const ExecCtx* ec);			// フレーム制御

protected:
	void execExtinction(const ExecCtx* ec);
	bool appearParticles(const MagicClusterCreateParam* mccprm);

	//======================================================================
	// 変数
private:
	f32 _radiusCur;			// 現在の半径
	f32 _radiusMax;			// 最大半径
	f32 _radiusDelta;		// １フレームあたりの増加半径
	List<Unit*>* _listTrg;	// ターゲットリスト
	bool _isHitUnit;		// ユニットにヒットしたかどうか
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_IMPLEMENT_EXPLOSION_MC_H_
