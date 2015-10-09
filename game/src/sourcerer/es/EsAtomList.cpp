/***********************************************************************//**
 *	EsAtomList.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/31.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsAtomList.h"

// Friends
#include "EsAtom.h"
#include "EsItNode.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/hash/HashTable.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsAtomList メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	数を取得する
**//*---------------------------------------------------------------------*/
s32 EsAtomList::getCount() const
{
	return _listEntity->getCount();
}

/*---------------------------------------------------------------------*//**
	アトム取得
**//*---------------------------------------------------------------------*/
const EsAtom* EsAtomList::getAtom(u32 index) const
{
	EsAtomListEntity* entity = _listEntity->getObject(index);
	if(entity == 0L)	{	return 0L;	}
	return entity->getAtom();
}

/*---------------------------------------------------------------------*//**
	エンティティを取得する
**//*---------------------------------------------------------------------*/
EsAtomList::EsAtomListEntity* EsAtomList::getEntity(u32 index) const
{
	return _listEntity->getObject(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アトム追加
**//*---------------------------------------------------------------------*/
EsAtomList::EsAtomListEntity* EsAtomList::add(const EsAtom* atom, AddHow ah)
{
	EsAtomListEntity* ale = find(atom);
	if((ale == 0L) || (ah != AH_UNIQUE))
	{
		u32 index = _listEntity->getCount();	// ⇒ ALE_SET_INDEX(ale, count++);
		ale = new EsAtomListEntity(atom, 0L, index);

		if(ah == AH_TAIL)
		{
			_listEntity->addTail(ale);
		}
		else
		{
			_listEntity->addHead(ale);
		}
	}

	return ale;
}

/*---------------------------------------------------------------------*//**
	エンティティ削除
**//*---------------------------------------------------------------------*/
void EsAtomList::remove(EsAtomListEntity* entity)
{
	_listEntity->remove(entity);
}

/*---------------------------------------------------------------------*//**
	アトム削除
**//*---------------------------------------------------------------------*/
void EsAtomList::remove(const EsAtom* atom)
{
	EsAtomListEntity* ale = find(atom);
	if(ale == 0L)	{	return;	}
	_listEntity->remove(ale);
}

/*---------------------------------------------------------------------*//**
	アトム全削除
**//*---------------------------------------------------------------------*/
void EsAtomList::removeAll()
{
	_listEntity->removeAll();
}

/*---------------------------------------------------------------------*//**
	既存のアトムを検索
**//*---------------------------------------------------------------------*/
EsAtomList::EsAtomListEntity* EsAtomList::find(const EsAtom* atom)
{
	for(ListIterator<EsAtomListEntity*> it = _listEntity->iterator(); it.has(); it.next())
	{
		if(it.object()->getAtom() == atom)
		{
			return it.object();
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	文字列からアトム検索
**//*---------------------------------------------------------------------*/
EsAtomList::EsAtomListEntity* EsAtomList::find(const VcString* str)
{
	for(ListIterator<EsAtomListEntity*> it = _listEntity->iterator(); it.has(); it.next())
	{
		if(it.object()->getAtom()->isSameString(str))
		{
			return it.object();
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	リストインデックスを得る
**//*---------------------------------------------------------------------*/
u32 EsAtomList::getListIndex(EsAtomListEntity* entity)
{
	return _listEntity->getIndex(entity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsAtomList::EsAtomList()
	: _listEntity(new List<EsAtomListEntity*>(true))
{
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
EsAtomList::EsAtomList(const EsAtomList& src)
	: _listEntity(new List<EsAtomListEntity*>(true))
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsAtomList::~EsAtomList()
{
	delete _listEntity;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EsAtomList::copy(const EsAtomList* src)
{
	for(ListIterator<EsAtomListEntity*> it = src->_listEntity->iterator(); it.has(); it.next())
	{
		EsAtomListEntity* entitySrc = it.object();
		EsAtomListEntity* entityDst = new EsAtomListEntity(*entitySrc);
		_listEntity->addTail(entityDst);
	}
}

//==========================================================================
// EsAtomList::EsAtomListEntity メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	値を設定する ⇒ ALE_SET_VALUE
**//*---------------------------------------------------------------------*/
void EsAtomList::EsAtomListEntity::setValue(const EsValue* val)
{
	_val->copy(val);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
