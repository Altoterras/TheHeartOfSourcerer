/***********************************************************************//**
 *  FrameExec.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FRAMEMOD_FRAME_EXEC_H_
#define _TFW_FRAMEMOD_FRAME_EXEC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

// # 前方参照で定義しても良いが、各所で面倒なのでインクルードしておく
#include "ExecCtx.h"
#include "ExecRes.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	フレーム制御モジュールの基底クラス
 *
**//*---------------------------------------------------------------------*/
TFW_INTERFACE FrameExec
{
public:
	// フレーム制御
	virtual void exec(ExecRes* res, const ExecCtx* ec) = 0;
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FRAMEMOD_FRAME_EXEC_H_
