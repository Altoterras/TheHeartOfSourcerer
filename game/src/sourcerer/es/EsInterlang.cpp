/***********************************************************************//**
 *	EsInterlang.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsInterlang.h"

// Friends
#include "EsAtomList.h"
#include "EsBcExInfo.h"
#include "EsBytecodeBuilder.h"
#include "EsFunction.h"
#include "EsFunctionBox.h"
#include "EsObject.h"

// External
#include "../../tfw/collection/InsArray.h"
#include "../../tfw/collection/List.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	アトムを得る ⇒ script->atomMap.vector[i]
**//*---------------------------------------------------------------------*/
const EsAtom* EsInterlang::getAtom(s32 index)
{
	if(_arrAtom == 0L)	{	return 0L;	}
	ASSERT((0 <= index) && ((u32)index < _arrAtom->getCount()));
	if((index < 0) || (_arrAtom->getCount() <= (u32)index))	{	return 0L;	}
	return _arrAtom->getAt(index);
}

/*---------------------------------------------------------------------*//**
	アトムの数を得る ⇒ script->atomMap.length
**//*---------------------------------------------------------------------*/
s32 EsInterlang::getAtomNum() const
{
	if(_arrAtom == 0L)	{	return 0L;	}
	return _arrAtom->getCount();
}

/*---------------------------------------------------------------------*//**
	オブジェクト配列からオブジェクトを得る
**//*---------------------------------------------------------------------*/
EsObject* EsInterlang::getObject(s32 index) const
{
	if(_arrObj == 0L)	{	return 0L;	}
	ASSERT((0 <= index) && ((u32)index < _arrObj->getCount()));
	if((index < 0) || (_arrObj->getCount() <= (u32)index))	{	return 0L;	}
	return _arrObj->getAt(index);
}

/*---------------------------------------------------------------------*//**
	オブジェクト数を得る
**//*---------------------------------------------------------------------*/
s32 EsInterlang::getObjectNum() const
{
	if(_arrObj == 0L)	{	return 0L;	}
	return _arrObj->getCount();
}

/*---------------------------------------------------------------------*//**
	上域変数情報の取得
**//*---------------------------------------------------------------------*/
const EsUpvarInfo* EsInterlang::getUpvarInfo(s32 index) const
{
	ASSERT((0 <= index) && ((u32)index < _arrUpvar->getCount()));
	if((index < 0) || (_arrUpvar->getCount() <= (u32)index))	{	return 0L;	}
	return _arrUpvar->getPointer(index);
}

/*---------------------------------------------------------------------*//**
	上域変数情報数を得る
**//*---------------------------------------------------------------------*/
s32 EsInterlang::getUpvarInfoNum() const
{
	if(_arrUpvar == 0L)	{	return 0L;	}
	return _arrUpvar->getCount();
}

