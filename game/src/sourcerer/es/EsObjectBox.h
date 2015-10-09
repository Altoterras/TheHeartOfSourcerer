/***********************************************************************//**
 *	EsObjectBox.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/02/02.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_OBJECT_BOX_H_
#define _ES_ES_OBJECT_BOX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsUnit.h"
#include "EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsBytecodeBuilder;
class EsClass;
class EsNameItNode;
class EsScope;
class EsScopeProperty;
class EsFunction;
class EsObjectOp;
class EsValue;

/*---------------------------------------------------------------------*//**
 *	ES オブジェクト ボックス
 *	⇒ JSObjectBox
 *
**//*---------------------------------------------------------------------*/
class EsObjectBox : public EsUnit
{
	#define ENABLE_OB_LINK	0

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// オブジェクトを取得する
	inline EsObject* getBlockObject() const { return _obj; }
#if ENABLE_OB_LINK
	// ??? を得る
	inline EsObjectBox* getTraceLink() const { return _traceLink; }
	// ??? を得る
	inline EsObjectBox* getEmitLink() const { return _emitLink; }
#endif

	// オブジェクトを設定する
	inline void setBlockObject(EsObject* objBlock) { _obj = objBlock; }
#if ENABLE_OB_LINK
	// ??? を設定する
	inline void setTraceLink(EsObjectBox* objbox) { _traceLink = objbox; }
	// ??? を設定する
	inline void setEmitLink(EsObjectBox* objbox) { _emitLink = objbox; }
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsObjectBox();

	virtual bool isParseTimeOnly() const { return true; }
	void setup(EsObject* objBlock, EsObjectBox* objboxTraceLink, EsObjectBox* objboxEmitLink);

	//======================================================================
	// 変数
protected:
	EsObject* _obj;				// ⇒ object
#if ENABLE_OB_LINK
	EsObjectBox* _traceLink;	// ⇒ traceLink
	EsObjectBox* _emitLink;		// ⇒ emitLink
#endif
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_OBJECT_BOX_H_
