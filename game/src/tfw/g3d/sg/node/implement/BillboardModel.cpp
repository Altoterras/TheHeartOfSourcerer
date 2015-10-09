/***********************************************************************//**
 *  BillboardModel.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/19.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BillboardModel.h"

// Friends
#include "Camera.h"
#include "../SgNodeType.h"
#include "../../../Shape.h"
#include "../../../ShapeCtx.h"
#include "../../../Material.h"
#include "../../../HrAnimSet.h"
#include "../../../../gcmn/Texture.h"
#include "../../../../gcmn/Renderer.h"
#include "../../../../collection/List.h"
#include "../../../../collision/Gcol.h"
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"
#include "../../../../string/VcString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BillboardModel メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	同じシェイプを持つモデルか判定する
**//*---------------------------------------------------------------------*/
bool BillboardModel::isSameShape(const BillboardModel* mdlTrg) const
{
	return this->_shapeRef == mdlTrg->_shapeRef;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BillboardModel::BillboardModel()
	: _shapeRef(0L)
	, _shpctx(0L)
	, _rShape(0.0f)
{
	setModelClass(MODELCLASS_BILLBOARD);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BillboardModel::~BillboardModel()
{
	ASSERT(_shpctx == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BillboardModel::create(Shape* shapeRef)
{
	if(!Model::create())	// モデル作成
	{
		return false;
	}

	// シェイプへのポインタをコピー
	_shapeRef = shapeRef;

	// シェイプコンテキストの作成
	if(_shapeRef != 0L)
	{
		_shpctx = new ShapeCtx();
		_shapeRef->createContext(_shpctx);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BillboardModel::destroy()
{
	// シェイプコンテキストの削除
	if(_shpctx != 0L)
	{
		ASSERT(_shapeRef != 0L);
		_shapeRef->destroyContext(_shpctx);
		delete _shpctx;
		_shpctx = 0L;
	}

	// ポインタの NULL 化（インスタンスを委譲されていない）
	_shapeRef = 0L;

	Model::destroy();
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
bool BillboardModel::copy(const BillboardModel* mdlSrc)
{
	this->_shapeRef = mdlSrc->_shapeRef;
	this->_rShape = mdlSrc->_rShape;

	// シェイプコンテキストを作成する
	if(this->_shapeRef != 0L)
	{
		this->_shpctx = new ShapeCtx();
		this->_shapeRef->createContext(this->_shpctx);
	}

	// モデル作成
	if(!this->Model::create())
	{
		return false;
	}

	// モデルの属性をコピー
	this->Model::copy(mdlSrc);

	return true;
}

/*---------------------------------------------------------------------*//**
	コピーを作る
	※	ResourcePool でリソースを管理している場合は、
		ResourcePool に追加するのを忘れないこと
**//*---------------------------------------------------------------------*/
Model* BillboardModel::clone() const
{
	BillboardModel* mdlNew = new BillboardModel();
	if(mdlNew->copy(this))
	{
		return mdlNew;
	}
	else
	{
		delete mdlNew;
		return 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void BillboardModel::exec(ExecRes* res, const ExecCtx* ec)
{
	// テクスチャアニメのフレーム更新
	_shapeRef->exec(ec, _shpctx);
	
	Model::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	バウンディングボックス等による描画対象判定
**//*---------------------------------------------------------------------*/
bool BillboardModel::testRenderingTarget(const RenderCtx* rc)
{
	// クリップ無しフラグが立っている場合は常に合格
	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_CLIP))
	{
		return true;
	}

	// 現在のカメラを取得
	Camera* cam = rc->getRenderer()->getCurrentCamera();
	if(cam == 0L)	{	return false;	}

	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_VIEWPOS))	// ビュー座標計算無しフラグが立っている場合
	{
		// シェイプより半径を得る
		if(_rShape == 0.0f)	// 未取得の場合のみ
		{
			Vector3F vBbMinWk;
			Vector3F vBbMaxWk;
			_shapeRef->getBoundingBox(&vBbMinWk, &vBbMaxWk);

			// 取得済みとして保存する
			_rShape = (vBbMaxWk - vBbMinWk).length() * 0.5f;
		}

		#if 1
			// 視錐台カリング判定
			return cam->isClipWithWorldPos(_posPosture, _rShape);

		#else
			// 位置を適用
			bbMinWk.set(_posPosture->x() - _rShape, _posPosture->y() - _rShape, _posPosture->z() - _rShape);
			bbMaxWk.set(_posPosture->x() + _rShape, _posPosture->y() + _rShape, _posPosture->z() + _rShape);

			// カメラ座標でのバウンディングボックスに変換
			const Matrix4F* mtxCam = cam->getCameraCoordMatrix();
			mtxCam->mult(&bbMinWk, &bbMinWk);
			mtxCam->mult(&bbMaxWk, &bbMaxWk);

			// とりあえずざっくり判定
			f32 xMin = -1300.0f;	// 数値は現在適当
			f32 xMax = 1300.0f;			// 本来は視体積から正確に求められるはずである
			f32 yMin = -1100.0f;			// 将来的にパースペクティブ等から正確に求めるように改良したい
			f32 yMax = 1100.0f;
			f32 zMin = - cam->getNear();
			f32 zMax = - cam->getFar();
			if((bbMinWk.x() < xMin) && (bbMaxWk.x() < xMin))	{	return false;	}
			if((bbMinWk.x() > xMax) && (bbMaxWk.x() > xMax))	{	return false;	}
			if((bbMinWk.y() < yMin) && (bbMaxWk.y() < yMin))	{	return false;	}
			if((bbMinWk.y() > yMax) && (bbMaxWk.y() > yMax))	{	return false;	}
			if((bbMinWk.z() > zMin) && (bbMaxWk.z() > zMin))	{	return false;	}
			if((bbMinWk.z() < zMax) && (bbMaxWk.z() < zMax))	{	return false;	}

			return true;

		#endif
	}
	else if(_viewpos != 0L)
	{
		// 視錐台カリング判定
		return cam->isClipWithViewPos(_viewpos, _rShape);
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void BillboardModel::previseContents(const RenderCtx* rc)
{
	// ビュー座標計算無しフラグが立っている場合は抜ける
	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_VIEWPOS) || (_viewpos == 0L))
	{
		return;
	}

	// 現在のカメラを取得
	Camera* cam = rc->getRenderer()->getCurrentCamera();
	if(cam == 0L)	{	return;	}

	// シェイプより半径を得る
	if(_rShape == 0.0f)	// 未取得の場合のみ
	{
		Vector3F vBbMinWk;
		Vector3F vBbMaxWk;
		_shapeRef->getBoundingBox(&vBbMinWk, &vBbMaxWk);

		// 取得済みとして保存する
		_rShape = (vBbMaxWk - vBbMinWk).length() * 0.5f;
	}

	// ビュー座標を計算
	const Matrix4F* mtxView = cam->getViewMatrix();
	mtxView->mult(_viewpos, _posPosture);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void BillboardModel::renderContents(const RenderCtx* rc)
{
	// ビルボード行列適用
	Camera* cam = rc->getRenderer()->getCurrentCamera();
	if(cam != 0L)
	{
		::glMultMatrixf(cam->getBillboardMatrix()->ptr());
	}

	// シェイプのレンダリング
	_shapeRef->render(rc, _shpctx);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS