/***********************************************************************//**
 *	EsContext.cpp
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
#include "EsContext.h"

// Friends
#include "EsArray.h"
#include "EsAtom.h"
#include "EsAtomList.h"
#include "EsBlock.h"
#include "EsClass.h"
#include "EsError.h"
#include "EsExternalExtension.h"
#include "EsSysClasses.h"
#include "EsFunction.h"
#include "EsInterlang.h"
#include "EsIterator.h"
#include "EsItNodeFactory.h"
#include "EsKeywordSet.h"
#include "EsNumber.h"
#include "EsObject.h"
#include "EsDefaultObjectOp.h"
#include "EsResolver.h"
#include "EsRunEnv.h"
#include "EsStackFrame.h"
#include "EsString.h"
#include "EsValue.h"
#include "EsWith.h"

// External
#include "../tfw/collection/Array.h"
#include "../tfw/collection/List.h"
#include "../tfw/hash/HashTable.h"
#include "../tfw/lib/TypeUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsContext メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ディスプレイスタックフレームの取得
**//*---------------------------------------------------------------------*/
EsStackFrame* EsContext::getDisplayStackFrame(s32 depth) const
{
	ASSERT((0 <= depth) && ((u32)depth < _arrSfrmDisp->getAllocSize()));
	if((depth < 0) || (_arrSfrmDisp->getAllocSize() <= (u32)depth))
	{
		return 0L;
	}
	return _arrSfrmDisp->getAt(depth);
}

