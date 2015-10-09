/***********************************************************************//**
 *  Model.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/27.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Model.h"

// Friends
#include "../SgNodeType.h"
#include "../../../Shape.h"
#include "../../../Material.h"
#include "../../../HrAnimSet.h"
#include "../../../../gcmn/Texture.h"
#include "../../../../gcmn/Renderer.h"
#include "../../../../collection/List.h"
#include "../../../../string/VcString.h"
#include "../../../../lib/Color.h"
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"
#include "../../../../collision/Gcol.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Model メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	位置と向きを設定する
**//*---------------------------------------------------------------------*/
void Model::setPosition(const Vector3F* position, const Vector3F* direction)
{
	if(position != 0L)
	{
		if(_posPosture == 0L)	{	_posPosture = new Vector3F();	}
		_posPosture->copy(position);
	}

	if(direction != 0L)
	{
		if(_dirPosture == 0L)	{	_dirPosture = new Vector3F();	}
		_dirPosture->copy(direction);
	}

	if(_mtxPosture == 0L)	{	_mtxPosture = new Matrix4F();	}
	
	_isChangePosture = true;

	if(_viewpos == 0L)		{	_viewpos = new Vector3F();		}
}

/*---------------------------------------------------------------------*//**
	姿勢（位置と回転）を設定する
**//*---------------------------------------------------------------------*/
void Model::setPosture(const Vector3F* translate, const Vector3F* rotate, const Vector3F* scale)
{
	if(translate != 0L)
	{
		if(_posPosture == 0L)	{	_posPosture = new Vector3F(translate);	}
		else					{	_posPosture->copy(translate);			}
	}

	if(rotate != 0L)
	{
		if(_rotPosture == 0L)	{	_rotPosture = new Vector3F(rotate);		}
		else					{	_rotPosture->copy(rotate);				}
	}

	if(scale != 0L)
	{
		if(_sclPosture == 0L)	{	_sclPosture = new Vector3F(scale);		}
		else					{	_sclPosture->copy(scale);				}
	}

	if(_mtxPosture == 0L)	{	_mtxPosture = new Matrix4F();	}
	
	_isChangePosture = true;

	if(_viewpos == 0L)		{	_viewpos = new Vector3F();		}
}

/*---------------------------------------------------------------------*//**
	スケールを設定する
**//*---------------------------------------------------------------------*/
void Model::setScale(const Vector3F* scale)
{
	if(_sclPosture == 0L)	{	_sclPosture = new Vector3F(scale);		}
	else					{	_sclPosture->copy(scale);				}

	if(_mtxPosture == 0L)	{	_mtxPosture = new Matrix4F();			}
	if(_posPosture == 0L)	{	_posPosture = new Vector3F();			}	// _mtxPosture の計算をさせるため
	
	_isChangePosture = true;
}

