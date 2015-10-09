/***********************************************************************//**
 *	EsScope.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsScope.h"

// Friends
#include "EsContext.h"
#include "EsScopeProperty.h"
#include "EsObject.h"
#include "EsObjectOp.h"

// External
#include "../../tfw/collection/List.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	参照カウンタをインクリメントする ⇒ js_HoldScope
**//*---------------------------------------------------------------------*/
void EsScope::retainRef()
{
	_refcnt++;
}

/*---------------------------------------------------------------------*//**
	参照カウンタをデクリメントする ⇒ js_DropScope

	@return 参照がまだあるか
**//*---------------------------------------------------------------------*/
bool EsScope::releaseRef(EsObject* obj)
{
	_refcnt--;
	if(_refcnt == 0L)
	{
		// とりあえず削除はまだしないでおく
		return false;	// もうない
	}

	if(_objRef == obj)
	{
		_objRef = 0L;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スコーププロパティの数を得る ⇒ entryCount, OBJ_BLOCK_COUNT
**//*---------------------------------------------------------------------*/
s32 EsScope::getScopePropertyNum() const
{
	return _splist->getCount();
}

/*---------------------------------------------------------------------*//**
	最後のスコーププロパティを得る ⇒ SCOPE_LAST_PROP, lastProp
**//*---------------------------------------------------------------------*/
EsScopeProperty* EsScope::getLastScopeProperty() const
{
	if(_splist->getCount() <= 0)	{	return 0L;	}
	return _splist->getObject(_splist->getCount() - 1);
}

/*---------------------------------------------------------------------*//**
	スコーププロパティを検索する ⇒ js_SearchScope
**//*---------------------------------------------------------------------*/
EsScopeProperty* EsScope::findScopeProperty(const EsValue* spropid) const
{
	for(ListIterator<EsScopeProperty*> it = _splist->iterator(); it.has(); it.next())
	{
		EsScopeProperty* sp = it.object();
		if(sp->getId()->equals(spropid))
		{
			return sp;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	スコーププロパティを追加する ⇒ js_AddScopeProperty
**//*---------------------------------------------------------------------*/
EsScopeProperty* EsScope::addScopeProperty(EsContext* ctx, const EsValue* spropid, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf, s16 shortid)
{
	// ダミーのゲッタ、セッタは NULL に正規化
	if(funcGetter == g_funcDummyPropertyGetter)
	{
		funcGetter = 0L;
	}
	if(funcSetter == g_funcDummyPropertySetter)
	{
		funcSetter = 0L;
	}

	// 既存のスコーププロパティを探す ⇒ js_SearchScope
	EsScopeProperty* sprop = findScopeProperty(spropid);
	if(sprop != 0L)
	{
		if(	!TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_SHARED) &&
			(slotidx == EsScopeProperty::SLOTIDX_INVALUD) &&
			(sprop->getSlotIndex() < getFreeSlotIndex()) )	// ⇒ SPROP_HAS_VALID_SLOT(sprop, scope)
		{
			slotidx = sprop->getSlotIndex();
		}

		// パラメータが一致するならば探したプロパティをそのまま返す
		if(sprop->isMatchParams(funcGetter, funcSetter, slotidx, atrbf, optf, shortid))
		{
			return sprop;
		}

		// 既存のプロパティを削除してオーバーライトする ⇒ SCOPE_LAST_PROP, SCOPE_HAD_MIDDLE_DELETE...
		_splist->remove(sprop);	// リストから削除する
		makeUniqueShape(ctx);	// ⇒ js_MakeScopeShapeUnique

		sprop = 0L;
	}
	else
	{
		// ※※※ 実装保留 ※※※ ⇒ SCOPE_CAPACITY, removedCount ...
	}

	if(sprop == 0L)
	{
		// エイリアスの場合はもう一つのプロパティのスロットを共有する
		if(!TFW_IS_FLAG(optf, EsScopeProperty::OPTF_IS_ALIAS))
		{
			if(TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_SHARED))
			{
				slotidx = EsScopeProperty::SLOTIDX_INVALUD;
			}
			else
			{
				if(slotidx == EsScopeProperty::SLOTIDX_INVALUD)
				{
					if(!_objRef->slots()->create())	// ⇒ js_AllocSlot
					{
						return 0L;
					}
					slotidx = getFreeSlotIndex() - 1;	// ⇒ js_AllocSlot, *slotp = scope->freeslot++;
				}
			}
		}
	}

	// 新規にスコーププロパティを作成する
	sprop = new EsScopeProperty(spropid, funcGetter, funcSetter, slotidx, atrbf, optf);
	if(sprop == 0L)
	{
		return 0L;
	}

	// 親プロパティを設定する ⇒ GetPropertyTreeChild(cx, scope->lastProp, &child)
	EsScopeProperty* spropParent = getLastScopeProperty();
	if(spropParent != 0L)
	{
		sprop->setParent(spropParent);
	}

	// プロパティリストに追加
	_splist->addTail(sprop);

	return sprop;
}

/*---------------------------------------------------------------------*//**
	スコーププロパティを削除する ⇒ js_RemoveScopeProperty
**//*---------------------------------------------------------------------*/
bool EsScope::deleteScopeProperty(EsContext* ctx, const EsValue* spropid)
{
	// 不変チェック
	if(TFW_IS_FLAG(_flags, SCOPEF_SEALED))
	{
		// 読み込み専用である ⇒ ReportReadOnlyScope(cx, scope);
		return false;
	}

	EsScopeProperty* sprop = findScopeProperty(spropid);	// ⇒ spp = js_SearchScope(scope, id, JS_FALSE);
	if(sprop == 0L)
	{
		// 既にない
		return true;
	}

	if(sprop->getSlotIndex() < _slotidxFree)				// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, scope))
	{
		_objRef->freeSlot(sprop->getSlotIndex());
		// ※※※ 実装保留 ※※※ ⇒ JS_ATOMIC_INCREMENT(&cx->runtime->propertyRemovals);
	}

	return _splist->remove(sprop);
}

/*---------------------------------------------------------------------*//**
	スコープにユニークなシェイプを作成する ⇒ js_MakeScopeShapeUnique
**//*---------------------------------------------------------------------*/
void EsScope::makeUniqueShape(EsContext* ctx)
{
	_idShape = ctx->makeUniqueShapeId();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsScope::EsScope(EsObject* objRef, s32 slotidxFree)
	: _objRef(objRef)
	, _splist(new List<EsScopeProperty*>(true))
	, _slotidxFree(slotidxFree)
	, _flags(0)
	, _refcnt(0)
	, _idShape(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsScope::~EsScope()
{
	delete _splist;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
