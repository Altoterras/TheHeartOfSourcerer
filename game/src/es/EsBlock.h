/***********************************************************************//**
 *	EsBlock.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/09/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_BLOCK_H_
#define _ES_ES_BLOCK_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsObject.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

class EsContext;
class EsStackFrame;

/*---------------------------------------------------------------------*//**
 *	ブロック
 *
**//*---------------------------------------------------------------------*/
class EsBlock : public EsObject
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	ブロック スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsBlockClass : public EsClass
	{
	public:
		virtual bool getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const;

		static const EsBlockClass* getInstance() { static EsBlockClass instance; return &instance; }

	private:
		EsBlockClass();
		static bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsBlock* clone(EsContext* ctx, EsObject* objParent, EsStackFrame* sfrm);
	bool isCloned() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsBlock();
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_BLOCK_H_
