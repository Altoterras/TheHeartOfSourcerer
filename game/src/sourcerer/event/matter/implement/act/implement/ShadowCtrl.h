/***********************************************************************//**
 *	ShadowCtrl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/05/01.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SHADOW_CTRL_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SHADOW_CTRL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../ShadowCtrlMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	影制御
 *	
**//*---------------------------------------------------------------------*/
class ShadowCtrl : public Act
{
public:
	ShadowCtrlMatter::Setting _setting;	// 動作設定

public:
	ShadowCtrl() : Act(ACT_SHADOW_CTRL) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SHADOW_CTRL_H_
