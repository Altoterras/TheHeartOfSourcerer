/***********************************************************************//**
 *  VwString.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/05.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "VwString.h"

// Friends
#include "VcString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// VwString メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	文字列を単純設定する
	※	単純コピー（キャスト）なので、文字エンコード機能は無い点に要注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::setEasily(const char* psz)
{
	int length = VStringBase<char, CStringBase>::calcStrLen(psz);
	if(!_arr->setArray(0L, length + 1))
	{
		return false;
	}

	for(int i = 0; i < length; i++)
	{
		if(!_arr->setAt(i, psz[i]))
		{
			return false;
		}
	}

	if(!_arr->setAt(length, '\0'))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列を単純設定する
	※	単純コピーであるので、文字エンコード機能は無い点に注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::setEasily(const CStringBase* str)
{
	return setEasily(str->getRaw());
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
#else
////////////////////////////////////////////////////////////////////////////
// コンパイル設定

#if defined(_WINDOWS)
	#pragma warning(disable: 4996)
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Self
#include "VwString.h"

// Friends
#include "../collection/InsArray.h"

// External

// Library
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

static int wstrlen(const u16* sz)
{
	int i = 0;
	while(sz[i] != '\0'){ i++; }
	return i;
}

// ※要デバッグ！！　［2012/12/05 r-kishi］
static const u16* wstrchr(const u16* sz, u16 c)
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

// ※要デバッグ！！　［2012/12/05 r-kishi］
static const u16* wstrrchr(const u16* sz, u16 c)
{
	const u16* szTop = sz;
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

// ※要デバッグ！！　［2012/12/05 r-kishi］
static const u16* wstrstr(const u16* sz, const u16* str)
{
	const u16* szTop = sz;
	while(*sz != 0)
	{
		sz++;
	}
	sz--;
	if(sz < szTop)
	{
		return 0L;
	}
	const u16* strTop = str;
	while(*str != 0)
	{
		str++;
	}
	str--;
	if(str < strTop)
	{
		return 0L;
	}
	const u16* strTail = str;
	while(sz != szTop)
	{
		if(*sz == *strTail)
		{
			const u16* szWk = sz;
			const u16* strWk = strTail;
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
int VwString::getLength() const
{
	return _arr->getCount() - 1;
}

/*---------------------------------------------------------------------*//**
	文字列ポインタを得る
	@return		文字列ポインタ
**//*---------------------------------------------------------------------*/
const u16* VwString::getRaw(int index) const
{
	return _arr->getPointer(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
VwString::VwString()
	: _arr(new InsArray<u16>(true, false))
{
	/* ///test
	u16 buf1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
	u16 buf2[] = { 3, 2, 0 };
	const u16* f1 = wstrchr(buf1, 0);
	const u16* f2 = wstrrchr(buf1, 1);
	const u16* f3 = wstrstr(buf1, buf2);
	TRACE("%x, %x, %x\n", f1, f2, f3);
	ASSERT(false);	// 要デバッグコード削除
	*/

	_arr->setGrow2time(true);
	set(0, 1);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	str				WStringBase インスタンスへのポインタ
**//*---------------------------------------------------------------------*/
VwString::VwString(const WStringBase* str)
	: _arr(new InsArray<u16>(true, false))
{
	_arr->setGrow2time(true);
	set(str->getRaw(), str->getLength() + 1);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
VwString::VwString(const u16* psz)
	: _arr(new InsArray<u16>(true, false))
{
	_arr->setGrow2time(true);
	set(psz);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	pc				文字配列
	@param	length			長さ
**//*---------------------------------------------------------------------*/
VwString::VwString(const u16* pc, int length)
	: _arr(new InsArray<u16>(true, false))
{
	_arr->setGrow2time(true);
	set(pc, length);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	c				文字
**//*---------------------------------------------------------------------*/
VwString::VwString(const u16 c)
	: _arr(new InsArray<u16>(true, false))
{
	_arr->setGrow2time(true);
	set(c);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	sizeBuf			初期バッファサイズ
**//*---------------------------------------------------------------------*/
VwString::VwString(int sizeBuf)
	: _arr(new InsArray<u16>(true, false))
{
	_arr->setGrow2time(true);
	if(sizeBuf <= 0)	{	sizeBuf = 1;	}
	sizeBuf = FIT_ALIGN(sizeBuf, 4);
	_arr->alloc(sizeBuf);
	_arr->addArray(0, 1);
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
	@param	str				VwString インスタンスへの参照
**//*---------------------------------------------------------------------*/
VwString::VwString(const VwString& str)
	: _arr(new InsArray<u16>(*str._arr))
{
	_arr->setGrow2time(true);
	set(str.getRaw(), str.getLength() + 1);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
VwString::~VwString()
{
	delete _arr;
	_arr = 0L;
}

/*---------------------------------------------------------------------*//**
	文字列のクリア（メモリの解放を行わない簡易版）
**//*---------------------------------------------------------------------*/
void VwString::empty()
{
	set(0, 1);
}

/*---------------------------------------------------------------------*//**
	文字列のクリア（メモリの解放を行わない簡易版）
**//*---------------------------------------------------------------------*/
void VwString::emptyEasy()
{
	_arr->setAt(0, '\0');
	_arr->setCount(1);
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	str				WStringBase インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::set(const WStringBase* str)
{
	return set(str->getRaw(), str->getLength() + 1);	// NULL 文字分もコピー
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	※	単純コピーであるので、文字エンコード機能は無い点に注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	str				CStringBase インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::set(const CStringBase* str)
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

	if(!_arr->add('\0'))
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
bool VwString::set(const u16* psz)
{
	return set(psz, wstrlen(psz) + 1);				// NULL 文字分もコピー
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	pc				u16 配列
			length			配列の長さ（文字列の場合は NULL 文字も含む）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::set(const u16* pc, int length)
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
bool VwString::set(const u16 c)
{
	u16 cWork[2] = { c, '\0' };
	return set(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	※	単純コピーであるので、文字エンコード機能は無い点に注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::set(const char* psz)
{
	int length = ::strlen(psz);
	if(!_arr->setArray(0L, length + 1))
	{
		return false;
	}

	for(int i = 0; i < length; i++)
	{
		if(!_arr->setAt(i, psz[i]))
		{
			return false;
		}
	}

	if(!_arr->add('\0'))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	文字の取得
	@param		index 文字列インデックス
	@return		指定インデックスの文字
**//*---------------------------------------------------------------------*/
u16 VwString::getAt(int index) const
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
bool VwString::setAt(int index, const u16 c)
{
	return _arr->setAt(index, c);
}

/*---------------------------------------------------------------------*//**
	文字列の追加
	@param	str				VwString インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::add(const VwString* str)
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
bool VwString::add(const u16* psz)
{
	_arr->setCount(_arr->getCount() - 1);								// 現在の文字列から NULL 文字を除外

	return _arr->addArray(psz, wstrlen(psz) + 1);					// NULL 文字もコピー
}

/*---------------------------------------------------------------------*//**
	u16 配列の追加
	@param	pc				u16 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::add(const u16* pc, int length)
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
				_arr->addArray(0, 1);
			}

			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	u16 配列の追加
	@param	c				文字
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::add(const u16 c)
{
	u16 cWork[2] = { c, '\0' };
	return add(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	str				VwString インスタンスへの参照
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VwString VwString::operator+(const VwString& str) const
{
	VwString rstrReturn(*this);	// コピーの作成
	rstrReturn.add(str);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	psz				文字列
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VwString VwString::operator+(const u16* psz) const
{
	VwString rstrReturn(*this);	// コピーの作成
	rstrReturn.add(psz);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	c				文字
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VwString VwString::operator+(const u16 c) const
{
	VwString rstrReturn(*this);	// コピーの作成
	u16 cAdd[2] = {c, '\0'};
	rstrReturn.add(cAdd, 2);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	文字列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	str				VwString インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::insert(int index, const VwString* str)
{
	return insert(index, str->getRaw(), str->getLength());
}

/*---------------------------------------------------------------------*//**
	文字列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::insert(int index, const u16* psz)
{
	return insert(index, psz, wstrlen(psz));
}

/*---------------------------------------------------------------------*//**
	u16 配列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	pc				u16 配列
	@param	length			配列の長さ（NULL 文字は含めない）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::insert(int index, const u16* pc, int length)
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
bool VwString::insert(int index, const u16 c)
{
	return insert(index, &c, 1);
}

/*---------------------------------------------------------------------*//**
	文字列の削除
	@param	index			削除する配列インデックス
	@param	count			削除する要素の数
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VwString::remove(int index, int count)
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
bool VwString::equals(const u16* psz) const
{
	return equals(psz, wstrlen(psz) + 1);
}

/*---------------------------------------------------------------------*//**
	u16 配列が同一であるか判定
	@param	pc				u16 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
**//*---------------------------------------------------------------------*/
bool VwString::equals(const u16* pc, int length) const
{
	return _arr->equals(pc, length);
}

/*---------------------------------------------------------------------*//**
	同一であるか判定
	@param	c				比較文字
**//*---------------------------------------------------------------------*/
bool VwString::equals(u16 c) const
{
	u16 cWork[2] = { c, '\0' };
	return equals(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	str				文字列
**//*---------------------------------------------------------------------*/
bool VwString::equalsIgnoreCase(const WStringBase* str) const
{
	VwString trgL = VwString(str).toLower();
	VwString thisL = this->toLower();
	return thisL.equals(&trgL);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	str				文字列
**//*---------------------------------------------------------------------*/
bool VwString::equalsIgnoreCase(const VwString* str) const
{
	VwString trgL = str->toLower();
	VwString thisL = this->toLower();
	return thisL.equals(&trgL);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
bool VwString::equalsIgnoreCase(const u16* psz) const
{
	VwString strWk(psz);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、u16 配列が同一であるか判定
	@param	pc				u16 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
**//*---------------------------------------------------------------------*/
bool VwString::equalsIgnoreCase(const u16* pc, int length) const
{
	VwString strWk(pc, length);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、同一であるか判定
	@param	c				比較文字
**//*---------------------------------------------------------------------*/
bool VwString::equalsIgnoreCase(u16 c) const
{
	VwString strWk(c);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	文字の検索
	@param	c				検索文字
	@param	idxBegin		検索を開始する文字列インデックス
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VwString::find(const u16 c, int idxBegin) const
{
	if((idxBegin < 0) || ((u32)idxBegin >= (_arr->getCount() - 1)))	{	return -1;	}

	const u16* pcFind = wstrchr(getRaw(idxBegin), c);
	if(pcFind == NULL)								{	return -1;	}

	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字列の検索
	@param	psz				検索文字列
	@param	idxBegin		検索を開始する文字列インデックス
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VwString::find(const u16* psz, int idxBegin) const
{
	if((idxBegin < 0) || ((u32)idxBegin >= (_arr->getCount() - 1)))	{	return -1;	}

	const u16* pcFind = wstrstr(getRaw(idxBegin), psz);
	if(pcFind == NULL)								{	return -1;	}

	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字の逆検索
	@param	c				検索文字
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VwString::findReverse(const u16 c) const
{
	const u16* pcFind = wstrrchr(getRaw(0), c);
	if(pcFind == NULL)								{	return -1;	}

	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字列の逆検索
	@param	psz				検索文字列
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VwString::findReverse(const u16* psz) const
{
	int i, j;
	int lenSrc = wstrlen(psz);

	if(lenSrc == 0)								{	return -1;	}

	u16* pc = _arr->getPointer();
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
bool VwString::replace(const u16 cOld, const u16* pszNew, int idxBegin, int cnt)
{
	VwString strResult;
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
bool VwString::replace(const u16* pszOld, const u16* pszNew, int idxBegin, int cnt)
{
	VwString strResult;
	bool isReplace = false;
	if(idxBegin > 0)	{	strResult += this->substring(0, idxBegin);	}

	int cntReplace = 0;
	int lenOld = wstrlen(pszOld);
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
VwString VwString::substring(int idxBegin, int idxEnd) const
{
	if(idxBegin < 0)							{	return VwString();	}
	if((u32)idxEnd > (_arr->getCount() - 1))	{	return VwString();	}
	if(idxBegin > idxEnd)						{	return VwString();	}

	if((idxBegin == 0) && (idxEnd == (_arr->getCount() - 1)))	{	return *this;	}

	int size = idxEnd - idxBegin;
	VwString str(size + 1);
	str._arr->setCount(0);	// 現在の文字列から NULL 文字を除外
	str._arr->addArray(_arr->getPointer(idxBegin), size);
	str._arr->addArray(0, 1);
	return str;
}

/*---------------------------------------------------------------------*//**
	部分文字列の作成
	@param	idxBegin		開始インデックス
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VwString VwString::substring(int idxBegin) const
{
	return substring(idxBegin, _arr->getCount() - 1);
}

/*---------------------------------------------------------------------*//**
	小文字化した文字列の作成
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VwString VwString::toLower() const
{
	int len = this->getLength();
	const u16* sz = this->getRaw();
	u16* cbuf = new u16[len + 1];
	for(int i = 0; i < len; i++)	{	cbuf[i] = (u16)::tolower(sz[i]);	}
	cbuf[len] = '\0';
	VwString str(cbuf);
	delete[] cbuf;
	return str;
}

/*---------------------------------------------------------------------*//**
	大文字化した文字列の作成
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VwString VwString::toUpper() const
{
	int len = this->getLength();
	const u16* sz = this->getRaw();
	u16* cbuf = new u16[len + 1];
	for(int i = 0; i < len; i++)	{	cbuf[i] = (u16)::toupper(sz[i]);	}
	cbuf[len] = '\0';
	VwString str(cbuf);
	delete[] cbuf;
	return str;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
#endif