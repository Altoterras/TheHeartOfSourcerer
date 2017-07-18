/***********************************************************************//**
 *	EsKeyword.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_KEYWORD_H_
#define _ES_ES_KEYWORD_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsToken.h"
#include "EsOpcode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsKeywordSet;

/*---------------------------------------------------------------------*//**
 *	キーワード
 *
**//*---------------------------------------------------------------------*/
class EsKeyword
{
	//======================================================================
	// 定数
private:
	static const s32	P1_REFATOM	= S32_MIN;
	static const s32	P1_HASATOM	= S32_MIN + 1;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline const VcString*	getName() const			{	return (_param1 <= P1_HASATOM) ? getAtomString() : (const VcString*)_p;	}
	const EsAtom*			getAtom() const			{	return (_param1 <= P1_HASATOM) ? (const EsAtom*)_p : 0L;			}
	inline EsTokenType		getTokenType() const	{	return (EsTokenType)_param1;	}
	inline EsOpcodeType		getOpcodeType() const	{	return (EsOpcodeType)_param2;	}
	const VcString*			getAtomString() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
private:
	void set(EsKeywordSet* kwset, const char* name);
public:
	void set(const char* name);
	void set(const char* name, EsTokenType tt, EsOpcodeType ot);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsKeyword();
	EsKeyword(const char* name);
	EsKeyword(const char* name, EsTokenType tt, EsOpcodeType ot);
	~EsKeyword();
	void init();

	//======================================================================
	// 変数
private:
	const void* _p;
	s32 _param1;
	addr_size _param2;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_KEYWORD_H_
