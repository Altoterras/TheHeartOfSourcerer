/***********************************************************************//**
 *	GetThing.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GetThing.h"

// Friends
#include "../../ActMatter.h"
#include "../../TalkMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"
#include "../../../../../body/GameFixedStrTbl.h"
#include "../../../../../body/Party.h"
#include "../../../../../gui/GameGui.h"
#include "../../../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../../../item/Item.h"
#include "../../../../../item/ItemDef.h"
#include "../../../../../item/ItemDefTbl.h"
#include "../../../../../item/ItemManager.h"
#include "../../../../../quest/QuestManager.h"
#include "../../../../../source/FixedSourceDef.h"
#include "../../../../../source/SourceManager.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GetThing メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool GetThing::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;
	VcString strwk;
	bool bwk;
	s32* iarrWk = 0L;
	s32 numArr = 0;

	// ［必須］種類
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "kind"))
	{
		return false;
	}
	if(strwk.equalsIgnoreCase("fixed_source"))		{	_kind = KIND_FIXED_SOURCE;	}
	if(strwk.equalsIgnoreCase("item"))				{	_kind = KIND_ITEM;			}

	// ［オプション］パラメータ
	CcString cstrParam("param");
	if(XmlParserUtils::analyzeAtrb(&iwk, node, &cstrParam))
	{
		_param = iwk;
	}

	// ［オプション］現質の元素エネルギー量
	if(XmlParserUtils::analyzeAtrb(&iarrWk, &numArr, node, "cql_elene"))
	{
		for(int i = 0; (i < Elene::NUM_EL) && (i < numArr); i++)
		{
			_eleneCql.set(i, iarrWk[i] * 100);
		}

		delete[] iarrWk;
	}

	// ［オプション］ベール１の元素エネルギー量
	if(XmlParserUtils::analyzeAtrb(&iarrWk, &numArr, node, "veil1_elene"))
	{
		for(int i = 0; (i < Elene::NUM_EL) && (i < numArr); i++)
		{
			_eleneVeil1.set(i, iarrWk[i] * 100);
		}

		delete[] iarrWk;
	}

	// ［オプション］取得メッセージを表示するかどうか
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "show_get_msg"))
	{
		_isShowGetMsg = bwk;
	}

	// ［オプション］取得できなかったメッセージを表示するかどうか
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "show_noget_msg"))
	{
		_isShowNogetMsg = bwk;
	}

	// スキップ確認
	//	※	この Act はスキップすると致命的なバグになりかねないので、
	//		デフォルトでは、スキップを防止すべきと考える
	ASSERTM(TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP), "{GetThing::analyze} Illegal skip-flag setting of GetThing-Act.");
	if(!TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP))
	{
		TFW_SET_FLAG(_flags, F_NOSKIP, true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void GetThing::play(ActMatter* owner)
{
	// 作業変数の値クリア
	_evid = 0;

	// 取得処理とメッセージ表示
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	switch(_kind)
	{
	case KIND_FIXED_SOURCE:
		{
			SourceManager* srcmng = party->getSourceManager(); ASSERT(srcmng != 0L);
			const FixedSourceDef* fsrcdef = srcmng->addFixedSource((u16)_param);
			if(fsrcdef != 0L)	// 追加された
			{			
				///srcmng->sortForEmptySource();	// 空きソースを並び替える

				if(_isShowGetMsg)
				{
					// ソース取得メッセージ表示
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_SOURCE)->getRaw(), fsrcdef->getName()->getRaw(), ""));
					showMessage(&msg, owner);
				}
			}
			else
			{
				if(_isShowNogetMsg)
				{
					// Nothing メッセージ表示
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_FOUND)->getRaw(), ""));
					showMessage(&msg, owner);
				}
			}
		}
		break;
	case KIND_ITEM:
		{
			ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);

			u16 itmdid = (u16)_param;
			bool isSuccess = false;

			const ItemDefTbl* itmdftbl = itmmng->getItemDefTableRef();
			const ItemDef* itmdf = itmdftbl->getDef(itmdid);
			if(itmdf != 0L)
			{
				// デフォルトのエネルギーバランス決定
				EleneStat eestat;
				eestat.setup(itmdf->getOqlElene(), _eleneCql.isInitialValue() ? itmdf->getOqlElene() : &_eleneCql);
				if(!_eleneVeil1.isInitialValue())
				{
					eestat.veil(0)->elene()->copy(&_eleneVeil1);
					eestat.updateResultValue(true);
				}

				// アイテム取得
				s32 indexItem = itmmng->newItem(itmdid, &eestat, 0, false);
				if(indexItem != -1)
				{
					if(_isShowGetMsg)
					{
						// アイテム取得メッセージ表示
						VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_SOMETHING)->getRaw(), itmdf->getName(Env_getLangId())->getRaw(), ""));
						showMessage(&msg, owner);
					}

					// クエストの可変達成状態の更新
					QuestManager* qsmng = Game::getGame()->getQuestManager();
					qsmng->updateVariableAchievementState();

					isSuccess = true;
				}
			}

			if(!isSuccess)
			{
				if(_isShowNogetMsg)
				{
					// Nothing メッセージ表示
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_FOUND)->getRaw(), ""));
					showMessage(&msg, owner);
				}
			}
		}
		break;
	}
	
	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void GetThing::stop(ActMatter* owner)
{
	// メッセージを終了する
	if(_evid != 0)
	{
		EventData* evdat = owner->getEventSystemRefer()->getEventData();	ASSERT(evdat != 0L);
		TalkMatter* tm = (TalkMatter*)evdat->getMatterFromEvid(_evid);		ASSERT(tm != 0L);
		if(tm != 0L)
		{
			tm->terminate();
		}
	}

	Act::stop(owner);
}

/*---------------------------------------------------------------------*//**
	メッセージを表示する
**//*---------------------------------------------------------------------*/
void GetThing::showMessage(const VcString* msg, ActMatter* owner)
{
	// メッセージを表示する
	EventData* evdat = owner->getEventSystemRefer()->getEventData();	ASSERT(evdat != 0L);
	_evid = evdat->addChildMatter(owner->getEvId(), "TALK");			ASSERT(_evid != 0);
	TalkMatter* tm = (TalkMatter*)evdat->getMatterFromEvid(_evid);		ASSERT(tm != 0L);
	tm->setMessage(msg, 1, false);			// メッセージ設定
	tm->setWindowType(true);				// システムウインドウに設定
	tm->setKind(TalkMatter::KIND_TALK);		// 種別設定
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	// 一時停止設定
	TFW_SET_FLAG(_flags, F_PAUSE, true);
	_duration = -1;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
