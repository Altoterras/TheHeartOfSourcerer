/***********************************************************************//**
 *  AnimCamera.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/26.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_ANIM_CAMERA_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_ANIM_CAMERA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "LookAtCamera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CamAnim;

/*---------------------------------------------------------------------*//**
 *	アニメーションにより制御されるカメラ
 *
**//*---------------------------------------------------------------------*/
class AnimCamera : public LookAtCamera
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline bool	isDoneAnim() const				{	return _isDoneAnim;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void		setAnim(CamAnim* camanimRef);
	s32			getMaxFrame() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AnimCamera();
	virtual bool		create();
	virtual void		exec(ExecRes* res, const ExecCtx* ec);

protected:
	virtual void		calcViewMatrix();

	//======================================================================
	// メンバ変数

protected:
	CamAnim*				_camanimRef;
	bool					_isDoneAnim;
};

TFW_END_NS
	
////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_IMPLEMENT_ANIM_CAMERA_H_
