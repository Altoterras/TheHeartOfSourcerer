/***********************************************************************//**
 *	ConfrontMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ConfrontMode.h"

// Friends
#include "ConfrontRole.h"
#include "../MagicTypingMode.h"
#include "../ItemUsingMode.h"
#include "../../../confront/Confront.h"
#include "../../../body/Game.h"
#include "../../../camera/ConfrontCam.h"
#include "../../../episode/char/pc/MyUnit.h"
#include "../../../magic/MagicCluster.h"
#include "../../../magic/MagicSys.h"
#include "../../../souma/Souma.h"
#include "../../../spirit/Spirit.h"
#include "../../../status/CharStat.h"
#include "../../../status/FocusArray.h"
#include "../../../status/EleneStat.h"
#include "../../../status/StatusChangeResult.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/sg/CamSgnc.h"
#include "../../../../etk/sg/SorSgm.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ConfrontMode メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ConfrontMode::ConfrontMode()
	: _confrontRef(0L)
	, _camCf(0L)
	, _camidBk(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ConfrontMode::~ConfrontMode()
{
	ASSERT(_camCf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ConfrontMode::create()
{
	if(!GameModeBase::create(GameMode::MODE_GAME_INGAME_CONFRONT__START_, GameMode::MODE_GAME_INGAME_CONFRONT__END_, new ConfrontRole()))
	{
		return false;
	}

	// モード登録
	registerMode((s16)GameMode::MODE_GAME_INGAME_CONFRONT_NORMAL, new GameModeBase());
	registerMode((s16)GameMode::MODE_GAME_INGAME_CONFRONT_MAGICTYPING, new MagicTypingMode());
	registerMode((s16)GameMode::MODE_GAME_INGAME_CONFRONT_ITEMUSING, new ItemUsingMode());

	// 対決モジュールを取得する
	_confrontRef = Game::getGame()->getConfront();
	((ConfrontRole*)getSelfRole())->setConfrontModule(_confrontRef);	// 対決モジュールを設定する

	// 専用カメラを作成する
	ConfrontCam* cam = new ConfrontCam();
	if(!cam->create())
	{
		delete cam;
		ASSERTM(false, "{ConfrontMode::create} ConfrontCam-create failed.");
		return false;
	}
	_camCf = cam;
	((ConfrontRole*)getSelfRole())->setCamera(_camCf);	// ロールにカメラを設定する

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ConfrontMode::destroy()
{
	// 専用カメラを削除する
	if(_camCf != 0L)
	{
		_camCf->destroy();
		delete _camCf;
		_camCf = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool ConfrontMode::notifyBegin(void* objParamRef)
{
	TRACE("{ConfrontMode::notifyBegin} called.\n");

	// 現在のカメラ ID を得る
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	Camera* cam = sgm->getVisibleCamera();
	_camidBk = cam->getId();	// 現在のカメラの ID を得ておく

	// カメラを変更する
	if(sgm->addNode(_camCf, false))	// リソースプールに追加しないで自力管理する
	{
		CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
		nodecCam->switchVisible(_camCf->getId());	// カメラを有効化
	}

	// ロールコマンド設定
	getSelfRole()->reset();
	getSelfRole()->stackCommand(ConfrontRole::START, 0L);
	getSelfRole()->stackCommand(ConfrontRole::STABLE, 0L);
	getSelfRole()->stackCommand(ConfrontRole::CONCLUSION, 0L);
	getSelfRole()->stackCommand(ConfrontRole::END, 0L);
	getSelfRole()->begin();

///	// 初期モード　→ GameModeBase::reserveDefaultChildModeDive で自動的に推移するように変更【2015/02/10 r-kishi】
///	changeChildMode((s16)GameMode::MODE_GAME_INGAME_CONFRONT_NORMAL, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ConfrontMode::notifyEnd(void* objParamRef)
{
	TRACE("{ConfrontMode::notifyEnd} called.\n");

	// カメラを戻す
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(	(sgm->getNodeFromId(_camidBk) != 0L) &&		// カメラがまだ存在するときのみ（destroy 中など、無い場合もある）
		(sgm->getVisibleCamera() == _camCf)	)		// かつ、削除しようとしているカメラが有効のとき
	{
		CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
		nodecCam->switchVisible(_camidBk);	// カメラを戻す
	}
	sgm->removeNode(_camCf);
}

/*---------------------------------------------------------------------*//**
	初期子モードを得る
**//*---------------------------------------------------------------------*/
short ConfrontMode::getDefaultChildMode()
{
	return (s16)GameMode::MODE_GAME_INGAME_CONFRONT_NORMAL;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void ConfrontMode::exec(ExecRes* res, const ExecCtx* ec)
{
	Mode::exec(res, ec);

	// 対決モジュールへ制御をまわす
	_confrontRef->exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ConfrontMode::render(const RenderCtx* rc)
{
	Mode::render(rc);
}

/*---------------------------------------------------------------------*//**
	ソウマ発射通知
**//*---------------------------------------------------------------------*/
void ConfrontMode::notifyDischargeSouma(Souma* souma, u16 mcid)
{
	GameModeBase::notifyDischargeSouma(souma, mcid);

	if(_camCf != 0L)
	{
		// マジッククラスタ及び伝達ステータスを得る
		const MagicCluster* mc = 0L;
		const TransStat* tstat = 0L;
		{
			MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
			if(mgcsys != 0L)
			{
				mc = mgcsys->getCluster(mcid);
				if(mc != 0L)
				{
					tstat = mc->getTransStat();
				}
			}
		}

		do
		{
			if((mc == 0L) || (tstat == 0L))
			{
				break;
			}
			u32 ffform = tstat->getFuncFlags() & EleneStat::FF_MASK_FORM;

			// 全体効果設定
			if(((ffform == EleneStat::FF_FORM_FIRE) || (ffform == EleneStat::FF_FORM_WIND)) && ((::rand() & 5) > 0))
			{
				_camCf->setMode(ConfrontCam::MODE_WHOLE, souma->getOwnerUnit(), 0L, 0L, 120.0f);
				_camCf->startPerformance(ConfrontCam::PERFF_LEAVE, 0.0f, 200.0f);
				break;
			}

			// たまにカメラを変えない
			if((::rand() & 3) == 0)
			{
				break;
			}

			// フォーカスに自分を含むか
			s32 numFocus = mc->focusArray()->getCount();
			bool isIncludeSelf = false;
			const Unit* unit2 = 0L;
			const Unit* unit3 = 0L;
			for(int i = 0; i < numFocus; i++)
			{
				const Unit* unitWk = mc->focusArray()->getUnit(i);
				if(souma->getOwnerUnit() == unitWk)
				{
					isIncludeSelf = true;
				}
				else
				{
					if(unit2 == 0L)			{	unit2 = unitWk;	}
					else if(unit3 == 0L)	{	unit3 = unitWk;	}
				}
			}

#if 0
			// 自分から自分へ
			if((numFocus == 1) && isIncludeSelf)
			{
				_camCf->setMode(ConfrontCam::MODE_GAZE, souma->getOwnerUnit(), 0L, 0L, 30.0f);
				break;
			}

			// 自分から相手へ
			if((numFocus == 1) || ((numFocus == 2) && isIncludeSelf))
			{
				_camCf->setMode(ConfrontCam::MODE_TWO, souma->getOwnerUnit(), unit2, 0L, 30.0f);
				_camCf->startPerformance(ConfrontCam::PERFF_LEAVE, 0.0f, 50.0f);
				break;
			}

			// 自分から二者へ
			if((numFocus == 2) || ((numFocus == 3) && isIncludeSelf))
			{
				_camCf->setMode(ConfrontCam::MODE_THREE, souma->getOwnerUnit(), unit2, unit3, 30.0f);
				_camCf->startPerformance(ConfrontCam::PERFF_LEAVE, 0.0f, 50.0f);
				break;
			}
#endif

			// 全体俯瞰
			_camCf->setMode(ConfrontCam::MODE_WHOLE, souma->getOwnerUnit(), 0L, 0L, 30.0f);
			_camCf->startPerformance(ConfrontCam::PERFF_LEAVE, 0.0f, 50.0f);
		}
		while(false);
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void ConfrontMode::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	GameModeBase::notifyUnitInfluenceStat(unitRecv, scres, tstat);

	if(_camCf != 0L)
	{
		// 残り一体が活動停止したらランダムでカメラモードチェンジ
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_DOWN) && _confrontRef->isAllTargetDown())
		{
			if(_camCf->getCurrentMode() == ConfrontCam::MODE_DEFAULT)	// デフォルトカメラ時
			{
				switch(::rand() & 5)
				{
				case 0:
				case 2:
				case 3:
					_camCf->setMode(ConfrontCam::MODE_TWO, tstat->getOwnerUnit(), unitRecv, 0L, 60.0f);
					_camCf->startPerformance(ConfrontCam::PERFF_LEAVE, 0.0f, 70.0f);
					break;
				case 1:
					_camCf->setMode(ConfrontCam::MODE_GAZE, tstat->getOwnerUnit(), 0L, 0L, 60.0f);
					_camCf->startPerformance(ConfrontCam::PERFF_LEAVE, 0.0f, 70.0f);
					break;
				default:
					// 何もしない
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
