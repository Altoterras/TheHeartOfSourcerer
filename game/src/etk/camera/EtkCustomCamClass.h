/***********************************************************************//**
 *	EtkCustomCamClass.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2012/06/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_CAMERA_CUSTOM_CAM_CLASS_H_
#define _ETK_CAMERA_CUSTOM_CAM_CLASS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カスタム カメラ クラス定義
 *
**//*---------------------------------------------------------------------*/
class EtkCustomCamClass
{
	//======================================================================
	// 定数
public:
	// Camera::CAMCLS__CUSTOM_ の続きを定義する
	static const u16 CAMCLS__ETK_CUSTOM_START_	= Camera::CAMCLS__CUSTOM_ + 1000;

	static const u16 CAMCLS_MODELVIEW			= CAMCLS__ETK_CUSTOM_START_ + 1;

	static const u16 CAMCLS__ETK_CUSTOM_END_	= CAMCLS__ETK_CUSTOM_START_ + 999;

};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_CAMERA_CUSTOM_CAM_CLASS_H_
