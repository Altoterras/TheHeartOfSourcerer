/***********************************************************************//**
 *	TexFacial.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/18.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_TEX_FACIAL_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_TEX_FACIAL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../TexFacialMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	テクスチャ フェイシャル
 *	
**//*---------------------------------------------------------------------*/
class TexFacial : public Act
{
public:
	TexFacialMatter::Setting _setting;	// 動作設定

public:
	TexFacial() : Act(ACT_TEX_FACIAL) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_TEX_FACIAL_H_
