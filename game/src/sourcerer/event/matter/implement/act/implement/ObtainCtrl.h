/***********************************************************************//**
 *	ObtainCtrl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_OBTAIN_CTRL_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_OBTAIN_CTRL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	制御権を取得する
 *	
**//*---------------------------------------------------------------------*/
class ObtainCtrl : public Act
{
private:
	enum Target
	{
		T_NULL,
		T_BGM,
	};

public:
	Target			_target;		// ［必須］ターゲット

public:
	ObtainCtrl() : Act(ACT_OBTAIN_CTRL), _target(T_NULL) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_OBTAIN_CTRL_H_
