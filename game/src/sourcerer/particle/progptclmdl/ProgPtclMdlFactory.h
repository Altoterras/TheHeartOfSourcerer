/***********************************************************************//**
 *	ProgPtclMdlFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/11.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PARTICLE_PROGPTCLMDL_PROG_PTCL_MDL_FACTORY_H_
#define _SRCR_PARTICLE_PROGPTCLMDL_PROG_PTCL_MDL_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	プログラム制御パーティクル モデル ファクトリー
 *	
**//*---------------------------------------------------------------------*/
class ProgPtclMdlFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static Model* makeProgPrtlModel(s32 pmdlid);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PARTICLE_PROGPTCLMDL_PROG_PTCL_MDL_FACTORY_H_
