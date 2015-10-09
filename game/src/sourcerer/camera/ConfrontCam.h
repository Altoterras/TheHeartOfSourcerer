/***********************************************************************//**
 *	ConfrontCam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/01/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CAMERA_CONFRONT_CAM_H_
#define _SRCR_CAMERA_CONFRONT_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/LookAtCamera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class Unit;
class VelocityInterpolation;

/*---------------------------------------------------------------------*//**
	歩行移動時カメラ

**//*---------------------------------------------------------------------*/
class ConfrontCam : public LookAtCamera
{
	//======================================================================
	// 定数
public:
	enum Mode
	{
		MODE_NULL,
		MODE_DEFAULT,
		MODE_WHOLE = MODE_DEFAULT,
		MODE_PARTY,
		MODE_TRG,
		MODE_GAZE,
		MODE_TWO,
		MODE_THREE,
	};

	// 演出フラグ
	static const u32 PERFF_ROTATE	= 0x00000001;	// 回転
	static const u32 PERFF_LEAVE	= 0x00000002;	// 離れる

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	Mode getCurrentMode() const { return _modeCur; }
	void setMode(Mode mode, const Unit* unit1, const Unit* unit2, const Unit* unit3, f32 framecnt);
	inline void setMode(Mode mode) { setMode(mode, 0L, 0L, 0L, 0.0f); }
	void startPerformance(u32 perfflags, f32 angle, f32 speed);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ConfrontCam();
	~ConfrontCam();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void notifyVisibleChange(bool isVisibleNew);

private:
	void aimUnits(const Unit* unit1, const Unit* unit2, const Unit* unit3, f32 yCam, f32 xCamAimDiff, f32 zCamAimDiff);

	//======================================================================
	// メンバ変数
private:
	Vector3F* _locTrg;
	Vector3F* _aimTrg;
	Vector3F* _velLoc;
	Vector3F* _velAim;
	VelocityInterpolation* _vintpLoc;
	VelocityInterpolation* _vintpAim;
	f32 _zZoomOfsCur;
	f32 _zZoomOfsTrg;
	Mode _modeCur;
	Mode _modePrev;

	u32 _perfflags;
	f32 _thetaCamPosRot;
	f32 _zCamPosLeave;
	f32 _speedPerf;

	f32 _fcntMode;
	const Unit* _unitModeUnit1;
	const Unit* _unitModeUnit2;
	const Unit* _unitModeUnit3;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CAMERA_CONFRONT_CAM_H_
