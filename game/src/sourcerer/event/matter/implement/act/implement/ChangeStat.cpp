/***********************************************************************//**
 *	ChangeStat.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ChangeStat.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"
#include "../../../../../status/CharStat.h"
#include "../../../../../unit/UnitManager.h"
#include "../../../../../unit/char/CharUnit.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ChangeStat メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ChangeStat::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;
	VcString strwk;

	// ［必須］種類
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "kind"))
	{
		return false;
	}
	if(strwk.equalsIgnoreCase("full_recover"))		{	_kind = KIND_FULL_RECOVER;		}

	// ［オプション（いずれか必須）］対象制御用イベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "ctrl_evid"))
	{
		_evidCtrl = (u16)iwk;
		if(emcp != 0L) { _evidCtrl = emcp->offsetEvid(_evidCtrl); }	// イベント ID のオフセット処理
	}

	// ［オプション（いずれか必須）］対象キャラクタ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "char_id"))
	{
		_charid = (u16)iwk;
	}

	// スキップ確認
	//	※	この Act はスキップすると致命的なバグになりかねないので、
	//		デフォルトでは、スキップを防止すべきと考える
	ASSERTM(TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP), "{ChangeStat::analyze} Illegal skip-flag setting of ChangeStat-Act.");
	if(!TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP))
	{
		TFW_SET_FLAG(_flags, F_NOSKIP, true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ChangeStat::play(ActMatter* owner)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager();

	// 対象を制御用イベント ID かキャラクタ ID から取得する
	CharUnit* cunitTrg = unitmng->findCharUnitFromCtrlEvidOrCharId(_evidCtrl, _charid);
	ASSERT(cunitTrg != 0L);
	CharStat* cstatTrg = (cunitTrg != 0L) ? cunitTrg->charStat() : 0L;
	ASSERT(cstatTrg != 0L);

	switch(_kind)
	{
	case KIND_FULL_RECOVER:
		// 全快
		if(cstatTrg != 0L)	{	cstatTrg->recoverFull();	}
		break;
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
