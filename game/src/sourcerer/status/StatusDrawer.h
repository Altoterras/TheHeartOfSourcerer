/***********************************************************************//**
 *	StatusDrawer.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/16.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_STATUS_DRAWER_H_
#define _SRCR_STATUS_STATUS_DRAWER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Elene.h"

#include "../../tfw/lib/Color.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class ExecCtx;
class RenderCtx;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class Elene;
class EleneStat;

/*---------------------------------------------------------------------*//**
 *	ステータス描画
 *
**//*---------------------------------------------------------------------*/
class StatusDrawer
{
	//======================================================================
	// 定数
public:
	// 表示定数
	static const f32 W_GAUGE_DEFAULT;		// 標準のゲージ幅
	static const f32 H_GAUGE_DEFAULT;		// 標準のゲージ高さ
	static const f32 H_NAME_DEFAULT;		// 標準の名前表示分高さ
	static const f32 H_LFGAUGE_DEFAULT;		// 標準の生命ゲージ高さ
	static const f32 H_MPGAUGE_DEFAULT;		// 標準の精神ゲージ高さ
	static const f32 H_CONSGAUGE_DEFAULT;	// 標準の意識深度ゲージ高さ
	static const f32 H_CHARGEGAUGE_DEFAULT;	// 標準のためゲージ高さ
	static const f32 H_ENEGAUGE_DEFAULT;	// 標準の元素ゲージ高さ

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	元素エネルギー描画時のスケール情報
	 *
	**//*-----------------------------------------------------------------*/
	class DrawInfo
	{
	public:
		f32 _scaleEne;
		f32 _scaleCol;
		u8 _alphaH;
		u8 _alphaV;
		f32 _yOfs;
	public:
		DrawInfo() { reset(); }
		void set(f32 scaleEne, f32 scaleCol, u8 alphaH, u8 alphaV, f32 yOfs) { _scaleEne = scaleEne; _scaleCol = scaleCol; _alphaH = alphaH; _alphaV = alphaV; _yOfs = yOfs; }
		void reset() { _scaleEne = 0.0f; _scaleCol = 0.0f; _alphaH = 0; _alphaV = 0; _yOfs = 0.0f; }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static void init();
	static void exec(const ExecCtx* ec);

	// 基準色を得る
	inline static const ColorU8* getElementColor(Elene::ElKind el)
	{
		static const ColorU8 col[Elene::NUM_EL] =
		//	地								水								火								風								空
		{	ColorU8(191, 191,   0, 255),	ColorU8(  0, 127, 223, 255),	ColorU8(223, 127,   0, 255),	ColorU8(  0, 127,   0, 255),	ColorU8(191, 191, 180, 255),	};
		return &col[el];
	}

	// 基準色（明）を得る
	inline static const ColorU8* getElementColorLight(Elene::ElKind el)
	{
		static const ColorU8 col[Elene::NUM_EL] =
		//	地								水								火								風								空
		{	ColorU8(255, 255,   0, 255),	ColorU8(  0, 191, 255, 255),	ColorU8(255, 191,   0, 255),	ColorU8(  0, 191,   0, 255),	ColorU8(255, 255, 241, 255),	};
		return &col[el];
	}

	// 基準色（暗）を得る
	inline static const ColorU8* getElementColorDark(Elene::ElKind el)
	{
		static const ColorU8 col[Elene::NUM_EL] =
		//	地								水								火								風								空
		{	ColorU8(127, 127,   0, 255),	ColorU8(  0,  63, 191, 255),	ColorU8(191,  63,   0, 255),	ColorU8(  0,  63,   0, 255),	ColorU8(127, 127, 111, 255),	};
		return &col[el];
	}

	static void drawCharStat(const RenderCtx* rc, Texture* texGui, const CharStat* cstat, f32 x, f32 y, f32 w, f32 h, bool drawMpGauge, bool drawDifEleneNum);

	static void drawEleneStat(DrawInfo* di, const RenderCtx* rc, Texture* tex, const EleneStat* eestat, f32 x, f32 y, f32 w, f32 h, bool drawDifEleneNum);
	static void drawEleneStat(const RenderCtx* rc, Texture* tex, const EleneStat* eestat, f32 x, f32 y, f32 w, f32 h, bool drawDifEleneNum);

	static void drawElene(DrawInfo* di, const RenderCtx* rc, Texture* tex, const Elene* elene, f32 x, f32 y, f32 w, f32 h);
	static void drawElene(const RenderCtx* rc, Texture* tex, const Elene* elene, f32 x, f32 y, f32 w, f32 h);

	static void drawTinyNumLeftAlign(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, f32 scale, s32 num, bool sign);
	static void drawTinyNumRightAlign(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, f32 scale, s32 num, bool sign);

	static void calcEleneColorForParticle(ColorU8* col, const Elene* elene);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_STATUS_DRAWER_H_
