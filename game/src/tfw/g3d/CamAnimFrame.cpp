/***********************************************************************//**
 *  CamAnimFrame.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/25.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CamAnimFrame.h"

// Friends
#include "../lib/Vector.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CamAnimFrame::CamAnimFrame メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CamAnimFrame::CamAnimFrame()
	: _frame(0)
	, _loc(new Vector3F())
	, _aim(new Vector3F())
	, _lookup(new Vector3F())
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CamAnimFrame::~CamAnimFrame()
{
	delete _lookup;
	delete _aim;
	delete _loc;
}

/*---------------------------------------------------------------------*//**
	値の設定
**//*---------------------------------------------------------------------*/
void CamAnimFrame::set(s32 frame, const Vector3F* loc, const Vector3F* aim, const Vector3F* lookup)
{
	_frame = frame;
	_loc->copy(loc);
	_aim->copy(aim);
	_lookup->copy(lookup);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS