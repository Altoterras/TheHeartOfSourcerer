/***********************************************************************//**
 *  TxtUtils.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/12/06.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_TXT_UTILS_H_
#define _TFW_STRING_TEXT_TXT_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "TxtDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class SjisTxt;
class TxtChar;
class UnicodeTxt;
class Utf8Txt;

/*---------------------------------------------------------------------*//**
 *	テキスト ユーティリティ
 *		※	主に変換テーブルや OS API などを使用するアルゴリズムを含む
 *	
**//*---------------------------------------------------------------------*/
class TxtUtils
{
	//======================================================================
	// メソッド
public:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	// 全角半角判定
	static bool isFullWidth(TxtDef::Encode enc, const TxtChar* tc);
	// Unicode 文字を Shift-JIS 文字に変換する
	static u32 convUnicodeToSjisCode(u32 cUnicode);
	// Unicode テキストを Shift-JIS テキストに変換する
	static bool convUnicodeToSjisText(SjisTxt* dst, const UnicodeTxt* src);
	// Shift-JIS テキストを Unicode テキストに変換する
	static bool convSjisToUnicodeText(UnicodeTxt* dst, const SjisTxt* src);
	// UTF-8 テキストを Shift-JIS テキストに変換する
	static bool convUtf8ToSjisText(SjisTxt* dst, const Utf8Txt* src);
	// Shift-JIS テキストを UTF-8 テキストに変換する
	static bool convSjisToUtf8Text(Utf8Txt* dst, const SjisTxt* src);

	// テキストエンコード変換
	static bool convEncode(Txt* txtDst, const Txt* txtSrc);

	// テキストを短くする
	static bool shortenByRawCharCount(Txt* txt, u32 countRawChar);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_TXT_UTILS_H_
