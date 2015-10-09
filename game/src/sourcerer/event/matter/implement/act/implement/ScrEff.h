/***********************************************************************//**
 *	ScrEff.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SCR_EFF_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SCR_EFF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../ScrEffMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリーンエフェクト
 *	
**//*---------------------------------------------------------------------*/
class ScrEff : public Act
{
public:
	ScrEffMatter::Setting _setting;	// 動作設定

public:
	ScrEff() : Act(ACT_SCR_EFF) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SCR_EFF_H_
