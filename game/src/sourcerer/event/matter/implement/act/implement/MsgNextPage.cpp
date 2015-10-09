/***********************************************************************//**
 *	Msg.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MsgNextPage.h"

// Friends
#include "Msg.h"
#include "../../ActMatter.h"
#include "../../TalkMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../tfw/collection/List.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MsgNextPage メソッド

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void MsgNextPage::play(ActMatter* owner)
{
	// 実行中の Act から Msg を探し次のページ送りを指示する．なくてもエラーにしない
	for(ListIterator<Act*> it = owner->getPlayingActList()->iterator(); it.has(); it.next())
	{
		Act* act = it.object();
		if(act->_type == ACT_MSG)
		{
			EventData* evdat = owner->getEventSystemRefer()->getEventData();		ASSERT(evdat != 0L);

			Msg* msg = (Msg*)act;
			TalkMatter* tm = (TalkMatter*)evdat->getMatterFromEvid(msg->_evid);		ASSERT(tm != 0L);
			tm->nextPage();
			break;
		}
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
