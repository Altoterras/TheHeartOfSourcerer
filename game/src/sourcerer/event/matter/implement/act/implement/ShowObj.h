/***********************************************************************//**
 *	ShowObj.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SHOW_OBJ_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SHOW_OBJ_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	オブジェクト表示／非表示 Act
 *	
**//*---------------------------------------------------------------------*/
class ShowObj : public Act
{
public:
	bool			_show;				// ［必須］表示か非表示か
	u16				_evidCtrlTrg;		// ［オプション（いずれか必須）］対象制御用イベント ID （優先）
	u16				_charidTrg;			// ［オプション（いずれか必須）］対象キャラクタ ID

public:
	ShowObj() : Act(ACT_SHOW_OBJ), _show(false), _evidCtrlTrg(0), _charidTrg(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SHOW_OBJ_H_
