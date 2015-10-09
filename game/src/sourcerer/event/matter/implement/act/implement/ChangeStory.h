/***********************************************************************//**
 *	ChangeStory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_STORY_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_STORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ストーリー変更 Act
 *	
**//*---------------------------------------------------------------------*/
class ChangeStory : public Act
{
public:
	s32				_storyid;		// ［必須］ストーリー ID

public:
	ChangeStory() : Act(ACT_CHANGE_STORY), _storyid(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_STORY_H_
