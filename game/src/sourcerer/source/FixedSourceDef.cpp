/***********************************************************************//**
 *	FixedSourceDef.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FixedSourceDef.h"

// Friends

// External
#include "../../tfw/file/FileBase.h"
#include "../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FixedSourceDef メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FixedSourceDef::FixedSourceDef()
	: _fsrcdid(0)
	, _name(0L)
	, _sourceJs(0L)
	, _sourceSs(0L)
	, _explain(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FixedSourceDef::~FixedSourceDef()
{
	ASSERT(_name == 0L);
	ASSERT(_sourceJs == 0L);
	ASSERT(_sourceSs == 0L);
	ASSERT(_explain == 0L);
}

/*---------------------------------------------------------------------*//**
	データファイルからデータ部を読み込む
**//*---------------------------------------------------------------------*/
bool FixedSourceDef::loadFromDataFile(FileBase* file)
{
	char c = 0;
	VcString wk;

	s32 col = 0;
	while(true)
	{
		if(file->read(&c, sizeof(char)) != sizeof(char))
		{
			return false;
		}

		if((c == 0x1e) || (c == 0x1f))
		{
			if(col == 0)		// ID
			{
				_fsrcdid = StringUtils::toInteger(&wk);
			}
			if(col == 1)		// 名前
			{
				_name = new VcString(&wk);
			}
			else if(col == 2)	// ソース (JS)
			{
				_sourceJs = new VcString(&wk);
			}
			else if(col == 3)	// ソース (SS)
			{
				_sourceSs = new VcString(&wk);
			}
			wk.emptyEasy();

			if(c == 0x1e)
			{
				// 次のカラム
				col++;
			}
			else	// 0x1f
			{
				// 次の行
				return true;
			}
		}
		else
		{
			wk.add(c);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列を読み込む（loadFromStringFile のサブメソッド）
**//*---------------------------------------------------------------------*/
bool FixedSourceDef::loadString(VcString* str, FileBase* file)
{
	str->emptyEasy();

	while(true)
	{
		char c;
		if(file->read(&c, sizeof(char)) != sizeof(char))	{	return false;	}

		switch(c)
		{
		case ',':
			return true;
		case '\r':
			break;
		case '\n':
			return true;
		default:
			str->add(c);
			break;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	文字列ファイルから文字列部を読み込む
**//*---------------------------------------------------------------------*/
bool FixedSourceDef::loadFromStringFile(FileBase* file)
{
	VcString wk;

	// 日本語説明
	if(!loadString(&wk, file))	{	return false;	}
	if(Env_getLangId() == LANGID_JP)
	{
		_explain = new VcString(&wk);
	}

	// 英語説明
	if(!loadString(&wk, file))	{	return false;	}
	if(Env_getLangId() == LANGID_EN)
	{
		_explain = new VcString(&wk);
	}

	// 改行までまわす
	if(!loadString(&wk, file))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void FixedSourceDef::cleanup()
{
	delete _sourceSs;
	_sourceSs = 0L;
	delete _sourceJs;
	_sourceJs = 0L;
	delete _name;
	_name = 0L;
	delete _explain;
	_explain = 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
