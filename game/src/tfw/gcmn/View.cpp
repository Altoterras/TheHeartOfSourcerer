/***********************************************************************//**
 *  View.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "View.h"

// Friends
#include "../gcmn/Gcalc.h"
#include "../lib/Matrix.h"
#include "../lib/Point.h"
#include "../lib/Matrix.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////

//==========================================================================
// View メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ビューモードを設定
**//*---------------------------------------------------------------------*/
void View::setViewMode(ViewMode vmode, bool setViewport)
{
	_vmode = vmode;

	if (setViewport) { applyViewport(); }
}

/*---------------------------------------------------------------------*//**
	ビューモードを設定
**//*---------------------------------------------------------------------*/
void View::setDisplayFlags(u32 dispflags, bool setViewport)
{
	_dispflags = dispflags;

	if (setViewport) { applyViewport(); }
}

/*---------------------------------------------------------------------*//**
	実解像度を設定
**//*---------------------------------------------------------------------*/
void View::setRealSize(f32 width, f32 height, bool setViewport)
{
	_widthCur = width;
	_heightCur = height;
	_aspectCur = _widthCur / _heightCur;
	_aspectCurInv = 1.0f / _aspectCur;

	// 論理サイズが未設定の場合は設定する
	if ((_widthLgc == 0) || (_heightLgc == 0))	// 論理サイズが未設定
	{
		_widthLgc = width;
		_heightLgc = height;
	}

	if (setViewport) { applyViewport(); }
}

/*---------------------------------------------------------------------*//**
	論理解像度を設定
**//*---------------------------------------------------------------------*/
void View::setLogicalSize(f32 width, f32 height, bool setViewport)
{
	// 論理サイズ設定
	_widthLgc = width;
	_heightLgc = height;
	_acpectLgc = _widthLgc / _heightLgc;
	_acpectLgcInv = 1.0f / _acpectLgc;

	// 実解像度が未設定の場合は設定する
	if ((_widthCur == 0) || (_heightCur == 0))	// ビューポートの大きさが未設定
	{
		_widthCur = width;
		_heightCur = height;
	}

	if (setViewport) { applyViewport(); }
}

/*---------------------------------------------------------------------*//**
	スクリーンの表示物スケールの設定
**//*---------------------------------------------------------------------*/
void View::setCoordinatesScale(f32 scaleCoord)
{
	_scaleCoord = scaleCoord;
	_scaleCoordInv = 1.0f / scaleCoord;
}