/*---------------------------------------------------------------------*//**
	ディスプレイスタックフレームの設定
**//*---------------------------------------------------------------------*/
void EsContext::setDisplayStackFrame(s32 depth, EsStackFrame* sfrm)
{
	ASSERT((0 <= depth) && ((u32)depth < _arrSfrmDisp->getAllocSize()));
	if((depth < 0) || (_arrSfrmDisp->getAllocSize() <= (u32)depth))
	{
		return;
	}
	_arrSfrmDisp->setAt(depth, sfrm);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	解析準備
**//*---------------------------------------------------------------------*/
void EsContext::prepareParse(const EsKeywordSet* kwsetRef, EsExternalExtension* extextRef, u32 mflags, EsExtParam paramParse)
{
	_kwsetRef = kwsetRef;
	_extextRef = extextRef;
	_mflags = mflags;
	_paramParse = paramParse;

	ASSERT(_kwsetRef != 0L);
}

/*---------------------------------------------------------------------*//**
	コードを設定する
**//*---------------------------------------------------------------------*/
void EsContext::setCode(const VcString* code)
{
	_code->set(code);
}

/*---------------------------------------------------------------------*//**
	解析終了の確認

	@param codeExtScriptOut	追加スクリプトコード（戻り値 false の場合有効）
	@retval true	解析を終了して良し
	@retval false	解析を続ける（追加スクリプトコードが有効になる）
**//*---------------------------------------------------------------------*/
bool EsContext::checkParseEnd()
{
	if(_extextRef == 0L)	{	return true;	}	// 終了を許可

	VcString codeExtScript;

	// 未解決の関数がある場合は外部拡張で解決を試みる
	if(_extextRef->resolveUndefinedFunctionByExtFunc(&codeExtScript, _paramParse))
	{
		return true;
	}
	else
	{
		_code->add(&codeExtScript);
		return false;
	}
}

/*---------------------------------------------------------------------*//**
	実行準備 ⇒ js_Execute の冒頭部分
**//*---------------------------------------------------------------------*/
bool EsContext::prepareRun(EsInterlang* il, EsStackFrame* sfrmCaller, u16 flagsSfrm, EsValue* valRet, EsExtParam paramRun)
{
	// 現在の関数呼び出し深さを得る
	s32 depthCall = (_runenvCur != 0L) ? _runenvCur->getCallDepth() : -1;

	// 実行環境スタックを積む
	{
		RunStack* rs = new RunStack(_runenvCur);
		rs->_rsUp = _rsCur;
		_rsCur = rs;
	}
	
	// 中間言語を設定
	_rsCur->_il = il;

	// 実行環境の作成
	if(sfrmCaller == 0L)
	{
		ASSERT(_runenvCur == 0L);
		_runenvCur = new EsRunEnv();
	}
	ASSERT(_runenvCur != 0L);
	if(_runenvCur == 0L)
	{
		return false;
	}

	// スタックフレームの実行準備
	EsStackFrame* sfrmNew = newStackFrame();
	if(sfrmNew == 0L)
	{
		return false;
	}
	sfrmNew->prepareForRun(
		this,
		_rsCur->_il,
		_objGlobalRef,
		sfrmCaller,
		flagsSfrm	);
	_rsCur->_sfrm = sfrmNew;	// 現在のスタックフレームとして設定 ⇒ cx->fp = &frame;

	// その他変数の初期化
	_paramRun = paramRun;
	_isStop = false;

	// 実行環境の準備
	if(!_runenvCur->begin(this, depthCall + 1, valRet))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	コール準備 ⇒ inline_call:
**//*---------------------------------------------------------------------*/
bool EsContext::prepareCall(EsFunction* func, EsInterlang* il, u16 flagsSfrm, EsValue* vaStack, s32 numArg, EsObject* objCallee, EsObject* objThis, EsObject* objScopeChain, EsStackFrame* sfrmCaller)
{
	// 現在の関数呼び出し深さを得る
	ASSERT(_runenvCur != 0L);
	s32 depthCall = (_runenvCur != 0L) ? _runenvCur->getCallDepth() : -1;

	// 実行環境スタックを積む
	ASSERT(sfrmCaller != 0L);
	{
		RunStack* rsNew = new RunStack(_runenvCur);
		rsNew->_rsUp = _rsCur;
		_rsCur = rsNew;
	}

	// 中間言語を設定
	_rsCur->_il = il;

	// スタックフレームの実行準備
	EsStackFrame* sfrmNew = newStackFrame();
	if(sfrmNew == 0L)
	{
		return false;
	}
	sfrmNew->prepareForCall(
		this,
		func,
		_rsCur->_il,
		flagsSfrm,
		vaStack,
		numArg,
		func,
		objThis,
		objScopeChain,
		sfrmCaller	);
	_rsCur->_sfrm = sfrmNew;	// 現在のスタックフレームとして設定

	// 実行環境の開始処理
	if(!_runenvCur->begin(this, depthCall + 1, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	コール後処理
**//*---------------------------------------------------------------------*/
bool EsContext::disposeCall()
{
	// 実行環境スタックを一段崩す
	if(_rsCur->_rsUp != 0L)
	{
		RunStack* rsDel = _rsCur;

		// 実行環境の終了処理
		_runenvCur->end(rsDel->_runenv);

		// スタックを一段上げる
		_rsCur = _rsCur->_rsUp;
		rsDel->_rsUp = 0L;
		delete rsDel;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スコープチェインからプロパティを検索する
	⇒ js_FindProperty, js_FindPropertyHelper
**//*---------------------------------------------------------------------*/
bool EsContext::findPropertyFromScopeChain(EsScopeProperty** spropOut, EsObject** objScopeOut, EsObject** objScopeParentOut, const EsValue* spropid)
{
	EsScopeProperty* sprop = 0L;
	EsObject* objScopeOwner = 0L;
	EsObject* objWk = getTopStackFrame()->getScopeChainObject();
	EsObject* objParentWk = objWk->slots()->parentObject();
	while(
		(objParentWk != 0L) ?
			EsSysClasses::isNonGlobalScope(objWk->getClass()->getClassType()) :
			(objWk->getObjectOp()->getFindPropertyFunc() == &EsDefaultObjectOp::findProperty)	)
	{
		if(!objWk->findPropertyBase(&sprop, &objScopeOwner, this, spropid))
		{
			return 0L;
		}
		if(sprop != 0L)
		{
			// ※※※ 実装保留 ※※※ プロパティキャッシュ ⇒ js_FillPropertyCache
			goto END;
		}

		if(objParentWk == 0L)
		{
			objScopeOwner = 0L;
			goto END;
		}

		objWk = objParentWk;
		objParentWk = objWk->slots()->parentObject();
	}

	while(true)
	{
		if(!objWk->findPropertyOv(&sprop, &objScopeOwner, this, spropid))
		{
			return false;
		}
		if(sprop != 0L)
		{
			break;
		}

		objParentWk = objWk->slots()->parentObject();
		if(objParentWk == 0L)
		{
			objScopeOwner = 0L;
			break;
		}
		objWk = objParentWk;
	}

END:
	if(spropOut != 0L)			{	*spropOut = sprop;					}
	if(objScopeParentOut != 0L)	{	*objScopeParentOut = objScopeOwner;	}
	if(objScopeOut != 0L)		{	*objScopeOut = objWk;				}
	return true;
}

/*---------------------------------------------------------------------*//**
	識別子を捜す ⇒ js_FindIdentifierBase
**//*---------------------------------------------------------------------*/
EsObject* EsContext::findIdentifierBase(EsObject* objScopeChain, const EsValue* id)
{
	EsScopeProperty* sprop = 0L;
	EsObject* objScopeOwner = 0L;
	EsObject* objWk = objScopeChain;
	while(EsSysClasses::isNonGlobalScope(objWk->getClass()->getClassType()))
	{
		if(!objWk->findPropertyBase(&sprop, &objScopeOwner, this, id))
		{
			return 0L;
		}
		if(sprop != 0L)
		{
			// ※※※ 実装保留 ※※※ プロパティキャッシュ ⇒ js_FillPropertyCache

			return objWk;
		}

		objWk = objWk->slots()->parentObject();
		if(objWk->getSlots()->getParentObject() == 0L)
		{
			return objWk;
		}
	}

	while(true)
	{
		if(!objWk->findPropertyOv(&sprop, &objScopeOwner, this, id))
		{
			return 0L;
		}
		if(sprop != 0L)
		{
			objScopeOwner->dropPropertyOv(this, sprop);
			return objWk;
		}

		EsObject* objParentWk = objWk->slots()->parentObject();
		if(objParentWk == 0L)
		{
			return objWk;
		}
		objWk = objParentWk;
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	グローバルオブジェクトのセット
**//*---------------------------------------------------------------------*/
void EsContext::setGlobalObject(EsObject* objGlobalRef)
{
	_objGlobalRef = objGlobalRef;
}

/*---------------------------------------------------------------------*//**
	外部拡張用に関数情報を追加する

	@param atom 関数名アトム
	@param isDef true で関数定義、false で関数呼び出し
**//*---------------------------------------------------------------------*/
void EsContext::addParsedFunctionInfoForExternalExtension(const EsAtom* atom, bool isDef)
{
	if(_extextRef == 0L)	{	return;	}
	_extextRef->addParsedFuncInfo(atom->getString(), isDef);
}

/*---------------------------------------------------------------------*//**
	呼ばれているスクリプトを取得 ⇒ js_GetScriptedCaller
**//*---------------------------------------------------------------------*/
EsStackFrame* EsContext::getScriptedCallerFrame() const
{
	EsStackFrame* sfrmWk = getTopStackFrame();
	while(sfrmWk != 0L)
	{
		if(sfrmWk->getInterlang() != 0L)
		{
			return sfrmWk;
		}
		sfrmWk = sfrmWk->getDownStackFrame();
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	ツリーから外れたノードを追加する
**//*---------------------------------------------------------------------*/
void EsContext::addLeakNode(EsItNode* node)
{
#if DEL_TREE_CHILD
	_listDelNode->addTail(node);
#endif
}

/*---------------------------------------------------------------------*//**
	ツリーから外れたノードを除去する
**//*---------------------------------------------------------------------*/
void EsContext::removeLeakNode(EsItNode* node)
{
#if DEL_TREE_CHILD
	// そのまま remove すると実体が List::_isAutoObjDelete : true 設定により消えてしまうため、オブジェクトを NULL 化する
	ListNode<EsItNode*>* lnode = _listDelNode->findNode(node); 
	if(lnode == 0L)	{	return;	}
	lnode->setObject(0L);
	_listDelNode->removeNode(lnode);
#endif
}

/*---------------------------------------------------------------------*//**
	収集削除用のノードを全削除する
**//*---------------------------------------------------------------------*/
void EsContext::addDeleteCollectionNode(EsItNode* node)
{
#if !DEL_TREE_CHILD
	_listDelNode->addTail(node);
#endif
}

/*---------------------------------------------------------------------*//**
	収集削除用のノードを全削除する
**//*---------------------------------------------------------------------*/
void EsContext::clearDeleteCollectionNodes()
{
	_listDelNode->removeAll();
}

/*---------------------------------------------------------------------*//**
	新しい中間言語の作成
**//*---------------------------------------------------------------------*/
EsInterlang* EsContext::newInterlang()
{
	EsInterlang* il = new EsInterlang();

	_listDelUnit->addTail(il);		// 削除ユニットリストに追加する
	return il;
}

/*---------------------------------------------------------------------*//**
	新しいスタックフレームの作成
**//*---------------------------------------------------------------------*/
EsStackFrame* EsContext::newStackFrame()
{
	EsStackFrame* sfrm = new EsStackFrame();

	_listDelUnit->addTail(sfrm);	// 削除ユニットリストに追加する
	return sfrm;
}

/*---------------------------------------------------------------------*//**
	新しいオブジェクトの作成 ⇒ js_NewObject
**//*---------------------------------------------------------------------*/
EsObject* EsContext::newObject(const EsClass* cls, EsObject* objPrototype, EsObject* objParent)
{
	if(objPrototype == 0L)
	{
		EsValue id;
		if(!cls->getClassId(&id, this))	{	return 0L;	}
		if(!objParent->getClassPrototype(&objPrototype, this, &id))	{	return 0L;	}
		if(objPrototype == 0L)
		{
			EsValue valPtypeKindObj((s32)EsSysPrototypes::PTYPEKIND_OBJECT);
			if(!objParent->getClassPrototype(&objPrototype, this, &valPtypeKindObj))	{	return 0L;	}
		}
	}

	return newObjectWithGivenPrototype(cls, objPrototype, objParent);
}

/*---------------------------------------------------------------------*//**
	与えられたプロトタイプで新しいオブジェクトの作成
	⇒ js_NewObjectWithGivenProto
**//*---------------------------------------------------------------------*/
EsObject* EsContext::newObjectWithGivenPrototype(const EsClass* cls, EsObject* objPrototype, EsObject* objParent)
{
	EsObject* obj = 0L;
	switch(cls->getClassType())
	{
	case EsSysClasses::CLASSTYPE_ARRAY:		obj = new EsArray();					break;
	case EsSysClasses::CLASSTYPE_BLOCK:		obj = new EsBlock();					break;
	case EsSysClasses::CLASSTYPE_FUNC:		obj = new EsFunction();					break;
	case EsSysClasses::CLASSTYPE_ITERATOR:	obj = new EsIterator();					break;
	default:								obj = new EsObject(cls, OBJT_OBJECT);	break;
	}
	if(obj == 0L)	{	return 0L;	}

	obj->slots()->setPrototypeObject(objPrototype);
	obj->slots()->setParentObject(objParent);

	_listDelUnit->addTail(obj);		// 削除ユニットリストに追加する
	return obj;
}

/*---------------------------------------------------------------------*//**
	新しい内部関数オブジェクトの作成 ⇒ js_NewFunction + JSFUN_INTERPRETED
**//*---------------------------------------------------------------------*/
EsFunction* EsContext::newFunction(const EsAtom* atom, EsObject* objPrototype, EsObject* objParent, bool isLambda)
{
	EsFunction* func = 0L;
	if(objPrototype != 0L)
	{
		ASSERT(objPrototype->getClass()->getClassType() == EsSysClasses::CLASSTYPE_FUNC);
		ASSERT(objPrototype->getObjType() == OBJT_FUNC);
		objPrototype->slots()->setParentObject(objParent);
		func = (EsFunction*)objPrototype;
	}
	else
	{
		func = (EsFunction*)newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_FUNC), 0L, objParent);
		if(func == 0L)
		{
			return 0L;
		}
	}
	if(!func->createAsInterpreted(this, atom, objParent, isLambda))
	{
		delete func;
		return 0L;
	}

	return func;
}

/*---------------------------------------------------------------------*//**
	新しい外部関数オブジェクトの作成 ⇒ js_NewFunction
**//*---------------------------------------------------------------------*/
EsFunction* EsContext::newFunction(const EsAtom* atom, EsNativeCall call, EsObject* objParent, EsExtParam paramDef)
{
	EsFunction* func = (EsFunction*)newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_FUNC), 0L, objParent);
	if(func == 0L)
	{
		return 0L;
	}
	if(!func->createAsNative(this, atom, objParent, call, paramDef))
	{
		delete func;
		return 0L;
	}

	return func;
}

/*---------------------------------------------------------------------*//**
	新しい配列オブジェクトの作成 ⇒ js_NewArrayObject
**//*---------------------------------------------------------------------*/
EsArray* EsContext::newArray(EsValue* valarr, s32 lenArr)
{
	EsArray* arr = (EsArray*)newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARRAY), 0L, 0L);
	if(arr == 0L)	{	return 0L;	}

	if(!arr->create(this, valarr, lenArr))
	{
		delete arr;
		return 0L;
	}

	return arr;
}

/*---------------------------------------------------------------------*//**
	新しいイテレータオブジェクトの作成
**//*---------------------------------------------------------------------*/
EsIterator* EsContext::newIterator(EsObject* objParent, u8 flagsIter)
{
	EsIterator* iter = (EsIterator*)newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ITERATOR), 0L, objParent);
	if(iter == 0L)	{	return 0L;	}

	if(!iter->create(this, objParent, flagsIter))
	{
		delete iter;
		return 0L;
	}

	return iter;
}

/*---------------------------------------------------------------------*//**
	新しい文字列の作成 ⇒ js_NewString
**//*---------------------------------------------------------------------*/
const VcString* EsContext::newString(const VcString* str)
{
	if(str->getLength() == 0)
	{
		return TypeUtils::getEmptyString();
	}

	VcString* strNew = new VcString(str); 
	if(strNew == 0L)
	{
		return 0L;
	}

	_listDelString->addTail(strNew);
	return strNew;
}

/*---------------------------------------------------------------------*//**
	カスタムユニットの登録
**//*---------------------------------------------------------------------*/
void EsContext::registerCustomUnit(EsUnit* unit)
{
	_listDelUnit->addTail(unit);
}

/*---------------------------------------------------------------------*//**
	登録済みカスタムユニットの削除
**//*---------------------------------------------------------------------*/
void EsContext::deleteRegisteredCustomUnit(EsUnit* unit)
{
	for(ListIterator<EsUnit*> it = _listDelUnit->iterator(); it.has(); it.next())
	{
		EsUnit* eu = it.object();
		if(eu == unit)
		{
			_listDelUnit->removeNode(it.node());
			return;
		}
	}
}

/*---------------------------------------------------------------------*//**
	ユニットの削除
**//*---------------------------------------------------------------------*/
void EsContext::clearDeleteCollectionUnits(bool isAfterParse)
{
	if(isAfterParse)
	{
		ListIterator<EsUnit*> itCur;
		ListIterator<EsUnit*> itNext = _listDelUnit->iterator();
		while(itNext.has())
		{
			itCur = itNext;
			itNext.next();
			const EsUnit* eu = itCur.object();
			if(eu->isParseTimeOnly())
			{
				_listDelUnit->removeNode(itCur.node());
			}
		}
	}
	else
	{
		_listDelUnit->removeAll();
	}
}

/*---------------------------------------------------------------------*//**
	不要ユニットの削除
**//*---------------------------------------------------------------------*/
void EsContext::clearGarbageUnits()
{
	ListIterator<EsUnit*> itCur;
	ListIterator<EsUnit*> itNext = _listDelUnit->iterator();
	while(itNext.has())
	{
		itCur = itNext;
		itNext.next();
		const EsUnit* eu = itCur.object();
		if(eu->isGarbage())
		{
			_listDelUnit->removeNode(itCur.node());
		}
	}
}

/*---------------------------------------------------------------------*//**
	作成済みの文字列アトムを検索する ⇒ JSSTRING_IS_ATOMIZED
**//*---------------------------------------------------------------------*/
const EsAtom* EsContext::findMadeStringAtom(const VcString* str) const
{
	// まずキーワードセットから検索する
	ASSERT(_kwsetRef != 0L);
	const EsAtom* atom = _kwsetRef->findAtom(str);
	if(atom != 0L)
	{
		return atom;
	}

	// ハッシュテーブルから検索する
	u32 hashkey = _htblStringAtoms->calcHashKey(str);
	EsAtom::EsAtomHashEntry* entity = (EsAtom::EsAtomHashEntry*)_htblStringAtoms->find(str);
	if(entity == 0L)
	{
		return 0L;
	}
	return entity->getAtom();
}

/*---------------------------------------------------------------------*//**
	文字列アトムの作成 ⇒ js_AtomizeString, JS_InternString
**//*---------------------------------------------------------------------*/
const EsAtom* EsContext::makeStringAtom(const VcString* str)
{
	// まずキーワードセットから検索する
	ASSERT(_kwsetRef != 0L);
	const EsAtom* atom = _kwsetRef->findAtom(str);
	if(atom != 0L)
	{
		return atom;
	}

	// 文字列アトムを作成し、ハッシュテーブルに追加する
	u32 hashkey = _htblStringAtoms->calcHashKey(str);
	EsAtom::EsAtomHashEntry* entity = (EsAtom::EsAtomHashEntry*)_htblStringAtoms->add(str);
	if(entity == 0L)
	{
		ASSERT(false);
		return 0L;
	}
	return entity->getAtom();
}

/*---------------------------------------------------------------------*//**
	ダブル値アトムの作成 ⇒ js_AtomizeDouble
**//*---------------------------------------------------------------------*/
const EsAtom* EsContext::makeDoubleAtom(const f64* fval)
{
	// ダブル値アトムを作成し、ハッシュテーブルに追加する
	u32 hashkey = _htblDoubleAtoms->calcHashKey(fval);
	EsAtom::EsAtomHashEntry* entity = (EsAtom::EsAtomHashEntry*)_htblDoubleAtoms->add(fval);
	if(entity == 0L)
	{
		ASSERT(false);
		return 0L;
	}
	return entity->getAtom();
}

/*---------------------------------------------------------------------*//**
	値アトムの作成
**//*---------------------------------------------------------------------*/
const EsAtom* EsContext::makeValueAtom(const EsValue* val)
{
	// ダブル値アトムを作成し、ハッシュテーブルに追加する
	u32 hashkey = _htblValueAtoms->calcHashKey(val);
	EsAtom::EsAtomHashEntry* entity = (EsAtom::EsAtomHashEntry*)_htblValueAtoms->add(val);
	if(entity == 0L)
	{
		ASSERT(false);
		return 0L;
	}
	return entity->getAtom();
}

/*---------------------------------------------------------------------*//**
	プリミティブ値アトムの作成 ⇒ js_AtomizePrimitiveValue
**//*---------------------------------------------------------------------*/
const EsAtom* EsContext::makePrimitiveValueAtom(const EsValue* val)
{
	if(val->isString())
	{
		return makeStringAtom(val->getString());
	}
	else if(val->isDouble())
	{
		f64 fval = val->getDouble();
		return makeDoubleAtom(&fval);
	}
	else
	{
		return makeValueAtom(val);
	}
}

/*---------------------------------------------------------------------*//**
	EsValue からオブジェクトを作成する
	⇒ js_ValueToObject, js_ValueToNonNullObject
**//*---------------------------------------------------------------------*/
bool EsContext::convValueToObject(EsObject** objOut, const EsValue* val)
{
	EsObject* objWk = 0L;

	if(val->isNull() || val->isVoid())
	{
		objWk = 0L;
	}
	else if(val->isObject())
	{
		objWk = val->getObject();

		EsValue valWk;
		if(!objWk->convDefaultValueOv(this, &valWk, LTYPE_OBJ))
		{
			return false;
		}

		if(!valWk.isPrimitive())
		{
			objWk = valWk.getObject();
		}
	}
	else
	{
		EsValue valWk(*val);
		if(!convPrimitiveValueToObject(&valWk))
		{
			return false;
		}
		objWk = valWk.getObject();
	}

	*objOut = objWk;
	return true;
}

/*---------------------------------------------------------------------*//**
	EsValue から関数オブジェクトを作成する ⇒ js_ValueToFunction
**//*---------------------------------------------------------------------*/
bool EsContext::convValueToFunction(EsFunction** funcOut, const EsValue* val, bool isConstructor)
{
	EsObject* objWk = 0L;
	if(val->isObject())
	{
		objWk = val->getObject();
		if((objWk != 0L) && (objWk->getClass()->getClassType() == EsSysClasses::CLASSTYPE_FUNC))
		{
			EsValue valWk;
			if(!objWk->convDefaultValueOv(this, &valWk, LTYPE_FUNC))
			{
				return false;
			}

			if(valWk.isFunctionObject())
			{
				objWk = (EsFunction*)valWk.getObject();
			}
		}
	}

	if(objWk == 0L)
	{
		addRunError(isConstructor ? EsError::ID_NOT_CONSTRUCTOR : EsError::ID_NOT_FUNCTION);	// ⇒ js_ReportIsNotFunction(cx, vp, flags);
		return false;
	}

	ASSERT(objWk->isFunction());
	*funcOut = (EsFunction*)objWk->slots()->privateData();
	return true;
}

/*---------------------------------------------------------------------*//**
	EsValue から文字列 ID を作成する
	⇒ js_InternNonIntElementId, js_ValueToStringId
**//*---------------------------------------------------------------------*/
bool EsContext::convValueToStringId(EsValue* id, const EsValue* val)
{
	const VcString* cstr;
	if(val->isString())
	{
		const EsAtom* atom = findMadeStringAtom(val->getString());
		if(atom != 0L)
		{
			id->setValue(atom);
			return true;
		}
		cstr = val->getString();
	}
	else
	{
		if(!val->toString(&cstr, this))
		{
			return false;
		}
	}

	const EsAtom* atom = makeStringAtom(cstr);
	if(atom == 0L)
	{
		return false;
	}
	id->setValue(atom);
	return true;
}

/*---------------------------------------------------------------------*//**
	EsValue からアクセサ ID を作成する ⇒ JS_ValueToId
**//*---------------------------------------------------------------------*/
bool EsContext::convValueToId(EsValue* id, const EsValue* val)
{
	if(val->isInt())
	{
		id->copy(val);
		return true;
	}
	else
	{
		return convValueToStringId(id, val);
	}
}

/*---------------------------------------------------------------------*//**
	EsValue からイテレータを作成する ⇒ js_ValueToIterator
**//*---------------------------------------------------------------------*/
bool EsContext::convValueToIterator(EsIterator** iter, EsValue* val, u8 flagsIter)
{
	bool isDefaultIter = false;
	
	EsObject* obj;
	if(val->isPrimitive())
	{
		obj = val->getObject();
	}
	else
	{
		if(TFW_IS_FLAG(flagsIter, EsIterator::F_ENUMERATE))
		{
			if(!convValueToObject(&obj, val))	{	return false;	}
			if(obj == 0L)	{	isDefaultIter = true;	}
		}
		else
		{
			if(!convValueToObject(&obj, val))	{	return false;	}
			if(obj == 0L)	{	return false;	}
		}
	}

	if(!isDefaultIter)
	{
		ASSERT(obj != 0L);
		EsValue valAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ITERATOR)->getAtom());	// ⇒ atom = cx->runtime->atomState.iteratorAtom;
		if(!obj->getMethod(val, this, &valAtom))	{	return false;	}
	}

	if(val->isVoid() || isDefaultIter)
	{
		EsIterator* iterNew = newIterator(obj, flagsIter);
		if(iterNew == 0L)	{	return false;	}

		val->setValue(iterNew);
		*iter = iterNew;
		return true;
	}
	else if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR)
	{
		EsValue valArg((bool)!TFW_IS_FLAG(flagsIter, EsIterator::F_FOREACH));
		if(!_runenvCur->invokeInternal(val, obj, val, &valArg, 1, EsStackFrame::F_ITERATOR))
		{
			return false;
		}
		if(val->isPrimitive())
		{
			addRunError(EsError::ID_BAD_ITERATOR_RETURN);
			return false;
		}
		*iter = (EsIterator*)obj;
		return true;
	}

	ASSERT(false);
	*iter = 0L;
	return false;
}

