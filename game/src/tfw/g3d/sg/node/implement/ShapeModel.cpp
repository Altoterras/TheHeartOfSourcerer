/***********************************************************************//**
 *  ShapeModel.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/27.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ShapeModel.h"

// Friends
#include "Camera.h"
#include "../SgNodeType.h"
#include "../../../Shape.h"
#include "../../../ShapeCtx.h"
#include "../../../Material.h"
#include "../../../HrAnimSet.h"
#include "../../../HrAnimSetCtx.h"
#include "../../../../collection/List.h"
#include "../../../../collision/Gcol.h"
#include "../../../../gcmn/Texture.h"
#include "../../../../gcmn/Renderer.h"
#include "../../../../gcmn/RendererUtils.h"
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
// ShapeModel メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アニメーション数を得る
**//*---------------------------------------------------------------------*/
s32 ShapeModel::getAnimNum() const
{
	if(_asetRef == 0L)	{	return 0;	}
	return (s32)_asetRef->getAnimNum();
}

/*---------------------------------------------------------------------*//**
	現在のアニメーション ID を得る
**//*---------------------------------------------------------------------*/
u16 ShapeModel::getCurAnimId() const
{
	if(_asetRef == 0L)	{	return 0;	}
	return _asetRef->getCurAnimId(_ascAsetCtx);
}

/*---------------------------------------------------------------------*//**
	最大のアニメーション ID を得る
**//*---------------------------------------------------------------------*/
u16 ShapeModel::getMaxAnimId() const
{
	if(_asetRef == 0L)	{	return 0;	}
	return _asetRef->getMaxAnimId(_ascAsetCtx);
}

/*---------------------------------------------------------------------*//**
	アニメーション ID を設定する
**//*---------------------------------------------------------------------*/
void ShapeModel::setAnimId(u16 animid, s32 frameIntp, bool replaceHeadKeyFrameByCurPos, bool isForceReset, bool isLoop, bool isChangeFirstFrame, bool isChangeEndFrame)
{
	if(_asetRef == 0L)														{	return;		}	// アニメーションがない
	if((_asetRef->getCurAnimId(_ascAsetCtx) == animid) && !isForceReset)	{	return;		}	// 現在のアニメと同じか、強制リセットでない
	_asetRef->setAnimId(_ascAsetCtx, animid, frameIntp, replaceHeadKeyFrameByCurPos, isLoop, isChangeFirstFrame, isChangeEndFrame);
}

/*---------------------------------------------------------------------*//**
	現在のアニメーションフレームを得る
**//*---------------------------------------------------------------------*/
f32 ShapeModel::getCurAnimFrame() const
{
	if(_asetRef == 0L)	{	return -1.0f;	}
	return _asetRef->getCurAnimFrame(_ascAsetCtx);
}

/*---------------------------------------------------------------------*//**
	テクスチャアニメのフレームをリセットする
**//*---------------------------------------------------------------------*/
void ShapeModel::resetTexAnimFrame()
{
	for(int i = 0; i < _numShape; i++)
	{
		Material* mtrl = this->_sarrShape[i]->getMaterial();
		if(mtrl != 0L)
		{
			mtrl->resetTexAnimFrame(_scarrShpCtx[i]->_mtrlctx);
		}
	}
}

