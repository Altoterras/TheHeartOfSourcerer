/***********************************************************************//**
 *  CamAnimFrame.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/25.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_CAM_ANIM_FRAME_H_
#define _TFW_G3D_CAM_ANIM_FRAME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

/*---------------------------------------------------------------------*//**
 *	階層アニメ フレーム情報 クラス
 *
**//*---------------------------------------------------------------------*/
class CamAnimFrame
{
public:
	s32			_frame;
	Vector3F*	_loc;
	Vector3F*	_aim;
	Vector3F*	_lookup;

	CamAnimFrame();
	~CamAnimFrame();

	void set(s32 frame, const Vector3F* loc, const Vector3F* aim, const Vector3F* lookup);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_CAM_ANIM_FRAME_H_