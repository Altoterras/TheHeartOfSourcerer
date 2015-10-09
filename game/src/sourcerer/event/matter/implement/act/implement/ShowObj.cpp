/***********************************************************************//**
 *	ShowObj.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ShowObj.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"
#include "../../../../../unit/UnitManager.h"
#include "../../../../../unit/char/CharUnit.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ShowObj メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ShowObj::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;

	// ［必須］表示か非表示か
	if(!XmlParserUtils::analyzeAtrb(&_show, node, "show"))
	{
		return false;
	}

	// ［オプション（いずれか必須）］対象制御用イベント ID （優先）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "ctrl_evid"))
	{
		_evidCtrlTrg = (u16)iwk;
		if(emcp != 0L) { _evidCtrlTrg = emcp->offsetEvid(_evidCtrlTrg); }	// イベント ID のオフセット処理
	}

	// ［オプション］キャラクタ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "char_id"))
	{
		_charidTrg = (u16)iwk;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ShowObj::play(ActMatter* owner)
{
	// ユニットを取得する
	Unit* unit = 0L;
	if((_evidCtrlTrg != 0) || (_charidTrg != 0))
	{
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		unit = unitmng->findCharUnitFromCtrlEvidOrCharId(_evidCtrlTrg, _charidTrg);
	}

	// ユニット有効・無効の設定
	if(unit != 0L)
	{
		unit->setEnable(_show);
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
