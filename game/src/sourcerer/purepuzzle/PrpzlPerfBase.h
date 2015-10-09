/***********************************************************************//**
 *	PrpzlPerfBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/11.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PUREPUZZLE_PRPZL_PERF_H_
#define _SRCR_PUREPUZZLE_PRPZL_PERF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
class Camera;

TFW_END_NS

SRCR_BEGIN_NS

class PurePuzzle;

/*---------------------------------------------------------------------*//**
 *	パズル演出
 *
**//*---------------------------------------------------------------------*/
class PrpzlPerfBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual ~PrpzlPerfBase() {}
	virtual bool begin(PurePuzzle* pzlRef) = 0;
	virtual void end() = 0;
	virtual void exec(const ExecCtx* ec) {}
	virtual void render(const RenderCtx* rc) {}
	virtual void draw(const RenderCtx* rc) {}
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PUREPUZZLE_PRPZL_PERF_H_
