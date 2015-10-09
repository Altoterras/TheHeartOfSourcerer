/***********************************************************************//**
 *	ValFader.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_VAL_FADER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_VAL_FADER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../ValFaderMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	バリュー フェーダー（浮動小数点値の逐次変更機能）
 *	
**//*---------------------------------------------------------------------*/
class ValFader : public Act
{
public:
	ValFaderMatter::Setting _setting;	// 動作設定

public:
	ValFader() : Act(ACT_VAL_FADER) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_VAL_FADER_H_
