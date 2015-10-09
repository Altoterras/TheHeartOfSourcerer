/***********************************************************************//**
 *	ScrEffMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ScrEffMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ScrEffMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void ScrEffMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool ScrEffMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	// ［必須］種類
	VcString strWk;
	if(!XmlParserUtils::analyzeAtrb(&strWk, node, "kind"))
	{
		return false;
	}
	if(strWk.equalsIgnoreCase("fill"))			{	setting->_kind = KIND_FILL;		}
	else if(strWk.equalsIgnoreCase("fade_in"))	{	setting->_kind = KIND_FADEIN;	}
	else if(strWk.equalsIgnoreCase("fade_out"))	{	setting->_kind = KIND_FADEOUT;	}

	// ［必須］フレーム数
	s32 wk;
	if(!XmlParserUtils::analyzeAtrb(&wk, node, "durf"))
	{
		return false;
	}
	setting->_nframe = wk;

	// ［オプション］色
	if(XmlParserUtils::analyzeAtrb(&strWk, node, "color"))
	{
		if(strWk.getLength() <= 6)
		{
			setting->_rgb = StringUtils::toInteger(&strWk, 16);
		}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ScrEffMatter::ScrEffMatter()
	: EventMatter(EMTYPE_SCREFF)
	, _hintCtrlScreff(0)
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ScrEffMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
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
void ScrEffMatter::begin(u32 param)
{
	EventMatter::begin(param);

	if(_setting._kind != KIND_NULL)
	{
		ScreenEffect* screff = Game::getGame()->getSceneScreenEffect();

		// 色設定
		if(_setting._rgb != -1)	// -1 は未設定
		{
			ColorU8 col((u8)((_setting._rgb >> 16) & 0xff), (u8)((_setting._rgb >> 8) & 0xff), (u8)(_setting._rgb & 0xff), 255);
			screff->setColor1(&col);
		}

		// フレーム数設定
		screff->setMaxFrame((f32)_setting._nframe);

		// 実行
		u8 modeScreff = ScreenEffect::MODE_NULL;
		switch(_setting._kind)
		{
		case KIND_FILL:		modeScreff = ScreenEffect::MODE_FILL;		break;
		case KIND_FADEIN:	modeScreff = ScreenEffect::MODE_FADE_IN;	break;
		case KIND_FADEOUT:	modeScreff = ScreenEffect::MODE_FADE_OUT;	break;
		}
		_hintCtrlScreff = screff->start(modeScreff);
	}
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ScrEffMatter::end()
{
	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect();
	screff->end(_hintCtrlScreff);

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void ScrEffMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect();
	if(!screff->isWorking())
	{
		end();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