/*---------------------------------------------------------------------*//**
	色の設定
	※	ただし、内部のオブジェクト自体が色を持っている場合は効かない事があ
		る．例えば、一般的な 3D モデルは頂点カラーを持っていると色を変更でき
		ない．
**//*---------------------------------------------------------------------*/
void Model::setColor(const ColorU8*	color)
{
	if(_color == 0L)	{	_color = new ColorU8(color);	}
	else				{	_color->copy(color);			}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Model::Model()
	: _modelclass(0)
	, _mflags(0)
	, _rendorder(0)
	, _exflags(0)
	, _spid(0)
	, _flagsRenderOpt(0)
	, _flagsRenderRes(0)
	, _posPosture(0L)
	, _dirPosture(0L)
	, _rotPosture(0L)
	, _sclPosture(0L)
	, _mtxPosture(0L)
	, _isChangePosture(false)
	, _viewpos(0L)
	, _color(0L)
	, _camanimRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Model::~Model()
{
	ASSERT(_posPosture == 0L);
	ASSERT(_dirPosture == 0L);
	ASSERT(_rotPosture == 0L);
	ASSERT(_sclPosture == 0L);
	ASSERT(_mtxPosture == 0L);
	ASSERT(_viewpos == 0L);
	ASSERT(_color == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Model::create()
{
	return SgNode::create(SGNTYPE_MODEL);
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Model::destroy()
{
	delete _color;
	_color = 0L;
	delete _viewpos;
	_viewpos = 0L;
	delete _mtxPosture;
	_mtxPosture = 0L;
	delete _sclPosture;
	_sclPosture = 0L;
	delete _rotPosture;
	_rotPosture = 0L;
	delete _dirPosture;
	_dirPosture = 0L;
	delete _posPosture;
	_posPosture = 0L;

	SgNode::destroy();
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void Model::copy(const Model* mdlSrc)
{
	SgEntity::copy(this);
	// ※ _flagsRenderRes は特に意味もないと思われるのでコピーしない
	this->_rendorder = mdlSrc->_rendorder;
	this->_mflags = mdlSrc->_mflags;
	this->_exflags = mdlSrc->_exflags;
	this->_spid = mdlSrc->_spid;
	this->_flagsRenderOpt = mdlSrc->_flagsRenderOpt;
	if(mdlSrc->_posPosture != 0L)	{	this->_posPosture = new Vector3F(mdlSrc->_posPosture);	}
	if(mdlSrc->_dirPosture != 0L)	{	this->_dirPosture = new Vector3F(mdlSrc->_dirPosture);	}
	if(mdlSrc->_rotPosture != 0L)	{	this->_rotPosture = new Vector3F(mdlSrc->_rotPosture);	}
	if(mdlSrc->_sclPosture != 0L)	{	this->_sclPosture = new Vector3F(mdlSrc->_sclPosture);	}
	if(mdlSrc->_mtxPosture != 0L)	{	this->_mtxPosture = new Matrix4F(mdlSrc->_mtxPosture);	}
	this->_isChangePosture = false;
	if(mdlSrc->_viewpos != 0L)		{	this->_viewpos = new Vector3F(mdlSrc->_viewpos);		}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Model::exec(ExecRes* res, const ExecCtx* ec)
{
	///ASSERT(getId() > 0);

	if(isShow())
	{
		// 行列計算
		if((_posPosture != 0L) && _isChangePosture)	// 平行移動がある場合．現在は回転やスケールも基本的には平行移動があることを前提
		{
			calcPostureMatrix();
			_isChangePosture = false;
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void Model::previseImplements(const RenderCtx* rc)
{
	previseContents(rc);
}

/*---------------------------------------------------------------------*//**
	3D 描画実装
**//*---------------------------------------------------------------------*/
void Model::renderImplements(const RenderCtx* rc)
{
	_flagsRenderRes = 0;

	// バウンディングボックス等による描画対象判定
	if(!testRenderingTarget(rc))
	{
		TFW_SET_FLAG(_flagsRenderRes, RRESFLAG_CLIPPED, true);
		return;
	}

	// 半透明設定
	if(TFW_IS_FLAG(_flagsRenderOpt, ROPTFLAG_TRANS_NML))		// 通常半透明
	{
		rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);	// 通常合成
		rc->getRenderer()->setFog(true);								// フォグ有効
	}
	else if(TFW_IS_FLAG(_flagsRenderOpt, ROPTFLAG_TRANS_ADD))	// 加算半透明
	{
		rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_ADD);		// 加算合成
		rc->getRenderer()->setFog(false);								// フォグ無効
	}
	else if(TFW_IS_FLAG(_flagsRenderOpt, ROPTFLAG_TRANS_SUB))	// 減算半透明
	{
		rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_SUB);		// 減算合成
		rc->getRenderer()->setFog(false);								// フォグ無効
	}

	// 色の設定
	if(_color != 0L)
	{
		rc->getRenderer()->setSolidColor(_color);
	}

	// 描画
	if(_mtxPosture != 0L)	// 姿勢行列がある場合 
	{
		// 行列を設定
		::glPushMatrix();
		::glMultMatrixf(_mtxPosture->ptr());

		// コンテンツ描画
		renderContents(rc);

		// 行列を戻す
		::glPopMatrix();
	}
	else
	{
		// コンテンツ描画
		renderContents(rc);
	}

	// 色を戻す（これだけ統一感が無いが、たいていのモデルは色の設定がないと考える）
	if(_color != 0L)
	{
		ColorU8 col(255, 255, 255, 255);
		rc->getRenderer()->setSolidColor(&col);
	}
}

/*---------------------------------------------------------------------*//**
	姿勢行列を計算する
**//*---------------------------------------------------------------------*/
void Model::calcPostureMatrix()
{
	ASSERT(_mtxPosture != 0L);
	_mtxPosture->setIdentity();
	if(_dirPosture != 0L)
	{
		_mtxPosture->rotateZ( _dirPosture->z() );
		_mtxPosture->rotateY( _dirPosture->y() );
		_mtxPosture->rotateX( _dirPosture->x() );
	}
	_mtxPosture->translate( _posPosture );
	if(_rotPosture != 0L)
	{
		_mtxPosture->rotateX( _rotPosture->x() );
		_mtxPosture->rotateY( _rotPosture->y() );
		_mtxPosture->rotateZ( _rotPosture->z() );
	}
	if(_sclPosture != 0L)
	{
		_mtxPosture->scale( _sclPosture );
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS