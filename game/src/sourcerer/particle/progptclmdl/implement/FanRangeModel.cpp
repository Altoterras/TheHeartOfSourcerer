/***********************************************************************//**
 *  FanRangeModel.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/04.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FanRangeModel.h"

// Friends

// External
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/Matrix.h"

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FanRangeModel メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	基本設定
**//*---------------------------------------------------------------------*/
void FanRangeModel::setConfig(f32 radius, f32 angleOpen, f32 angleStart, s32 numDiv, s32 numFrame)
{
	// 半径を設定
	_radius = radius;
	_angleOpen = angleOpen;
	if(_angleOpen > TFW_DPI)	{	_angleOpen = TFW_DPI;	}
	_angleStart = angleStart;
	_numDiv = numDiv;
	_numVtxs = numDiv + 2;
	_numFrame = numFrame;

	// 頂点位置配列を作成する
	delete[] _posarrFanVtx;
	_posarrFanVtx = new Vector3F[_numVtxs];

	// 頂点位置を計算する
	_posarrFanVtx[0].set(0.0f, 0.0f, 0.0f);
	f32 theta = Gcalc::adjustAngle(_angleStart - (_angleOpen * 0.5f));
	const f32 ANG_PART = angleOpen / (f32)_numDiv;
	for(int i = 0; i < _numDiv + 1; i++)
	{
		_posarrFanVtx[1 + i].x() = _radius * ::cosf(theta);
		_posarrFanVtx[1 + i].y() = 0.0f;
		_posarrFanVtx[1 + i].z() = _radius * ::sinf(theta);
		theta += ANG_PART;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FanRangeModel::FanRangeModel()
	: _radius(0.0f)
	, _angleOpen(0.0f)
	, _angleStart(0.0f)
	, _numDiv(0)
	, _numVtxs(0)
	, _numFrame(0)
	, _framecnt(0.0f)
	, _posarrFanVtx(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FanRangeModel::~FanRangeModel()
{
	ASSERT(_posarrFanVtx == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FanRangeModel::create()
{
	if(!Model::create())	// モデル作成
	{
		return false;
	}

	// その他変数の初期化
	_framecnt = 0.0f;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FanRangeModel::destroy()
{
	// 頂点位置配列を削除する
	delete[] _posarrFanVtx;
	_posarrFanVtx = 0L;

	// Z 値描画の精度を戻す（シェーダーに移行するまでの暫定処理）
#if defined(_IOS) || defined(_ANDROID)
	::glDepthRangef(0.0f, 1.0f);
#else
	::glDepthRange(0.0, 1.0);
#endif

	Model::destroy();
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
bool FanRangeModel::copy(const FanRangeModel* mdlSrc)
{
	this->_radius = mdlSrc->_radius;

	// モデル作成
	if(!create())
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
Model* FanRangeModel::clone() const
{
	FanRangeModel* mdlNew = new FanRangeModel();
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
void FanRangeModel::exec(ExecRes* res, const ExecCtx* ec)
{
	Model::exec(res, ec);

	// フレームカウンタ更新
	_framecnt += ec->getDeltaFrame();

	// 透明度更新
	s32 a = (s32)((_numFrame - _framecnt) / (f32)_numFrame * 255.0f);
	if(a < 0)	{	a = 0;	}
	_color->a() = (u8)a;
}

/*---------------------------------------------------------------------*//**
	バウンディングボックス等による描画対象判定
**//*---------------------------------------------------------------------*/
bool FanRangeModel::testRenderingTarget(const RenderCtx* rc)
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
		// 視錐台カリング判定
		return cam->isClipWithWorldPos(_posPosture, _radius);
	}
	else if(_viewpos != 0L)
	{
		// 視錐台カリング判定
		return cam->isClipWithViewPos(_viewpos, _radius);
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void FanRangeModel::previseContents(const RenderCtx* rc)
{
	// ビュー座標計算無しフラグが立っている場合は抜ける
	if(TFW_IS_FLAG(getModelFlag(), MFLAG_NO_VIEWPOS) || (_viewpos == 0L))
	{
		return;
	}

	// 現在のカメラを取得
	Camera* cam = rc->getRenderer()->getCurrentCamera();
	if(cam == 0L)	{	return;	}

	// ビュー座標を計算
	const Matrix4F* mtxView = cam->getViewMatrix();
	mtxView->mult(_viewpos, _posPosture);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void FanRangeModel::renderContents(const RenderCtx* rc)
{
	// 範囲円を線で描く
	rc->getRenderer()->setSolidColorForce(_color);				// 色の設定（強制なのはパーティクルモデル内に設定されていることがあるため【2012/04/05 r-kishi】※要設計見直し）
	rc->getRenderer()->setAlphaBlend(true);						// アルファ合成あり
	rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_ADD);	// アルファ加算合成
	rc->getRenderer()->bindTexture(0L);							// テクスチャなし
	rc->getRenderer()->setCulling(false);						// 背面カリングなし
	rc->getRenderer()->setZTest(true);							// Z テストあり
	rc->getRenderer()->setZWriting(false);						// Z 書き込みなし
	::glDepthFunc(GL_EQUAL);									// Z 値が同じピクセルのみ描画
#if defined(_IOS) || defined(_ANDROID)
	::glDepthRangef(0.0f, 0.005f);	// Z 値の精度を一時的に落として描画されやすくする（シェーダーに移行するまでの暫定処理）
#else
	::glDepthRange(0.0, 0.005);		// Z 値の精度を一時的に落として描画されやすくする（シェーダーに移行するまでの暫定処理）
#endif
	::glEnableClientState(GL_VERTEX_ARRAY);
	Vector3F vtxs[4];
	if(_angleOpen < TFW_DPI)
	{
		for(int i = 0; i < _numVtxs - 1; i++)
		{
			vtxs[0].copy(&_posarrFanVtx[i + 0]);
			vtxs[1].copy(&_posarrFanVtx[i + 1]);
			vtxs[2].copy(&vtxs[0]);
			vtxs[3].copy(&vtxs[1]);
			vtxs[0].y() -= 1000.0f;
			vtxs[1].y() -= 1000.0f;
			vtxs[2].y() += 1000.0f;
			vtxs[3].y() += 1000.0f;
			::glVertexPointer(3, GL_FLOAT, 0, vtxs);
			::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		vtxs[0].copy(&_posarrFanVtx[_numVtxs - 1]);
		vtxs[1].copy(&_posarrFanVtx[0]);
		vtxs[2].copy(&vtxs[0]);
		vtxs[3].copy(&vtxs[1]);
		vtxs[0].y() -= 1000.0f;
		vtxs[1].y() -= 1000.0f;
		vtxs[2].y() += 1000.0f;
		vtxs[3].y() += 1000.0f;
		::glVertexPointer(3, GL_FLOAT, 0, vtxs);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	else
	{
		for(int i = 0; i < _numDiv; i++)
		{
			vtxs[0].copy(&_posarrFanVtx[i + 1]);
			vtxs[1].copy(&_posarrFanVtx[i + 2]);
			vtxs[2].copy(&vtxs[0]);
			vtxs[3].copy(&vtxs[1]);
			vtxs[0].y() -= 1000.0f;
			vtxs[1].y() -= 1000.0f;
			vtxs[2].y() += 1000.0f;
			vtxs[3].y() += 1000.0f;
			::glVertexPointer(3, GL_FLOAT, 0, vtxs);
			::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	// 範囲円を円盤で描く
	rc->getRenderer()->setColorAlpha(_color->a() / 8);			// 透明度の設定
	rc->getRenderer()->setZTest(false);							// Z テストなし
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(3, GL_FLOAT, 0, _posarrFanVtx);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, _numVtxs);

	// 描画設定を戻す
	::glDisableClientState(GL_VERTEX_ARRAY);
	::glDepthFunc(GL_LEQUAL);					// Z 値が小さいもの（近いもの）が Z テスト合格
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS