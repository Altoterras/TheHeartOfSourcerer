/***********************************************************************//**
 *	CamConf.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CAM_CONF_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CAM_CONF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カメラ設定 Act
 *	
**//*---------------------------------------------------------------------*/
class CamConf : public Act
{
public:
	bool			_isIntp;		// ［オプション］補間するかどうか

public:
	CamConf() : Act(ACT_CAM_CONF), _isIntp(false) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CAM_CONF_H_
