/***********************************************************************//**
 *	GameTest.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameTest.h"

// Friends
#include "SubTest_FontPerformance.h"
#include "SubTest_Texture.h"
#include "SubTest_Video.h"
#include "SubTest_MessageWindow.h"
#include "../body/Conductor.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../debug/MenuFuncTable_Debug.h"
#include "../es/Es.h"
#include "../es/EsExtParam.h"
#include "../es/EsRunStat.h"
#include "../es/kwset/JsKeywordSet.h"
#include "../es/kwset/SsKeywordSet.h"
#include "../episode/StoryManager.h"
#include "../episode/party/MyParty.h"
#include "../episode/char/pc/MySpirit.h"
#include "../episode/char/pc/MyUnit.h" 
#include "../episode/data/FixedData.h" 
#include "../event/EventSys.h"
#include "../file/GameFileIdDef.h"
#include "../gui/GameGui.h"
#include "../gui/GameFontSet.h"
#include "../gui/menu/GameMenuCreateParam.h"
#include "../gui/menu/GameMenuPanelFactory.h"
#include "../map/Map.h"
#include "../magic/MagicSys.h"
#include "../magic/MagicCluster.h"
#include "../magic/MagicClusterCreateParam.h"
#include "../mode/GameMode.h"
#include "../particle/GameParticleDef.h"
#include "../particle/progptclmdl/implement/FanRangeModel.h"
#include "../save/SaveSys.h"
#include "../save/SysConf.h"
#include "../souma/script/SsSouma.h" 
#include "../souma/script/SoumaScriptUtils.h"
#include "../sound/GameSoundDef.h" 
#include "../source/Source.h" 
#include "../source/SourceManager.h"
#include "../status/EleneUtils.h"

// External
#include "../../etk/particle/Particle.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../etk/sound/SoundMngr.h" 
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/InsArray.h"
#include "../../tfw/collision/Gcol.h"
#include "../../tfw/common/PerformanceMeter.h"
#include "../../tfw/file/File.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/Calc.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/modemng/Mode.h"
#include "../../tfw/psns/PsnsBase.h"
#include "../../tfw/smenu/Menu.h"
#include "../../tfw/smenu/MenuTreeNode.h"
#include "../../tfw/string/StringUtils.h"
#include "../../tfw/ui/TouchPanel.h"
#if _WINDOWS
#include "../../windows/StopWatch.h"
#endif

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TEST_FONT_PERFORMANCE	(1)
#define TEST_TEXTURE			(2)
#define TEST_VIDEO				(3)
#define TEST_MESSAGE_WINDOW		(4)
#define TEST_ES					(0) //(0)
#define TEST_ES_SS				(0)

#define ACTIVE_TEST				(0)	//TEST_MESSAGE_WINDOW//0

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameTest メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	テストビューのモード変更
**//*---------------------------------------------------------------------*/
void GameTest::changeTestViewMode()
{
	Mode* mode = Game::getGame()->getGameMode()->getChildEndMode();
	VcString param("GameTest::changeTestViewMode");
	mode->callCustomEvent(0L, &param, 0L);	// ModelViewMode::callCustomEvent が応答するはず…
}

/*---------------------------------------------------------------------*//**
	固定データを強制的に再読込する
**//*---------------------------------------------------------------------*/
void GameTest::reloadFixedData()
{
#if defined(_DEBUG)
	FixedData::cleanup();
	FixedData::init();

	GameFontSet* fontset = (GameFontSet*)Game::getGame()->getGui()->getFontSet();
	fontset->debug_reload();

	SourceManager* srcmngRef = ((MyParty*)Game::getGame()->getMyParty())->getSourceManager(); ASSERT(srcmngRef != 0L);
	srcmngRef->debug_updateFixedSources();
#endif
}

