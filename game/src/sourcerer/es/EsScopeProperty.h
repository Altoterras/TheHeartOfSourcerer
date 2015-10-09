/***********************************************************************//**
 *	EsScopeProperty.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_SCOPE_PROPERTY_H_
#define _ES_ES_SCOPE_PROPERTY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"
#include "EsValue.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;

/*---------------------------------------------------------------------*//**
 *	スコープ アクセサ ⇒ JSScopeProperty
 *
**//*---------------------------------------------------------------------*/
class EsScopeProperty
{
	//======================================================================
	// 定数
public:
	// 属性フラグ
	static const u8 ATRBF_ENUMERATE			= 0x01;		// 列挙 ⇒ JSPROP_ENUMERATE
	static const u8 ATRBF_READONLY			= 0x02;		// 読み込み専用 ⇒ JSPROP_READONLY
	static const u8 ATRBF_PERMANENT			= 0x04;		// 不変 ⇒ JSPROP_PERMANENT
	static const u8 ATRBF_OBJ_GETTER		= 0x10;		// EsObject 型のゲッター ⇒ JSPROP_GETTER
	static const u8 ATRBF_OBJ_SETTER		= 0x20;		// EsObject 型のセッター ⇒ JSPROP_SETTER
	static const u8 ATRBF_SHARED			= 0x40;		// 共有 ⇒ JSPROP_SHARED
	static const u8 ATRBF_INDEX				= 0x80;		// インデックス ⇒ JSPROP_INDEX

	// オプションフラグ
	static const u8 OPTF_MARK				= 0x01;		// ⇒ SPROP_MARK
	static const u8 OPTF_IS_ALIAS			= 0x02;		// ⇒ SPROP_IS_ALIAS
	static const u8 OPTF_HAS_SHORTID		= 0x04;		// ショート ID が有効 ⇒ SPROP_HAS_SHORTID
	static const u8 OPTF_FLAG_SHAPE_REGEN	= 0x08;		// ⇒ SPROP_FLAG_SHAPE_REGEN

	// 無効なスロットインデックス
	static const s32 SLOTIDX_INVALUD		= -1;		// ⇒ SPROP_INVALID_SLOT

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ショート ID を得る
	inline s16 getShortId() const { return _shortid; }
	// 属性フラグを得る
	inline u8 getAtrbFlags() const { return _atrbf; }
	// オプションフラグを得る
	inline u8 getOptionFlags() const { return _optf; }
	// スロットインデックスを得る
	inline s32 getSlotIndex() const { return _slotidx; }
	// アクセサ ID を得る
	inline const EsValue* getId() const { return &_id; }
	// ユーザープロパティ ID を取得
	EsValue getUserId() const;
	// ゲッター関数を得る
	inline EsPropertyGetterFunc getGetterFunc() const { return _funcGetter; }
	// セッター関数を得る
	inline EsPropertySetterFunc getSetterFunc() const { return _funcSetter; }
	// 親ノードを得る
	inline EsScopeProperty* getParent() const { return _parent; }

	// スロットインデックスを設定する
	inline void setSlotIndex(s32 idxSlot) { _slotidx = idxSlot; }
	// 属性フラグを設定する
	inline void setAtrbFlags(u8 atrbf, bool isValid) { TFW_SET_FLAG(_atrbf, atrbf, isValid); }
	// ゲッター関数を設定する
	inline void setGetterFunc(EsPropertyGetterFunc func) { _funcGetter = func; }
	// セッター関数を設定する
	inline void setSetterFunc(EsPropertySetterFunc func) { _funcSetter = func; }
	// 親ノードを設定する
	inline void setParent(EsScopeProperty* spropParent) { _parent = spropParent; }

	// 共有不変属性かを得る ⇒ SPROP_IS_SHARED_PERMANENT
	inline bool isSharedPermanent() const { return ((~_atrbf & (ATRBF_SHARED | ATRBF_PERMANENT)) == 0); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj);
	bool callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* val);

	bool isMatchParams(const EsScopeProperty* s);
	bool isMatchParams(const EsValue* id, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf, s16 shortid);
	bool isMatchParams(EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf, s16 shortid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsScopeProperty();
	EsScopeProperty(const EsValue* id, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf);
	~EsScopeProperty();

private:
	void clear();

	//======================================================================
	// 変数
private:
	EsValue _id;						// アクセサ ID ⇒ id
	s16 _shortid;						// ショートプロパティ ID または、引数(arg)や変数(var)のスロットインデックス
	u8 _atrbf;							// 属性フラグ ⇒ attrs
	u8 _optf;							// オプションフラグ ⇒ flags
	s32 _slotidx;						// スロット インデックス ⇒ slot;
	EsPropertyGetterFunc _funcGetter;	// ゲッター関数 ⇒ getter
	EsPropertySetterFunc _funcSetter;	// セッター関数 ⇒ setter
	EsScopeProperty* _parent;			// 親ノード ⇒ parent
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_SCOPE_PROPERTY_H_
