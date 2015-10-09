/***********************************************************************//**
 *	Act.cpp
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
#include "Act.h"

// Friends

// External
#include "../../../../../tfw/lib/XmlParser.h"
#include "../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Act メソッド

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Act::~Act()
{
	delete _name;
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Act::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	// ［必須］開始フレーム
	s32 frame;
	if(!XmlParserUtils::analyzeAtrb(&frame, node, "frame"))
	{
		return false;
	}

	// ［オプション］継続フレーム
	if(TFW_IS_FLAG(_flags, F_PAUSE) && (_duration == 0))	{	_duration = -1;	}	// ポーズ Act は自力終了するので無限デュレーションを設定する
	s32 duration = _duration;
	XmlParserUtils::analyzeAtrb(&duration, node, "durf");

	// ［オプション］名前
	VcString strWk;
	if(XmlParserUtils::analyzeAtrb(&strWk, node, "name"))
	{
		_name = new VcString(strWk);
	}

	// ［オプション］スキップ設定
	VcString skipval;
	if(XmlParserUtils::analyzeAtrb(&skipval, node, "skip"))
	{
		if(skipval.equalsIgnoreCase("no"))			// スキップ中でも実行する
		{
			TFW_SET_FLAG(_flags, F_NOSKIP, true);
		}
		else if(skipval.equalsIgnoreCase("only"))	// スキップ中のみ実行する
		{
			TFW_SET_FLAG(_flags, F_SKIPONLY, true);
		}
		else if(skipval.equalsIgnoreCase("stop"))	// スキップ強制停止
		{
			TFW_SET_FLAG(_flags, F_SKIPSTOP, true);
		}
	}

	// 値を設定
	_frame = frame;
	_duration = duration;

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
