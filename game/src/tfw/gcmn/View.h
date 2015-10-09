/***********************************************************************//**
 *	View.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GCMN_VIEW_H_
#define _TFW_GCMN_VIEW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Matrix4F;
template<class TYPE> class Point;
typedef Point<f32> PointF32;

/*---------------------------------------------------------------------*//**
 *	ビュー
 *	
**//*---------------------------------------------------------------------*/
class View
{
	//======================================================================
	// 定数
public:
	enum ViewMode
	{
		VMODE_VAR,			// 可変: 実解像度のアスペクト比に準拠
		VMODE_FIX,			// 固定: 固定アスペクト比で余った部分は黒枠表示する
		VMODE_SCALE,		// スケール: 実解像度にスケーリングする（アスペクト非維持）
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のビューモードを得る
	inline ViewMode getViewMode() const { return _vmode; }
	// 現在の表示フラグ（TFW_DISPF_*）を得る
	inline u32 getDisplayFlags() const { return _dispflags; }
	// 現在の回転を得る
	inline u32 getRotZ() const { return _dispflags & TFW_DISPF_RANG_MASK; }
///	// ワイド画面設定かを得る
///	inline bool isWideScreen() const { return TFW_IS_FLAG(_dispflags, TFW_DISPF_WIDE); }
	// 現在の横幅を得る
	inline f32 getWidth() const { return _widthCur; }
	// 現在の縦幅を得る
	inline f32 getHeight() const { return _heightCur; }
	// 現在のアスペクト比を得る
	inline f32 getAspect() const { return _aspectCur; }
	// 現在のアスペクト比の逆数を得る
	inline f32 getInvAspect() const { return _aspectCurInv; }
	// 論理横幅を得る
	inline f32 getLogicalWidth() const { return _widthLgc; }
	// 論理縦幅を得る
	inline f32 getLogicalHeight() const { return _heightLgc; }
	// 論理アスペクト比を得る
	inline f32 getLogicalAspect() const { return _acpectLgc; }
	// 論理アスペクト比の逆数を得る
	inline f32 getInvLogicalAspect() const { return _acpectLgcInv; }
	// 論理横幅に対する比を得る
	inline f32 getLogicalScaleX() const { return _xscaleLgc; }
	// 論理縦幅に対する比を得る
	inline f32 getLogicalScaleY() const { return _yscaleLgc; }
	// 論理横幅に対する逆比を得る
	inline f32 getInvLogicalScaleX() const { return _xscaleLgcInv; }
	// 論理縦幅に対する逆比を得る
	inline f32 getInvLogicalScaleY() const { return _yscaleLgcInv; }
	// スクリーンに対する座標スケールを得る
	inline f32 getCoordinatesScale() const { return _scaleCoord; }
	// スクリーンに対する座標スケールの逆数を得る
	inline f32 getInvCoordinatesScale() const { return _scaleCoordInv; }
	// パースペクティブ行列を得る
	inline const Matrix4F* getPerspectiveMatrix() const { return _mtxPerspect; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setViewMode(ViewMode vmode, bool setViewport);
	void setDisplayFlags(u32 dispflags, bool setViewport);
	void setRealSize(f32 width, f32 height, bool setViewport);
	void setLogicalSize(f32 width, f32 height, bool setViewport);
	void setCoordinatesScale(f32 scaleCoord);
	
	void convRealToLogicalPoint(PointF32* ptLogical, const PointF32* ptReal) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	View();
	virtual ~View();
	virtual bool create();
	void destroy();

	void apply2dDrawSetting();
	const Matrix4F* calcPerspectiveMatrix(f32 fovy, f32 cnear, f32 cfar);

private:
	void applyViewport();
	void calcScaleToLogicalSize();
	
	//======================================================================
	// 変数
private:
	ViewMode		_vmode;				// ビューモード
	u32				_dispflags;			// 画面表示フラグ
	f32				_xLeft;				// ビューポート左辺
	f32				_yTop;				// ビューポート上辺
	f32				_xRight;			// ビューポート右辺
	f32				_yBottom;			// ビューポート下辺
	f32				_scaleCoord;		// 座標スケール
	f32				_scaleCoordInv;		// 座標スケールの逆数
	f32				_widthCur;			// 現在の横幅
	f32				_heightCur;			// 現時の縦幅
	f32				_aspectCur;			// 現在のアスペクト比
	f32				_aspectCurInv;		// 現在のアスペクト比の逆数

	f32				_widthLgc;			// 論理横幅
	f32				_heightLgc;			// 論理縦幅
	f32				_acpectLgc;			// 論理アクペクト比
	f32				_acpectLgcInv;		// 論理アクペクト比の逆数
	f32				_xscaleLgc;			// 論理サイズに対するスケール X
	f32				_yscaleLgc;			// 論理サイズに対するスケール Y
	f32				_xscaleLgcInv;		// 論理サイズに対する逆スケール X
	f32				_yscaleLgcInv;		// 論理サイズに対する逆スケール Y

	Matrix4F*		_mtxPerspect;		// パースペクティブ行列
	bool			_isChPerspect;		// パースペクティブが変更され、再計算する必要がある
	f32				_fovyCache;			// パースペクティブ再計算の必要性検討用 fovy
	f32				_cnearCache;		// パースペクティブ再計算の必要性検討用 near クリップ値
	f32				_cfarCache;			// パースペクティブ再計算の必要性検討用 far クリップ値
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_VIEW_H_

