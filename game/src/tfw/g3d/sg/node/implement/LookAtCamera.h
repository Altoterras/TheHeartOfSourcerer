/***********************************************************************//**
 *  LookAtCamera.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/04/16.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_LOOK_AT_CAMERA_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_LOOK_AT_CAMERA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Camera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	位置と注視点によるカメラ
 *
**//*---------------------------------------------------------------------*/
class LookAtCamera : public Camera
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 位置を得る
	inline const Vector3F* getLoc() const				{	return _loc;				}
	// 注視点を得る
	inline const Vector3F* getAim() const				{	return _aim;				}
	// 上方ベクトルを得る
	inline const Vector3F* getLookUp() const			{	return _lup;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setLoc(const Vector3F* loc);
	virtual void setAim(const Vector3F* aim);
	virtual void setLookUp(const Vector3F* lup);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	LookAtCamera();
	~LookAtCamera();
	virtual bool		create(f32 fovy, f32 cnear, f32 cfar, u16 flags);
	virtual void		destroy();

protected:
	virtual void		calcViewMatrix();

	//======================================================================
	// メンバ変数
protected:
	Vector3F*				_loc;
	Vector3F*				_aim;
	Vector3F*				_lup;
};

TFW_END_NS
	
////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_IMPLEMENT_LOOK_AT_CAMERA_H_
