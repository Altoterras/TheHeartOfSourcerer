/***********************************************************************//**
 *	ScrMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/05/20.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SCR_MODE_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SCR_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../ScrModeMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリーンモード
 *	
**//*---------------------------------------------------------------------*/
class ScrMode : public Act
{
public:
	ScrModeMatter::Setting _setting;	// 動作設定

public:
	ScrMode() : Act(ACT_SCR_MODE) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SCR_MODE_H_
