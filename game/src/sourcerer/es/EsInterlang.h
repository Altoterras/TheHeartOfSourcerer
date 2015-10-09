/***********************************************************************//**
 *	EsInterlang.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_TEMPLATE_H_
#define _ES_ES_TEMPLATE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsUnit.h"
#include "EsUpvarInfo.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class InsArray;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsAtomList;
class EsBytecodeBuilder;
class EsObject;

/*---------------------------------------------------------------------*//**
 *	ES 中間言語（パーサーの解析結果）⇒ JSScript
 *
**//*---------------------------------------------------------------------*/
class EsInterlang : public EsUnit
{
	//======================================================================
	// 定数
protected:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// バイトコードを得る
	inline VcString* getBytecode() const { return _bc; }
	// アトムを得る
	const EsAtom* getAtom(s32 index);
	// アトムの数を得る
	s32 getAtomNum() const;
	// オブジェクト配列からオブジェクトを得る
	EsObject* getObject(s32 index) const;
	// オブジェクト数を得る
	s32 getObjectNum() const;
	// 上域変数情報の取得
	const EsUpvarInfo* getUpvarInfo(s32 index) const;
	// 上域変数情報数を得る
	s32 getUpvarInfoNum() const;
	// 上域変数情報配列が NULL かどうかを得る ⇒ upvarsOffset == 0
	inline bool isUpvarArrayNull() const { return _arrUpvar == 0L; }
	// 正規表現オブジェクト数を得る
	s32 getRegexpNum() const;
	// スロットの数を得る
	inline u16 getSlotNum() const { return _numSlot; }
	// 固定スロット数を得る ⇒ nfixed
	inline u16 getFixedSlotNum() const { return _numFixedSlot; }
	// ネスト深度を得る ⇒ staticLevel
	inline u32 getNestDepth() const { return _depthNest; }
	// デバッグ情報付加を行うかを得る
	inline bool isBcExInfoAdd() const { return _isAddBcExInfo; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsInterlang();
	~EsInterlang();
	inline bool copy(const EsInterlang* src) { return create(src); }
	bool create(s32 numUpvar, bool isAddBcExInfo);
	bool create(const EsInterlang* src);
	bool create(EsBytecodeBuilder* bcb);

private:

	//======================================================================
	// 変数
private:
	VcString* _bc;						// バイトコード
	InsArray<const EsAtom*>* _arrAtom;	// リテラルアトム配列 ⇒ atomMap
	InsArray<EsObject*>* _arrObj;		// オブジェクト配列 ⇒ objectsOffset
	InsArray<EsUpvarInfo>* _arrUpvar;	// 上域変数情報配列 ⇒ JS_SCRIPT_UPVARS, upvarsOffset
	InsArray<EsObject*>* _arrRegexp;	// 正規表現オブジェクト配列 ⇒ regexpsOffset
	u16 _numFixedSlot;					// 引数の数 ⇒ nfixed
	u16 _numSlot;						// スロットの数 ⇒ nslots
	u32 _depthNest;						// ブロックの深さ ⇒ staticLevel
	bool _isAddBcExInfo;				// バイトコードに拡張情報を含んでいるかどうか
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_TEMPLATE_H_
