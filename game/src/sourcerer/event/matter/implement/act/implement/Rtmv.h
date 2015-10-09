/***********************************************************************//**
 *	Rtmv.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_RTMV_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_RTMV_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	リアルタイムムービー Act
 *	
**//*---------------------------------------------------------------------*/
class Rtmv : public Act
{
public:
	u32				_fileid;		// ［必須］ファイル ID

public:
	Rtmv() : Act(ACT_RTMV), _fileid(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_RTMV_H_
