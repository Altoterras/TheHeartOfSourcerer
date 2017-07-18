/***********************************************************************//**
 *	EsScope.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_SCOPE_H_
#define _ES_ES_SCOPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsObjectOp;
class EsScopeProperty;

/*---------------------------------------------------------------------*//**
 *	スコープ
 *
**//*---------------------------------------------------------------------*/
class EsScope
{
	//======================================================================
	// 定数
public:
	///static const u8	SCOPEF_MIDDLE_DELETE		= 0x01;	// ※※※ 実装なし ※※※ ⇒ SCOPE_MIDDLE_DELETE
	static const u8	SCOPEF_SEALED				= 0x02;	// 保護（不変）フラグ ⇒ SCOPE_SEALED
	static const u8	SCOPEF_BRANDED				= 0x03;	// ⇒ SCOPE_BRANDED
	static const u8	SCOPEF_INDEXED_PROPERTIES	= 0x04;	// ⇒ SCOPE_INDEXED_PROPERTIES

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スコープを所有するオブジェクトを得る
	inline EsObject* getOwnerObject() const { return _objRef; }
	// フリースロットの数を得る
	inline s32 getFreeSlotIndex() const { return _slotidxFree; }
	// フラグを得る
	inline u8 getScopeFlags() const { return _flags; }

	// フリースロットの数を設定する
	inline void setFreeSlotIndex(u16 numSlot) { _slotidxFree = numSlot; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void retainRef();
	bool releaseRef(EsObject* obj);
	s32 getScopePropertyNum() const;
	EsScopeProperty* getLastScopeProperty() const;
	EsScopeProperty* findScopeProperty(const EsValue* spropid) const;
	EsScopeProperty* addScopeProperty(EsContext* ctx, const EsValue* spropid, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf, s16 shortid);
	bool deleteScopeProperty(EsContext* ctx, const EsValue* spropid);
	void makeUniqueShape(EsContext* ctx);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsScope(EsObject* objRef, s32 numFreeSlot);
	~EsScope();

	//======================================================================
	// 変数
private:
	EsObject* _objRef;
	List<EsScopeProperty*>* _splist;
	s32 _slotidxFree;		// 自由に使える開始スロットインデックス ⇒ freeslot
	u8 _flags;
	s32 _refcnt;			// 参照カウンタ ⇒ nrefs
	u32 _idShape;			// シェイプ ID ⇒ shape
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_SCOPE_H_
