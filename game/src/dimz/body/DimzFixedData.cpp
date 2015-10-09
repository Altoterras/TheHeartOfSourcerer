/***********************************************************************//**
 *	DimzFixedData.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "DimzFixedData.h"

// Friends
#include "../file/DimzFileIdDef.h"

// External
#include "../../tfw/file/IdFile.h"

// Library

////////////////////////////////////////////////////////////////////////////

DIMZ_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// DimzFixedData 静的変数

StringTable DimzFixedData::_strtblSys;
ParticleDefTbl DimzFixedData::_ptcldftbl;

//==========================================================================
// DimzFixedData メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool DimzFixedData::init()
{
	ASSERT(!_strtblSys.isValid());

	// システム固定文字列テーブルを初期化
	initFixedSystemStringTable();

	// パーティクル定義テーブルを初期化
	_ptcldftbl.init(PARTICLE_PARTICLEDEF01_DAT);

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void DimzFixedData::cleanup()
{
	// パーティクル定義テーブルを解放する
	_ptcldftbl.cleanup();

	// システム固定文字列テーブルを解放する
	_strtblSys.destroy();
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void DimzFixedData::notifyChangeLanguage()
{
	if(_strtblSys.isValid())
	{
		// システム固定文字列テーブルを一度解放する
		_strtblSys.destroy();

		// システム固定文字列テーブルを再初期化する
		initFixedSystemStringTable();
	}
}

/*---------------------------------------------------------------------*//**
	システム固定文字列テーブルを初期化する
**//*---------------------------------------------------------------------*/
void DimzFixedData::initFixedSystemStringTable()
{
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
}

////////////////////////////////////////////////////////////////////////////

DIMZ_END_NS
