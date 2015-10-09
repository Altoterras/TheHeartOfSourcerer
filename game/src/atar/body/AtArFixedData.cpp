/***********************************************************************//**
 *	AtArFixedData.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AtArFixedData.h"

// Friends
#include "../file/AtArFileIdDef.h"

// External
#include "../../tfw/file/IdFile.h"

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ENABLE_STRING_TABLE		0

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AtArFixedData 静的変数

StringTable AtArFixedData::_strtblSys;
ParticleDefTbl AtArFixedData::_ptcldftbl;

//==========================================================================
// AtArFixedData メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool AtArFixedData::init()
{
#if ENABLE_STRING_TABLE
	ASSERT(!_strtblSys.isValid());

	// システム固定文字列テーブルを初期化
	initFixedSystemStringTable();
#endif

	// パーティクル定義テーブルを初期化
	_ptcldftbl.init(PARTICLE_PARTICLEDEF01_DAT);

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void AtArFixedData::cleanup()
{
	// パーティクル定義テーブルを解放する
	_ptcldftbl.cleanup();

#if ENABLE_STRING_TABLE
	// システム固定文字列テーブルを解放する
	_strtblSys.destroy();
#endif
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void AtArFixedData::notifyChangeLanguage()
{
#if ENABLE_STRING_TABLE
	if(_strtblSys.isValid())
	{
		// システム固定文字列テーブルを一度解放する
		_strtblSys.destroy();

		// システム固定文字列テーブルを再初期化する
		initFixedSystemStringTable();
	}
#endif
}

/*---------------------------------------------------------------------*//**
	システム固定文字列テーブルを初期化する
**//*---------------------------------------------------------------------*/
void AtArFixedData::initFixedSystemStringTable()
{
#if ENABLE_STRING_TABLE
	// ファイルを開く
	IdFile file(SYSTEM_FIXEDSTRTBL_DAT);
	if(file.isOpened())
	{
		s32 colValidString[1];
		switch(Env_getLangId())
		{
		case LANGID_EN:	colValidString[0] = 2;		break;
		case LANGID_JP:	colValidString[0] = 1;		break;
		}
		_strtblSys.loadFromFile(&file, colValidString, 1);
	}

	// ファイルを閉じる
	file.close();
#endif
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS
