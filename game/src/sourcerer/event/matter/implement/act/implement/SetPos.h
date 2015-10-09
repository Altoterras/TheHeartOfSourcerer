/***********************************************************************//**
 *	SetPos.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SET_POS_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SET_POS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	姿勢変更 Act
 *	
**//*---------------------------------------------------------------------*/
class SetPos : public Act
{
private:
	static const s32		CHARID_CAM = 0xffffffff;

public:
	s32				_charid;		// ［オプション（いずれか必須）］キャラクタ ID
	Vector3F*		_pos;			// ［オプション］設定位置
	Vector3F*		_dir;			// ［オプション］設定向き

public:
	SetPos() : Act(ACT_SET_POS), _charid(0), _pos(0L), _dir(0L) {}
	~SetPos();
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_SET_POS_H_
