/***********************************************************************//**
 *  LocAngCamera.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/04/16.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_LOC_ANG_CAMERA_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_LOC_ANG_CAMERA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Camera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

/*---------------------------------------------------------------------*//**
 *　位置と角度によるカメラ
 *
**//*---------------------------------------------------------------------*/
class LocAngCamera : public Camera
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 位置を得る
	inline const Vector3F* getLoc() const				{	return _loc;				}
	// 角度を得る
	inline const Vector3F* getAngle() const				{	return _ang;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setLoc(const Vector3F* loc);
	virtual void setAngle(const Vector3F* ang);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	LocAngCamera();
	virtual ~LocAngCamera();
	virtual bool		create(f32 fovy, f32 cnear, f32 cfar, u16 flags);
	virtual void		destroy();

protected:
	virtual void		calcViewMatrix();

	//======================================================================
	// メンバ変数
protected:
	Vector3F*			_loc;
	Vector3F*			_ang;
};

TFW_END_NS
	
////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_IMPLEMENT_LOC_ANG_CAMERA_H_
