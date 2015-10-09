/***********************************************************************//**
 *  ArrayBase.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

// delete 対応ポインタ配列クラスの場合は
// #define TFW_PTR_ARRAY
// インスタンス配列クラスの場合は
// #define TFW_INS_ARRAY

#if !defined(FIT_ALIGN)
	#define FIT_ALIGN(n_, a_)	( ((n_) + ((a_) - 1)) & ~((a_) - 1) )
#endif

// 配列容量増加値
#define SIZE_GROW		16	// 配列容量不足時の自動増加要素数
#define SIZE_ALIGN		4	// 配列容量増加数のアライン

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	配列基底クラス
 *		格納できる数は 0x0(0) ～ 0x1fffffff(536870911).
 *
**//*---------------------------------------------------------------------*/
template<class TYPE>
class TFW_ARRAY_CLASS_NAME
{
	//======================================================================
	// 定数
private:
	static const u32	F1_AUTO_GROW		= 0x1;	// 自動配列拡張
#if defined(TFW_PTR_ARRAY)
	static const u32	F1_AUTO_OBJ_DELETE	= 0x2;	// 自動インスタンス削除
#elif defined(TFW_INS_ARRAY)
	static const u32	F1_AUTO_ZERO_MEMORY	= 0x2;	// 自動ゼロ初期化
#endif
	static const u32	F2_GROW_2TIMES		= 0x1;	// 配列拡張時に容量２倍
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 配列の要素数を得る
	inline u32 getCount() const		{	return _count;	}
	u32 getAllocSize() const;														// 配列アロケート数を得る

	// 配列の要素数を設定する
	bool setCount(u32 count);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 配列確保・解放
	bool alloc(u32 sizeAlloc);														// 配列の明示的作成
	bool realloc(u32 sizeAlloc);													// 配列拡張
	bool release();																	// 配列の明示的解放
	void setGrow2time(bool is);
	
	// ポインタ取得
	TYPE* getPointer(u32 index) const;
	inline TYPE* getPointer() const { return getPointer(0); }

	// 単要素アクセス
	TYPE getAt(u32 index) const;													// 配列の要素を得る
	bool setAt(u32 index, TYPE el);													// 配列の要素を設定する
	bool add(TYPE el);																// 要素を最後に追加する

	// 配列を設定する
	bool setArray(const TYPE* pelArray, u32 count);
	bool setArray(const TFW_ARRAY_CLASS_NAME* rar);

	// 配列を追加する
	bool addArray(const TYPE* pelArray, u32 count);
	bool addArray(const TFW_ARRAY_CLASS_NAME* rar);

	// 配列の挿入
	bool insert(u32 index, const TYPE* el);
	bool insert(u32 index, const TYPE& el);
	bool insert(u32 index, const TYPE* pelArray, u32 count);
	bool insert(u32 index, const TFW_ARRAY_CLASS_NAME* rar);

	// 配列の削除
	bool remove(u32 index, u32 count);
	inline bool remove(u32 index) { return remove(index, 1); }
	bool removeAll(bool isEasy = true);

	// 配列の比較
	bool equals(const TYPE* pelArray, u32 count) const;
	bool equals(const TFW_ARRAY_CLASS_NAME* rar) const;

	// 演算子
	inline TYPE& operator[](u32 index) const { return *getPointer(index); }
	inline TFW_ARRAY_CLASS_NAME& operator=(const TFW_ARRAY_CLASS_NAME& rar) { setArray(&rar); return *this; }
	inline TFW_ARRAY_CLASS_NAME& operator+(const TFW_ARRAY_CLASS_NAME& rar) { addArray(&rar); return *this; }
	inline void operator+=(const TFW_ARRAY_CLASS_NAME& rar) { addArray(&rar); }
	inline bool operator==(const TFW_ARRAY_CLASS_NAME& rar) const { return equals(&rar); }
	inline bool operator!=(const TFW_ARRAY_CLASS_NAME& rar) const { return !equals(&rar); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	// コンストラクション
	TFW_ARRAY_CLASS_NAME();
#if defined(TFW_PTR_ARRAY)
	explicit TFW_ARRAY_CLASS_NAME(bool isAutoGrow, bool isAutoObjDelete);
#elif defined(TFW_INS_ARRAY)
	explicit TFW_ARRAY_CLASS_NAME(bool isAutoGrow, bool isAutoZeroMemory);
#endif
	explicit TFW_ARRAY_CLASS_NAME(const TFW_ARRAY_CLASS_NAME& rar);
	~TFW_ARRAY_CLASS_NAME();
	void setAutoGrow(bool isAuto);
#if defined(TFW_PTR_ARRAY)
	void setAutoObjectDelete(bool isAuto);
#elif defined(TFW_INS_ARRAY)
	void setAutoZeroMemory(bool isAuto);
#endif

private:
	void init(bool isAutoZeroMemory, bool isAutoParam2);

	//======================================================================
	// 変数
protected:
	TYPE* _pelArray;		// 配列ポインタ
	u32 _count : 30;		// 配列要素数
	u32 _flags1 : 2;		// フラグ１
	u32 _sizeAlloc : 30;	// アロケート数
	u32 _flags2 : 2;		// フラグ２
};

//==========================================================================
// TFW_ARRAY_CLASS_NAME メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	配列の要素数を設定する

	@param	count 新しい配列要素数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::setCount(u32 count)
{
	if(_sizeAlloc < count)
	{
		return false;
	}
	_count = count;
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	配列作成

	@param	size 配列アロケート数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::alloc(u32 sizeAlloc)
{
	release();	// 全削除

	if(sizeAlloc == 0)
	{
		return true;	// 何もしないで正常終了
	}

	// 配列を確保
	_pelArray = new TYPE[sizeAlloc];
	if(_pelArray == 0L)
	{
		return false;
	}
	_sizeAlloc = sizeAlloc;

	// ゼロ初期化
#if defined(TFW_INS_ARRAY)
	if(TFW_IS_FLAG(_flags1, F1_AUTO_ZERO_MEMORY))
#endif
	{
		::memset(_pelArray, 0, sizeof(TYPE) * _sizeAlloc);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	配列拡張

	@param	size 新しい配列アロケート数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::realloc(u32 sizeAlloc)
{
	if(sizeAlloc <= _sizeAlloc)
	{
		return true;	// 何もしないで正常終了
	}

	// 新しい配列を確保
	TYPE* pelNewArray = new TYPE[sizeAlloc];
	if(pelNewArray == 0L)
	{
		return false;	// 確保できなかった
	}

	// 既存の配列があるかどうかで処理分岐
	if(_pelArray != 0L)
	{
		// 新しい配列へコピー
		for(u32 i = 0; i < _count; i++)
		{
			pelNewArray[i] = _pelArray[i];
		}

		// ゼロ初期化
#if defined(TFW_INS_ARRAY)
		if(TFW_IS_FLAG(_flags1, F1_AUTO_ZERO_MEMORY))
#endif
		{
			if(_count < sizeAlloc)
			{
				::memset(&pelNewArray[_count], 0, sizeof(TYPE) * (sizeAlloc - _count));
			}
		}

		// 古い配列を削除
		delete[] _pelArray;
	}
	else
	{
		// ゼロ初期化
#if defined(TFW_INS_ARRAY)
		if(TFW_IS_FLAG(_flags1, F1_AUTO_ZERO_MEMORY))
#endif
		{
			::memset(pelNewArray, 0, sizeof(TYPE) * sizeAlloc);
		}
	}

	// 新しい配列にポインタを付け替え
	_pelArray	= pelNewArray;
	_sizeAlloc	= sizeAlloc;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列解放

	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::release()
{
	if(_pelArray == 0L)
	{
		return true;	// 不要である
	}

	#if defined(TFW_PTR_ARRAY)
		if(TFW_IS_FLAG(_flags1, F1_AUTO_OBJ_DELETE))
		{
			// ポインタ参照先のインスタンスを削除
			for(u32 i = 0; i < _count; i++)
			{
				delete _pelArray[i];
			}
		}
	#endif

	delete[]	_pelArray;
	_pelArray	= 0L;
	_count		= 0;
	_sizeAlloc	= 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列サイズ拡張を２倍ペースで行う																		  

	@param	is 設定するか否か
**//*---------------------------------------------------------------------*/
template<class TYPE>
void TFW_ARRAY_CLASS_NAME<TYPE>::setGrow2time(bool is)
{
	u32 flags = _flags2;
	TFW_SET_FLAG(flags, F2_GROW_2TIMES, is);
	_flags2 = flags;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

/*---------------------------------------------------------------------*//**
	配列アロケート数を得る

	@return	配列アロケート数（配列の長さ）
**//*---------------------------------------------------------------------*/
template<class TYPE>
u32	TFW_ARRAY_CLASS_NAME<TYPE>::getAllocSize() const
{
	return _sizeAlloc;
}

/*---------------------------------------------------------------------*//**
	ポインタ取得

	@param	index 取得する配列インデックス
	@return	ポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
TYPE* TFW_ARRAY_CLASS_NAME<TYPE>::getPointer(u32 index) const
{
	if(index >= _sizeAlloc)
	{
		ASSERT(false);
		return 0L;
	}

	return &_pelArray[index];
}

/*---------------------------------------------------------------------*//**
	配列の要素を得る

	@param	index 取得する配列インデックス
	@return	要素
**//*---------------------------------------------------------------------*/
template<class TYPE>
TYPE TFW_ARRAY_CLASS_NAME<TYPE>::getAt(u32 index) const
{
	ASSERT((getPointer(index) != 0L));
	return *getPointer(index);
}

/*---------------------------------------------------------------------*//**
	配列の要素を設定する

	@param	index 設定する配列インデックス
	@param	el 設定する要素
	@param	isAutoGrow 自動的に配列を拡張するかどうか
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::setAt(u32 index, TYPE el)
{
	// 配列確保数の不足をチェック
	if(index >= _sizeAlloc)
	{
		if(TFW_IS_FLAG(_flags1, F1_AUTO_GROW))
		{
			// 配列確保数を増やす
			u32 sizeNewAlloc;
			if(TFW_IS_FLAG(_flags2, F2_GROW_2TIMES))
			{
				sizeNewAlloc = FIT_ALIGN(_sizeAlloc * 2, SIZE_ALIGN);
			}
			else
			{
				sizeNewAlloc = FIT_ALIGN(_sizeAlloc + SIZE_GROW, SIZE_ALIGN);
			}
			if(!realloc(sizeNewAlloc))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	// 伸長チェック
	ASSERT(index < _sizeAlloc);
	if(index >= _count)
	{
		if(!setCount(index + 1))
		{
			return false;
		}
	}

	// 値をコピー
	_pelArray[index] = el;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列の要素を最後尾に追加する

	@param	el 追加する要素
	@param	isAutoGrow 自動的に配列を拡張するかどうか
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::add(TYPE el)
{
	return setAt(_count, el);
}

/*---------------------------------------------------------------------*//**
	配列をコピーする

	@param	pelArray コピー元の配列
	@param	count 配列要素数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::setArray(const TYPE* pelArray, u32 count)
{
	ASSERT((this->_pelArray == 0L) || (this->_pelArray != pelArray));

	release();	// 現在の配列を破棄

	u32 sizeAlloc = FIT_ALIGN(count, SIZE_ALIGN);

	if(!alloc(sizeAlloc))
	{
		return false;
	}

	// 値をコピー
	if(pelArray != 0L)
	{
		for(u32 i = 0; i < count; i++)
		{
			_pelArray[i] = pelArray[i];
		}
	}

	// 配列要素数を更新	
	_count = count;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列をコピーする

	@param	rar 設定する配列のポインタ
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::setArray(const TFW_ARRAY_CLASS_NAME* rar)
{
	return setArray(rar->_pelArray, rar->_count);
}

/*---------------------------------------------------------------------*//**
	配列を追加する

	@param	pelArray 追加する配列
	@param	count 追加する配列要素数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::addArray(const TYPE* pelArray, u32 count)
{
	if(pelArray == 0L)
	{
		return false;
	}

	u32 cntTotalArray	= _count + count;						// 新しい長さ
	u32 sizeTotalAlloc	= FIT_ALIGN(cntTotalArray, SIZE_ALIGN);	// 新しい配列数

	// 配列拡張
	if(sizeTotalAlloc > _sizeAlloc)
	{
		if(!realloc(sizeTotalAlloc))
		{
			return false;
		}
	}

	// 値をコピー
	ASSERT(_sizeAlloc >= (_count + count));
	for(u32 i = 0; i < count; i++)
	{
		_pelArray[_count + i] = pelArray[i];
	}

	// 長さ更新
	_count = cntTotalArray;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列を追加する

	@param	rar 追加する配列のポインタ
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::addArray(const TFW_ARRAY_CLASS_NAME* rar)
{
	return addArray(rar->_pelArray, rar->_count);
}

/*---------------------------------------------------------------------*//**
	配列に要素を挿入する

	@param	index 挿入する配列インデックス
	@param	el 追加する要素
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::insert(u32 index, const TYPE* el)
{
	return insert(index, el, 1);
}
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::insert(u32 index, const TYPE& el)
{
	return insert(index, &el, 1);
}

/*---------------------------------------------------------------------*//**
	配列に要素を挿入する

	@param	index 挿入する配列インデックス
	@param	pelArray 追加する要素配列
	@param	count 配列要素数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::insert(u32 index, const TYPE* pelArray, u32 count)
{
	if(count == 0)
	{
		return true;
	}

	// 挿入の可能性チェック
	if(index >= _count)
	{
		return addArray(pelArray, count);
	}

	// 配列確保数の不足をチェック
	if((_count + count) >= _sizeAlloc)
	{
		// 配列確保数を増やす
		u32 sizeAllocNew = FIT_ALIGN(_sizeAlloc + count, SIZE_ALIGN);
		if(TFW_IS_FLAG(_flags2, F2_GROW_2TIMES))
		{
			sizeAllocNew = TFW_MAX(FIT_ALIGN(_sizeAlloc * 2, SIZE_ALIGN), sizeAllocNew);
		}
		if(!realloc(sizeAllocNew))
		{
			return false;
		}
	}

	ASSERT(_sizeAlloc >= (_count + count));

	u32 i;

	// 要素を後ろにコピーして挿入個数分空ける
	i = _count + count - 1;
	while(i >= (index + count))
	{
		_pelArray[i] = _pelArray[i - count];
		i--;
	}

	// 値をコピー
	for(i = 0; i < count; i++)
	{
		_pelArray[index + i] = pelArray[i];
	}

	// 長さ増加
	_count += count;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列に要素を挿入する

	@param	index 挿入する配列インデックス
	@param	rar 追加する配列のポインタ
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::insert(u32 index, const TFW_ARRAY_CLASS_NAME* rar)
{
	return insert(index, rar->_pelArray, rar->_count);
}

/*---------------------------------------------------------------------*//**
	配列の局所削除

	@param	index 削除する配列インデックス
	@param 	count 削除する要素の数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::remove(u32 index, u32 count)
{
	u32 i;

	// 削除する要素数の適正化
	if((count > _count) || ((index + count) > _count))
	{
		count = _count - index;
	}

	// 要素をクリア
	for(i = 0; i < count; i++)
	{
		if((index + i) >= _count)
		{
			break;
		}

		// インスタンス削除
		#if defined(TFW_PTR_ARRAY)
			if(TFW_IS_FLAG(_flags1, F1_AUTO_OBJ_DELETE))
			{
				delete _pelArray[index + i];
			}
		#endif

		// ゼロ初期化
#if defined(TFW_INS_ARRAY)
		if(TFW_IS_FLAG(_flags1, F1_AUTO_ZERO_MEMORY))
#endif
		{
			::memset(&_pelArray[index + i], 0, sizeof(TYPE));
		}
	}

	// 要素を前方に削除個数分コピー
	for(i = index; i < _count; i++)
	{
		if((i + count) >= _count)
		{
			break;
		}

		_pelArray[i] = _pelArray[i + count];
	}

	// 長さ減少
	_count -= count;

	return true;
}

/*---------------------------------------------------------------------*//**
	配列全削除

	@param	isEasy 簡易削除かどうか（簡易削除は配列の解放をしない）
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::removeAll(bool isEasy)
{
	if(isEasy)
	{
		// インスタンスを削除
		#if defined(TFW_PTR_ARRAY)
			if(TFW_IS_FLAG(_flags1, F1_AUTO_OBJ_DELETE))
			{
				for(u32 i = 0; i < _count; i++)
				{
					delete _pelArray[i];
				}
			}
		#endif

		// ゼロ初期化
#if defined(TFW_INS_ARRAY)
		if(TFW_IS_FLAG(_flags1, F1_AUTO_ZERO_MEMORY))
#endif
		{
			::memset(_pelArray, 0, sizeof(TYPE) * _count);
		}
	
		// 長さをゼロに
		_count = 0;
	}
	else
	{
		// 配列を作り直す
		release();	// 現在の配列を破棄
		if(!alloc(0))	{	return false;	}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	配列を比較する

	@param	pbtArray 比較する配列
	@param	count 比較する配列要素数
	@return	true == 一致 / false == 不一致
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::equals(const TYPE* pelArray, u32 count) const
{
	if(_count != count)
	{
		return false;
	}

	for(u32 i = 0; i < _count; i++)
	{
		#if defined(TFW_PTR_ARRAY)
			// インスタンスで比較
			if(*_pelArray[i] != *pelArray[i])
			{
				return false;
			}
		#elif defined(TFW_INS_ARRAY)
			// 値で比較
			if(_pelArray[i] != pelArray[i])
			{
				return false;
			}
		#endif
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	配列を比較する

	@param	rar 比較する配列のポインタ
	@return	true == 一致 / false == 不一致
**//*---------------------------------------------------------------------*/
template<class TYPE>
bool TFW_ARRAY_CLASS_NAME<TYPE>::equals(const TFW_ARRAY_CLASS_NAME* rar) const
{
	return equals(rar->_pelArray, rar->_count);
}
	
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_ARRAY_CLASS_NAME<TYPE>::TFW_ARRAY_CLASS_NAME()
{
	init(false, false);
}

#if defined(TFW_PTR_ARRAY)
/*---------------------------------------------------------------------*//**
	コンストラクタ
	
	@param	isAutoGrow 自動で配列拡張をするかどうか
	@param	isAutoObjDelete	自動インスタンス削除をするかどうか
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_ARRAY_CLASS_NAME<TYPE>::TFW_ARRAY_CLASS_NAME(bool isAutoGrow, bool isAutoObjDelete)
{
	init(isAutoGrow, isAutoObjDelete);
}
#endif

#if defined(TFW_INS_ARRAY)
/*---------------------------------------------------------------------*//**
	コンストラクタ
	
	@param	isAutoGrow 自動で配列拡張をするかどうか
	@param	isAutoZeroMemory 自動でゼロ初期化をするかどうか
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_ARRAY_CLASS_NAME<TYPE>::TFW_ARRAY_CLASS_NAME(bool isAutoGrow, bool isAutoZeroMemory)
{
	init(isAutoGrow, isAutoZeroMemory);
}
#endif

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ

	@param	rar コピーする配列クラスの参照
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_ARRAY_CLASS_NAME<TYPE>::TFW_ARRAY_CLASS_NAME(const TFW_ARRAY_CLASS_NAME& rar)
{
	init(false, false);
	_flags1 = rar._flags1;
	setArray(&rar);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_ARRAY_CLASS_NAME<TYPE>::~TFW_ARRAY_CLASS_NAME()
{
	release();
}

/*-----------------------------------------------------------------*//**
	自動配列拡張をするかどうかの設定

	@param	isAuto 自動配列拡張をするかどうか
**//*-----------------------------------------------------------------*/
template<class TYPE>
void TFW_ARRAY_CLASS_NAME<TYPE>::setAutoGrow(bool isAuto)
{
	u32 flags = _flags1;
	TFW_SET_FLAG(flags, F1_AUTO_GROW, isAuto);
	_flags1 = flags;
}

#if defined(TFW_PTR_ARRAY)
/*-----------------------------------------------------------------*//**
	自動インスタンス削除をするかどうかの設定

	@param	isAuto 自動インスタンス削除をするかどうか
**//*-----------------------------------------------------------------*/
template<class TYPE>
void TFW_ARRAY_CLASS_NAME<TYPE>::setAutoObjectDelete(bool isAuto)
{
	u32 flags = _flags1;
	TFW_SET_FLAG(flags, F1_AUTO_OBJ_DELETE, isAuto);
	_flags1 = flags;
}
#endif

#if defined(TFW_INS_ARRAY)
/*-----------------------------------------------------------------*//**
	自動ゼロ初期化をするかどうかの設定

	@param	isAuto 自動ゼロ初期化をするかどうか
**//*-----------------------------------------------------------------*/
template<class TYPE>
void TFW_ARRAY_CLASS_NAME<TYPE>::setAutoZeroMemory(bool isAuto)
{
	u32 flags = _flags1;
	TFW_SET_FLAG(flags, F1_AUTO_ZERO_MEMORY, isAuto);
	_flags1 = flags;
}
#endif

/*-----------------------------------------------------------------*//**
	初期化

	@param	isAutoGrow 自動配列拡張をするかどうか
	@param	isAutoParam2 自動フラグ２（クラスによって違う）
**//*-----------------------------------------------------------------*/
template<class TYPE>
void TFW_ARRAY_CLASS_NAME<TYPE>::init(bool isAutoGrow, bool isAutoParam2)
{
	_pelArray = 0L;
	_count = 0;
	_flags1 = 0;
	_sizeAlloc = 0;
	_flags2 = 0;

	setAutoGrow(isAutoGrow);
	#if defined(TFW_PTR_ARRAY)
		setAutoObjectDelete(isAutoParam2);
	#elif defined(TFW_INS_ARRAY)
		setAutoZeroMemory(isAutoParam2);
	#endif
}

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#undef TFW_ARRAY_CLASS_NAME
