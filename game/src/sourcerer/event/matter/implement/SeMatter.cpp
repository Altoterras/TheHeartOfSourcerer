/***********************************************************************//**
 *	SeMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SeMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"

// External
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SeMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void SeMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool SeMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	// ［必須］SE ID
	s32 seid;
	if(XmlParserUtils::analyzeAtrb(&seid, node, "se_id"))
	{
		setting->_seid = (u16)seid;
	}

	// ［オプション］ループするかどうか
	bool isLoop;
	if(XmlParserUtils::analyzeAtrb(&isLoop, node, "loop"))
	{
		setting->_isLoop = isLoop;
	}

	// ［オプション］音量
	f32 wk;
	if(XmlParserUtils::analyzeAtrb(&wk, node, "volume"))
	{
		setting->_volume = wk;
	}

	// ［オプション］継続フレーム
	s32 duration = setting->_isLoop ? 30 : 1;	// 指定なしはループ SE は 30 フレーム、非ループ SE は 1 フレーム
	XmlParserUtils::analyzeAtrb(&duration, node, "durf");
	setting->_duration = (f32)duration;

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SeMatter::SeMatter()
	: EventMatter(EMTYPE_SE)
	, _sech(0)
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool SeMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 動作設定解析

	analyzeSettingFromXml(&_setting, node);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void SeMatter::begin(u32 param)
{
	EventMatter::begin(param);

	// SE 再生
	SoundMngr* sndmng = Game::getGame()->getSoundManager();
	_sech = sndmng->playSe(_setting._seid, _setting._isLoop, _setting._volume, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void SeMatter::end()
{
	// SE 停止
	SoundMngr* sndmng = Game::getGame()->getSoundManager();
	if((_setting._isLoop) && (_sech > 0))
	{
		sndmng->stopSe(_sech);
	}

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void SeMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	if(frameBegun >= _setting._duration)
	{
		end();
	}
}

//==========================================================================
// SeMatter::Setting メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SeMatter::Setting::Setting()
	: _seid(0)
	, _isLoop(false)
	, _volume(0.0f)
	, _duration(0)
{
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void SeMatter::Setting::copy(const Setting* s)
{
	_seid = s->_seid;
	_isLoop = s->_isLoop;
	_volume = s->_volume;
	_duration = s->_duration;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
