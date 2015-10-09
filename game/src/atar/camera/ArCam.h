/***********************************************************************//**
 *	ArCam.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/18.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_CAMERA_AR_CAM_H_
#define _ATAR_CAMERA_AR_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	モデルビュー カメラ
 *
**//*---------------------------------------------------------------------*/
class ArCam : public Camera
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	Vector3F* loc() { return _loc; }
	Vector3F* dir() { return _dir; }
	Vector3F* ang() { return _ang; }
	virtual const Vector3F* getLoc() const { return _loc; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ArCam();
	virtual ~ArCam();

	bool			create();
	void			destroy();

#if defined(_DEBUG)
	virtual void	exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
#endif

protected:
#if defined(_DEBUG)
	virtual void	renderImplements(const RenderCtx* rc);	// 描画
#endif
	virtual void	calcViewMatrix();

	//======================================================================
	// メンバ変数
private:
	Vector3F*			_loc;
	Vector3F*			_dir;
	Vector3F*			_ang;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_CAMERA_AR_CAM_H_
