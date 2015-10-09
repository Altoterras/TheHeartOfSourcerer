/***********************************************************************//**
 *	PrpzlPerf_Enlight.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/11.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PUREPUZZLE_PRPZL_PERF_ENLIGHT_H_
#define _SRCR_PUREPUZZLE_PRPZL_PERF_ENLIGHT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "PrpzlPerfBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;

TFW_END_NS

SRCR_BEGIN_NS

class PurePuzzle;

/*---------------------------------------------------------------------*//**
 *	パズル演出
 *
**//*---------------------------------------------------------------------*/
class PrpzlPerf_Enlight : public PrpzlPerfBase
{
	//======================================================================
	// 定数
private:
	// セルのサイズ
	static const f32 CELL_XW;			// セルの横幅
	static const f32 CELL_YH;			// セルの縦幅

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PrpzlPerf_Enlight();
	~PrpzlPerf_Enlight();
	bool begin(PurePuzzle* pzlRef);
	void end();
	void exec(const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void draw(const RenderCtx* rc);

private:
	void renderSubBlock(f32 x, f32 y, f32 z, f32 w, f32 h, u8 r, u8 g, u8 b, u8 a);
	static void getEltColor(ColorU8* col, u8 elt, bool isHi);

private:
	void presetCells();

	//======================================================================
	// 変数
private:
	PurePuzzle* _pzlRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PUREPUZZLE_PRPZL_PERF_ENLIGHT_H_
