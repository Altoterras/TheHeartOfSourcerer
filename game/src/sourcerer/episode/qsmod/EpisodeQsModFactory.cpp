/***********************************************************************//**
 *	EpisodeQsModFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EpisodeQsModFactory.h"

// Friends
#include "implement/QsMod10101.h"
#include "implement/QsMod10102.h"
#include "implement/QsMod10103.h"
#include "../../body/Game.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsModFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ストーリー作成
**//*---------------------------------------------------------------------*/
QsMod* EpisodeQsModFactory::newQsMod(s32 questid, QuestArticle* qsdatRef)
{
	switch(questid)
	{
	case 10101:	return new QsMod10101();
	case 10102:	return new QsMod10102();
	case 10103:	return new QsMod10103();
	}
	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
