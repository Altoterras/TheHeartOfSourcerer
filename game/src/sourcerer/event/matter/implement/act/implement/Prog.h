/***********************************************************************//**
 *	Prog.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PROG_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PROG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../ProgMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カスタムプログラム実行 Act
 *	
**//*---------------------------------------------------------------------*/
class Prog : public Act
{
public:
	VcString* _name;				// ［必須］プログラム名
	ProgMatter::Setting _setting;	// 動作設定

public:
	Prog() : Act(ACT_PROG), _name(0) {}
	~Prog();
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PROG_H_
