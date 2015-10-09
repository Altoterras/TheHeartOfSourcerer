/***********************************************************************//**
 *  AnimCamera.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/26.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AnimCamera.h"

// Friends
#include "../../../CamAnim.h"
#include "../../../CamAnimFrame.h"
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"
#include "../../../../gcmn/Gcalc.h"
#include "../../../../gcmn/Renderer.h"
#include "../../../../gcmn/RendererUtils.h"
#include "../../../../gcmn/View.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アニメーションセット
**//*---------------------------------------------------------------------*/
void AnimCamera::setAnim(CamAnim* camanimRef)
{
	_camanimRef = camanimRef;

	if(_camanimRef != 0L)
	{
		_fovy = _camanimRef->getFovY();
		_cnear = _camanimRef->getNearClip();
		_cfar = _camanimRef->getFarClip();
	}
}

/*---------------------------------------------------------------------*//**
	最大フレーム数を得る
**//*---------------------------------------------------------------------*/
s32 AnimCamera::getMaxFrame() const
{
	if(_camanimRef == 0L)	{	return 0;	}
	return _camanimRef->getMaxFrame();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AnimCamera::AnimCamera()
	: _camanimRef(0L)
	, _isDoneAnim(false)
{
	_camclass = CAMCLS_ANIM;
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool AnimCamera::create()
{
	// fov, near, far は後でアニメーションデータから設定されるので、ここでは適当なデフォルト値
	return LookAtCamera::create(10.0f, 100.0f, 100000.0f, F_NO_CALC_INVMTX);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void AnimCamera::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isVisible())		{	return;	}
	if(_camanimRef == 0L)	{	return;	}
	if(TFW_IS_FLAG(ec->getExecFlags(), ExecCtx::EF_PAUSE_ANIM))	{	return;	}	// 一時停止中

	// アニメーションフレームを進める
	u16 idxCafCur = CamAnim::CAFIDX_INVALID;
	u16 idxCafNext = CamAnim::CAFIDX_INVALID;
	if(_camanimRef->advNextFrame(&idxCafCur, &idxCafNext, ec->getDeltaFrame()) == CamAnim::ADVRES_END)
	{
		_isDoneAnim = true;
	}
	f32 frame = _camanimRef->getFrame();

	// 補間
	ASSERT((idxCafCur == CamAnim::CAFIDX_INVALID) || ((0 <= idxCafCur) && (idxCafCur < _camanimRef->getKeyFrameNum())));
	ASSERT((idxCafNext == CamAnim::CAFIDX_INVALID) || ((0 <= idxCafNext) && (idxCafNext < _camanimRef->getKeyFrameNum())));
	const CamAnimFrame* caf1 = (idxCafCur != CamAnim::CAFIDX_INVALID) ? _camanimRef->getAnimFrame(idxCafCur) : 0L;
	const CamAnimFrame* caf2 = (idxCafNext != CamAnim::CAFIDX_INVALID) ? _camanimRef->getAnimFrame(idxCafNext) : 0L;
	bool isIntp = !TFW_IS_FLAG(ec->getExecFlags(), ExecCtx::EF_NO_ANIM_INTERP);	// 補間は有効か

	// フレーム間補間
	if((caf1 == 0L) && (caf2 == 0L))				// キーフレーム無し
	{
	}
	else if((caf1 != 0L) && (caf2 == 0L))			// ジャストフレーム
	{
		_loc->copy(caf1->_loc);
		_aim->copy(caf1->_aim);
		_lup->copy(caf1->_lookup);
	}
	else if((caf1 != 0L) && (caf2 != 0L))			// ２点間
	{
		if(isIntp)		// 補間あり
		{
			f32 rate = (frame - caf1->_frame) / (caf2->_frame - caf1->_frame);
			Gcalc::interpolate(_loc, caf1->_loc, caf2->_loc, rate);
			Gcalc::interpolate(_aim, caf1->_aim, caf2->_aim, rate);
			Gcalc::interpolate(_lup, caf1->_lookup, caf2->_lookup, rate);
		}
		else			// 補間なし
		{
			_loc->copy(caf1->_loc);
			_aim->copy(caf1->_aim);
			_lup->copy(caf1->_lookup);
		}
	}
	else if((caf1 == 0L) && (caf2 != 0L))			// 初期フレームまでの間
	{
		_loc->copy(caf2->_loc);
		_aim->copy(caf2->_aim);
		_lup->copy(caf2->_lookup);
	}

	LookAtCamera::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	カメラ座標（ビュー座標）マトリックスの計算
**//*---------------------------------------------------------------------*/
void AnimCamera::calcViewMatrix()
{
	// カメラの姿勢を計算する
	Gcalc::lookat( _mtxView, _loc, _aim, _lup );
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS