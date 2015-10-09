/***********************************************************************//**
 *	ModelViewCam.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/07/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ModelViewCam.h"

// Friends
#include "EtkCustomCamClass.h"
#include "../body/EtkExecCtx.h"

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

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ModelViewCam 静的メンバ変数

//==========================================================================
// ModelViewCam メソッド
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ModelViewCam::ModelViewCam()
	: _loc(0L)
	, _dir(0L)
	, _ang(0L)
	, _isNoUiReact(false)
{
	_camclass = EtkCustomCamClass::CAMCLS_MODELVIEW;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ModelViewCam::~ModelViewCam()
{
	ASSERT(_ang == 0L);
	ASSERT(_dir == 0L);
	ASSERT(_loc == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ModelViewCam::create()
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
void ModelViewCam::destroy()
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
void ModelViewCam::calcViewMatrix()
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
void ModelViewCam::exec(ExecRes* res, const ExecCtx* ec)
{
#if 0

	// 角度 (0,0,0) で、単位ベクトル (0,0,1) を基準に考える
	//
	//	ang x
	//	│z
	//	↑　／
	//	│／θ  y
	//	┼───
	//
	//	ang y
	//	│x
	//	│　／
	//	│／θ  z
	//	┼─→─
	//
	//	ang z
	//	│y
	//	│　／
	//	│／θ  x
	//	┼───
	//
	Vector3F pos;
	pos.x() = _dist * ::sinf(_angy);
	pos.y() = - _dist * ::sinf(_angx);
	pos.z() = _dist * ::cosf(_angx) * ::cosf(_angy);
	setLoc(&pos);
	Matrix4F mtxRot;
	Vector3F rot;
//	if(pos.z() >= 0.0f)	{	rot.x() = - ::atanf( pos.y() / pos.z() );			}
//	else				{	rot.x() = Gcalc::adjustAngle(TFW_PI - ::atanf( pos.y() / pos.z() ));	}
	rot.x() = - ::atan2f( pos.y(), pos.z() );
	mtxRot.rotateX(- rot.x());
	mtxRot.mult(&pos, &pos);
//	if(pos.z() >= 0.0f)	{	rot.y() = ::atanf( pos.x() / pos.z() );				}
//	else				{	rot.y() = Gcalc::adjustAngle(TFW_PI + ::atanf( pos.x() / pos.z() ));	}
	rot.y() = ::atan2f( pos.x(), pos.z() );
	mtxRot.rotateY(- rot.y());
	setAngle(&rot);

	mtxRot.inverse();
	TouchPanel* ui = ((GameExecCtx*)ec)->getTouchPanel();
	switch(ui->getFunction())
	{
	case TouchPanel::ACT_CLICK:
		break;
	case TouchPanel::ACT_DRAG:
		{
			PointF32 ptCur, ptPrev;
			ui->getDragParam(0L, &ptCur, &ptPrev);
			f32 dx = (ptCur.y() - ptPrev.y()) * 0.001f;
			f32 dy = (ptCur.x() - ptPrev.x()) * 0.001f;
			Vector3F v(
				dx,
				dy,
				0.0f);//::sqrtf(dx * dx + dy * dy) );
			mtxRot.mult(&v, &v);
			_angx += - ::atan2f( v.y(), v.z() );
			_angy += ::atan2f( v.x(), v.z() );
		}
		break;
	case TouchPanel::ACT_FLIP:
		{
			PointF32 ptCur, ptPrev;
			ui->getFlipParam(0L, &ptCur, &ptPrev);
			f32 dx = (ptCur.y() - ptPrev.y()) * 10.005f;
			f32 dy = (ptCur.x() - ptPrev.x()) * 10.005f;
			Vector3F v(
				dx,
				dy,
				0.0f);//::sqrtf(dx * dx + dy * dy) );
			mtxRot.mult(&v, &v);
			_angx += - ::atan2f( v.y(), v.z() );
			_angy += ::atan2f( v.x(), v.z() );
		}
		break;
	case TouchPanel::ACT_ZOOM:
		{
			int rCur, rPrev;
			ui->getZoomParam(0L, 0L, &rCur, &rPrev);
			_dist += rCur - rPrev;
		}
		break;
	case TouchPanel::ACT_CANCEL:
		break;
	}

#else

	if(!isVisible())	{	return;	}

	if(!_isNoUiReact)
	{
		TouchPanel* touchp = ((EtkExecCtx*)ec)->getTouchPanel();
		Keyboard* keybrd = ((EtkExecCtx*)ec)->getKeyboard();
		if(!touchp->isReacted())
		{
			switch(touchp->getLastAction())
			{
			case TouchPanel::ACT_CLICK:
				break;
			case TouchPanel::ACT_DRAG:
				if(keybrd->isOn(Keyboard::SYSKEY_ALT))
				{
					goto ROTATION;
				}
				else
				{
					PointF32 ptCur, ptPrev;
					touchp->getDragParam(0L, &ptCur, &ptPrev);
					_loc->x() -= (ptCur.x() - ptPrev.x()) * 3.8f;
					_loc->y() += (ptCur.y() - ptPrev.y()) * 3.8f;
					touchp->setReacted(true);	// 反応済みフラグを立てる
				}
				break;
			case TouchPanel::ACT_FLIP: ROTATION:
				{
					PointF32 ptCur, ptPrev;
					touchp->getFlipParam(0L, 0L, &ptCur, &ptPrev);
					_dir->x() = Gcalc::adjustAngle(_dir->x() + (ptCur.y() - ptPrev.y()) * 0.001f);
					_dir->y() = Gcalc::adjustAngle(_dir->y() + (ptCur.x() - ptPrev.x()) * 0.001f);
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

#endif
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ModelViewCam::renderImplements(const RenderCtx* rc)
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
	// Z
	rdr->setSolidColor(0, 0, 255, 255);
	static const Vector3F vtxZ[2] = { Vector3F( 0.0f, 0.0f, 0.0f ), Vector3F( 0.0f, 0.0f, len ) };
	RendererUtils::render3dLines(rdr, vtxZ, 2, RendererUtils::LINES);
	// 色を白に戻す
	rdr->setSolidColor(255, 255, 255, 255);	// 色を戻す
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
