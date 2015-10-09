/***********************************************************************//**
 *  HrAnimPlayingFrame.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/11/19.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimPlayingFrame.h"

// Friends
#include "HrAnim.h"
#include "HrAnimObjFrame.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimPlayingFrame メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimPlayingFrame::HrAnimPlayingFrame(s32 numShapes)
{
	_aopfShapes = new HrAnimObjPlayingFrame[numShapes];

	_aofIntpShapes = new HrAnimObjFrame[numShapes];

	_idxarrAofTmpShapes = new u16[numShapes];
	for(int ishape = 0; ishape < numShapes; ishape++)
	{
		_idxarrAofTmpShapes[ishape] = HrAnim::AOFIDX_VALID_MAX;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnimPlayingFrame::~HrAnimPlayingFrame()
{
	delete[] _aopfShapes;

	delete[] _aofIntpShapes;

	delete[] _idxarrAofTmpShapes;
}

//==========================================================================
// HrAnimPlayingFrame::HrAnimObjPlayingFrame メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimPlayingFrame::HrAnimObjPlayingFrame::HrAnimObjPlayingFrame()
{
	for(int kp = 0; kp < NUM_KP; kp++)
	{
		_aof[kp] = 0L;
		_rframe[kp] = 0.0f;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS