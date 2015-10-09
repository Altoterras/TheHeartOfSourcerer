/***********************************************************************//**
 *	Confront.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Confront.h"

// Friends
#include "../char/CharLogicData.h"
#include "../common/CalcUtils.h"
#include "../episode/MapIdDef.h"
#include "../episode/char/pc/MyUnit.h"
#include "../episode/save/Score.h"
#include "../event/EventSys.h"
#include "../body/GameFixedStrTbl.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameNotifier.h"
#include "../body/MoveMap.h"
#include "../body/Party.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/GameActMsgWindow.h"
#include "../gui/FocusCursor.h"
#include "../item/ItemManager.h"
#include "../map/Map.h"
#include "../map/PlacementObj.h"
#include "../mode/GameMode.h"
#include "../souma/Souma.h"
#include "../sound/GameSoundDef.h"
#include "../spirit/char/pc/PcSpirit.h"
#include "../status/CharStat.h"
#include "../unit/char/CharUnit.h"
#include "../unit/char/npc/NpcCharUnit.h"

// External
#include "../../etk/sound/SoundMngr.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Confront メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	対決開始（詳細）
**//*---------------------------------------------------------------------*/
void Confront::start(Unit* unitTrgRef, u16 evidCtrl, const Unit* unitCaller, u16 bgmid, u16 evidAfterWin, u16 evidAfterLose, bool isPreventEscape)
{
	// オプション設定
	if(isPreventEscape)	{	_isPreventEscape = true;	}	// 途中から false に戻すことはできない
	if(evidAfterWin != 0)
	{
		ASSERT(_evidAfterWin == 0);
		if(_evidAfterWin == 0)	{	_evidAfterWin = evidAfterWin;	}	// 書き換えることはできない
	}
	if(evidAfterLose != 0)
	{
		ASSERT(_evidAfterLose == 0);
		if(_evidAfterLose == 0)	{	_evidAfterLose = evidAfterLose;	}	// 書き換えることはできない
	}

	ASSERT(unitTrgRef != 0L);
	if(!_isConfront)	// 対決中ではない
	{
		// 対決開始
		startConfront(unitTrgRef, evidCtrl, unitCaller, bgmid);
	}
	else				// 既に対決時
	{
		// 対決相手の追加
		addConfront(unitTrgRef, evidCtrl, unitCaller, bgmid);
	}
}

/*---------------------------------------------------------------------*//**
	配置オブジェクトの設定を用いて対決開始
**//*---------------------------------------------------------------------*/
void Confront::start(Unit* unitConfront, u16 evidCtrl, const Unit* unitCaller, const PlacementObj* pobjSetting)
{
	if(pobjSetting != 0L)
	{
		// 配置オブジェクトの設定で対決へ
		start(unitConfront, evidCtrl, unitCaller, pobjSetting->getBgmId(), pobjSetting->getAfterWinEvent(), pobjSetting->getAfterLoseEvent(), !pobjSetting->isAllowConfrontEscape());
	}
	else	// NULL 指定の場合
	{
		// デフォルト設定で対決へ
		start(unitConfront, evidCtrl, unitCaller, BGMID_DEFAULT, 0, 0, false);
	}
}

/*---------------------------------------------------------------------*//**
	対決開始（簡易）
**//*---------------------------------------------------------------------*/
void Confront::start(Unit* unitConfront, u16 evidCtrl, const Unit* unitCaller)
{
	// 配置オブジェクトを探す
	const PlacementObj* pobjSetting = 0L;
	if(TFW_IS_FLAG(unitConfront->getUnitFlags(), Unit::UNITFLAG_NPC))
	{
		const NpcCharUnit* npc = (NpcCharUnit*)unitConfront;
		pobjSetting = npc->getPlacementObj();
	}

	start(unitConfront, evidCtrl, unitCaller, pobjSetting);
}

