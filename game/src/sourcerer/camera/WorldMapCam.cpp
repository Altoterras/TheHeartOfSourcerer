/***********************************************************************//**
 *	WorldMapCam.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WorldMapCam.h"

// Friends
#include "GameCustomCamClass.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"

// External
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define	Y_ZOOM_MIN			(0.0f)		// ズーム高さの最小値
#define	Y_ZOOM_MAX			(3800.0f)	// ズーム高さの最大値
#define	Y_ZOOM_DEFAULT		(0.0f)		// デフォルトのズーム高さ
#define RATIO_ZOOM			(0.25f)		// タッチ座標に対するズーム率
#define X_MIN				(50.0f)		// X 位置最小値
#define X_MAX				(950.0f)	// X 位置最大値
#define Z_MIN				(100.0f)	// Z 位置最小値
#define Z_MAX				(800.0f)	// Z 位置最大値

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WorldMapCam メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WorldMapCam::WorldMapCam()
	: _posBase(0L)
	, _posBaseClickStart(0L)
	, _locTrg(0L)
	, _angTrg(0L)
	, _yZoom(Y_ZOOM_DEFAULT)
	, _isOutRangeY(false)
{
	_camclass = GameCustomCamClass::CAMCLS_WORLDMAP;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
WorldMapCam::~WorldMapCam()
{
	ASSERT(_posBase == 0L);
	ASSERT(_posBaseClickStart == 0L);
	ASSERT(_locTrg == 0L);
	ASSERT(_angTrg == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool WorldMapCam::create()
{
	if(!LocAngCamera::create(10.0f, 100.0f, 100000.0f, 0))
	{
		return false;
	}

	_posBase = new Vector3F();
	_posBaseClickStart = new Vector3F(0.0f, F32_PMAX, 0.0f);
	_locTrg = new Vector3F();
	_angTrg = new Vector3F();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void WorldMapCam::destroy()
{
	delete _angTrg;
	_angTrg = 0L;
	delete _locTrg;
	_locTrg = 0L;
	delete _posBaseClickStart;
	_posBaseClickStart = 0L;
	delete _posBase;
	_posBase = 0L;

	LocAngCamera::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void WorldMapCam::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isVisible())	{	return;	}

	// 移動・ズーム処理
	TouchPanel* ui = ((GameExecCtx*)ec)->getTouchPanel();
	switch(ui->getLastAction())
	{
	case TouchPanel::ACT_ZOOM:
		{
			f32 rCur, rPrev;
			bool isMinY = (_yZoom == Y_ZOOM_MIN);
			ui->getZoomParam(0L, 0L, &rCur, &rPrev);
			_yZoom -= (rCur - rPrev) * RATIO_ZOOM * (1.0f + ((_yZoom - Y_ZOOM_MIN) / (Y_ZOOM_MAX - Y_ZOOM_MIN)));	// 高い方が変化量が多いように調節
			if(_yZoom < Y_ZOOM_MIN)
			{
				_yZoom = Y_ZOOM_MIN;

				if(isMinY)	// 最小高さの位置から更に下に下がろうとした
				{
					_isOutRangeY = true;
				}
			}
			else if(_yZoom > Y_ZOOM_MAX)
			{
				_yZoom = Y_ZOOM_MAX;
			}
			
			_posBaseClickStart->y() = F32_PMAX;	// ドラッグ開始位置を初期化
			
			ui->setReacted(true);	// 反応済みフラグを立てる
		}
		break;
	case TouchPanel::ACT_DRAG:
		{
			if(_posBaseClickStart->y() == F32_PMAX)	// ドラッグ開始位置が設定されていない場合
			{
				_posBaseClickStart->copy(_posBase);	// ここを基準にする
			}
			
			PointF32 ptStart, ptCur, ptPrev;
			ui->getDragParam(&ptStart, &ptCur, &ptPrev);
			f32 yRatio = 0.11f + _yZoom / 30000.0f * 16.5f;
			f32 dx = - (ptCur.x() - ptStart.x()) * yRatio;
			f32 dy = - (ptCur.y() - ptStart.y()) * yRatio;
			_posBase->x() = _posBaseClickStart->x() + dx;
			_posBase->z() = _posBaseClickStart->z() + dy;
		}
		break;
	}
	if(ui->isTapTrigger(0L))
	{
		_posBaseClickStart->copy(_posBase);
	}

	// ターゲット座標を求める
	if(_posBase->x() < X_MIN)		{	_posBase->x() = X_MIN;	}
	else if(_posBase->x() > X_MAX)	{	_posBase->x() = X_MAX;	}
	if(_posBase->z() < Z_MIN)		{	_posBase->z() = Z_MIN;	}
	else if(_posBase->z() > Z_MAX)	{	_posBase->z() = Z_MAX;	}
	_locTrg->set(_posBase->x(), _posBase->y() + _yZoom, _posBase->z());

	// ターゲット座標への補間
	f32 rateLoc = 0.8f * ec->getDeltaFrame();
	if(rateLoc > 1.0f)	{	rateLoc = 1.0f;	}
	f32 rrateLoc = 1.0f - rateLoc;
	Vector3F locNew((_loc->_v[0] * rrateLoc) + (_locTrg->_v[0] * rateLoc), (_loc->_v[1] * rrateLoc) + (_locTrg->_v[1] * rateLoc), (_loc->_v[2] * rrateLoc) + (_locTrg->_v[2] * rateLoc));
	setLoc(&locNew);
	f32 rateAng = 0.7f * ec->getDeltaFrame();
	if(rateAng > 1.0f)	{	rateAng = 1.0f;	}
	f32 rrateAng = 1.0f - rateAng;
	Vector3F angNew((_ang->_v[0] * rrateAng) + (_angTrg->_v[0] * rateAng), (_ang->_v[1] * rrateAng) + (_angTrg->_v[1] * rateAng), (_ang->_v[2] * rrateAng) + (_angTrg->_v[2] * rateAng));
	setAngle(&angNew);

	LocAngCamera::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	有効化・無効化通知
**//*---------------------------------------------------------------------*/
void WorldMapCam::notifyVisibleChange(bool isVisibleNew)
{
	// 範囲外移動フラグのクリア
	_isOutRangeY = false;
}

/*---------------------------------------------------------------------*//**
	開始通知
**//*---------------------------------------------------------------------*/
void WorldMapCam::notifyBegin(const Vector3F* posBase)
{
	// 基本位置を設定する
	*_posBase = *posBase;

	// 初期位置を設定する
	_yZoom = Y_ZOOM_MIN;
	_locTrg->set(_posBase->x(), _posBase->y() + _yZoom, _posBase->z());
	setLoc(_locTrg);
	_angTrg->set(- TFW_HPI, 0.0f, 0.0f);
	setAngle(_angTrg);
}

/*---------------------------------------------------------------------*//**
	終了通知
**//*---------------------------------------------------------------------*/
void WorldMapCam::notifyEnd()
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
