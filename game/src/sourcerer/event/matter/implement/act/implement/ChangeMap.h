/***********************************************************************//**
 *	ChangeMap.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_MAP_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_MAP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	マップ変更 Act
 *	
**//*---------------------------------------------------------------------*/
class ChangeMap : public Act
{
public:
	s32				_mapid;			// ［必須］マップ ID
	bool			_isRsvChange;	// ［オプション］次のフレームで変更
	bool			_isForceChange;	// ［オプション］同じマップ番号などでも強制的にマップ読込処理を行う

public:
	ChangeMap() : Act(ACT_CHANGE_MAP), _mapid(0), _isRsvChange(false), _isForceChange(false) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CHANGE_MAP_H_
