/***********************************************************************//**
 *	ParticleDefTbl.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/05/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_PARTICLE_PARTICLE_DEF_TBL_H_
#define _ETK_PARTICLE_PARTICLE_DEF_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ParticleDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	パーティクル定義テーブル
 *	
**//*---------------------------------------------------------------------*/
class ParticleDefTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_PTCLDEF_MAX	= 256;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	const ParticleDef* getDef(u16 ptcldid) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ParticleDefTbl();
	~ParticleDefTbl();
	bool init(u32 fileidData);
	void cleanup();

	//======================================================================
	// 変数
private:
	ParticleDef _ptcldefarr[NUM_PTCLDEF_MAX];
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_PARTICLE_PARTICLE_DEF_TBL_H_
