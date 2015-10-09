/***********************************************************************//**
 *	EmittedMc.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/04/03.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_IMPLEMENT_EMITTED_MC_H_
#define _SRCR_MAGIC_IMPLEMENT_EMITTED_MC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "ServedMc.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	放射態のクラスター
 *
**//*---------------------------------------------------------------------*/
class EmittedMc : public ServedMc
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EmittedMc();
	virtual bool create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm);
	virtual void exec(const ExecCtx* ec);			// フレーム制御

protected:
	void execExtinction(const ExecCtx* ec);
	///virtual bool collUnit(const ExecCtx* ec, Unit* unit);
	virtual void calcRadius();

	//======================================================================
	// 変数
private:
	f32 _radiusVel;			// 半径増減速度
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_IMPLEMENT_EMITTED_MC_H_
