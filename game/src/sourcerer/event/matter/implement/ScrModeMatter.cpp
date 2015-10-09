/***********************************************************************//**
 *	ScrModeMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/05/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ScrModeMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"

// External
#include "../../../../tfw/gcmn/View.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ScrModeMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void ScrModeMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool ScrModeMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	// ［必須］モード
	VcString strWk;
	if(!XmlParserUtils::analyzeAtrb(&strWk, node, "mode"))
	{
		return false;
	}
	if(strWk.equalsIgnoreCase("fix"))			{	setting->_mode = MODE_FIX;		}
	else if(strWk.equalsIgnoreCase("scale"))	{	setting->_mode = MODE_SCALE;	}

	// ［オプション］継続フレーム数
	s32 wk;
	if(XmlParserUtils::analyzeAtrb(&wk, node, "durf"))
	{
		setting->_nframe = wk;
	}


	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ScrModeMatter::ScrModeMatter()
	: EventMatter(EMTYPE_SCRMODE)
	, _vmodeBk(-1)
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ScrModeMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
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
void ScrModeMatter::begin(u32 param)
{
	EventMatter::begin(param);

	if(_setting._mode != MODE_NULL)
	{
		_vmodeBk = (s32)Game::getGame()->getTfw()->getView()->getViewMode();

		Game::getGame()->changeScreenSizeMode(_setting._mode == MODE_SCALE);
	}
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ScrModeMatter::end()
{
	if((_setting._nframe > 0) && (_vmodeBk != -1))
	{
		Game::getGame()->changeScreenSizeMode(_vmodeBk == View::VMODE_SCALE);
	}

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void ScrModeMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	if((_setting._nframe == -1) || (_setting._nframe <= getElapsedFrame()))
	{
		end();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
