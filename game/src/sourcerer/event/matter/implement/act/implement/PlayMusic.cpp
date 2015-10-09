/***********************************************************************//**
 *	PlayMusic.cpp
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
#include "PlayMusic.h"

// Friends
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
// PlayMusic メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PlayMusic::PlayMusic()
	: Act(ACT_PLAY_MUSIC)
	, _bgmid(0)
	, _cntLoop(0)
	, _volume(SoundMngr::VOLUME_BGM_DEFAULT)
	, _frameFadeOut(SoundMngr::FRAME_FADE_OUT_DEFAULT)
	, _frameFadeIn(SoundMngr::FRAME_FADE_IN_DEFAULT)
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool PlayMusic::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;
	f32 fwk;

	// ［必須］BGM ID
	if(!XmlParserUtils::analyzeAtrb(&iwk, node, "bgm_id"))
	{
		return false;
	}
	_bgmid = (u16)iwk;

	// ［オプション］ループするかどうか
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "loop_cnt"))
	{
		_cntLoop = iwk;
	}

	// ［オプション］音量
	if(XmlParserUtils::analyzeAtrb(&fwk, node, "volume"))
	{
		_volume = fwk;
	}

	// ［オプション］フェードアウトフレーム数
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "fade_out_frame_num"))
	{
		_frameFadeOut = iwk;
	}

	// ［オプション］フェードインフレーム数
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "fade_in_frame_num"))
	{
		_frameFadeIn = iwk;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void PlayMusic::play(ActMatter* owner)
{
	SoundMngr* sndmngr = Game::getGame()->getSoundManager();
	if(_bgmid != 0)
	{
		sndmngr->playBgm(_bgmid, _cntLoop, _volume, true, _frameFadeOut, _frameFadeIn, CTRLKEY_BGM_ACT);
	}
	else
	{
		sndmngr->stopBgm(_frameFadeOut);
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