/*---------------------------------------------------------------------*//**
	ファンクションキーによるテスト実行
**//*---------------------------------------------------------------------*/
void GameTest::onKeydownFunctionKey(s32 funckey)
{
	switch(funckey)
	{
	case 2:		// F2
		// Souma 関数テスト
		#if defined(_DEBUG)
		{
			// Sourcerer 1 Ver3.x から意図によるソウマクラスター作成機能を追加する。
			// 今までは、元素バランス値から作用値（EleneStat::FF_OP_*）などを算出していたが、
			// SoumaScriptUtils::computeEleneBalanceFromIntent により、意図を込めたクラスターを作成できる。
			// この場合、元素バランスは今までの算出された作用値等を必要としない（設定された意図を強制的に施行する）が、
			// なるべく算出された作用値に近づけるようにする。
			// 【2014/11/06 r-kishi】
			const u32 flagForms[] =
			{
				EleneStat::FF_FORM_SOIL,		// 0
				EleneStat::FF_FORM_WATER,		// 1
				EleneStat::FF_FORM_FIRE,		// 2
				EleneStat::FF_FORM_WIND,		// 3
			};
			const u32 flagOps[] =
			{
				EleneStat::FF_OP_PURIFY,		// 0
				EleneStat::FF_OP_CURE,			// 1
				EleneStat::FF_OP_CATHARSIS,		// 2
				EleneStat::FF_OP_EXCEED,		// 3
				EleneStat::FF_OP_WRAP,			// 4
				EleneStat::FF_OP_BARRIER,		// 5
				EleneStat::FF_OP_SUFFUSE,		// 6
				EleneStat::FF_OP_UNWRAP,		// 7
				EleneStat::FF_OP_UNBARRIER,		// 8
				EleneStat::FF_OP_UNSUFFUSE,		// 9
				EleneStat::FF_OP_ASCEN_S,		// 10
				EleneStat::FF_OP_ASCEN_A,		// 11
				EleneStat::FF_OP_DESCEN_S,		// 12
				EleneStat::FF_OP_DESCEN_A,		// 13
				EleneStat::FF_OP_ATTACK,		// 14
				EleneStat::FF_OP_EXC_ATTACK,	// 15
				EleneStat::FF_OP_UNW_ATTACK,	// 16
				EleneStat::FF_OP_UNB_ATTACK,	// 17
				EleneStat::FF_OP_UNS_ATTACK,	// 18			
			};
			for(int ifm = 0; ifm < (sizeof(flagForms) / sizeof(u32)); ifm++)
			{
				for(int iop = 0; iop < (sizeof(flagOps) / sizeof(u32)); iop++)
				{
					if(ifm == 1 && iop == 13)
					{
						TRACE("");
					}

					// ターゲットフラグ作成
					u32 eeffTrg = flagForms[ifm] | flagOps[iop];

					// 属性付加
					if((flagOps[iop] == EleneStat::FF_OP_ASCEN_S) || (flagOps[iop] == EleneStat::FF_OP_DESCEN_S))
					{
						//eeffTrg |= EleneStat::FF_ATRB_SOIL;
						eeffTrg |= EleneStat::FF_ATRB_WATER;
						//eeffTrg |= EleneStat::FF_ATRB_FIRE;
						//eeffTrg |= EleneStat::FF_ATRB_WIND;
						//eeffTrg |= EleneStat::FF_ATRB_AETHEL;
					}

					// 意図からの元素バランス逆算
					Elene eleneNql, eleneCql;
					SoumaScriptUtils::computeEleneBalanceFromIntent(&eleneNql, &eleneCql, eeffTrg, 10000);	// 最低 10000 で調整

					// 単元素アセンションのための属性付加
					SoumaScriptUtils::computeEleneBalanceAttributeAdded(&eleneNql, &eleneCql, eeffTrg);

					// 機能を算出
					EleneStat eestat(&eleneNql, &eleneCql);
					u32 qfCalced = eestat.calcQualityFlags();
					u32 eeffCalced = eestat.calcFuncFlags(qfCalced, true);
					u32 eeffCalcedOf = (eeffCalced & EleneStat::FF_MASK_OP) | (eeffCalced & EleneStat::FF_MASK_FORM) | (eeffCalced & EleneStat::FF_MASK_ATRB);

					if(eeffTrg != eeffCalcedOf)
					{
						// 作成不能ではあるが許可されるパターン
						if(((flagForms[ifm] == EleneStat::FF_FORM_SOIL) && (flagOps[iop] == EleneStat::FF_OP_UNSUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_CURE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_CATHARSIS))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_SUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_UNSUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_ASCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_DESCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WATER) && (flagOps[iop] == EleneStat::FF_OP_UNS_ATTACK))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_CURE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_CATHARSIS))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_ASCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_DESCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_CURE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_DESCEN_A)))
						{
							TRACE("*");
							//continue;
						}
						// 作成不能で許可されないパターン
						if(((flagForms[ifm] == EleneStat::FF_FORM_SOIL) && (flagOps[iop] == EleneStat::FF_OP_ASCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_SOIL) && (flagOps[iop] == EleneStat::FF_OP_DESCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_WRAP))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_BARRIER))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_SUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_UNWRAP))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_UNBARRIER))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_UNSUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_UNW_ATTACK))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_UNB_ATTACK))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_FIRE) && (flagOps[iop] == EleneStat::FF_OP_UNS_ATTACK))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_CATHARSIS))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_WRAP))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_BARRIER))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_SUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_UNWRAP))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_UNBARRIER))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_UNSUFFUSE))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_ASCEN_A))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_UNW_ATTACK))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_UNB_ATTACK))
							|| ((flagForms[ifm] == EleneStat::FF_FORM_WIND) && (flagOps[iop] == EleneStat::FF_OP_UNS_ATTACK)))
						{
							if(eleneNql.getAethel() == 0) { continue; }	// 正常
							TRACE("!");
						}
						if((flagOps[iop] == EleneStat::FF_OP_ASCEN_S)
							|| (flagOps[iop] == EleneStat::FF_OP_DESCEN_S))
						{
							TRACE("?");
						}
						//TRACE(">>> [%02d][%02d] trg=%08x, calced=%08x(%08x), qf=%08x ", ifm, iop, eeffTrg, eeffCalcedOf, eeffCalced, qfCalced);
						VcString str;
						TRACE(">>> [%02d][%02d] trg=", ifm, iop);
						EleneUtils::_debug_toOpStringFromFuncFlags(&str, flagOps[iop]);
						TRACE(str + "|");
						EleneUtils::_debug_toFormStringFromFuncFlags(&str, flagForms[ifm]);
						TRACE(str + ", cal=");
						EleneUtils::_debug_toOpStringFromFuncFlags(&str, eeffCalced & EleneStat::FF_MASK_OP);
						TRACE(str + "|");
						EleneUtils::_debug_toFormStringFromFuncFlags(&str, eeffCalced & EleneStat::FF_MASK_FORM);
						TRACE(str + " qf=");
						EleneUtils::_debug_toQualityStringFromQualityFlags(&str, qfCalced);
						TRACE(str + " ");
						eleneCql._debug_traceValue();
						TRACE("/");
						eleneNql._debug_traceValue();
						TRACE("\n");
					}
				}
			}
		}
		#endif
		break;
	case 3:		// F3
		changeTestViewMode();
		_dispCntF3 = 0;	// F3 ガイド表示カウンタリセット
		break;
	case 5:		// F5
		reloadFixedData();
		break;
	case 6:		// F6
		_dbgdisp = (DbgDisp)(_dbgdisp + 1);
		if(_dbgdisp >= NUM_DBGDISP) { _dbgdisp = DBGDISP_NULL; }
		switch(_dbgdisp)
		{
		case DBGDISP_NULL:
			setTestFlags(F_SHOW_PMETER, false);
			setTestFlags(F_SHOW_DBGMENUST, false);
			setTestFlags(F_SHOW_ACT_FRAME, false);
			setTestFlags(F_SHOW_MY_POS, false);
			break;
		case DBGDISP_ACTFRAME:
			setTestFlags(F_SHOW_PMETER, true);
			setTestFlags(F_SHOW_DBGMENUST, true);
			setTestFlags(F_SHOW_ACT_FRAME, true);
			setTestFlags(F_SHOW_MY_POS, false);
			break;
		case DBGDISP_MYUNITPOS:
			setTestFlags(F_SHOW_PMETER, true);
			setTestFlags(F_SHOW_DBGMENUST, true);
			setTestFlags(F_SHOW_ACT_FRAME, false);
			setTestFlags(F_SHOW_MY_POS, true);
			break;
		}
		break;
	case 8:		// F8
		// ポップ数字テスト
		{
			Conductor* conductor = Game::getGame()->getConductor();
			MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
			if((unitMy != 0L) && (conductor != 0L))
			{
				conductor->on(::rand(), ::rand() % 6, unitMy);
				//conductor->on(0, /*PopNumEffect::K_MISS*/5, unitMy);
			}
		}
		break;
	case 9:		// F9
		// パーティクルテスト
		{
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();
			MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
			if((unitMy != 0L) && (ptclsys != 0L))
			{
				// 新しいパーティクルを出現させる
				for(int iSafeCnt = 0; iSafeCnt < 100; iSafeCnt++)
				{
					if((0 <= _ptclkind) && (_ptclkind < 600))
					{
						s16 ptclid = ptclsys->appearParticle(_ptclkind++, unitMy->getCenterPos(), GameParticleDef::CF_NO_CALC_VEL);
						if(ptclid > 0)	{	break;	}	// 成功したら抜ける
					}
					else
					{
						_ptclkind = 0;
					}
				}
			}
		}
		break;
	case 10:	// F10
		// 範囲パーティクルテスト
		{
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();
			MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
			if((unitMy != 0L) && (ptclsys != 0L))
			{
				// 新しいパーティクルを出現させる
				s16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_PROG_FAN_RANGE, unitMy->getPosition(), GameParticleDef::CF_NO_CALC_VEL);
				if(ptclid > 0)
				{
					Particle* ptcl = ptclsys->getParticle(ptclid);
					ASSERT(ptcl != 0L);
					FanRangeModel* mdl = (FanRangeModel*)ptcl->getModel();
					ASSERT(mdl != 0L);
					mdl->setConfig(1000.0f, TFW_HPI, - TFW_HPI - (TFW_HPI * 0.5f), 4, 120);
				}
			}
		}
		break;
	case 11:	// F11
		// ショックウェーブテスト
		#if defined(_DEBUG)
		{
			MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
			MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
			MagicClusterCreateParam mccprm(MagicClusterType::SERVED, GameParticleDef::PTCLK_SKILL_SHOCK_WAVE);
			Elene eeOql(1000, 1000, 1000, 1000, 1000);
			Elene eeCql(1000, 1000, 2900, 1000, 1000);
			EleneStat eestatSkill(&eeOql, &eeCql);
			Vector3F pos(*unitMy->getCenterPos());
			mccprm._unitOwnrRef = unitMy;
			mccprm._unitMainTrgRef = 0L;
			mccprm._pos = &pos;
			mccprm._tstat.setupForSkill(unitMy, &eestatSkill);
			mccprm._lifeframe = 10 * eeCql.getWater() / Elene::ELENE_VALID_FIGURE;
			mccprm._isNoOnlyTrgColl = true;
			u16 mcidNew = mgcsys->appearCluster(&mccprm);
			if(mcidNew == 0)		{ return; }	// 成功ではないが終えるために次のステップへ
			MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
			if(mcNew == 0L)			{ return; }	// 成功ではないが終えるために次のステップへ
			TFW_SET_FLAG(*mcNew->ctrlFlags(), MagicCluster::CTRLF_DIR_TO_ROTZ, true);	// 方向を Z 軸回転に反映させる

			// クラスタ発射
			f32 speed = 2.0f * eeCql.getWind() * (0.1f / (f32)Elene::ELENE_VALID_FIGURE);
			if(speed < 0.033f)	{ speed = 0.033f; }
			Vector3F velMc(0.0f, 0.0f, 1.0f);
			Matrix4F mtxRot;
			mtxRot.rotateY(unitMy->getDirection()->y());
			mtxRot.mult(&velMc, &velMc);
			Vector3F accMc(&velMc);
			velMc *= speed;
			mcNew->setInitialVelocity(&velMc);
			accMc *= speed;
			mcNew->accelerate(&accMc);
			mcNew->reflectPosture();
		}
		#endif
		break;
	case 12:	// F12
		// GameCenter テスト
		#if defined(_DEBUG)
		{
			PsnsBase* psns = (PsnsBase*)Game::getGame()->getPsns();
			psns->debug_changeValidationUserAccountTest(!psns->isValidUserAccount());
		}
		#endif
		break;
	}
}

