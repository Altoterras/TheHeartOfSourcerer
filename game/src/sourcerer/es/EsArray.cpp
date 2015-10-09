/***********************************************************************//**
 *	EsArray.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/07/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsArray.h"

// Friends
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsDefaultObjectOp.h"
#include "EsResolver.h"
#include "EsRunEnv.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsTokenGetter.h"
#include "EsValue.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/BitFlagArray.h"
#include "../../tfw/lib/MergeSort.h"
#include "../../tfw/string/StringUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	配列容量数を得る ⇒ js_DenseArrayCapacity
**//*---------------------------------------------------------------------*/
s32 EsArray::getIntIndexedCapacity() const
{
	const EsValue* val = getSlots()->getValue(EsArrayClass::getInstance()->getArrayCapacitySlotIndex());
	ASSERT(val->isVoid() || val->isInt());
	return val->isInt() ? val->getInt() : 0;
}

/*---------------------------------------------------------------------*//**
	配列容量数を設定する ⇒ js_SetDenseArrayCapacity
**//*---------------------------------------------------------------------*/
void EsArray::setIntIndexedCapacity(s32 size)
{
	EsValue* val = slots()->value(EsArrayClass::getInstance()->getArrayCapacitySlotIndex());
	val->setValue(size);
}

/*---------------------------------------------------------------------*//**
	実行時の要素ゲット
	⇒ js_Interpret BEGIN_CASE(JSOP_GETELEM) OBJ_IS_DENSE_ARRAY(cx, obj) ～
**//*---------------------------------------------------------------------*/
bool EsArray::getElementForRunOp(EsValue* val, s32 index)
{
	// 配列容量を得る
	s32 capacity = getIntIndexedCapacity();

	// 配列長さを得る
	s32 length = this->getLengthValueFromSlot();

	// インデックスが配列内であれば値を取得
	if((index < capacity) && (index < length))	// ⇒ if ((jsuint)i < length && i < obj->fslots[JSSLOT_ARRAY_LENGTH])
	{
		val->copy(getSlots()->getDvalue(index));	// ⇒ rval = obj->dslots[i];
		if(!val->isHole())	// ⇒ if (rval != JSVAL_HOLE)
		{
			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	実行時の要素セット
	⇒ js_Interpret BEGIN_CASE(JSOP_SETELEM) OBJ_IS_DENSE_ARRAY(cx, obj) ～
**//*---------------------------------------------------------------------*/
bool EsArray::setElementForRunOp(s32 index, const EsValue* val)
{
	// 配列容量を得る
	s32 capacity = getIntIndexedCapacity();

	// インデックスが配列内であれば値を設定
	if(index < capacity)
	{
		const EsValue* val = getSlots()->getDvalue(index);	// ⇒ rval = obj->dslots[i];
		if(val->isHole())	// ⇒ if (obj->dslots[i] == JSVAL_HOLE)
		{
			if(isPrototypeHasIndexedPropeties())
			{
				return false;
			}

			// 配列長さが足りなければ伸ばす
			EsValue* valLength;
			s32 length = getLengthValueFromSlot(&valLength);
			if(index >= length)
			{
				valLength->setValue(length + 1);
			}

			// 配列使用数を増やす
			EsValue* valCount;
			s32 count = getCountValueFromSlot(&valCount);
			valCount->setValue(count + 1);
		}

		slots()->dvalue(index)->copy(val);
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	プロトタイプがインデックスプロパティを持っているかどうか
	⇒ js_PrototypeHasIndexedProperties
**//*---------------------------------------------------------------------*/
bool EsArray::isPrototypeHasIndexedPropeties() const
{
	const EsObject* obj = this;
	while(true)
	{
		obj = obj->getSlots()->getPrototypeObject();
		if(obj == 0L)	{	break;	}

		if(!obj->isNative())
		{
			return true;
		}
		if(TFW_IS_FLAG(obj->getScope()->getScopeFlags(), EsScope::SCOPEF_INDEXED_PROPERTIES))
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	整数インデックス配列かを得る ⇒ OBJ_IS_DENSE_ARRAY
**//*---------------------------------------------------------------------*/
bool EsArray::isIntIndexedArray(const EsObject* obj)
{
	if(obj->getObjType() != OBJT_ARRAY)
	{
		return false;
	}
	return ((EsArray*)obj)->_isIntIndexed;
}

/*---------------------------------------------------------------------*//**
	要素を取得する ⇒ GetArrayElement
**//*---------------------------------------------------------------------*/
bool EsArray::getElement(EsValue* val, bool* isHole, EsContext* ctx, EsObject* obj, f64 index)
{
	// 整数インデックス配列の取得処理
	if(isIntIndexedArray(obj))	//  ⇒ OBJ_IS_DENSE_ARRAY
	{
		s32 iidx = (s32)index;
		if(iidx < ((EsArray*)obj)->getIntIndexedCapacity())
		{
			val->copy(obj->getSlots()->getDvalue(iidx));
			if(isHole != 0L)	{	*isHole = val->isHole();	}
			return true;
		}
	}

	// 非整数インデックス配列処理の取得処理
	EsValue id;
	id.setNumber(index);	// ⇒ IndexToId
	EsScopeProperty* sprop;
	EsObject* objScope;
	if(!obj->findPropertyOv(&sprop, &objScope, ctx, &id))
	{
		return false;
	}
	if(sprop != 0L)
	{
		val->setVoid();
		if(isHole != 0L)	{	*isHole = true;	}
	}
	else
	{
		obj->dropPropertyOv(ctx, sprop);
		if(!obj->callGetPropertyOv(val, ctx, &id))
		{
			return false;
		}
		if(isHole != 0L)	{	*isHole = false;	}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	要素を設定する ⇒ SetArrayElement
**//*---------------------------------------------------------------------*/
bool EsArray::setElement(EsContext* ctx, EsObject* obj, f64 index, const EsValue* val)
{
	// 整数インデックス配列のセット処理
	if(isIntIndexedArray(obj))	//  ⇒ OBJ_IS_DENSE_ARRAY
	{
		EsArray* arr = (EsArray*)obj;
		s32 iidx = (s32)index;
		if((iidx < arr->getIntIndexedCapacity()) && (iidx <= INDEX_INTIDX_MAX))	// ⇒ !INDEX_TOO_SPARSE(obj, idx)
		{
			// 配列容量の決定
			if(!arr->ensureCapacity(iidx + 1))
			{
				return false;
			}
			// 配列長さの決定
			EsValue* valLength;
			s32 length = arr->getLengthValueFromSlot(&valLength);
			if(iidx >= length)
			{
				valLength->setValue(length + 1);
			}
			// 要素数の決定
			EsValue* valElm = arr->slots()->dvalue(iidx);
			if(valElm->isHole())
			{
				EsValue* valCount;
				s32 count = arr->getCountValueFromSlot(&valCount);
				valCount->setValue(count + 1);
			}
			// 要素値セット
			valElm->copy(val);
			return true;
		}

		// 非整数インデックス配列に転換
		if(!arr->changeNoIntIndexedArray(ctx))
		{
			return false;
		}
	}

	// 非整数インデックス配列処理のセット処理
	EsValue id;
	id.setNumber(index);	// ⇒ IndexToId
	return obj->callSetPropertyOv(ctx, &id, val);
}

/*---------------------------------------------------------------------*//**
	要素を削除する ⇒ DeleteArrayElement
**//*---------------------------------------------------------------------*/
bool EsArray::deleteElement(EsContext* ctx, EsObject* obj, f64 index)
{
	if(isIntIndexedArray(obj))	//  ⇒ OBJ_IS_DENSE_ARRAY
	{
		s32 iidx = (s32)index;
		if((iidx < ((EsArray*)obj)->getIntIndexedCapacity()) && (iidx <= INDEX_INTIDX_MAX))	// ⇒ !INDEX_TOO_SPARSE(obj, idx)
		{
			EsValue* val = obj->slots()->dvalue(iidx);
			if(!val->isHole())
			{
				val->setHole();
			}
		}
		return true;
	}

	EsValue id;
	id.setNumber(index);	// ⇒ IndexToId

	EsValue valWk;
	return obj->deletePropertyOv(&valWk, ctx, &id);
}

/*---------------------------------------------------------------------*//**
	オブジェクトのプロパティから配列長さを得る ⇒ js_GetLengthProperty
**//*---------------------------------------------------------------------*/
bool EsArray::getArrayLengthProperty(s32* lengthOut, EsContext* ctx, EsObject* obj)
{
	if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)
	{
		*lengthOut = getLengthValueFromSlot(obj);
		return true;
	}

	EsValue id(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	EsValue valLen;
	if(obj->callGetPropertyOv(&valLen, ctx, &id))
	{
		if(valLen.isInt())
		{
			*lengthOut = valLen.getInt();
			return true;
		}
		else
		{
			u32 lenWk;
			if(valLen.toUint32(&lenWk, ctx))	// ⇒ *lengthp = js_ValueToECMAUint32(cx, &tvr.u.value);
			{
				*lengthOut = (s32)lenWk;
				return true;
			}
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	長さプロパティを設定する ⇒ js_SetLengthProperty
**//*---------------------------------------------------------------------*/
bool EsArray::setArrayLengthProperty(EsContext* ctx, EsObject* obj, f64 length)
{
	EsValue val;
	val.setNumber(length);	// ⇒ IndexToValue(cx, length, &v)
	EsValue id(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	return obj->callSetPropertyOv(ctx, &id, &val);
}

/*---------------------------------------------------------------------*//**
	EsValue から配列長さを得る ⇒ ValueIsLength
**//*---------------------------------------------------------------------*/
bool EsArray::getArrayLengthFromValue(s32* lengthOut, EsContext* ctx, const EsValue* val)
{
	f64 fval;
	u32 uival;

	if(val->isInt())
	{
		s32 ival = val->getInt();
		if(ival < 0)
		{
			goto FAILED;
		}
		*lengthOut = ival;
		return true;
	}

	val->toNumber(&fval, 0L);
	if(val->isNull())
	{
		goto FAILED;
	}

	if(TFW_F64_IS_NAN(fval))
	{
		goto FAILED;
	}

	uival = (u32)fval;
	if(fval != (f64)uival)
	{
		goto FAILED;
	}

	*lengthOut = (s32)uival;
	return true;

FAILED:
	ctx->addRunError(EsError::ID_BAD_ARRAY_LENGTH);
	*lengthOut = 0;
	return false;
}

/*---------------------------------------------------------------------*//**
	配列指定子がインデックスかどうかを判定する ⇒ js_IdIsIndex
**//*---------------------------------------------------------------------*/
bool EsArray::isArrayIndexId(s32* index, const EsValue* id)
{
	if(id->isInt())
	{
		s32 ival = id->getInt();
		if(ival < 0)
		{
			return false;
		}
		*index = ival;
		return true;
	}

	if(!id->isString())
	{
		return false;
	}

	// 文字列から数値に変換する（簡易）
	const VcString* str = id->getString();
	if(str->getLength() <= 0)
	{
		return false;
	}
	s32 idxStr = 0;
	s32 indexWk = 0;
	s32 indexWkPrev = 0;
	while(true)
	{
		char c = str->getAt(idxStr);
		if(!EsTokenGetter::Cu::isDecimal(c))	{	break;	}
		s32 ival = EsTokenGetter::Cu::toDecimal(c);
		indexWkPrev = indexWk;
		indexWk = (indexWk * 10) + ival;
		idxStr++;
	}
	if(indexWkPrev > indexWk)	// オーバーフローチェック
	{
		return false;
	}
	*index = indexWk;
	return true;
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ array_enumerate
**//*---------------------------------------------------------------------*/
bool EsArray::enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop)
{
	EsArrayStat* stat = reinterpret_cast<EsArrayStat*>(valState->getInt64());

	switch(enumop)
	{
	case ENUMOP_START:
		// 開始処理
		if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)
		{
			EsArray* arr = (EsArray*)obj;
			s32 capacity = arr->getIntIndexedCapacity();

			// 状態変数が未作成の場合は作成する
			if(stat == 0L)
			{
				stat = new EsArrayStat(capacity);
				if(stat == 0L) { return false; }
				arr->_listPreventLeak->addTail(stat);	// メモリリーク防止リストから追加する
				if(stat->_bfarr == 0L) { return false; }
			}

			// カウントを保存する
			if(id != 0L)
			{
				s32 count = getCountValueFromSlot(obj);
				id->setNumber(count);
			}

			// 穴を調べる
			for(int i = 0; i < capacity; i++)
			{
				const EsValue* val = arr->getSlots()->getDvalue(i);
				if(val->isHole())	// 穴
				{
					stat->_bfarr->setFlag((u16)i, true);
				}
			}

			// ステイトに保存する
			valState->setValue(reinterpret_cast<u64>(stat));
		}
		else
		{
			ASSERT(false);
		}
		break;

	case ENUMOP_NEXT:
		ASSERT(valState->isInt64());
		if(stat->_index >= stat->_capacity)	// 終わりまで来た
		{
			goto END;
		}
		if(stat->_bfarr != 0L)	// 穴がある
		{
			while(stat->_bfarr->getFlag((u16)stat->_index) && (stat->_index < stat->_capacity))
			{
				stat->_index++;
			}
			if(stat->_index >= stat->_capacity)	// 終わりまで来た
			{
				goto END;
			}
		}
		// ID とステイトに保存する
		id->setValue(stat->_index);
		break;

	case ENUMOP_END: END:
		// 終了処理
		ASSERT(valState->isInt64());
		delete stat;
		// メモリリーク防止リストから削除する
		if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)
		{
			EsArray* arr = (EsArray*)obj;
			arr->_listPreventLeak->remove(stat);
		}
		valState->setNull();
		break;
	}
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsArray::EsArray()
	: EsObject(EsArrayClass::getInstance(), OBJT_ARRAY)
	, _isIntIndexed(true)
	, _listPreventLeak(0L)
{
	// EsArrayStat メモリリーク防止リスト作成
	_listPreventLeak = new List<EsArrayStat*>();
}


/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsArray::~EsArray()
{
	// EsArrayStat メモリリーク防止リストからの削除
	for(ListIterator<EsArrayStat*> it = _listPreventLeak->iterator(); it.has(); it.next())
	{
		delete it.object();
	}
	delete _listPreventLeak;
}

/*---------------------------------------------------------------------*//**
	作成 ⇒ InitArrayObject
**//*---------------------------------------------------------------------*/
bool EsArray::create(EsContext* ctx, EsValue* valarr, s32 lenArr)
{
	// 容量を確定し、要素数をカウントする
	s32 count = 0;
	if(valarr != 0L)
	{
		if(!ensureCapacity(lenArr))
		{
			return false;
		}

		count = lenArr;
		for(int i = 0; i < lenArr; i++)
		{
			if(valarr[i].isHole())
			{
				count--;
			}
			slots()->dvalue(i)->copy(&valarr[i]);
		}
	}

	// 配列長と要素数を設定する
	EsValue valLenArr(lenArr);
	slots()->setValue(EsArrayClass::getInstance()->getArrayLengthSlotIndex(), &valLenArr);
	EsValue valCount(count);
	slots()->setValue(EsArrayClass::getInstance()->getArrayCountSlotIndex(), &valCount);

	return true;
}

/*---------------------------------------------------------------------*//**
	要素を設定する ⇒ InitArrayElements
**//*---------------------------------------------------------------------*/
bool EsArray::setArrayElements(EsContext* ctx, EsObject* obj, EsValue* valarr, s32 indexStart, s32 count, bool isAllHoles)
{
	s32 length = indexStart + count;

	if(isIntIndexedArray(obj))
	{
		EsArray* arr = (EsArray*)obj;

		if(!arr->isPrototypeHasIndexedPropeties())
		{
			if(indexStart <= (INDEX_INTIDX_MAX - count))
			{
				// 容量確保
				if(!arr->ensureCapacity(length))
				{
					return false;
				}

				// 配列長値更新
				EsValue* valLength;
				if(arr->getLengthValueFromSlot(&valLength) < length)
				{
					valLength->setValue(length);
				}

				// 現在の要素数
				EsValue* valCount;
				s32 countWk = arr->getCountValueFromSlot(&valCount);
				if(!isAllHoles)
				{
					s32 cnt = 0;
					for(int i = 0; i < count; i++)
					{
						if(!arr->getSlots()->getDvalue(i)->isHole())
						{
							cnt++;
						}
					}
					countWk -= cnt;
				}

				// 配列コピー
				for(int i = 0; i < count; i++)
				{
					arr->slots()->dvalue(i)->copy(&valarr[i]);
				}

				// 要素数更新
				countWk += count;
				if(valCount != 0L)	{	valCount->setValue(countWk);	}
				return true;
			}
		}
	}

	// 要素設定
	int index = indexStart;
	while((index < length) && (index <= INDEX_INTIDX_MAX))
	{
		if(!EsArray::setElement(ctx, obj, index, &valarr[index]))
		{
			return false;
		}
		index++;
	}

	if(index == length)	// 最後まで実行された
	{
		return true;
	}
	// 途中で終わった（INDEX_INTIDX_MAX に達した）
	ASSERT(index == INDEX_INTIDX_MAX);

	// 残りの追加処理（文字列 ID として追加）
	while(index < length)
	{
		EsValue id;
		EsValue valIndex(index);
		if(!ctx->convValueToStringId(&id, &valIndex))
		{
			return false;
		}
		if(!obj->callSetPropertyOv(ctx, &id, &valarr[index]))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スロットから配列長を得る
**//*---------------------------------------------------------------------*/
s32 EsArray::getLengthValueFromSlot(const EsObject* obj)
{
	const EsValue* val = obj->getSlots()->getValue(EsArrayClass::getInstance()->getArrayLengthSlotIndex());
	ASSERT(val->isVoid() || val->isInt());
	return val->isVoid() ? 0 : val->getInt();
}

/*---------------------------------------------------------------------*//**
	スロットから配列長を得る
**//*---------------------------------------------------------------------*/
s32 EsArray::getLengthValueFromSlot(EsValue** valLength, EsObject* obj)
{
	EsValue* val = obj->slots()->value(EsArrayClass::getInstance()->getArrayLengthSlotIndex());
	ASSERT(val->isVoid() || val->isInt());
	*valLength = val;
	return val->isVoid() ? 0 : val->getInt();
}

/*---------------------------------------------------------------------*//**
	スロットから要素数を得る
**//*---------------------------------------------------------------------*/
s32 EsArray::getCountValueFromSlot(const EsObject* obj)
{
	const EsValue* val = obj->getSlots()->getDvalue(EsArrayClass::getInstance()->getArrayCountSlotIndex());
	if(val == 0L)	{	return 0;	}
	ASSERT(val->isVoid() || val->isInt());
	return val->isVoid() ? 0 : val->getInt();
}

/*---------------------------------------------------------------------*//**
	スロットから要素数を得る
**//*---------------------------------------------------------------------*/
s32 EsArray::getCountValueFromSlot(EsValue** valCount, EsObject* obj)
{
	EsValue* val = obj->slots()->value(EsArrayClass::getInstance()->getArrayCountSlotIndex());
	*valCount = val;
	if(val == 0L)	{	return 0;	}
	ASSERT(val->isVoid() || val->isInt());
	return val->isVoid() ? 0 : val->getInt();
}

/*---------------------------------------------------------------------*//**
	ID が整数インデックス配列のインデックスかを調べる
	⇒ IsDenseArrayId
**//*---------------------------------------------------------------------*/
bool EsArray::isIntIndexedArrayIndex(EsContext* ctx, const EsValue* id) const
{
	if(!isIntIndexedArray())		{	return false;	}
	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	if(id->equalsStrict(&valLengthAtom))	{	return true;	}
	s32 index;
	if(!isArrayIndexId(&index, id))			{	return false;	}
	if(index > INDEX_INTIDX_MAX)			{	return false;	}
	if(index >= getIntIndexedCapacity())	{	return false;	}
	if(getLengthValueFromSlot() <= 0)		{	return false;	}
	return !getSlots()->getDvalue(index)->isHole();
}

/*---------------------------------------------------------------------*//**
	非整数インデックス配列への変更 ⇒ js_MakeArraySlow, ENSURE_SLOW_ARRAY
**//*---------------------------------------------------------------------*/
bool EsArray::changeNoIntIndexedArray(EsContext* ctx)
{
	if(!_isIntIndexed)	{	return true;	}

	EsScope* scope = getScope();

	s32 capacity = getIntIndexedCapacity();
	if(capacity > 0)
	{
		scope->setFreeSlotIndex(getSlots()->getNum() + EsStackFrame::NUM_INITIAL_SLOT);
	}
	else
	{
		scope->setFreeSlotIndex(getSlots()->getNum());
	}

	for(int i = 0; i < capacity; i++)
	{
		if(getSlots()->getDvalue(i)->isHole())
		{
			EsValue valVoid;
			slots()->setDvalue(i, &valVoid);	// ⇒ obj->dslots[i] = JSVAL_VOID;
			continue;
		}

		EsValue valIndex(i);
		if(getScope()->addScopeProperty(ctx, &valIndex, 0L, 0L, getClass()->getStartDynamicSlotIndex() + i, EsScopeProperty::ATRBF_ENUMERATE, 0, 0) == 0)
		{
			return false;
		}
	}

	EsValue valLength(getLengthValueFromSlot());
	slots()->setValue(EsArrayClass::getInstance()->getArrayCountSlotIndex(), &valLength);

	_isIntIndexed = false;
	return true;
}

/*---------------------------------------------------------------------*//**
	配列容量の確保 ⇒ EnsureCapacity
**//*---------------------------------------------------------------------*/
bool EsArray::ensureCapacity(s32 size)
{
	s32 sizeOld = getIntIndexedCapacity();

	if(size > sizeOld)
	{
		const s32 SIZE_CAPACITY_MIN = 7;
		s32 sizeNew = sizeOld * 2;
		size = TFW_MAX(size, sizeNew);
		size = TFW_MAX(size, SIZE_CAPACITY_MIN);
		return resizeSlots(size);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	配列スロットのリサイズ ⇒ ResizeSlots
**//*---------------------------------------------------------------------*/
bool EsArray::resizeSlots(s32 size)
{
	s32 numDslotsOld = getSlots()->getDynamicSlotNum();
	if(size != numDslotsOld)
	{
		slots()->recreate((s32)(getSlots()->getStartDynamicIndex() + size));
	}

	setIntIndexedCapacity(size);

	if(numDslotsOld < 0)	{	numDslotsOld = 0;	}
	for(s32 i = numDslotsOld; i < getSlots()->getDynamicSlotNum(); i++)
	{
		slots()->dvalue(i)->setHole();
	}

	return true;
}


//==========================================================================
// EsArray::EsArrayPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = EsArray::EsArrayClass::makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsArray::EsArrayPrototype::EsArrayPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_ARRAY)->getAtom(), EsSysPrototypes::PTYPEKIND_ARRAY)
{
}


//==========================================================================
// EsArray::EsArrayClass メソッド

/*---------------------------------------------------------------------*//**
	配列スクリプトクラスのオブジェクト演算子セットを得る
	⇒ js_ArrayObjectOps
**//*---------------------------------------------------------------------*/
EsObjectOp* EsArray::EsArrayClass::getObjectOp() const
{
	return new EsArrayObjectOp();
}

/*---------------------------------------------------------------------*//**
	配列スクリプトクラスの作成 ⇒ js_InitArrayClass
**//*---------------------------------------------------------------------*/
EsObject* EsArray::EsArrayClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, &callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_ArrayClass, js_Array, 1, array_props, array_methods, NULL, NULL);
	if((objPtype == 0L) || (objCtor == 0L))
	{
		return 0L;
	}
	// 非静的プロパティを定義する ⇒ array_props
	objPtype->defineClassProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom(), callLengthGetter, callLengthSetter, -1, EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	// 非静的関数を定義する ⇒ array_methods
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(), callToString, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_JOIN)->getAtom(), callJoin, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_REVERSE)->getAtom(), callReverse, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SORT)->getAtom(), callSort, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PUSH)->getAtom(), callPush, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_POP)->getAtom(), callPop, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SHIFT)->getAtom(), callShift, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_UNSHIFT)->getAtom(), callUnshift, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SPLICE)->getAtom(), callSplice, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CONCAT)->getAtom(), callConcat, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SLICE)->getAtom(), callSlice, 0);
	// 静的プロパティを定義する
	///objCtor->defineClassProperty();
	// 静的関数を定義する
	///objCtor->defineClassFunction();

	// オブジェクト初期化
	ASSERT(objPtype->getObjType() == OBJT_ARRAY);
	if(!((EsArray*)objPtype)->create(ctx, 0L, 0))	// ⇒ InitArrayObject(cx, proto, 0, NULL)
	{
		return 0L;
	}

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	配列オブジェクトクラスのコンストラクタ ⇒ js_ArrayClass
**//*---------------------------------------------------------------------*/
EsArray::EsArrayClass::EsArrayClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_ARRAY)->getAtom()
		, EsSysClasses::CLASSTYPE_ARRAY
		, EsClass::F_HAS_PRIVATE | EsClass::F_ENUMERATE_OUT_PARAM
		, EsSysPrototypes::PTYPEKIND_ARRAY
		, 1 )
{
	_funcAddProp = callAddPropertySlowArray;
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	配列スクリプトクラスのコンストラクタ ⇒ js_Array
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// new なしのコンストラクタ
	if(!TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))
	{
		objThis = ctx->newObject(getInstance(), 0L, 0L);
		if(objThis == 0L)
		{
			return false;
		}
		valRet->setValue(objThis);
	}

	// 配列を得る
	s32 length;
	EsValue* valarr;
	if(numArg == 0L)
	{
		length = 0;
		valarr = 0L;
	}
	else if(numArg > 1)
	{
		length = numArg;
		valarr = vaArg;
	}
	else if(!vaArg->isNumber())
	{
		length = 1;
		valarr = vaArg;
	}
	else
	{
		EsArray::getArrayLengthFromValue(&length, ctx, vaArg);
		if(vaArg->isNull())
		{
			return false;
		}
		valarr = 0L;
	}

	ASSERT(objThis->getObjType() == OBJT_ARRAY);
	return ((EsArray*)objThis)->create(ctx, valarr, length);	// ⇒ InitArrayObject(cx, obj, length, vector)
}

/*---------------------------------------------------------------------*//**
	非整数インデックス配列のアクセサ追加 ⇒ slowarray_addProperty
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callAddPropertySlowArray(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	s32 index;
	if(!EsArray::isArrayIndexId(&index, id))
	{
		return true;
	}

	EsValue* valLength;
	s32 length = EsArray::getLengthValueFromSlot(&valLength, obj);
	if(length <= (s32)index)
	{
		valLength->setValue((s32)index + 1);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	長さ取得ゲッター ⇒ array_length_getter
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callLengthGetter(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	while(obj != 0L)
	{
		if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)	// ⇒ OBJ_IS_ARRAY
		{
			// 長さをスロットから取得して返答値にセットする ⇒ IndexToValue(cx, obj->fslots[JSSLOT_ARRAY_LENGTH], vp);
			s32 length = EsArray::getLengthValueFromSlot(obj);
			val->setNumber(length);
			return true;
		}

		obj = obj->slots()->prototypeObject();
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	長さ設定セッター ⇒ array_length_setter
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callLengthSetter(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	// 配列以外の場合
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_ARRAY)	// ⇒ !OBJ_IS_ARRAY(cx, obj)
	{
		EsValue spropidLength(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());	// ⇒ id = ATOM_TO_JSID(rt->atomState.lengthAtom);
		return obj->definePropertyOv(0L, ctx, &spropidLength, val, 0L, 0L, EsScopeProperty::ATRBF_ENUMERATE);
	}

	// 配列である．新しい長さと現在の長さを得る
	EsArrayClass* cls = (EsArrayClass*)obj->getClass();
	EsArray* arr = (EsArray*)obj;
	s32 lenNew;
	EsArray::getArrayLengthFromValue(&lenNew, ctx, val);
	if(val->isNull())
	{
		return false;
	}
	s32 lenOld = obj->getSlots()->getValue(cls->getArrayLengthSlotIndex())->getInt();
	if(lenNew == lenOld)
	{
		return true;
	}
	
	// 配列拡大
	if(lenOld < lenNew)
	{
		EsValue valLenNew(lenNew);
		obj->slots()->setValue(cls->getArrayLengthSlotIndex(), &valLenNew);
		return true;
	}

	// 配列縮小
	if(arr->isIntIndexedArray())	//  ⇒ OBJ_IS_DENSE_ARRAY
	{
		s32 capacityOld = arr->getIntIndexedCapacity();
		if(capacityOld >= lenNew)
		{
			if(!arr->slots()->recreate(lenNew))
			{
				return false;
			}
		}
	}
	else
	{
		s32 cnt = lenOld;
		while(lenOld > lenNew)
		{
			if(!arr->deleteElement(ctx, cnt))
			{
				return false;
			}
			cnt--;
		}
	}

	EsValue valLenNew(lenNew);
	obj->slots()->setValue(cls->getArrayLengthSlotIndex(), &valLenNew);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ array_toString
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	if(!ctx->instanceOf(objThis, getInstance(), vaArg))	{	return false;	}

	VcString strComma(",");
	return joinSub(valRet, ctx, objThis, &strComma);
}

/*---------------------------------------------------------------------*//**
	文字列連結 ⇒ array_join
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callJoin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// セパレータ文字列を得る
	const VcString* cstrSeparator = 0L;
	if((numArg > 0) && !vaArg->isVoid())
	{
		if(!vaArg->toString(&cstrSeparator, ctx) || (cstrSeparator == 0L))
		{
			return false;
		}
		vaArg->setValue(cstrSeparator);
	}

	return joinSub(valRet, ctx, objThis, cstrSeparator);
}

/*---------------------------------------------------------------------*//**
	要素の順序反転 ⇒ array_reverse
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callReverse(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 配列の長さを得る ⇒ js_GetLengthProperty
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))	{	return false;	}

	// 整数インデックス配列の処理
	if(EsArray::isIntIndexedArray(objThis))
	{
		EsArray* arr = (EsArray*)objThis;
		if(arr->isPrototypeHasIndexedPropeties())
		{
			// 不要判定
			if((lenArray == 0) || (arr->getSlots()->getDynamicSlotNum() <= 0))	// ⇒ if (len == 0 || !objThis->dslots)
			{
				return true;
			}

			// 容量決定
			if(!arr->ensureCapacity(lenArray))
			{
				return false;
			}

			// 並び替え
			u16 i = 0;
			u16 j = (u16)(lenArray - 1);
			while(i < j)
			{
				EsValue valTmp(*arr->getSlots()->getDvalue(i));
				const EsValue* valJ = arr->getSlots()->getDvalue(j);
				arr->slots()->setDvalue(i, valJ);
				arr->slots()->setDvalue(j, &valTmp);
				i++;
				j--;
			}

			valRet->setValue(objThis);
			return true;
		}
	}

	// 非整数インデックス配列の処理
	s32 i = 0;
	s32 j = lenArray - 1;
	EsValue valI, valJ;
	bool isHoleI, isHoleJ;
	bool isOk = true;
	while(i < j)
	{
		if(	!EsArray::getElement(&valI, &isHoleI, ctx, objThis, i) ||
			!EsArray::getElement(&valJ, &isHoleJ, ctx, objThis, j) ||
			!(isHoleI ? EsArray::deleteElement(ctx, objThis, j) : EsArray::setElement(ctx, objThis, j, &valI)) ||	// ⇒ SetOrDeleteArrayElement(cx, objThis, len - i - 1, hole, tvr.u.value)
			!(isHoleJ ? EsArray::deleteElement(ctx, objThis, i) : EsArray::setElement(ctx, objThis, i, &valJ))	)
		{
			isOk = false;
		}
		i++;
		j--;
	}

	valRet->setValue(objThis);
	return isOk;
}

/*---------------------------------------------------------------------*//**
	要素のソート ⇒ array_sort
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callSort(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 比較関数
	EsValue valFunc;
	if(numArg > 0)
	{
		if(vaArg->isPrimitive())
		{
			ctx->addRunError(EsError::ID_BAD_SORT_ARG);
			return false;
		}
		valFunc.copy(&vaArg[0]);
	}
	else
	{
		valFunc.setNull();
	}

	// 配列の長さを得る ⇒ js_GetLengthProperty
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))	{	return false;	}
	if(lenArray <= 1)
	{
		valRet->setValue(objThis);
		return true;	// ソート不要
	}

	// ソート用配列を確保する
	EsValue* valarrWk = new EsValue[lenArray];
	EsValue* valarrTmp = 0L;
	if(valarrWk == 0L)
	{
		return false;
	}

	// 配列を収集する
	s32 numUndef = 0;
	s32 cnt = 0;
	s32 lenValid;
	bool isAllStrings = true;
	bool isHole;
	for(int i = 0; i < lenArray; i++)
	{
		valarrWk[cnt].setNull();
		if(!EsArray::getElement(&valarrWk[cnt], &isHole, ctx, objThis, i))
		{
			goto FAILED;
		}

		if(isHole)
		{
			continue;
		}

		if(valarrWk[cnt].isVoid())
		{
			numUndef++;
			continue;
		}

		if(!valarrWk[cnt].isString() && isAllStrings)
		{
			isAllStrings = false;
		}

		cnt++;
	}
	lenValid = cnt;
	if(lenValid <= 0)
	{
		goto SUCCEEDED;
	}

	if(valFunc.isNull())	// デフォルトのソート関数で並び替え
	{
		if(isAllStrings)
		{
			// 一時作業用配列を確保する
			valarrTmp = new EsValue[lenValid];
			if(valarrTmp == 0L)
			{
				goto FAILED;
			}
			TFW_ZERO_MEMORY(valarrTmp, sizeof(EsValue) * lenValid);	// 全て NULL 化
			// ソート実行
			if(!MergeSort::sort(
				valarrWk,
				valarrTmp,
				sizeof(EsValue),
				lenValid,
				compareStrings,
				4,
				ctx)	)
			{
				goto FAILED;
			}
		}
		else
		{
			// 文字列値、オリジナル値･･･の連続形式の配列に変更する
			EsValue* valarrWkOld = valarrWk;
			EsValue* valarrWk = new EsValue[lenValid * 2];
			for(int i = 0; i < lenValid; i++)
			{
				const VcString* cstr;
				if(!valarrWkOld[i].toString(&cstr, ctx))
				{
					delete[] valarrWkOld;
					goto FAILED;
				}
				valarrWk[(i * 2) + 0].setValue(cstr);
				valarrWk[(i * 2) + 1].copy(&valarrWkOld[i]);
			}
			delete[] valarrWkOld;
			// 一時作業用配列を確保する
			valarrTmp = new EsValue[lenValid * 2];
			if(valarrTmp == 0L)
			{
				goto FAILED;
			}
			TFW_ZERO_MEMORY(valarrTmp, sizeof(EsValue) * 2 * lenValid);	// 全て NULL 化
			// ソート実行
			if(!MergeSort::sort(
				valarrWk,
				valarrTmp,
				sizeof(EsValue) * 2,
				lenValid,
				compareStrings,
				4,
				ctx)	)
			{
				goto FAILED;
			}
			// 元の配列形式に戻す
			for(int i = 0; i < lenValid; i++)
			{
				valarrWk[i].copy(&valarrWk[(i * 2) + 1]);
			}
		}
	}
	else	// 指定のソート関数で並び替え
	{
		// 比較関数を得る
		MergeSort::CompareFunc func = (MergeSort::CompareFunc)valFunc.getPrivateData();
		// 一時作業用配列を確保する
		valarrTmp = new EsValue[lenValid];
		if(valarrTmp == 0L)
		{
			goto FAILED;
		}
		// ソート実行
		if(!MergeSort::sort(
			valarrWk,
			valarrTmp,
			sizeof(EsValue),
			lenValid,
			func,
			4,
			ctx)	)
		{
			goto FAILED;
		}
	}

	// 配列設定
	if(!setArrayElements(ctx, objThis, valarrWk, 0, lenValid, false))
	{
		goto FAILED;
	}

	// 未定義要素を末尾に並べる
	while(numUndef > 0)
	{
		EsValue valVoid;
		if(!EsArray::setElement(ctx, objThis, lenValid, &valVoid))
		{
			goto FAILED;
		}
		lenValid++;
		numUndef--;
	}

	// 不要な穴要素を削除する
	while(lenArray > lenValid)
	{
		lenArray--;
		if(!EsArray::deleteElement(ctx, objThis, lenArray))
		{
			goto FAILED;
		}
	}

SUCCEEDED:
	delete[] valarrWk;
	delete[] valarrTmp;
	return true;

FAILED:
	delete[] valarrWk;
	delete[] valarrTmp;
	return false;
}

/*---------------------------------------------------------------------*//**
	配列のプッシュ ⇒ array_push
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callPush(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 整数インデックス配列、かつ、追加要素が１つの場合の処理
	if(EsArray::isIntIndexedArray(objThis) && (numArg == 1))
	{
		do
		{
			EsArray* arr = (EsArray*)objThis;

			// 長さ取得
			EsValue* valLength;
			s32 lenArray = arr->getLengthValueFromSlot(&valLength);
			// 隙間が多い場合は非整数インデックス配列へ変更
			if((lenArray < arr->getIntIndexedCapacity()) && (lenArray <= INDEX_INTIDX_MAX))	// ⇒ !INDEX_TOO_SPARSE(objThis, length)
			{
				if(!arr->changeNoIntIndexedArray(ctx))
				{
					return false;
				}

				break;	// 非整数インデックス配列のプッシュへ
			}
			// 配列容量の決定
			if(!arr->ensureCapacity(lenArray + 1))
			{
				return false;
			}
			// 配列長の決定
			valLength->setValue(lenArray + 1);
			ASSERT(arr->getSlots()->getDvalue(lenArray)->isHole());
			// 要素数の決定
			EsValue* valCount;
			s32 count = arr->getCountValueFromSlot(&valCount);
			valCount->setValue(count + 1);
			// 要素値セット
			arr->slots()->setDvalue(lenArray, &vaArg[0]);

			valRet->setNumber(lenArray);
			return true;
		}
		while(false);
	}

	// 非整数インデックス配列のプッシュ処理 ⇒ array_push_slowly
	// 長さ取得
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))
	{
		return false;
	}
	// 配列セット
	if(!EsArray::setArrayElements(ctx, objThis, vaArg, lenArray, numArg, false))
	{
		return false;
	}
	// 長さセット
	f64 lenNew = lenArray + (f64)numArg;
	valRet->setNumber(lenNew);	// ⇒ IndexToValue(cx, newlength, rval)
	return EsArray::setArrayLengthProperty(ctx, objThis, lenNew);	// ⇒ js_SetLengthProperty(cx, objThis, newlength)
}

/*---------------------------------------------------------------------*//**
	配列のポップ ⇒ array_pop
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callPop(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 整数インデックス配列の場合の処理
	if(EsArray::isIntIndexedArray(objThis))
	{
		EsArray* arr = (EsArray*)objThis;

		// 長さ取得
		EsValue* valLength;
		s32 lenArray = arr->getLengthValueFromSlot(&valLength);
		if(lenArray <= 0)
		{
			valRet->setVoid();
			return true;
		}
		// 要素１つ取得
		lenArray--;
		bool isHole;
		if(!arr->getElement(valRet, &isHole, ctx, lenArray))
		{
			return false;
		}
		// 取得分をオブジェクトから削除
		if(!isHole)
		{
			if(!arr->deleteElement(ctx, lenArray))
			{
				return false;
			}
		}
		// 長さ設定
		valLength->setValue(lenArray);
		return true;
	}

	// 非整数インデックス配列のプッシュ処理 ⇒ array_pop_slowly
	// 長さ取得
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))
	{
		return false;
	}
	if(lenArray <= 0)
	{
		valRet->setVoid();
		return EsArray::setArrayLengthProperty(ctx, objThis, 0);
	}
	// 要素１つ取得
	lenArray--;
	bool isHole;
	if(!EsArray::getElement(valRet, &isHole, ctx, objThis, lenArray))
	{
		return false;
	}
	// 取得分をオブジェクトから削除
	if(!isHole)
	{
		if(!EsArray::deleteElement(ctx, objThis, lenArray))
		{
			return false;
		}
	}
	// 長さセット
	return EsArray::setArrayLengthProperty(ctx, objThis, lenArray);
}

/*---------------------------------------------------------------------*//**
	配列の前方にシフト ⇒ array_shift
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callShift(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 長さ取得
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))
	{
		return false;
	}
	if(lenArray <= 0)
	{
		valRet->setVoid();
		return EsArray::setArrayLengthProperty(ctx, objThis, 0);
	}
	s32 lenNew = lenArray - 1;

	// 整数インデックス配列の場合の処理
	if(EsArray::isIntIndexedArray(objThis))
	{
		EsArray* arr = (EsArray*)objThis;

		if(!arr->isPrototypeHasIndexedPropeties())
		{
			if(lenArray < arr->getIntIndexedCapacity())
			{
				if(arr->getSlots()->getDynamicSlotNum() > 0)
				{
					// 先頭の要素を取得
					valRet->copy(arr->getSlots()->getDvalue(0));
					// 要素数を更新
					if(valRet->isHole())
					{
						valRet->setVoid();
					}
					else
					{
						EsValue* valCount;
						s32 count = arr->getCountValueFromSlot(&valCount);
						valCount->setValue(count - 1);
					}
					// 要素を前方シフト
					for(int i = 0; i < lenNew; i++)
					{
						arr->slots()->setDvalue(i, arr->getSlots()->getDvalue(i + 1));
					}
					// 移動後の要素を穴に
					arr->slots()->dvalue(lenNew)->setHole();
				}
				else
				{
					ASSERT(arr->getCountValueFromSlot() == 0);
					valRet->setVoid();
				}
				// 長さを設定
				return EsArray::setArrayLengthProperty(ctx, objThis, lenNew);
			}
		}
	}

	// 非整数インデックス配列の場合の処理
	// 先頭の要素を取得
	bool isHole;
	if(!EsArray::getElement(valRet, &isHole, ctx, objThis, 0))
	{
		return false;
	}
	// 要素をシフト
	EsValue valWk;
	for(int i = 0; i < lenNew; i++)
	{
		if(!EsArray::getElement(&valWk, &isHole, ctx, objThis, i + 1))
		{
			return false;
		}
		if(!(isHole ? EsArray::deleteElement(ctx, objThis, i) : EsArray::setElement(ctx, objThis, i, &valWk)))	// ⇒ SetOrDeleteArrayElement(cx, objThis, i, hole, tvr.value())
		{
			return false;
		}
	}
	// 最後の要素を削除
	if(!isHole)
	{
		if(!EsArray::deleteElement(ctx, objThis, lenNew))
		{
			return false;
		}
	}
	// 長さを設定
	return EsArray::setArrayLengthProperty(ctx, objThis, lenNew);
}

/*---------------------------------------------------------------------*//**
	配列の後方にシフト ⇒ array_unshift
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callUnshift(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 長さ取得
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))
	{
		return false;
	}
	s32 lenNew = lenArray;

	if(numArg > 0)
	{
		if(lenArray > 0)
		{
			bool isIntIndexedShift = false;
			if(EsArray::isIntIndexedArray(objThis))
			{
				EsArray* arr = (EsArray*)objThis;
				if(!arr->isPrototypeHasIndexedPropeties())
				{
					s32 iidx = lenNew + numArg;
					if((iidx < arr->getIntIndexedCapacity()) && (iidx <= INDEX_INTIDX_MAX))	// ⇒ !INDEX_TOO_SPARSE(objThis, newlen + argc)
					{
						// 容量確定
						if(!arr->ensureCapacity(iidx))
						{
							return false;
						}
						// 要素を後方シフト
						s32 idxSrc = lenArray - 1;
						s32 idxDst = idxSrc + numArg;
						while(idxSrc >= 0)
						{
							arr->slots()->setDvalue(idxDst, arr->getSlots()->getDvalue(idxSrc));
							idxSrc--;
							idxDst--;
						}
						// 移動後の要素を穴に
						while(idxDst >= 0)
						{
							arr->slots()->dvalue(idxDst)->setHole();
							idxDst--;
						}
						isIntIndexedShift = true;
					}
				}
			}
			if(!isIntIndexedShift)
			{
				// 要素を後方シフト
				EsValue valWk;
				bool isHole;
				s32 idxSrc = lenArray - 1;
				s32 idxDst = idxSrc + numArg;
				while(idxSrc >= 0)
				{
					if(!EsArray::getElement(&valWk, &isHole, ctx, objThis, idxSrc))
					{
						return false;
					}
					if(!(isHole ? EsArray::deleteElement(ctx, objThis, idxDst) : EsArray::setElement(ctx, objThis, idxDst, &valWk)))	// ⇒ SetOrDeleteArrayElement(cx, objThis, upperIndex, hole, tvr.value())
					{
						return false;
					}
					idxSrc--;
					idxDst--;
				}
			}
		}
		// 配列セット
		if(!EsArray::setArrayElements(ctx, objThis, vaArg, 0, numArg, true))
		{
			return false;
		}
		// 長さ設定
		lenNew += numArg;
		if(!EsArray::setArrayLengthProperty(ctx, objThis, lenNew))
		{
			return false;
		}
	}

	valRet->setNumber(lenNew);
	return true;
}

/*---------------------------------------------------------------------*//**
	配列要素置換 ⇒ array_splice
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callSplice(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	EsArray* arrThis = EsArray::isIntIndexedArray(objThis) ? (EsArray*)objThis : 0L;

	// 新しい返答値配列を作成
	EsArray* arrRet = ctx->newArray(0L, 0);
	if(arrRet == 0L)
	{
		return false;
	}
	valRet->setValue(arrRet);	// 返答値にセット

	// 置換要素なし
	if(numArg <= 0)
	{
		return true;
	}

	// 長さ取得
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))
	{
		return false;
	}

	// 第一引数：開始インデックス
	s32 idxArg = 0;
	f64 fval;
	if(!vaArg[idxArg].toNumber(&fval, ctx))
	{
		return false;
	}
	fval = EsValue::convF64ToS32(fval);
	if(fval < 0)
	{
		fval += lenArray;
		if(fval < 0)
		{
			fval = 0;
		}
	}
	else if(fval > lenArray)
	{
		fval = lenArray;
	}
	idxArg++;
	s32 idxBegin = (s32)fval;

	// 第二引数：置換要素数
	s32 numElm = lenArray - idxBegin;
	if(numArg >= 2)	// 第二引数がある
	{
		if(!vaArg[idxArg].toNumber(&fval, ctx))
		{
			return false;
		}
		fval = EsValue::convF64ToS32(fval);
		if(fval < 0)
		{
			numElm = 0;
		}
		else if(fval < numElm)
		{
			numElm = (s32)fval;
		}
		idxArg++;
	}
	s32 idxEnd = idxBegin + numElm;

	// 返答値の作成
	if(numElm > 0)
	{
		if(	(arrThis != 0L) &&
			(!arrThis->isPrototypeHasIndexedPropeties()) &&
			(!arrRet->isPrototypeHasIndexedPropeties()) &&
			(idxEnd <= arrThis->getIntIndexedCapacity())	)
		{
			// 返答配列（整数インデックス配列）の要素設定
			if(!arrRet->ensureCapacity(numElm))	{	return false;	}
			s32 count = numElm;
			for(int i = 0; i < numElm; i++)
			{
				const EsValue* valSrc = arrThis->getSlots()->getDvalue(i + idxBegin);
				if(valSrc->isHole())	{	count--;	}
				arrRet->slots()->dvalue(i)->copy(valSrc);
			}
			EsValue valNumElm(numElm);
			arrRet->slots()->setValue(EsArrayClass::getInstance()->getArrayLengthSlotIndex(), &valNumElm);
			EsValue valCount(count);
			arrRet->slots()->setValue(EsArrayClass::getInstance()->getArrayCountSlotIndex(), &valCount);
		}
		else
		{
			arrRet->changeNoIntIndexedArray(ctx);
			// 返答配列（非整数インデックス配列）の要素設定
			EsValue valWk;
			bool isHole;
			for(int i = 0; i < numElm; i++)
			{
				if(!arrThis->getElement(&valWk, &isHole, ctx, i + idxBegin))
				{
					return false;
				}
				if(!isHole && !arrRet->setElement(ctx, objThis, i, &valWk))
				{
					return false;
				}
			}
			// 返答配列の長さを設定
			arrRet->setArrayLengthProperty(ctx, numElm);
		}
	}

	// 第三引数の置換データ配列が入る分を空ける
	s32 numData = (numArg > 2) ? numArg - 2 : 0;
	if(numData > numElm)
	{
		s32 diff = numArg - numElm;
		if(	(arrThis != 0L) &&
			(!arrThis->isPrototypeHasIndexedPropeties()) &&
			(lenArray <= arrThis->getIntIndexedCapacity()) &&
			((lenArray == 0) || (!arrThis->getSlots()->getDvalue(lenArray - 1)->isHole()))	)
		{
			// 容量拡張
			if(!arrThis->ensureCapacity(lenArray + diff))
			{
				return false;
			}
			// 要素移動
			EsValue* valCount;
			s32 count = arrThis->getCountValueFromSlot(&valCount);
			for(int i = lenArray - 1; i >= idxEnd; i--)
			{
				const EsValue* valSrc = arrThis->getSlots()->getDvalue(i);
				EsValue* valDst = arrThis->slots()->dvalue(i + diff);
				if(valDst->isHole() && !valSrc->isHole())
				{
					valCount->setValue(++count);
				}
				valDst->copy(valSrc);
			}
			// 長さ更新
			EsValue* valLength;
			s32 length = arrThis->getLengthValueFromSlot(&valLength);
			valLength->setValue(length + diff);
		}
		else
		{
			// 要素移動
			EsValue valWk;
			bool isHole;
			for(int i = lenArray - 1; i >= idxEnd; i--)
			{
				if(!EsArray::getElement(&valWk, &isHole, ctx, objThis, i))
				{
					return false;
				}
				if(!(isHole ? EsArray::deleteElement(ctx, objThis, i + diff) : EsArray::setElement(ctx, objThis, i + diff, &valWk)))	// ⇒ SetOrDeleteArrayElement(cx, objThis, last + delta, hole, tvr.value())
				{
					return false;
				}
			}
		}
		lenArray += diff;
	}
	else if(numData < numElm)
	{
		s32 diff = numElm - numArg;
		if(	(arrThis != 0L) &&
			(!arrThis->isPrototypeHasIndexedPropeties()) &&
			(lenArray <= arrThis->getIntIndexedCapacity())	)
		{
			// 要素移動
			EsValue* valCount;
			s32 count = arrThis->getCountValueFromSlot(&valCount);
			for(int i = idxEnd; i < lenArray; i++)
			{
				const EsValue* valSrc = arrThis->getSlots()->getDvalue(i);
				EsValue* valDst = arrThis->slots()->dvalue(i - diff);
				if(valDst->isHole() && !valSrc->isHole())
				{
					valCount->setValue(++count);
				}
				valDst->copy(valSrc);
			}
		}
		else
		{
			// 要素移動
			EsValue valWk;
			bool isHole;
			for(int i = idxEnd; i < lenArray; i++)
			{
				if(!EsArray::getElement(&valWk, &isHole, ctx, objThis, i))
				{
					return false;
				}
				if(!(isHole ? EsArray::deleteElement(ctx, objThis, i - diff) : EsArray::setElement(ctx, objThis, i - diff, &valWk)))	// ⇒ SetOrDeleteArrayElement(cx, objThis, last - delta, hole, tvr.value())
				{
					return false;
				}
			}
		}
		lenArray -= diff;
	}

	// 第三引数のデータ配列をコピー
	if(numData > 0)
	{
		if(!EsArray::setArrayElements(ctx, objThis, &vaArg[idxArg], idxBegin, numData, false))
		{
			return false;
		}
	}

	// 長さ設定
	return EsArray::setArrayLengthProperty(ctx, objThis, lenArray);
}

/*---------------------------------------------------------------------*//**
	配列連結 ⇒ array_concat
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callConcat(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ aobj = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	ASSERT(valThis->getObject() == objThis);

	EsValue* vaArgv = &vaArg[- 1];
	u32 nArgc = numArg;

	// 返答値配列の作成
	EsArray* arrRet;
	s32 lenRet;
	if(EsArray::isIntIndexedArray(objThis))
	{
		EsArray* arrThis = (EsArray*)objThis;
		lenRet = arrThis->getLengthValueFromSlot();

		// 返答用統合配列を作成
		arrRet = ctx->newArray(0L, 0);
		if(arrRet == 0L)
		{
			return false;
		}
		if(!arrRet->ensureCapacity(lenRet))
		{
			return false;
		}
		s32 count = lenRet;
		for(int i = 0; i < lenRet; i++)
		{
			const EsValue* valSrc = arrThis->getSlots()->getDvalue(i);
			if(valSrc->isHole())	{	count--;	}
			arrRet->slots()->dvalue(i)->copy(valSrc);
		}
		EsValue valLenRet(lenRet);
		arrRet->slots()->setValue(EsArrayClass::getInstance()->getArrayLengthSlotIndex(), &valLenRet);
		EsValue valCount(count);
		arrRet->slots()->setValue(EsArrayClass::getInstance()->getArrayCountSlotIndex(), &valCount);
		valRet->setValue(arrRet);	// 返答値にセット

		if(numArg <= 0)
		{
			return true;
		}
		vaArgv++;
		nArgc--;
	}
	else
	{
		lenRet = 0;
		// 返答用統合配列を作成
		arrRet = ctx->newArray(0L, 0);
		if(arrRet == 0L)
		{
			return false;
		}
		valRet->setValue(arrRet);	// 返答値にセット
	}

	EsValue valWk;
	for(u32 i = 0; i < nArgc; i++)
	{
		if(!vaArgv[i].isPrimitive())
		{
			ASSERT(vaArgv[i].isObject());
			EsObject* objAdd = vaArgv[i].getObject();
			// ※※※ 実装不要 ※※※ ⇒ js_GetWrappedObject, JSCLASS_IS_EXTENDED
			if(objAdd->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)
			{
				// 追加する配列の長さプロパティを取得
				EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
				if(!objAdd->callGetPropertyOv(&valWk, ctx, &valLengthAtom))	// ⇒ OBJ_GET_PROPERTY(cx, aobj, ATOM_TO_JSID(cx->runtime->atomState.lengthAtom), &tvr.u.value)
				{
					return false;
				}
				s32 lenAdd;
				if(!EsArray::getArrayLengthFromValue(&lenAdd, ctx, &valWk))
				{
					return false;
				}
				// 要素追加
				bool isHole;
				for(int idxSlot = 0; idxSlot < lenAdd; idxSlot++)
				{
					if(!EsArray::getElement(&valWk, &isHole, ctx, objAdd, idxSlot))
					{
						return false;
					}
					if(!isHole && !EsArray::setElement(ctx, arrRet, idxSlot, &valWk))
					{
						return false;
					}
				}
				lenRet += lenAdd;
				continue;
			}
		}

		if(!EsArray::setElement(ctx, arrRet, lenRet, &vaArgv[i]))
		{
			return false;
		}
		lenRet++;
	}

	// 返答値の長さ設定
	return EsArray::setArrayLengthProperty(ctx, arrRet, lenRet);
}

/*---------------------------------------------------------------------*//**
	配列抽出 ⇒ array_slice
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::callSlice(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ objThis = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	// 長さ取得
	s32 lenArray;
	if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objThis))
	{
		return false;
	}

	s32 idxBegin, idxEnd;
	idxBegin = idxEnd = 0;
	if(numArg > 0)
	{
		// 第一引数：開始インデックス
		f64 fval;
		if(!vaArg[0].toNumber(&fval, ctx))
		{
			return false;
		}
		fval = EsValue::convF64ToS32(fval);
		if(fval < 0)
		{
			fval += lenArray;
			if(fval < 0)
			{
				fval = 0;
			}
		}
		else if(fval > lenArray)
		{
			fval = lenArray;
		}
		idxBegin = (s32)fval;

		// 第二引数：終了インデックス
		if(numArg > 1)	// 第二引数がある
		{
			if(!vaArg[1].toNumber(&fval, ctx))
			{
				return false;
			}
			fval = EsValue::convF64ToS32(fval);
			if(fval < 0)
			{
				fval += lenArray;
				if(fval < 0)
				{
					fval = 0;
				}
			}
			else if(fval > lenArray)
			{
				fval = lenArray;
			}
			idxEnd = (s32)fval;
		}
	}
	if(idxBegin > idxEnd)
	{
		idxBegin = idxEnd;
	}

	// 整数インデックス配列の場合
	if(EsArray::isIntIndexedArray(objThis))
	{
		EsArray* arrThis = (EsArray*)objThis;
		if(!arrThis->isPrototypeHasIndexedPropeties() && (idxEnd <= arrThis->getIntIndexedCapacity()))
		{
			// 返答値配列を作成し返る
			EsArray* arrRet = ctx->newArray(0L, 0);
			if(arrRet == 0L)
			{	
				return false;
			}
			s32 lenRet = idxEnd - idxBegin;
			if(!arrRet->ensureCapacity(lenRet))
			{
				return false;
			}
			s32 count = lenRet;
			for(int i = idxBegin; i < idxEnd; i++)
			{
				const EsValue* valSrc = arrThis->getSlots()->getDvalue(i);
				if(valSrc->isHole())	{	count--;	}
				arrRet->slots()->dvalue(i - idxBegin)->copy(valSrc);
			}
			EsValue valLenRet(lenRet);
			arrRet->slots()->setValue(EsArrayClass::getInstance()->getArrayLengthSlotIndex(), &valLenRet);
			EsValue valCount(count);
			arrRet->slots()->setValue(EsArrayClass::getInstance()->getArrayCountSlotIndex(), &valCount);
			valRet->setValue(arrRet);	// 返答値にセット
			return true;
		}
	}

	// 非整数インデックス配列の場合
	// 返答値配列を作成する
	EsArray* arrRet = ctx->newArray(0L, 0);
	if(arrRet == 0L)
	{
		return false;
	}
	EsValue valWk;
	bool isHole;
	for(int i = idxBegin; i < idxEnd; i++)
	{
		if(!EsArray::getElement(&valWk, &isHole, ctx, objThis, i))
		{	
			return false;
		}
		if(!isHole && !arrRet->setElement(ctx, i - idxBegin, &valWk))
		{
			return false;
		}
	}
	valRet->setValue(arrRet);	// 返答値にセット
	return EsArray::setArrayLengthProperty(ctx, arrRet, idxEnd - idxBegin);
}

/*---------------------------------------------------------------------*//**
	文字列連結サブ関数 ⇒ array_join_sub
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayClass::joinSub(EsValue* valRet, EsContext* ctx, EsObject* obj, const VcString* strSeparator)
{
	ASSERT(obj->getObjType() == OBJT_ARRAY);
	if(obj->getObjType() != OBJT_ARRAY)	{	return false;	}	// 念のため
	EsArray* arr = (EsArray*)obj;

	// 配列の長さを得る
	s32 length;
	if(!EsArray::getArrayLengthProperty(&length, ctx, obj))
	{
		return false;
	}

	// 文字列を集めてリスト化
	EsValue valWk;
	List<VcString*> listElemStr(true);
	for(int index = 0; index < length; index++)
	{
		bool isHole;
		arr->getElement(&valWk, &isHole, ctx, index);
		if(isHole || valWk.isVoid() || valWk.isNull())
		{
		}
		else
		{
			VcString* strElem = new VcString();
			valWk.toString(strElem, ctx);
			listElemStr.addTail(strElem);
		}
	}

	// 文字列を連結する
	VcString strMade;
	s32 lenElem = listElemStr.getCount();
	s32 countElem = 0;
	for(ListIterator<VcString*> it = listElemStr.iterator(); it.has(); it.next())
	{
		const VcString* strElem = it.object();
		strMade.add(strElem);
		countElem++;
		if(countElem < lenElem)
		{
			strMade.add(strSeparator);
		}
	}

	// 固定文字列化
	const VcString* str = 0;
	str = ctx->newString(&strMade);
	valRet->setValue(str);
	return false;
}

/*---------------------------------------------------------------------*//**
	文字列比較ソート関数 ⇒ sort_compare_strings
**//*---------------------------------------------------------------------*/
int EsArray::EsArrayClass::compareStrings(const void* a, const void* b, void* param)
{
	ASSERT(((const EsValue*)a)->isString());
	ASSERT(((const EsValue*)b)->isString());
	const VcString* s1 = ((const EsValue*)a)->getString();
	const VcString* s2 = ((const EsValue*)b)->getString();
	return StringUtils::compare(s1, s2, s1->getLength());
}


//==========================================================================
// EsArray::EsArrayObjectOp メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	プロパティを定義 ⇒ array_defineProperty
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::defineProperty(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf)
{
	// .length プロパティの場合
	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	if(spropid->equalsStrict(&valLengthAtom))	// ⇒ if (id == ATOM_TO_JSID(cx->runtime->atomState.lengthAtom))
	{
		return true;
	}

	// ID が配列インデックスの場合
	s32 index;
	if(EsArray::isArrayIndexId(&index, spropid) && (atrbf == EsScopeProperty::ATRBF_ENUMERATE))
	{
		return callSetProperty(ctx, obj, spropid, value);
	}

	// 非整数インデックス配列へ変換する
	if(obj->getObjType() != OBJT_ARRAY)
	{
		return false;
	}
	if(!((EsArray*)obj)->changeNoIntIndexedArray(ctx))
	{
		return false;
	}

	return obj->definePropertyBase(sprop, ctx, spropid, value, funcGetter, funcSetter, atrbf);
}

/*---------------------------------------------------------------------*//**
	プロパティを削除 ⇒ array_deleteProperty
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::deleteProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	// 整数インデックス配列の処理
	if(EsArray::isIntIndexedArray(obj))
	{
		EsArray* arr = (EsArray*)obj;

		// .length プロパティの場合
		EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
		if(spropid->equalsStrict(&valLengthAtom))
		{
			val->setValue(false);
			return true;
		}

		// アクセサ ID が有効なインデックスの場合
		s32 index;
		if(EsArray::isArrayIndexId(&index, spropid) && (index < arr->getIntIndexedCapacity()))
		{
			EsValue* valTrg = arr->slots()->dvalue(index);
			if(!valTrg->isHole())
			{
				// 要素数を減らす
				EsValue* valCount;
				s32 count = arr->getCountValueFromSlot(&valCount);
				valCount->setValue(count - 1);

				// 穴に
				valTrg->setHole();
			}
		}

		val->setValue(true);
		return true;
	}

	// 非整数インデックス配列の場合の処理
	return obj->deletePropertyBase(val, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	プロパティをドロップ
**//*---------------------------------------------------------------------*/
void EsArray::EsArrayObjectOp::dropProperty(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop)
{
}

/*---------------------------------------------------------------------*//**
	プロパティを探す ⇒ array_lookupProperty
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::findProperty(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	// 整数インデックス配列の処理
	if(EsArray::isIntIndexedArray(obj))
	{
		EsArray* arr = (EsArray*)obj;

		if(arr->isIntIndexedArrayIndex(ctx, spropid))
		{
			*sprop = (EsScopeProperty*)reinterpret_cast<addr_size>(spropid);	// 危ない･･･
			*objScopeOwner = obj;
			return true;
		}

		EsObject* objPtype = obj->slots()->prototypeObject();
		if(objPtype == 0L)
		{
			*sprop = 0L;
			*objScopeOwner = 0L;
			return true;
		}

		return objPtype->findPropertyOv(sprop, objScopeOwner, ctx, spropid);
	}

	// 非整数インデックス配列の場合の処理
	return obj->findPropertyBase(sprop, objScopeOwner, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	取得プロパティのコール ⇒ array_getProperty
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	// .length プロパティの場合
	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	if(spropid->equalsStrict(&valLengthAtom))	// ⇒ if (id == ATOM_TO_JSID(cx->runtime->atomState.lengthAtom))
	{
		// 長さをスロットから取得して返答値にセットする ⇒ IndexToValue(cx, obj->fslots[JSSLOT_ARRAY_LENGTH], vp);
		s32 length = EsArray::getLengthValueFromSlot(obj);
		val->setNumber(length);
		return true;
	}

	// .__proto__ プロパティの場合
	EsValue valProtoAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTO)->getAtom());
	if(spropid->equalsStrict(&valProtoAtom))	// ⇒ if (id == ATOM_TO_JSID(cx->runtime->atomState.protoAtom))
	{
		val->copy(obj->getSlots()->getValue(EsArrayClass::getInstance()->getPrototypeSlotIndex()));
		return true;
	}

	// 整数インデックス配列の処理
	if(EsArray::isIntIndexedArray(obj))
	{
		EsArray* arr = (EsArray*)obj;

		// アクセサ ID が有効なインデックスの場合
		s32 index;
		if(EsArray::isArrayIndexId(&index, spropid) && (index < arr->getIntIndexedCapacity()))
		{
			const EsValue* valTrg = obj->getSlots()->getDvalue(index);
			if(!valTrg->isHole())
			{
				val->copy(valTrg);
				return true;
			}
		}

		// 有効なインデックスではなかった、もしくは穴であった場合
		val->setVoid();
		EsScopeProperty* sprop;
		EsObject* objScope;
		if(!arr->findPropertyNative(&sprop, &objScope, ctx, spropid, ctx->getResolver()->getResolvFlags()))
		{
			return false;
		}
		if(sprop != 0L)
		{
			if(arr->isNative())
			{
				if(!arr->callGetPropertyNative(val, ctx, objScope, sprop))
				{
					return false;
				}
			}
			objScope->dropPropertyOv(ctx, sprop);
		}
		return true;
	}

	// 非整数インデックス配列の場合の処理
	return obj->callGetPropertyBase(val, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	設定プロパティのコール ⇒ array_setProperty
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val)
{
	// .length プロパティの場合
	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	if(spropid->equalsStrict(&valLengthAtom))	// ⇒ if (id == ATOM_TO_JSID(cx->runtime->atomState.lengthAtom))
	{
		return EsArrayClass::callLengthSetter(ctx, obj, spropid, val);
	}

	// 整数インデックス配列の処理
	if(EsArray::isIntIndexedArray(obj))
	{
		EsArray* arr = (EsArray*)obj;

		// アクセサ ID が有効なインデックスの場合
		s32 index;
		if(EsArray::isArrayIndexId(&index, spropid) && (index <= INDEX_INTIDX_MAX))
		{
			// 容量決定
			if(!arr->ensureCapacity(index + 1))
			{
				return false;
			}

			// 配列長さを設定
			{
				EsValue* valLength;
				s32 length = EsArray::getLengthValueFromSlot(&valLength, arr);
				if(index >= length)
				{
					valLength->setValue(index + 1);
				}
			}

			// 要素数を設定
			EsValue* valTrg = obj->slots()->dvalue(index);
			if(valTrg->isHole())
			{
				EsValue* valCount;
				s32 count = EsArray::getCountValueFromSlot(&valCount, obj);
				valCount->setValue(count + 1);
			}

			// 要素を設定
			valTrg->copy(val);
			return true;
		}

		// アクセサ ID がインデックス以外、もしくはインデックスが大きすぎる場合は非整数インデックス配列へ
		if(!arr->changeNoIntIndexedArray(ctx))
		{
			return false;
		}
	}

	// 非整数インデックス配列の場合の処理
	return obj->callSetPropertyBase(ctx, spropid, val);
}

/*---------------------------------------------------------------------*//**
	スコープ属性フラグを得る ⇒ array_getAttributes
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::getPropertyAttributeFlags(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop)
{
	// .length アクセサ判定
	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	if(spropid->equalsStrict(&valLengthAtom))	// ⇒ if (id == ATOM_TO_JSID(cx->runtime->atomState.lengthAtom))
	{
		*spatrbf = EsScopeProperty::ATRBF_PERMANENT;
	}
	else
	{
		*spatrbf = EsScopeProperty::ATRBF_ENUMERATE;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	オブジェクトのデフォルト値を得る
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::convDefaultValue(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype)
{
	return obj->convDefaultValueBase(ctx, val, ltype);
}

/*---------------------------------------------------------------------*//**
	アクセスチェック
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::checkAccess(u8* spatrbf, EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags)
{
	return obj->checkAccessBase(spatrbf, val, ctx, spropid, acsflags);
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ slowarray_enumerate
**//*---------------------------------------------------------------------*/
bool EsArray::EsArrayObjectOp::enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop)
{
	if(enumop != ENUMOP_START)
	{
		if(valState->isInt())
		{
			return EsArray::enumerate(valState, id, ctx, obj, enumop);
		}
	}
	return obj->enumerateBase(valState, id, ctx, enumop);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsArray::EsArrayObjectOp::EsArrayObjectOp()
{
	_funcDefineProperty = defineProperty;
	_funcDeleteProperty = deleteProperty;
	_funcDropProperty = dropProperty;
	_funcFindProperty = findProperty;
	_funcGetProperty = callGetProperty;
	_funcSetProperty = callSetProperty;
	_funcGetPropertyAttributeFlags = getPropertyAttributeFlags;
	_funcConvDefaultValue = convDefaultValue;
	_funcCheckAccess = checkAccess;
	_funcEnumerate = enumerate;
}


//==========================================================================
// EsArray::EsArrayStat メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsArray::EsArrayStat::EsArrayStat(s32 capacity)
	: _bfarr(0L)
	, _index(0)
	, _capacity(capacity)
{
	_bfarr = new BitFlagArray();
	ASSERT(_bfarr != 0L);
	if(_bfarr != 0L)
	{
		if(!_bfarr->create(capacity))
		{
			delete _bfarr;
			_bfarr = 0L;
			return;
		}
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsArray::EsArrayStat::~EsArrayStat()
{
	if(_bfarr != 0L)
	{
		_bfarr->destroy();
		delete _bfarr;
		_bfarr = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
