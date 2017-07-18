/***********************************************************************//**
 *	EsExternalExtension.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/02/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsExternalExtension.h"

// Friends

// External
#include "../tfw/string/VcString.h"
#include "../tfw/collection/List.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsExternalExtension メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	拡張ネイティブ関数の数を得る
**//*---------------------------------------------------------------------*/
s32 EsExternalExtension::getExtNativeFuncNum() const
{
	if(_listExtNativeFunc == 0L)	{	return 0;	}
	return _listExtNativeFunc->getCount();
}

/*---------------------------------------------------------------------*//**
	拡張ネイティブ関数を得る
**//*---------------------------------------------------------------------*/
const EsExternalExtension::EsExtNativeFunc* EsExternalExtension::getExtNativeFunc(s32 index) const
{
	if(_listExtNativeFunc == 0L)										{	return 0L;	}
	if((index < 0) && (_listExtNativeFunc->getCount() <= (u32)index))	{	return 0L;	}
	return _listExtNativeFunc->getObject(index);
}

/*---------------------------------------------------------------------*//**
	関数解析情報の数を得る
**//*---------------------------------------------------------------------*/
s32	EsExternalExtension::getParsedFuncInfoNum() const
{
	if(_listParsedFuncInfo == 0L)	{	return 0;	}
	return _listParsedFuncInfo->getCount();
}

