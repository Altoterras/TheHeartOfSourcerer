/***********************************************************************//**
 *	GameCustomCamClass.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CAMERA_GAME_CUSTOM_CAM_CLASS_H_
#define _SRCR_CAMERA_GAME_CUSTOM_CAM_CLASS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/camera/EtkCustomCamClass.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カスタム カメラ クラス定義
 *
**//*---------------------------------------------------------------------*/
class GameCustomCamClass
{
	//======================================================================
	// 定数
public:
	// EtkCustomCamClass::CAMCLS__ETK_CUSTOM_END_ の続きを定義する
	static const u16 CAMCLS__GAME_CUSTOM_START_	= EtkCustomCamClass::CAMCLS__ETK_CUSTOM_END_ + 1;

	static const u16 CAMCLS_WALKING				= CAMCLS__GAME_CUSTOM_START_ + 1;
	static const u16 CAMCLS_CONFRONT			= CAMCLS__GAME_CUSTOM_START_ + 2;
	static const u16 CAMCLS_GAZE				= CAMCLS__GAME_CUSTOM_START_ + 3;
	static const u16 CAMCLS_WORLDMAP			= CAMCLS__GAME_CUSTOM_START_ + 4;
	static const u16 CAMCLS_BOOKVIEW			= CAMCLS__GAME_CUSTOM_START_ + 5;

	static const u16 CAMCLS__GAME_CUSTOM_END_	= CAMCLS__GAME_CUSTOM_START_ + 999;

};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CAMERA_GAME_CUSTOM_CAM_CLASS_H_
