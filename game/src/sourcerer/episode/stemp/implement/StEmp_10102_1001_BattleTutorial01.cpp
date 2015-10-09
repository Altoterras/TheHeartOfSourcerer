/***********************************************************************//**
 *	StEmp_10102_1001_BattleTutorial01.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10102_1001_BattleTutorial01.h"

// Friends
#include "../StEmpType.h"
#include "../../char/CharIdDef.h"
#include "../../char/enemy/Char1404_BattleTutorial_EnemySpirit.h"
#include "../../char/pc/MyUnit.h"
#include "../../char/pc/PartnerSpirit.h"
#include "../../char/pc/PartnerUnit.h"
#include "../../party/MyParty.h"
#include "../../../camera/GameCustomCamClass.h"
#include "../../../camera/WalkingCam.h"
#include "../../../char/CharDef.h"
#include "../../../common/SimpleSprite.h"
#include "../../../confront/Confront.h"
#include "../../../event/EventSys.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../gui/CharFronts.h"
#include "../../../gui/FocusCursor.h"
#include "../../../gui/FocusMenu.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/IngameFront.h"
#include "../../../gui/MoveCursor.h"
#include "../../../item/Item.h"
#include "../../../item/ItemDef.h"
#include "../../../magic/MagicCluster.h"
#include "../../../mode/GameMode.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/CharUnit.h"
#include "../../../unit/item/EquipmentItemUnit.h"
#include "../../../spirit/Spirit.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../status/EleneVeil.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../tfw/ui/TouchPanel.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define CTRLEVID_TARGET					(51003)		// 戦闘チュートリアルの敵

#define EVID_MSG_RESULT_SOUMA_1			(10002)
#define EVID_MSG_ENEMY_ATTACK_1			(10003)
#define EVID_MSG_PC2_ATTACK_1			(10004)
#define EVID_MSG_PLEASE_SOUMA_2			(10006)
#define EVID_MSG_MSG_FAIL_SOUMA_2_EF	(10008)
#define EVID_MSG_MSG_SUCCESS_SOUMA_2_EF	(10009)
#define EVID_MSG_RESULT_SKILL_1			(10011)

#define FILEID_SPRITE_SOUMA_1			FILEID_CONV_LANG( OTHEREVENT_STEV10102_TUTORIAL01_EN_PNG )
#define FILEID_SPRITE_PC2_ATTACK_1		FILEID_CONV_LANG( OTHEREVENT_STEV10102_TUTORIAL11_EN_PNG )
#define FILEID_SPRITE_PC2WEP_FOCUS		FILEID_CONV_LANG( OTHEREVENT_STEV10102_TUTORIAL21_EN_PNG )
#define FILEID_SPRITE_SOUMA_2			FILEID_CONV_LANG( OTHEREVENT_STEV10102_TUTORIAL22_EN_PNG )
#define FILEID_SPRITE_PHYS_DAMAGE_1		FILEID_CONV_LANG( OTHEREVENT_STEV10102_TUTORIAL31_EN_PNG )

#define FRAMECNT_DELAY_CLOSE_SPRITE		(15.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10102_1001_BattleTutorial01 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::begin(u32 param)
{
	// スプライト作成
	_sprite = new SimpleSprite();

	// 最初のテクスチャを読み込む
	readTexture(FILEID_SPRITE_SOUMA_1);

	// チュートリアル敵のスピリットを得る
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unit = unitmng->getUnitFromIndex(i);
		if(unit->getCtrlEvid() == CTRLEVID_TARGET)
		{
			_sprtEnemy = (Char1404_BattleTutorial_EnemySpirit*)unit->getSpirit();
			break;
		}
	}
	ASSERT(_sprtEnemy != 0L);	// 検索で見つかるはず

	// ユニットに有効・無効化
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	CharUnit* unitPc1 = party->pcUnit(MyParty::PC_MY); ASSERT(unitPc1 != 0L);
	PartnerUnit* unitPc2 = (PartnerUnit*)party->pcUnit(MyParty::PC_PTN); ASSERT(unitPc2 != 0L);
	CharUnit* unitPc3 = party->pcUnit(MyParty::PC_HLP); ASSERT(unitPc3 != 0L);
	unitPc1->setEnable(true);
	unitPc2->setEnable(true);
	unitPc3->setEnable(false);

	// PC2 のスピリットを得る
	_sprtPtnr = (PartnerSpirit*)unitPc2->getSpirit();
	ASSERT(_sprtPtnr != 0L);

	// PC2 の武器ユニットを得る
	unitmng = Game::getGame()->getUnitManager();
	Unit* unitTrg = 0L;
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(i);
		if(unitWk->getUnitType() == UNITTYPE_EQUIPMENT_ITEM)
		{
			EquipmentItemUnit* itmunit = (EquipmentItemUnit*)unitWk;
			Unit* unitItemOwner = itmunit->getOwnerUnit();
			if((unitItemOwner != 0L) && (unitItemOwner->getSpirit() == _sprtPtnr))
			{
				const Item* item = itmunit->getItem();
				if(item != 0L)
				{
					const ItemDef* itmdef = item->getItemDef();
					if(itmdef != 0L)
					{
						if(itmdef->getCategory() == ItemDef::CAT_WEAPON)
						{
							_unitPtnrWeapon = unitWk;

							// こん棒のベールを強制的に解除する
							EleneStat* eestatWep = _unitPtnrWeapon->eleneStat(); ASSERT(eestatWep != 0L);
							eestatWep->clearVeil();
							eestatWep->updateResultValue(true);
							break;
						}
					}
				}
			}
		}
	}
	ASSERT(_unitPtnrWeapon != 0L);

	// PC1、PC2 のエネルギーを満タンに
	unitPc1->charStat()->recoverFull();
	unitPc2->charStat()->recoverFull();
	// PC2 へのベールを強制的に解除する
	unitPc2->eleneStat()->clearVeil();
	unitPc2->eleneStat()->updateResultValue(true);
	// PC2 の装備表示状態を更新する（２周目などでコルポと合流せずに直接外に出た場合）
	unitPc2->updateEquipModelObjVisibility();
	
	// 他の通常配置敵を消去
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(i);
		if(unitWk->getUnitType() == UNITTYPE_ENEMY_CHAR)
		{
			if(unitWk->getCtrlEvid() != CTRLEVID_TARGET)	// チュートリアル敵以外
			{
				if(unitWk->isEnable())
				{
					unitWk->setEnable(false);
				}
			}
		}
	}
	
	// フレームカウンタリセット
	_frameElapsedWk = 0.0f; 
	_frameElapsedStep = 0.0f;

	// ダウン防止
	getOwner()->setPreventDown(true);
	// ワールドマップ禁止
	getOwner()->setNoWorldMap(true);
	// 移動カーソルを無効に
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
	if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}

	return StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::end()
{	
	// 移動カーソルを有効に
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
	if(!mvcsr->isEnable(MoveCursor::EDCH_EVENT))	{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, true);		}
	// インゲームフロントのメニューアイコンを有効化
	IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
	igfr->setEneblePanel(IngameFront::BTN_MENU, true);
	// ワールドマップ禁止解除
	getOwner()->setNoWorldMap(false);
	// ダウン防止解除
	getOwner()->setPreventDown(false);
	// GUI のサブモジュールの自動有効・無効化機能を有効に戻す
	Game::getGame()->getGui()->setExecFlags(GameGui::EF_EXT_ENABLE_CTRL, false);
	// PC2 メニュー項目の制限解除
	setEnablePc2MenuExceptWeapon(true);

	// スプライト削除
	if(_sprite != 0L)
	{
		_sprite->destroy();
		delete _sprite;
		_sprite = 0L;
	}

	StEmp::end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10102_1001_BattleTutorial01::StEmp_10102_1001_BattleTutorial01()
	: StEmp(STEMPTYPE_10102_1001)
	, _sprite(0L)
	, _evmatter(0L)
	, _sprtEnemy(0L)
	, _sprtPtnr(0L)
	, _unitPtnrWeapon(0L)
	, _frameElapsedWk(0.0f)
	, _frameElapsedStep(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10102_1001_BattleTutorial01::~StEmp_10102_1001_BattleTutorial01()
{
	ASSERT(_sprite == 0L);
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel*	touchp = gec->getTouchPanel();
	s16 step = getStep();

	if(step >= STEP_ENEMY_ATTACK_1)
	{
		// なんらかの方法で戦闘を終えたら終了
		if(!Game::getGame()->isConfront())
		{
			this->end();	// 終了
		}
	}
	else
	{
		// 戦闘前になんらかの方法で敵を倒したら強制回復
		if(_sprtEnemy->getCharStat()->getStat() == CharStat::STAT_DOWN)
		{
			_sprtEnemy->charStat()->recoverFull();
		}
	}

	// ピンチ時強制回復
	{
		CharStat* cstatMy = ((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat();
		CharStat* cstatPtnr = ((PartnerUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_PTN))->charStat();
		if((cstatMy->getEnergy() * 4) <= cstatMy->getMaxEnergy())		// HP が 1/4 以下
		{
			cstatMy->recoverEnergy(cstatMy->getEnergyDamage());
		}
		if((cstatPtnr->getEnergy() * 4) < cstatPtnr->getMaxEnergy())	// HP が 1/4 以下
		{
			cstatPtnr->recoverEnergy(cstatPtnr->getEnergyDamage());
		}
	}

	switch(step)
	{
	case STEP_SPRITE_SOUMA_1:
		// “モンスターにフォーカスを設定して、
		// 　ソウマ「neruros」のソースを入力してください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 操作ロック解除
			setLockWithParent(false);
			// GUI のサブモジュールの自動有効・無効化機能を切る
			Game::getGame()->getGui()->setExecFlags(GameGui::EF_EXT_ENABLE_CTRL, true);
			// フォーカスのための GUI サブモジュール以外を無効化
			FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
			if(!fccsr->isEnable()) { fccsr->setEnable(true); }
			FocusMenu* fcmenu = Game::getGame()->getGui()->getFocusMenu();
			if(!fcmenu->isEnable()) { fcmenu->setEnable(true); }
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
			if(mvcsr->isEnable(MoveCursor::EDCH_EVENT)) { mvcsr->setEnable(MoveCursor::EDCH_EVENT, false); }
			// インゲームフロントのソウマアイコンのみを有効化
			IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
			igfr->setEnable(true);
			igfr->setEneblePanel(IngameFront::BTN_SOUMA, true);
			igfr->setEneblePanel(IngameFront::BTN_MENU, false);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// カメラを調整
			Camera* cam = Game::getGame()->getSceneGraph()->getVisibleCamera();
			if(cam->getCameraClass() == GameCustomCamClass::CAMCLS_WALKING)
			{
				WalkingCam* wcam = (WalkingCam*)cam;
				wcam->setZoom(2000.0f);
			}
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_DO_SOUMA_1:
		// notifyUnitInfluenceStat か notifyDisappearMagicCluster で判定して次のステップへ進める
		break;
	case STEP_SHOW_RESULT_SOUMA_1:
		{
			// エネルギーを満タンに
			((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();
			// 最初のソウマの結果メッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_RESULT_SOUMA_1, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_MSG_RESULT_SOUMA_1:
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// 戦闘へ
			if(!Game::getGame()->isConfront())
			{
				Game::getGame()->getConfront()->start(_sprtEnemy->unit(), 0, _sprtPtnr->getUnit());
			}
			// コルポをイベントモードへ
			_sprtPtnr->beginEventMode();
			// 敵を最初の攻撃ステップへ
			ASSERT(_sprtEnemy != 0L);
			_sprtEnemy->setStep(Char1404_BattleTutorial_EnemySpirit::TSTEP_ATTACK_1);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 経過フレームリセット
			_frameElapsedWk = 0.0f;
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_ENEMY_ATTACK_1:
		if(_frameElapsedWk >= (FRAMERATE * 6.7f))
		{
			// 敵を最初の攻撃受けステップへ
			ASSERT(_sprtEnemy != 0L);
			_sprtEnemy->setStep(Char1404_BattleTutorial_EnemySpirit::TSTEP_WAIT_1);
			// 敵の最初の攻撃に対するメッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_ENEMY_ATTACK_1, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_MSG_ENEMY_ATTACK_1:
		// 敵の最初の攻撃に対するメッセージ終了待ち
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// コルポのイベントモードを解除
			_sprtPtnr->endEventMode();
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 経過フレームリセット
			_frameElapsedWk = 0.0f;
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_PC2_ATTACK_1:
		if(_frameElapsedWk >= (FRAMERATE * 10.0f))	// 10 秒
		{
			jumpWaterEnchantmentSoumaEvent();
			/*
			// コルポをイベントモードへ
			_sprtPtnr->beginEventMode();
			// PC2 の最初の攻撃に対するメッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_PC2_ATTACK_1, 0, &evcause);
			// 次のステップへ
			advanceStep();
			*/
		}
		break;
	case STEP_MSG_PC2_ATTACK_1:
		// PC2 の最初の攻撃に対するメッセージ終了待ち
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// コルポをイベントモードへ
			_sprtPtnr->beginEventMode();
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// テクスチャを読み込む
			readTexture(FILEID_SPRITE_PC2_ATTACK_1);
			// 操作ロック有効
			setLockWithParent(true);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_SPRITE_PC2_ATTACK_1:
		// “コルポの持つこん棒による物理攻撃は、肉体を持たない霊体にはそのままでは当たりません。
		// 　しかし、霊体によるエネルギー攻撃は肉体と霊体の両方を持つ人間のような存在には有効です。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 操作ロック解除
			setLockWithParent(false);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 水付帯ソウマを促すメッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_PLEASE_SOUMA_2, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_MSG_SOUMA_2:
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// コルポをイベントモードへ
			_sprtPtnr->beginEventMode();
			// テクスチャを読み込む
			readTexture(FILEID_SPRITE_PC2WEP_FOCUS);
			// フォーカスのための GUI サブモジュールを有効化
			CharFronts* chrfrnt = Game::getGame()->getGui()->getCharFronts();
			if(!chrfrnt->isEnable())		{ chrfrnt->setEnable(true); }
			// 操作ロック有効
			setLockWithParent(true);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_SPRITE_PC2WEP_FOCUS:
		// “コルポの持つこん棒にソウマをかけて、霊体にも当たるようにエネルギーを加えます。
		// 　コルポのアイコンをタップして、メニューから「武器をフォーカス」を選んでください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// PC2 メニュー項目を制限
			setEnablePc2MenuExceptWeapon(false);
			// 操作ロック解除
			setLockWithParent(false);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 経過フレームリセット
			_frameElapsedWk = 0.0f;
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_PC2WEP_FOCUS:
		// コルポの武器にフォーカスが当たるのを待つ
		if(_frameElapsedWk >= (FRAMERATE * 60.0f))	// 60 秒
		{
			jumpWaterEnchantmentSoumaEvent();	// タイムオーバー：水付帯促進イベントへジャンプ
		}
		else
		{
			// フォーカスされたら次へ
			FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
			if(fccsr->getFocus(0) == _unitPtnrWeapon)
			{
				// テクスチャを読み込む
				readTexture(FILEID_SPRITE_SOUMA_2);
				// 操作ロック有効
				setLockWithParent(true);
				// 次のステップへ
				advanceStep();
			}
		}
		break;
	case STEP_SPRITE_SOUMA_2:
		// “武器にフォーカスが設定されたら、
		// 　ソウマ「neruhamad」のソースを入力してください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 操作ロック解除
			setLockWithParent(false);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// コルポのイベントモードを解除
			_sprtPtnr->endEventMode();
			// 経過フレームリセット
			_frameElapsedWk = 0.0f;
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_SOUMA_2:
		if(_frameElapsedWk >= (FRAMERATE * 60.0f))	// 60 秒
		{
			jumpWaterEnchantmentSoumaEvent();	// タイムオーバー：水付帯促進イベントへジャンプ
		}
		// notifyUnitInfluenceStat か notifyDisappearMagicCluster で判定して次のステップへ進める
		break;
	case STEP_SHOW_FAIL_SOUMA_2:
		if(_frameElapsedWk >= (FRAMERATE * 0.5f))
		{
			// ベール効果失敗メッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_MSG_FAIL_SOUMA_2_EF, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_MSG_FAIL_SOUMA_2:
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// エネルギーを満タンに
			((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 戻る
			jumpStep(STEP_WAIT_SOUMA_2);
		}
		break;
	case STEP_SHOW_SUCCESS_SOUMA_2:
		if(_frameElapsedWk >= (FRAMERATE * 2.0f))
		{
			// PC1、PC2 のエネルギーを満タンに
			((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();
			((PartnerUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_PTN))->charStat()->recoverFull();
			// ベール効果成功メッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_MSG_SUCCESS_SOUMA_2_EF, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_MSG_SUCCESS_SOUMA_2:
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// PC2 メニュー項目の制限解除
			setEnablePc2MenuExceptWeapon(true);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 進む
			jumpStep(STEP_ENTER_FREE_BATTLE_1);
		}
		break;
	case STEP_ENTER_FREE_BATTLE_1:
		// 自由戦闘へ
		{
			// 敵を攻撃ステップへ
			ASSERT(_sprtEnemy != 0L);
			_sprtEnemy->setStep(Char1404_BattleTutorial_EnemySpirit::TSTEP_ATTACK_2);
			// コルポのイベントモードを解除
			_sprtPtnr->endEventMode();
			// 経過フレームリセット
			_frameElapsedWk = 0.0f;
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_DO_FREE_BATTLE_1:
		// しばらく自由戦闘
		if(_frameElapsedWk >= (FRAMERATE * 7.0f))	// 7 秒
		{
			// 技発動ステップへ
			ASSERT(_sprtEnemy != 0L);
			_sprtEnemy->setStep(Char1404_BattleTutorial_EnemySpirit::TSTEP_SKILL_1);
			// コルポの技を停止（即座に再実行した場合は仕方なし）
			_sprtPtnr->breakBehavior(Spirit::BEHV_NULL, false, false);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_SKILL_1:
		// notifyUnitInfluenceStat か notifyDisappearMagicCluster で判定して次のステップへ進める
		break;
	case STEP_SHOW_RESULT_SKILL_1:
		if(_frameElapsedWk >= (FRAMERATE * 2.0f))
		{
			// 技を受けたメッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			_evmatter = evsys->begin(EVID_MSG_RESULT_SKILL_1, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_MSG_RESULT_SKILL_1:
		if((_evmatter != 0L) && _evmatter->isEnd())
		{
			// コルポをイベントモードへ
			_sprtPtnr->beginEventMode();
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// 敵を攻撃受けステップへ
			ASSERT(_sprtEnemy != 0L);
			_sprtEnemy->setStep(Char1404_BattleTutorial_EnemySpirit::TSTEP_WAIT_1);
			// テクスチャを読み込む
			readTexture(FILEID_SPRITE_PHYS_DAMAGE_1);
			// 操作ロック有効
			setLockWithParent(true);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_SPRITE_PHYS_DAMAGE_1:
		// “物理ダメージを受けると、ゲージが短くなりエネルギー最大値が減ります。
		// 　エネルギーはソウマなどで回復できますが、物理ダメージは怪我をしてしまっているため、一度コメサット村で休まないと回復しません。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 操作ロック解除
			setLockWithParent(false);
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
			// コルポのイベントモードを解除
			_sprtPtnr->endEventMode();
			// 敵を攻撃ステップへ
			ASSERT(_sprtEnemy != 0L);
			_sprtEnemy->setStep(Char1404_BattleTutorial_EnemySpirit::TSTEP_ATTACK_2);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_DO_FREE_BATTLE_2:
		{
			GameMode::ModeKind kindEnd = Game::getGame()->getGameMode()->getChildEndModeKind();
			if(kindEnd == GameMode::MODE_GAME_INGAME_WALKING_NORMAL)
			{
				// 終了
				this->end();
			}
		}
	}

	// フレームカウンタ更新
	_frameElapsedWk += ec->getDeltaFrame();
	// ステップ内フレームカウンタ更新
	if(step != getStep())	{	_frameElapsedStep = 0.0f;					}
	else					{	_frameElapsedStep += gec->getDeltaFrame();	}

	// スプライトのフレーム制御
	switch(getStep())
	{
	case STEP_SPRITE_SOUMA_1:
	case STEP_SPRITE_PC2_ATTACK_1:
	case STEP_SPRITE_PC2WEP_FOCUS:
	case STEP_SPRITE_SOUMA_2:
	case STEP_SPRITE_PHYS_DAMAGE_1:
		if(_sprite != 0L) { _sprite->exec(ec); }
		break;
	}

	/*/
	// 移動カーソルは常にオフに（MyRole::endAria で有効化してしまうので）
	disableMoveCursor(ec);
	/*/
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::draw(const RenderCtx* rc)
{
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = rc->getRenderer();

	switch(getStep())
	{
	case STEP_SPRITE_SOUMA_1:
	case STEP_SPRITE_PC2_ATTACK_1:
	case STEP_SPRITE_PC2WEP_FOCUS:
	case STEP_SPRITE_SOUMA_2:
	case STEP_SPRITE_PHYS_DAMAGE_1:
		if((_sprite != 0L) && _sprite->isValid())
		{
			_sprite->draw(rc);
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	switch(getStep())
	{
	case STEP_DO_SOUMA_1:
		if(unitRecv->getCtrlEvid() == CTRLEVID_TARGET)
		{
			const Spirit* sprtMy = Game::getGame()->getMyParty()->getPcSpirit(MyParty::PC_MY);
			if((tstat->getOwnerUnit()->getSpirit() == sprtMy))	// PC1 による何か
			{
				// 次のステップへ
				advanceStep();
			}
		}
		break;
	case STEP_WAIT_PC2WEP_FOCUS:
	case STEP_WAIT_SOUMA_2:
		if((unitRecv->getUnitType() == UNITTYPE_ENEMY_CHAR) && (unitRecv->getSpirit() == _sprtEnemy))
		{
			// 敵がステータス影響を受けた
			if((tstat->getOwnerUnit()->getSpirit() == _sprtPtnr))	// PC2 による攻撃
			{
				// ステータス影響を与えたのは、PC2 の武器である
				if(_unitPtnrWeapon->getEleneStat()->getVeil(EleneStat::VEIL_WP)->isValid())
				{
					// PC2 の武器はベールが有効である
					_frameElapsedWk = 0.0f;					// 経過フレームリセット
					jumpStep(STEP_SHOW_SUCCESS_SOUMA_2);	// 成功メッセージへ
				}
			}
		}
		else if((unitRecv->getSpirit() == _sprtPtnr) && (tstat->getKind() == TransStat::KIND_SOUMA))
		{
			// 誤ったソウマ、水しぶき（neruros）がコルポにかかった
			if((tstat->getFuncFlags() & EleneStat::FF_MASK_OP) != EleneStat::FF_OP_WRAP)
			{
				_frameElapsedWk = 0.0f;				// 経過フレームリセット
				jumpStep(STEP_SHOW_FAIL_SOUMA_2);	// 失敗メッセージへ
			}
		}
		else if((unitRecv == _unitPtnrWeapon) && (unitRecv->getUnitType() == UNITTYPE_EQUIPMENT_ITEM) && (tstat->getKind() == TransStat::KIND_SOUMA))
		{
			// 誤ったソウマ、水しぶき（neruros）が武器にかかった
			if((tstat->getFuncFlags() & EleneStat::FF_MASK_OP) != EleneStat::FF_OP_WRAP)
			{
				_frameElapsedWk = 0.0f;				// 経過フレームリセット
				jumpStep(STEP_SHOW_FAIL_SOUMA_2);	// 失敗メッセージへ
			}
		}
		break;
	case STEP_DO_FREE_BATTLE_1:
	case STEP_WAIT_SKILL_1:
		if((unitRecv->getSpirit() == _sprtPtnr) && (tstat->getKind() == TransStat::KIND_SKILL))
		{
			// 技の結果表示
			_frameElapsedWk = 0.0f;					// 経過フレームリセット
			jumpStep(STEP_SHOW_RESULT_SKILL_1);
		}
		break;
	}

	// この戦闘に限り、敵へのベールはキャンセルされる
	if((unitRecv->getUnitType() == UNITTYPE_ENEMY_CHAR) && (unitRecv->getSpirit() == _sprtEnemy))
	{
		if((tstat->getFuncFlags() & EleneStat::FF_MASK_OP) == EleneStat::FF_OP_WRAP)
		{
			unitRecv->eleneStat()->clearVeil();
			unitRecv->eleneStat()->updateResultValue(true);
		}
	}
}

/*---------------------------------------------------------------------*//**
	マジッククラスタの消滅通知
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags)
{
	if(TFW_IS_FLAG(dcflags, MagicCluster::DCF_IS_CREATION))	{	return;	}	// 創作クラスターは除外

	switch(getStep())
	{
	case STEP_DO_SOUMA_1:
	case STEP_WAIT_SOUMA_2:
		// notifyUnitInfluenceStat が呼ばれなかったみたいだ
		break;
	}
}

/*---------------------------------------------------------------------*//**
	テクスチャを読み込む
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::readTexture(u32 fileid)
{
	ASSERT(_sprite != 0L);

	// 以前のスプライトを削除する
	if(_sprite->isValid())
	{
		_sprite->destroy();
	}

	// 新たにスプライトを読み込む
	_sprite->create(fileid, FRAMECNT_DELAY_CLOSE_SPRITE, true, 0x0000007f, true);
}

/*---------------------------------------------------------------------*//**
	PC2 メニューの武器以外の項目を有効／無効にする
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::setEnablePc2MenuExceptWeapon(bool isEnable)
{
	const s32 MTNODEID_PC2_SETFOCUS		= 12001;
	const s32 MTNODEID_PC2_LEADER		= 12002;
	const s32 MTNODEID_PC2_GUARDCHARGE	= 12003;
	const s32 MTNODEID_PC2_ATTRACT		= 12004;
	const s32 MTNODEID_PC2_STOP			= 12005;

	FocusMenu* fcmenu = Game::getGame()->getGui()->getFocusMenu();
	if(fcmenu == 0L)	{	return;	}
	fcmenu->setMenuElementEnable(FocusMenu::MENU_PC2, MTNODEID_PC2_SETFOCUS, isEnable);
	fcmenu->setMenuElementEnable(FocusMenu::MENU_PC2, MTNODEID_PC2_LEADER, isEnable);
	fcmenu->setMenuElementEnable(FocusMenu::MENU_PC2, MTNODEID_PC2_GUARDCHARGE, isEnable);
	fcmenu->setMenuElementEnable(FocusMenu::MENU_PC2, MTNODEID_PC2_ATTRACT, isEnable);
	fcmenu->setMenuElementEnable(FocusMenu::MENU_PC2, MTNODEID_PC2_STOP, isEnable);
}

/*---------------------------------------------------------------------*//**
	水付帯ソウマを促すイベントへジャンプする
**//*---------------------------------------------------------------------*/
void StEmp_10102_1001_BattleTutorial01::jumpWaterEnchantmentSoumaEvent()
{
	// コルポをイベントモードへ
	_sprtPtnr->beginEventMode();
	// PC2 の最初の攻撃に対するメッセージイベント起動
	EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
	EvCause evcause(EventCause::CID_EVENT);
	_evmatter = evsys->begin(EVID_MSG_PC2_ATTACK_1, 0, &evcause);
	// 経過フレームリセット
	_frameElapsedWk = 0.0f;
	// 次のステップへ
	jumpStep(STEP_MSG_PC2_ATTACK_1);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
