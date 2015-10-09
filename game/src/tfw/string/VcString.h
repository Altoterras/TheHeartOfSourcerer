/***********************************************************************//**
 *  VcString.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_VC_STRING_H_
#define _TFW_STRING_VC_STRING_H_

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "VStringBase.h"

// Friends
#include "CStringBase.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class WStringBase;

/*---------------------------------------------------------------------*//**
 *	可変文字列（Variable Character）クラス
 *		※ 扱える文字列サイズは 32bit 分までとする
 *
**//*---------------------------------------------------------------------*/
class VcString : public VStringBase<char, CStringBase>
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	VcString() : VStringBase() {}
	VcString(const CStringBase* str) : VStringBase(str) {}
	VcString(const char* psz) : VStringBase(psz) {}
	VcString(const char* pc, int length) : VStringBase(pc, length) {}
	VcString(const char c) : VStringBase(c) {}
	VcString(int sizeBuf) : VStringBase(sizeBuf) {}
	VcString(const VcString& str) : VStringBase(str) {}
	VcString(const CStringBase& str) : VStringBase(str.getRaw()) {}
	VcString(const VStringBase& str) : VStringBase(str) {}

	// 文字列のコピー
	VcString& operator=(const VcString& str) { set(&str); return *this; }
	VcString& operator=(const CStringBase& str) { set(&str); return *this; }
	VcString& operator=(const char* psz) { set(psz); return *this; }
	VcString& operator=(const char c) { set(c); return *this; }
	inline static VcString* setout(VcString* s, const CStringBase* str) { s->set(str); return s; }
	inline static VcString* setout(VcString* s, const char* psz) { s->set(psz); return s; }
	inline static VcString* setout(VcString* s, const char* pc, int length) { s->set(pc, length); return s; }
	inline static VcString* setout(VcString* s, const char c) { s->set(c); return s; }
	bool setEasily(const u16* psz);
	bool setEasily(const WStringBase* str);

	// 文字列を書式化設定
	static VcString format(const char* psz, ...);
	static VcString* format(VcString* s, const char* psz, ...);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "CStringBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

template<class TYPE> class InsArray;

