/***********************************************************************//**
 *  WaveDef.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/10/09.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WaveDef.h"

// Friends
#include "../string/VcString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define GAIN_DEFAULT				(1.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WaveDef メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WaveDef::WaveDef(u16 id, const CStringBase* pathDir, const CStringBase* filename, const CStringBase* type, s32 smplLoopStart, s32 smplLoopEnd)
	: _id(id)
	, _wfmt(0)
	, _fileid(0)
	, _pathDir(new VcString(pathDir))
	, _filename(new VcString(filename))
	, _type(new VcString(type))
	, _smplLoopStart(smplLoopStart)
	, _smplLoopEnd(smplLoopEnd)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WaveDef::WaveDef(u16 id, u32 fileid, u8 wfmt, s32 smplLoopStart, s32 smplLoopEnd)
	: _id(id)
	, _wfmt(wfmt)
	, _fileid(fileid)
	, _pathDir(0L)
	, _filename(0L)
	, _type(0L)
	, _smplLoopStart(smplLoopStart)
	, _smplLoopEnd(smplLoopEnd)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
WaveDef::~WaveDef()
{
	delete _pathDir;
	delete _filename;
	delete _type;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS