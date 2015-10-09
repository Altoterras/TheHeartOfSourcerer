/***********************************************************************//**
 *	TiledRectDraw.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/09/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *	
**//***********************************************************************/

#ifndef _TFW_GFW_TILED_RECT_DRAW_H_
#define _TFW_GFW_TILED_RECT_DRAW_H_

#include "../Base.h"

TFW_BEGIN_NS
	
////////////////////////////////////////////////////////////////////////////
// クラス

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
template<class TYPE> class Size;
typedef Size<f32> SizeF32;
class RenderCtx;
class Renderer;
class Texture;

/*---------------------------------------------------------------------*//**
 *	タイル矩形描画
 *
**//*---------------------------------------------------------------------*/
class TiledRectDraw
{
	//======================================================================
	// 定数
public:
	// タイル位置
	enum TilePlace
	{
		TP_LT,		// 左上端（非ループ）
		TP_CT,		// 上端（左右ループ）
		TP_RT,		// 右上端（非ループ）
		TP_LM,		// 左端（上下ループ）
		TP_CM,		// 中央（上下左右ループ）
		TP_RM,		// 右端（上下ループ）
		TP_LB,		// 左下端（非ループ）
		TP_CB,		// 下端（左右ループ）
		TP_RB,		// 右下端（非ループ）
		NUM_TP
	};

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	タイル情報
	 *
	**//*-----------------------------------------------------------------*/
	class TileInfo
	{
	public:
		RectF32* _uvwh;
		SizeF32* _size;
		u32 _rot;		// 表示回転フラグ TFW_DISPF_RANG_*
	public:
		TileInfo();
		~TileInfo();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 設定テクスチャを得る
	inline Texture* getTexture() const { return _texRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setTexture(Texture* texRef);
	void setTile(TilePlace tp, const RectF32* uvwh, f32 wDraw, f32 hDraw, u32 rot);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TiledRectDraw();
	virtual ~TiledRectDraw();

	bool create();
	bool create(Texture* texRef);
	void destroy();
	void draw(const RenderCtx* rc, const RectF32* rect);
	void draw(const RenderCtx* rc, f32 xRect, f32 yRect, f32 wRect, f32 hRect);

private:
	f32 drawLine(Renderer* rdr, TileInfo** tiLineArray, f32 x, f32 y, f32 xscale, f32 yscale, f32 xRect, f32 wRect);

	//======================================================================
	// 変数
private:
	Texture*			_texRef;				// テクスチャ
	TileInfo*			_ti[NUM_TP];			// タイル情報
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_GFW_TILED_RECT_DRAW_H_

