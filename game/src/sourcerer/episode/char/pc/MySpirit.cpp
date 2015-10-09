/***********************************************************************//**
 *	MySpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MySpirit.h"

// Friends
#include "MyUnit.h"
#include "../../../camera/GazeCam.h"
#include "../../../body/Game.h"
#include "../../../effect/GaugeAnimEffect.h"
#include "../../../episode/FixedSourceIdDef.h"
#include "../../../episode/data/FixedData.h"
#include "../../../es/Es.h"
#include "../../../es/EsContext.h"
#include "../../../es/EsExternalExtension.h"
#include "../../../es/EsRunStat.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/MoveCursor.h"
#include "../../../gui/window/implement/AriaWindow.h"
#include "../../../magic/MagicCluster.h"
#include "../../../magic/MagicSys.h"
#include "../../../source/FixedSourceDef.h"
#include "../../../source/FixedSourceDefTbl.h"
#include "../../../souma/Souma.h"
#include "../../../source/Source.h"
#include "../../../source/SourceManager.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/Elene.h"
#include "../../../status/StatusChangeResult.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/sg/CamSgnc.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// +++ Adjustable Parameter >>>
// フィールドアイテムを拾える距離
#define DIST_PICKUP_ITEM			(80.0f)
#define DIST_PICKUP_ITEM_SQ			(DIST_PICKUP_ITEM * DIST_PICKUP_ITEM)
// +++ Adjustable Parameter <<<

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

#if 0
/*---------------------------------------------------------------------*//**
	フォーカス設定
**//*---------------------------------------------------------------------*/
void MySpirit::setFocusUnit(Unit* unit, bool isByGui)
{
#if 0
	if(_souma->isRunning() && !isByGui)
	{
		// ソウマ実行中は自動でフォーカス変更しない
	}
	else
	{
		Spirit::setFocusUnit(unit, isByGui);
	}
#else
	if(isByGui || (unit == 0L))
	{
		// GUI によるものとフォーカス外しは反映
		PcSpirit::setFocusUnit(unit, isByGui);
	}
#endif
}
#endif

