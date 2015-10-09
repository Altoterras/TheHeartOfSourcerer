/***********************************************************************//**
 *	TexFacialMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/18.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TexFacialMatter.h"

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
// TexFacialMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void TexFacialMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool TexFacialMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	VcString swk;
	s32 iwk;
	bool bwk;

	// ［必須］追加描画モデルファイル ID
	if(!XmlParserUtils::analyzeAtrb(&iwk, node, "char_id"))
	{
		return false;
	}
	setting->_charid = (u16)iwk;

	// ［必須］種別
	if(!XmlParserUtils::analyzeAtrb(&swk, node, "kind"))
	{
		return false;
	}
	if(swk.equals("eye"))
	{
		setting->_kind = AutoDecorationEffect::KIND_EYE;
	}
	else if(swk.equals("mouth"))
	{
		setting->_kind = AutoDecorationEffect::KIND_MOUTH;
	}

	// ［オプション］有効・無効
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "enable"))
	{
		setting->_enable = bwk;
		TFW_SET_FLAG(setting->_flagsSet, AutoDecorationEffect::TFCONFF_ENABLE, true);
	}

	// ［オプション］非表示から表示に切り替える確率 0 ～ 100（100 は即時）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "show_rand"))
	{
		setting->_rand = (u16)iwk;
		TFW_SET_FLAG(setting->_flagsSet, AutoDecorationEffect::TFCONFF_NEXTANIMRAND, true);
	}

	// ［オプション］連続表示フレーム数
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "show_durf"))
	{
		setting->_durfShow = iwk;
		TFW_SET_FLAG(setting->_flagsSet, AutoDecorationEffect::TFCONFF_NEXTANIMDURF, true);
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TexFacialMatter::TexFacialMatter()
	: EventMatter(EMTYPE_TEXFACIAL)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TexFacialMatter::~TexFacialMatter()
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool TexFacialMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
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
void TexFacialMatter::begin(u32 param)
{
	EventMatter::begin(param);

	AutoDecorationEffect* adeff = Game::getGame()->getAutoDecorationEffect(); ASSERT(adeff != 0L);

	// キャラクタ登録
	adeff->registerChar(_setting._charid, false);

	// 設定変更
	adeff->setTexFacialConfig(
		_setting._flagsSet,
		_setting._charid,
		_setting._kind,
		_setting._enable,
		(_setting._rand != 0) ? _setting._rand : AutoDecorationEffect::RAND_EYE_NEXT_DEFAULT,
		(_setting._durfShow != 0) ? _setting._durfShow : AutoDecorationEffect::DURF_EYE_NEXT_DEFAULT	);
}

//==========================================================================
// TexFacialMatter::Setting メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TexFacialMatter::Setting::Setting()
	: _charid(0)
	, _kind(0)
	, _enable(true)		// デフォルトは有効
	, _rand(0)
	, _durfShow(0)
	, _flagsSet(0)
{
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void TexFacialMatter::Setting::copy(const Setting* s)
{
	_charid = s->_charid;
	_kind = s->_kind;
	_enable = s->_enable;
	_rand = s->_rand;
	_durfShow = s->_durfShow;
	_flagsSet = s->_flagsSet;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
