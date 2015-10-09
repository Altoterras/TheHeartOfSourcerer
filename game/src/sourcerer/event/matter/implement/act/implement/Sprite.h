/***********************************************************************//**
 *	Sprite.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SPRITE_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SPRITE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../SpriteMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スプライトの表示
 *	
**//*---------------------------------------------------------------------*/
class Sprite : public Act
{
public:
	SpriteMatter::Setting _setting;	// 動作設定

public:
	Sprite() : Act(ACT_SPRITE) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SPRITE_H_
