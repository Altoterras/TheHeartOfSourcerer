/***********************************************************************//**
 *	QsEmp_ConfirmHandOver.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsEmp_ConfirmHandOver.h"

// Friends
#include "../QsEmpType.h"
#include "../../../char/CharDef.h"
#include "../../../char/CharDefTbl.h"
#include "../../../episode/data/FixedData.h"
#include "../../../event/EventData.h"
#include "../../../event/EventSys.h"
#include "../../../event/matter/EventMatterPresvParam.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../item/ItemDef.h"
#include "../../../item/ItemDefTbl.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define MSGWND_CTRL_HINT	675432

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsEmp_ConfirmHandOver メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void QsEmp_ConfirmHandOver::begin(u32 param)
{
	QsEmp::begin(param);

	// 値クリア
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QsEmp_ConfirmHandOver::QsEmp_ConfirmHandOver()
	: QsEmp(QSEMPTYPE_CONFIRM_HANDOVER_THING)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理実装
**//*---------------------------------------------------------------------*/
void QsEmp_ConfirmHandOver::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	enum Step
	{
		STEP_NULL,
		STEP_WAIT_BEGIN,
		STEP_WAIT_YESNO,
	};

	GameGui* gui = Game::getGame()->getGui(); ASSERT(gui != 0L);
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)gui->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect(); ASSERT(screff != 0L);

	switch(getStep())
	{
	case STEP_WAIT_BEGIN:		// 継続監視
		///STRACE("!!! WAIT...\n");
		if(!getOwner()->checkBegunOtherMatter(this, true))	// 自身以外のイベントが起動していない
		{
			// 誰に何を
			VcString nameWho, nameThing;
			for(int i = 0; i < this->getParamNum(); i++)
			{
				const VcString* key = this->getParamKey(i);
				if(key != 0L)
				{
					if(key->equals("who_char_id"))
					{
						const VcString* valu = this->getParamValue(i);
						u16 chrdid = StringUtils::toInteger(valu);
						const CharDef* chrdef = FixedData::getCharDefTable()->getDef(chrdid);
						if(chrdef != 0L)
						{
							nameWho.set(chrdef->getName(Env_getLangId()));
						}
					}
					else if(key->equals("what_item_id"))
					{
						const VcString* valu = this->getParamValue(i);
						u16 itmdid = StringUtils::toInteger(valu);
						const ItemDef* itmdef = FixedData::getItemDefTable()->getDef(itmdid);
						if(itmdef != 0L)
						{
							nameThing.set(itmdef->getName(Env_getLangId()));
						}
					}
				}
			}

			// メッセージ作成
			EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);
			const MsgDataSet* msgdatset = evdat->getMsgDataSet(); ASSERT(msgdatset != 0L);
			const VcString* msgTmp = GameFixedStrTbl::getString(GameFixedStrTbl::SID_QUEST_CONFIRM_HANDOVER);
			if(msgTmp == 0L) { msgTmp = TypeUtils::getEmptyString(); }
			VcString msg(VcString::format(msgTmp->getRaw(), nameWho.getRaw(), nameThing.getRaw()));

			// YES / NO メッセージウインドウを表示する
			wndSysMsg->setKind(MessageWindow::KIND_YESNO);
			wndSysMsg->showMessage(&msg, MSGWND_CTRL_HINT);

			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_YESNO:
		{
			bool isNo = false;
			const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
			if(res->isClickYesButton())
			{
				// ものを手渡す
				if(!handoverThing())
				{
					// 失敗
					isNo = true;
				}

				// ウインドウを閉じる
				wndSysMsg->showWindow(false);
				// 終了
				end();
			}
			else if(res->isClickNoButton())
			{
				isNo = true;
				// ウインドウを閉じる
				wndSysMsg->showWindow(false);
				// 終了
				end();
			}
			if(isNo)
			{
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
	ものを手渡す
**//*---------------------------------------------------------------------*/
bool QsEmp_ConfirmHandOver::handoverThing()
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();
	if(qstmng == 0L)										{	return false;	}

	EventMatterPresvParam* empp = reinterpret_cast<EventMatterPresvParam*>(this->getPreserveParam());
	if(empp == 0L)											{	return false;	}
	if(empp->_kind != EventMatterPresvParam::KIND_QUEST)	{	return false;	}

	QuestArticle* qsta = (QuestArticle*)empp->_d;
	if(qsta == 0L)											{	return false;	}

	return qstmng->handoverThing(qsta->getDef()->getQuestId());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
