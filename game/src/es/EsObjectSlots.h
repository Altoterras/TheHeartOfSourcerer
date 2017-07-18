/***********************************************************************//**
 *	EsObject.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/06/29.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_OBJECT_SLOTS_H_
#define _ES_ES_OBJECT_SLOTS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class InsArray;

TFW_END_NS

ES_BEGIN_NS

class EsObject;
class EsValue;

/*---------------------------------------------------------------------*//**
 *	スロット配列
 *
**//*---------------------------------------------------------------------*/
class EsObjectSlots
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スロット数を得る ⇒ STOBJ_NSLOTS
	s32 getNum() const;
	// ダイナミックスロット数を得る
	s32 getDynamicSlotNum() const { return getNum() - getStartDynamicIndex(); }

	// スロットの値を得る ⇒ STOBJ_GET_SLOT
	const EsValue* getValue(u16 slotidx) const;
	EsValue* value(u16 slotidx);
	// スロットの値を設定する ⇒ STOBJ_SET_SLOT
	bool setValue(u16 slotidx, const EsValue* val);

	// ダイナミックスロットの値を得る ⇒ dslots
	const EsValue* getDvalue(u16 dslotidx) const;
	EsValue* dvalue(u16 dslotidx);
	// ダイナミックスロットの値を設定する
	bool setDvalue(u16 dslotidx, const EsValue* val);

	// 予約スロットの値を得る ⇒ JS_GetReservedSlot
	bool getReservedSlotValue(EsValue* val, EsContext* ctx, u16 slotidx) const;
	// 予約スロットの値を設定する ⇒ JS_SetReservedSlot
	bool setReservedSlotValue(EsContext* ctx, u16 slotidx, const EsValue* val);

	// プロトタイプオブジェクトの値を得る ⇒ STOBJ_GET_PROTO, OBJ_GET_PROTO
	const EsObject* getPrototypeObject() const;
	EsObject* prototypeObject();
	// プロトタイプオブジェクトを設定する ⇒ STOBJ_SET_PARENT, OBJ_SET_PROTO
	void setPrototypeObject(EsObject* obj);

	// 親オブジェクトを得る ⇒ STOBJ_GET_PARENT, OBJ_GET_PARENT
	const EsObject* getParentObject() const;
	EsObject* parentObject();
	// 親オブジェクトを設定する ⇒ STOBJ_SET_PARENT, STOBJ_CLEAR_PARENT, OBJ_SET_PARENT
	void setParentObject(EsObject* obj);

	// プライベート スロットのデータを得る ⇒ STOBJ_GET_PRIVATE, OBJ_GET_PRIVATE
	const void* getPrivateData() const;
	void* privateData();
	// プライベート スロットにデータを設定 ⇒ JS_SetPrivate
	void setPrivateData(void* data);
	// プライベート スロットの値を得る
	const EsValue* getPrivateValue() const;
	EsValue* privateValue();
	// プライベート スロットに値を設定
	void setPrivateValue(const EsValue* val);

	// ブロック深度を得る ⇒ OBJ_BLOCK_DEPTH
	s32 getBlockDepth() const;
	// ブロック深度を設定 ⇒ OBJ_SET_BLOCK_DEPTH
	void setBlockDepth(s32 depth);

	// プロトタイプ オブジェクト スロットのインデックスを得る
	u16 getPrototypeIndex() const;
	// 親オブジェクト スロットのインデックスを得る
	u16 getParentIndex() const;
	// プライベート スロットのインデックスを得る
	u16 getPrivateIndex() const;
	// ブロック深度スロットのインデックスを得る
	u16 getBlockDepthIndex() const;
	// 開始スロットインデックスを得る
	u16 getStartIndex() const;
	// ダイナミックスロット開始インデックスを得る
	u16 getStartDynamicIndex() const;

	/* 【メモ】

		＜スロットの構成＞（EsClass のインデックス定義も参照のこと）
		[0][1][2][3][4]
		↑ ↑ ↑ ↑ ↑
		│ │ │ │ └─ ダイナミックスロット開始
		│ │ │ └─ 配列確保数（配列オブジェクトの場合．それ以外は左に詰める）
		│ │ └─ プライベート（プライベートを持つ場合．それ以外は左に詰める）
		│ └─ 親オブジェクト
		└─ プロトタイプ
		
		⇒ MozillaJs の場合
		＜dslots について＞
		[0][1][2][3][4][5][6]...
		↑ ↑
		│ └─ dslots[0] : dslots の先頭
		└─ dslots[-1] : ダイナミックスロット数（メモリアロケート数 - 1）
	*/

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool create();
	bool recreate(s32 numSlot);
	s32 addValue(const EsValue* val);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsObjectSlots(EsObject* objParent);
	~EsObjectSlots();

private:
	bool isSafeReservedSlotIndex(u16 slotidx, u16 numLimit) const;

	//======================================================================
	// 変数
private:
	InsArray<EsValue>* _arrSlot;
	EsObject* _objParentRef;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_OBJECT_SLOTS_H_
