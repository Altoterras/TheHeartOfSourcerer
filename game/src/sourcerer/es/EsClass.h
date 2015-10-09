/***********************************************************************//**
 *	EsClass.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_CLASS_H_
#define _ES_ES_CLASS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsObjectOp;
class EsScope;
class EsScopeProperty;

/*---------------------------------------------------------------------*//**
 *	クラス
 *
**//*---------------------------------------------------------------------*/
class EsClass
{
	//======================================================================
	// 定数
public:
	// フラグ
	static const u16 F_HAS_PRIVATE				= 0x0001;	// プライベートすとっとを持つ ⇒ JSCLASS_HAS_PRIVATE
	static const u16 F_RESOLVE_OUT_PARAM		= 0x0002;	// パラメータを受け取るタイプの解決処理 ⇒ JSCLASS_NEW_RESOLVE
	static const u16 F_IS_GLOBAL				= 0x0004;	// グローバルクラスである ⇒ JSCLASS_IS_GLOBAL
	static const u16 F_IS_ANONYMOUS				= 0x0010;	// 匿名 ⇒ JSCLASS_IS_ANONYMOUS
	static const u16 F_SHARE_ALL_PROPS			= 0x0020;	// 共有アクセサ ⇒ JSCLASS_SHARE_ALL_PROPERTIES
	static const u16 F_ENUMERATE_OUT_PARAM		= 0x0040;	// パラメータを受け取るタイプの列挙 ⇒ JSCLASS_NEW_ENUMERATE
	//static const u16 F_PTYPE_CTOR				= 0x0100;	// ※※※ 実装なし ※※※ XML 用実装のため対応なし ⇒ JSCLASS_CONSTRUCT_PROTOTYPE

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 名称を得る
	const VcString* getName() const;
	// 名称アトムを得る
	inline const EsAtom* getNameAtom() const { return _atomName; }
	// クラスタイプを得る
	inline u16 getClassType() const { return _clstype; }
	// フラグを得る
	inline u16 getFlags() const { return _flags; }
	// プロトタイプ種別を得る ⇒ JSCLASS_HAS_CACHED_PROTO, JSCLASS_CACHED_PROTO_KEY
	inline u16 getPrototypeKind() const { return _ptypekind; }
	// デフォルトのスロット予約数を得る ⇒ JSCLASS_RESERVED_SLOTS
	inline u16 getDefaultReservedSlotNum() const { return _numDefaultRsvSlot; }

	// プロトタイプ オブジェクト スロットのインデックスを得る ⇒ JSSLOT_PROTO
	inline u16 getPrototypeSlotIndex() const { return 0; }
	// 親オブジェクト スロットのインデックスを得る ⇒ JSSLOT_PARENT
	inline u16 getParentSlotIndex() const { return 1; }
	// プライベート スロットのインデックスを得る ⇒ JSSLOT_PRIVATE
	inline u16 getPrivateSlotIndex() const { return 2; }
	// ブロック深度スロットのインデックスを得る ⇒ JSSLOT_BLOCK_DEPTH
	inline u16 getBlockDepthSlotIndex() const { return getPrivateSlotIndex() + 1; }
	// 開始スロットインデックスを得る ⇒ JSSLOT_START
	inline u16 getStartSlotIndex() const { return TFW_IS_FLAG(_flags, F_HAS_PRIVATE) ? getPrivateSlotIndex() + 1 : getParentSlotIndex() + 1; }
	// ダイナミックスロット開始インデックスを得る
	virtual u16 getStartDynamicSlotIndex() const { return getStartSlotIndex(); }
	// フリースロットの数を得る ⇒ JSSLOT_FREE
	inline u16 getFreeSlotIndex() const { return getStartSlotIndex() + getDefaultReservedSlotNum(); }

	// 各種スクリプトメソッドを得る
	inline EsPropertyGetterFunc getGetPropertyFunc() const { return _funcGetProp; }
	inline EsPropertySetterFunc getSetPropertyFunc() const { return _funcSetProp; }
	inline EsPropertySetterFunc getAddPropertyFunc() const { return _funcAddProp; }
	inline EsPropertySetterFunc getDeletePropertyFunc() const { return _funcDelProp; }
	inline EsCheckAccessFunc getCheckAccessFunc() const { return _funcCheckAccess; }
	inline EsEnumerateFunc getEnumrateFunc() const { return _funcEnumerate; }
	inline EsEnumerateOutFunc getEnumrateOutParamFunc() const { return _funcEnumOutParam; }
	inline EsResolveFunc getResolvFunc() const { return _funcResolv; }
	inline EsConvertFunc getConvertFunc() const { return _funcConvert; }
	inline EsNativeCall getCallFunc() const { return _funcCall; }
	inline EsNativeCall getConstructFunc() const { return _funcConstruct; }
	inline EsHasInstanceFunc getHasInstanceFunc() const { return _funcHasInst; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const;
	virtual EsObjectOp* getObjectOp() const;
	
	bool getClassId(EsValue* id, EsContext* ctx) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsClass();
	EsClass(const EsAtom* atomName, u16 clstype, u16 flags, u16 ptypekind, u16 numDefaultReservedSlot);

protected:
	void clear();

private:
	static bool convertDefaultFunc(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype);

	//======================================================================
	// 変数
private:
	const EsAtom* _atomName;
	u16 _clstype;
	u16 _ptypekind;							// ⇒ JSCLASS_HAS_CACHED_PROTO, JSCLASS_CACHED_PROTO_KEY
	u16 _flags;
	u16 _numDefaultRsvSlot;

protected:
	EsPropertyGetterFunc _funcGetProp;		// ⇒ getProperty
	EsPropertySetterFunc _funcSetProp;		// ⇒ setProperty
	EsPropertySetterFunc _funcAddProp;		// ⇒ addProperty
	EsPropertySetterFunc _funcDelProp;		// ⇒ delProperty
	EsEnumerateFunc _funcEnumerate;
	EsEnumerateOutFunc _funcEnumOutParam;
	EsResolveFunc _funcResolv;
	EsConvertFunc _funcConvert;
	EsCheckAccessFunc _funcCheckAccess;
	EsNativeCall _funcCall;
	EsNativeCall _funcConstruct;
	EsHasInstanceFunc _funcHasInst;			// ⇒ hasInstance
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_CLASS_H_
