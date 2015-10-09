/***********************************************************************//**
 *  HrAnimObj.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/01.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimObj.h"

// Friends
#include "HrAnimObjCtx.h"
#include "HrAnimObjCtxArray.h"
#include "HrAnimObjPosture.h"
#include "Shape.h"
#include "../lib/Matrix.h"
#include "../lib/Quaternion.h"
#include "../lib/Vector.h"
#include "../string/VcString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimObj::HrAnimObj メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	現在の姿勢の取得
**//*---------------------------------------------------------------------*/
void HrAnimObj::getPosture(HrAnimObjCtxArray* ctxa, HrAnimObjPosture* aop)
{
	ASSERT(_index < ctxa->_numObjCtx);
	HrAnimObjCtx* ctx = ctxa->_aocarrObjCtx[_index];

	aop->setTranslate(ctx->getTranslate());
	bool quaternion = TFW_IS_FLAG(ctx->getFlags(), HrAnimObjCtx::F_QUATERNION_ROT);
	if(quaternion)	{	aop->setQuaternion(ctx->getQuaternion());	}
	else			{	aop->setAngle(ctx->getAngle());				}
	aop->setScale(ctx->getScale());
}

/*---------------------------------------------------------------------*//**
	現在の姿勢の設定
**//*---------------------------------------------------------------------*/
void HrAnimObj::setPosture(HrAnimObjCtxArray* ctxa, const HrAnimObjPosture* aop)
{
	ASSERT(_index < ctxa->_numObjCtx);
	HrAnimObjCtx* ctx = ctxa->_aocarrObjCtx[_index];

	ctx->setTranslate(aop->getTranslate());
	bool quaternion = aop->hasQuaternion();
	if(quaternion)	{	ctx->setQuaternion(aop->getQuaternion());	}
	else			{	ctx->setAngle(aop->getAngle());				}
	const Vector3F* scale = aop->getScale();
	if(scale != 0L)	{	ctx->setScale(aop->getScale());				}
}

/*---------------------------------------------------------------------*//**
	インデックスによる該当オブジェクトの姿勢の設定
**//*---------------------------------------------------------------------*/
bool HrAnimObj::setPosture(HrAnimObjCtxArray* ctxa, u16 index, const HrAnimObjPosture* aop)
{
	bool isSucceeded = false;

	if(index == _index)
	{
		setPosture(ctxa, aop);
		return true;
	}

	if(_child != 0L)
	{
		if(_child->setPosture(ctxa, index, aop))	{	isSucceeded = true;	}
	}
	if(_sibling != 0L)
	{
		if(_sibling->setPosture(ctxa, index, aop))	{	isSucceeded = true;	}
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	シェイプよりオブジェクト検索
**//*---------------------------------------------------------------------*/
HrAnimObj* HrAnimObj::find(HrAnimObj* aobj, const Shape* shapeTrg)
{
	if(aobj->_shapeRef == shapeTrg)
	{
		return aobj;
	}

	if(aobj->_child != 0L)
	{
		HrAnimObj* aobjRes = find(aobj->_child, shapeTrg);
		if(aobjRes != 0L)	{	return aobjRes;	}
	}
	if(aobj->_sibling != 0L)
	{
		HrAnimObj* aobjRes = find(aobj->_sibling, shapeTrg);
		if(aobjRes != 0L)	{	return aobjRes;	}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	行列を計算する
**//*---------------------------------------------------------------------*/
void HrAnimObj::calcMatrix(Matrix4F* mtx, HrAnimObjCtxArray* ctxa) const
{
	ASSERT(_index < ctxa->_numObjCtx);
	HrAnimObjCtx* ctx = ctxa->_aocarrObjCtx[_index];

	// 行列計算
	ctx->calcModelViewMatrixIgnoreSaveFlag(mtx);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObj::HrAnimObj()
	: _index(0)
	, _shapeRef(0L)
	, _child(0L)
	, _sibling(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObj::~HrAnimObj()
{
	ASSERT(_child == 0L);
	ASSERT(_sibling == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HrAnimObj::create(u16 index, Shape* shapeRef)
{
	_index = index;
	_shapeRef = shapeRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HrAnimObj::destroy()
{
	if(_child != 0L)
	{
		_child->destroy();
		delete _child;
		_child = 0L;
	}
	if(_sibling != 0L)
	{
		_sibling->destroy();
		delete _sibling;
		_sibling = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool HrAnimObj::createContext(HrAnimObjCtxArray* ctxa, Shape** sarr, ShapeCtx** scarr, u16 numShape)
{
	ASSERT(_index < ctxa->_numObjCtx);
	HrAnimObjCtx* ctx = ctxa->_aocarrObjCtx[_index];

	// シェイプコンテキストを探し当てる
	ShapeCtx* shpctx = 0L;
	for(int i = 0; i < numShape; i++)
	{
		if(sarr[i] == _shapeRef)
		{
			shpctx = scarr[i];
			break;
		}
	}

	if(!ctx->create(shpctx))	{	return false;	}

	if(_child != 0L)
	{
		if(!_child->createContext(ctxa, sarr, scarr, numShape))	{	return false;	}
	}
	if(_sibling != 0L)
	{
		if(!_sibling->createContext(ctxa, sarr, scarr, numShape))	{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void HrAnimObj::destroyContext(HrAnimObjCtxArray* ctxa)
{
	ASSERT(_index < ctxa->_numObjCtx);
	HrAnimObjCtx* ctx = ctxa->_aocarrObjCtx[_index];
	ctx->destroy();

	if(_child != 0L)
	{
		_child->destroyContext(ctxa);
	}
	if(_sibling != 0L)
	{
		_sibling->destroyContext(ctxa);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void HrAnimObj::render(const RenderCtx* rc, HrAnimObjCtxArray* ctxa)
{
	ASSERT(_index < ctxa->_numObjCtx);
	HrAnimObjCtx* ctx = ctxa->_aocarrObjCtx[_index];
	u8 ctxflags = ctx->getFlags();

	// 表示オフ確認
	if(TFW_IS_FLAG(ctxflags, HrAnimObjCtx::F_OFF))
	{
		if(TFW_IS_FLAG(ctxflags, HrAnimObjCtx::F_SAVE_MV_MTX))	// モデルビュー行列を保存
		{
			// モデルビュー行列を保存
			Matrix4F mtx;
			ctx->calcAndSaveModelViewMatrix(&mtx);
		}

		return;
	}

	::glPushMatrix();

	// 行列設定
	Matrix4F mtx;
	ctx->calcAndSaveAndSupplyModelViewMatrix(&mtx);

	// シェイプをレンダリング
	_shapeRef->render(rc, ctx->getShapeCtx());

	// 子へ
	if(_child != 0L)
	{
		_child->render(rc, ctxa);
	}

	::glPopMatrix();

	// 兄弟へ
	if(_sibling != 0L)
	{
		_sibling->render(rc, ctxa);
	}
}

/*---------------------------------------------------------------------*//**
	デバッグ出力
**//*---------------------------------------------------------------------*/
void HrAnimObj::debug_print(int depth)
{
	for(int i = 0; i < depth; i++) { TRACE(" "); }
	TRACE(VcString::format("%s\n", (_shapeRef != 0L) ? VcString::format("%d", _shapeRef->getId()).getRaw() : "(null)"));

	if(_child != 0L)
	{
		_child->debug_print(depth + 1);
	}
	if(_sibling != 0L)
	{
		_sibling->debug_print(depth);
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS