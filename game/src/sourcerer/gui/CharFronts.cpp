/***********************************************************************//**
 *	CharFronts.cpp
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
#include "CharFronts.h"

// Friends
#include "FocusMenu.h"
#include "FocusCursor.h"
#include "GameGui.h"
#include "MoveCursor.h"
#include "MoveCursorExtension.h"
#include "StatGauge.h"
#include "../episode/char/pc/MyUnit.h"
#include "../episode/party/MyParty.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameRenderCtx.h"
#include "../item/Item.h"
#include "../item/ItemEquip.h"
#include "../save/SaveData.h"
#include "../save/SaveSys.h"
#include "../status/ActStat.h"
#include "../status/CharStat.h"
#include "../status/StatusDrawer.h"
#include "../spirit/Spirit.h"

// External

// Library
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gui/Panel.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 配置場所
#define W_FACE				(32)
#define H_FACE				(32)
#define Y_CK_PNL			20
#define RECT_PC1_PNL		10, Y_CK_PNL, W_FACE, H_FACE
#define RECT_PC2_PNL		140, Y_CK_PNL, W_FACE, H_FACE
#define RECT_PC3_PNL		270, Y_CK_PNL, W_FACE, H_FACE
#define MARGIN_HIT_H		(8)
#define MARGIN_HIT_V		(16)
#define W_LEADER_MARK		(8)
#define H_LEADER_MARK		(8)

#define W_GAUGE				(80)
#define H_GAUGET			(24)
#define H_CHAREESTAT_MIN	(22)//(18)
#define H_CHAREESTAT_MAX	(36)
#define PAD_GAUGE_X			(2)

#define Y_PC2_WEP_MARGIN		1
#define Y_PC2_WEP_MIN			Y_CK_PNL + H_GAUGET + H_CHAREESTAT_MIN + Y_PC2_WEP_MARGIN
#define Y_PC2_WEP_MAX			Y_CK_PNL + H_GAUGET + H_CHAREESTAT_MAX + Y_PC2_WEP_MARGIN
#define RECT_PC2_WEP_DEFAULT	152, Y_PC2_WEP_MAX, 12, 12

#define W_ITEM_ICON			(12)
#define H_ITEM_ICON			(12)
#define H_ITEMEESTAT_MIN	(15)
#define H_ITEMEESTAT_MAX	(33)//(30)

#define X_TRG				(10)
#define X_TRG_STAT			(44)
#define Y_TRG_MARGIN		(16)
#define Y_TRG				(Y_PC2_WEP_MIN + Y_TRG_MARGIN)
#define W_TRG_ICON			(W_FACE)
#define H_TRG_ICON			(H_FACE)
#define Y_TRG_BOTTOM		(Game::getGame()->getLogicalHeight() - Y_CK_PNL - H_GAUGET - H_CHAREESTAT_MAX)	// test

// テクスチャ UV 値
#define W_TEX				(1024.0f)
#define H_TEX				(1024.0f)
#define UV_PC1_N_L			(800.0f)
#define UV_PC1_N_T			(960.0f)
#define UV_PC1_N_W			(32.0f)
#define UV_PC1_N_H			(32.0f)
#define UV_PC1_H_L			(800.0f)
#define UV_PC1_H_T			(960.0f)
#define UV_PC1_H_W			(32.0f)
#define UV_PC1_H_H			(32.0f)
#define UV_PC2_N_L			(832.0f)
#define UV_PC2_N_T			(960.0f)
#define UV_PC2_N_W			(32.0f)
#define UV_PC2_N_H			(32.0f)
#define UV_PC2_H_L			(832.0f)
#define UV_PC2_H_T			(960.0f)
#define UV_PC2_H_W			(32.0f)
#define UV_PC2_H_H			(32.0f)
#define UV_PC3_N_L			(864.0f)
#define UV_PC3_N_T			(960.0f)
#define UV_PC3_N_W			(32.0f)
#define UV_PC3_N_H			(32.0f)
#define UV_PC3_H_L			(864.0f)
#define UV_PC3_H_T			(960.0f)
#define UV_PC3_H_W			(32.0f)
#define UV_PC3_H_H			(32.0f)
#define UV_LEADER_MARK				880.0f, 944.0f, 8.0f, 8.0f
#define UV_TARGET_ICON				800.0f, 992.0f, 32.0f, 32.0f
#define UV_ITEM_ICON_GRASS_S		 976.0f,  0.0f, 24.0f, 24.0f
#define UV_ITEM_ICON_STONE_S		1000.0f,  0.0f, 24.0f, 24.0f
#define UV_ITEM_ICON_WEAPON_S		 928.0f, 24.0f, 24.0f, 24.0f
#define UV_ITEM_ICON_SHIELD_S		 952.0f, 24.0f, 24.0f, 24.0f

#define UV_WHITE_DOT				640.0f, 992.0f, 16.0f, 16.0f

// 対戦相手ポインタがクリアされるフレーム数
#define DFRAME_TRG_CLEAR	(3.0f * FRAMERATE)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharFronts メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	外部有効かどうかを設定する
**//*---------------------------------------------------------------------*/
void CharFronts::setEnable(bool isEnable)
{
	/*
	if(!_isEnableOut && isEnable)
	{
		// キャラクタステータスを取得する
		_cstat[PC_1] = Game::getGame()->getSaveSys()->getCharStat(SaveSys::CHAR_PC01);
		_cstat[PC_2] = Game::getGame()->getSaveSys()->getCharStat(SaveSys::CHAR_PC02);
		_cstat[PC_3] = Game::getGame()->getSaveSys()->getCharStat(SaveSys::CHAR_PC03);
		MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyCharUnit();					ASSERT(unitMy != 0L);
		Unit* unitPt = (Unit*)unitMy->getFrientCharUnit(MyUnit::FRIEND_PARTNER);	ASSERT(unitPt != 0L);
		Unit* unitHp = (Unit*)unitMy->getFrientCharUnit(MyUnit::FRIEND_HELPER);		ASSERT(unitHp != 0L);
		_astat[PC_1] = unitMy->getSpirit()->getActStat();
		_astat[PC_2] = unitPt->getSpirit()->getActStat();
		_astat[PC_3] = unitHp->getSpirit()->getActStat();
	}
	*/

	_isEnableOut = isEnable;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	PC のステータス表示全体の矩形を得る
**//*---------------------------------------------------------------------*/
bool CharFronts::getPcStatRect(RectF32* rectOut, s32 ipc) const
{
	if((MyParty::PC_MY <= ipc) && (ipc < MyParty::NUM_PC))
	{
		const RectF32* rectPnl = _ci[ipc]._pnlMain->getRectangle();
		rectOut->set(rectPnl->x(), rectPnl->y(), rectPnl->w() + PAD_GAUGE_X + StatusDrawer::W_GAUGE_DEFAULT, _ci[ipc]._hMain);
		return true;
	}
	return false;
}

#if 0	// GaugeAnimEffect によるダメージゲージアニメーションは StatGauge に任せるように変更した	【2015/02/24 r-kishi】
/*---------------------------------------------------------------------*//**
	PC のゲージ位置を得る
**//*---------------------------------------------------------------------*/
bool CharFronts::getPcGaugePos(PointF32* ptOut, s32 ipc) const
{
	if((MyParty::PC_MY <= ipc) && (ipc < MyParty::NUM_PC))
	{
		const RectF32* rectPnl = _ci[ipc]._pnlMain->getRectangle();
		ptOut->set(rectPnl->x() + rectPnl->w() + PAD_GAUGE_X, rectPnl->y() + StatusDrawer::H_NAME_DEFAULT);
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	PC のゲージ位置を得る
**//*---------------------------------------------------------------------*/
bool CharFronts::getTargetGaugePos(PointF32* ptOut, const Unit* unitTrg) const
{
	CharUnit* cunitTrg = getTargetUnit();
	if((cunitTrg != 0L) && ((cunitTrg == unitTrg) || (unitTrg == 0L)))
	{
		const CharStat* cstatTrg = cunitTrg->getCharStat();
		if(cstatTrg != 0L)
		{
			ptOut->set(X_TRG_STAT, (_ci[0]._pnlMain != 0L) ? _ci[0]._pnlMain->getRectangle()->y() + _ci[0]._hMain + Y_TRG_MARGIN + StatusDrawer::H_NAME_DEFAULT : Y_TRG + StatusDrawer::H_NAME_DEFAULT);
			return true;
		}
	}

	return false;
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CharFronts::CharFronts()
	: _isEnableOut(true)
	, _isEnableSelf(false)
	, _cstatTrgRef(0L)
	, _dfcntTrgClearDelay(0.0f)
	, _uvwhLeaderMark(0L)
	, _uvwhTargetIcon(0L)
	, _uvwhWhiteDot(0L)
	, _fccsrRef(0L)
	, _fcmenuRef(0L)
	, _texGuiRef(0L)
{
	for(int i = 0; i < NUM_PC; i++)
	{
		_ci[i]._pnlMain = 0L;
		_ci[i]._pnlSub = 0L;
		_ci[i]._cunit = 0L;
		_ci[i]._sprt = 0L;
	}
	for(int i = 0; i < NUM_CGG; i++)
	{
		_sg[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CharFronts::~CharFronts()
{
	ASSERT(_uvwhLeaderMark == 0L);
	ASSERT(_uvwhTargetIcon == 0L);
	ASSERT(_uvwhWhiteDot == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_PC; i++)
		{
			ASSERT(_ci[i]._pnlMain == 0L);
			ASSERT(_ci[i]._pnlSub == 0L);
			ASSERT(_ci[i]._cunit == 0L);
			ASSERT(_ci[i]._sprt == 0L);
		}
		for(int i = 0; i < NUM_CGG; i++)
		{
			ASSERT(_sg[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool CharFronts::create(Texture* texGuiRef, FocusCursor* fccsrRef, FocusMenu* fcmenuRef)
{
	RectF32 rect;
	RectF32 uv;
	ColorU8 col;

	// パネルの作成
	_ci[PC_1]._pnlMain = new Panel();
	if(!_ci[PC_1]._pnlMain->create(RectF32::setout(&rect, RECT_PC1_PNL))) { ASSERT(false);	return false; }
	_ci[PC_1]._pnlMain->setStateInfo(Panel::STATE_NORMAL, texGuiRef, Gcalc::calcTexUv(&uv, UV_PC1_N_L, UV_PC1_N_T, UV_PC1_N_W, UV_PC1_N_H, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 211));
	_ci[PC_1]._pnlMain->setStateInfo(Panel::STATE_HOVER, texGuiRef, Gcalc::calcTexUv(&uv, UV_PC1_H_L, UV_PC1_H_T, UV_PC1_H_W, UV_PC1_H_H, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 255));
	_ci[PC_1]._pnlMain->setEnable(false);
	_ci[PC_1]._pnlMain->setHorizontalHitMargin(MARGIN_HIT_H);
	_ci[PC_1]._pnlMain->setVerticalHitMargin(MARGIN_HIT_V);
	_ci[PC_1]._hMain = H_GAUGET + H_CHAREESTAT_MAX;
	_ci[PC_2]._pnlMain = new Panel();
	if(!_ci[PC_2]._pnlMain->create(RectF32::setout(&rect, RECT_PC2_PNL))) { ASSERT(false);	return false; }
	_ci[PC_2]._pnlMain->setStateInfo(Panel::STATE_NORMAL, texGuiRef, Gcalc::calcTexUv(&uv, UV_PC2_N_L, UV_PC2_N_T, UV_PC2_N_W, UV_PC2_N_H, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 211));
	_ci[PC_2]._pnlMain->setStateInfo(Panel::STATE_HOVER, texGuiRef, Gcalc::calcTexUv(&uv, UV_PC2_H_L, UV_PC2_H_T, UV_PC2_H_W, UV_PC2_H_H, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 255));
	_ci[PC_2]._pnlMain->setEnable(false);
	_ci[PC_2]._pnlMain->setHorizontalHitMargin(MARGIN_HIT_H);
	_ci[PC_2]._pnlMain->setVerticalHitMargin(MARGIN_HIT_V);
	_ci[PC_2]._hMain = H_GAUGET + H_CHAREESTAT_MAX;
	_ci[PC_2]._pnlSub = new Panel();
	if(!_ci[PC_2]._pnlSub->create(RectF32::setout(&rect, RECT_PC2_WEP_DEFAULT))) { ASSERT(false);	return false; }
	_ci[PC_2]._pnlSub->setStateInfo(Panel::STATE_NORMAL, texGuiRef, Gcalc::calcTexUv(&uv, UV_ITEM_ICON_WEAPON_S, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 211));
	_ci[PC_2]._pnlSub->setEnable(false);
	_ci[PC_2]._pnlSub->setHorizontalHitMargin(MARGIN_HIT_H);
	_ci[PC_2]._pnlSub->setVerticalHitMargin(MARGIN_HIT_V);
	_ci[PC_2]._hSub = H_ITEMEESTAT_MAX;
	_ci[PC_3]._pnlMain = new Panel();
	if(!_ci[PC_3]._pnlMain->create(RectF32::setout(&rect, RECT_PC3_PNL))) { ASSERT(false);	return false; }
	_ci[PC_3]._pnlMain->setStateInfo(Panel::STATE_NORMAL, texGuiRef, Gcalc::calcTexUv(&uv, UV_PC3_N_L, UV_PC3_N_T, UV_PC3_N_W, UV_PC3_N_H, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 211));
	_ci[PC_3]._pnlMain->setStateInfo(Panel::STATE_HOVER, texGuiRef, Gcalc::calcTexUv(&uv, UV_PC3_H_L, UV_PC3_H_T, UV_PC3_H_W, UV_PC3_H_H, W_TEX, H_TEX, 1.0f), ColorU8::setout(&col, 255, 255, 255, 255));
	_ci[PC_3]._pnlMain->setEnable(false);
	_ci[PC_3]._pnlMain->setHorizontalHitMargin(MARGIN_HIT_H);
	_ci[PC_3]._pnlMain->setVerticalHitMargin(MARGIN_HIT_V);
	_ci[PC_3]._hMain = H_GAUGET + H_CHAREESTAT_MAX;

	// リーダーマーク UV の作成
	_uvwhLeaderMark = new RectF32();
	Gcalc::calcTexUv(_uvwhLeaderMark, UV_LEADER_MARK, W_TEX, H_TEX, 1.0f);
	// 対象マーク UV の作成
	_uvwhTargetIcon = new RectF32();
	Gcalc::calcTexUv(_uvwhTargetIcon, UV_TARGET_ICON, W_TEX, H_TEX, 1.0f);
	// 行動中白ドット UV の作成
	_uvwhWhiteDot = new RectF32();
	Gcalc::calcTexUv(_uvwhWhiteDot, UV_WHITE_DOT, W_TEX, H_TEX, 1.0f);
	
	// ステータスゲージの作成
	for(int i = 0; i < NUM_CGG; i++)
	{
		_sg[i] = new StatGauge();
		if(i < NUM_PC)
		{
			const RectF32* rectPnl = _ci[i]._pnlMain->getRectangle();
			rect.set(rectPnl->x() + rectPnl->w() + PAD_GAUGE_X, rectPnl->y(), StatusDrawer::W_GAUGE_DEFAULT, _ci[i]._hMain);
		}
		else
		{
			rect.set(X_TRG_STAT, Y_TRG, StatusDrawer::W_GAUGE_DEFAULT, H_GAUGET + H_CHAREESTAT_MAX);
		}
		if(!_sg[i]->create(texGuiRef, &rect))
		{
			ASSERT(false);	return false;
		}
	}

	// 初期化
	_isEnableSelf = true;
	_fccsrRef = fccsrRef;
	_fcmenuRef = fcmenuRef;
	_texGuiRef = texGuiRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void CharFronts::destroy()
{
	_isEnableSelf = false;

	// ステータスゲージの削除
	for(int i = 0; i < NUM_CGG; i++)
	{
		if(_sg[i] != 0L)
		{
			_sg[i]->destroy();
			delete _sg[i];
			_sg[i] = 0L;
		}
	}

	// 対象マーク UV の削除
	delete _uvwhTargetIcon;
	_uvwhTargetIcon = 0L;
	// リーダーマーク UV の削除
	delete _uvwhLeaderMark;
	_uvwhLeaderMark = 0L;
	// 行動中白ドット UV の削除
	delete _uvwhWhiteDot;
	_uvwhWhiteDot = 0L;

	// パネルの削除
	for(int i = 0; i < NUM_PC; i++)
	{
		if(_ci[i]._pnlMain != 0L)
		{
			_ci[i]._pnlMain->destroy();
			delete _ci[i]._pnlMain;
			_ci[i]._pnlMain = 0L;
		}
		if(_ci[i]._pnlSub != 0L)
		{
			_ci[i]._pnlSub->destroy();
			delete _ci[i]._pnlSub;
			_ci[i]._pnlSub = 0L;
		}
		_ci[i]._cunit = 0L;
		_ci[i]._sprt = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void CharFronts::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isEnableOut)	{	return;	}	// 外部からの無効化

	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 

	// 有効性判定
	_isEnableSelf = !gec->isLockOperating();
	bool isEnableUiReact = _isEnableSelf && !gec->isOpenedLockGui() && !ui->isReacted();

	if(_isEnableSelf)
	{
		// キャラクタポインターを取得する
		acquireCharPtr();

		// アニメーション
		for(int i = 0; i < NUM_PC; i++)
		{
			if(_ci[i]._cunit == 0L) { continue; }
			const EleneStat* eestat = _ci[i]._cunit->getCharStat()->getEleneStat();
			bool close = eestat->getOql()->equals(eestat->getCql()) && eestat->isCleardVeil();
			f32 hTrg = close ? (f32)(H_GAUGET + H_CHAREESTAT_MIN) : (f32)(H_GAUGET + H_CHAREESTAT_MAX);
			if(_ci[i]._hMain != hTrg)
			{
				f32 rate = ec->getDeltaFrame() * 0.1f;
				if(rate > 1.0f) { rate = 1.0f; }
				_ci[i]._hMain = (_ci[i]._hMain * (1.0f - rate)) + (hTrg * rate);
				if(TFW_ABS(_ci[i]._hMain - hTrg) < 0.2f) { _ci[i]._hMain = hTrg; }
				_sg[i]->rectangle()->h() = _ci[i]._hMain;
			}

			// 武器パネルの位置も同時に調整する
			if(_ci[i]._pnlSub != 0L)
			{
				RectF32 rect = *_ci[i]._pnlSub->getRectangle();
				rect.top() = Y_CK_PNL + _ci[i]._hMain + Y_PC2_WEP_MARGIN;
				_ci[i]._pnlSub->setRectangle(&rect);

				eestat = _ci[i]._item->getEleneStat();
				close = eestat->getOql()->equals(eestat->getCql()) && eestat->isCleardVeil();
				hTrg = close ? (f32)(H_ITEMEESTAT_MIN) : (f32)(H_ITEMEESTAT_MAX);
				if(_ci[i]._hSub != hTrg)
				{
					f32 rate = ec->getDeltaFrame() * 0.1f;
					if(rate > 1.0f) { rate = 1.0f; }
					_ci[i]._hSub = (_ci[i]._hSub * (1.0f - rate)) + (hTrg * rate);
					if(TFW_ABS(_ci[i]._hSub - hTrg) < 0.2f) { _ci[i]._hSub = hTrg; }
				}
			}
		}
		if(_sg[CGG_TRG]->getAttachedCharStat() != 0L)
		{
			f32 yTrg = _ci[0]._pnlMain->getRectangle()->y() + _ci[0]._hMain + Y_TRG_MARGIN;
			if(yTrg != _sg[CGG_TRG]->getRectangle()->y())
			{
				_sg[CGG_TRG]->rectangle()->y() = yTrg;
			}
		}

		// 対戦相手ポインタ解除遅延処理
		if(_dfcntTrgClearDelay > 0.0f)
		{
			_dfcntTrgClearDelay -= gec->getDeltaFrame();
			if(_dfcntTrgClearDelay <= 0.0f)
			{
				_cstatTrgRef = 0L;
				_sg[CGG_TRG]->attachCharStat(_cstatTrgRef);
				_dfcntTrgClearDelay = 0.0f;
			}
		}

		// ゲージ制御
		for(int i = 0; i < NUM_CGG; i++)
		{
			_sg[i]->exec(res, ec);
		}

		if(isEnableUiReact)
		{
			// アクティブ表示に変更
			PointF32 ptNow(F32_PMAX, F32_PMAX);
			PointF32 ptStart(F32_PMAX, F32_PMAX);
			bool isActive = (ui->getTouchingNum() == 1) && !ui->isReacted();	// シングル
			ui->getTouchPosition(&ptNow);
			for(int i = 0; i < NUM_PC; i++)
			{
				_ci[i]._pnlMain->hit(&ptNow, isActive);
			}

			// 操作
			bool isTap = false;
			bool isDblTapRel = false;
			if(!ui->isReacted())
			{
				const f32 DIFF_TAP_RANGE = 20.0f;
				if(ui->isTapRelease(&ptNow, &ptStart)) { isTap = ptNow.isNear(&ptStart, DIFF_TAP_RANGE, DIFF_TAP_RANGE); }	// 面積が狭いので、isClickRelease だとストレスに感じる
				if(ui->isDoubleClickRelease(&ptNow)) { isDblTapRel = true; }
			}
			if(isTap || isDblTapRel)
			{
				for(int i = 0; i < NUM_PC; i++)
				{
					if(_ci[i]._pnlMain->hitTest(&ptNow) || _ci[i]._pnlMain->hitTest(&ptStart))
					{
						if(isDblTapRel)	// ダブルクリックの場合
						{
							// ユニットを特定する
							if(_ci[i]._cunit != 0L)
							{
								// フォーカスを設定する
								_fccsrRef->setFocus(0, _ci[i]._cunit);
								ui->setReacted(true);	// UI に反応済みフラグを立てる
							}
						}
						else if(isTap)	// タップの場合
						{
							showFocusMenu(i, true);	// フォーカスメニューを開く
							ui->setReacted(true);	// UI に反応済みフラグを立てる
						}
					}
					if((_ci[i]._pnlSub != 0L) && (_ci[i]._pnlSub->hitTest(&ptNow) || _ci[i]._pnlSub->hitTest(&ptStart)))
					{
						if(isDblTapRel)	// ダブルクリックの場合
						{
							// 現状では右手装備のみ対応
							if(_ci[i]._wunit != 0L)
							{
								_fccsrRef->setFocus(0, _ci[i]._wunit);	// フォーカスを設定する
								ui->setReacted(true);	// UI に反応済みフラグを立てる
							}
						}
						else if(isTap)	// タップの場合
						{
							showFocusMenu(i, false);	// フォーカスメニューを開く
							ui->setReacted(true);		// UI に反応済みフラグを立てる
						}
					}
				}
			}

			// パネルのフレーム処理
			for(int i = 0; i < NUM_PC; i++)
			{
				if(_ci[i]._cunit != 0L)
				{
					_ci[i]._pnlMain->setEnable(_ci[i]._cunit->isEnable());
					if(_ci[i]._pnlSub != 0L) { _ci[i]._pnlSub->setEnable(_ci[i]._cunit->isEnable()); }
				}
				_ci[i]._pnlMain->exec(res, ec);
				if(_ci[i]._pnlSub != 0L) { _ci[i]._pnlSub->exec(res, ec); }
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void CharFronts::draw(const RenderCtx* rc)
{
	if(!_isEnableOut || !_isEnableSelf)	{	return;	}	// 無効化中

	Renderer* rdr = rc->getRenderer();
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	const MoveCursorExtension* mcex = mvcsr->getExtension(); ASSERT(mcex != 0L);
	RectF32 rect;
	
	// パーティのステータス表示
	rdr->setEnableTexture(true);	// テクスチャを有効に
	rdr->setSolidColor(255, 255, 255, 255);
	for(int i = 0; i < NUM_PC; i++)
	{
		if(_ci[i]._cunit == 0L)			{ continue; }
		if(!_ci[i]._cunit->isEnable())	{ continue; }
		const RectF32* rectPnl = _ci[i]._pnlMain->getRectangle();

		// パネルの描画
		_ci[i]._pnlMain->draw(rc);

		// リーダーマーク
		if(TFW_IS_FLAG(_ci[i]._cunit->getUnitFlags(), Unit::UNITFLAG_PC) && ((const PcUnit*)_ci[i]._cunit)->isLeader())
		{
			rdr->setSolidColor(255, 255, 255, 255);
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, rectPnl->x(), rectPnl->y(), W_LEADER_MARK, H_LEADER_MARK), _uvwhLeaderMark);
		}

		// 行動中マーク
		drawBehaviorActing(rc, _ci[i]._sprt, rectPnl->x() + W_FACE / 2, rectPnl->y() + H_FACE / 2 + 5.0f);

		// ゲージ描画
//		f32 xGauge = (f32)(rectPnl->x() + rectPnl->w() + PAD_GAUGE_X);
//		StatusDrawer::drawCharStat(rc, _texGuiRef, _ci[i]._sprt->getCharStat(), xGauge, (f32)rectPnl->y(), StatusDrawer::W_GAUGE_DEFAULT, _ci[i]._hMain, true, true);

		// 装備アイテムの元素エネルギー表示
		if((_ci[i]._pnlSub != 0L) && (_ci[i]._item != 0L))
		{
			_ci[i]._pnlSub->draw(rc);
//			StatusDrawer::drawEleneStat(rc, _texGuiRef, _ci[i]._item->getEleneStat(), xGauge, (f32)_ci[i]._pnlSub->getRectangle()->y(), StatusDrawer::W_GAUGE_DEFAULT, _ci[i]._hSub, true);
			StatusDrawer::drawEleneStat(rc, _texGuiRef, _ci[i]._item->getEleneStat(), _sg[i]->getRectangle()->x(), (f32)_ci[i]._pnlSub->getRectangle()->y(), StatusDrawer::W_GAUGE_DEFAULT, _ci[i]._hSub, true);
		}
	}

	// 対決相手を表示
	if(_cstatTrgRef != 0L)
	{
		rdr->setEnableTexture(true);	// テクスチャを有効に
		rdr->setSolidColor(255, 255, 255, 255);
		f32 y = (_ci[0]._pnlMain != 0L) ? _ci[0]._pnlMain->getRectangle()->y() + _ci[0]._hMain + Y_TRG_MARGIN : Y_TRG;	//Y_TRG_BOTTOM // test
		RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, X_TRG, y, W_TRG_ICON, H_TRG_ICON), _uvwhTargetIcon);
//		StatusDrawer::drawCharStat(rc, _texGuiRef, cstatTrg, X_TRG_STAT, y, StatusDrawer::W_GAUGE_DEFAULT, H_GAUGET + H_CHAREESTAT_MAX, true, true);

		// 行動中マーク
		drawBehaviorActing(rc, _cstatTrgRef->getOwnerUnit()->getSpirit(), X_TRG + W_FACE / 2, y + H_FACE / 2 + 5.0f);
	}

	// ゲージ描画
	for(int i = 0; i < NUM_CGG; i++)
	{
		_sg[i]->draw(rc);
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void CharFronts::notifyChangeMapBefore()
{
	// 対戦相手のポインタをクリアする
	_cstatTrgRef = 0L;
	_sg[CGG_TRG]->attachCharStat(_cstatTrgRef);
	_dfcntTrgClearDelay = 0.0f;
}

/*---------------------------------------------------------------------*//**
	キャラクターへのポインタを得る
**//*---------------------------------------------------------------------*/
void CharFronts::acquireCharPtr()
{
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);

	// パーティキャラクタのポインタを得る
	const MyParty::PC pcs[] = { MyParty::PC_MY, MyParty::PC_PTN, MyParty::PC_HLP };
	for(int i = 0; i < NUM_PC; i++)
	{
		if(_ci[i]._cunit == 0L)
		{
			CharUnit* unit = party->pcUnit(pcs[i]);
			if(unit != 0L)
			{
				_ci[i]._cunit = unit;
				_ci[i]._sprt = unit->getSpirit();
			}
		}
		if(_ci[i]._cunit != 0L)
		{
			const CharStat* cstat = _ci[i]._cunit->isEnable() ? _ci[i]._cunit->getCharStat() : 0L;
			if(_sg[i]->getAttachedCharStat() != cstat)
			{
				_sg[i]->attachCharStat(cstat);
			}
		}
	}

	// アイテム装備はアドレス移動があるので毎回取得する
	if(_ci[PC_2]._cunit != 0L)
	{
		const ItemEquip* itmeq = ((PcUnit*)_ci[PC_2]._cunit)->getItemEquip();
		int ieq = EquipPlace::EQP_RHAND;	// 今のところ、PC2 の右手武器のみ対応
		_ci[PC_2]._item = itmeq->getEquipStat()->getEquipItem(ieq);
		_ci[PC_2]._wunit = itmeq->getEquipUnit(ieq);
		//ASSERT(_ci[PC_2]._wunit->getUnitType() == UnitType::UNITTYPE_EQUIPMENT_ITEM);
	}

	// 対戦相手のポインタを得る
	const CharStat* cstatTrgNew = 0L;
	CharUnit* cunitTrg = getTargetUnit();
	if(cunitTrg != 0L)
	{
		cstatTrgNew = cunitTrg->getCharStat();
	}
	if(_sg[CGG_TRG]->getAttachedCharStat() != cstatTrgNew)
	{
		if(cunitTrg == 0L)
		{
			if(_dfcntTrgClearDelay == 0.0f)
			{
				_dfcntTrgClearDelay = DFRAME_TRG_CLEAR;
			}
		}
		else
		{
			_cstatTrgRef = cstatTrgNew;
			_sg[CGG_TRG]->attachCharStat(_cstatTrgRef);
			_dfcntTrgClearDelay = 0.0f;
		}
	}
}

/*---------------------------------------------------------------------*//**
	フォーカスメニューを開く
**//*---------------------------------------------------------------------*/
void CharFronts::showFocusMenu(s32 imchar, bool isChar)
{
	if(_fcmenuRef == 0L)	{	return;	}
	if(_fccsrRef == 0L)		{	return;	}

	const s32 MTNODEID_PC1_FOCUS		= 11001;
	const s32 MTNODEID_PC1_LEADERS		= 11002;
	const s32 MTNODEID_PC2_FOCUS		= 12001;
	const s32 MTNODEID_PC2_LEADERS		= 12002;
	const s32 MTNODEID_PC2_WEAPON_FOCUS	= 12006;
	const s32 MTNODEID_PC3_FOCUS		= 13001;
	const s32 MTNODEID_PC3_LEADERS		= 13002;

	FocusMenu::MenuKind mkind = FocusMenu::NUM_MENU;
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	Unit* unit = 0L;

	// キャラクタポインターを取得する
	acquireCharPtr();

	// メニュー種別と準備
	switch(imchar)
	{
	case PC_1:

		mkind = FocusMenu::MENU_PC1;
		unit = _ci[PC_1]._cunit;

		// フォーカス
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC1_FOCUS, _fccsrRef->getFocus(0) != unit);

		// リーダー変更
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC1_LEADERS, party->isValidLeader(MyParty::PC_MY) && (unit != party->getLeaderPcUnit()));

		break;
	case PC_2:

		if(isChar)
		{
			mkind = FocusMenu::MENU_PC2;
			unit = _ci[PC_2]._cunit;

			// フォーカス
			_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC2_FOCUS, _fccsrRef->getFocus(0) != unit);

			// リーダー変更
			_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC2_LEADERS, party->isValidLeader(MyParty::PC_PTN) && (unit != party->getLeaderPcUnit()));

			// 装備品のフォーカス
			if(_ci[PC_2]._wunit != 0L)
			{
				_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC2_WEAPON_FOCUS, _fccsrRef->getFocus(0) != _ci[PC_2]._wunit);
			}
		}
		else
		{
			mkind = FocusMenu::MENU_PC2_WEP;
			unit = _ci[PC_2]._wunit;
			ASSERT(unit->getUnitType() == UNITTYPE_EQUIPMENT_ITEM);
		}

		break;
	case PC_3:

		mkind = FocusMenu::MENU_PC3;
		unit = _ci[PC_3]._cunit;

		// フォーカス
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC3_FOCUS, _fccsrRef->getFocus(0) != unit);

		// リーダー変更
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_PC3_LEADERS, party->isValidLeader(MyParty::PC_HLP) && (unit != party->getLeaderPcUnit()));

		break;
	}

	// メニュー開く
	if(mkind != FocusMenu::NUM_MENU)
	{
		PointF32 ptCenter;
		if(isChar) { ptCenter = _ci[imchar]._pnlMain->getRectangle()->center(); }
		else { ptCenter = _ci[imchar]._pnlSub->getRectangle()->center(); }
		_fcmenuRef->showMenu(mkind, &ptCenter, unit, false);
	}
}

/*---------------------------------------------------------------------*//**
	対決相手のユニットを得る
**//*---------------------------------------------------------------------*/
CharUnit* CharFronts::getTargetUnit() const
{
	if(_ci[PC_1]._sprt != 0L)
	{
		const Unit* unitTrg = _ci[PC_1]._sprt->getFocusUnit();
		if((unitTrg == 0L) || (unitTrg->getUnitType() != UNITTYPE_ENEMY_CHAR) || !unitTrg->isEnable())
		{
			unitTrg = _ci[PC_1]._sprt->getConfrontUnit();
		}

		if((unitTrg != 0L) && unitTrg->isCategory(Unit::UNITCAT_CHAR) && unitTrg->isEnable())
		{
			return (CharUnit*)unitTrg;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	対決相手のユニットを得る
**//*---------------------------------------------------------------------*/
void CharFronts::drawBehaviorActing(const RenderCtx* rc, const Spirit* sprt, f32 x, f32 y)
{
	if(!sprt->isBehaviorActing())
	{
		return;
	}

	if(sprt->getBefaviorMaxFrame() == 0.0f)	// 終了時間のわからない行動
	{
		s32 tick = ((s32)sprt->getBefaviorCurrentFrame() >> 4) & 0x3;
		rc->getRenderer()->setSolidColor(255, 255, 255, 159);
#if 1
		if(tick >= 1)
		{
			RectF32 vtx;
			rc->getRenderer()->bindTexture(Game::getGame()->getGui()->getGuiTexture());
			RendererUtils::draw2dTextureRect(rc->getRenderer(), RectF32::setout(&vtx, x - 12.0f, y - 4.0f, 8.0f, 8.0f), _uvwhWhiteDot);
			if(tick >= 2)
			{
				RendererUtils::draw2dTextureRect(rc->getRenderer(), RectF32::setout(&vtx, x - 4.0f, y - 4.0f, 8.0f, 8.0f), _uvwhWhiteDot);
				if(tick >= 3)
				{
					RendererUtils::draw2dTextureRect(rc->getRenderer(), RectF32::setout(&vtx, x + 4.0f, y - 4.0f, 8.0f, 8.0f), _uvwhWhiteDot);
				}
			}
		}
#else
		// ドット描画で処理中であることを表示する	/// 現在はテクスチャを使わない円弧描画だが、将来的にはテクスチャで円を描画したい…【2015/03/05 r-kishi】
		if(tick >= 1)	{	RendererUtils::draw2dFillArc(rc->getRenderer(), x - 8.0f, y, 2.0f, 0.0f, TFW_DPI, 0.7853976f);	}
		if(tick >= 2)	{	RendererUtils::draw2dFillArc(rc->getRenderer(), x       , y, 2.0f, 0.0f, TFW_DPI, 0.7853976f);	}
		if(tick >= 3)	{	RendererUtils::draw2dFillArc(rc->getRenderer(), x + 8.0f, y, 2.0f, 0.0f, TFW_DPI, 0.7853976f);	}
#endif
	}
	else	// 終了時間のわかる行動
	{
		const f32 thetaStart = -TFW_HPI;
		f32 thetaEnd = ((1.0f - (sprt->getBefaviorCurrentFrame() / sprt->getBefaviorMaxFrame())) * TFW_DPI) - TFW_HPI;
		rc->getRenderer()->setSolidColor(255, 255, 255, 159);
		// 円弧描画で残り時間を表示する
		RendererUtils::draw2dFillArc(rc->getRenderer(), x, y, 8.0f, thetaStart, thetaEnd, 0.5f);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
