/***********************************************************************//**
 *	IntvlSaveMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "IntvlSaveMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../EventData.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/GameTaskModDef.h"
#include "../../../gui/GameFontSet.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/SaveLoadWindow.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../save/SaveData.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../tfw/framemod/FedTaskSys.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../tfw/string/StringUtils.h"

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IntvlSaveMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void IntvlSaveMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool IntvlSaveMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IntvlSaveMatter::IntvlSaveMatter()
	: EventMatter(EMTYPE_INTVLSAVE)
	, _wndSave(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IntvlSaveMatter::~IntvlSaveMatter()
{
	ASSERT(_wndSave == 0L);
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool IntvlSaveMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 動作設定解析

	analyzeSettingFromXml(&_setting, node);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void IntvlSaveMatter::begin(u32 param)
{
	// 自身が途中セーブデータのロード時は実行しない
	if(this->getCause()->getCid() == EventCause::CID_INTVLEVSTART)
	{
		// 終了して抜ける
		EventMatter::end();
		return;
	}
	// 親が途中セーブデータのロード時は実行しない
	u16 evidParent = getParentEvId();
	if(evidParent != 0L)
	{
		EventMatter* matterParent = (EventMatter*)getOwner()->getEvData()->getMatterFromEvid(evidParent);
		if((matterParent != 0L) && matterParent->isBegin())
		{
			if(matterParent->getCause()->getCid() == EventCause::CID_INTVLEVSTART)
			{
				// 終了して抜ける
				EventMatter::end();
				return;
			}
		}
	}

	// セーブウインドウの作成
	GameGui* gui = Game::getGame()->getGui();
	_wndSave = new SaveLoadWindow();
	if((_wndSave == 0L) || !_wndSave->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture(), SaveLoadWindow::PAGE_SAVE, true, SaveData::SVF_EVENT_INTVL))
	{
		ASSERT(false);
		delete _wndSave;
		_wndSave = 0L;
	}

	// GUI タスクにセーブウインドウを登録
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	ASSERT(!tasksys->isExistModule(GameTaskModDef::MOD_INTVL_SAVE_WND));
	if (!tasksys->registerExecModule(_wndSave, GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_INTVL_SAVE_WND), GameTaskModDef::MOD_INTVL_SAVE_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }
	if (!tasksys->registerDrawModule(_wndSave, GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_INTVL_SAVE_WND), false, GameTaskModDef::MOD_INTVL_SAVE_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }
	
	EventMatter::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void IntvlSaveMatter::end()
{
	// GUI タスクからセーブウインドウを削除
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	if(!tasksys->removeModule(GameTaskModDef::MOD_INTVL_SAVE_WND))	{	ASSERT(false);	}

	// セーブウインドウの削除
	if(_wndSave != 0L)
	{
		_wndSave->destroy();
		delete _wndSave;
		_wndSave = 0L;
	}

	EventMatter::end();
}


/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void IntvlSaveMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel*	touchp = gec->getTouchPanel();

	enum Step
	{
		STEP_NULL,
		STEP_FIRST,
		STEP_CONFIRM_SAVE,
		STEP_DO_SAVE,
		STEP_END
	};

	s16 step = getStep();
	switch(step)
	{
	case STEP_FIRST:
		{
			ColorU8 colBlack(0, 0, 0, 255);

			// セーブするか確認
			GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
			wndSysMsg->setKind(GameSysMsgWindow::KIND_YESNO);
			wndSysMsg->setReadingAnimation(false);
			wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_SAVE), 0);

			// 背景塗りつぶし
			ScreenEffect* screffBack = Game::getGame()->getSceneScreenEffect();
			screffBack->setColor1(&colBlack);
			screffBack->setMaxFrame(-1);
			screffBack->start(ScreenEffect::MODE_FILL);

			// フェードイン
			ScreenEffect* screffFore = Game::getGame()->getWholeScreenEffect();
			screffFore->setColor1(&colBlack);
			screffFore->setMaxFrame(30);
			screffFore->start(ScreenEffect::MODE_FADE_IN);

			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_CONFIRM_SAVE:
		{
			// YES / NO 応答
			GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
			const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
			if(res->isClickYesButton())
			{
				// ウインドウを閉じる
				wndSysMsg->showWindow(false);
				// セーブウインドウを表示
				ASSERT(_wndSave != 0L);
				if(_wndSave != 0L)	{	_wndSave->showWindow(true);	}

				// 次のステップへ
				advanceStep();
			}
			else if(res->isClickNoButton())
			{
				ColorU8 colBlack(0, 0, 0, 255);

				// ウインドウを閉じる
				wndSysMsg->showWindow(false);
				// １フレーム隠しフェードイン
				ScreenEffect* screffFore = Game::getGame()->getWholeScreenEffect();
				screffFore->setColor1(&colBlack);
				screffFore->setMaxFrame(2);
				screffFore->start(ScreenEffect::MODE_FADE_IN);

				// 終了
				this->end();
			}
		}
		break;
	case STEP_DO_SAVE:
		if((_wndSave == 0L) || _wndSave->isDone())
		{
			ColorU8 colBlack(0, 0, 0, 255);

			// セーブウインドウは描画のみに設定
			ASSERT(_wndSave != 0L);
			if(_wndSave != 0L)	{	_wndSave->setActFlags(Window::AWNDF_DRAW_ONLY, true);	}

			// フェードアウト
			ScreenEffect* screffFore = Game::getGame()->getWholeScreenEffect();
			screffFore->setColor1(&colBlack);
			screffFore->setMaxFrame(30);
			screffFore->start(ScreenEffect::MODE_FADE_OUT);

			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_END:
		{
			// フェードアウトが終了したら終わり
			ScreenEffect* screffFore = Game::getGame()->getWholeScreenEffect();
			if(!screffFore->isWorking())
			{
				ColorU8 colBlack(0, 0, 0, 255);

				// 背景塗りつぶし終了
				ScreenEffect* screffBack = Game::getGame()->getSceneScreenEffect();
				screffBack->end(0);
				// １フレーム隠しフェードイン
				screffFore->setColor1(&colBlack);
				screffFore->setMaxFrame(2);
				screffFore->start(ScreenEffect::MODE_FADE_IN);

				// 終了
				this->end();
			}
		}
		break;
	}
}
////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
