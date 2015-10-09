/***********************************************************************//**
 *	CharLogicData.cpp
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
#include "CharLogicData.h"

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
// CharLogicData メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CharLogicData::CharLogicData()
	: _chrdid(0)
	, _probSkill(0)
	, _rangeSkill(0.0f)
{
	TFW_ZERO_MEMORY(_params, sizeof(u16) * NUM_PARAM);
}

/*---------------------------------------------------------------------*//**
	データファイルからデータ部を読み込む
**//*---------------------------------------------------------------------*/
bool CharLogicData::loadFromDataFile(FileBase* file)
{
	u16 wk;

	// キャラクタ定義 ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_chrdid = wk;

	// 技確率
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_probSkill = (u8)wk;
	// 技範囲
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_rangeSkill = (f32)wk;

	// 五元素原質 - 地元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOqlSkill.set(Elene::EL_SO, (s32)wk * 100);
	// 五元素原質 - 水元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOqlSkill.set(Elene::EL_WA, (s32)wk * 100);
	// 五元素原質 - 火元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOqlSkill.set(Elene::EL_FI, (s32)wk * 100);
	// 五元素原質 - 風元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOqlSkill.set(Elene::EL_WI, (s32)wk * 100);
	// 五元素原質 - 空元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneOqlSkill.set(Elene::EL_AE, (s32)wk * 100);

	// 五元素現在値 - 地元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCqlSkill.set(Elene::EL_SO, (s32)wk * 100);
	// 五元素現在値 - 水元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCqlSkill.set(Elene::EL_WA, (s32)wk * 100);
	// 五元素現在値 - 火元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCqlSkill.set(Elene::EL_FI, (s32)wk * 100);
	// 五元素現在値 - 風元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCqlSkill.set(Elene::EL_WI, (s32)wk * 100);
	// 五元素現在値 - 空元素
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	ASSERT(false);	return false;	}
	_eleneCqlSkill.set(Elene::EL_AE, (s32)wk * 100);

	for(s32 i = 0; i < NUM_PARAM; i++)
	{
		// 条件
		if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
		_params[i] = wk;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
