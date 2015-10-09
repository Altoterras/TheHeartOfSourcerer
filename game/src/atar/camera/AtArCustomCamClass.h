/***********************************************************************//**
 *	AtArCustomCamClass.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/18.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_CAMERA_AT_AR_CUSTOM_CAM_CLASS_H_
#define _ATAR_CAMERA_AT_AR_CUSTOM_CAM_CLASS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/camera/EtkCustomCamClass.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カスタム カメラ クラス定義
 *
**//*---------------------------------------------------------------------*/
class AtArCustomCamClass
{
	//======================================================================
	// 定数
public:
	// EtkCustomCamClass::CAMCLS__ETK_CUSTOM_END_ の続きを定義する
	static const u16 CAMCLS__GAME_CUSTOM_START_	= EtkCustomCamClass::CAMCLS__ETK_CUSTOM_END_ + 1;

	static const u16 CAMCLS_AR					= CAMCLS__GAME_CUSTOM_START_ + 1;

	static const u16 CAMCLS__GAME_CUSTOM_END_	= CAMCLS__GAME_CUSTOM_START_ + 999;

};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_CAMERA_AT_AR_CUSTOM_CAM_CLASS_H_
