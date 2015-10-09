/***********************************************************************//**
 *  HrAnimCtx.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_CTX_H_
#define _TFW_G3D_HR_ANIM_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	階層アニメーション 可変値コンテキスト
 *
**//*---------------------------------------------------------------------*/
class HrAnimCtx
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimCtx();
	~HrAnimCtx();
	bool create();
	void destroy();

	//======================================================================
	// メンバ変数
public:
	f32						_frame;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_CTX_H_