/***********************************************************************//**
 *	StringTable.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/03/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StringTable.h"

// Friends
#include "../file/FileBase.h"
#include "../string/StringUtils.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StringTable メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	文字列を得る
**//*---------------------------------------------------------------------*/
const VcString* StringTable::getStringByIndex(s32 row, s32 col) const
{
	if((row < 0) || (_numArr <= row))	{	return 0L;	}
	if((col < 0) || (_numCol <= col))	{	return 0L;	}

	return &(_sdarr[row]._strarr[col]);
}

/*---------------------------------------------------------------------*//**
	ID から文字列を得る
**//*---------------------------------------------------------------------*/
const VcString* StringTable::getString(u16 id, s32 col) const
{
	if((col < 0) || (_numCol <= col))	{	return 0L;	}

	for(s32 i = 0; i < _numArr; i++)
	{
		if(_sdarr[i]._id == id)
		{
			return &_sdarr[i]._strarr[col];
		}
	}

	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StringTable::StringTable()
	: _sdarr(0L)
	, _numCol(0)
	, _numArr(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StringTable::~StringTable()
{
	ASSERT(_sdarr == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルから読み込む

	@param file 読み込みで開かれているファイルへの参照
	@param isValidFileColArray 読み込む文字列として有効なカラム番号配列
			※	0 は必ず ID なので、その後に例えば、日本語、英語…
				と並んでいて、日本語を読み込みたい場合は 1 を指定、
				英語を読み込みたい場合は 2 を指定、となる
	@param numValidFileColl 有効カラム番号配列の要素数
	@retval true 成功
	@retval false 失敗
**//*---------------------------------------------------------------------*/
bool StringTable::loadFromFile(FileBase* file, s32* isValidFileColArray, s32 numValidFileColl)
{
	destroy();	// 一度リセットする

	char c = 0;
	VcString wk;

	// 行数を得る
	s32 numLine = 0;
	while((file->read(&c, sizeof(char)) == sizeof(char)) && (c != 0x1f))
	{
		wk += c;
	}
	numLine = StringUtils::toInteger(&wk);
	wk.emptyEasy();

	// 配列を確保する
	StringDef* sdarr = new StringDef[numLine];
	if(sdarr == 0)	{	return false;	}
	for(int i = 0; i < numLine; i++)
	{
		sdarr[i].create(numValidFileColl);
	}

	// 各行を読み込む
	s32 line = 0;
	while(line < numLine)
	{
		s32 col = 0;
		while(true)
		{
			if(file->read(&c, sizeof(char)) != sizeof(char))
			{
				delete[] sdarr;
				return false;
			}

			if((c == 0x1e) || (c == 0x1f))
			{
				if(col == 0)	// ID 列
				{
					sdarr[line]._id = StringUtils::toInteger(&wk);
				}
				else
				{
					for(int j = 0; j < numValidFileColl; j++)
					{
						if(isValidFileColArray[j] == col)	// 有効列である
						{
							sdarr[line]._strarr[j] = wk;
							break;
						}
					}
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
					line++;
					break;
				}
			}
			else
			{
				wk.add(c);
			}
		}
	}

	// 結果を保存する
	_sdarr = sdarr;
	_numArr = numLine;
	_numCol = numValidFileColl;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StringTable::destroy()
{
	if(_sdarr != 0L)
	{
		for(int i = 0; i < _numArr; i++)
		{
			_sdarr[i].destroy();
		}
		delete[] _sdarr;
		_sdarr = 0L;

		_numArr = 0;
	}
}

//==========================================================================
// StringTable::StringDef メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StringTable::StringDef::StringDef()
	: _id(0)
	, _strarr(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StringTable::StringDef::~StringDef()
{
	ASSERT(_strarr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool StringTable::StringDef::create(s32 numCol)
{
	_strarr = new VcString[numCol];
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StringTable::StringDef::destroy()
{
	delete[] _strarr;
	_strarr = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
