/***********************************************************************//**
 *	GetThing.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_GET_THING_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_GET_THING_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../../../../status/Elene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アイテムなどの取得 Act
 *	
**//*---------------------------------------------------------------------*/
class GetThing : public Act
{
private:
	enum KIND
	{
		KIND_NULL,
		KIND_FIXED_SOURCE,
		KIND_ITEM,
	};

public:
	KIND			_kind;				// ［必須］種類
	s32				_param;				// ［オプション］パラメータ
	Elene			_eleneCql;			// ［オプション］現質の元素エネルギー量
	Elene			_eleneVeil1;		// ［オプション］ベール１の元素エネルギー量
	bool			_isShowGetMsg;		// ［オプション］取得メッセージを表示するかどうか
	bool			_isShowNogetMsg;	// ［オプション］取得できなかったメッセージを表示するかどうか

public:
	GetThing() : Act(ACT_GET_THING), _kind(KIND_NULL), _param(0), _isShowGetMsg(false), _isShowNogetMsg(false) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);

private:
	void showMessage(const VcString* msg, ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_GET_THING_H_
