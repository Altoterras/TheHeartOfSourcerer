/***********************************************************************//**
 *	Mcut.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MCUT_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MCUT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../MovieCutMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ムービーカット定義
 *	
**//*---------------------------------------------------------------------*/
class Mcut : public Act
{
public:
	MovieCutMatter::Setting _setting;	// 動作設定

public:
	Mcut() : Act(ACT_MCUT) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MCUT_H_
