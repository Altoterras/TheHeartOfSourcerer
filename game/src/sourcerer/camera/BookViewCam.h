/***********************************************************************//**
 *	BookViewCam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/31.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CAMERA_BOOK_VIEW_CAM_H_
#define _SRCR_CAMERA_BOOK_VIEW_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	モデルビュー カメラ
 *
**//*---------------------------------------------------------------------*/
class BookViewCam : public Camera
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setNoUiReact(bool isNoUiReact) { _isNoUiReact = isNoUiReact; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BookViewCam();
	virtual ~BookViewCam();

	bool			create();
	void			destroy();

	virtual void	exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:
	virtual void	renderImplements(const RenderCtx* rc);	// 描画
	virtual void	calcViewMatrix();

	//======================================================================
	// メンバ変数
private:
	Vector3F*			_loc;
	Vector3F*			_dir;
	Vector3F*			_ang;
	bool				_isNoUiReact;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CAMERA_BOOK_VIEW_CAM_H_
