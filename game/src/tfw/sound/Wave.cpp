/***********************************************************************//**
 *  Wave.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/04/16.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Wave.h"

// Friends
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Wave メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	ファイルパスから作成
**//*---------------------------------------------------------------------*/
bool Wave::create(const CStringBase* filepath, u16 cflag)
{
	File* file = new File(filepath, File::MODE_READ);
	if(!file->isOpened())	{	return false;	}

	return createFromFile(file, cflag);
}

/*---------------------------------------------------------------------*//**
	バッファから作成
**//*---------------------------------------------------------------------*/
bool Wave::create(const char* fbuf, int size, u16 cflag)
{
	MemoryFile* file = new MemoryFile(fbuf, size);
	if(!file->isOpened())	{	return false;	}

	return createFromFile(file, cflag);
}

/*---------------------------------------------------------------------*//**
	ファイル ID から作成
**//*---------------------------------------------------------------------*/
bool Wave::create(u32 fileid, u16 cflag)
{
	IdFile* file = new IdFile(fileid);
	if(!file->isOpened())	{	return false;	}

	return createFromFile(file, cflag);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS