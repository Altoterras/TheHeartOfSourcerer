/***********************************************************************//**
 *  VwString.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/05.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_VW_STRING_H_
#define _TFW_STRING_VW_STRING_H_

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "VStringBase.h"

// Friends
#include "WStringBase.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

/*---------------------------------------------------------------------*//**
 *	可変ワイド文字列（Variable Wide-Character）クラス
 *		※ 扱える文字列サイズは 32bit 分までとする
 *
**//*---------------------------------------------------------------------*/
class VwString : public VStringBase<u16, WStringBase>
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	VwString() : VStringBase() {}
	VwString(const WStringBase* str) : VStringBase(str) {}
	VwString(const u16* psz) : VStringBase(psz) {}
	VwString(const u16* pc, int length) : VStringBase(pc, length) {}
	VwString(const u16 c) : VStringBase(c) {}
	VwString(int sizeBuf) : VStringBase(sizeBuf) {}
	VwString(const VwString& str) : VStringBase(str) {}
	VwString(const WStringBase& str) : VStringBase(str.getRaw()) {}
	VwString(const VStringBase& str) : VStringBase(str) {}

	// 文字列のコピー
	VwString& operator=(const VwString& str) { set(&str); return *this; }
	VwString& operator=(const WStringBase& str) { set(&str); return *this; }
	VwString& operator=(const u16* psz) { set(psz); return *this; }
	inline static VwString* setout(VwString* s, const WStringBase* str) { s->set(str); return s; }
	inline static VwString* setout(VwString* s, const u16* psz) { s->set(psz); return s; }
	inline static VwString* setout(VwString* s, const u16* pc, int length) { s->set(pc, length); return s; }
	inline static VwString* setout(VwString* s, const u16 c) { s->set(c); return s; }
	bool setEasily(const char* psz);
	bool setEasily(const CStringBase* str);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "WStringBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

template<class TYPE> class InsArray;

/*---------------------------------------------------------------------*//**
 *	可変ワイド文字列（Character of Variability）クラス
 *
**//*---------------------------------------------------------------------*/
class VwString : public WStringBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字列の長さを得る
	int getLength() const;

	// 文字列の取得
	operator const u16*() const { return (const u16*)getRaw(0); }
	const u16* getRaw(int index = 0) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	VwString();
	explicit VwString(const WStringBase* str);
	VwString(const u16* psz);
	VwString(const u16* pc, int length);
	VwString(const u16 c);
	VwString(int sizeBuf);
	VwString(const VwString& str);
	virtual ~VwString();

	// 文字列のクリア
	void empty();
	void emptyEasy();

	// 文字列のコピー
	bool set(const WStringBase* str);
	bool set(const CStringBase* str);
	bool set(const u16* psz);
	bool set(const u16* pc, int length);
	bool set(const u16 c);
	bool set(const char* psz);
	void copy(const WStringBase* str) { set(str); }
	void copy(const CStringBase* str) { set(str); }
	void copy(const u16* psz) { set(psz); }
	void copy(const u16* pc, int length) { set(pc, length); }
	void copy(const u16 c) { set(c); }
	VwString& operator=(const VwString& str) { set(&str); return *this; }
	VwString& operator=(const u16* psz) { set(psz); return *this; }
	VwString& operator=(const u16 c) { set(c); return *this; }
	inline static VwString* setout(VwString* s, const VwString* str) { s->set(str); return s; }
	inline static VwString* setout(VwString* s, const WStringBase* str) { s->set(str); return s; }
	inline static VwString* setout(VwString* s, const u16* psz) { s->set(psz); return s; }
	inline static VwString* setout(VwString* s, const u16* pc, int length) { s->set(pc, length); return s; }
	inline static VwString* setout(VwString* s, const u16 c) { s->set(c); return s; }

	// 文字の取得・設定
	u16 getAt(int index) const;
	bool setAt(int index, const u16 c);

	// 文字列の追加
	bool add(const VwString* str);
	bool add(const u16* psz);
	bool add(const u16* pc, int length);
	bool add(const u16 c);
	void operator+=(const VwString& str) { add(str); }
	void operator+=(const u16* psz) { add(psz); }
	void operator+=(const u16 c) { u16 cAdd[2] = {c, '\0'}; add(cAdd, 2); }

	// 追加文字列の作成
	VwString operator+(const VwString& str) const;
	VwString operator+(const u16* psz) const;
	VwString operator+(const u16 c) const;

	// 文字列の挿入
	bool insert(int index, const VwString* str);
	bool insert(int index, const u16* psz);
	bool insert(int index, const u16* pc, int length);
	bool insert(int index, const u16 c);

	// 文字列の削除
	bool remove(int index, int count);

	// 文字列の比較
	bool equals(const WStringBase* str) const { return equals(str->getRaw()); }
	bool equals(const u16* psz) const;
	bool equals(const u16* pc, int length) const;
	bool equals(u16 c) const;
	bool equalsIgnoreCase(const WStringBase* str) const;
	bool equalsIgnoreCase(const VwString* str) const;
	bool equalsIgnoreCase(const u16* psz) const;
	bool equalsIgnoreCase(const u16* pc, int length) const;
	bool equalsIgnoreCase(u16 c) const;
	bool operator==(const u16* psz) const { return equals(psz); }
	bool operator!=(const u16* psz) const { return !equals(psz); }

	// 文字（列）の検索
	int	find(const WStringBase* str, int idxBegin) const { return find(str->getRaw(), idxBegin); }
	int	find(const u16 c, int idxBegin) const;
	int	find(const u16* psz, int idxBegin) const;
	int	findReverse(const WStringBase* str) const { return findReverse(str->getRaw()); }
	int	findReverse(const u16 c) const;
	int	findReverse(const u16* psz) const;

	// 文字（列）の置換
	bool replace(const WStringBase* strOld, const u16* pszNew, int idxBegin, int cnt) { return replace(strOld->getRaw(), pszNew, idxBegin, cnt); }
	bool replace(const u16 cOld, const u16* pszNew, int idxBegin, int cnt);
	bool replace(const u16* pszOld, const u16* pszNew, int idxBegin, int cnt);
	inline bool replace(const WStringBase* strOld, const WStringBase* strNew, int idxBegin, int cnt) { return replace(strOld, strNew->getRaw(), idxBegin, cnt); }
	inline bool replace(const u16 cOld, const WStringBase* strNew, int idxBegin, int cnt) { return replace(cOld, strNew->getRaw(), idxBegin, cnt); }
	inline bool replace(const u16* pszOld, const WStringBase* strNew, int idxBegin, int cnt) { return replace(pszOld, strNew->getRaw(), idxBegin, cnt); }
	bool replaceAll(const WStringBase* strOld, const WStringBase* strNew) { return replace(strOld->getRaw(), strNew, 0, -1); }
	bool replaceAll(const u16 cOld, const WStringBase* strNew) { return replace(cOld, strNew, 0, -1); }
	bool replaceAll(const u16* pszOld,  const WStringBase* strNew) { return replace(pszOld, strNew, 0, -1); }
	bool replaceAll(const WStringBase* strOld, const u16* pszNew) { return replace(strOld->getRaw(), pszNew, 0, -1); }
	bool replaceAll(const u16 cOld, const u16* pszNew) { return replace(cOld, pszNew, 0, -1); }
	bool replaceAll(const u16* pszOld,  const u16* pszNew) { return replace(pszOld, pszNew, 0, -1); }

	// 部分文字列の作成
	VwString substring(int idxBegin, int idxEnd) const;
	VwString substring(int idxBegin) const;

	// 小文字化・大文字化
	VwString toLower() const;
	VwString toUpper() const;

	//======================================================================
	// 変数
protected:
	InsArray<u16>*	_arr;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////
#endif

#endif	// _TFW_STRING_VW_STRING_H_
