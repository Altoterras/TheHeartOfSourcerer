/***********************************************************************//**
 *	MagicSys.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_MAGIC_SYS_H_
#define _SRCR_MAGIC_MAGIC_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExec.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

ETK_BEGIN_NS

class ParticleSys;

ETK_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class Game;
class MagicCluster;
class MagicClusterCreateParam;
class TransStat;

/*---------------------------------------------------------------------*//**
 *	マジック クラスター システム
 *		※	ややこしいが、ここでいうマジックはソウマそのものではない．
 *			マジックは、ある効果と動きと演出を伴う、
 *			パーティクルの上位なゲーム要素である．
 *			ソウマはもちろん主にマジックを生成するものと考えられる．
 *			しかし、敵の技やマップ上の仕掛けとしてマジックが出ることもある．
 *			マジックはクラスターという単位を持ち、
 *			クラスターが１つ以上のパーティクルを持つことが多い．
 *			クラスター自体が動きアルゴリズムと、
 *			効果パラメータ（TransStat 等）を持つ．
 *
**//*---------------------------------------------------------------------*/
class MagicSys : public FrameExec
{
	//======================================================================
	// 定数
protected:
	static const s32		NUM_CLUSTER_MAX	= 32;	// 最大表示クラスター数

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// クラスターを得る
	MagicCluster* getCluster(u16 mcid) const;
	// クラスターをインデックスから得る
	MagicCluster* getClusterFromIndex(s32 index) const;
	// クラスターの最大数を得る
	inline s32 getMaxClusterNum() const { return NUM_CLUSTER_MAX; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	u16 appearCluster(const MagicClusterCreateParam* mccprm);
	bool disappearCluster(u16 mcid, u16 dcflags);
	bool disappearAllCluster();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MagicSys();
	~MagicSys();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void notifyChangedMap();

private:
	s32 findUnusedClusterIndex() const;
	bool disappearClusterFromIndex(s32 index, u16 dcflags);

	//======================================================================
	// 変数
private:
	MagicCluster* _mcarr[NUM_CLUSTER_MAX];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_MAGIC_SYS_H_