/*---------------------------------------------------------------------*//**
	PlayAct 全実行開始
**//*---------------------------------------------------------------------*/
void GameTest::startPlayActAll()
{
	_paa._index = 0;
}

/*---------------------------------------------------------------------*//**
	ゲーム起動初期に呼び出される、基本スクリプト テスト
**//*---------------------------------------------------------------------*/
void GameTest::testBasicScript()
{
	#if TEST_ES && 1
	{
		File file(CcString("test_es.txt"), File::MODE_READ);
		if(file.isOpened())
		{
			/**/	// 実行テスト
			es::Es esr;
			es::EsContext esctx;
			u32 mflags = es::ES_MF_ADD_DEBUG_INFO;
			#if TEST_ES_SS	// SS
				mflags |= es::ES_MF_PARSE_STRING_SIGN;
				es::SsKeywordSet sskwset;
				const es::EsKeywordSet* kwset = SourceDef::getKeywordSet(SourceDef::SRCKIND_SS);
			#else			// JS
				es::JsKeywordSet jskwset;
				const es::EsKeywordSet* kwset = SourceDef::getKeywordSet(SourceDef::SRCKIND_JS);
			#endif
			esctx.create();
			File file(CcString("test_es.txt"), File::MODE_READ);
			if(file.isOpened())
			{
				VcString strScript;
				file.readString(&strScript, file.getLength());
				if(esr.prepareParse(&esctx, kwset, mflags, 0L, es::EsExtParam()))
				{
					if(esr.parse(&esctx, &strScript))
					{
						if(esr.prepareRun(&esctx, es::EsExtParam()))
						{
							es::EsRunStat stat;
							do
							{
								esr.runStep(&esctx, &stat);
							}
							while(stat.isRunning());
						}
					}
				}
			}
			esctx.destroy();
			/**/

			/** /	// 変換テスト
			VcString strScript;
			CcString strNull("");
			file.readString(&strScript, file.getLength());
			Source source;
			source.setup(&strNull, &strScript, &strNull, SourceDef::SRCKIND_SS);
			TRACE("---\n");
			source.convToJs();
			TRACE(source.getCode()->getRaw());
			TRACE("\n---\n");
			source.convToSs();
			TRACE(source.getCode()->getRaw());
			TRACE("\n---\n");
			/**/
		}
	}
	#endif	// #if TEST_ES
}

