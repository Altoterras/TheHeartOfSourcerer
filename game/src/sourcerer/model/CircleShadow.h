/***********************************************************************//**
 *	CircleShadow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/27.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODEL_CIRCLE_SHADOW_H_
#define _SRCR_MODEL_CIRCLE_SHADOW_H_

////////////////////////////////////////////////////////////////////////////
// コンパイルスイッチ

#define ENABLE_CIRCLESHADOW_AFFINE	(0)	// 影の傾きを表現するアフィン変換有効

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/Model.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Matrix4F;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ユニット基本クラス
 *
**//*---------------------------------------------------------------------*/
class CircleShadow : public Model
{
	//======================================================================
	// 定数
public:
	static f32	RADIUS_DEFAULT;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// X オフセット位置を得る
	inline f32 getOffsetPosX() const { return _xOffsetPos; }
	// Z オフセット位置を得る
	inline f32 getOffsetPosZ() const { return _zOffsetPos; }
	// 半径を得る
	inline f32 getRadius() const { return _radius; }
	// 不透明度を得る
	inline u8 getOpacity() const { return _opacity; }

	// 半径を設定する
	inline void setRadius(f32 r) { _radius = r; }
	// 不透明度を設定する
	inline void setOpacity(u8 alpha) { _opacity = alpha; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setOffsetPos(f32 x, f32 z);
#if ENABLE_CIRCLESHADOW_AFFINE
	void setAffineMatrix(const Matrix4F* mtx);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CircleShadow();
	virtual ~CircleShadow();
	virtual bool copy(const CircleShadow* mdlSrc);
	virtual Model* clone() const;
	virtual bool create();
	virtual void destroy();

	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:
	virtual bool testRenderingTarget(const RenderCtx* rc);
	virtual void previseContents(const RenderCtx* rc);
	virtual void renderContents(const RenderCtx* rc);	// 描画

	//======================================================================
	// 変数
protected:
	Texture*	_tex;
#if ENABLE_CIRCLESHADOW_AFFINE
	Matrix4F*	_mtxAffine;
#endif
	f32			_xOffsetPos;
	f32			_zOffsetPos;
	f32			_radius;
	u8			_opacity;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODEL_CIRCLE_SHADOW_H_
