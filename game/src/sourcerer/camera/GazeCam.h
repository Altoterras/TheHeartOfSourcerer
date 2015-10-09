/***********************************************************************//**
 *	GazeCam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CAMERA_GAZE_CAM_H_
#define _SRCR_CAMERA_GAZE_CAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/implement/LocAngCamera.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	対象注視カメラ
 *
**//*---------------------------------------------------------------------*/
class GazeCam : public LocAngCamera
{
	//======================================================================
	// 定数
public:
	// 動作モード
	enum ModeKind
	{
		MODE_NULL,
		MODE_CHANTING,		// ソウマ詠唱中
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// カメラから抜けるかどうかを得る
	inline bool isLeaveRequest() const { return _isLeaveRequest; }

	// モード設定
	inline void setMode(ModeKind mode) { _mode = mode; }
	// ターゲット設定
	inline void setTarget(const Unit* unit) { _unitTrg = unit; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setVisible(bool isVisible);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GazeCam();
	virtual ~GazeCam();
	bool create();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	//======================================================================
	// 変数
private:
	const Unit*			_unitTrg;
	ModeKind			_mode;
	f32					_rotY;
	bool				_isLeaveRequest;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CAMERA_GAZE_CAM_H_
