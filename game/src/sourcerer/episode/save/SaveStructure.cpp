/***********************************************************************//**
 *	SaveStructure.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SaveStructure.h"

// Friends

// External
#include "../../../tfw/collection/List.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SaveStructure メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	配置オブジェクトリストのセーブデータを保存する

	@param listPlaceObj 配置オブジェクトリスト
	@retval true 成功
	@retval false 失敗
**//*---------------------------------------------------------------------*/
bool SaveStructure::storePlacementObjData(const List<PlacementObj*>* listPlaceObj)
{
	// 末端を得る
	s32 idx = 0;
	while(idx < NUM_POBJ_MAX)
	{
		if(_pobjsvst[idx]._ucase == 0)	// 未記録
		{
			break;
		}
		idx++;
	}

	for(ListIterator<PlacementObj*> it = listPlaceObj->iterator(); it.has(); it.next())
	{
		if(idx >= NUM_POBJ_MAX)
		{
			ASSERT(idx < NUM_POBJ_MAX);
			return false;
		}
		const PlacementObj* pobj = it.object();
		if(!pobj->isNullState())	// 初期状態以外なら
		{
			PlacementObj::SaveStructure* pobjsvst = &_pobjsvst[idx];
			pobj->storeToStructure(pobjsvst);	// 保存
			idx++;
		}
	}
	ASSERT(idx <= NUM_POBJ_MAX);

	return true;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクトリストへセーブデータを復元する

	@param listPlaceObj 配置オブジェクトリスト
	@retval true 成功
	@retval false 失敗
**//*---------------------------------------------------------------------*/
bool SaveStructure::restorePlacementObjData(List<PlacementObj*>* listPlaceObj) const
{
	for(ListIterator<PlacementObj*> it = listPlaceObj->iterator(); it.has(); it.next())
	{
		PlacementObj* pobj = it.object();

		for(int idx = 0; idx < NUM_POBJ_MAX; idx++)
		{
			const PlacementObj::SaveStructure* pobjsvst = &_pobjsvst[idx];
			if(isSamePlacementObj(pobjsvst, pobj))
			{
				pobj->restoreFromStructure(pobjsvst);	// 復元
				break;
			}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	同一の配置オブジェクトかを判定する
**//*---------------------------------------------------------------------*/
bool SaveStructure::isSamePlacementObj(const PlacementObj::SaveStructure* pobjsvst, const PlacementObj* pobj) const
{
#if 1

	// 「制御イベント ID」と「キャラクタ／アイテム 定義 ID」で判定する旧方法
	if(	(pobjsvst->_evidCtrl != 0) &&
		(pobjsvst->_evidCtrl == pobj->getCtrlEvid()) &&
		((pobjsvst->_did == pobj->getCharDefId()) || (pobjsvst->_did == pobj->getItemDefId()))	)
	{
		return true;
	}

#else

#if defined(_ENLIGHT_V01)
	if(pobjsvst->_ucase == PlacementObj::UCASE_QUEST)
#endif
	{
		// ※	「使用種別」と「種別内 ID」で判定する新方法
		//		Ver1.2 以降（クエスト実装後）はこの方法で判定する
		//		しかし、以前のセーブデータとの互換性を保つために、
		//		現時点ではクエストのみをこの方法で判定する
		//		いずれどこかのタイミング（Volume2 など）で完全にこちらに移行したい
		if(	(pobjsvst->_ucase == pobj->getUseCase()) &&
			(pobjsvst->_subid == pobj->getSubId()) &&
			(pobjsvst->_des
			)
		{
			return true;
		}
	}
#if defined(_ENLIGHT_V01)
	else	// ※旧仕様セーブデータ
	{
		// ※	「制御イベント ID」と「キャラクタ / アイテム 定義 ID」で判定する旧方法
		//		Ver1.2 以前（クエスト実装前）はこの方法で判定していた	【2010/12/19 r-kishi】
		if(	(pobjsvst->_evidCtrl != 0) &&
			(pobjsvst->_evidCtrl == pobj->getCtrlEvid()) &&
			((pobjsvst->_did == pobj->getCharDefId()) || (pobjsvst->_did == pobj->getItemDefId()))	)
		{
			return true;
		}
	}
#endif

#endif
	return false;
}

/*---------------------------------------------------------------------*//**
	チェックサムを計算する
**//*---------------------------------------------------------------------*/
void SaveStructure::calcCheckSum(u32* sumF, u32* sumR) const
{
	// 前方計算チェックサム
	if(sumF != 0L)
	{
		*sumF = 0;
		u32* p = (u32*)this;
		int cnt = 0;
		for(int i = 0; i < (sizeof(SaveStructure) - sizeof(SaveStructure::MakingSaveStructure)); i += 4)
		{
			*sumF += *p;
			p++;
			if((++cnt & 27) == 0)	{	*sumF += 2336;	}
		}
	}
	// 後方計算チェックサム
	if(sumR != 0L)
	{
		*sumR = 0;
		u32* p = (u32*)((u8*)this + sizeof(SaveStructure) - sizeof(SaveStructure::MakingSaveStructure));
		int cnt = 0;
		for(int i = sizeof(SaveStructure) - sizeof(SaveStructure::MakingSaveStructure); i >= 0; i -= 4)
		{
			*sumR += *p;
			p--;
			if((++cnt & 28) == 0)	{	*sumR += 2305;	}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
