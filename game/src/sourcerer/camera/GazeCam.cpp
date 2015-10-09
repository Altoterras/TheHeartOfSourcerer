/***********************************************************************//**
 *	GazeCam.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GazeCam.h"

// Friends
#include "GameCustomCamClass.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../map/Map.h"
#include "../unit/UnitManager.h"
#include "../unit/char/CharUnit.h"

// External
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GazeCam 静的メンバ変数

//==========================================================================
// GazeCam メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	有効／無効化切替
**//*---------------------------------------------------------------------*/
void GazeCam::setVisible(bool isVisible)
{
	LocAngCamera::setVisible(isVisible);

	if(isVisible)
	{
		// 値のクリア
		_isLeaveRequest = false;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GazeCam::GazeCam()
	: _unitTrg(0L)
	, _mode(MODE_NULL)
	, _rotY(0.0f)
	, _isLeaveRequest(false)
{
	_camclass = GameCustomCamClass::CAMCLS_GAZE;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GazeCam::~GazeCam()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GazeCam::create()
{
	if(!LocAngCamera::create(10.0f, 100.0f, 100000.0f, 0))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void GazeCam::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isVisible())	{	return;	}

	// ズーム処理によって注視を抜ける
	TouchPanel* ui = ((GameExecCtx*)ec)->getTouchPanel();
	switch(ui->getLastAction())
	{
	case TouchPanel::ACT_ZOOM:
		_isLeaveRequest = true;
		break;
	}

	// ターゲット座標を求める
	Vector3F loc;
	Vector3F ang;
	if(_unitTrg != 0L)
	{
		const Vector3F* posUnit = _unitTrg->getCenterPos();
		Vector3F posGaze(posUnit);
		loc.copy(posUnit);
		loc.y() += 500.0f;
		loc.z() += 1000.0f;

		f32 yDiff = - (loc.y() - posUnit->y());
		f32 zDiff = - (loc.z() - posUnit->z());
		ang.x() = - ::atanf( yDiff / zDiff );
	}

	// ターゲット座標への補間
	setLoc(&loc);
	setAngle(&ang);

	LocAngCamera::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
