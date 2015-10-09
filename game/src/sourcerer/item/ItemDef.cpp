/***********************************************************************//**
 *	ItemDef.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ItemDef.h"

// Friends
#include "../../tfw/file/FileBase.h"

// External

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemDef メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemDef::ItemDef()
	: _itmdid(0)
	, _cat(0)
	, _price(0)
	, _param1(0)
{
	for(s32 i = 0; i < NUM_LANGID; i++)
	{
		_name[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ItemDef::~ItemDef()
{
	#if defined(_DEBUG)
		for(s32 i = 0; i < NUM_LANGID; i++)
		{
			ASSERT(_name[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	データファイルからデータ部を読み込む
**//*---------------------------------------------------------------------*/
bool ItemDef::loadFromDataFile(FileBase* file)
{
	u16 wk;

	// 定義 ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_itmdid = wk;

	// カテゴリ
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_cat = wk;

	// 五元素バランス - 地元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneOql.set(Elene::EL_SO, (s32)wk * 100);
	// 五元素バランス - 水元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneOql.set(Elene::EL_WA, (s32)wk * 100);
	// 五元素バランス - 火元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneOql.set(Elene::EL_FI, (s32)wk * 100);
	// 五元素バランス - 風元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneOql.set(Elene::EL_WI, (s32)wk * 100);
	// 五元素バランス - 空元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneOql.set(Elene::EL_AE, (s32)wk * 100);

	// 種族特徴率 - 地元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneFamilyRatio.set(Elene::EL_SO, (s32)wk);
	// 種族特徴率 - 水元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneFamilyRatio.set(Elene::EL_WA, (s32)wk);
	// 種族特徴率 - 火元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneFamilyRatio.set(Elene::EL_FI, (s32)wk);
	// 種族特徴率 - 風元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneFamilyRatio.set(Elene::EL_WI, (s32)wk);
	// 種族特徴率 - 空元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_eleneFamilyRatio.set(Elene::EL_AE, (s32)wk);

	// 値段
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_price = wk;

	// 汎用パラメータ１
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param1 = wk;

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列を読み込む（loadFromStringFile のサブメソッド）
**//*---------------------------------------------------------------------*/
bool ItemDef::loadString(VcString* str, FileBase* file)
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
bool ItemDef::loadFromStringFile(FileBase* file)
{
	VcString wk;

	// 日本語名
	if(!loadString(&wk, file))	{	return false;	}
	_name[LANGID_JP] = new VcString(&wk);

	// 英語名
	if(!loadString(&wk, file))	{	return false;	}
	_name[LANGID_EN] = new VcString(&wk);

	// 改行までまわす
	if(!loadString(&wk, file))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void ItemDef::cleanup()
{
	for(s32 i = 0; i < NUM_LANGID; i++)
	{
		delete _name[i];
		_name[i] = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
