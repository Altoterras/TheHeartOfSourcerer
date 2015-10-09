/***********************************************************************//**
 *	CircleShadow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/27.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CircleShadow.h"

// Friends
#include "../file/GameFileIdDef.h"

// External
#include "../../etk/sg/SorSgm.h"
#include "../../etk/sg/TbfDefineEx.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CircleShadow 静的変数

f32	CircleShadow::RADIUS_DEFAULT = 70.0f;

//==========================================================================
// CircleShadow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	オフセット位置を設定する
**//*---------------------------------------------------------------------*/
void CircleShadow::setOffsetPos(f32 x, f32 z)
{
	_xOffsetPos = x;
	_zOffsetPos = z;
}

#if ENABLE_CIRCLESHADOW_AFFINE
/*---------------------------------------------------------------------*//**
	アフィン行列を設定する
**//*---------------------------------------------------------------------*/
void CircleShadow::setAffineMatrix(const Matrix4F* mtx)
{
	_mtxAffine->set(mtx);
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CircleShadow::CircleShadow()
	: _tex(0L)
#if ENABLE_CIRCLESHADOW_AFFINE
	, _mtxAffine(0L)
#endif
	, _xOffsetPos(0.0f)
	, _zOffsetPos(0.0f)
	, _radius(RADIUS_DEFAULT)
	, _opacity(255)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CircleShadow::~CircleShadow()
{
	ASSERT(_tex == 0L);
#if ENABLE_CIRCLESHADOW_AFFINE
	ASSERT(_mtxAffine == 0L);
#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool CircleShadow::create()
{
	// テクスチャ読み込み
	Texture* tex = new Texture;
	if(!tex->create(SYSTEM_CRCSHDW_PNG, Texture::FMT_PNG))
	{
		delete tex;
		ASSERT(false);
		return false;
	}
	_tex = tex;

#if ENABLE_CIRCLESHADOW_AFFINE
	// アフィン行列の作成
	_mtxAffine = new Matrix4F;
#endif

	setExFlag(TbfDefineEx::MEXF_CHAR, true);	// キャラクタとして扱う
	setRenderOrder(SorSgm::RO_STUCK_TRANS_MIN);	// 密着半透明として描画

	return Model::create();
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
bool CircleShadow::copy(const CircleShadow* mdlSrc)
{
	this->_tex = mdlSrc->_tex;
#if ENABLE_CIRCLESHADOW_AFFINE
	this->_mtxAffine->copy(mdlSrc->_mtxAffine);
#endif
	this->_xOffsetPos = mdlSrc->_xOffsetPos;
	this->_zOffsetPos = mdlSrc->_zOffsetPos;
	this->_radius = mdlSrc->_radius;
	this->_opacity = mdlSrc->_opacity;

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
Model* CircleShadow::clone() const
{
	CircleShadow* mdlNew = new CircleShadow();
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
	破棄
**//*---------------------------------------------------------------------*/
void CircleShadow::destroy()
{
	Model::destroy();

	_tex->destroy();
	delete _tex;
	_tex = 0L;

#if ENABLE_CIRCLESHADOW_AFFINE
	delete _mtxAffine;
	_mtxAffine = 0L;
#endif
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void CircleShadow::exec(ExecRes* res, const ExecCtx* ec)
{
	Model::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	バウンディングボックス等による描画対象判定
**//*---------------------------------------------------------------------*/
bool CircleShadow::testRenderingTarget(const RenderCtx* rc)
{
	return true;	// 暫定
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void CircleShadow::previseContents(const RenderCtx* rc)
{
	// 暫定
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void CircleShadow::renderContents(const RenderCtx* rc)
{
#if ENABLE_CIRCLESHADOW_AFFINE
	// 傾きアフィン行列反映
	::glMultMatrixf(_mtxAffine->ptr());
#endif
	// オフセット反映
	::glTranslatef(_xOffsetPos, 0.0f, _zOffsetPos);

	const GLfloat vx = - _radius * 0.5f;
	const GLfloat vy = 2.0f;		// 少し浮かせる
	const GLfloat vz = vx;
	const GLfloat vw = _radius;
	const GLfloat vh = _radius;
	const GLfloat tu = 0.0f;
	const GLfloat tv = 0.0f;
	const GLfloat tw = 1.0f;
	const GLfloat th = 1.0f;
	const GLfloat vtxs[] =
	{
		vx,			vy,		vz,
		vx,			vy,		vz + vh,
		vx + vw,	vy,		vz,
		vx + vw,	vy,		vz + vh,
	};
	const GLfloat uvs[] =
	{
		tu,			tv + th,
		tu,			tv,
		tu + tw,	tv + th,
		tu + tw,	tv,
	};

	Renderer* rdr = rc->getRenderer();
	rdr->bindTexture(_tex);								// テクスチャを設定
	rdr->setAlphaBlend(true);							// アルファブレンドあり
	rdr->setAlphaBlendFunc(Renderer::AFUNC_SUB);		// アルファファンクションを減算に
	rdr->setFog(false);									// フォグを無効に
	rdr->setSolidColor(255, 255, 255, _opacity);		// カラー指定

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(3, GL_FLOAT, 0, vtxs);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, uvs);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);

	///*test*/RendererUtils::render3dLineSphere(rdr, _radius, 1);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
