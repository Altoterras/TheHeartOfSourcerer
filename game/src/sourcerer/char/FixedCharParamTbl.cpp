/***********************************************************************//**
 *	FixedCharParamTbl.cpp
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
#include "FixedCharParamTbl.h"

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
// FixedCharParamTbl メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	固定パラメータを得る
**//*---------------------------------------------------------------------*/
const CharParam* FixedCharParamTbl::getParam(s32 ichar, s32 ilv) const
{
	if(((ichar < 0) || (NUM_CHAR_MAX <= ichar)) || ((ilv < 0) || (NUM_LV_MAX <= ilv)))
	{
		return 0L;
	}
	if(_numLv <= ilv)
	{
		return 0L;
	}
	return &_cprm[ichar][ilv];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FixedCharParamTbl::FixedCharParamTbl()
	: _numLv(0)
{
	TFW_ZERO_MEMORY(_cprm, sizeof(CharParam) * NUM_CHAR_MAX * NUM_LV_MAX);
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool FixedCharParamTbl::init(u32 fileidData)
{
	// ファイルを開く
	IdFile file(fileidData);
	if(!file.isOpened())
	{
		return false;
	}

	// レベル定義の数
	s32 numLv;
	u32 sizeRead = file.read(&numLv, sizeof(s32));
	if(sizeRead != sizeof(s32))
	{
		ASSERT(false);
		return false;
	}
	ASSERT(numLv <= NUM_LV_MAX);

	// レベルごとにデータを読み込む
	for(s32 ilv = 0; ilv < numLv; ilv++)
	{
		u16 wk;

		for(s32 ichar = 0; ichar < NUM_CHAR_MAX; ichar++)
		{
			u32 exp;
			u16 uni, act, stb;
			s32 lfene;

			// PC 経験値
			if(file.read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
			exp = (u32)wk;

			// PC HP
			if(file.read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
			lfene = wk * 1000;

			// PC UNI
			if(file.read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
			uni = wk;

			// PC ACT
			if(file.read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
			act = wk;

			// PC STB
			if(file.read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
			stb = wk;

			_cprm[ichar][ilv].set((u16)(ilv + 1), exp, lfene, uni, act, stb);
		}
	}
	_numLv = numLv;

	// ファイルを閉じる
	file.close();
	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void FixedCharParamTbl::cleanup()
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
