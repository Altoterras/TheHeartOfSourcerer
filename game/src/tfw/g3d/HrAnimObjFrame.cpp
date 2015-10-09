/***********************************************************************//**
 *  HrAnimObjFrame.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimObjFrame.h"

// Friends
#include "Shape.h"
#include "../lib/Quaternion.h"
#include "../lib/Vector.h"
#include "../tbf/TbfDefine.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimObjFrame メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	値の設定
**//*---------------------------------------------------------------------*/
void HrAnimObjFrame::set(s32 frame, const Vector3F* trs, const Vector3F* angle, const Vector3F* scl, HrAnimObj* aobjTrgRef)
{
	_frame = frame;
	_pos.set(trs, angle, scl);
	_aobjTrgRef = aobjTrgRef;
}

/*---------------------------------------------------------------------*//**
	値の設定
**//*---------------------------------------------------------------------*/
void HrAnimObjFrame::set(const TbfI1HrAnimPosture* tbfhapos, HrAnimObj* aobjTrgRef)
{
	_frame = tbfhapos->_frame;
	_pos.set((Vector3F*)tbfhapos->_translate, (Vector3F*)tbfhapos->_angle, (Vector3F*)tbfhapos->_scale);
	_aobjTrgRef = aobjTrgRef;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjFrame::HrAnimObjFrame()
	: _frame(0)
	, _aobjTrgRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void HrAnimObjFrame::copy(const HrAnimObjFrame* src)
{
	this->_frame = src->_frame;
	this->_pos.copy(&src->_pos);
	this->_aobjTrgRef = src->_aobjTrgRef;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS