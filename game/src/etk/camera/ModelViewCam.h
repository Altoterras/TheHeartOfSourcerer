/***********************************************************************//**
 *	ModelViewCam.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/07/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_CAMERA_MODEL_VIEW_CAM_H_
#define _ETK_CAMERA_MODEL_VIEW_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	モデルビュー カメラ
 *
**//*---------------------------------------------------------------------*/
class ModelViewCam : public Camera
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
	// 外部サービス
public:
	void setNoUiReact(bool isNoUiReact) { _isNoUiReact = isNoUiReact; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ModelViewCam();
	virtual ~ModelViewCam();
	bool create();
	void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:
	virtual void renderImplements(const RenderCtx* rc);	// 描画
	virtual void calcViewMatrix();

	//======================================================================
	// メンバ変数
private:
	Vector3F* _loc;
	Vector3F* _dir;
	Vector3F* _ang;
	bool _isNoUiReact;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_CAMERA_MODEL_VIEW_CAM_H_
