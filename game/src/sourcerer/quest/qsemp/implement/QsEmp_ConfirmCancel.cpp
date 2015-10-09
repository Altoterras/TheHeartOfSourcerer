/***********************************************************************//**
 *	QsEmp_ConfirmCancel.cpp
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
#include "QsEmp_ConfirmCancel.h"

// Friends
#include "../QsEmpType.h"
#include "../../../event/EventData.h"
#include "../../../event/EventSys.h"
#include "../../../event/matter/EventMatterPresvParam.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
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
// QsEmp_ConfirmCancel メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void QsEmp_ConfirmCancel::begin(u32 param)
{
	QsEmp::begin(param);

	// 値クリア
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QsEmp_ConfirmCancel::QsEmp_ConfirmCancel()
	: QsEmp(QSEMPTYPE_CONFIRM_CANCEL_QUEST)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理実装
**//*---------------------------------------------------------------------*/
void QsEmp_ConfirmCancel::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	enum Step
	{
		STEP_NULL,
		STEP_WAIT_BEGIN,
		STEP_WAIT_YESNO,
		STEP_WAIT_FOUT,
	};

	GameGui* gui = Game::getGame()->getGui(); ASSERT(gui != 0L);
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)gui->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);

	switch(getStep())
	{
	case STEP_WAIT_BEGIN:		// 継続監視
		///STRACE("!!! WAIT...\n");
		if(!getOwner()->checkBegunOtherMatter(this, true))	// 自身以外のイベントが起動していない
		{
			// メッセージを得る
			EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);
			const MsgDataSet* msgdatset = evdat->getMsgDataSet(); ASSERT(msgdatset != 0L);
			// YES / NO メッセージウインドウを表示する
			wndSysMsg->setKind(MessageWindow::KIND_YESNO);
			wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_QUEST_CONFIRM_CANCEL), MSGWND_CTRL_HINT);

			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_YESNO:
		{
			const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
			if(res->isClickYesButton())
			{
				// クエストキャンセルの予約
				reserveCancelQuest();

				// ウインドウを閉じる
				wndSysMsg->showWindow(false);
				// 終了
				end();
			}
			else if(res->isClickNoButton())
			{
				// ウインドウを閉じる
				wndSysMsg->showWindow(false);
				// 終了
				end();
				// 親マターも終了してイベントを閉じる
				EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);
				EvMatter* matter = evdat->getMatterFromEvid(getParentEvId());
				if(matter != 0L) { matter->end(); }
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	クエストキャンセル
**//*---------------------------------------------------------------------*/
void QsEmp_ConfirmCancel::reserveCancelQuest()
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();
	if(qstmng == 0L)										{	return;	}

	EventMatterPresvParam* empp = reinterpret_cast<EventMatterPresvParam*>(this->getPreserveParam());
	if(empp == 0L)											{	return;	}
	if(empp->_kind != EventMatterPresvParam::KIND_QUEST)	{	return;	}

	QuestArticle* qsta = (QuestArticle*)empp->_d;
	if(qsta == 0L)											{	return;	}

	qstmng->reserveCancelQuest(qsta->getDef()->getQuestId());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
