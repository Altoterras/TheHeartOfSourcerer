/***********************************************************************//**
 *  TxtEncode.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/28.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_TXT_ENCODE_H_
#define _TFW_STRING_TEXT_TXT_ENCODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "TxtIterator.h"
#include "TxtPos.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class TxtChar;

/*---------------------------------------------------------------------*//**
 *	テキスト定義
 *
**//*---------------------------------------------------------------------*/
class TxtDef
{
	//======================================================================
	// 定数
public:
	// エンコード
	enum Encode
	{
		ENC_NULL,
		ENC_ASCII,
		ENC_SJIS,
		ENC_UTF8,
		ENC_UNICODE,
	};
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_TXT_ENCODE_H_
