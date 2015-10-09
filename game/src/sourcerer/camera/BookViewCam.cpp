/***********************************************************************//**
 *	BookViewCam.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/31.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BookViewCam.h"

// Friends
#include "GameCustomCamClass.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"

// External
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Point.h"
#include "../../tfw/ui/Keyboard.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BookViewCam 静的メンバ変数

//==========================================================================
// BookViewCam メソッド
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BookViewCam::BookViewCam()
	: _loc(0L)
	, _dir(0L)
	, _ang(0L)
	, _isNoUiReact(false)
{
	_camclass = GameCustomCamClass::CAMCLS_BOOKVIEW;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BookViewCam::~BookViewCam()
{
	ASSERT(_ang == 0L);
	ASSERT(_dir == 0L);
	ASSERT(_loc == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BookViewCam::create()
{
	if(!Camera::create(10.0f, 100.0f, 100000.0f, 0))
	{
		return false;
	}

	_loc = new Vector3F(0.0f, 0.0f, 10000.0f);
	_dir = new Vector3F();
	_ang = new Vector3F();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BookViewCam::destroy()
{
	delete _ang;
	_ang = 0L;
	delete _dir;
	_dir = 0L;
	delete _loc;
	_loc = 0L;

	Camera::destroy();
}

/*---------------------------------------------------------------------*//**
	カメラ座標マトリックスの計算
**//*---------------------------------------------------------------------*/
void BookViewCam::calcViewMatrix()
{
	// 方向を先にしたオリジナルの計算法を使用する
	Vector3F trans(- *_loc);
	_mtxView->setIdentity();
	_mtxView->rotateZ(_dir->z());
	_mtxView->rotateY(_dir->y());
	_mtxView->rotateX(_dir->x());
	_mtxView->translate(&trans);
	_mtxView->rotateX(- _ang->x());
	_mtxView->rotateY(- _ang->y());
	_mtxView->rotateZ(- _ang->z());
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void BookViewCam::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isVisible())	{	return;	}

	if(!_isNoUiReact)
	{
		TouchPanel* touchp = ((GameExecCtx*)ec)->getTouchPanel();
		Keyboard* keybrd = ((GameExecCtx*)ec)->getKeyboard();
		if(!touchp->isReacted())
		{
			switch(touchp->getLastAction())
			{
			case TouchPanel::ACT_CLICK:
				break;
			case TouchPanel::ACT_DRAG:
			case TouchPanel::ACT_FLIP:
				{
					PointF32 ptCur, ptPrev;
					touchp->getFlipParam(0L, 0L, &ptCur, &ptPrev);
					_dir->x() = Gcalc::adjustAngle(_dir->x() + (ptCur.y() - ptPrev.y()) * 0.005f);
					touchp->setReacted(true);	// 反応済みフラグを立てる
				}
				break;
			case TouchPanel::ACT_ZOOM:
				{
					f32 rCur, rPrev;
					touchp->getZoomParam(0L, 0L, &rCur, &rPrev);
					_loc->z() -= rCur - rPrev;
					touchp->setReacted(true);	// 反応済みフラグを立てる
				}
				break;
			case TouchPanel::ACT_CANCEL:
				break;
			}
		}
	}

	Camera::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void BookViewCam::renderImplements(const RenderCtx* rc)
{
	if(!isVisible())	{	return;	}
	Camera::renderImplements(rc);

	// 軸線を描画する
	Renderer* rdr = rc->getRenderer();
	f32 len = 1000.0f;
	// X
	rdr->setSolidColor(255, 0, 0, 255);
	static const Vector3F vtxX[2] = { Vector3F( 0.0f, 0.0f, 0.0f ), Vector3F( len, 0.0f, 0.0f ) };
	RendererUtils::render3dLines(rdr, vtxX, 2, RendererUtils::LINES);
	// Y
	rdr->setSolidColor(0, 255, 0, 255);
	static const Vector3F vtxY[2] = { Vector3F( 0.0f, 0.0f, 0.0f ), Vector3F( 0.0f, len, 0.0f ) };
	RendererUtils::render3dLines(rdr, vtxY, 2, RendererUtils::LINES);
	// z
	rdr->setSolidColor(0, 0, 255, 255);
	static const Vector3F vtxZ[2] = { Vector3F( 0.0f, 0.0f, 0.0f ), Vector3F( 0.0f, 0.0f, len ) };
	RendererUtils::render3dLines(rdr, vtxZ, 2, RendererUtils::LINES);
	// 色を白に戻す
	rdr->setSolidColor(255, 255, 255, 255);	// 色を戻す
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
