/***********************************************************************//**
 *	EasyArray.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/12/06.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_TYPE_ARRAY_H_
#define _TFW_LIB_TYPE_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Self

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定長の簡易配列 クラス
 *
**//*---------------------------------------------------------------------*/
template<class TYPE>
class EasyArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字列取得
	inline TYPE* get(int index) const { return _arr[index]; }
	// 文字列数を得る
	inline int getCount() const { return _num; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EasyArray();
	EasyArray(TYPE** arrEntr, int num);
	explicit EasyArray(const TYPE* elm1);
	EasyArray(const TYPE* elm1, const TYPE* elm2);
	EasyArray(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3);
	EasyArray(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3, const TYPE* elm4);
	EasyArray(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3, const TYPE* elm4, const TYPE* elm5);
	EasyArray(const EasyArray& src);
	~EasyArray();
	EasyArray& operator=(const EasyArray& src);
	void copy(const EasyArray& src);
	void clear();
	void set(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3, const TYPE* elm4, const TYPE* elm5, const TYPE* elm6, const TYPE* elm7, const TYPE* elm8, const TYPE* elm9, const TYPE* elm10);
	void set(TYPE** elmarr, int num);

	//======================================================================
	// メンバ変数
private:
	TYPE**	_arr;
	int		_num;
};

//==========================================================================
// EasyArray メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
EasyArray<TYPE>::EasyArray() :
	_arr(0L),
	_num(0)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
		効率のため、外部のポインタをそのまま保持するので、
		外部でインスタンスを削除しないように
**//*---------------------------------------------------------------------*/
template<class TYPE>
EasyArray<TYPE>::EasyArray(TYPE** arrEntr, int num) :
	_arr(arrEntr),
	_num(num)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
		少ない数の配列定義用
		存在しないインスタンスには NULL(0L) を指定する
**//*---------------------------------------------------------------------*/
template<class TYPE> EasyArray<TYPE>::EasyArray(const TYPE* elm1) : _arr(0L), _num(0)	{	set(elm1, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);	}
template<class TYPE> EasyArray<TYPE>::EasyArray(const TYPE* elm1, const TYPE* elm2) : _arr(0L), _num(0)	{	set(elm1, elm2, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);	}
template<class TYPE> EasyArray<TYPE>::EasyArray(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3) : _arr(0L), _num(0)	{	set(elm1, elm2, elm3, 0L, 0L, 0L, 0L, 0L, 0L, 0L);	}
template<class TYPE> EasyArray<TYPE>::EasyArray(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3, const TYPE* elm4) : _arr(0L), _num(0)	{	set(elm1, elm2, elm3, elm4, 0L, 0L, 0L, 0L, 0L, 0L);	}
template<class TYPE> EasyArray<TYPE>::EasyArray(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3, const TYPE* elm4, const TYPE* elm5) : _arr(0L), _num(0)	{	set(elm1, elm2, elm3, elm4, elm5, 0L, 0L, 0L, 0L, 0L);	}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
EasyArray<TYPE>::EasyArray(const EasyArray& src) :
	_arr(0L),
	_num(0)
{
	copy(src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
EasyArray<TYPE>::~EasyArray()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	コピー演算子
**//*---------------------------------------------------------------------*/
template<class TYPE>
EasyArray<TYPE>& EasyArray<TYPE>::operator=(const EasyArray& src)
{
	copy(src);
	return *this;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
template<class TYPE>
void EasyArray<TYPE>::copy(const EasyArray& src)
{
	clear();
	_num = src._num;
	_arr = new TYPE*[_num];
	for(int i = 0; i < _num; i++)
	{
		_arr[i] = new TYPE(src._arr[i]);
	}
}

/*---------------------------------------------------------------------*//**
	クリア（内容を破棄）
**//*---------------------------------------------------------------------*/
template<class TYPE>
void EasyArray<TYPE>::clear()
{
	for(int i = 0; i < _num; i++)
	{
		delete _arr[i];
	}
	delete[] _arr;
	_arr = 0L;
	_num = 0;
}

/*---------------------------------------------------------------------*//**
	少ない数の配列定義
**//*---------------------------------------------------------------------*/
template<class TYPE>
void EasyArray<TYPE>::set(const TYPE* elm1, const TYPE* elm2, const TYPE* elm3, const TYPE* elm4, const TYPE* elm5, const TYPE* elm6, const TYPE* elm7, const TYPE* elm8, const TYPE* elm9, const TYPE* elm10)
{
	clear();

	const TYPE* stmp[] = { elm1, elm2, elm3, elm4, elm5, elm6, elm7, elm8, elm9, elm10 };

	int cnt = 0;
	for(int i = 0; i < sizeof(stmp) / sizeof(TYPE*); i++)
	{
		if(stmp[i] != 0L)	{	cnt++;	}
	}
	if(cnt > 0)
	{
		_num = cnt;
		_arr = new TYPE*[_num];
		cnt = 0;
		for(int i = 0; i < sizeof(stmp) / sizeof(TYPE*); i++)
		{
			if(stmp[i] != 0L)	{	_arr[cnt++] = new TYPE(stmp[i]);	}
		}
	}
}

/*---------------------------------------------------------------------*//**
	配列定義
		効率のため、外部のポインタをそのまま保持するので、
		外部でインスタンスを削除しないように
***//*---------------------------------------------------------------------*/
template<class TYPE>
void EasyArray<TYPE>::set(TYPE** elmarr, int num)
{
	clear();

	_arr = elmarr;
	_num = num;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_LIB_TYPE_ARRAY_H_