/*---------------------------------------------------------------------*//**
	数値をオブジェクトに変換する ⇒ js_PrimitiveToObject
**//*---------------------------------------------------------------------*/
bool EsContext::convPrimitiveValueToObject(EsValue* val)
{
	const EsClass* cls = 0L;
	switch(val->getType())
	{
	case EsValue::VTYPE_INT:		cls = EsSysClasses::getClass(EsSysClasses::CLASSTYPE_NUMBER);	break;
	case EsValue::VTYPE_DOUBLE:		cls = EsSysClasses::getClass(EsSysClasses::CLASSTYPE_NUMBER);	break;
	case EsValue::VTYPE_STRING:		cls = EsSysClasses::getClass(EsSysClasses::CLASSTYPE_STRING);	break;
	case EsValue::VTYPE_BOOLEAN:	cls = EsSysClasses::getClass(EsSysClasses::CLASSTYPE_BOOLEAN);	break;
	}
	ASSERT(cls != 0L);
	if(cls == 0L)	{	return false;	}

	EsObject* obj = newObject(cls, 0L, 0L);
	if(obj == 0L)	{	return false;	}

	obj->slots()->setPrivateValue(val);
	val->setValue(obj);
	return true;
}

/*---------------------------------------------------------------------*//**
	ユニークなシェイプ ID を生成する ⇒ js_GenerateShape
**//*---------------------------------------------------------------------*/
u32 EsContext::makeUniqueShapeId()
{
	ASSERT(_gencntShape <= U32_MAX);
	return ++_gencntShape;
}

