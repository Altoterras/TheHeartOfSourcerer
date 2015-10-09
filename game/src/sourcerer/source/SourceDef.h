/***********************************************************************//**
 *	SourceDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOURCE_SOURCE_DEF_H_
#define _SRCR_SOURCE_SOURCE_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

namespace es
{
	class EsKeywordSet;
}
using namespace es;

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ソウマ ソース 共通定義
 *
**//*---------------------------------------------------------------------*/
class SourceDef
{
	//======================================================================
	// 定数
public:
	// ソース種別
	enum SourceKind
	{
		SRCKIND_NULL,	// （未定義）
		SRCKIND_SS,		// SS
		SRCKIND_JS,		// JS
		NUM_SRCKIND
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// キーワードセットを得る
	static EsKeywordSet* getKeywordSet(SourceKind srckind) { ASSERT((SRCKIND_NULL < srckind) && (srckind < SourceDef::NUM_SRCKIND)); return _kwsets[srckind]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:

	//======================================================================
	// 変数
private:
	static EsKeywordSet* _kwsets[NUM_SRCKIND];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOURCE_SOURCE_DEF_H_
