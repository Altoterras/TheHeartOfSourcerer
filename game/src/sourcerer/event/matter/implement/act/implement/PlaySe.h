/***********************************************************************//**
 *	PlaySe.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PLAY_SE_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PLAY_SE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../SeMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	SE 再生 Act
 *	
**//*---------------------------------------------------------------------*/
class PlaySe : public Act
{
public:
	SeMatter::Setting _setting;	// 動作設定

public:
	PlaySe() : Act(ACT_PLAY_SE) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PLAY_SE_H_
