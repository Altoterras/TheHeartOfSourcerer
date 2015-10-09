/***********************************************************************//**
 *	QsEmp_CallFinish.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsEmp_CallFinish.h"

// Friends
#include "../QsEmpType.h"
#include "../../../common/SimpleSprite.h"
#include "../../../event/EventSys.h"
#include "../../../event/matter/EventMatterPresvParam.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define MSGWND_CTRL_HINT	675432

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsEmp_CallFinish メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void QsEmp_CallFinish::begin(u32 param)
{
	QsEmp::begin(param);

	// クエストクリアの予約
	reserveFinishQuest();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QsEmp_CallFinish::QsEmp_CallFinish()
	: QsEmp(QSEMPTYPE_CALL_FINISH_QUEST)
{
}

/*---------------------------------------------------------------------*//**
	クエストクリアの予約
**//*---------------------------------------------------------------------*/
void QsEmp_CallFinish::reserveFinishQuest()
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();
	if(qstmng == 0L)										{	return;	}

	EventMatterPresvParam* empp = reinterpret_cast<EventMatterPresvParam*>(this->getPreserveParam());
	if(empp == 0L)											{	return;	}
	if(empp->_kind != EventMatterPresvParam::KIND_QUEST)	{	return;	}

	QuestArticle* qsta = (QuestArticle*)empp->_d;
	if(qsta == 0L)											{	return;	}

	qstmng->reserveFinishQuest(qsta->getDef()->getQuestId());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
