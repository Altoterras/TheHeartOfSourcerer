/***********************************************************************//**
 *	EsBlock.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/09/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsBlock.h"

// Friends
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsInterlang.h"
#include "EsObject.h"
#include "EsScope.h"
#include "EsStackFrame.h"
#include "EsValue.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsBlock メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	複製 ⇒ js_CloneBlockObject
**//*---------------------------------------------------------------------*/
EsBlock* EsBlock::clone(EsContext* ctx, EsObject* objParent, EsStackFrame* sfrm)
{
	ASSERT(getObjType() == OBJT_BLOCK);
	ASSERT(getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK);
	ASSERT(!this->isCloned());
	EsBlock* blockClone = (EsBlock*)ctx->newObject(EsBlockClass::getInstance(), this, objParent);
	if(blockClone == 0L)
	{
		return 0L;
	}
	blockClone->slots()->setPrivateData(sfrm);
	blockClone->slots()->setValue(getClass()->getBlockDepthSlotIndex(), this->getSlots()->getValue(getClass()->getBlockDepthSlotIndex()));
	ASSERT(blockClone->isCloned());
	ASSERT(blockClone->getScope()->getOwnerObject() == this);
	return blockClone;
}

/*---------------------------------------------------------------------*//**
	複製されたブロックである ⇒ OBJ_IS_CLONED_BLOCK
**//*---------------------------------------------------------------------*/
bool EsBlock::isCloned() const
{
	ASSERT(getObjType() == OBJT_BLOCK);
	ASSERT(getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK);

	return getScope()->getOwnerObject() != this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsBlock::EsBlock()
	: EsObject(EsBlockClass::getInstance(), OBJT_BLOCK)
{
}


//==========================================================================
// EsBlock::EsBlockClass メソッド

/*---------------------------------------------------------------------*//**
	追加の予約スロット数を得る ⇒ block_reserveSlots
**//*---------------------------------------------------------------------*/
bool EsBlock::EsBlockClass::getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const
{
	*slotnumOut = ((EsBlock*)obj)->isCloned() ? obj->getScope()->getScopePropertyNum() : 0;	// ⇒ OBJ_IS_CLONED_BLOCK(obj) ? OBJ_BLOCK_COUNT(cx, obj) : 0;
	return true;
}

/*---------------------------------------------------------------------*//**
	ブロックオブジェクトクラスのコンストラクタ ⇒ js_BlockClass
**//*---------------------------------------------------------------------*/
EsBlock::EsBlockClass::EsBlockClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_BLOCK)->getAtom()
		, EsSysClasses::CLASSTYPE_BLOCK
		, EsClass::F_HAS_PRIVATE | EsClass::F_IS_ANONYMOUS
		, EsSysPrototypes::PTYPEKIND_NULL
		, 1 )
{
	_funcGetProp = callGetProperty;
	_funcSetProp = callSetProperty;
}

/*---------------------------------------------------------------------*//**
	アクセサ取得 ⇒ block_getProperty
**//*---------------------------------------------------------------------*/
bool EsBlock::EsBlockClass::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(!id->isInt())
	{
		return true;
	}

	s32 idx = id->getInt();

	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm != 0L)
	{
		idx += sfrm->getInterlang()->getFixedSlotNum() + obj->getSlots()->getBlockDepth();
		val->copy(sfrm->getSlot(idx));
		return true;
	}

	idx += getInstance()->getBlockDepthSlotIndex() + getInstance()->getParentSlotIndex();
	return obj->getSlots()->getReservedSlotValue(val, ctx, idx);
}

/*---------------------------------------------------------------------*//**
	アクセサ設定 ⇒ block_setProperty
**//*---------------------------------------------------------------------*/
bool EsBlock::EsBlockClass::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	if(!id->isInt())
	{
		return true;
	}

	s32 idx = id->getInt();

	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm != 0L)
	{
		idx += sfrm->getInterlang()->getFixedSlotNum() + obj->getSlots()->getBlockDepth();
		sfrm->setSlot(idx, val);
		return true;
	}

	idx += getInstance()->getBlockDepthSlotIndex() + getInstance()->getParentSlotIndex();
	return obj->slots()->setReservedSlotValue(ctx, idx, val);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
