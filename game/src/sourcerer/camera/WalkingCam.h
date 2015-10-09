/***********************************************************************//**
 *	WalkingCam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CAMERA_WALKING_CAM_H_
#define _SRCR_CAMERA_WALKING_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/LookAtCamera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class MapConfig;
class VelocityInterpolation;

/*---------------------------------------------------------------------*//**
 *	歩行移動時カメラ
 *
**//*---------------------------------------------------------------------*/
class WalkingCam : public LookAtCamera
{
	//======================================================================
	// 定数
private:
	// カメラ状態
	static const u8 STATE_STABLE		= 0;	// 定常
	static const u8 STATE_LEAVING		= 1;	// 離脱中
	static const u8 STATE_RETURN		= 2;	// 復帰中
	static const u8 STATE_SLOW_RETURN	= 3;	// 低速復帰中

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユーザーが更に上方にカメラ移動しようとした
	inline bool isInputOutRange() const { return _isOutRangeY; }
	// 外部コントロールに設定する（フレームごとにクリアされるので、毎フレーム設定する必要がある）
	inline void setOutControl(bool isOut) { _isOutCtrl = isOut; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setZoom(f32 zoom);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WalkingCam();
	~WalkingCam();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void notifyVisibleChange(bool isVisibleNew);
	void notifyChangedMap();

private:
	void calcTargetPos(const MapConfig* mconf);
	void calcValidPos();
	void toPreferablyStable();

	//======================================================================
	// メンバ変数
private:
	Vector3F* _locTrg;
	Vector3F* _aimTrg;
	Vector3F* _velLoc;
	Vector3F* _velAim;
	VelocityInterpolation* _vintpLoc;
	VelocityInterpolation* _vintpAim;
	f32 _yZoomCur;
	f32 _yZoomMin;
	f32 _yZoomMax;
	f32 _yZoomDefault;
	f32 _vyZoom;
	bool _isOutRangeY;
	bool _isOutCtrl;
	bool _isReverseWhenReturnState;
	u8 _state;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CAMERA_WALKING_CAM_H_