/*---------------------------------------------------------------------*//**
	型が指定クラスもしくはサブクラスであるか判定 ⇒ JS_InstanceOf
**//*---------------------------------------------------------------------*/
bool EsContext::instanceOf(EsObject* obj, const EsClass* cls, const EsValue* val)
{
	if((obj != 0L) && (obj->getClass() == cls))
	{
		return true;
	}
	if(val != 0L)
	{
		EsFunction* func = 0L;
		convValueToFunction(&func, val, false);
		if(func != 0L)
		{
			addRunError(EsError::ID_INCOMPATIBLE_PROTO);
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	実行時エラーを追加する
**//*---------------------------------------------------------------------*/
void EsContext::addRunError(s32 id, bool isWarning, bool isStrict)
{
	_err->addRunError(this, (EsError::Id)id, isWarning, isStrict);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsContext::EsContext()
	: _nodefct(0L)
	, _err(0L)
	, _code(0L)
	, _htblStringAtoms(0L)
	, _htblDoubleAtoms(0L)
	, _htblValueAtoms(0L)
	, _resolv(0L)
	, _mflags(0)
	, _listDelUnit(0L)
	, _listDelNode(0L)
	, _listDelString(0L)
	, _kwsetRef(0L)
	, _objGlobalRef(0L)
	, _extextRef(0L)
	, _runenvCur(0L)
	, _rsCur(0L)
	, _arrSfrmDisp(0L)
	, _valRet2nd(0L)
	, _isRet2ndValSet(false)
	, _isStop(false)
	, _gencntShape(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsContext::~EsContext()
{
	ASSERT(_nodefct == 0L);
	ASSERT(_err == 0L);
	ASSERT(_code == 0L);
	ASSERT(_htblStringAtoms == 0L);
	ASSERT(_htblDoubleAtoms == 0L);
	ASSERT(_htblValueAtoms == 0L);
	ASSERT(_resolv == 0L);
	ASSERT(_listDelUnit == 0L);
	ASSERT(_listDelNode == 0L);
	ASSERT(_listDelString == 0L);
	ASSERT(_runenvCur == 0L);
	ASSERT(_rsCur == 0L);
	ASSERT(_arrSfrmDisp == 0L);
	ASSERT(_valRet2nd == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EsContext::create()
{
	// 各モジュールのインスタンス作成
	_nodefct = new EsItNodeFactory();
	_err = new EsError();
	_code = new VcString();
	_htblStringAtoms = new HashTable(new EsString::EsStringAtomHashOp(), 1024);
	_htblDoubleAtoms = new HashTable(new EsNumber::EsF64AtomHashOp(), 512);
	_htblValueAtoms = new HashTable(new EsValue::EsValueAtomHashOp(), 512);
	_resolv = new EsResolver();
	_listDelUnit = new List<EsUnit*>(true);
	_listDelNode = new List<EsItNode*>(true);
	_listDelString = new List<VcString*>(true);
	_arrSfrmDisp = new Array<EsStackFrame*>;
	_valRet2nd = new EsValue();

	if(!_arrSfrmDisp->alloc(SIZE_SF_DISPLAY))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EsContext::destroy()
{
	// 各モジュールのインスタンス削除
	delete _valRet2nd;
	_valRet2nd = 0L;
	delete _arrSfrmDisp;
	_arrSfrmDisp = 0L;
	delete _runenvCur;
	_runenvCur = 0L;
	delete _rsCur;
	_rsCur = 0L;
	delete _listDelString;
	_listDelString = 0L;
	delete _listDelNode;
	_listDelNode = 0L;
	delete _listDelUnit;
	_listDelUnit = 0L;
	delete _resolv;
	_resolv = 0L;
	delete _htblValueAtoms;
	_htblValueAtoms = 0L;
	delete _htblDoubleAtoms;
	_htblDoubleAtoms = 0L;
	delete _htblStringAtoms;
	_htblStringAtoms = 0L;
	delete _code;
	_code = 0L;
	delete _err;
	_err = 0L;
	delete _nodefct;
	_nodefct = 0L;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void EsContext::reset()
{
	_arrSfrmDisp->removeAll();
	_listDelString->removeAll();
	_listDelNode->removeAll();
	_listDelUnit->removeAll();
	_htblValueAtoms->reset();
	_htblDoubleAtoms->reset();
	_htblStringAtoms->reset();
	_err->clear();
	_code->emptyEasy();
	if(_extextRef != 0L)	{	_extextRef->reset();	}

	_objGlobalRef = 0L;
	_valRet2nd->setNull();
	_isRet2ndValSet = false;
	_isStop = false;

	delete _runenvCur;
	_runenvCur = 0L;
	delete _rsCur;
	_rsCur = 0L;

	_arrSfrmDisp->alloc(SIZE_SF_DISPLAY);
}

//==========================================================================
// EsContext メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsContext::RunStack::RunStack(EsRunEnv* runenv)
	: _rsUp(0L)
	, _runenv(0L)
	, _il(0L)
	, _sfrm(0L)
{
	if(runenv != 0L)
	{
		_runenv = new EsRunEnv();
		_runenv->copy(runenv);
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsContext::RunStack::~RunStack()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EsContext::RunStack::clear()
{
	if(_rsUp != 0L)
	{
		_rsUp->clear();
		delete _rsUp;
		_rsUp = 0L;
	}
	
	if(_runenv != 0L)
	{
		delete _runenv;
		_runenv = 0L;
	}

	_il = 0L;
	_sfrm = 0L;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