/*---------------------------------------------------------------------*//**
 *	可変文字列（Character of Variability）クラス
 *
**//*---------------------------------------------------------------------*/
class VcString : public CStringBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字列の長さを得る
	int getLength() const;

	// 文字列の取得
	operator const char*() const { return (const char*)getRaw(0); }
	const char* getRaw(int index = 0) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	VcString();
	explicit VcString(const CStringBase* str);
	VcString(const char* psz);
	VcString(const char* pc, int length);
	VcString(const char c);
	VcString(int sizeBuf);
	VcString(const VcString& str);
	virtual ~VcString();

	// 文字列のクリア
	void empty();
	void emptyEasy();

	// 文字列のコピー
	bool set(const CStringBase* str);
	bool set(const char* psz);
	bool set(const char* pc, int length);
	bool set(const char c);
	void copy(const CStringBase* str) { set(str); }
	void copy(const char* psz) { set(psz); }
	void copy(const char* pc, int length) { set(pc, length); }
	void copy(const char c) { set(c); }
	VcString& operator=(const VcString& str) { set(&str); return *this; }
	VcString& operator=(const char* psz) { set(psz); return *this; }
	VcString& operator=(const char c) { set(c); return *this; }
	inline static VcString* setout(VcString* s, const VcString* str) { s->set(str); return s; }
	inline static VcString* setout(VcString* s, const CStringBase* str) { s->set(str); return s; }
	inline static VcString* setout(VcString* s, const char* psz) { s->set(psz); return s; }
	inline static VcString* setout(VcString* s, const char* pc, int length) { s->set(pc, length); return s; }
	inline static VcString* setout(VcString* s, const char c) { s->set(c); return s; }

	// 文字の取得・設定
	char getAt(int index) const;
	bool setAt(int index, const char c);

	// 文字列の追加
	bool add(const VcString* str);
	bool add(const char* psz);
	bool add(const char* pc, int length);
	bool add(const char c);
	void operator+=(const VcString& str) { add(str); }
	void operator+=(const char* psz) { add(psz); }
	void operator+=(const char c) { char cAdd[2] = {c, '\0'}; add(cAdd, 2); }

	// 追加文字列の作成
	VcString operator+(const VcString& str) const;
	VcString operator+(const char* psz) const;
	VcString operator+(const char c) const;

	// 文字列の挿入
	bool insert(int index, const VcString* str);
	bool insert(int index, const char* psz);
	bool insert(int index, const char* pc, int length);
	bool insert(int index, const char c);

	// 文字列の削除
	bool remove(int index, int count);

	// 文字列の比較
	bool equals(const CStringBase* str) const { return equals(str->getRaw()); }
	bool equals(const char* psz) const;
	bool equals(const char* pc, int length) const;
	bool equals(char c) const;
	bool equalsIgnoreCase(const CStringBase* str) const;
	bool equalsIgnoreCase(const VcString* str) const;
	bool equalsIgnoreCase(const char* psz) const;
	bool equalsIgnoreCase(const char* pc, int length) const;
	bool equalsIgnoreCase(char c) const;
	bool operator==(const char* psz) const { return equals(psz); }
	bool operator!=(const char* psz) const { return !equals(psz); }

	// 文字（列）の検索
	int	find(const CStringBase* str, int idxBegin) const { return find(str->getRaw(), idxBegin); }
	int	find(const char c, int idxBegin) const;
	int	find(const char* psz, int idxBegin) const;
	int	findReverse(const CStringBase* str) const { return findReverse(str->getRaw()); }
	int	findReverse(const char c) const;
	int	findReverse(const char* psz) const;

	// 文字（列）の置換
	bool replace(const CStringBase* strOld, const char* pszNew, int idxBegin, int cnt) { return replace(strOld->getRaw(), pszNew, idxBegin, cnt); }
	bool replace(const char cOld, const char* pszNew, int idxBegin, int cnt);
	bool replace(const char* pszOld, const char* pszNew, int idxBegin, int cnt);
	inline bool replace(const CStringBase* strOld, const CStringBase* strNew, int idxBegin, int cnt) { return replace(strOld, strNew->getRaw(), idxBegin, cnt); }
	inline bool replace(const char cOld, const CStringBase* strNew, int idxBegin, int cnt) { return replace(cOld, strNew->getRaw(), idxBegin, cnt); }
	inline bool replace(const char* pszOld, const CStringBase* strNew, int idxBegin, int cnt) { return replace(pszOld, strNew->getRaw(), idxBegin, cnt); }
	bool replaceAll(const CStringBase* strOld, const CStringBase* strNew) { return replace(strOld->getRaw(), strNew, 0, -1); }
	bool replaceAll(const char cOld, const CStringBase* strNew) { return replace(cOld, strNew, 0, -1); }
	bool replaceAll(const char* pszOld,  const CStringBase* strNew) { return replace(pszOld, strNew, 0, -1); }
	bool replaceAll(const CStringBase* strOld, const char* pszNew) { return replace(strOld->getRaw(), pszNew, 0, -1); }
	bool replaceAll(const char cOld, const char* pszNew) { return replace(cOld, pszNew, 0, -1); }
	bool replaceAll(const char* pszOld,  const char* pszNew) { return replace(pszOld, pszNew, 0, -1); }

	// 部分文字列の作成
	VcString substring(int idxBegin, int idxEnd) const;
	VcString substring(int idxBegin) const;

	// 小文字化・大文字化
	VcString toLower() const;
	VcString toUpper() const;

	// 文字列を書式化設定
	static VcString format(const char* psz, ...);
	static VcString* format(VcString* s, const char* psz, ...);

	//======================================================================
	// 変数
protected:
	InsArray<char>*	_arr;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////
#endif

#endif	// _TFW_STRING_VC_STRING_H_
