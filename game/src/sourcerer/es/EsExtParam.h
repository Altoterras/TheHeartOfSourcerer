/***********************************************************************//**
 *	EsExtParam.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_EXT_PARAM_H_
#define _ES_ES_EXT_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../tfw/Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace es {

/*---------------------------------------------------------------------*//**
 *	外部パラメータクラス
 *		パラメータは基本的に、
 *		unsigned int や void* で事足りるのだが、
 *		非ポインタ言語移植時（Java 等）に問題が起きかねないので、
 *		一度このクラスで吸収し、このクラスの変更のみで対応できるようにする．
 *
**//*---------------------------------------------------------------------*/
class EsExtParam
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 整数値で取得
	inline u32		getIntValue() const		{	return (u32)_value;						}
	// ヴォイドポインタで取得
	inline void*	getObject() const		{	return reinterpret_cast<void*>(_value);	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsExtParam() : _value(0)											{}
	EsExtParam(u32 val) : _value(val)									{}
	EsExtParam(void* val) : _value(reinterpret_cast<addr_size>(val))	{}
	EsExtParam(const EsExtParam& src) : _value(src._value)				{}
	inline EsExtParam& operator=(const EsExtParam& src)	{	_value = src._value;	return *this;	}

	//======================================================================
	// 変数
private:
	addr_size _value;
};

}	// namespace es

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_EXT_PARAM_H_