/*---------------------------------------------------------------------*//**
	正規表現オブジェクト数を得る
**//*---------------------------------------------------------------------*/
s32 EsInterlang::getRegexpNum() const
{
	if(_arrRegexp == 0L)	{	return 0L;	}
	return _arrRegexp->getCount();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsInterlang::EsInterlang()
	: _bc(0L)
	, _arrAtom(0L)
	, _arrObj(0L)
	, _arrUpvar(0L)
	, _arrRegexp(0L)
	, _numFixedSlot(0)
	, _numSlot(0)
	, _depthNest(0)
	, _isAddBcExInfo(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsInterlang::~EsInterlang()
{
	delete _arrRegexp;
	delete _arrUpvar;
	delete _arrObj;
	delete _arrAtom;
	delete _bc;
}

/*---------------------------------------------------------------------*//**
	作成 ⇒ js_NewScript
**//*---------------------------------------------------------------------*/
bool EsInterlang::create(s32 numUpvar, bool isAddBcExInfo)
{
	// バイトコード作成
	_bc = new VcString();

	// ダミーのバイトコードを書き込む
	{
		// 拡張情報を付加する
		_isAddBcExInfo = isAddBcExInfo;
		if(_isAddBcExInfo)	// 拡張情報付加されている場合
		{
			EsBcExInfo exi(0, 0);
			u8 bbc[EsBcExInfo::LEN_BC];
			exi.toBytecode(bbc);
			for(int i = 0; i < EsBcExInfo::LEN_BC; i++)
			{
				*_bc += bbc[i];
			}
		}

		// STOP コードを入れておく
		*_bc += (u8)OP_STOP;
	}

	// 上域変数情報配列の作成
	_arrUpvar = new InsArray<EsUpvarInfo>;
	_arrUpvar->alloc(numUpvar);
	for(int i = 0; i < numUpvar; i++) { _arrUpvar->setAt(i, EsUpvarInfo()); }

	// 正規表現オブジェクト配列の作成
	_arrRegexp = new InsArray<EsObject*>;

	return true;
}

/*---------------------------------------------------------------------*//**
	コピー作成
**//*---------------------------------------------------------------------*/
bool EsInterlang::create(const EsInterlang* src)
{
	_bc = new VcString(src->_bc);

	_arrAtom = new InsArray<const EsAtom*>;
	_arrAtom->setArray(src->_arrAtom);

	_arrObj = new InsArray<EsObject*>;
	_arrObj->setArray(src->_arrObj);

	_arrUpvar = new InsArray<EsUpvarInfo>;
	_arrUpvar->setArray(src->_arrUpvar);

	_arrRegexp = new InsArray<EsObject*>;
	_arrRegexp->setArray(src->_arrRegexp);

	_numFixedSlot = src->_numFixedSlot;
	_numSlot = src->_numSlot;
	_depthNest = src->_depthNest;
	_isAddBcExInfo = src->_isAddBcExInfo;

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコードビルダーより作成 ⇒ js_NewScriptFromCG
**//*---------------------------------------------------------------------*/
bool EsInterlang::create(EsBytecodeBuilder* bcb)
{
	// アトム配列を作成する ⇒ js_InitAtomMap(cx, &script->atomMap, &cg->atomList);
	_arrAtom = new InsArray<const EsAtom*>;
	if(_arrAtom == 0L)
	{
		return false;
	}
	const EsAtomList* alist = bcb->getLiteralAtomList();
	// 最大のインデックスを得る
	u32 indexMax = 0;
	for(int i = 0; i < alist->getCount(); i++)
	{
		const EsAtomList::EsAtomListEntity* ale = alist->getEntity(i);
		if(indexMax < ale->getIndex())
		{
			indexMax = ale->getIndex();
		}
	}
	// 配列を作成し、コピーする
	if(_arrAtom->alloc(indexMax + 1))
	{
		for(int i = 0; i < alist->getCount(); i++)
		{
			const EsAtomList::EsAtomListEntity* ale = alist->getEntity(i);
			_arrAtom->setAt(ale->getIndex(), ale->getAtom());
		}
	}
	else
	{
		delete _arrAtom;
		_arrAtom = 0L;
		return false;
	}

	// オブジェクトボックスリストをオブジェクト配列に引き継ぐ
	{
		const List<EsObjectBox*>* listObjbox = bcb->getObjectBoxList();
		s32 numObjArr = listObjbox->getCount();
		_arrObj = new InsArray<EsObject*>;
		if(_arrObj == 0L)
		{	
			return false;
		}
		if(!_arrObj->alloc(numObjArr))
		{
			delete _arrObj;
			_arrObj = 0L;
			return false;
		}
		int index = 0;
		for(ListIterator<EsObjectBox*> it = listObjbox->iterator(); it.has(); it.next())
		{
			_arrObj->setAt(index++, it.object()->getBlockObject());
		}
	}

	// バイトコードはメイン外とメインを足し合わせたもの
	_bc = new VcString(*bcb->getOtherBytecode());
	if(_bc == 0L)
	{
		return false;
	}
	_bc->add(bcb->getMainBytecode());

	// 固定スロット数 ⇒ nfixed = (cg->flags & TCF_IN_FUNCTION) ? cg->fun->u.i.nvars : cg->ngvars + cg->regexpList.length;
	if(TFW_IS_FLAG(bcb->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION))
	{
		_numFixedSlot =	bcb->getCurrentFunction()->getLocalVarNum(EsFunction::LVK_VAR);
	}
	else
	{
		_numFixedSlot =	bcb->getGlobalVarsNum();
	}
	// 合計スロット数 ⇒ script->nslots = script->nfixed + cg->maxStackDepth;
	_numSlot = (u16)(_numFixedSlot + bcb->getMaxStackDepth());

	// 拡張情報付加フラグ
	_isAddBcExInfo = bcb->isExInfoAdd();

	// 上域変数情報配列の作成
	_arrUpvar = new InsArray<EsUpvarInfo>;
	if(_arrUpvar == 0L)
	{
		return false;
	}

	// 正規表現オブジェクト配列の作成
	_arrRegexp = new InsArray<EsObject*>;
	if(_arrRegexp == 0L)
	{
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
