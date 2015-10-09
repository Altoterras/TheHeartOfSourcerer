/***********************************************************************//**
 *  MenuFuncTable_Debug.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MenuFuncTable_Debug.h"

// Friends
#include "GameTest.h"
#include "../common/CalcUtils.h"
#include "../file/GameFileIdDef.h"
#include "../episode/StoryManager.h"
#include "../episode/char/pc/MyUnit.h"
#include "../episode/item/ItemIdDef.h"
#include "../episode/party/MyParty.h"
#include "../episode/save/Trophy.h"
#include "../event/EventSys.h"
#include "../body/Conductor.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/MoveMap.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/SourceEditWindow.h"
#include "../item/Item.h"
#include "../item/ItemManager.h"
#include "../mode/GameMode.h"
#include "../quest/QuestArticle.h"
#include "../quest/QuestDef.h"
#include "../quest/QuestManager.h"
#include "../save/SaveSys.h"
#include "../save/SysConf.h"
#include "../source/SourceManager.h"
#include "../status/CharStat.h" 
#include "../unit/UnitManager.h" 
#include "../unit/char/CharUnit.h" 

// External
#include "../../etk/sg/SorSgm.h"
#include "../../etk/sound/SoundMngr.h" 
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../tfw/g3d/sg/node/implement/Model.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/psns/PsnsBase.h"
#include "../../tfw/smenu/MenuTreeNode.h"
#include "../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

/*---------------------------------------------------------------------*//**
	表示通知
**//*---------------------------------------------------------------------*/
void MenuFuncTable_Debug::onShow(smenu::Menu* menu, smenu::MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void MenuFuncTable_Debug::onHide(smenu::Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
#if defined(_IOS)
SRCR_END_NS
extern void testMovie();
SRCR_BEGIN_NS
#endif
bool MenuFuncTable_Debug::onCall(smenu::Menu* menu, const ExecCtx* ec, smenu::MenuTreeNode* mtnodeCall, smenu::MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
#if defined(_DEBUG)
	//const GameExecCtx* gec = (const GameExecCtx*)ec;
	GameMode* gmode = Game::getGame()->getGameMode();

	const VcString* cmd = mtnodeCall->getName();
	const VcString* strNodeValue = mtnodeCall->getValue();
	VcStringArray saCmd = StringUtils::split(cmd, "_");

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	if(	cmd->equals("Resume_Game") )
	{
		Game::getGame()->enterGame(Game::EGKIND_LOAD, SaveSys::SVDID_RESUME, 0);	// ゲーム再開
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if((saCmd.getCount() == 3) && saCmd.get(0)->equals("Enter") && saCmd.get(1)->equals("Map"))
	{
		// 盾入手
		#if defined(_DEBUG) && 1
		{
			ItemManager* itmmng = Game::getGame()->getMyParty()->getItemManager();
			// 盾の追加
			itmmng->newItem(ItemIdDef::ITEMDID_SHLD_FIRST, 0L, Item::F_NOEDIT, true);	// 編集不可を末尾に
		}
		#endif

		// マップに入る
		int mapid = StringUtils::toInteger(strNodeValue);
		Game::getGame()->getMoveMap()->reserveChangeMap(mapid);
		Game::getGame()->enterGame(Game::EGKIND_DEBUG, 0, 0);

		// 固定ソウマ全表示
		#if defined(_DEBUG) && 1
			SourceManager* srcmng = Game::getGame()->getMyParty()->getSourceManager();
			srcmng->debug_addFixedSourceAll();
		#endif
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if((saCmd.getCount() == 3) && saCmd.get(0)->equals("Model") && saCmd.get(1)->equals("Viewer"))
	{
		u32 pval = 0;
		if(cmd->equals("Model_Viewer_01"))		{	pval = GameModeParam::F_DEBUG_MODEL_VIEW_FILE1;	}
		else if(cmd->equals("Model_Viewer_02"))	{	pval = GameModeParam::F_DEBUG_MODEL_VIEW_FILE2;	}
		else if(cmd->equals("Model_Viewer_03"))	{	pval = GameModeParam::F_DEBUG_MODEL_VIEW_FILE3;	}
		GameModeParam* gmparam = (GameModeParam*)gmode->getParameter();
		gmparam->setFlags(pval, true);
		gmode->changeEndMode(GameMode::MODE_GAME_MODELVW);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if((saCmd.getCount() == 4) && saCmd.get(0)->equals("StEv") && saCmd.get(2)->equals("ActEvId"))
	{
		s32 storyid = StringUtils::toInteger(saCmd.get(1));
		s32 evidAct = StringUtils::toInteger(saCmd.get(3));
		Game::getGame()->enterGame(Game::EGKIND_DEBUG, 0, storyid);
		Game::getGame()->getStoryManager()->reserveEventForResume(evidAct, false);
	}
	else if(cmd->equals("PlayActAll"))
	{
		GameTest* test = (GameTest*)Game::getGame()->getTest();
		if(test != 0L)	{	test->startPlayActAll();	}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if((saCmd.getCount() == 3) && saCmd.get(0)->equals("StEv") && saCmd.get(2)->equals("Jump"))
	{
		s32 storyid = StringUtils::toInteger(saCmd.get(1));
		Game::getGame()->enterGame(Game::EGKIND_DEBUG, 0, storyid);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Enter_Title"))
	{
		gmode->changeEndMode(GameMode::MODE_GAME_TITLE);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Enter_RealtimeMovieTest"))
	{
		EventSys* evsys = Game::getGame()->getEventSys();
		EvCause evcause;
		evsys->begin(901, 0, &evcause);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Enter_MpegMovieTest"))
	{
		gmode->changeEndMode(GameMode::MODE_GAME_MOVIE);
		#if defined(_IOS) && 0
			testMovie();
		#endif
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Enter_TmpEventTest"))
	{
		int tmpevid = StringUtils::toInteger(strNodeValue);
		EventSys* evsys = Game::getGame()->getEventSys();
		EvCause evcause;
		evsys->beginTmpEvent(tmpevid, 2001, 0, &evcause);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Test_Softkb"))
	{
		SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
		wndSrcedt->showWindow(true);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Reset_Trophies"))
	{
		for(int i = 0; i < Trophy::NUM_TI; i++)
		{
			Game::getGame()->getTrophy()->achievementTrophy(i, 0);
		}
		Game::getGame()->getPsns()->resetTrophies();
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Scr_Orien_M90"))
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setScreenOrientation(false);
	}
	else if(cmd->equals("Scr_Orien_P90"))
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setScreenOrientation(true);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Language_English"))
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setLanguage(LANGID_EN);
	}
	else if(cmd->equals("Language_Japanese"))
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setLanguage(LANGID_JP);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("SayMsg_Pause"))
	{
		#if 1
			Game::getGame()->setOptionFlags(Game::OPTF_AUTO_FLOW_MOVIE_MSG, false);
			Game::getGame()->getEventSys()->setEnableWaitMovieMessage(true);

		#else
			SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
			SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
			sysconf->setEnableWaitMovieMessage(true);
		#endif
	}
	else if(cmd->equals("SayMsg_NoWait"))
	{
		#if 1
			Game::getGame()->setOptionFlags(Game::OPTF_AUTO_FLOW_MOVIE_MSG, true);
			Game::getGame()->getEventSys()->setEnableWaitMovieMessage(false);
		#else
			SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
			SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
			sysconf->setEnableWaitMovieMessage(false);
		#endif
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("Music_Stereo"))
	{
		Game::getGame()->getSoundManager()->setStereoMusic(true);
	}
	else if(cmd->equals("Music_Mono"))
	{
		Game::getGame()->getSoundManager()->setStereoMusic(false);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if((saCmd.getCount() == 3) && saCmd.get(0)->equals("DebugMode") && saCmd.get(1)->equals("Lv"))
	{
		s32 lv = StringUtils::toInteger(saCmd.get(2));
		debug_onDebugLevel(lv);
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Full_Fixed_Source"))
	{
		#if defined(_DEBUG)
			MyParty* party = (MyParty*)Game::getGame()->getMyParty();
			SourceManager* srcmng = party->getSourceManager();
			srcmng->debug_addFixedSourceAll();
		#endif
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Force_Half_Enlight"))
	{
		MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			TFW_SET_FLAG(*unitMy->charStat()->debug_statFlags(), CharStat::SPSTAT_TRANS, true);
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Show_Gcol"))
	{
		GameTest* test = (GameTest*)Game::getGame()->getTest();
		if(test != 0L)
		{
			test->setShowGcol(!test->isShowGcol());
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Show_InvalidDrawOptObj"))
	{
		SorSgm* sgm = Game::getGame()->getSceneGraph(); ASSERT(sgm != 0L);
		SgNodeChain* sgc = sgm->getKindChain(SorSgm::EK_OPCTY);
		ListIterator<SgNode*> itSgn;
		sgc->getIterator(&itSgn);
		while(itSgn.has())
		{
			if(itSgn.object()->getType() == SGNTYPE_MODEL)
			{
				Model* mdl = (Model*)itSgn.object();
				// DrawOrder 0 のオブジェクトを表示する
				if((mdl->getRenderOrder() == 0) && !mdl->isShow())
				{
					mdl->setShow(true);
				}
			}
			itSgn.next();
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Force_Down_Enemy_Test"))
	{
		// 敵強制ダウン
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		for(int i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unit = unitmng->getUnitFromIndex(i);
			if(unit->getUnitType() == UNITTYPE_ENEMY_CHAR)
			{
				CharUnit* cunit = (CharUnit*)unit;
				CharStat* cstat = cunit->charStat();
				cstat->debug_damageEnergyDirect(cstat->getEnergy());
			}
		}

		// 便乗で、回復させる
		for(int i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unit = unitmng->getUnitFromIndex(i);
			if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC))
			{
				(((CharUnit*)unit)->charStat())->recoverFull();
			}
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Force_Down_Me_Test"))
	{
		// PC 強制ダウン
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		for(int i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unit = unitmng->getUnitFromIndex(i);
			if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC))
			{
				///if(Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY) == unit)	{	continue;	}
				CharUnit* cunit = (CharUnit*)unit;
				CharStat* cstat = cunit->charStat();
				cstat->debug_damageEnergyDirect(cstat->getEnergy());
			}
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Force_Down_Lf1_Test"))
	{
		// PC 強制 エネルギー 1
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		for(int i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unit = unitmng->getUnitFromIndex(i);
			if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC))
			{
				CharUnit* cunit = (CharUnit*)unit;
				CharStat* cstat = cunit->charStat();
				if(Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY) == cunit)
				{	
					cstat->debug_damageEnergyDirect(cstat->getEnergy() / 2);
				}
				else
				{
					cstat->debug_damageEnergyDirect(cstat->getEnergy() - 1);
				}

				#if 0
					// 便乗で、増悪させる
					EleneStat* eestat = cstat->eleneStat();
					Elene eleneWk(*eestat->getNql());
					eleneWk.scale(2.0f);
					eestat->impurification(&eleneWk);
				#endif
			}
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if(cmd->equals("DebugMode_Full_Check_Quest"))
	{
		QuestManager* qstmng = Game::getGame()->getQuestManager(); ASSERT(qstmng != 0L);
		for(int i = 0; i < 1; i++)
		{
			const QuestArticle* qsta = qstmng->debug_bootQuestByIndex(i);
			qstmng->contractQuest(qsta->getDef()->getQuestId());
			qstmng->finishQuest(qsta->getDef()->getQuestId());
		}
	}
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	else if((saCmd.getCount() == 2) && saCmd.get(0)->equals("Save"))
	{
		s32 ifile = StringUtils::toInteger(saCmd.get(1));
		Game::getGame()->getSaveSys()->save(ifile, 0);
	}
#endif	// #if defined(_DEBUG)

	return true;
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	デバッグレベル設定
**//*---------------------------------------------------------------------*/
void MenuFuncTable_Debug::debug_onDebugLevel(s32 lv)
{
	StoryManager* strymng = Game::getGame()->getStoryManager();
	Conductor* conductor = Game::getGame()->getConductor();
	MyParty* party = (MyParty*)Game::getGame()->getMyParty();

	strymng->debug_onDebugLevel(lv);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Lv 1
	if(lv >= 1)
	{
		conductor->onExperienceNow(30);
	}

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Lv 2
	if(lv >= 2)
	{
		conductor->onExperienceNow(90);
	}

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Lv 3
	if(lv >= 3)
	{
		conductor->onExperienceNow(770);
	}

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Lv 4
	if(lv >= 4)
	{
		conductor->onExperienceNow(900);

		// コツメを有効化
		Unit* unitPc3 = party->pcUnit(MyParty::PC_HLP);
		if(unitPc3 != 0L)
		{
			unitPc3->setEnable(true);
		}
	}

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Lv 5
	if(lv >= 5)
	{
		conductor->onExperienceNow(60000);
	}

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Lv 99
	if(lv >= 99)
	{
		conductor->onExperienceNow(60000);
		conductor->onExperienceNow(60000);

		// アサを半覚醒に
		MyUnit* unitMy = (MyUnit*)party->getPcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			TFW_SET_FLAG(*unitMy->charStat()->debug_statFlags(), CharStat::SPSTAT_TRANS, true);
		}
	}
}
#endif	// #if defined(_DEBUG)

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
