/***********************************************************************//**
 *	EsPrototype.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/05/10.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_PROTOTYPE_H_
#define _ES_ES_PROTOTYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsAtom.h"
#include "EsClass.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	プロトタイプ定義
 *
**//*---------------------------------------------------------------------*/
class EsPrototype
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// プロトタイプ種別を得る
	inline u16 getPrototypeKind() const { return _kind; }
	// 名前アトムを得る
	inline const EsAtom* getNameAtom() const { return _atomName; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsPrototype(const EsAtom* atomName, u16 kind);
	virtual ~EsPrototype() {}
	virtual bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const = 0;

	//======================================================================
	// 変数
private:
	u16 _kind;
	const EsAtom* _atomName;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_PROTOTYPE_H_
