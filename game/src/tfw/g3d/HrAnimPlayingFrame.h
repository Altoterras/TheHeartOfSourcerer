/***********************************************************************//**
 *  HrAnimPlayingFrame.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/11/19.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_PLAYING_FRAME_CTX_H_
#define _TFW_G3D_HR_ANIM_PLAYING_FRAME_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HrAnimObjFrame;

/*---------------------------------------------------------------------*//**
 *	階層アニメーション再生枠
 *
**//*---------------------------------------------------------------------*/
class HrAnimPlayingFrame
{
	//======================================================================
	// 定数
public:
	#define TEST_CUBIC_INTP			(0)		// 三次関数による補間

	enum KeyPoint
	{
#if TEST_CUBIC_INTP
		KP_M1,		// -1
#endif
		KP_P0,		// 0
		KP_P1,		// +1
#if TEST_CUBIC_INTP
		KP_P2,		// +2
#endif
		NUM_KP
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	階層アニメーション オブジェクト再生枠
	 *
	**//*-----------------------------------------------------------------*/
	class HrAnimObjPlayingFrame
	{
	public:
		const HrAnimObjFrame* _aof[NUM_KP];	// キーフレーム
		f32 _rframe[NUM_KP];				// 次のキーフレームまでの差分フレーム値

	public:
		HrAnimObjPlayingFrame();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimPlayingFrame(s32 numShapes);
	~HrAnimPlayingFrame();

	//======================================================================
	// 変数
public:
	HrAnimObjPlayingFrame* _aopfShapes;	// 再生枠ののシェイプ配列
	HrAnimObjFrame* _aofIntpShapes;		// 補間開始姿勢用アニメーションフレーム情報のシェイプ配列
	u16* _idxarrAofTmpShapes;			// 一次計算用 HrAnimObjFrame のインデックスのシェイプ配列
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_PLAYING_FRAME_CTX_H_