/***********************************************************************//**
 *  VcString.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "VcString.h"

// Friends
#include "VwString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// VcString メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	文字列を単純設定する
	※	単純コピー（キャスト）なので、文字エンコード機能は無い点に要注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::setEasily(const u16* psz)
{
	int length = VStringBase<u16, WStringBase>::calcStrLen(psz);
	if(!_arr->setArray(0L, length + 1))
	{
		return false;
	}

	for(int i = 0; i < length; i++)
	{
		if(!_arr->setAt(i, (char)psz[i]))
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
	文字列を単純設定する
	※	単純コピーであるので、文字エンコード機能は無い点に注意
		文字エンコードを行う場合は Txt クラス郡を使うこと
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::setEasily(const WStringBase* str)
{
	return setEasily(str->getRaw());
}

/*---------------------------------------------------------------------*//**
	文字列を書式化設定
	@param	printf 書式の引数
	@return	文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::format(const char* psz, ...)
{
	VcString str;

	va_list args;
	va_start(args, psz);

	int			count;
	static char	szBuffer[1024];

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		count = _vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#else
		count = vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#endif

	szBuffer[1023] = 0;	// 念のため
	str.set(szBuffer);	// 文字列を設定

	va_end(args);

	return str;
}

/*---------------------------------------------------------------------*//**
	文字列を書式化設定
	@param	printf 書式の引数
	@return	文字列のコピー
**//*---------------------------------------------------------------------*/
VcString* VcString::format(VcString* s, const char* psz, ...)
{
	va_list args;
	va_start(args, psz);

	int			count;
	static char	szBuffer[1024];

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		count = _vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#else
		count = vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#endif

	szBuffer[1023] = 0;	// 念のため
	s->set(szBuffer);	// 文字列を設定

	va_end(args);

	return s;
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
#include "VcString.h"

// Friends
#include "../collection/InsArray.h"

// External

// Library
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

#if 0
////////////////////////////////////////////////////////////////////////////
// 静的関数

static int strlen_c(const char* sz)
{
	int i = 0;
	while(sz[i] != '\0'){ i++; }
	return i;
}
#endif

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
int VcString::getLength() const
{
	return _arr->getCount() - 1;
}

/*---------------------------------------------------------------------*//**
	文字列ポインタを得る
	@return		文字列ポインタ
**//*---------------------------------------------------------------------*/
const char* VcString::getRaw(int index) const
{
	return _arr->getPointer(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
VcString::VcString()
	: _arr(new InsArray<char>(true, false))
{
	_arr->setGrow2time(true);
	set("", 1);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	str				CStringBase インスタンスへのポインタ
**//*---------------------------------------------------------------------*/
VcString::VcString(const CStringBase* str)
	: _arr(new InsArray<char>(true, false))
{
	_arr->setGrow2time(true);
	set(str->getRaw(), str->getLength() + 1);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
VcString::VcString(const char* psz)
	: _arr(new InsArray<char>(true, false))
{
	_arr->setGrow2time(true);
	set(psz);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	pc				文字配列
	@param	length			長さ
**//*---------------------------------------------------------------------*/
VcString::VcString(const char* pc, int length)
	: _arr(new InsArray<char>(true, false))
{
	_arr->setGrow2time(true);
	set(pc, length);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	c				文字
**//*---------------------------------------------------------------------*/
VcString::VcString(const char c)
	: _arr(new InsArray<char>(true, false))
{
	_arr->setGrow2time(true);
	set(c);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	@param	sizeBuf			初期バッファサイズ
**//*---------------------------------------------------------------------*/
VcString::VcString(int sizeBuf)
	: _arr(new InsArray<char>(true, false))
{
	_arr->setGrow2time(true);
	if(sizeBuf <= 0)	{	sizeBuf = 1;	}
	sizeBuf = FIT_ALIGN(sizeBuf, 4);
	_arr->alloc(sizeBuf);
	_arr->addArray("", 1);
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
	@param	str				VcString インスタンスへの参照
**//*---------------------------------------------------------------------*/
VcString::VcString(const VcString& str)
	: _arr(new InsArray<char>(*str._arr))
{
	_arr->setGrow2time(true);
	set(str.getRaw(), str.getLength() + 1);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
VcString::~VcString()
{
	delete _arr;
	_arr = 0L;
}

/*---------------------------------------------------------------------*//**
	文字列のクリア（メモリの解放を行わない簡易版）
**//*---------------------------------------------------------------------*/
void VcString::empty()
{
	set("", 1);
}

/*---------------------------------------------------------------------*//**
	文字列のクリア（メモリの解放を行わない簡易版）
**//*---------------------------------------------------------------------*/
void VcString::emptyEasy()
{
	_arr->setAt(0, '\0');
	_arr->setCount(1);
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	str				CStringBase インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::set(const CStringBase* str)
{
	return set(str->getRaw(), str->getLength() + 1);	// NULL 文字分もコピー
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::set(const char* psz)
{
	return set(psz, (int)strlen(psz) + 1);				// NULL 文字分もコピー
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
	@param	pc				char 配列
			length			配列の長さ（文字列の場合は NULL 文字も含む）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::set(const char* pc, int length)
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
bool VcString::set(const char c)
{
	char cWork[2] = { c, '\0' };
	return set(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	文字の取得
	@param		index 文字列インデックス
	@return		指定インデックスの文字
**//*---------------------------------------------------------------------*/
char VcString::getAt(int index) const
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
bool VcString::setAt(int index, const char c)
{
	return _arr->setAt(index, c);
}

/*---------------------------------------------------------------------*//**
	文字列の追加
	@param	str				VcString インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::add(const VcString* str)
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
bool VcString::add(const char* psz)
{
	_arr->setCount(_arr->getCount() - 1);								// 現在の文字列から NULL 文字を除外

	return _arr->addArray(psz, (int)strlen(psz) + 1);					// NULL 文字もコピー
}

/*---------------------------------------------------------------------*//**
	char 配列の追加
	@param	pc				char 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::add(const char* pc, int length)
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
				_arr->addArray("", 1);
			}

			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	char 配列の追加
	@param	c				文字
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::add(const char c)
{
	char cWork[2] = { c, '\0' };
	return add(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	str				VcString インスタンスへの参照
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::operator+(const VcString& str) const
{
	VcString rstrReturn(*this);	// コピーの作成
	rstrReturn.add(str);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	psz				文字列
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::operator+(const char* psz) const
{
	VcString rstrReturn(*this);	// コピーの作成
	rstrReturn.add(psz);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	追加文字列の作成
	@param	c				文字
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::operator+(const char c) const
{
	VcString rstrReturn(*this);	// コピーの作成
	char cAdd[2] = {c, '\0'};
	rstrReturn.add(cAdd, 2);
	return rstrReturn;
}

/*---------------------------------------------------------------------*//**
	文字列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	str				VcString インスタンスへのポインタ
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::insert(int index, const VcString* str)
{
	return insert(index, str->getRaw(), str->getLength());
}

/*---------------------------------------------------------------------*//**
	文字列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	psz				文字列
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::insert(int index, const char* psz)
{
	return insert(index, psz, (int)strlen(psz));
}

/*---------------------------------------------------------------------*//**
	char 配列の挿入
	@param	index			挿入する位置の文字列インデックス
	@param	pc				char 配列
	@param	length			配列の長さ（NULL 文字は含めない）
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::insert(int index, const char* pc, int length)
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
bool VcString::insert(int index, const char c)
{
	return insert(index, &c, 1);
}

/*---------------------------------------------------------------------*//**
	文字列の削除
	@param	index			削除する配列インデックス
	@param	count			削除する要素の数
	@return	true == 成功 / false == 失敗
**//*---------------------------------------------------------------------*/
bool VcString::remove(int index, int count)
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
bool VcString::equals(const char* psz) const
{
	return equals(psz, (int)strlen(psz) + 1);
}

/*---------------------------------------------------------------------*//**
	char 配列が同一であるか判定
	@param	pc				char 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
**//*---------------------------------------------------------------------*/
bool VcString::equals(const char* pc, int length) const
{
	return _arr->equals(pc, length);
}

/*---------------------------------------------------------------------*//**
	同一であるか判定
	@param	c				比較文字
**//*---------------------------------------------------------------------*/
bool VcString::equals(char c) const
{
	char cWork[2] = { c, '\0' };
	return equals(cWork, 2);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	str				文字列
**//*---------------------------------------------------------------------*/
bool VcString::equalsIgnoreCase(const CStringBase* str) const
{
	VcString trgL = VcString(str).toLower();
	VcString thisL = this->toLower();
	return thisL.equals(&trgL);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	str				文字列
**//*---------------------------------------------------------------------*/
bool VcString::equalsIgnoreCase(const VcString* str) const
{
	VcString trgL = str->toLower();
	VcString thisL = this->toLower();
	return thisL.equals(&trgL);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、文字列が同一であるか判定
	@param	psz				文字列
**//*---------------------------------------------------------------------*/
bool VcString::equalsIgnoreCase(const char* psz) const
{
	VcString strWk(psz);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、char 配列が同一であるか判定
	@param	pc				char 配列
	@param	length			配列の長さ（文字列の場合は NULL 文字も含む）
**//*---------------------------------------------------------------------*/
bool VcString::equalsIgnoreCase(const char* pc, int length) const
{
	VcString strWk(pc, length);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して、同一であるか判定
	@param	c				比較文字
**//*---------------------------------------------------------------------*/
bool VcString::equalsIgnoreCase(char c) const
{
	VcString strWk(c);
	return equalsIgnoreCase(&strWk);
}

/*---------------------------------------------------------------------*//**
	文字の検索
	@param	c				検索文字
	@param	idxBegin		検索を開始する文字列インデックス
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VcString::find(const char c, int idxBegin) const
{
	if((idxBegin < 0) || ((u32)idxBegin >= (_arr->getCount() - 1)))	{	return -1;	}

	const char* pcFind = ::strchr(getRaw(idxBegin), c);
	if(pcFind == NULL)								{	return -1;	}

	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字列の検索
	@param	psz				検索文字列
	@param	idxBegin		検索を開始する文字列インデックス
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VcString::find(const char* psz, int idxBegin) const
{
	if((idxBegin < 0) || ((u32)idxBegin >= (_arr->getCount() - 1)))	{	return -1;	}

	const char* pcFind = ::strstr(getRaw(idxBegin), psz);
	if(pcFind == NULL)								{	return -1;	}

	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字の逆検索
	@param	c				検索文字
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VcString::findReverse(const char c) const
{
	const char* pcFind = ::strrchr(getRaw(0), c);
	if(pcFind == NULL)								{	return -1;	}

	return (int)(pcFind - getRaw(0));
}

/*---------------------------------------------------------------------*//**
	文字列の逆検索
	@param	psz				検索文字列
	@return	文字列インデックス（見つからなかった場合は -1 を返す）
**//*---------------------------------------------------------------------*/
int VcString::findReverse(const char* psz) const
{
	int i, j;
	int lenSrc = (int)strlen(psz);

	if(lenSrc == 0)								{	return -1;	}

	char* pc = _arr->getPointer();
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
bool VcString::replace(const char cOld, const char* pszNew, int idxBegin, int cnt)
{
	VcString strResult;
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
bool VcString::replace(const char* pszOld, const char* pszNew, int idxBegin, int cnt)
{
	VcString strResult;
	bool isReplace = false;
	if(idxBegin > 0)	{	strResult += this->substring(0, idxBegin);	}

	int cntReplace = 0;
	int lenOld = (int)strlen(pszOld);
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
VcString VcString::substring(int idxBegin, int idxEnd) const
{
	if(idxBegin < 0)							{	return VcString();	}
	if((u32)idxEnd > (_arr->getCount() - 1))	{	return VcString();	}
	if(idxBegin > idxEnd)						{	return VcString();	}

	if((idxBegin == 0) && (idxEnd == (_arr->getCount() - 1)))	{	return *this;	}

	int size = idxEnd - idxBegin;
	VcString str(size + 1);
	str._arr->setCount(0);	// 現在の文字列から NULL 文字を除外
	str._arr->addArray(_arr->getPointer(idxBegin), size);
	str._arr->addArray("", 1);
	return str;
}

/*---------------------------------------------------------------------*//**
	部分文字列の作成
	@param	idxBegin		開始インデックス
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::substring(int idxBegin) const
{
	return substring(idxBegin, _arr->getCount() - 1);
}

/*---------------------------------------------------------------------*//**
	小文字化した文字列の作成
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::toLower() const
{
	int len = this->getLength();
	const char* sz = this->getRaw();
	char* cbuf = new char[len + 1];
	for(int i = 0; i < len; i++)	{	cbuf[i] = (char)::tolower(sz[i]);	}
	cbuf[len] = '\0';
	VcString str(cbuf);
	delete[] cbuf;
	return str;
}

/*---------------------------------------------------------------------*//**
	大文字化した文字列の作成
	@return	作成した文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::toUpper() const
{
	int len = this->getLength();
	const char* sz = this->getRaw();
	char* cbuf = new char[len + 1];
	for(int i = 0; i < len; i++)	{	cbuf[i] = (char)::toupper(sz[i]);	}
	cbuf[len] = '\0';
	VcString str(cbuf);
	delete[] cbuf;
	return str;
}

/*---------------------------------------------------------------------*//**
	文字列を書式化設定
	@param	printf 書式の引数
	@return	文字列のコピー
**//*---------------------------------------------------------------------*/
VcString VcString::format(const char* psz, ...)
{
	VcString str;

	va_list args;
	va_start(args, psz);

	int			count;
	static char	szBuffer[1024];

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		count = _vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#else
		count = vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#endif

	szBuffer[1023] = 0;	// 念のため
	str.set(szBuffer);	// 文字列を設定

	va_end(args);

	return str;
}

/*---------------------------------------------------------------------*//**
	文字列を書式化設定
	@param	printf 書式の引数
	@return	文字列のコピー
**//*---------------------------------------------------------------------*/
VcString* VcString::format(VcString* s, const char* psz, ...)
{
	va_list args;
	va_start(args, psz);

	int			count;
	static char	szBuffer[1024];

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		count = _vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#else
		count = vsnprintf(szBuffer, sizeof(szBuffer), psz, args);
	#endif

	szBuffer[1023] = 0;	// 念のため
	s->set(szBuffer);	// 文字列を設定

	va_end(args);

	return s;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
#endif