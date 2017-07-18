/***********************************************************************//**
 *	EsSysPrototypes.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/05/10.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_SYS_PROTOTYPES_H_
#define _ES_ES_SYS_PROTOTYPES_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsKeywordSet.h"
#include "EsPrototype.h"

#include "../tfw/collection/Array.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定クラス定義
 *
**//*---------------------------------------------------------------------*/
class EsSysPrototypes
{
	//======================================================================
	// 定数
public:
	// 固定プロトタイプ種別 ⇒ JSProtoKey
	enum FixedPrototypeKind
	{
		PTYPEKIND_NULL				= 0,
		PTYPEKIND_OBJECT,
		PTYPEKIND_FUNCTION,
		PTYPEKIND_ARRAY,
		PTYPEKIND_BOOLEAN,
//		PTYPEKIND_JSON,
//		PTYPEKIND_DATE,
		PTYPEKIND_MATH,
		PTYPEKIND_NUMBER,
		PTYPEKIND_STRING,
//		PTYPEKIND_REGEXP,
		PTYPEKIND_SCRIPT,
//		PTYPEKIND_XML,
//		PTYPEKIND_NAMESPACE,
//		PTYPEKIND_QNAME,
//		PTYPEKIND_ANYNAME,
//		PTYPEKIND_ATTRIBUTENAME,
//		PTYPEKIND_ERROR,
//		PTYPEKIND_INTERNALERROR,
//		PTYPEKIND_EVALERROR,
//		PTYPEKIND_RANGEERROR,
//		PTYPEKIND_REFERENCEERROR,
//		PTYPEKIND_SYNTAXERROR,
//		PTYPEKIND_TYPEERROR,
//		PTYPEKIND_URIERROR,
//		PTYPEKIND_GENERATOR,
		PTYPEKIND_ITERATOR,
//		PTYPEKIND_STOPITERATION,
//		PTYPEKIND_FILE,
		PTYPEKIND__CUSTOM_,
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	NULL プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsNullPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const { return false; }
		static const EsNullPrototype* getInstance() { static EsNullPrototype instance; return &instance; }

	private:
		EsNullPrototype() : EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_NULL)->getAtom(), PTYPEKIND_NULL) {}
	};

	//======================================================================
	// メソッド
public:
	static const EsPrototype* getPrototype(u16 ptypekind);
	static inline u16 generatePrototypeKind() { return generatePrototypeKind(0L); }
	static u16 generatePrototypeKind(const EsAtom* atomName);
	static void registerExternalPrototype(const EsPrototype* ptype);

	//======================================================================
	// 変数
private:
	static Array<const EsPrototype*> _arrExtPtypes;		// 拡張プロトタイプ配列
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_SYS_PROTOTYPES_H_
