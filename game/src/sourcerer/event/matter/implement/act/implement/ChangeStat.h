/***********************************************************************//**
 *	ChangeStat.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_STAT_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ステータス変更 Act
 *	
**//*---------------------------------------------------------------------*/
class ChangeStat : public Act
{
private:
	enum KIND
	{
		KIND_NULL,
		KIND_FULL_RECOVER,
	};

public:
	KIND			_kind;				// ［必須］種類
	u16				_evidCtrl;			// ［オプション（いずれか必須）］対象を指定する制御用イベント ID （優先）
	u16				_charid;			// ［オプション（いずれか必須）］キャラクタ ID

public:
	ChangeStat() : Act(ACT_CHANGE_STAT), _kind(KIND_NULL), _evidCtrl(0), _charid(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_STAT_H_
