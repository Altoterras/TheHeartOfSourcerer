/***********************************************************************//**
 *  CcString.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_CC_STRING_H_
#define _TFW_STRING_CC_STRING_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "CStringBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

/*---------------------------------------------------------------------*//**
 *	const char* 簡易ラップ文字列クラス
 *		const char* 型の C 文字列を CStringBase として扱えるようにしたクラス
 *		内部でメモリを確保しないので高速だが、
 *		外部に依存したメモリをポイントするため寿命には注意が必要
 *		Java 移植時には Java ビルトイン VcString を単に内包したクラス、
 *		となるだろう…
 *
**//*---------------------------------------------------------------------*/
class CcString : public CStringBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	const char* getRaw(int index = 0) const { return _sz; };
	int getLength() const;
	char getAt(int index) const { return _sz[index]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CcString(const char* sz) : _sz(sz){}	// explicit でないところがミソ
	CcString(const CcString& src) { _sz = src._sz; }

	inline static CcString* setout(CcString* s, const char* sz) { s->_sz = sz; return s; }

	void copy(const CcString* src) { _sz = src->_sz; }
	void copy(const char* sz) { _sz = sz; }
	CcString& operator=(const CcString& src) { _sz = src._sz; return *this; }
	CcString& operator=(const char* sz) { _sz = sz; return *this; }

	//======================================================================
	// 変数
protected:
	const char* _sz;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_CC_STRING_H_
