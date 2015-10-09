/***********************************************************************//**
 *	IntervalSave.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_INTERVAL_SAVE_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_INTERVAL_SAVE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../IntvlSaveMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	途中セーブ Act
 *	
**//*---------------------------------------------------------------------*/
class IntervalSave : public Act
{
public:
	IntvlSaveMatter::Setting _setting;	// 動作設定

public:
	IntervalSave() : Act(ACT_INTVL_SAVE) {}
	~IntervalSave();
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_INTERVAL_SAVE_H_
