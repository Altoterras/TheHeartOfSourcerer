/***********************************************************************//**
 *	ObtainCtrl.cpp
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
#include "ObtainCtrl.h"

// Friends
#include "PlayMusic.h"
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../etk/sound/SoundMngr.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ObtainCtrl メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ObtainCtrl::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］ターゲット
	VcString strTarget;
	if(!XmlParserUtils::analyzeAtrb(&strTarget, node, "target"))
	{
		return false;
	}
	if(strTarget.equalsIgnoreCase("music") || strTarget.equalsIgnoreCase("bgm"))
	{
		_target = T_BGM;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ObtainCtrl::play(ActMatter* owner)
{
	// 制御制限開始
	switch(_target)
	{
	case T_BGM:
		Game::getGame()->getSoundManager()->restrictControlBgm(PlayMusic::CTRLKEY_BGM_ACT);
		break;
	}

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void ObtainCtrl::stop(ActMatter* owner)
{
	/*
	// 制御制限終了
	switch(_target)
	{
	case T_BGM:
		Game::getGame()->getSoundManager()->restrictControlBgm(0);
		break;
	}
	*/

	Act::stop(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