/*---------------------------------------------------------------------*//**
	対決終了
**//*---------------------------------------------------------------------*/
void Confront::end(bool* isBegunEvent)
{
	if(_isConfront)		// 対決中
	{
		// 対決終了
		bool isBegunAfterEvent = endConfront();
		if(isBegunEvent != 0L)	{	*isBegunEvent = isBegunAfterEvent;	}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Confront::Confront()
	: _uarrCfTrg(0L)
	, _unitPromoter(0L)
	, _isConfront(false)
	, _isAllTrgDown(false)
	, _isMyUnitDown(false)
	, _isPreventEscape(false)
	, _bgmidCur(0)
	, _evidCtrl(0)
	, _evidAfterWin(0)
	, _evidAfterLose(0)
	, _frameCnt(0.0f)
///	, _gmkAfterConfront(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Confront::~Confront()
{
	ASSERT(_uarrCfTrg == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Confront::create()
{
	// 対戦相手ユニット配列を作成
	_uarrCfTrg = new UnitArray();

	// 対象が全員ダウンしている、とする
	_isAllTrgDown = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Confront::destroy()
{
	// 対戦相手ユニット配列を作成
	delete _uarrCfTrg;
	_uarrCfTrg = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void Confront::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_uarrCfTrg == 0L)	{	return;	}	// 生成されていない

	// 操作ロック時は進めない
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(gec->isLockOperating())	// 操作ロック時
	{
		return;
	}

	// ダウンした敵の消去およびアイテムドロップ処理
	for(s32 iunit = 0; iunit < _uarrCfTrg->getCount(); iunit++)
	{
		Unit* unit = _uarrCfTrg->unit(iunit);
		if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
		{
			const CharStat* cstat = ((CharUnit*)unit)->getCharStat();
			if(cstat->getStat() == CharStat::STAT_DOWN)
			{
				if(unit->isEnable())
				{
					const CharLogicData* cldat = (unit->getSpirit() != 0L) ? unit->getSpirit()->getCharLogicData() : 0L;
					u16 ecflags = (cldat != 0L) ? cldat->getParam(CharLogicData::PARAM_EC_FLAGS) : 0;
					if(!TFW_IS_FLAG(ecflags, CharLogicData::ECF_NO_DOWN_VANISH))	// 無効化しない場合以外
					{
						unit->setEnable(false);	// 無効化
					}

					// アイテムドロップ処理
					dropItem((CharUnit*)unit);
				}
			}
		}
	}

	// 対決中、対決相手と自キャラのダウン判定
	if(_isConfront)
	{
		// 対決相手のダウン判定
		if(!_isAllTrgDown)
		{
			// 全ての対決対象がダウンしたかを調べる
			_isAllTrgDown = checkAllTargetDown();
		}
		
		// 自キャラのダウン判定
		if(!_isMyUnitDown)
		{
			Party* party = Game::getGame()->getMyParty();
			const CharUnit* cunit = (const CharUnit*)party->getLeaderPcUnit();
			if(cunit->getCharStat()->getStat() == CharStat::STAT_DOWN)
			{
				if(gec->isPreventDown())	// ダウン防止中
				{
					party->leaderPcUnit()->charStat()->revive(true);	// 1 だけ回復
				}
				else
				{
					_isMyUnitDown = true;
				}
			}
		}

		// フレームカウンタ更新
		_frameCnt += ec->getDeltaFrame();
	}
}

/*---------------------------------------------------------------------*//**
	対決開始
**//*---------------------------------------------------------------------*/
void Confront::startConfront(Unit* unitTrgRef, u16 evidCtrl, const Unit* unitCaller, u16 bgmid)
{
	// 値の初期化
	_isConfront = true;		// 対決開始
	_evidCtrl = evidCtrl;
	_unitPromoter = unitCaller;
	_frameCnt = 0.0f;

	// PC を配列に追加する
	for(int i = 0; i < Game::getGame()->getMyParty()->getPcNum(); i++)
	{
		Unit* unit = Game::getGame()->getMyParty()->pcUnit(i);
		if((unit != 0L) && unit->isEnable())
		{
			_uarrCfTrg->addUnit(unit);
		}
	}

	// 対戦相手を配列に追加する
	if(unitTrgRef == 0L)
	{
		ASSERT(false);	// ありうるか？？
		// すでに敵はいなくなったようだ
		TRACE("{ConfrontMode::notifyBegin} Enemy is already down.\n");
		// 終了
		endConfront();
		return;
	}
	_uarrCfTrg->addUnit(unitTrgRef);
	UnitArray uaFriends(unitTrgRef->getFriends());
	_uarrCfTrg->add(&uaFriends);

	// ダウンフラグクリア
	_isAllTrgDown = false;
	_isMyUnitDown = false;

	// メッセージウインドウを表示する
	GameActMsgWindow* wndActMsgRef = (GameActMsgWindow*)Game::getGame()->getGui()->getActionMsgWindow(); ASSERT(wndActMsgRef != 0L);
	wndActMsgRef->setResident(true);	// 常に表示に

	// 各ユニットに通知
	Game::getGame()->getGameNotifier()->notifyChangedConfront(true, this, unitTrgRef, unitCaller, false);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// その他モジュールの制御

	#if 1
		// 誰かが何か行動中のとき以外、フォーカスをセットする
		bool isBehaviorActingSomeone = false;
		for(int ipc = 0; ipc < Game::getGame()->getMyParty()->getPcNum(); ipc++)
		{
			const PcSpirit* sprt = Game::getGame()->getMyParty()->getPcSpirit(ipc);
			if(sprt->isBehaviorActing())
			{
				isBehaviorActingSomeone = true;
				break;
			}
		}
		if(!isBehaviorActingSomeone)
		{
			FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
			if(fccsr->getFocus(0) != unitTrgRef)
			{
				fccsr->setFocus(0, unitTrgRef);
			}
		}
	#else
		// 詠唱中以外、フォーカスをセットする
		for(int ipc = 0; ipc < Game::getGame()->getMyParty()->getPcNum(); ipc++)
		{
			const PcSpirit* sprt = Game::getGame()->getMyParty()->getPcSpirit(ipc);
			if(!sprt->isBehaviorActing())
			{
				FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
				if(fccsr->getFocus(ipc) != unitTrgRef)
				{
					fccsr->setFocus(ipc, unitTrgRef);
				}
			}
		}
	#endif

	// ゲームモードの変更
	GameMode* gmode = Game::getGame()->getGameMode();
	GameMode::ModeKind gmkindNow = gmode->getChildEndModeKind();
	if(	(gmkindNow == GameMode::MODE_GAME_INGAME_MENU) ||
		(gmkindNow == GameMode::MODE_GAME_INGAME_WORLDMAP)	)
	{
		// メニュー中、ワールドマップ中は強制的に歩行モードへ
		gmode->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING);
	}
#if 0
	// 終了時に戻すゲームモードを取得
	if(_gmkAfterConfront == 0)	// setConfrontAfterGameMode などで外部から設定されていない場合
	{
		_gmkAfterConfront = gmode->getChildEndModeKindConsiderReserved();
	}
#endif
	// ゲームモードを対決へ
	gmode->changeEndMode(GameMode::MODE_GAME_INGAME_CONFRONT);

	// マップのゲートを閉じる
	if(_isPreventEscape)
	{
		Game::getGame()->getMap()->setCloseGate(true);
	}

	// BGM 変更
	if(bgmid == BGMID_DEFAULT)	{	bgmid = GameSoundDef::BGM_108_CONFRONT;	}
	if(bgmid != BGMID_NO_CHANGE)
	{
		SoundMngr* sndmng = Game::getGame()->getSoundManager();
		sndmng->playBgm(bgmid, true);
		_bgmidCur = bgmid;
	}
}

/*---------------------------------------------------------------------*//**
	追加の対決相手
**//*---------------------------------------------------------------------*/
void Confront::addConfront(Unit* unitTrgRef, u16 evidCtrl, const Unit* unitCaller, u16 bgmid)
{
	// 対戦相手配列に追加
	if(unitTrgRef == 0L)
	{
		ASSERT(false);	// ありうるか？？
		// すでに敵はいなくなったようだ
		TRACE("{ConfrontMode::notifyBegin} Enemy is already down.\n");
		return;
	}
	_uarrCfTrg->addUnit(unitTrgRef);
	UnitArray uaFriends(unitTrgRef->getFriends());
	_uarrCfTrg->add(&uaFriends);

	// 全対決相手の再ダウン判定
	if(_isAllTrgDown)
	{
		_isAllTrgDown = checkAllTargetDown();
	}

	// 各ユニットに通知
	Game::getGame()->getGameNotifier()->notifyChangedConfront(true, this, unitTrgRef, unitCaller, true);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// その他モジュールの制御

	// マップのゲートを閉じる
	if(_isPreventEscape && !Game::getGame()->getMap()->isCloseGate())
	{
		Game::getGame()->getMap()->setCloseGate(true);
	}

	// BGM 変更
	if((bgmid != BGMID_DEFAULT) && (_bgmidCur != bgmid))
	{
		SoundMngr* sndmng = Game::getGame()->getSoundManager();
		sndmng->playBgm(bgmid, true);
		_bgmidCur = bgmid;
	}
}

/*---------------------------------------------------------------------*//**
	対決終了
**//*---------------------------------------------------------------------*/
bool Confront::endConfront()
{
	// 各ユニットに通知
	Game::getGame()->getGameNotifier()->notifyChangedConfront(false, this, 0L, 0L, false);

	// メッセージウインドウを非表示にする
	GameActMsgWindow* wndActMsgRef = (GameActMsgWindow*)Game::getGame()->getGui()->getActionMsgWindow(); ASSERT(wndActMsgRef != 0L);
	if(wndActMsgRef != 0L)
	{
		wndActMsgRef->setResident(false);	// 常に表示を解除
	}

	// 対戦相手配列をクリア
	_uarrCfTrg->clear();

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// その他モジュールの制御

	bool isBegunAfterEvent = false;
	if(_isMyUnitDown)	// 負け
	{
		if(_evidAfterLose != 0)
		{
			// 敗北時のイベント実行
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_NULL);
			if(evsys->begin(_evidAfterLose, 0, &evcause) != 0L)
			{
				isBegunAfterEvent = true;
			}
		}
		else
		{
			/*
			// 村へ戻る
			Game::getGame()->changeMap(MapIdDef::MAPID_EP01_BASE_VILLAGE_GATE, 0);
			*/
		}
	}
	else				// 勝ち
	{
		if(_evidAfterWin != 0)
		{
			// 勝利時のイベント実行
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_NULL);
			if(evsys->begin(_evidAfterWin, 0, &evcause) != 0L)
			{
				isBegunAfterEvent = true;
			}
		}
		else
		{
			// BGM 変更
			Map* map = Game::getGame()->getMap();
			map->playMapBgm();
		}

		// ［スコア］対決勝ち
		ScoreBase* score = Game::getGame()->getScore(); ASSERT(score != 0L);
		score->upScore(Score::SC_CONFRONT_WIN);
	}

	// マップのゲートを戻す
	if(_isPreventEscape)
	{
		Game::getGame()->getMap()->setCloseGate(false);
	}

	// ゲームモード変更処理
#if 1
//	if(!isBegunAfterEvent)
	{
		GameMode* gmode = Game::getGame()->getGameMode();
		gmode->changePrevEndMode(GameMode::MODE_GAME_INGAME_CONFRONT);
	}
#else
	if(isBegunAfterEvent)
	{
		// イベントが開始された場合は、イベントシステムに終了後のゲームモード戻しを引き継がせる
		EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
		evsys->setEventAfterGameMode(_gmkAfterConfront);
		_gmkAfterConfront = 0;	// 対決モジュールの設定は NULL 化
	}
	else
	{
		// ゲームモードを戻す
		GameMode* gmode = Game::getGame()->getGameMode();
		if(_gmkAfterConfront == 0)
		{
			gmode->changePrevEndMode();
		}
		else
		{
			gmode->changeEndMode((GameMode::ModeKind)_gmkAfterConfront);
			_gmkAfterConfront = 0;
		}
	}
#endif

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// 値のクリア

	_isPreventEscape = false;
	_isMyUnitDown = false;
	_bgmidCur = 0;
	_evidAfterWin = 0;
	_evidAfterLose = 0;
	_evidCtrl = 0;
	_unitPromoter = 0L;
	_isConfront = false;	// 対決終了
///	_gmkAfterConfront = 0;

	return isBegunAfterEvent;
}

/*---------------------------------------------------------------------*//**
	全対象ダウン判定
**//*---------------------------------------------------------------------*/
bool Confront::checkAllTargetDown() const
{
	for(s32 iunit = 0; iunit < _uarrCfTrg->getCount(); iunit++)
	{
		const Unit* unit = _uarrCfTrg->unit(iunit);
		if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
		{
			const CharStat* cstat = ((CharUnit*)unit)->getCharStat();
			if(cstat->getStat() != CharStat::STAT_DOWN)
			{
				return false;
			}
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	アイテムドロップ処理
**//*---------------------------------------------------------------------*/
void Confront::dropItem(CharUnit* unit)
{
	const CharLogicData* cldat = (unit->getSpirit() != 0L) ? unit->getSpirit()->getCharLogicData() : 0L;
	if(cldat == 0L)			{	return;	}

	u16 itemdidDrop = cldat->getParam(CharLogicData::PARAM_DROP_ITEM_ID);			// ドロップアイテム ID
	if(itemdidDrop == 0)	{	return;	}

	u16 probability = cldat->getParam(CharLogicData::PARAM_DROP_ITEM_PROBABILITY);	// 確率
	if(probability == 0)	{	return;	}

	// 乱数で判定
	s32 rand100 = (s32)(100.0f * CalcUtils::randF());
	if(probability < rand100)
	{
		// ドロップなし
		return;
	}

	// ドロップアイテム配置
	ItemManager* itmmng = Game::getGame()->getMyParty()->getItemManager(); ASSERT(itmmng != 0L);
	Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
	itmmng->dropItem(itemdidDrop, map, unit->getPosition(), 0L);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
