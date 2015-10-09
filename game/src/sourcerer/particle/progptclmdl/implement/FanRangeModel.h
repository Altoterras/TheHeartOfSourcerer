/***********************************************************************//**
 *  FanRangeModel.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/04.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PARTICLE_PROGPTCLMDL_IMPLEMENT_FAN_RANGE_MODEL_H_
#define _SRCR_PARTICLE_PROGPTCLMDL_IMPLEMENT_FAN_RANGE_MODEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../tfw/g3d/sg/node/implement/Model.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Shape;
class ShapeCtx;
class Texture;
class Material;
class HrAnimSet;
class Gcol;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	扇形範囲エフェクト モデル
 *
**//*---------------------------------------------------------------------*/
class FanRangeModel : public Model
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setConfig(f32 radius, f32 angleOpen, f32 angleStart, s32 numDiv, s32 numFrame);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FanRangeModel();
	virtual ~FanRangeModel();
	virtual bool copy(const FanRangeModel* mdlSrc);
	virtual Model* clone() const;
	bool create();
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);

protected:
	virtual bool testRenderingTarget(const RenderCtx* rc);
	virtual void previseContents(const RenderCtx* rc);
	virtual void renderContents(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	// 設定変数
	f32 _radius;							// 半径
	f32 _angleOpen;							// 扇の開き角度
	f32 _angleStart;						// Z 方向を 0 とした開始角度
	s32 _numDiv;							// 分割数
	s32 _numVtxs;							// 頂点数
	s32 _numFrame;							// フレーム数

	// 作業変数
	f32 _framecnt;							// フレームカウンタ
	Vector3F* _posarrFanVtx;				// 頂点位置配列
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PARTICLE_PROGPTCLMDL_IMPLEMENT_FAN_RANGE_MODEL_H_
