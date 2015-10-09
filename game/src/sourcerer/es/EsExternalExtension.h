/***********************************************************************//**
 *	EsExternalExtension.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/02/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_EXTERNAL_EXTENSION_H_
#define _ES_ES_EXTERNAL_EXTENSION_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsExtParam.h"

#include "../../tfw/string/CcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class VcString;

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	外部拡張モジュール
 *
**//*---------------------------------------------------------------------*/
class EsExternalExtension
{
	//======================================================================
	// 定数
protected:

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	外部拡張ネイティブ関数
	 *
	**//*-----------------------------------------------------------------*/
	class EsExtNativeFunc
	{
	public:
		inline EsNativeCall		getFunc() const		{	return _func;			}
		inline const VcString*	getName() const		{	return _name;			}
		inline EsExtParam		getDefParam() const	{	return _paramFuncDef;	}

	public:
		EsExtNativeFunc(EsNativeCall func, const CStringBase* name, EsExtParam paramDef);
		EsExtNativeFunc(EsNativeCall func, CcString name, EsExtParam paramDef);
		~EsExtNativeFunc();

	private:
		EsNativeCall	_func;
		VcString*		_name;
		EsExtParam		_paramFuncDef;
	};

	/*-----------------------------------------------------------------*//**
	 *	スクリプト内の関数解析情報
	 *
	**//*-----------------------------------------------------------------*/
	class EsParsedFuncInfo
	{
		//==================================================================
		// 定数
	public:
		enum Kind
		{
			KIND_NULL	= 0x00000000,
			KIND_CALL	= 0x00000001,
			KIND_DEF	= 0x00000002,
			MASK_KIND	= 0x000000ff,
		};
		static const u32 F_UNRESOLV	= 0x00000100;

		//==================================================================
		// メソッド
	public:
		const VcString* getFuncName() const { return _nameFunc; }
		Kind getKind() const { return (Kind)(_flags & MASK_KIND); }
		bool isUnresolved() const { return TFW_IS_FLAG(_flags, F_UNRESOLV); }
		void setUnresolved(bool isUnresolved) { TFW_SET_FLAG(_flags, F_UNRESOLV, isUnresolved); }
	public:
		EsParsedFuncInfo(const VcString* nameFunc, Kind kind);
		~EsParsedFuncInfo();

		//==================================================================
		// 変数
	private:
		const VcString* _nameFunc;
		u32 _flags;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 拡張ネイティブ関数の数を得る
	s32	getExtNativeFuncNum() const;
	// 拡張ネイティブ関数を得る
	const EsExtNativeFunc* getExtNativeFunc(s32 index) const;

	// 関数解析情報の数を得る
	s32	getParsedFuncInfoNum() const;
	// 関数解析情報を得る
	const EsParsedFuncInfo* getParsedFuncInfo(s32 index) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 addExtNativeFunc(EsExtNativeFunc* efEntr);
	bool removeExtNativeFunc(s32 index);

	void addParsedFuncInfo(const VcString* nameFunc, bool isDef);

	virtual bool resolveUndefinedFunctionByExtFunc(VcString* codeExtScriptOut, EsExtParam paramParse);

protected:
	virtual bool resolveUndefinedFunction(VcString* codeExtScriptOut, const VcString* funcnameUndefined, EsExtParam paramParse);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsExternalExtension();
	virtual ~EsExternalExtension();
	void reset();

	//======================================================================
	// 変数
protected:
	List<EsExtNativeFunc*>* _listExtNativeFunc;
	List<EsParsedFuncInfo*>* _listParsedFuncInfo;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_EXTERNAL_EXTENSION_H_
