/***********************************************************************//**
 *	CharDef.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CharDef.h"

// Friends
#include "../../tfw/file/FileBase.h"

// External

// Library


SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharDef メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CharDef::CharDef()
	: _chrdid(0)
	, _xShadow(0)
	, _zShadow(0)
	, _rShadow(0)
	, _param1(0)
	, _param2(0)
	, _param3(0)
	, _param4(0)
{
	for(s32 i = 0; i < NUM_LANGID; i++)
	{
		_name[i] = 0L;
		_fname[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CharDef::~CharDef()
{
	#if defined(_DEBUG)
		for(s32 i = 0; i < NUM_LANGID; i++)
		{
			ASSERT(_name[i] == 0L);
			ASSERT(_fname[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	データファイルからデータ部を読み込む
**//*---------------------------------------------------------------------*/
bool CharDef::loadFromDataFile(FileBase* file)
{
	u16 wk;

	// 定義 ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_chrdid = wk;

	// 五元素原質 - 地元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOql.set(Elene::EL_SO, (s32)wk * 100);
	// 五元素原質 - 水元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOql.set(Elene::EL_WA, (s32)wk * 100);
	// 五元素原質 - 火元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOql.set(Elene::EL_FI, (s32)wk * 100);
	// 五元素原質 - 風元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOql.set(Elene::EL_WI, (s32)wk * 100);
	// 五元素原質 - 空元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOql.set(Elene::EL_AE, (s32)wk * 100);

	// 五元素現在値 - 地元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCql.set(Elene::EL_SO, (s32)wk * 100);
	// 五元素現在値 - 水元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCql.set(Elene::EL_WA, (s32)wk * 100);
	// 五元素現在値 - 火元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCql.set(Elene::EL_FI, (s32)wk * 100);
	// 五元素現在値 - 風元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCql.set(Elene::EL_WI, (s32)wk * 100);
	// 五元素現在値 - 空元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCql.set(Elene::EL_AE, (s32)wk * 100);

	// キャラクターパラメーター - エネルギーボーナス
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	s32 lfenergy = wk * 1000;

	// キャラクターパラメーター - 集中力・統合力
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	u16 unification = wk;

	// キャラクターパラメーター - 変換力
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	u16 conversion = wk;
	
	// キャラクターパラメーター - 安定度
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	u16 stability = wk;

	// キャラクターパラメーター設定
	_chrprm.set(1, 0, lfenergy, unification, conversion, stability);

	// 影オフセット X
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	TFW_COPY_MEMORY(&_xShadow, &wk, sizeof(u16));

	// 影オフセット Z
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	TFW_COPY_MEMORY(&_zShadow, &wk, sizeof(u16));

	// 影半径
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	TFW_COPY_MEMORY(&_rShadow, &wk, sizeof(u16));

	// （予約）
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}

	// 汎用パラメータ１
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_param1 = wk;

	// 汎用パラメータ２
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_param2 = wk;

	// 汎用パラメータ３
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_param3 = wk;

	// 汎用パラメータ４
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_param4 = wk;

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列を読み込む（loadFromStringFile のサブメソッド）
**//*---------------------------------------------------------------------*/
bool CharDef::loadString(VcString* str, FileBase* file)
{
	str->emptyEasy();

	while(true)
	{
		char c;
		if(file->read(&c, sizeof(char)) != sizeof(char))	{	ASSERT(false);	return false;	}

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
bool CharDef::loadFromStringFile(FileBase* file)
{
	VcString wk;

	// 日本語名
	if(!loadString(&wk, file))	{	ASSERT(false);	return false;	}
	_name[LANGID_JP] = new VcString(&wk);

	// 日本語苗字
	if(!loadString(&wk, file))	{	ASSERT(false);	return false;	}
	_fname[LANGID_JP] = new VcString(&wk);

	// 英語名
	if(!loadString(&wk, file))	{	ASSERT(false);	return false;	}
	_name[LANGID_EN] = new VcString(&wk);

	// 英語苗字
	if(!loadString(&wk, file))	{	ASSERT(false);	return false;	}
	_fname[LANGID_EN] = new VcString(&wk);

	// 改行までまわす
	if(!loadString(&wk, file))	{	ASSERT(false);	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void CharDef::cleanup()
{
	for(s32 i = 0; i < NUM_LANGID; i++)
	{
		delete _name[i];
		_name[i] = 0L;
		delete _fname[i];
		_fname[i] = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