/*---------------------------------------------------------------------*//**
	行動を開始する
**//*---------------------------------------------------------------------*/
void MySpirit::startBehavior(Behavior behv, const BehaviorParam* bparam)
{
	PcSpirit::startBehavior(behv, bparam);

	// アクション待ちではなく、即座に実行するものの開始
	switch(_behvCur)
	{
	case BEHV_SOUMA:
		{
			const SoumaBehaviorParam* sbp = (const SoumaBehaviorParam*)bparam;
			startSouma(sbp->_sourceCall, sbp->_srckindCall, sbp->_isUserCustomizedSource);
		}
		break;
	case BEHV_MEDITATION:
		{
			const FixedSourceDefTbl* fsrcdftbl = FixedData::getFixedSourceDefTable(); ASSERT(fsrcdftbl != 0L);
			const FixedSourceDef* fsrcdef = fsrcdftbl->getDef(FixedSourceIdDef::FSRCID_EP01_AIER);
			if(fsrcdef != 0L)
			{
				VcString source(fsrcdef->getSsSource());
				source += "\n";
				source += *fsrcdef->getName();
				source += " a o.\n";
				startSouma(&source, SourceDef::SRCKIND_SS, false);
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	行動の中断

	@param behv 中断する行動（BEHV_NULL は全てを対象とした中断）
**//*---------------------------------------------------------------------*/
void MySpirit::breakBehavior(Behavior behv, bool isByGui, bool duetoConfu)
{
	if((_behvCur == behv) || (behv == BEHV_NULL))
	{
		// アクション待ちではなく、即座に実行するものの中断
		switch(_behvCur)
		{
		case BEHV_SOUMA:
		case BEHV_MEDITATION:
			if(_souma->isRunning())
			{
				_souma->terminate();
			}
			break;
		}
	}

	PcSpirit::breakBehavior(behv, isByGui, duetoConfu);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MySpirit::MySpirit()
	: PcSpirit(SPRTTYPE_MY)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MySpirit::create()
{
	// モードの作成
	MyMode* mode = new MyMode();
	if(!mode->create(this))
	{
		delete mode;
		return false;
	}

	if(!PcSpirit::create(mode))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MySpirit::destroy()
{
	// ソウマの削除
	if(_souma != 0L)
	{
		_souma->destroy();
		delete _souma;
		_souma = 0L;
	}

	PcSpirit::destroy();
}

/*---------------------------------------------------------------------*//**
	作成された自らのキャラユニットをバインド
**//*---------------------------------------------------------------------*/
bool MySpirit::bind(CharUnit* cunitSelfRef, const CharDef* chrdef, const CharLogicData* chrlgd)
{
	PcSpirit::bind(cunitSelfRef, chrdef, chrlgd);

	// ソウマの作成
	_souma = new Souma();
	if(!_souma->create(this, cunitSelfRef))
	{
		delete _souma;
		_souma = 0L;
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void MySpirit::notifyChangeMapBefore()
{
	// 強制中断
	if(_souma->isRunning())	// ソウマ実行中
	{
		_souma->terminate();	// 強制中断
	}

	PcSpirit::notifyChangeMapBefore();
}

/*---------------------------------------------------------------------*//**
	ステータス影響発生通知
**//*---------------------------------------------------------------------*/
void MySpirit::notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat)
{
	// ダメージ時の意識低下で強制中断
	if(_souma->isRunning())	// ソウマ実行中
	{
		if(tstat->getOwnerUnit() != getUnit())	// 自らのソウマ故ではなく
		{
			if(TFW_IS_FLAG(tstat->getQualityFlags(), EleneStat::QF_EXCT_1) || TFW_IS_FLAG(tstat->getQualityFlags(), EleneStat::QF_DULL_1))
			{
				if(_cstatRef->getConcentrateDepth() == 0)	// 意識深さがゼロである
				{
					_souma->terminate();	// 強制中断
				}
			}
		}
	}

	PcSpirit::notifyInfluenceStat(scres, tstat);
}

/*---------------------------------------------------------------------*//**
	ソウマ アクション通知
**//*---------------------------------------------------------------------*/
void MySpirit::notifySoumaAction(s32 snotifid, const StatusChangeResult* scres, const VcString* msg)
{
	((MyRole*)pcMode()->pcRole())->notifySoumaAction(snotifid, scres, msg);	// ロールに伝達

	PcSpirit::notifySoumaAction(snotifid, scres, msg);
}

/*---------------------------------------------------------------------*//**
	対決ゲームモード変更通知
**//*---------------------------------------------------------------------*/
void MySpirit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
#if 1
	// ソウマ詠唱中はモードを切り替えない
	if(_souma->isRunning())
	{
		pcMode()->pcRole()->setConfrontModule(confront);	// 対決モジュールをロールに設定する
		Spirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);	// PcSpirit の notifyChangedConfront はモードを切り替えてしまうためスキップする
		return;
	}

	PcSpirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);	// PcSpirit の notifyChangedConfront はモードを切り替えてしまうためスキップする

#else
	if(isConfront)
	{
		// 戦闘モードへ
		((MyMode*)_mode)->toConfront(confrontRef);
	}
	else if(!_souma->isRunning())	// ソウマ実行中以外
	{
		// 歩行モードへ
		((MyMode*)_mode)->toWalking(true);
	}

	Spirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);

#endif
}

/*---------------------------------------------------------------------*//**
	ソウマ 開始
**//*---------------------------------------------------------------------*/
bool MySpirit::startSouma(const VcString* srccode, u8 srckind, bool isUserCustomizedSource)
{
	// 実行済みの場合は強制終了
	if(_souma->isRunning())
	{
		_souma->terminate();
	}

	// 生存確認
	if(getCharStat()->getStat() != CharStat::STAT_ALIVE)
	{
		return false;	// 失敗
	}

	// 入力確認
	if(srccode == 0L)						{	return false;	}
	if(srccode->getLength() <= 0)			{	return false;	}
	if(srckind == SourceDef::SRCKIND_NULL)	{	return false;	}

	// ロールを一度終了する
	((MyMode*)_mode)->toNull();

	// ソウマ開始
	if(!_souma->start(srccode, (SourceDef::SourceKind)srckind, isUserCustomizedSource ? Souma::F_USER_CUSTOMIZED_SOURCE : 0))
	{
		// 失敗．歩行に戻る
		((MyMode*)_mode)->toWalking(false);

		return false;
	}

	// 詠唱に入る
	((MyMode*)_mode)->toAria();

	// ゲージアニメの開始
	Game::getGame()->getGaugeAnimEffect()->startSoumaMaking(getPcUnit());

	return true;
}

/*---------------------------------------------------------------------*//**
	ソウマ 終了
**//*---------------------------------------------------------------------*/
bool MySpirit::endSouma()
{
	// ソウマ終了
	if(!_souma->end(true))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ソウマ 実行
**//*---------------------------------------------------------------------*/
bool MySpirit::execSouma(const ExecCtx* ec, const VcString** code, bool* isStep, bool* isRun, s32* idxBegin, s32* idxEnd)
{
	_souma->exec(isStep, ec);
	_souma->getState(code, isRun, idxBegin, idxEnd);

	return true;
}

//==========================================================================
// MySpirit::MyRole メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MySpirit::MyRole::MyRole(MySpirit* sprtRef)
	: PcRole(sprtRef)
	, _camGaze(0L)
	, _camidBk(0)
	, _wndAriaRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MySpirit::MyRole::~MyRole()
{
	ASSERT(_camGaze == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MySpirit::MyRole::create(Mode* modeOwnerRef)
{
	if(!PcRole::create(modeOwnerRef))	{	return false;	}

	// 専用カメラを作成する
	GazeCam* cam = new GazeCam();
	if(!cam->create())
	{
		delete cam;
		ASSERTM(false, "{MySpirit::MyRole::create} GazeCam-create failed.");
		return false;
	}
	_camGaze = cam;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MySpirit::MyRole::destroy()
{
	// 専用カメラを削除する
	if(_camGaze != 0L)
	{
		// カメラをシーングラフから削除する
		if(_camGaze->isOnSg())
		{
			SorSgm* sgm = Game::getGame()->getSceneGraph();
			sgm->removeNode(_camGaze);
		}

		_camGaze->destroy();
		delete _camGaze;
		_camGaze = 0L;
	}

	PcRole::destroy();
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int MySpirit::MyRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	switch(param->_cid)
	{
	case NOTHING:			return execCommand_nothing(res, ec, param);			// 何もしない
	case WALK_NORMAL:		return execCommand_walkNormal(res, ec, param);		// 通常歩行
	case CHANTING:			return execCommand_chanting(res, ec, param);		// ソウマ詠唱
	case USE_ITEM:			return execCommand_useItem(res, ec, param);			// アイテム使用
	case PICKUP_ITEM:		return execCommand_pickupItem(res, ec, param);		// アイテム拾い
	case ENTER_CF:			return execCommand_enterCf(res, ec, param);			// 対決開始
	case LEAVE_CF:			return execCommand_leaveCf(res, ec, param);			// 対決終了
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 通常歩行
**//*---------------------------------------------------------------------*/
int MySpirit::MyRole::execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
#if 0	// 【2012/03/30 r-kishi】コルポだけにやらせる
	// アイテム拾い処理
	u16 unitidFiUnit = _sprtRef->searchFieldItem(DIST_PICKUP_ITEM_SQ);
	if(unitidFiUnit != 0)
	{
		Spirit::PickupItemBehaviorParam ibp(unitidFiUnit);
		_sprtRef->startBehavior(Spirit::BEHV_PICKUP_ITEM, &ibp);
	}
#endif

	return PcRole::execCommand_walkNormal(res, ec, param);
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - ソウマ詠唱
**//*---------------------------------------------------------------------*/
int MySpirit::MyRole::execCommand_chanting(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_START			= 1;
	const int	STEP_CHANTING		= 2;
	const int	END					= 999;
	int step = param->_step;

	updateChantingPerf(_sprtRef->isLeader() && (step == STEP_CHANTING));	// 演出更新
	
	_sprtRef->setBefaviorCurrentFrame(param->_frmcntCmd);	// 現在の行動フレーム数設定

	switch(step)
	{
	case STEP_START:
		{
			_wndAriaRef = Game::getGame()->getGui()->getMyAriaWindow(); ASSERT(_wndAriaRef != 0L);
			if(_wndAriaRef != 0L)
			{
				// ソースのリセット
				_wndAriaRef->setSourceCode(TypeUtils::getEmptyString(), true);
			}

			// キャラクタのアニメーション変更
			_sprtRef->pcUnit()->changeAnim(MyUnit::ANIMID_MAGIC_CHANT_1, true);

			step = STEP_CHANTING;
		}
		break;

	case STEP_CHANTING:
		ASSERT(_wndAriaRef != 0L);
		if(_wndAriaRef != 0L)
		{
			// ソウマ実行処理
			const VcString* code = 0L;
			bool isStep = false;
			bool isRun = false;
			s32 idxBegin = 0;
			s32 idxEnd = 0;
			getThisSpirit()->execSouma(ec, &code, &isStep, &isRun, &idxBegin, &idxEnd);

			if(isStep)
			{
				// 詠唱ウインドウにソウマを設定し、ウインドウの位置決めのヒントを与える
				_wndAriaRef->setSouma(getThisSpirit()->getSouma());
				// 詠唱ウインドウにソースを設定し、アクティブテキストを表示する
				if(_wndAriaRef->isEmptySourceCode())	{	_wndAriaRef->setSourceCode(code, false);	}
				_wndAriaRef->setActiveCode(idxBegin, idxEnd);

				// 終了処理
				if(!isRun)	// 終了した
				{
					// 詠唱終了処理
					endChanting();
					step = END;
				}
				else	// 実行中
				{
					if(_camGaze->isLeaveRequest())
					{
						// カメラ自体からの切替要求があるかを確認
						restoreGazeCamera();
					}
				}
			}
			else if(!isRun)	// 終了した
			{
				// 詠唱終了処理
				endChanting();
				step = END;
			}
		}
		break;
		
	case END:
		if(param->_frmcntStep >= 30.0f)
		{
			res->setDone();
		}
		break;
	}

	//STRACE("... aria ...\n");
	
	return step;
}

/*---------------------------------------------------------------------*//**
	終了通知
**//*---------------------------------------------------------------------*/
void MySpirit::MyRole::notifyEnd()
{
	// この時点で詠唱ウインドウが残っているということは、
	// なんらかの原因で詠唱が強制終了した
	if(_wndAriaRef != 0L)
	{
		// 詠唱の強制終了
		endChanting();
	}
}

/*---------------------------------------------------------------------*//**
	ソウマ アクション通知
**//*---------------------------------------------------------------------*/
void MySpirit::MyRole::notifySoumaAction(s32 snotifid, const StatusChangeResult* scres, const VcString* msg)
{
	switch(snotifid)
	{
	case Souma::SNOTIFID_WASTE_ENE:
	case Souma::SNOTIFID_FORCE_WASTE_ENE:
//		Game::getGame()->getGaugeAnimEffect()->onDamage(_sprtRef->getUnit(), scres->getEneDamage(), scres->getNewCharStat()->getMaxEnergy());	// ゲージアニメのダメージ通知
		break;
	case Souma::SNOTIFID_SERVE:
		Game::getGame()->getGaugeAnimEffect()->endSoumaMaking(_sprtRef->getUnit());	// ゲージアニメの作成終了通知
		break;
	}
}

/*---------------------------------------------------------------------*//**
	詠唱の終了
**//*---------------------------------------------------------------------*/
void MySpirit::MyRole::endChanting()
{
	if(_wndAriaRef == 0L)	{	return;	}

	MyUnit* unitMy = getThisSpirit()->getThisUnit(); ASSERT(unitMy != 0L);

	// 演出更新
	updateChantingPerf(false);

	// ウインドウを NULL 化
	_wndAriaRef = 0L;	// フラグの代わりに使うので NULL 化する

	// ソウマ終了
	getThisSpirit()->endSouma();

	// キャラクタのアニメーション変更
	u16 animidCur = unitMy->getCurrentAnimId();
	if(animidCur == MyUnit::ANIMID_MAGIC_CHANT_1)
	{
		unitMy->changeAnim(MyUnit::ANIMID_STOP, true);
	}

	// ゲージアニメの作成終了通知（主に中断時用）
	Game::getGame()->getGaugeAnimEffect()->endSoumaMaking(_sprtRef->getUnit());
}

/*---------------------------------------------------------------------*//**
	注視カメラを元に戻す
**//*---------------------------------------------------------------------*/
void MySpirit::MyRole::restoreGazeCamera()
{
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(_camidBk != 0)
	{
		if(	(sgm->getNodeFromId(_camidBk) != 0L) &&		// カメラがまだ存在するときのみ（destroy 中など、無い場合もある）
			(sgm->getVisibleCamera() == _camGaze) )		// かつ、削除しようとしているカメラが有効のとき
		{
			CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
			nodecCam->switchVisible(_camidBk);	// カメラを戻す
		}
		_camidBk = 0;
	}
	if(_camGaze->isOnSg())
	{
		sgm->removeNode(_camGaze);
	}
}

/*---------------------------------------------------------------------*//**
	詠唱中演出の状態を更新する
**//*---------------------------------------------------------------------*/
void MySpirit::MyRole::updateChantingPerf(bool isChantingPerf)
{
	// 状態更新
	if(_isChantPerf != isChantingPerf)
	{
		// 状態変更時のみの更新
		_isChantPerf = isChantingPerf;

		// 詠唱ウインドウはリーダー時、かつ詠唱中にのみ表示する
		if(_wndAriaRef != 0L)
		{
			bool isShowAriaWnd = _isChantPerf;
			
			// 詠唱ウインドウのモード設定
			if(TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_ARIA_WINDOUW_RAW_SRC))
			{
				if(_wndAriaRef->getMode() == AriaWindow::MODE_SPELL)	{	_wndAriaRef->setMode(AriaWindow::MODE_RAW);		}
			}
			else
			{
				if(_wndAriaRef->getMode() == AriaWindow::MODE_RAW)		{	_wndAriaRef->setMode(AriaWindow::MODE_SPELL);	}
			}
			// フェード中のことがあるため状態をチェックせずにウインドウを表示させる　///if(_wndAriaRef->isShowWindow() != isShowAriaWnd)
			_wndAriaRef->showWindowWithFade(isShowAriaWnd, AriaWindow::FCNT_WINDOWS_FADE);
		}

		// 移動カーソルは非リーダー時、かつ詠唱中にのみ非表示する
		MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
		if(mvcsr != 0L)
		{
			bool isShowMvcsr = !_isChantPerf;
			if(mvcsr->isEnable(MoveCursor::EDCH_PC1) != isShowMvcsr)
			{
				mvcsr->setEnable(MoveCursor::EDCH_PC1, isShowMvcsr);
			}
		}

		// カメラ更新
		SorSgm* sgm = Game::getGame()->getSceneGraph();
		if(sgm != 0L)
		{
			// 専用カメラに設定／戻す（対決中以外）
			if(_isChantPerf && !_sprtRef->isConfront())
			{
				// 現在のカメラ ID を得る
				Camera* cam = sgm->getVisibleCamera();
				if(cam != 0L)
				{
					_camidBk = cam->getId();	// 現在のカメラの ID を得ておく

					// カメラを変更する
					_camGaze->setTarget(getThisSpirit()->getThisUnit());
					_camGaze->setMode(GazeCam::MODE_CHANTING);
					if(sgm->addNode(_camGaze, false))
					{
						CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
						nodecCam->switchVisible(_camGaze->getId());	// カメラを有効化
					}
				}
			}
			else
			{
				// カメラを戻す
				restoreGazeCamera();
			}

			#if 0
				// 背景表示更新
				sgm->showBg(!_isChantPerf);
			#endif
		}
	}

	// ゲージアニメ更新
	GaugeAnimEffect* ggaeff = Game::getGame()->getGaugeAnimEffect();
	if(ggaeff != 0L)
	{
		// 値の更新
		if(_isChantPerf)	// 有効時
		{
			do
			{
				Souma* souma = getThisSpirit()->getSouma();
				if(souma == 0L)									{	break;	}

				u16 mcid = souma->getCurClusterId();
				if(mcid == 0)									{	break;	}

				MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
				MagicCluster* mc = mgcsys->getCluster(mcid); ASSERT(mc != 0L);
				if(mc == 0L)									{	break;	}

				const TransStat* tstat = mc->getTransStat();
				if(tstat == 0L)									{	break;	}

				const Unit* unitOwner = tstat->getOwnerUnit();
				if(unitOwner == 0L)								{	break;	}
				if(!unitOwner->isCategory(Unit::UNITCAT_CHAR))	{	break;	}
				CharUnit* cunitOwner = ((CharUnit*)unitOwner);

				const PointF32* posEestat = _wndAriaRef->getEleneStatPos();
				const Elene* elenePreset = mc->getPresetElene();
				ggaeff->updateSoumaMaking(cunitOwner, (f32)posEestat->x(), (f32)posEestat->y() - 10.0f, tstat->getEleneStat()->getDfv()->getSoumaMaterializeLifeEnergy(), cunitOwner->getCharStat()->getMaxEnergy(), (elenePreset != 0L) ? elenePreset->getSoumaMaterializeLifeEnergy() : 0);
			}
			while(false);
		}
	}
}

//==========================================================================
// MySpirit::MyMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	詠唱モードへ移行
**//*---------------------------------------------------------------------*/
void MySpirit::MyMode::toAria()
{
	changeMode((s16)ACT_CHANTING, false);
}

/*---------------------------------------------------------------------*//**
	アクション実行
**//*---------------------------------------------------------------------*/
bool MySpirit::MyMode::action()
{
	for(int itry = 0; itry < 2; itry++)	// judgeConfrontAutoBehavior 内での行動決定のため２回実行する
	{
		switch(_sprtRef->getBehavior())
		{
		case Spirit::BEHV_USE_ITEM:
			if(_sprtRef->getReservedBefaviorParameter() != 0)
			{
				changeMode((s16)ACT_USE_ITEM, false);
				return true;
			}
			break;
		case Spirit::BEHV_PICKUP_ITEM:
			if(_sprtRef->getReservedBefaviorParameter() != 0)
			{
				changeMode((s16)ACT_PICKUP_ITEM, false);
				return true;
			}
			break;
		case Spirit::BEHV_DEFAULT:
			if(!judgeConfrontAutoBehavior())
			{
				if(getCurrentModeId() != ACT_WALKING)
				{
					changeMode((s16)ACT_WALKING, false);
				}
			}
			break;
		}
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MySpirit::MyMode::create(MySpirit* sprtRef)
{
	// ロールの作成
	if(!PcMode::create(sprtRef, new MyRole(sprtRef)))
	{
		return false;
	}

	// モード登録と初期モード設定
	registerMode((s16)ACT_WALKING, new Mode());
	registerMode((s16)ACT_EVENT, new Mode());
	registerMode((s16)ACT_CHANTING, new Mode());
	registerMode((s16)ACT_USE_ITEM, new Mode());
	registerMode((s16)ACT_PICKUP_ITEM, new Mode());
	registerMode((s16)ACT_ENTER_CONFRONT, new Mode());
	registerMode((s16)ACT_LEAVE_CONFRONT, new Mode());
	changeMode((s16)ACT_WALKING, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	子モードの開始通知
**//*---------------------------------------------------------------------*/
void MySpirit::MyMode::notifyChildModeBegin(short modeid, void* objParamRef)
{
	getSelfRole()->reset();
	switch(modeid)
	{
	case ACT_WALKING:
		getSelfRole()->stackCommand(MySpirit::MyRole::WALK_NORMAL, 0L);
		getSelfRole()->begin();
		break;
	case ACT_CHANTING:
		getSelfRole()->stackCommand(MySpirit::MyRole::CHANTING, 0L);
		getSelfRole()->begin();
		break;
	case ACT_EVENT:
		getSelfRole()->stackCommand(MySpirit::MyRole::NOTHING, 0L);
		getSelfRole()->begin();
		break;
	case ACT_USE_ITEM:
		getSelfRole()->stackCommand(MySpirit::MyRole::USE_ITEM, 0L);
		getSelfRole()->begin();
		break;
	case ACT_PICKUP_ITEM:
		getSelfRole()->stackCommand(MySpirit::MyRole::PICKUP_ITEM, 0L);
		getSelfRole()->begin();
		break;
	case ACT_ENTER_CONFRONT:
		getSelfRole()->stackCommand(MySpirit::MyRole::ENTER_CF, 0L);
		getSelfRole()->begin();
		break;
	case ACT_LEAVE_CONFRONT:
		getSelfRole()->stackCommand(MySpirit::MyRole::LEAVE_CF, 0L);
		getSelfRole()->begin();
		break;
	}
}

/*---------------------------------------------------------------------*//**
	対決時の自動行動判定
**//*---------------------------------------------------------------------*/
bool MySpirit::MyMode::judgeConfrontAutoBehavior()
{
	if(!_sprtRef->isConfront())	{	return false;	}	// 非対決時
	if(_sprtRef->isLeader())	{	return false;	}	// リーダー時

	// 体力が半分以下ならば MEDITATION へ
	const CharStat* cstat = _sprtRef->getCharStat();
	if((cstat->getEnergy() * 2) < cstat->getMaxEnergy())
	{
		_sprtRef->startBehavior(Spirit::BEHV_MEDITATION, 0L);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
