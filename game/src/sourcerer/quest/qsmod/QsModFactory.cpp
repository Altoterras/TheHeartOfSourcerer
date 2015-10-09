/***********************************************************************//**
 *	QsModFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsModFactory.h"

// Friends
#include "QsMod.h"
#include "../../episode/qsmod/EpisodeQsModFactory.h"
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
QsMod* QsModFactory::makeQsMod(s32 questid, QuestArticle* qsdatRef)
{
	QsMod* qsmod = 0L;

	/*
	switch(questid)
	{
	case xxxx:	qsmod = new QsModXXXX();	break;
	}
	*/
	if(qsmod == 0L)
	{
		qsmod = EpisodeQsModFactory::newQsMod(questid, qsdatRef);
	}

	if(qsmod == 0L)
	{
		return 0L;
	}

	if(!qsmod->create(qsdatRef))
	{
		delete qsmod;
		return 0L;
	}

	return qsmod;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
