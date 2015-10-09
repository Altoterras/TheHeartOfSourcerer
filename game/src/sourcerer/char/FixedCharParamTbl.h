/***********************************************************************//**
 *	FixedCharParamTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/28.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CHAR_DATA_FIXED_CHAR_PARAM_H_
#define _SRCR_CHAR_DATA_FIXED_CHAR_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../status/CharParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定キャラクタ パラメータ テーブル
 *
**//*---------------------------------------------------------------------*/
class FixedCharParamTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_CHAR_MAX	= 3;
	static const s32	NUM_LV_MAX		= 50;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 固定パラメータを得る
	const CharParam* getParam(s32 ichar, s32 ilv) const;
	// 定義されているレベル数を得る
	inline s32 getLevelNum() const { return _numLv; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FixedCharParamTbl();
	bool init(u32 fileidData);
	void cleanup();

	//======================================================================
	// 変数
private:
	CharParam _cprm[NUM_CHAR_MAX][NUM_LV_MAX];
	s32 _numLv;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CHAR_DATA_FIXED_CHAR_PARAM_H_
