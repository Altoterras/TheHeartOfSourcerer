/***********************************************************************//**
 *	EnterConfront.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EnterConfront.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../EventSys.h"
#include "../../../../../char/CharDef.h"
#include "../../../../../confront/Confront.h"
#include "../../../../../body/Game.h"
#include "../../../../../map/PlacementObj.h"
#include "../../../../../unit/Unit.h"
#include "../../../../../unit/UnitManager.h"
#include "../../../../../unit/char/CharUnit.h"
#include "../../../../../unit/char/npc/NpcCharUnit.h"

// External
#include "../../../../../../tfw/lib/Color.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EnterConfront メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool EnterConfront::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;
	bool bwk;

	// ［オプション（いずれか必須）］対象制御用イベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "target_ctrl_evid"))
	{
		_evidCtrlTrg = iwk;
	}

	// ［オプション（いずれか必須）］対象キャラクタ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "chair_id"))
	{
		_charid = (u16)iwk;
	}

	// ［オプション］BGM ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "bgm_id"))
	{
		_bgmid = (u16)iwk;
		TFW_SET_FLAG(_optflags, OPTF_HAS_BGM_ID, true);
	}

	// ［オプション］対決制御用イベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "ctrl_evid"))
	{
		_evidCtrlConfront = (u16)iwk;
	}

	// ［オプション］勝利後に実行するイベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "win_evid"))
	{
		_evidAfterWin = (u16)iwk;
		TFW_SET_FLAG(_optflags, OPTF_HAS_EVID_AFTER_WIN, true);
	}

	// ［オプション］敗北後に実行するイベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "lose_evid"))
	{
		_evidAfterLose = (u16)iwk;
		TFW_SET_FLAG(_optflags, OPTF_HAS_EVID_AFTER_LOSE, true);
	}

	// ［オプション］逃げる許可
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "escape"))
	{
		_isAllowEscape = bwk;
		TFW_SET_FLAG(_optflags, OPTF_HAS_IS_ALLOW_ESCAPE, true);
	}

	// スキップ強制停止設定
	TFW_SET_FLAG(_flags, F_SKIPSTOP, true);

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void EnterConfront::play(ActMatter* owner)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	Confront* confront = Game::getGame()->getConfront();

	// 対象を制御用イベント ID かキャラクタ ID から取得する
	Unit* unitTrg = unitmng->findCharUnitFromCtrlEvidOrCharId(_evidCtrlTrg, _charid);
	ASSERT(unitTrg != 0L);

	// 配置オブジェクトを得る
	const PlacementObj* pobj = 0L;
	if(TFW_IS_FLAG(unitTrg->getUnitFlags(), Unit::UNITFLAG_NPC))
	{
		const NpcCharUnit* npc = (NpcCharUnit*)unitTrg;
		pobj = npc->getPlacementObj();
	}

	// ユニットが無効の場合は有効化
	if(unitTrg != 0L)
	{
		if(!unitTrg->isEnable())
		{
			unitTrg->setEnable(true);
		}
	}

	/*
	// イベント後に戻るゲームモードへの変更を対決モジュールに引き継がせる
	s32 gmkAfterEvent = owner->getEventSystemRefer()->getEventAfterGameMode();
	if(gmkAfterEvent != 0)
	{
		// 対決モジュールに委譲
		confront->setConfrontAfterGameMode(gmkAfterEvent);
		owner->getEventSystemRefer()->setEventAfterGameMode(0);	// イベントシステムの設定は NULL 化
	}
	*/

	// 対決オプションを確定（このイベントの設定優先で、次点が PlacementObj）
	u16 bgmid = PlacementObj::BGM_ID_DEFAULT;						// BGM ID
	u16 evidAfterWin = PlacementObj::EVID_ATER_WIN_DEFAULT;			// 勝利後に実行するイベント ID
	u16 evidAfterLose = PlacementObj::EVID_ATER_LOSE_DEFAULT;		// 敗北後に実行するイベント ID
	bool isAllowEscape = PlacementObj::IS_ALLOW_ESCAPE_DEFAULT;		// 逃げる許可
	if(TFW_IS_FLAG(_optflags, OPTF_HAS_BGM_ID))				{	bgmid =	_bgmid;									}
	else if(pobj != 0L)										{	bgmid =	pobj->getBgmId();						}
	if(TFW_IS_FLAG(_optflags, OPTF_HAS_EVID_AFTER_WIN))		{	evidAfterWin = _evidAfterWin;					}
	else if(pobj != 0L)										{	evidAfterWin =	pobj->getAfterWinEvent();		}
	if(TFW_IS_FLAG(_optflags, OPTF_HAS_EVID_AFTER_WIN))		{	evidAfterLose = _evidAfterLose;					}
	else if(pobj != 0L)										{	evidAfterLose =	pobj->getAfterLoseEvent();		}
	if(TFW_IS_FLAG(_optflags, OPTF_HAS_IS_ALLOW_ESCAPE))	{	isAllowEscape =	_isAllowEscape;					}
	else if(pobj != 0L)										{	isAllowEscape =	pobj->isAllowConfrontEscape();	}

	// 対決へ
	confront->start(unitTrg, _evidCtrlConfront, 0L, bgmid, evidAfterWin, evidAfterLose, !isAllowEscape);

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