/*---------------------------------------------------------------------*//**
	ソウマ スクリプト テスト
**//*---------------------------------------------------------------------*/
void GameTest::testSoumaScript(void* paramRun)
{
	#if TEST_ES && 0
	{
		es::Es esr;
		es::EsContext esctx;
		u32 mflags = es::ES_MF_ADD_DEBUG_INFO;
		#if 0
			mflags |= es::ES_MF_PARSE_STRING_SIGN;
			es::SsKeywordSet sskwset;
			const es::EsKeywordSet* kwset = SourceDef::getKeywordSet(SourceDef::SRCKIND_SS);
		#else
			es::JsKeywordSet jskwset;
			const es::EsKeywordSet* kwset = SourceDef::getKeywordSet(SourceDef::SRCKIND_JS);
		#endif
		esctx.create();
		File file(CcString("test_es.txt"), File::MODE_READ);
		if(file.isOpened())
		{
			VcString strScript;
			file.readString(&strScript, file.getLength());
			if(esr.prepareParse(&esctx, kwset, mflags, 0L, es::EsExtParam()))
			{
				if(esr.parse(&esctx, &strScript))
				{
					if(esr.prepareRun(&esctx, es::EsExtParam(paramRun)))
					{
						es::EsRunStat stat;
						do
						{
							esr.runStep(&esctx, &stat);
						}
						while(stat.isRunning());
					}
				}
			}
		}
		esctx.destroy();
	}
	#endif	// #if TEST_ES
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GameTest::GameTest()
	: _subtest(0L)
	, _pmeter(0L)
	, _menu(0L)
	, _rectTmenu(0L)
	, _cntShowGcol(0)
	, _dispCntF3(0)
	, _ptclkind(0)
	, _dbgdisp(DBGDISP_ACTFRAME)
{
	TRACE("sizeof(InsArray<s32>) = %d\n", sizeof(InsArray<s32>));
	TRACE("sizeof(List<s32>) = %d\n", sizeof(List<s32>));
//	ASSERT(sizeof(InsArray<s32>) == 12);
//	ASSERT(sizeof(List<s32>) == 12);

	setTestFlags(F_SHOW_PMETER | F_SHOW_DBGMENUST | F_SHOW_ACT_FRAME, true);

	#if _WINDOWS
	{
		{
			app::StopWatch sw;
			sw.start();
			for(f32 r = -TFW_PI; r <= TFW_PI; r += 0.001f)
			{
				Calc::sin(r);
			}
			TRACE("    #### time1:%f\n", sw.getElapsedSeconds());
		}
		{
			app::StopWatch sw;
			sw.start();
			for(f32 r = -TFW_PI; r <= TFW_PI; r += 0.001f)
			{
				::sinf(r);
			}
			TRACE("    #### time2:%f\n", sw.getElapsedSeconds());
		}
		TRACE("");
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GameTest::~GameTest()
{
	ASSERT(_subtest == 0L);
	ASSERT(_menu == 0L);
	ASSERT(_rectTmenu == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameTest::create(f32 spfRender, f32 spfLogic)
{
	// パフォーマンスメーターの作成
	_pmeter = new PerformanceMeter(35, spfRender);
	
	// デバッグメニュー作成
	Menu* menu = new Menu();
	MenuFuncTable* ftmv = new MenuFuncTable_Debug();
	MenuPanelFactory* pf = new GameMenuPanelFactory();
	GameMenuCreateParam* mcprm = new GameMenuCreateParam();
	mcprm->_case = GameMenuCreateParam::CASE_DEBUG;
	mcprm->_owner = this;
	if(!menu->create(MENU_MENU_DEBUG_XML, ftmv, pf, mcprm))
	{
		delete menu;
		delete ftmv;
		delete pf;
		delete mcprm;
		ASSERT(false);
		return false;
	}
	_menu = menu;

	// デバッグメニューの起動ボタン矩形の作成
	_rectTmenu = new RectF32(0.0f, (f32)Game::getGame()->getLogicalHeight() - 30.0f, 50.0f, 30.0f);

	// 全 Act 実行用にメニューツリーを解析する
	_paa._listPlayAct.setAutoObjectDelete(true);
	MenuTreeNode* mtnodePlayAct = _menu->findMenuTreeNode(19000);
	MenuTreeNode* mtnodeWk1 = _menu->getMenuTreeNodeChild(mtnodePlayAct);
	while(mtnodeWk1 != 0L)
	{
		MenuTreeNode* mtnodeWk2 = _menu->getMenuTreeNodeChild(mtnodeWk1);
		while(mtnodeWk2 != 0L)
		{
			MenuTreeNode* mtnodeWk3 = _menu->getMenuTreeNodeChild(mtnodeWk2);
			if((mtnodeWk3 != 0L) && mtnodeWk3->isFunction())
			{
				const VcString* strNodeValue = mtnodeWk3->getName();
				VcStringArray saCmd = StringUtils::split(strNodeValue, "_");
				PlayActAll::PlayAct* pa = new PlayActAll::PlayAct();
				pa->_storyid = StringUtils::toInteger(saCmd.get(1));
				pa->_evidAct = StringUtils::toInteger(saCmd.get(3));
				_paa._listPlayAct.addTail(pa);
			}
			mtnodeWk2 = _menu->getMenuTreeNodeSibling(mtnodeWk2);
		}
		mtnodeWk1 = _menu->getMenuTreeNodeSibling(mtnodeWk1);
	}

	// サブテスト作成
	#if ACTIVE_TEST == TEST_FONT_PERFORMANCE
		_subtest = new SubTest_FontPerformance();
	#elif ACTIVE_TEST == TEST_TEXTURE
		_subtest = new SubTest_Texture();
	#elif ACTIVE_TEST == TEST_VIDEO
		_subtest = new SubTest_Video();
	#elif ACTIVE_TEST == TEST_MESSAGE_WINDOW
		_subtest = new SubTest_MessageWindow();
	#endif
	if(_subtest != 0L)	{	_subtest->create();	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void GameTest::destroy()
{
	// メニュー破棄
	if(_menu != 0L)
	{
		MenuFuncTable* ftmv = _menu->getFuncTable();
		MenuPanelFactory* pf = _menu->getPanelFactory();
		GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menu->getCreateParam();
		_menu->destroy();
		delete _menu;
		delete ftmv;
		delete pf;
		delete mcprm;
		_menu = 0L;
	}

	// デバッグメニューの起動ボタン矩形の破棄
	delete _rectTmenu;
	_rectTmenu = 0L;

	// パフォーマンスメーター破棄
	if(_pmeter != 0L)
	{
		delete _pmeter;
		_pmeter = 0L;
	}

	// サブテスト破棄
	if(_subtest != 0L)
	{
		_subtest->destroy();
		delete _subtest;
		_subtest = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御前処理
**//*---------------------------------------------------------------------*/
void GameTest::prepareExec()
{
	if(_pmeter != 0L)	{	_pmeter->startFrame();	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void GameTest::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 

	if(_subtest != 0L)	{	_subtest->exec(res, ec);	}

	// PlayAct 全実行処理
	if(_paa._index != -1)
	{
		EventSys* evsys = Game::getGame()->getEventSys();
		if(!evsys->isBegunEvent())
		{
			if((_paa._index >= 0) && ((u32)_paa._index >= _paa._listPlayAct.getCount()))
			{
				_paa._index = -1;	// 終了
			}
			else
			{
				// 除外イベント
				switch(_paa._index)
				{
				case 0: case 1: case 2: case 3: case 4:	_paa._index = 5;	break;	// チュートリアル
				case 15:	_paa._index = 16;	break;	// 戦闘に入ってしまう
				}
				// イベント実行
				const PlayActAll::PlayAct* pa = _paa._listPlayAct.getObject(_paa._index++);
				Game::getGame()->enterGame(Game::EGKIND_DEBUG, 0, pa->_storyid);
				Game::getGame()->getStoryManager()->reserveEventForResume(pa->_evidAct, false);
			}
		}

		// 定期的にタップをしてチュートリアルを飛ばす
		_paa._cnt++;
		if((_paa._cnt % 60) == 0)
		{
			TouchPanel* ui = gec->getTouchPanel();
			TouchPanel::Touch ptcArr[2];
			TFW_ZERO_MEMORY(ptcArr, sizeof(TouchPanel::Touch) * 2);
			if((_paa._cnt % 120) == 0)
			{
				ui->beganTouches(ptcArr, 2);
			}
			else
			{
				ui->endedTouches(ptcArr, 2);
			}
		}
	}

	// 地面コリジョン表示デバッグ（トリプルクリックするとステイトが変わるスイッチ）
	#if 0
		if(!ui->isReacted() && !_menu->isShow() && (ui->getLastAction() == TouchPanel::ACT_CLICK))
		{
			if((0 <= _cntShowGcol) && (_cntShowGcol <= 30))		{	_cntShowGcol += 15;	}
			if((30 < _cntShowGcol) && (_cntShowGcol < 100))		{	_cntShowGcol = 100;	}
			if((100 <= _cntShowGcol) && (_cntShowGcol <= 130))	{	_cntShowGcol += 15;	}
			if((130 < _cntShowGcol) && (_cntShowGcol < 200))	{	_cntShowGcol = 0;	}
		}
		else
		{
			if((0 < _cntShowGcol) && (_cntShowGcol <= 30))		{	_cntShowGcol--;		}
			if((100 < _cntShowGcol) && (_cntShowGcol <= 130))	{	_cntShowGcol--;		}
		}
	#endif

	// デバッグメニュー制御
	#if 1
		ExecRes resMenu;
		_menu->exec(&resMenu, ec);
		if(!resMenu.isDone())
		{
			PointF32 ptTap;
			if(ui->isClickRelease(&ptTap))
			{
				PointF32 ptfTap((f32)ptTap.x(), (f32)ptTap.y());
				if(_rectTmenu->isPointIn(&ptfTap))
				{
					if(!_menu->isShow())
					{
						SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
						SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);

						// メニュー項目の変更
						{
							const s32 MTNODEID_LAND_EN			= 421;
							const s32 MTNODEID_LAND_JP			= 422;
							const s32 MTNODEID_SCRROT_M90		= 431;
							const s32 MTNODEID_SCRROT_P90		= 432;
							const s32 MTNODEID_SAYMSG_PAUSE		= 441;
							const s32 MTNODEID_SAYMSG_NOWAIT	= 442;
							const s32 MTNODEID_MUSIC_STEREO		= 451;
							const s32 MTNODEID_MUSIC_MONO		= 452;
							// 画面の方向
							const View* view = Game::getGame()->getTfw()->getView();
							MenuTreeNode* mtnodeSrM90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_M90); ASSERT(mtnodeSrM90 != 0L);
							MenuTreeNode* mtnodeSrP90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_P90); ASSERT(mtnodeSrP90 != 0L);
							#if defined(_WINDOWS)
								mtnodeSrM90->setChecked(view->getRotZ() == TFW_DISPF_RANG_0);
								mtnodeSrP90->setChecked(view->getRotZ() == TFW_DISPF_RANG_180);
							#elif defined(_IOS) || defined(_ANDROID)
								mtnodeSrM90->setChecked(view->getRotZ() == TFW_DISPF_RANG_90);
								mtnodeSrP90->setChecked(view->getRotZ() == TFW_DISPF_RANG_270);
							#endif
							// 言語
							LangId lang = Env_getLangId();
							MenuTreeNode* mtnodeEn = _menu->findMenuTreeNode(MTNODEID_LAND_EN); ASSERT(mtnodeEn != 0L);
							MenuTreeNode* mtnodeJp = _menu->findMenuTreeNode(MTNODEID_LAND_JP); ASSERT(mtnodeJp != 0L);
							mtnodeEn->setChecked(lang == LANGID_EN);
							mtnodeJp->setChecked(lang == LANGID_JP);
							// イベントメッセージ送り
							MenuTreeNode* mtnodeSayPause = _menu->findMenuTreeNode(MTNODEID_SAYMSG_PAUSE); ASSERT(mtnodeSayPause != 0L);
							MenuTreeNode* mtnodeSayNowait = _menu->findMenuTreeNode(MTNODEID_SAYMSG_NOWAIT); ASSERT(mtnodeSayNowait != 0L);
							#if 1
								mtnodeSayPause->setChecked(!TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_AUTO_FLOW_MOVIE_MSG));
								mtnodeSayNowait->setChecked(TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_AUTO_FLOW_MOVIE_MSG));
							#else
								mtnodeSayPause->setChecked(sysconf->isEnableWaitMovieMessage());
								mtnodeSayNowait->setChecked(!sysconf->isEnableWaitMovieMessage());
							#endif
							// 曲の再生がステレオかどうか
							bool isStereoMusic = Game::getGame()->getSoundManager()->isStereoMusic();
							MenuTreeNode* mtnodeStereo = _menu->findMenuTreeNode(MTNODEID_MUSIC_STEREO); ASSERT(mtnodeStereo != 0L);
							MenuTreeNode* mtnodeMono = _menu->findMenuTreeNode(MTNODEID_MUSIC_MONO); ASSERT(mtnodeMono != 0L);
							mtnodeStereo->setChecked(isStereoMusic);
							mtnodeMono->setChecked(!isStereoMusic);
						}

						// メニュー表示
						_menu->showMenu(0L);

						// SE
						Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_CSR_MOVE, false);
					}
				}
			}
		}
	#endif

	// F3 ガイド表示
	if(_dispCntF3 < 120)
	{
		STRACE("F3:next-mode\n");
	}
	_dispCntF3++;
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void GameTest::render(const RenderCtx* rc)
{
	if(_subtest != 0L)	{	_subtest->render(rc);	}

	// コリジョン描画
	if(_cntShowGcol >= 100)
	{
		Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
		if(gcol != 0L)
		{
			#if defined(_DEBUG)
				gcol->debug_draw(rc);
			#endif
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void GameTest::draw(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();

	_menu->draw(rc);
	if(!_menu->isShow() && TFW_IS_FLAG(getTestFlags(), F_SHOW_DBGMENUST))
	{
		ColorU8 col1, col2, col3, col4;
		RendererUtils::draw2dColorRect(rdr, _rectTmenu, ColorU8::setout(&col1, 255, 255, 255, 63), ColorU8::setout(&col2, 204, 255, 255, 63), ColorU8::setout(&col3, 200, 212, 212, 63), ColorU8::setout(&col4, 180, 202, 202, 63));
	}

	if(_subtest != 0L)	{	_subtest->draw(rc);	}

	if(TFW_IS_FLAG(getTestFlags(), F_SHOW_PMETER))
	{
		if(_pmeter != 0L)	{	_pmeter->render(rc);	}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS