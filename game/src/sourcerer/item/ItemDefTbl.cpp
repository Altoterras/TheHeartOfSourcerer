/***********************************************************************//**
 *	ItemDefTbl.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ItemDefTbl.h"

// Friends

// External
#include "../../tfw/file/IdFile.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	アイテム定義を取得する
**//*---------------------------------------------------------------------*/
const ItemDef* ItemDefTbl::getDef(u16 itmdid) const
{
	for(s32 i = 0; i < NUM_ITEMDEF_MAX; i++)
	{
		if(_itmdefarr[i].getItemDefId() == itmdid)
		{
			return &_itmdefarr[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	インデックスよりアイテム定義を取得する
**//*---------------------------------------------------------------------*/
const ItemDef* ItemDefTbl::getDefFromIndex(s32 index) const
{
	if((index < 0) || (NUM_ITEMDEF_MAX <= index))	{	return 0L;	}
	return &_itmdefarr[index];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemDefTbl::ItemDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ItemDefTbl::~ItemDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool ItemDefTbl::init(u32 fileidData, u32 fileidStr)
{
	s32 numItemDef = 0;

	// データファイルを開く
	{
		IdFile file(fileidData);
		if(!file.isOpened())
		{
			return false;
		}

		do
		{
			// アイテム定義の数
			u32 sizeRead = file.read(&numItemDef, sizeof(u32));
			if(sizeRead != sizeof(u32))
			{
				ASSERT(false);
				break;
			}
			ASSERT(numItemDef <= NUM_ITEMDEF_MAX);

			// アイテム定義ごとにデータを読み込む
			for(s32 i = 0; i < numItemDef; i++)
			{
				if(!_itmdefarr[i].loadFromDataFile(&file))
				{
					ASSERT(false);
					break;
				}
			}
		}
		while(false);

		// ファイルを閉じる
		file.close();
	}

	// 文字列ファイルを開く
	{
		IdFile file(fileidStr);
		if(!file.isOpened())
		{
			return false;
		}

		// アイテム定義ごとにデータを読み込む
		for(s32 i = 0; i < numItemDef; i++)
		{
			if(!_itmdefarr[i].loadFromStringFile(&file))
			{
				ASSERT(false);
				break;
			}
		}

		// ファイルを閉じる
		file.close();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void ItemDefTbl::cleanup()
{
	for(s32 i = 0; i < NUM_ITEMDEF_MAX; i++)
	{
		_itmdefarr[i].cleanup();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
