/***********************************************************************//**
 *	FontCharDefData.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/03/27.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FontCharDefData.h"

// Friends
#include "../../file/FileBase.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FontCharDefData メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	文字のテクスチャ配列上のインデックスを得る
**//*---------------------------------------------------------------------*/
u32 FontCharDefData::getCharIndex(const u16 c) const
{
	for(u32 i = 0; i < _countCharDef; i++)
	{
		if(_sarrCharDef[i] == c)
		{
			return i;
		}
	}
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FontCharDefData::FontCharDefData()
	: _countCharDef(0)
	, _sarrCharDef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FontCharDefData::~FontCharDefData()
{
	delete[] _sarrCharDef;
}

/*---------------------------------------------------------------------*//**
	読み込み処理
**//*---------------------------------------------------------------------*/
bool FontCharDefData::load(FileBase* file)
{
	// 総数を読み込む
	if(file->read(&_countCharDef, sizeof(u32)) < sizeof(u32))
	{
		return false;
	}

	// 文字列定義配列を作成する
	delete[] _sarrCharDef;
	_sarrCharDef = 0L;
	if(_countCharDef == 0)
	{
		return true;
	}
	_sarrCharDef = new u16[_countCharDef];
	if(_sarrCharDef == 0L)
	{
		return false;
	}

	// 文字列定義を読み込む
	if(file->read(_sarrCharDef, sizeof(u16) * _countCharDef) < (sizeof(u16) * _countCharDef))
	{
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
