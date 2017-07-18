/***********************************************************************//**
 *	EsAtomList.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/31.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_ATOM_LIST_H_
#define _ES_ES_ATOM_LIST_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsAtom.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsNameItNode;

/*---------------------------------------------------------------------*//**
 *	アトムリスト
 *
**//*---------------------------------------------------------------------*/
class EsAtomList
{
	//======================================================================
	// 定数
public:
	enum AddHow
	{
		AH_UNIQUE,
		AH_TAIL,		// ⇒ HOIST
		AH_HEAD,		// ⇒ SHADOW
	};

	//======================================================================
	// クラス
public:
	class EsAtomListEntity
	{
	public:
		// アトムを得る ⇒ ALE_ATOM
		inline const EsAtom*	getAtom() const						{	return _atom;		}
		// 定義ノードを得る ⇒ ALE_DEFN
		inline EsNameItNode*	getDefName() const					{	return _nodeNam;	}
		// 値を得る ⇒ ALE_VALUE
		inline const EsValue*	getValue() const					{	return _val;		}
		// インデックスを得る ⇒ ALE_INDEX
		inline u32				getIndex() const					{	return _index;		}

		// 定義ノードを設定する ⇒ ALE_SET_DEFN
		inline void				setDefName(EsNameItNode* nodeNam)	{	_nodeNam = nodeNam;	}
		// 値を設定する ⇒ ALE_SET_VALUE
		void					setValue(const EsValue* val);

	public:
		EsAtomListEntity(const EsAtom* atom, EsNameItNode* nodeNam, u32 index) : _atom(atom), _nodeNam(nodeNam), _index(index), _val(0L) {}
		EsAtomListEntity(const EsAtomListEntity& src) : _atom(src._atom), _nodeNam(src._nodeNam), _index(src._index), _val(src._val) {}

	private:
		const EsAtom*		_atom;
		EsNameItNode*		_nodeNam;
		u32					_index;
		EsValue*			_val;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 数を取得する
	s32 getCount() const;
	// アトムを取得する
	const EsAtom* getAtom(u32 index) const;
	// エンティティを取得する
	EsAtomListEntity* getEntity(u32 index) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsAtomListEntity* add(const EsAtom* atom, AddHow ah);
	inline EsAtomListEntity* add(const EsAtom* atom) { return add(atom, AH_UNIQUE); }
	void remove(EsAtomListEntity* entity);
	void remove(const EsAtom* atom);
	void removeAll();
	EsAtomListEntity* find(const EsAtom* atom);
	EsAtomListEntity* find(const VcString* str);

	u32 getListIndex(EsAtomListEntity* entity);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsAtomList();
	~EsAtomList();
	EsAtomList(const EsAtomList& src);
	void copy(const EsAtomList* src);

	//======================================================================
	// 変数
private:
	List<EsAtomListEntity*>*	_listEntity;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_ATOM_LIST_H_
