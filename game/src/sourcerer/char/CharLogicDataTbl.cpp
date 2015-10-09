/***********************************************************************//**
 *	CharLogicDataTbl.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/28.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CharLogicDataTbl.h"

// Friends
#include "../body/Game.h"

// External
#include "../../tfw/file/IdFile.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharLogicDataTbl メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	固定パラメータを得る
**//*---------------------------------------------------------------------*/
const CharLogicData* CharLogicDataTbl::getData(u16 chrdid) const
{
	for(int i = 0; i < NUM_CHAR_MAX; i++)
	{
		if(_cldat[i].getCharDefId() == chrdid)
		{
			return &_cldat[i];
		}
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CharLogicDataTbl::CharLogicDataTbl()
	: _numData(0)
{
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool CharLogicDataTbl::init(u32 fileidData)
{
	// ファイルを開く
	IdFile file(fileidData);
	if(!file.isOpened())
	{
		return false;
	}

	// データ定義の数
	s32 numData;
	u32 sizeRead = file.read(&numData, sizeof(s32));
	if(sizeRead != sizeof(s32))
	{
		ASSERT(false);
		return false;
	}
	ASSERT(numData <= NUM_CHAR_MAX);

	// データを読み込む
	for(s32 i = 0; i < numData; i++)
	{
		if(!_cldat[i].loadFromDataFile(&file))
		{
			ASSERT(false);
			return false;
		}
	}
	_numData = numData;

	// ファイルを閉じる
	file.close();
	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void CharLogicDataTbl::cleanup()
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
