/***********************************************************************//**
 *  VStringBase.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/06.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_V_STRING_BASE_H_
#define _TFW_STRING_V_STRING_BASE_H_

////////////////////////////////////////////////////////////////////////////
// コンパイル設定

#if defined(_WINDOWS)
	#pragma warning(disable: 4996)
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Friends
#include "StringBase.h"
#include "../collection/InsArray.h"
#include "../lib/TmpBuf.h"

// External

// Library
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class InsArray;

/*---------------------------------------------------------------------*//**
 *	可変（Variability）文字列クラス
 *
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
class VStringBase : public CLASS_BASE
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字列の長さを得る
	int getLength() const;

	// 文字列の取得
	const TYPE* getRaw(int index = 0) const;
	operator const TYPE*() const { return getRaw(0); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	VStringBase();
	explicit VStringBase(const CLASS_BASE* str);
	explicit VStringBase(const TYPE* psz);
	VStringBase(const TYPE* pc, int length);
	explicit VStringBase(const TYPE c);
	explicit VStringBase(int sizeBuf);
	VStringBase(const VStringBase& str);
	virtual ~VStringBase();

	// 文字列のクリア
	void empty();
	void emptyEasy();

	// 文字列のコピー
//	bool set(const VStringBase* str);
	bool set(const CLASS_BASE* str);
	bool set(const TYPE* psz);
	bool set(const TYPE* pc, int length);
	bool set(const TYPE c);
//	void copy(const VStringBase* str) { set(str); }
	void copy(const CLASS_BASE* str) { set(str); }
	VStringBase& operator=(const VStringBase& str) { set(&str); return *this; }
	VStringBase& operator=(const CLASS_BASE& str) { set(&str); return *this; }
	VStringBase& operator=(const TYPE* psz) { set(psz); return *this; }
	VStringBase& operator=(const TYPE c) { set(c); return *this; }
//	inline static VStringBase* setout(VStringBase* s, const VStringBase* str) { s->set(str); return s; }
	inline static VStringBase* setout(VStringBase* s, const CLASS_BASE* str) { s->set(str); return s; }
	inline static VStringBase* setout(VStringBase* s, const TYPE* psz) { s->set(psz); return s; }
	inline static VStringBase* setout(VStringBase* s, const TYPE* pc, int length) { s->set(pc, length); return s; }
	inline static VStringBase* setout(VStringBase* s, const TYPE c) { s->set(c); return s; }

	// 文字の取得・設定
	TYPE getAt(int index) const;
	bool setAt(int index, const TYPE c);

	// 文字列の追加
	bool add(const CLASS_BASE* str);
	bool add(const TYPE* psz);
	bool add(const TYPE* pc, int length);
	bool add(const TYPE c);
	void operator+=(const VStringBase& str) { add(str); }
	void operator+=(const TYPE* psz) { add(psz); }
	void operator+=(const TYPE c) { TYPE cAdd[2] = {c, '\0'}; add(cAdd, 2); }

	// 追加文字列の作成
	VStringBase operator+(const VStringBase& str) const;
	VStringBase operator+(const TYPE* psz) const;
	VStringBase operator+(const TYPE c) const;

	// 文字列の挿入
	bool insert(int index, const CLASS_BASE* str);
	bool insert(int index, const TYPE* psz);
	bool insert(int index, const TYPE* pc, int length);
	bool insert(int index, const TYPE c);

	// 文字列の削除
	bool remove(int index, int count);

	// 文字列の比較
	bool equals(const CLASS_BASE* str) const { return equals(str->getRaw()); }
	bool equals(const TYPE* psz) const;
	bool equals(const TYPE* pc, int length) const;
	bool equals(TYPE c) const;
	bool equalsIgnoreCase(const CLASS_BASE* str) const;
	bool equalsIgnoreCase(const VStringBase* str) const;
	bool equalsIgnoreCase(const TYPE* psz) const;
	bool equalsIgnoreCase(const TYPE* pc, int length) const;
	bool equalsIgnoreCase(TYPE c) const;
	bool operator==(const CLASS_BASE* str) const { return equals(str); }
	bool operator==(const TYPE* psz) const { return equals(psz); }
	bool operator!=(const TYPE* psz) const { return !equals(psz); }

	// 文字（列）の検索
	int	find(const CLASS_BASE* str, int idxBegin) const { return find(str->getRaw(), idxBegin); }
	int	find(const TYPE c, int idxBegin) const;
	int	find(const TYPE* psz, int idxBegin) const;
	int	findReverse(const CLASS_BASE* str) const { return findReverse(str->getRaw()); }
	int	findReverse(const TYPE c) const;
	int	findReverse(const TYPE* psz) const;

	// 文字（列）の置換
	bool replace(const CLASS_BASE* strOld, const TYPE* pszNew, int idxBegin, int cnt) { return replace(strOld->getRaw(), pszNew, idxBegin, cnt); }
	bool replace(const TYPE cOld, const TYPE* pszNew, int idxBegin, int cnt);
	bool replace(const TYPE* pszOld, const TYPE* pszNew, int idxBegin, int cnt);
	inline bool replace(const CLASS_BASE* strOld, const CLASS_BASE* strNew, int idxBegin, int cnt) { return replace(strOld, strNew->getRaw(), idxBegin, cnt); }
	inline bool replace(const TYPE cOld, const CLASS_BASE* strNew, int idxBegin, int cnt) { return replace(cOld, strNew->getRaw(), idxBegin, cnt); }
	inline bool replace(const TYPE* pszOld, const CLASS_BASE* strNew, int idxBegin, int cnt) { return replace(pszOld, strNew->getRaw(), idxBegin, cnt); }
	bool replaceAll(const CLASS_BASE* strOld, const CLASS_BASE* strNew) { return replace(strOld->getRaw(), strNew, 0, -1); }
	bool replaceAll(const TYPE cOld, const CLASS_BASE* strNew) { return replace(cOld, strNew, 0, -1); }
	bool replaceAll(const TYPE* pszOld,  const CLASS_BASE* strNew) { return replace(pszOld, strNew, 0, -1); }
	bool replaceAll(const CLASS_BASE* strOld, const TYPE* pszNew) { return replace(strOld->getRaw(), pszNew, 0, -1); }
	bool replaceAll(const TYPE cOld, const TYPE* pszNew) { return replace(cOld, pszNew, 0, -1); }
	bool replaceAll(const TYPE* pszOld,  const TYPE* pszNew) { return replace(pszOld, pszNew, 0, -1); }

	// 部分文字列の作成
	VStringBase substring(int idxBegin, int idxEnd) const;
	VStringBase substring(int idxBegin) const;

	// 小文字化・大文字化
	VStringBase toLower() const;
	VStringBase toUpper() const;

	// ユーティリティ関数
	static int calcStrLen(const TYPE* sz);
	static const TYPE* findStrByChar(const TYPE* sz, TYPE c);
	static const TYPE* findLastStrByChar(const TYPE* sz, TYPE c);
	static const TYPE* findStrByStr(const TYPE* sz, const TYPE* str);
	static const TYPE* findLastStrByStr(const TYPE* sz, const TYPE* str);

	//======================================================================
	// 変数
protected:
	InsArray<TYPE>*	_arr;
};

////////////////////////////////////////////////////////////////////////////
// グローバル関数

/*---------------------------------------------------------------------*//**
	文字列の長さを得る
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
int VStringBase<TYPE, CLASS_BASE>::calcStrLen(const TYPE* sz)
{
	int i = 0;
	while(sz[i] != '\0'){ i++; }
	return i;
}

/*---------------------------------------------------------------------*//**
	部分文字列を文字から検索する
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
const TYPE* VStringBase<TYPE, CLASS_BASE>::findStrByChar(const TYPE* sz, TYPE c)
{
	while(*sz != 0)
	{
		if(*sz == c)
		{
			return sz;
		}
		sz++;
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	最後の部分文字列を文字から検索する
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
const TYPE* VStringBase<TYPE, CLASS_BASE>::findLastStrByChar(const TYPE* sz, TYPE c)
{
	const TYPE* szTop = sz;
	while(*sz != 0)
	{
		sz++;
	}
	sz--;
	if(sz < szTop)
	{
		return 0L;
	}
	while(sz >= szTop)
	{
		if(*sz == c)
		{
			return sz;
		}
		sz--;
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	部分文字列を文字列から検索する
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
const TYPE* VStringBase<TYPE, CLASS_BASE>::findStrByStr(const TYPE* sz, const TYPE* str)
{
	const TYPE* strTop = str;
	while(*sz != 0)
	{
		if(*sz == *strTop)
		{
			const TYPE* szWk = sz;
			const TYPE* strWk = strTop;
			do
			{
				szWk++;
				strWk++;
				if(*strWk == 0)
				{
					return sz;
				}
				if(*szWk != *strWk)
				{
					break;
				}
			}
			while(*szWk != 0);
		}
		sz++;
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	最後の部分文字列を文字列から検索する
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
const TYPE* VStringBase<TYPE, CLASS_BASE>::findLastStrByStr(const TYPE* sz, const TYPE* str)
{
	const TYPE* szTop = sz;
	while(*sz != 0)
	{
		sz++;
	}
	sz--;
	if(sz < szTop)
	{
		return 0L;
	}
	const TYPE* strTop = str;
	while(*str != 0)
	{
		str++;
	}
	str--;
	if(str < strTop)
	{
		return 0L;
	}
	const TYPE* strTail = str;
	do
	{
		if(*sz == *strTail)
		{
			const TYPE* szWk = sz;
			const TYPE* strWk = strTail;
			do
			{
				if(strWk == strTop)
				{
					return szWk;
				}
				szWk--;
				strWk--;
				if(*szWk != *strWk)
				{
					break;
				}
			}
			while(szWk >= szTop);
		}
		sz--;
	}
	while(sz >= szTop);
	return 0L;
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	文字列の長さを得る
	@return		文字列の長さ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
int VStringBase<TYPE, CLASS_BASE>::getLength() const
{
	return _arr->getCount() - 1;
}

/*---------------------------------------------------------------------*//**
	文字列ポインタを得る
	@return		文字列ポインタ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
const TYPE* VStringBase<TYPE, CLASS_BASE>::getRaw(int index) const
{
	return _arr->getPointer(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase()
	: _arr(new InsArray<TYPE>(true, false))
{
	_arr->setGrow2time(true);
	TYPE z = 0;
	set(&z, 1);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	str				CLASS_BASE インスタンスへのポインタ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase(const CLASS_BASE* str)
	: _arr(new InsArray<TYPE>(true, false))
{
	_arr->setGrow2time(true);
	set(str->getRaw(), str->getLength() + 1);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase(const TYPE* psz)
	: _arr(new InsArray<TYPE>(true, false))
{
	_arr->setGrow2time(true);
	set(psz);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	pc				文字配列
	@param	length			長さ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase(const TYPE* pc, int length)
	: _arr(new InsArray<TYPE>(true, false))
{
	_arr->setGrow2time(true);
	set(pc, length);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	c				文字
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase(const TYPE c)
	: _arr(new InsArray<TYPE>(true, false))
{
	_arr->setGrow2time(true);
	set(c);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	sizeBuf			初期バッファサイズ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase(int sizeBuf)
	: _arr(new InsArray<TYPE>(true, false))
{
	_arr->setGrow2time(true);
	if(sizeBuf <= 0)	{	sizeBuf = 1;	}
	sizeBuf = FIT_ALIGN(sizeBuf, 4);
	_arr->alloc(sizeBuf);
	TYPE z = 0;
	_arr->addArray(&z, 1);
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
	@param	str				VStringBase インスタンスへの参照
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::VStringBase(const VStringBase& str)
	: _arr(new InsArray<TYPE>(*str._arr))
{
	_arr->setGrow2time(true);
	set(str.getRaw(), str.getLength() + 1);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE>::~VStringBase()
{
	delete _arr;
	_arr = 0L;
}

/*---------------------------------------------------------------------*//**
	文字列のクリア（メモリの解放を行わない簡易版）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
void VStringBase<TYPE, CLASS_BASE>::empty()
{
	TYPE z = 0;
	set(&z, 1);
}

/*---------------------------------------------------------------------*//**
	文字列のクリア（メモリの解放を行わない簡易版）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
void VStringBase<TYPE, CLASS_BASE>::emptyEasy()
{
	_arr->setAt(0, '\0');
	_arr->setCount(1);
}

#if 0
/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	str				CLASS_BASE インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::set(const VStringBase* str)
{
	return set(str->getRaw(), str->getLength() + 1);	// NULL 文字分もコピー
}
#endif

/*---------------------------------------------------------------------*//**
	文字列を設定する
	※	単純コピーであるので、文字エンコード機能は無い点に注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	str				CStringBase インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::set(const CLASS_BASE* str)
{
	if(!_arr->setArray(0L, str->getLength() + 1))
	{
		return false;
	}

	for(int i = 0; i < str->getLength(); i++)
	{
		if(!_arr->setAt(i, str->getAt(i)))
		{
			return false;
		}
	}

	if(!_arr->setAt(str->getLength(), '\0'))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::set(const TYPE* psz)
{
	return set(psz, calcStrLen(psz) + 1);				// NULL 文字分もコピー
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	pc				TYPE 配列
			length			配列の長さ（文字列の場合は NULL 文字も含む）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::set(const TYPE* pc, int length)
{
	if(!_arr->setArray(pc, length))
	{
		return false;
	}

	if((length == 0) || ((length > 0) && (pc[length - 1] != '\0')))
	{
		if(!_arr->add('\0'))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字を設定する
	@param	c				文字
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::set(const TYPE c)
{
	TYPE cWork[2] = { c, '\0' };
	return set(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	文字の取得
	@param		index 文字列インデックス
	@return		指定インデックスの文字
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
TYPE VStringBase<TYPE, CLASS_BASE>::getAt(int index) const
{
	return _arr->getAt(index);
}

/*---------------------------------------------------------------------*//**
	文字の設定
	@param		index 文字列インデックス
	@param		c 設定文字
	@retval		true 成功
	@retval		false 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::setAt(int index, const TYPE c)
{
	return _arr->setAt(index, c);
}

/*---------------------------------------------------------------------*//**
	文字列の追加
	@param	str				VStringBase インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::add(const CLASS_BASE* str)
{
	_arr->setCount(_arr->getCount() - 1);								// 現在の文字列から NULL 文字を除外

	if(_arr->addArray(str->getRaw(), str->getLength() + 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*---------------------------------------------------------------------*//**
	文字列の追加
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::add(const TYPE* psz)
{
	_arr->setCount(_arr->getCount() - 1);								// 現在の文字列から NULL 文字を除外

	return _arr->addArray(psz, calcStrLen(psz) + 1);					// NULL 文字もコピー
}

/*---------------------------------------------------------------------*//**
	TYPE 配列の追加
	@param	pc				TYPE 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::add(const TYPE* pc, int length)
{
	if(length == 0) {	return false;	}

	// 長さ１で NULL 文字のみの場合は追加の必要はない
	if((length == 1) && (pc[0] == '\0'))
	{
		//ASSERT(length > 0);
		//ASSERT(_pelArray[length - 1] == '\0');
		return true;	// 配列は常に NULL 終端されているはずなので追加の必要はない
	}

	if(_arr->setCount(_arr->getCount() - 1))	// 現在の文字列から NULL 文字を除外
	{
		if(_arr->addArray(pc, length))
		{
			// 終端
			if(*_arr->getPointer(_arr->getCount() - 1) != '\0')	// この関数では NULL 終端されていない文字列も許可している
			{
				TYPE z = 0;
				_arr->addArray(&z, 1);
			}

			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	TYPE 配列の追加
	@param	c				文字
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::add(const TYPE c)
{
	TYPE cWork[2] = { c, '\0' };
	return add(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	str				VStringBase インスタンスへの参照
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::operator+(const VStringBase& str) const
{
	VStringBase rstrReturn(*this);	// コピーの作成
	rstrReturn.add(str);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	psz				文字列
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::operator+(const TYPE* psz) const
{
	VStringBase rstrReturn(*this);	// コピーの作成
	rstrReturn.add(psz);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	c				文字
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::operator+(const TYPE c) const
{
	VStringBase rstrReturn(*this);	// コピーの作成
	TYPE cAdd[2] = {c, '\0'};
	rstrReturn.add(cAdd, 2);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	文字列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	str				VStringBase インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::insert(int index, const CLASS_BASE* str)
{
	return insert(index, str->getRaw(), str->getLength());
}

/*---------------------------------------------------------------------*//**
	文字列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::insert(int index, const TYPE* psz)
{
	return insert(index, psz, calcStrLen(psz));
}

/*---------------------------------------------------------------------*//**
	TYPE 配列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	pc				TYPE 配列
	@param	length			配列の長さ（NULL 文字は含めない）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::insert(int index, const TYPE* pc, int length)
{
	if(!_arr->insert(index, pc, length))
	{
		return false;
	}

	// NULL 終端を行う
	if((_arr->getCount() <= 0) || (*_arr->getPointer(_arr->getCount() - 1) != '\0'))
	{
		_arr->add('\0');
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	c				文字
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::insert(int index, const TYPE c)
{
	return insert(index, &c, 1);
}

/*---------------------------------------------------------------------*//**
	文字列の削除
	@param	index			削除する配列インデックス
	@param	count			削除する要素の数
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::remove(int index, int count)
{
	if(!_arr->remove(index, count))	{	return false;	}

	// NULL 終端を行う
	if((_arr->getCount() <= 0) || (*_arr->getPointer(_arr->getCount() - 1) != '\0'))
	{
		_arr->add('\0');
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列が同一であるか判定
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equals(const TYPE* psz) const
{
	return equals(psz, calcStrLen(psz) + 1);
}

/*---------------------------------------------------------------------*//**
	TYPE 配列が同一であるか判定
	@param	pc				TYPE 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equals(const TYPE* pc, int length) const
{
	return _arr->equals(pc, length);
}

/*---------------------------------------------------------------------*//**
	同一であるか判定
	@param	c				比較文字
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equals(TYPE c) const
{
	TYPE cWork[2] = { c, '\0' };
	return equals(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	str				文字列
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equalsIgnoreCase(const CLASS_BASE* str) const
{
	VStringBase trgL = VStringBase(str).toLower();
	VStringBase thisL = this->toLower();
	return thisL.equals(&trgL);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	str				文字列
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equalsIgnoreCase(const VStringBase* str) const
{
	VStringBase trgL = str->toLower();
	VStringBase thisL = this->toLower();
	return thisL.equals(&trgL);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equalsIgnoreCase(const TYPE* psz) const
{
	VStringBase strWk(psz);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、TYPE 配列が同一であるか判定
	@param	pc				TYPE 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equalsIgnoreCase(const TYPE* pc, int length) const
{
	VStringBase strWk(pc, length);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、同一であるか判定
	@param	c				比較文字
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::equalsIgnoreCase(TYPE c) const
{
	VStringBase strWk(c);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	文字の検索
	@param	c				検索文字
	@param	idxBegin		検索を開始する文字列インデックス
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
int VStringBase<TYPE, CLASS_BASE>::find(const TYPE c, int idxBegin) const
{
	if((idxBegin < 0) || ((u32)idxBegin >= (_arr->getCount() - 1)))	{	return -1;	}
	const TYPE* pcFind = findStrByChar(getRaw(idxBegin), c);
	if(pcFind == NULL)								{	return -1;	}
	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字列の検索
	@param	psz				検索文字列
	@param	idxBegin		検索を開始する文字列インデックス
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
int VStringBase<TYPE, CLASS_BASE>::find(const TYPE* psz, int idxBegin) const
{
	if((idxBegin < 0) || ((u32)idxBegin >= (_arr->getCount() - 1)))	{	return -1;	}
	const TYPE* pcFind = findStrByStr(getRaw(idxBegin), psz);
	if(pcFind == NULL)								{	return -1;	}
	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字の逆検索
	@param	c				検索文字
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
int VStringBase<TYPE, CLASS_BASE>::findReverse(const TYPE c) const
{
	const TYPE* pcFind = findLastStrByChar(getRaw(0), c);
	if(pcFind == NULL)								{	return -1;	}
	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字列の逆検索
	@param	psz				検索文字列
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
int VStringBase<TYPE, CLASS_BASE>::findReverse(const TYPE* psz) const
{
#if 1
	const TYPE* pcFind = findLastStrByStr(getRaw(0), psz);
	if(pcFind == NULL)								{	return -1;	}
	return (int)(pcFind - getRaw(0));
#else	// こちらでも問題ない
	int i, j;
	int lenSrc = calcStrLen(psz);

	if(lenSrc == 0)								{	return -1;	}

	TYPE* pc = _arr->getPointer();
	i = _arr->getCount() - 1;
	while(i >= 0)
	{
		if(pc[i] == psz[lenSrc - 1])	// 最後の文字の一致を発見
		{
			// 一致し続けるまでループ
			j = 1;
			while(((i - j) >= 0) && (j < lenSrc))
			{
				if(pc[i - j] != psz[lenSrc - 1 - j])	// 不一致を発見
				{
					break;
				}

				j++;
			}

			if(j == lenSrc)	// 完全一致した
			{
				return i - (lenSrc - 1);
			}
		}

		i--;
	}

	return -1;
#endif
}

/*---------------------------------------------------------------------*//**
	文字（列）の置換
	@param	cOld			置換対象文字
	@param	pszNew			置換後文字列
	@param	idxBegin		開始インデックス
	@param	cnt				置換を行う数（-1 は無限）
	@retval	true			置換が行われた
	@retval false			置換が行われなかった
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::replace(const TYPE cOld, const TYPE* pszNew, int idxBegin, int cnt)
{
	VStringBase strResult;
	bool isReplace = false;
	if(idxBegin > 0)	{	strResult += this->substring(0, idxBegin);	}

	int cntReplace = 0;
	int idxStart = idxBegin;
	int idxEnd = 0;
	while(true)
	{
		idxEnd = find(cOld, idxStart);
		if(idxEnd == -1)
		{
			strResult += this->substring(idxStart);
			break;
		}

		strResult += this->substring(idxStart, idxEnd);
		if(cntReplace++ < cnt)
		{
			strResult += pszNew;
		}
		else
		{
			strResult += cOld;
		}
		isReplace = true;

		idxStart = idxEnd + 1;
	}

	this->set(strResult);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字（列）の置換
	@param	pszOld			置換対象文字列
	@param	pszNew			置換後文字列
	@param	idxBegin		開始インデックス
	@param	cnt				置換を行う数（-1 は無限）
	@retval	true			置換が行われた
	@retval false			置換が行われなかった
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
bool VStringBase<TYPE, CLASS_BASE>::replace(const TYPE* pszOld, const TYPE* pszNew, int idxBegin, int cnt)
{
	VStringBase strResult;
	bool isReplace = false;
	if(idxBegin > 0)	{	strResult += this->substring(0, idxBegin);	}

	int cntReplace = 0;
	int lenOld = calcStrLen(pszOld);
	int idxStart = idxBegin;
	int idxEnd = 0;
	while(true)
	{
		idxEnd = find(pszOld, idxStart);
		if(idxEnd == -1)
		{
			strResult += this->substring(idxStart);
			break;
		}

		strResult += this->substring(idxStart, idxEnd);
		if((cnt == -1) || (cntReplace++ < cnt))
		{
			strResult += pszNew;
		}
		else
		{
			strResult += pszOld;
		}
		isReplace = true;

		idxStart = idxEnd + lenOld;
	}

	this->set(strResult);
	return true;
}

/*---------------------------------------------------------------------*//**
	部分文字列の作成
	@param	idxBegin		開始インデックス
	@param	idxEnd			終了インデックス（この値を含まない）
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::substring(int idxBegin, int idxEnd) const
{
	if(idxBegin < 0)							{	return VStringBase();	}
	if((u32)idxEnd > (_arr->getCount() - 1))	{	return VStringBase();	}
	if(idxBegin > idxEnd)						{	return VStringBase();	}

	if((idxBegin == 0) && (idxEnd == (_arr->getCount() - 1)))	{	return *this;	}

	int size = idxEnd - idxBegin;
	VStringBase str(size + 1);
	str._arr->setCount(0);	// 現在の文字列から NULL 文字を除外
	str._arr->addArray(_arr->getPointer(idxBegin), size);
	TYPE z = 0;
	str._arr->addArray(&z, 1);
	return str;
}

/*---------------------------------------------------------------------*//**
	部分文字列の作成
	@param	idxBegin		開始インデックス
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::substring(int idxBegin) const
{
	return substring(idxBegin, _arr->getCount() - 1);
}

/*---------------------------------------------------------------------*//**
	小文字化した文字列の作成
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::toLower() const
{
	int len = this->getLength();

	const u32 SIZE_SBUF = 16;
	u8 sbuf[SIZE_SBUF];
	TmpBuf tmpbuf(sizeof(TYPE) * (len + 1), sbuf, SIZE_SBUF);

	const TYPE* sz = this->getRaw();
	TYPE* cbuf = (TYPE*)tmpbuf.get();
	for(int i = 0; i < len; i++)	{	cbuf[i] = (TYPE)::tolower(sz[i]);	}
	cbuf[len] = '\0';

	return VStringBase(cbuf);
}

/*---------------------------------------------------------------------*//**
	大文字化した文字列の作成
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
template <class TYPE, class CLASS_BASE>
VStringBase<TYPE, CLASS_BASE> VStringBase<TYPE, CLASS_BASE>::toUpper() const
{
	int len = this->getLength();

	const u32 SIZE_SBUF = 16;
	u8 sbuf[SIZE_SBUF];
	TmpBuf tmpbuf(sizeof(TYPE) * (len + 1), sbuf, SIZE_SBUF);

	const TYPE* sz = this->getRaw();
	TYPE* cbuf = (TYPE*)tmpbuf.get();
	for(int i = 0; i < len; i++)	{	cbuf[i] = (TYPE)::toupper(sz[i]);	}
	cbuf[len] = '\0';

	return VStringBase(cbuf);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_STRING_V_STRING_BASE_H_