/*---------------------------------------------------------------------*//**
	関数解析情報を得る
**//*---------------------------------------------------------------------*/
const EsExternalExtension::EsParsedFuncInfo* EsExternalExtension::getParsedFuncInfo(s32 index) const
{
	if(_listParsedFuncInfo == 0L)										{	return 0L;	}
	if((index < 0) && (_listParsedFuncInfo->getCount() <= (u32)index))	{	return 0L;	}
	return _listParsedFuncInfo->getObject(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	拡張ネイティブ関数追加
**//*---------------------------------------------------------------------*/
s32 EsExternalExtension::addExtNativeFunc(EsExtNativeFunc* efEntr)
{
	if(_listExtNativeFunc == 0L)	{	return -1;	}
	_listExtNativeFunc->addTail(efEntr);
	return _listExtNativeFunc->getCount() - 1;
}

/*---------------------------------------------------------------------*//**
	拡張ネイティブ関数削除
**//*---------------------------------------------------------------------*/
bool EsExternalExtension::removeExtNativeFunc(s32 index)
{
	if(_listExtNativeFunc == 0L)	{	return false;	}
	ListNode<EsExtNativeFunc*>* ef = _listExtNativeFunc->node(index);
	if(ef == 0L)			{	return false;	}
	_listExtNativeFunc->removeNode(ef);
	return true;
}

/*---------------------------------------------------------------------*//**
	解析関数情報追加
**//*---------------------------------------------------------------------*/
void EsExternalExtension::addParsedFuncInfo(const VcString* nameFunc, bool isDef)
{
	if(_listParsedFuncInfo == 0L)	{	return;	}

	// 入力された関数の種類
	EsParsedFuncInfo::Kind kind = isDef ? EsParsedFuncInfo::KIND_DEF : EsParsedFuncInfo::KIND_CALL;

	// 重複チェック
	for(ListIterator<EsParsedFuncInfo*> it = _listParsedFuncInfo->iterator(); it.has(); it.next())
	{
		EsParsedFuncInfo* pfi = it.object();
		if(	(pfi->getKind() == kind) &&
			(pfi->getFuncName()->equals(nameFunc))	)
		{
			return;	// 重複
		}
	}

	// リストに追加する
	EsParsedFuncInfo* pfi = new EsParsedFuncInfo(nameFunc, kind);
	_listParsedFuncInfo->addTail(pfi);
}

/*---------------------------------------------------------------------*//**
	未定義の関数を外部関数によって解決を試みる

	@param codeExtScriptOut	追加スクリプトコード（戻り値 false の場合有効）
	@param paramParse		解析汎用パラメータ
	@retval true	解析を終了してよし
					※	外部拡張ネイティブ関数追加によって解決した場合も
						この戻り値でよい
	@retval false	解析を続ける
					※	codeExtScriptOut に該当関数のスクリプトコードを
						設定して、解決を試みる
**//*---------------------------------------------------------------------*/
bool EsExternalExtension::resolveUndefinedFunctionByExtFunc(VcString* codeExtScriptOut, EsExtParam paramParse)
{
	// 関数呼び出しが解決しているか調べる
	s32 numUnresolvedFuncCall = 0;
	for(ListIterator<EsParsedFuncInfo*> itCall = _listParsedFuncInfo->iterator(); itCall.has(); itCall.next())
	{
		if(itCall.object()->getKind() != EsParsedFuncInfo::KIND_CALL)	{	continue;	}

		const VcString* nameCall = itCall.object()->getFuncName();
		bool isDefFind = false;

		// スクリプト内定義関数から探す
		for(ListIterator<EsParsedFuncInfo*> itDef = _listParsedFuncInfo->iterator(); itDef.has(); itDef.next())
		{
			if(itDef.object()->getKind() != EsParsedFuncInfo::KIND_DEF)	{	continue;	}

			if(nameCall->equals(itDef.object()->getFuncName()))
			{
				isDefFind = true;
				break;
			}
		}
		
		if(!isDefFind)
		{
			// 既に追加済みの外部拡張ネイティブ関数から探す
			for(ListIterator<EsExtNativeFunc*> itEnf = _listExtNativeFunc->iterator(); itEnf.has(); itEnf.next())
			{
				if(nameCall->equals(itEnf.object()->getName()))
				{
					isDefFind = true;
					break;
				}
			}
		}

		itCall.object()->setUnresolved(!isDefFind);	// 未解決フラグを立てる
		if(!isDefFind) { numUnresolvedFuncCall++; }
	}

	if(numUnresolvedFuncCall <= 0)	// 未解決の関数呼び出しはない
	{
		return true;	// 解析を終わって問題ない
	}

	// 外部のスクリプト追加、もしくは外部拡張ネイティブ関数追加で解決を試みる
	bool isEndParse = true;	// 特に処理が入らなければ解決を終了する
	for(ListIterator<EsParsedFuncInfo*> itCall = _listParsedFuncInfo->iterator(); itCall.has(); itCall.next())
	{
		if(itCall.object()->getKind() != EsParsedFuncInfo::KIND_CALL)	{	continue;	}
		if(!itCall.object()->isUnresolved())	{	continue;	}
		if(!resolveUndefinedFunction(codeExtScriptOut, itCall.object()->getFuncName(), paramParse))
		{
			isEndParse = false;
		}
	}

	return isEndParse;
}

/*---------------------------------------------------------------------*//**
	未定義の関数の解決を試みる

	@param codeExtScriptOut	追加スクリプトコード（戻り値 false の場合有効）
	@param paramParse		解析汎用パラメータ
	@retval true	解析を終了してよし
					※	外部拡張ネイティブ関数新規追加によって解決した場合も
						この戻り値でよい
	@retval false	解析を続ける
					※	codeExtScriptOut に該当関数のスクリプトコードを
						設定して、解決を試みる
**//*---------------------------------------------------------------------*/
bool EsExternalExtension::resolveUndefinedFunction(VcString* codeExtScriptOut, const VcString* funcnameUndefined, EsExtParam paramParse)
{
	// 外部スクリプト追加は派生クラスによってなされる
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::EsExternalExtension()
	: _listExtNativeFunc(new List<EsExtNativeFunc*>(true))
	, _listParsedFuncInfo(new List<EsParsedFuncInfo*>(true))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::~EsExternalExtension()
{
	delete _listParsedFuncInfo;
	delete _listExtNativeFunc;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void EsExternalExtension::reset()
{
	_listParsedFuncInfo->removeAll();
}

//==========================================================================
// EsExternalExtension::EsExtNativeFunc メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::EsExtNativeFunc::EsExtNativeFunc(EsNativeCall func, const CStringBase* name, EsExtParam paramDef)
	: _func(func)
	, _name(new VcString(name))
	, _paramFuncDef(paramDef)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::EsExtNativeFunc::EsExtNativeFunc(EsNativeCall func, CcString name, EsExtParam paramDef)
	: _func(func)
	, _name(new VcString(&name))
	, _paramFuncDef(paramDef)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::EsExtNativeFunc::~EsExtNativeFunc()
{
	delete _name;
}

//==========================================================================
// EsExternalExtension::EsParsedFuncInfo メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::EsParsedFuncInfo::EsParsedFuncInfo(const VcString* nameFunc, Kind kind)
	: _nameFunc(new VcString(nameFunc))
	, _flags(kind)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsExternalExtension::EsParsedFuncInfo::~EsParsedFuncInfo()
{
	delete _nameFunc;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