/*---------------------------------------------------------------------*//**
	同じシェイプを持つモデルか判定する
**//*---------------------------------------------------------------------*/
bool ShapeModel::isSameShape(const ShapeModel* mdlTrg) const
{
	if(this->_numShape != mdlTrg->_numShape)	{	return false;	}
	for(int i = 0; i < _numShape; i++)
	{
		if(this->_sarrShape[i] != mdlTrg->_sarrShape[i])	{	return false;	}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	同じ定義名のシェイプを持つモデルか判定する
**//*---------------------------------------------------------------------*/
bool ShapeModel::isSameDefNameShape(const ShapeModel* mdlTrg) const
{
	if(this->_numShape != mdlTrg->_numShape)	{	return false;	}
	for(int i = 0; i < _numShape; i++)
	{
		const VcString* nameThis = this->_sarrShape[i]->getDefineName();
		const VcString* nameTrg = mdlTrg->_sarrShape[i]->getDefineName();

		if((nameThis == 0L) && (nameTrg == 0L))	{	continue;		}	// OK
		if((nameThis != 0L) && (nameTrg == 0L))	{	return false;	}	// NG
		if((nameThis == 0L) && (nameTrg != 0L))	{	return false;	}	// NG
		if(!nameThis->equals(nameTrg))			{	return false;	}	// NG
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	全シェイプを含めたバウンディングボックスを計算する
**//*---------------------------------------------------------------------*/
void ShapeModel::getAllShapesBoundingBox(Vector3F* vBbCenter, f32* rBb)
{
	if(_vShapesBbCenter == 0L)	// 未計算
	{
		Vector3F vBbMinWk;
		Vector3F vBbMaxWk;

		// 全シェイプ分を計算する
		Vector3F vBbMinS;
		Vector3F vBbMaxS;
		_sarrShape[0]->getBoundingBox(&vBbMinWk, &vBbMaxWk);
		for(int i = 1; i < (int)_numShape; i++)
		{
			_sarrShape[i]->getBoundingBox(&vBbMinS, &vBbMaxS);
			if(vBbMinWk._v[0] > vBbMinS._v[0])	{	vBbMinWk._v[0] = vBbMinS._v[0];	}
			if(vBbMinWk._v[1] > vBbMinS._v[1])	{	vBbMinWk._v[1] = vBbMinS._v[1];	}
			if(vBbMinWk._v[2] > vBbMinS._v[2])	{	vBbMinWk._v[2] = vBbMinS._v[2];	}
			if(vBbMaxWk._v[0] < vBbMaxS._v[0])	{	vBbMaxWk._v[0] = vBbMaxS._v[0];	}
			if(vBbMaxWk._v[1] < vBbMaxS._v[1])	{	vBbMaxWk._v[1] = vBbMaxS._v[1];	}
			if(vBbMaxWk._v[2] < vBbMaxS._v[2])	{	vBbMaxWk._v[2] = vBbMaxS._v[2];	}
		}

		// 取得済みとして保存する
		_vShapesBbCenter = new Vector3F((vBbMinWk.x() + vBbMaxWk.x()) * 0.5f, (vBbMinWk.y() + vBbMaxWk.y()) * 0.5f, (vBbMinWk.z() + vBbMaxWk.z()) * 0.5f);
		_rShapesBb = (vBbMaxWk - vBbMinWk).length() * 0.5f;
	}

	if(vBbCenter != 0L)	{	vBbCenter->copy(_vShapesBbCenter);	}
	if(rBb != 0L)		{	*rBb = _rShapesBb;					}
}

/*---------------------------------------------------------------------*//**
	アニメーションオブジェクトの姿勢を直接設定する
**//*---------------------------------------------------------------------*/
bool ShapeModel::setAnimObjPosture(u16 indexObj, const Vector3F* trs, const Vector3F* rot, const Vector3F* scl)
{
	if(_asetRef != 0L)
	{
		return _asetRef->setAnimObjPosture(_ascAsetCtx, indexObj, trs, rot, scl);
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ShapeModel::ShapeModel()
	: _sarrShape(0L)
	, _scarrShpCtx(0L)
	, _numShape(0)
	, _asetRef(0L)
	, _ascAsetCtx(0L)
	, _gcolRef(0L)
	, _vShapesBbCenter(0L)
	, _rShapesBb(0.0f)
	, _deftype(DEFTYPE_NULL)
{
	setModelClass(MODELCLASS_SHAPE);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ShapeModel::~ShapeModel()
{
	ASSERT(_sarrShape == 0L);
	ASSERT(_ascAsetCtx == 0L);
	ASSERT(_scarrShpCtx == 0L);
	ASSERT(_vShapesBbCenter == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ShapeModel::create(Shape** shapearrRef, s32 numShape, HrAnimSet* asetRef, Gcol* gcolRef)
{
	if(!Model::create())	// モデル作成
	{
		return false;
	}

	// 配列をコピー（インスタンスは参照のみで委譲されていない）
	if((numShape > 0) && (shapearrRef != 0L))
	{
		_sarrShape = new Shape*[numShape];
		::memcpy(_sarrShape, shapearrRef, sizeof(Shape*) * numShape);
		_numShape = numShape;
	}

	// シェイプコンテキストを作成する
	if((_numShape > 0) && (_sarrShape != 0L))
	{
		_scarrShpCtx = new ShapeCtx*[_numShape];
		for(int i = 0; i < _numShape; i++)
		{
			_scarrShpCtx[i] = new ShapeCtx();
			_sarrShape[i]->createContext(_scarrShpCtx[i]);
		}
	}

	// 参照を保持（インスタンスは委譲されていない）
	_asetRef = asetRef;
	_gcolRef = gcolRef;

	// アニメセットコンテキストを作成する
	if(_asetRef != 0L)	// アニメーションがある場合
	{
		_ascAsetCtx = new HrAnimSetCtx();
		_asetRef->createContext(_ascAsetCtx, _sarrShape, _scarrShpCtx);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ShapeModel::destroy()
{
	// アニメセットコンテキストを削除する
	if(_ascAsetCtx != 0L)
	{
		ASSERT(_asetRef != 0L);
		_asetRef->destroyContext(_ascAsetCtx);
		delete _ascAsetCtx;
		_ascAsetCtx = 0L;
	}

	// ポインタの NULL 化（インスタンスを委譲されていない）
	_gcolRef = 0L;
	_asetRef = 0L;

	// シェイプコンテキストを削除する
	for(int i = 0; i < _numShape; i++)
	{
		_sarrShape[i]->destroyContext(_scarrShpCtx[i]);
		delete _scarrShpCtx[i];
	}
	delete[] _scarrShpCtx;
	_scarrShpCtx = 0L;

	// 配列を削除（インスタンスは委譲されていない）
	delete[] _sarrShape;
	_sarrShape = 0L;
	_numShape = 0;

	// バウンディングボックスを削除
	delete _vShapesBbCenter;
	_vShapesBbCenter = 0L;

	Model::destroy();
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
bool ShapeModel::copy(const ShapeModel* mdlSrc)
{
	// 配列をコピー（インスタンスは参照のみで委譲されていない）
	if(mdlSrc->_sarrShape != 0L)
	{
		this->_sarrShape = new Shape*[mdlSrc->_numShape];
		::memcpy(this->_sarrShape, mdlSrc->_sarrShape, sizeof(Shape*) * mdlSrc->_numShape);
		this->_numShape = mdlSrc->_numShape;
	}

	// シェイプコンテキストを作成する
	if((this->_numShape > 0) && (this->_sarrShape != 0L))
	{
		this->_scarrShpCtx = new ShapeCtx*[this->_numShape];
		for(int i = 0; i < this->_numShape; i++)
		{
			this->_scarrShpCtx[i] = new ShapeCtx();
			this->_sarrShape[i]->createContext(this->_scarrShpCtx[i]);
		}
	}

	// 参照を保持（インスタンスは委譲されていない）
	this->_asetRef = mdlSrc->_asetRef;
	this->_gcolRef = mdlSrc->_gcolRef;

	// 宣言定義タイプフラグは初期化
	this->_deftype = DEFTYPE_NULL;

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
Model* ShapeModel::clone() const
{
	ShapeModel* mdlNew = new ShapeModel();
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
void ShapeModel::exec(ExecRes* res, const ExecCtx* ec)
{
	// 階層アニメのフレーム更新
	if(_asetRef != 0L)
	{
		_asetRef->exec(ec, _ascAsetCtx);
	}

	// テクスチャアニメのフレーム更新
	for(int i = 0; i < (int)_numShape; i++)
	{
		_sarrShape[i]->exec(ec, _scarrShpCtx[i]);
	}
	
	Model::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	バウンディングボックス等による描画対象判定
**//*---------------------------------------------------------------------*/
bool ShapeModel::testRenderingTarget(const RenderCtx* rc)
{
	// クリップ無しフラグが立っている場合は常に合格
	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_CLIP))
	{
		return true;
	}

	// 現在のカメラを取得
	Camera* cam = rc->getRenderer()->getCurrentCamera();
	if(cam == 0L)	{	return true;	}

	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_VIEWPOS))	// ビュー座標計算保存無しフラグが立っている場合
	{
		// シェイプよりバウンディングボックスを得る
		Vector3F vBbCenterWk;
		f32 rBb;
		getAllShapesBoundingBox(&vBbCenterWk, &rBb);	// 計算する

		// 自身の姿勢行列を適用
		if(_mtxPosture != 0L)
		{
			_mtxPosture->mult(&vBbCenterWk, &vBbCenterWk); 
		}

		// アニメーションによる移動も適用
		if(TFW_IS_FLAG(getModelFlag(), MFLAG_ANIM_VIEWPOS))
		{
			if((_asetRef != 0L) && (_asetRef->getObjRootNum() > 0))
			{
				Matrix4F mtxAnimRoot;
				_asetRef->calcObjRootMatrix(0, &mtxAnimRoot, _ascAsetCtx);
				mtxAnimRoot.mult(&vBbCenterWk, &vBbCenterWk); 
			}
		}

		#if 0	// クリップテストコード
		{
			bool isCorpo = false;
			for(int i = 0; i < (int)_numShape; i++)
			{
				if(_sarrShape[i]->getDebugName()->find("CLP", 0) != -1)
				{
					isCorpo = true;
					return cam->isClip(&vBbCenterWk, rBb);
				}
			}
		}
		#endif

		// 視錐台カリング判定
		return cam->isClipWithWorldPos(&vBbCenterWk, rBb);
	}
	else if(_viewpos != 0L)
	{
		// 視錐台カリング判定
		return cam->isClipWithViewPos(_viewpos, _rShapesBb);
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void ShapeModel::previseContents(const RenderCtx* rc)
{
	// ビュー座標計算無しフラグが立っている場合は抜ける
	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_VIEWPOS) || (_viewpos == 0L))
	{
		return;
	}

	// 現在のカメラを取得
	Camera* cam = rc->getRenderer()->getCurrentCamera();
	if(cam == 0L)	{	return;	}

	// シェイプよりバウンディングボックスを得る
	Vector3F vBbCenterWk;
	f32 rBb;
	getAllShapesBoundingBox(&vBbCenterWk, &rBb);	// 計算する

	// 自身の姿勢行列を適用
	if(_mtxPosture != 0L)
	{
		_mtxPosture->mult(&vBbCenterWk, &vBbCenterWk); 
	}

	// アニメーションによる移動も適用
	if(TFW_IS_FLAG(getModelFlag(), MFLAG_ANIM_VIEWPOS))
	{
		if((_asetRef != 0L) && (_asetRef->getObjRootNum() > 0))
		{
			Matrix4F mtxAnimRoot;
			_asetRef->calcObjRootMatrix(0, &mtxAnimRoot, _ascAsetCtx);
			mtxAnimRoot.mult(&vBbCenterWk, &vBbCenterWk); 
		}
	}

	// ビュー座標を計算
	const Matrix4F* mtxView = cam->getViewMatrix();
	mtxView->mult(_viewpos, &vBbCenterWk);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ShapeModel::renderContents(const RenderCtx* rc)
{
	// バウンディングボックス描画
	#if defined(_DEBUG) && 0
	{
		Vector3F vBbCenterWk;
		f32 rBb;
		getAllShapesBoundingBox(&vBbCenterWk, &rBb);	// 取得する

		Renderer* rdr = rc->getRenderer();
		rdr->setSolidColor(255, 255, 0, 255);
		Vector3F vtx[2] = { vBbCenterWk, vBbCenterWk };
		vtx[0]._v[0] = vBbCenterWk._v[0] - rBb;
		vtx[1]._v[0] = vBbCenterWk._v[0] + rBb;
		RendererUtils::render3dLines(rdr, vtx, 2, RendererUtils::LINES);
		vtx[0]._v[0] = vBbCenterWk._v[0];
		vtx[1]._v[0] = vBbCenterWk._v[0];
		vtx[0]._v[1] = vBbCenterWk._v[1] - rBb;
		vtx[1]._v[1] = vBbCenterWk._v[1] + rBb;
		RendererUtils::render3dLines(rdr, vtx, 2, RendererUtils::LINES);
		vtx[0]._v[1] = vBbCenterWk._v[1];
		vtx[1]._v[1] = vBbCenterWk._v[1];
		vtx[0]._v[2] = vBbCenterWk._v[2] - rBb;
		vtx[1]._v[2] = vBbCenterWk._v[2] + rBb;
		RendererUtils::render3dLines(rdr, vtx, 2, RendererUtils::LINES);
		rdr->setSolidColor(255, 255, 255, 255);
	}
	#endif

	// シェイプの描画
	if(_asetRef == 0L)	// アニメーション無し
	{
		for(int i = 0; i < (int)_numShape; i++)
		{
			_sarrShape[i]->render(rc, _scarrShpCtx[i]);
		}
	}
	else				// アニメーション有り
	{
		_asetRef->render(rc, _ascAsetCtx);
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS