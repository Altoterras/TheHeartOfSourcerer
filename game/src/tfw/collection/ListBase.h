/*---------------------------------------------------------------------*//**
 *  ListBase.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//*---------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

// delete 対応ポインタ リンクド リスト クラスの場合は
// #define TFW_PTR_LIST
// インスタンス リンクド リスト クラスの場合は
// #define TFW_INS_LIST

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
	
template<class TYPE> class TFW_LISTIT_CLASS_NAME;
template<class TYPE> class TFW_LISTNODE_CLASS_NAME;

/*---------------------------------------------------------------------*//**
 *	リスト
 *		格納できる数は 0x0(0) ～ 0x3fffffff(1073741823).
 *
**//*---------------------------------------------------------------------*/
template<class TYPE> class TFW_LIST_CLASS_NAME
{
	//======================================================================
	// 定数
private:
#if defined(TFW_PTR_LIST)
	static const u32	F_AUTO_OBJ_DELETE	= 0x1;	// 自動インスタンス削除
#endif

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ノード数取得
	inline u32 getCount() const	{ return _count; }

	// ノード取得
	TFW_LISTNODE_CLASS_NAME<TYPE>* node(u32 index) const;
	// 先頭ノード取得
	inline TFW_LISTNODE_CLASS_NAME<TYPE>* getHeadNode() const { return _lnodeHead; }
	// 末尾ノード取得
	inline TFW_LISTNODE_CLASS_NAME<TYPE>* getTailNode() const { return _lnodeTail; }

	// オブジェクト取得
	const TFW_LIST_REF_TYPE getObject(u32 index) const;
	TFW_LIST_REF_TYPE object(u32 index) const;
	// 先頭オブジェクト取得
	inline const TFW_LIST_REF_TYPE getHeadObject() const { return (_lnodeHead != 0L) ? _lnodeHead->getObject() : 0L; }
	inline TFW_LIST_REF_TYPE headObject() const { return (_lnodeHead != 0L) ? _lnodeHead->object() : 0L; }
	// 末尾オブジェクト取得
	inline const TFW_LIST_REF_TYPE getTailObject() const { return (_lnodeTail != 0L) ? _lnodeTail->getObject() : 0L; }
	inline TFW_LIST_REF_TYPE tailObject() const { return (_lnodeTail != 0L) ? _lnodeTail->object() : 0L; }

	// イテレータを得る
	inline TFW_LISTIT_CLASS_NAME<TYPE> iterator() const { return TFW_LISTIT_CLASS_NAME<TYPE>(_lnodeHead); }
	// 末尾イテレータを得る
	inline TFW_LISTIT_CLASS_NAME<TYPE> iteratorEnd() const { return TFW_LISTIT_CLASS_NAME<TYPE>(_lnodeTail); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// インデックス取得
	u32	getIndex(const TFW_LISTNODE_CLASS_NAME<TYPE>* node) const;
	u32 getIndex(const TFW_LIST_REF_TYPE obj) const;

	// 追加・挿入・削除
	TFW_LISTNODE_CLASS_NAME<TYPE>* addTail(TFW_LIST_SET_TYPE obj);
	TFW_LISTNODE_CLASS_NAME<TYPE>* addHead(TFW_LIST_SET_TYPE obj);
	TFW_LISTNODE_CLASS_NAME<TYPE>* addTail();
	TFW_LISTNODE_CLASS_NAME<TYPE>* addHead();
	TFW_LISTNODE_CLASS_NAME<TYPE>* insertAfter(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos);
	TFW_LISTNODE_CLASS_NAME<TYPE>* insertBefore(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos);
	bool insertAfter(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos, TFW_LIST_SET_TYPE obj);
	bool insertBefore(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos, TFW_LIST_SET_TYPE obj);
	void removeNode(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeTrg);
	bool remove(const TFW_LIST_REF_TYPE obj);
	void removeAll();

	// その他サービス
	TFW_LISTNODE_CLASS_NAME<TYPE>* findNode(const TFW_LIST_REF_TYPE obj) const;
	bool fromArray(TYPE* arr, u32 length);
	TYPE* toArray() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	// コンストラクション
	TFW_LIST_CLASS_NAME();
#if defined(TFW_PTR_LIST)
	explicit TFW_LIST_CLASS_NAME(bool isAutoObjDelete);
#endif
	TFW_LIST_CLASS_NAME(const TFW_LIST_CLASS_NAME& listSrc);
	~TFW_LIST_CLASS_NAME();
	void copy(const TFW_LIST_CLASS_NAME* listSrc);
#if defined(TFW_PTR_LIST)
	void setAutoObjectDelete(bool isAuto);
#endif

private:
	TFW_LISTNODE_CLASS_NAME<TYPE>* newNode();
	void deleteNode(TFW_LISTNODE_CLASS_NAME<TYPE>* lnode);

	//======================================================================
	// 変数
private:
	TFW_LISTNODE_CLASS_NAME<TYPE>* _lnodeHead;
	TFW_LISTNODE_CLASS_NAME<TYPE>* _lnodeTail;
	u32 _count : 31;
	u32 _flags : 1;		// フラグ．InsList では不要だが統一させる
};

/*---------------------------------------------------------------------*//**
 *	リスト ノード
 *
**//*---------------------------------------------------------------------*/
template<class TYPE> class TFW_LISTNODE_CLASS_NAME
{
	//======================================================================
	// 変数
private:
	TYPE _obj;
	TFW_LISTNODE_CLASS_NAME* _lnodeNext;
	TFW_LISTNODE_CLASS_NAME* _lnodePrev;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	const TFW_LIST_REF_TYPE getObject();
	TFW_LIST_REF_TYPE object();
	void setObject(TFW_LIST_SET_TYPE obj);
	TFW_LISTNODE_CLASS_NAME* getNext();
	void setNext(TFW_LISTNODE_CLASS_NAME* lnodeNext);
	TFW_LISTNODE_CLASS_NAME* getPrev();
	void setPrev(TFW_LISTNODE_CLASS_NAME* lnodePrev);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TFW_LISTNODE_CLASS_NAME();
	~TFW_LISTNODE_CLASS_NAME();
};

/*---------------------------------------------------------------------*//**
 *	リストイテレータ
 *
**//*---------------------------------------------------------------------*/
template<class TYPE> class TFW_LISTIT_CLASS_NAME 
{
	//======================================================================
	// 変数
private:
	TFW_LISTNODE_CLASS_NAME<TYPE>* _nodeCur;

	//======================================================================
	// メソッド
public:
	inline TFW_LISTIT_CLASS_NAME()												{	_nodeCur = 0L;							}
	inline TFW_LISTIT_CLASS_NAME(TFW_LISTNODE_CLASS_NAME<TYPE>* node)			{	_nodeCur = node;						}
	inline TFW_LISTIT_CLASS_NAME(const TFW_LISTIT_CLASS_NAME& lit)				{	_nodeCur = lit._nodeCur;				}
	inline TFW_LISTIT_CLASS_NAME& copy(const TFW_LISTIT_CLASS_NAME* lit)		{	_nodeCur = lit->_nodeCur; return *this;	}
	inline TFW_LISTIT_CLASS_NAME& operator =(const TFW_LISTIT_CLASS_NAME& lit)	{	return copy(&lit);						}
	inline bool equals(const TFW_LISTIT_CLASS_NAME& lit)						{	return _nodeCur == lit._nodeCur;		}
	inline TFW_LISTNODE_CLASS_NAME<TYPE>* node() const							{	return _nodeCur;						}
	inline TFW_LIST_REF_TYPE object() const										{	return _nodeCur->object();				}
	inline bool has() const														{	return _nodeCur != 0L;					}
	inline TFW_LIST_REF_TYPE next()	{	TFW_LIST_REF_TYPE obj = _nodeCur->object(); _nodeCur = _nodeCur->getNext(); return obj;	}
	inline TFW_LIST_REF_TYPE prev()	{	TFW_LIST_REF_TYPE obj = _nodeCur->object(); _nodeCur = _nodeCur->getPrev(); return obj;	}
};

//==========================================================================
// TFW_LIST_CLASS_NAME メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ノード取得

	@param	index インデックス．0xffffffff で末尾を返す
	@return	ノードへのポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::node(u32 index) const
{
	// index が 0xffffffff のときは末尾を返す
	if(index == 0xffffffff)
	{
		return _lnodeTail;
	}

	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeCur = _lnodeHead;
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePrev = lnodeCur;
	u32 indexWk = 0;
	while(lnodeCur != 0L)
	{
		lnodePrev = lnodeCur;
		if(indexWk == index)
		{
			return lnodeCur;
		}
			
		lnodeCur = lnodeCur->getNext();
		indexWk++;
	}
		
	return 0L;
}

/*---------------------------------------------------------------------*//**
	オブジェクト取得

	@param	index インデックス．0xffffffff で末尾を返す
	@return	格納オブジェクトへの定数ポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
const TFW_LIST_REF_TYPE TFW_LIST_CLASS_NAME<TYPE>::getObject(u32 index) const
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnode = node(index);
	if(lnode == 0L)	{	return 0L;	}
	return lnode->getObject();
}

/*---------------------------------------------------------------------*//**
	オブジェクト取得

	@param	index インデックス．0xffffffff で末尾を返す
	@return	格納オブジェクトへのポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LIST_REF_TYPE TFW_LIST_CLASS_NAME<TYPE>::object(u32 index) const
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnode = node(index);
	if(lnode == 0L)	{	return 0L;	}
	return lnode->object();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	インデックス取得

	@param	node ノードへのポインタ
	@return	インデックス
**//*---------------------------------------------------------------------*/
template<class TYPE> 
u32 TFW_LIST_CLASS_NAME<TYPE>::getIndex(const TFW_LISTNODE_CLASS_NAME<TYPE>* node) const
{
	u32 cnt = 0;
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeWk = _lnodeHead;
	while(lnodeWk != 0L)
	{
		if(lnodeWk == node)
		{
			return cnt;
		}
		cnt++;
		lnodeWk = lnodeWk->getNext();
	}
	return 0xffffffff;
}

/*---------------------------------------------------------------------*//**
	インデックス取得

	@param	obj オブジェクト
	@return	インデックス
**//*---------------------------------------------------------------------*/
template<class TYPE> 
u32 TFW_LIST_CLASS_NAME<TYPE>::getIndex(const TFW_LIST_REF_TYPE obj) const
{
	u32 cnt = 0;
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeWk = _lnodeHead;
	while(lnodeWk != 0L)
	{
#if defined(TFW_INS_LIST)
		if(*lnodeWk->getObject() == *obj)
#else
		if(lnodeWk->getObject() == obj)
#endif
		{
			return cnt;
		}
		cnt++;
		lnodeWk = lnodeWk->getNext();
	}
	return 0xffffffff;
}
	
/*---------------------------------------------------------------------*//**
	ノードを末尾に追加

	@param	obj オブジェクト
	@return	新規に作成された（追加された）ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::addTail(TFW_LIST_SET_TYPE obj)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = 0L;
	if(_lnodeHead == 0L)	{	lnodeNew = _lnodeTail = _lnodeHead = newNode();		}
	else					{	lnodeNew = insertAfter(_lnodeTail);					}
	if(lnodeNew != 0L)	{	lnodeNew->setObject(obj);	}
	return lnodeNew;
}
	
/*---------------------------------------------------------------------*//**
	ノードを先頭に追加

	@param	obj オブジェクト
	@return	新規に作成された（追加された）ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::addHead(TFW_LIST_SET_TYPE obj)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = 0L;
	if(_lnodeHead == 0L)	{	lnodeNew = _lnodeTail = _lnodeHead = newNode();		}
	else					{	lnodeNew = insertBefore(_lnodeHead);				}
	if(lnodeNew != 0L)	{	lnodeNew->setObject(obj);	}
	return lnodeNew;
}
	
/*---------------------------------------------------------------------*//**
	ノードを末尾に追加

	@return	新規に作成された（追加された）ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::addTail()
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = 0L;
	if(_lnodeHead == 0L)	{	lnodeNew = _lnodeTail = _lnodeHead = newNode();		}
	else					{	lnodeNew = insertAfter(_lnodeTail);					}
	return lnodeNew;
}
	
/*---------------------------------------------------------------------*//**
	ノードを先頭に追加

	@return	新規に作成された（追加された）ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::addHead()
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = 0L;
	if(_lnodeHead == 0L)	{	lnodeNew = _lnodeTail = _lnodeHead = newNode();		}
	else					{	lnodeNew = insertBefore(_lnodeHead);				}
	return lnodeNew;
}
	
/*---------------------------------------------------------------------*//**
	ノード後方挿入

	@param	lnodePos 挿入位置にある既存ノード
	@return	新規に作成された（挿入された）ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::insertAfter(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = newNode();
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeTmp = lnodePos->getNext();
	lnodeNew->setNext(lnodeTmp);
	lnodeNew->setPrev(lnodePos);
	lnodePos->setNext(lnodeNew);
	if(lnodeTmp != 0L)	{	lnodeTmp->setPrev(lnodeNew);	}
	else				{	_lnodeTail = lnodeNew;			} 
	return lnodeNew;
}
	
/*---------------------------------------------------------------------*//**
	ノード前方挿入

	@param	lnodePos 挿入位置にある既存ノード
	@return	新規に作成された（挿入された）ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::insertBefore(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = newNode();
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeTmp = lnodePos->getPrev();
	lnodeNew->setNext(lnodePos);
	lnodeNew->setPrev(lnodeTmp);
	lnodePos->setPrev(lnodeNew);
	if(lnodeTmp != 0L)	{	lnodeTmp->setNext(lnodeNew);	}
	else				{	_lnodeHead = lnodeNew;			}
	return lnodeNew;
}
	
/*---------------------------------------------------------------------*//**
	オブジェクト後方挿入

	@param	lnodePos 挿入位置にある既存ノード
	@param	obj オブジェクト
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE> 
bool TFW_LIST_CLASS_NAME<TYPE>::insertAfter(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos, TFW_LIST_SET_TYPE obj)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = insertAfter(lnodePos);
	if(lnodeNew == 0L)	{	return false;	}
	lnodeNew->setObject(obj);
	return true;
}
	
/*---------------------------------------------------------------------*//**
	オブジェクト前方挿入

	@param	lnodePos 挿入位置にある既存ノード
	@param	obj オブジェクト
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE> 
bool TFW_LIST_CLASS_NAME<TYPE>::insertBefore(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePos, TFW_LIST_SET_TYPE obj)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNew = insertBefore(lnodePos);
	if(lnodeNew == 0L)	{	return false;	}
	lnodeNew->setObject(obj);
	return true;
}

/*---------------------------------------------------------------------*//**
	ノード削除

	@param	lnodePos 削除するノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LIST_CLASS_NAME<TYPE>::removeNode(TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeTrg)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodePrev = lnodeTrg->getPrev();
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNext = lnodeTrg->getNext();
	if(lnodePrev != 0L)	{	lnodePrev->setNext(lnodeNext);	}
	else				{	_lnodeHead = lnodeNext;			}
	if(lnodeNext != 0L)	{	lnodeNext->setPrev(lnodePrev);	}
	else				{	_lnodeTail = lnodePrev;			}

	deleteNode(lnodeTrg);
}

/*---------------------------------------------------------------------*//**
	削除
	※ removeNode より低速である

	@param	obj 削除するオブジェクト
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE> 
bool TFW_LIST_CLASS_NAME<TYPE>::remove(const TFW_LIST_REF_TYPE obj)
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnode = findNode(obj);
	if(lnode == 0L)	{	return false;	}
	removeNode(lnode);
	return true;
}
	
/*---------------------------------------------------------------------*//**
	ノード全削除
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LIST_CLASS_NAME<TYPE>::removeAll()
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeCur = _lnodeHead;
	while(lnodeCur != 0L)
	{
		TFW_LISTNODE_CLASS_NAME<TYPE>* lnodeNext = lnodeCur->getNext();
		deleteNode(lnodeCur);
		lnodeCur = lnodeNext;
	}

	_lnodeHead = 0L;
	_lnodeTail = 0L;
}

/*---------------------------------------------------------------------*//**
	ノード検索

	@param	obj 検索するノードを内包しているオブジェクト
	@return	検索されたノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::findNode(const TFW_LIST_REF_TYPE obj) const
{
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnode = _lnodeHead;
	while(lnode != 0L)
	{
		if(lnode->getObject() == obj)
		{
			return lnode;
		}
		lnode = lnode->getNext();
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
	※要素はコピーを作成する点に注意

	@param	parr			配列ポインタ
	@param	length			配列数
	@return	成否
**//*---------------------------------------------------------------------*/
template<class TYPE> 
bool TFW_LIST_CLASS_NAME<TYPE>::fromArray(TYPE* arr, u32 length)
{
	for(u32 i = 0; i < length; i++)
	{
		TYPE* obj = new TYPE(arr[i]);
		if(obj == 0L)	{	return false;	}
		this->addTail();
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	配列化

	@return	配列ポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TYPE* TFW_LIST_CLASS_NAME<TYPE>::toArray() const
{
	TYPE* parr = new TYPE[_count];
	u32 idx = 0;
	TFW_LISTNODE_CLASS_NAME<TYPE>* lnode = _lnodeHead;
	while(lnode != 0L)
	{
		parr[idx++] = lnode->getObject();
		lnode = lnode->getNext();
	}
	return parr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_LIST_CLASS_NAME<TYPE>::TFW_LIST_CLASS_NAME()
	: _lnodeHead(0L)
	, _lnodeTail(0L)
	, _count(0)
	, _flags(0)
{
}

#if defined(TFW_PTR_LIST)
/*---------------------------------------------------------------------*//**
	コンストラクタ

	@param	isAutoObjDelete 自動インスタンス削除をするかどうか
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LIST_CLASS_NAME<TYPE>::TFW_LIST_CLASS_NAME(bool isAutoObjDelete)
	: _lnodeHead(0L)
	, _lnodeTail(0L)
	, _count(0)
	, _flags(isAutoObjDelete ? F_AUTO_OBJ_DELETE : 0)
{
}
#endif

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ

	@param	listSrc コピー元リスト
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LIST_CLASS_NAME<TYPE>::TFW_LIST_CLASS_NAME(const TFW_LIST_CLASS_NAME& listSrc)
	: _lnodeHead(0L)
	, _lnodeTail(0L)
	, _count(0)
	, _flags(0)
{
	copy(&listSrc);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LIST_CLASS_NAME<TYPE>::~TFW_LIST_CLASS_NAME()
{
	removeAll();
}

/*---------------------------------------------------------------------*//**
	コピー

	@param	listSrc コピー元リスト
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LIST_CLASS_NAME<TYPE>::copy(const TFW_LIST_CLASS_NAME* listSrc)
{
	removeAll();

	TFW_LISTNODE_CLASS_NAME<TYPE>* lnode = listSrc->_lnodeHead;
	while(lnode != 0L)
	{
		this->addTail(lnode->getObject());
		lnode = lnode->getNext();
	}
}

#if defined(TFW_PTR_LIST)
/*---------------------------------------------------------------------*//**
	自動インスタンス削除をするかどうかの設定

	@param	isAutoObjDelete 自動インスタンス削除をするかどうか
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LIST_CLASS_NAME<TYPE>::setAutoObjectDelete(bool isAuto)
{
	u32 flags = _flags;
	TFW_SET_FLAG(flags, F_AUTO_OBJ_DELETE, isAuto);
	_flags = flags;
}
#endif

/*---------------------------------------------------------------------*//**
	ノード作成
	
	@return	新規に作成されたノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LIST_CLASS_NAME<TYPE>::newNode()
{
	_count++;
	return new TFW_LISTNODE_CLASS_NAME<TYPE>();
}

/*---------------------------------------------------------------------*//**
	ノード削除

	@param	lnode 削除するノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LIST_CLASS_NAME<TYPE>::deleteNode(TFW_LISTNODE_CLASS_NAME<TYPE>* lnode)
{
	#if defined(TFW_PTR_LIST)
		if(TFW_IS_FLAG(_flags, F_AUTO_OBJ_DELETE))
		{
			delete lnode->getObject();
		}
	#endif

	delete lnode;
	_count--;
}

//==========================================================================
// TFW_LISTNODE_CLASS_NAME メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	オブジェクト取得
		
	@return	オブジェクトの定数ポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
const TFW_LIST_REF_TYPE TFW_LISTNODE_CLASS_NAME<TYPE>::getObject()
{
#if defined(TFW_INS_LIST)
	return &_obj;
#else
	return _obj;
#endif
}

/*---------------------------------------------------------------------*//**
	オブジェクト取得
		
	@return	オブジェクトのポインタ
**//*---------------------------------------------------------------------*/
template<class TYPE>
TFW_LIST_REF_TYPE TFW_LISTNODE_CLASS_NAME<TYPE>::object()
{
#if defined(TFW_INS_LIST)
	return &_obj;
#else
	return _obj;
#endif
}

/*---------------------------------------------------------------------*//**
	オブジェクト設定

	@param	obj オブジェクト
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LISTNODE_CLASS_NAME<TYPE>::setObject(TFW_LIST_SET_TYPE obj)
{
#if defined(TFW_INS_LIST)
	_obj = *obj;
#else
	_obj = obj;
#endif
}

/*---------------------------------------------------------------------*//**
	次の要素取得

	@return	ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LISTNODE_CLASS_NAME<TYPE>::getNext()
{
	return _lnodeNext;
}

/*---------------------------------------------------------------------*//**
	次の要素設定

	@param	lnodeNext ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LISTNODE_CLASS_NAME<TYPE>::setNext(TFW_LISTNODE_CLASS_NAME* lnodeNext)
{
	_lnodeNext = lnodeNext;
}
	
/*---------------------------------------------------------------------*//**
	前の要素取得

	@return	ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>* TFW_LISTNODE_CLASS_NAME<TYPE>::getPrev()
{
	return _lnodePrev;
}
	
/*---------------------------------------------------------------------*//**
	前の要素設定

	@param	lnodeNext ノード
**//*---------------------------------------------------------------------*/
template<class TYPE> 
void TFW_LISTNODE_CLASS_NAME<TYPE>::setPrev(TFW_LISTNODE_CLASS_NAME* lnodePrev)
{
	_lnodePrev = lnodePrev;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>::TFW_LISTNODE_CLASS_NAME()
	: _lnodeNext(0L)
	, _lnodePrev(0L)
#if !defined(TFW_INS_LIST)
	, _obj(0L)
#endif
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE> 
TFW_LISTNODE_CLASS_NAME<TYPE>::~TFW_LISTNODE_CLASS_NAME()
{
}

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#undef TFW_LIST_CLASS_NAME
