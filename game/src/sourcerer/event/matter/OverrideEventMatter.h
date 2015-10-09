/***********************************************************************//**
 *	OverrideEventMatter.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_OVERRIDE_EVENT_MATTER_H_
#define _SRCR_EVENT_MATTER_OVERRIDE_EVENT_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/evsys/OverrideEvMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class EventMatterCreateParam;
class Game;

/*---------------------------------------------------------------------*//**
 *	置換イベント マター情報
 *
**//*---------------------------------------------------------------------*/
class OverrideEventMatter : public OverrideEvMatter
{
	//======================================================================
	// 定数
public:
	// 情報の種別
	enum Kind
	{
		KIND_NULL,
		KIND_MAP,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 適用するマップ ID を得る
	inline s32 getMapId() const { return _mapid; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setupFromXmlNode(const XmlNode* node, const EventMatterCreateParam* emcp);

	bool isCondition() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OverrideEventMatter();

	//======================================================================
	// 変数
private:
	// 追加情報
	s32 _mapid;		// マップ ID
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EVENT_MATTER_OVERRIDE_EVENT_MATTER_H_
