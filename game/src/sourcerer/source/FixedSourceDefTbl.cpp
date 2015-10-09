/***********************************************************************//**
 *	FixedSourceDefTbl.cpp
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
#include "FixedSourceDefTbl.h"

// Friends

// External
#include "../../tfw/file/IdFile.h"
#include "../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	固定ソース定義を取得する
**//*---------------------------------------------------------------------*/
const FixedSourceDef* FixedSourceDefTbl::getDef(u16 fsrcdid) const
{
	for(s32 i = 0; i < NUM_FIXEDSRCDEF_MAX; i++)
	{
		if(_fsrcdefarr[i].getFixedSourceDefId() == fsrcdid)
		{
			return &_fsrcdefarr[i];
		}
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FixedSourceDefTbl::FixedSourceDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FixedSourceDefTbl::~FixedSourceDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool FixedSourceDefTbl::init(u32 fileidData, u32 fileidStr)
{
	s32 numFsrcDef = 0;

	// データファイルを開く
	{
		IdFile file(fileidData);
		if(file.isOpened())
		{
			do
			{
				char c = 0;
				VcString wk;

				// 数を得る
				while((file.read(&c, sizeof(char)) == sizeof(char)) && (c != 0x1f))
				{
					wk += c;
				}
				numFsrcDef = StringUtils::toInteger(&wk);
				wk.emptyEasy();
				ASSERT(numFsrcDef <= NUM_FIXEDSRCDEF_MAX);

				// 固定ソース定義ごとにデータを読み込む
				for(s32 i = 0; i < numFsrcDef; i++)
				{
					if(!_fsrcdefarr[i].loadFromDataFile(&file))
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
	}

	// 文字列ファイルを開く
	{
		IdFile file(fileidStr);
		if(file.isOpened())
		{
			// 固定ソース定義ごとにデータを読み込む
			for(s32 i = 0; i < numFsrcDef; i++)
			{
				if(!_fsrcdefarr[i].loadFromStringFile(&file))
				{
					ASSERT(false);
					break;
				}
			}

			// ファイルを閉じる
			file.close();
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void FixedSourceDefTbl::cleanup()
{
	for(s32 i = 0; i < NUM_FIXEDSRCDEF_MAX; i++)
	{
		_fsrcdefarr[i].cleanup();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
