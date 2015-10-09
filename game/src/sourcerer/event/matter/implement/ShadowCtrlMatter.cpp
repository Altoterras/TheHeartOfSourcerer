/***********************************************************************//**
 *	ShadowCtrlMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/05/01.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ShadowCtrlMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../../effect/AutoDecorationEffect.h"
#include "../../../body/Game.h"

// External
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ShadowCtrlMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void ShadowCtrlMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool ShadowCtrlMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	VcString swk;
	bool bwk;
	s32 iwk;
	f32 fwk;
	Vector3F vwk;

	// ［必須］追加描画モデルファイル ID
	if(!XmlParserUtils::analyzeAtrb(&iwk, node, "char_id"))
	{
		return false;
	}
	setting->_charid = (u16)iwk;

	// ［オプション］有効・無効
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "enable"))
	{
		setting->_enable = bwk;
		TFW_SET_FLAG(setting->_flagsSet, AutoDecorationEffect::CSCONFF_ENABLE, true);
	}

	// ［オプション］丸影半径
	if(XmlParserUtils::analyzeAtrb(&fwk, node, "radius"))
	{
		setting->_radius = fwk;
		TFW_SET_FLAG(setting->_flagsSet, AutoDecorationEffect::CSCONFF_RADIUS, true);
	}

	// ［オプション］位置オフセット
	if(XmlParserUtils::analyzeAtrb(&vwk, node, "offset_pos"))
	{
		setting->_offsetPos.copy(&vwk);
		TFW_SET_FLAG(setting->_flagsSet, AutoDecorationEffect::CSCONFF_OFFSET, true);
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ShadowCtrlMatter::ShadowCtrlMatter()
	: EventMatter(EMTYPE_SHDOWCTRL)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ShadowCtrlMatter::~ShadowCtrlMatter()
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ShadowCtrlMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
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
void ShadowCtrlMatter::begin(u32 param)
{
	EventMatter::begin(param);

	AutoDecorationEffect* adeff = Game::getGame()->getAutoDecorationEffect(); ASSERT(adeff != 0L);

	// キャラクタ登録
	adeff->registerChar(_setting._charid, false);

	// 設定変更
	adeff->setCircleShadowConfig(
		_setting._flagsSet,
		_setting._charid,
		_setting._enable,
		_setting._radius,
		&_setting._offsetPos	);
}

//==========================================================================
// ShadowCtrlMatter::Setting メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ShadowCtrlMatter::Setting::Setting()
	: _charid(0)
	, _enable(true)		// デフォルトは有効
	, _radius(0.0f)
	, _flagsSet(0)
{
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void ShadowCtrlMatter::Setting::copy(const Setting* s)
{
	_charid = s->_charid;
	_enable = s->_enable;
	_radius = s->_radius;
	_offsetPos.copy(&s->_offsetPos);
	_flagsSet = s->_flagsSet;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