/*---------------------------------------------------------------------*//**
	実座標を論理 座標に変換する
**//*---------------------------------------------------------------------*/
void View::convRealToLogicalPoint(PointF32* ptLogical, const PointF32* ptReal) const
{
	switch(getRotZ())
	{
	case TFW_DISPF_RANG_0:
		ptLogical->set(
			(             (ptReal->x() * _scaleCoord) - _xLeft) * _xscaleLgcInv,
			(             (ptReal->y() * _scaleCoord) - _yTop ) * _yscaleLgcInv);
		break;
	case TFW_DISPF_RANG_90:
		ptLogical->set(
			(             (ptReal->y() * _scaleCoord) - _xLeft) * _xscaleLgcInv,
			(_heightCur - (ptReal->x() * _scaleCoord) - _yTop ) * _yscaleLgcInv);
		break;
	case TFW_DISPF_RANG_180:
		ptLogical->set(
			(_widthCur  - (ptReal->x() * _scaleCoord) - _xLeft) * _xscaleLgcInv,
			(_heightCur - (ptReal->y() * _scaleCoord) - _yTop ) * _yscaleLgcInv);
		break;
	case TFW_DISPF_RANG_270:
		ptLogical->set(
			(_widthCur  - (ptReal->y() * _scaleCoord) - _xLeft) * _xscaleLgcInv,
			(             (ptReal->x() * _scaleCoord) - _yTop ) * _yscaleLgcInv);
		break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
View::View()
	: _vmode(VMODE_VAR)
	, _dispflags(0)
	, _xLeft(0)
	, _yTop(0)
	, _xRight(0)
	, _yBottom(0)
	, _scaleCoord(0.0f)
	, _scaleCoordInv(0.0f)
	, _widthCur(0)
	, _heightCur(0)
	, _aspectCur(0.0f)
	, _aspectCurInv(0.0f)
	, _widthLgc(0)
	, _heightLgc(0)
	, _acpectLgc(0.0f)
	, _acpectLgcInv(0.0f)
	, _xscaleLgc(0.0f)
	, _yscaleLgc(0.0f)
	, _xscaleLgcInv(0.0f)
	, _yscaleLgcInv(0.0f)
	, _mtxPerspect(0L)
	, _isChPerspect(false)
	, _fovyCache(0.0f)
	, _cnearCache(0.0f)
	, _cfarCache(0.0f)
{
}
	
/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
View::~View()
{
	ASSERT(_mtxPerspect == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool View::create()
{
	// パースペクティブ行列を作成する
	_mtxPerspect = new Matrix4F();
	_isChPerspect = true;	// 計算の必要あり
	
	_scaleCoord = 1.0f;
	_scaleCoordInv = 1.0f;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void View::destroy()
{
	// パースペクティブ行列を削除する
	delete _mtxPerspect;
	_mtxPerspect = 0L;
}

/*---------------------------------------------------------------------*//**
	2D の基本行列を設定
**//*---------------------------------------------------------------------*/
void View::apply2dDrawSetting()
{
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	Matrix4F mtx;
	f32 left, right, top, bottom, znear, zfar;

	// 左上を原点にし、ウインドウ座標系に設定
	///STRACELN("view: %d", _dispflags & TFW_DISPF_RANG_MASK);
	switch (_dispflags & TFW_DISPF_RANG_MASK)
	{
	case TFW_DISPF_RANG_0:
		// 0 度回転した glOrtho + α
		left = _xLeft;
		right = _xRight;
		bottom = _yBottom;
		top = _yTop;
		znear = -1.0f;
		zfar = 1.0f;
		mtx.set(
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (zfar - znear), 0.0f,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zfar + znear) / (zfar - znear), 1.0f);
		// 論理サイズに対するスケールを掛け合わせる
		mtx._v[0][0] *= _xscaleLgc;
		mtx._v[1][1] *= _yscaleLgc;
		::glMultMatrixf(mtx.ptr());
		::glTranslatef(left * _xscaleLgcInv, top * _yscaleLgcInv, 0.0f);
		break;
	case TFW_DISPF_RANG_90:
		// -90 度回転した glOrtho + α
		left = _yBottom;
		right = _yTop;
		bottom = _xRight;
		top = _xLeft;
		znear = -1.0f;
		zfar = 1.0f;
		mtx.set(
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (zfar - znear), 0.0f,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zfar + znear) / (zfar - znear), 1.0f);
		// 論理サイズに対するスケールを掛け合わせる
		mtx._v[1][0] *= _yscaleLgc;
		mtx._v[0][1] *= _xscaleLgc;
		::glMultMatrixf(mtx.ptr());
		::glTranslatef(top * _yscaleLgcInv, right * _xscaleLgcInv, 0.0f);
		break;
	case TFW_DISPF_RANG_180:
		// -180 度回転した glOrtho + α
		left = _xRight;
		right = _xLeft;
		bottom = _yTop;
		top = _yBottom;
		znear = -1.0f;
		zfar = 1.0f;
		mtx.set(
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (zfar - znear), 0.0f,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zfar + znear) / (zfar - znear), 1.0f);
		// 論理サイズに対するスケールを掛け合わせる
		mtx._v[0][0] *= _xscaleLgc;
		mtx._v[1][1] *= _yscaleLgc;
		::glMultMatrixf(mtx.ptr());
		::glTranslatef(right * _xscaleLgcInv, bottom * _yscaleLgcInv, 0.0f);
		break;
	case TFW_DISPF_RANG_270:
		// -270 度回転した glOrtho + α
		left = _yTop;
		right = _yBottom;
		bottom = _xLeft;
		top = _xRight;
		znear = -1.0f;
		zfar = 1.0f;
		mtx.set(
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (zfar - znear), 0.0f,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zfar + znear) / (zfar - znear), 1.0f);
		// 論理サイズに対するスケールを掛け合わせる
		mtx._v[1][0] *= _yscaleLgc;
		mtx._v[0][1] *= _xscaleLgc;
		::glMultMatrixf(mtx.ptr());
		::glTranslatef(bottom * _yscaleLgcInv, left * _xscaleLgcInv, 0.0f);
		break;
	}

//	mtx._v[0][3] = left * _xscaleLgc;

//	::glMultMatrixf(mtx.ptr());

//	::glTranslatef(left * _xscaleLgcInv, top * _yscaleLgcInv, 0.0f);

//	STRACE("w=%d,h=%d,sx=%f,sy=%f\n", , _xscaleLgc, _yscaleLgc);
}

/*---------------------------------------------------------------------*//**
	パースペクティブ行列を計算する
**//*---------------------------------------------------------------------*/
const Matrix4F* View::calcPerspectiveMatrix(f32 fovy, f32 cnear, f32 cfar)
{
	if(!_isChPerspect)
	{
		if((_fovyCache == fovy) && (_cnearCache == cnear) && (_cfarCache == cfar))
		{
			return _mtxPerspect;
		}
	}

	switch(_dispflags & TFW_DISPF_RANG_MASK)
	{
	case TFW_DISPF_RANG_0:
	case TFW_DISPF_RANG_180:
		Gcalc::perspectiveFovY(_mtxPerspect, fovy, _acpectLgc, cnear, cfar);
		break;
	default:
		Gcalc::perspectiveFovX(_mtxPerspect, fovy, _acpectLgc, cnear, cfar);
		break;
	}

	// とりあえずは再計算の必要は無い
	_fovyCache = fovy;
	_cnearCache = cnear;
	_cfarCache = cfar;
	_isChPerspect = false;

	return _mtxPerspect;
}

/*---------------------------------------------------------------------*//**
	ビューポートを適用
**//*---------------------------------------------------------------------*/
void View::applyViewport()
{
	calcScaleToLogicalSize();	// 論理サイズに対する比を計算する

	switch (_vmode)
	{
	default:
		_xLeft = 0;
		_yTop = 0;
		_xRight = _xLeft + _widthCur;
		_yBottom = _yTop + _heightCur;
		break;
	case VMODE_FIX:
		{
			f32 w = _widthLgc * _xscaleLgc;
			f32 h = _heightLgc * _yscaleLgc;
			ASSERT(w <= _widthCur);
			ASSERT(h <= _heightCur);
			_xLeft = (_widthCur - w) / 2;
			_yTop = (_heightCur - h) / 2;
			_xRight = _xLeft + w;
			_yBottom = _yTop + h;
		}
		break;
	}

	// ビューポート設定
	if (TFW_IS_FLAG(_dispflags, TFW_DISPF_ROT_COORD))
	{
		::glViewport((GLint)_yTop, (GLint)_xLeft, (GLint)(_yBottom - _yTop), (GLint)(_xRight - _xLeft));
	}
	else
	{
		::glViewport((GLint)_xLeft, (GLint)_yTop, (GLint)(_xRight - _xLeft), (GLint)(_yBottom - _yTop));
	}

	_isChPerspect = true;	// パースペクティブ計算の必要あり
}

/*---------------------------------------------------------------------*//**
	標準サイズに対する比を計算する
**//*---------------------------------------------------------------------*/
void View::calcScaleToLogicalSize()
{
	switch (_vmode)
	{
	case VMODE_VAR:
		_xscaleLgc = 1.0f;
		_yscaleLgc = 1.0f;
		break;
	case VMODE_FIX:
		_xscaleLgc = _widthCur / _widthLgc;
		_yscaleLgc = _heightCur / _heightLgc;
		if (_xscaleLgc < _yscaleLgc) { _yscaleLgc = _xscaleLgc; }
		else if (_xscaleLgc > _yscaleLgc) { _xscaleLgc = _yscaleLgc; }
		break;
	case VMODE_SCALE:
		_xscaleLgc = _widthCur / _widthLgc;
		_yscaleLgc = _heightCur / _heightLgc;
		break;
	}

	_xscaleLgcInv = (1.0f / _xscaleLgc);
	_yscaleLgcInv = (1.0f / _yscaleLgc);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
