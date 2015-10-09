/***********************************************************************//**
 *	WorldMapCam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CAMERA_WALKING_CAM_H_
#define _SRCR_CAMERA_WALKING_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/LocAngCamera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	歩行移動時カメラ
 *
**//*---------------------------------------------------------------------*/
class WorldMapCam : public LocAngCamera
{
	//======================================================================
	// 定数

public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユーザーが更に上方にカメラ移動しようとした
	inline bool isInputOutRange() const { return _isOutRangeY; }
	// 基本位置へのポインタを得る
	inline const Vector3F* getBasePos() const { return _posBase; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WorldMapCam();
	virtual ~WorldMapCam();
	bool create();
	void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyVisibleChange(bool isVisibleNew);
	void notifyBegin(const Vector3F* posBase);
	void notifyEnd();

	//======================================================================
	// メンバ変数
private:
	Vector3F* _posBase;
	Vector3F* _posBaseClickStart;
	Vector3F* _locTrg;
	Vector3F* _angTrg;
	f32 _yZoom;
	bool _isOutRangeY;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CAMERA_WALKING_CAM_H_
