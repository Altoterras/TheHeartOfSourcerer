/***********************************************************************//**
 *	CreationMc.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/17.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_IMPLEMENT_CREATION_MC_H_
#define _SRCR_MAGIC_IMPLEMENT_CREATION_MC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "BallMc.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	創作中のクラスター
 *
**//*---------------------------------------------------------------------*/
class CreationMc : public BallMc
{
	//======================================================================
	// 定数
private:
	static const s32 PTCLIDX_NQL	= 0;
	static const s32 PTCLIDX_CQL	= 1;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm);
	void exec(const ExecCtx* ec);			// フレーム制御

protected:
	bool appearParticles(const MagicClusterCreateParam* mccprm);

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_IMPLEMENT_CREATION_MC_H_
