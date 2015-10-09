/***********************************************************************//**
 *	PcCharStatWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PcCharStatWindow.h"

// Friends
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../status/CharStat.h"
#include "../../../status/CharParam.h"
#include "../../../status/StatusDrawer.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/Panel.h"
#include "../../../../tfw/gui/PanelText.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS
	
////////////////////////////////////////////////////////////////////////////
// 定数

#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define X_PARAM						(X_BASE + 160)
#define Y_PARAM						(70)
#define W_PARAM						(200)
#define H_PARAM						(20)
#define Y_PARAM_PAD					(4)
#define X_CHARSTAT					((f32)(X_BASE + 44))
#define Y_CHARSTAT					(200.0f)
#define W_CHARSTAT					(100.0f)
#define H_CHARSTAT					(75.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcCharStatWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcCharStatWindow::PcCharStatWindow()
{
	for(int i = 0; i < NUM_PNL; i++)		{	_pnl[i] = 0;			}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PcCharStatWindow::~PcCharStatWindow()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_PNL; i++)	{	ASSERT(_pnl[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PcCharStatWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	if(!PcStatusWindowBase::create(fontRef, texRef))
	{
		return false;
	}

	RectF32 rect;
	ColorU8 col(0, 0, 0, 255);

	// 子パネル作成
	for(int i = 0; i < NUM_PNL; i++)
	{
		_pnl[i] = new Panel();
		_pnl[i]->create(RectF32::setout(&rect, X_PARAM, Y_PARAM + (f32)(i * (H_PARAM + Y_PARAM_PAD)), W_PARAM, H_PARAM));
		_pnl[i]->createText(fontRef, 20, TypeUtils::getEmptyString(), &col, 0, 0, 0, 0, false, false, false);
		_pnl[i]->getPanelText()->setAlign(PanelText::F_ALIGN_RIGHT);
		_pnl[i]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	}

	// キャラクタの無効化
	Party* party = Game::getGame()->getMyParty();
	setEnablePc(MyParty::PC_MY, party->isEnablePc(MyParty::PC_MY));
	setEnablePc(MyParty::PC_PTN, party->isEnablePc(MyParty::PC_PTN));
	setEnablePc(MyParty::PC_HLP, party->isEnablePc(MyParty::PC_HLP));

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PcCharStatWindow::destroy()
{
	// 子パネル削除
	for(int i = 0; i < NUM_PNL; i++)
	{
		if(_pnl[i] != 0)
		{
			_pnl[i]->destroy();
			delete _pnl[i];
			_pnl[i] = 0L;
		}
	}

	PcStatusWindowBase::destroy();
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void PcCharStatWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	PcStatusWindowBase::drawContents(rc, rectBase);

	Renderer* rdr = rc->getRenderer();
	const RectF32* rect = getRectangle();
	const Party* party = Game::getGame()->getMyParty();
	const PcUnit* pcunit = (PcUnit*)party->getPcUnit(_ipc);
	const CharStat* cstat = pcunit->getCharStat();
	const CharParam* cprm = cstat->getCharParam();

	StatusDrawer::drawCharStat(rc, _texRef, cstat, X_CHARSTAT, Y_CHARSTAT, W_CHARSTAT, H_CHARSTAT, true, false);

	VcString str;
	f32 x = (f32)X_PARAM;
	f32 y = (f32)Y_PARAM + 2.0f;
	const f32 yd = (f32)(H_PARAM + Y_PARAM_PAD);
	rdr->setSolidColor(0, 0, 0, 255);
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_ENE_FULL)->getRaw()),    x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_ENEDMG_FULL)->getRaw()), x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_PHSDMG_FULL)->getRaw()), x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_LV_FULL)->getRaw()),     x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_UNI_FULL)->getRaw()),    x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_ACT_FULL)->getRaw()),    x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_STB_FULL)->getRaw()),    x, y, 16, rc);
	y += yd;
	EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%s:", GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_EXP_FULL)->getRaw()),    x, y, 16, rc);

	// 子パネル描画
	for(int i = 0; i < NUM_PNL; i++)
	{
		if(_pnl[i] != 0)	{	_pnl[i]->draw(rc);	}
	}
}

/*---------------------------------------------------------------------*//**
	ウインドウ表示・非表示通知
**//*---------------------------------------------------------------------*/
void PcCharStatWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// キャラクターのステータス値を更新する
		updateValue();
	}
}

/*---------------------------------------------------------------------*//**
	キャラクター変更ハンドラ
**//*---------------------------------------------------------------------*/
void PcCharStatWindow::onChangePc()
{
	// キャラクターのステータス値を更新する
	updateValue();
}

/*---------------------------------------------------------------------*//**
	キャラクターのステータス値を更新する
**//*---------------------------------------------------------------------*/
void PcCharStatWindow::updateValue()
{
	const Party* party = Game::getGame()->getMyParty();
	const PcUnit* pcunit = (PcUnit*)party->getPcUnit(_ipc);
	const CharStat* cstat = pcunit->getCharStat();
	const CharParam* cprm = cstat->getCharParam();

	VcString str;
	_pnl[PNL_ENE_VAL   ]->getPanelText()->setText(VcString::format(&str, "%d/%d", cstat->getEnergyShort(), cstat->getMaxEnergyShort()), false);
	_pnl[PNL_ENEDMG_VAL]->getPanelText()->setText(VcString::format(&str, "%d", cstat->getEnergyDamageShort()), false);
	_pnl[PNL_PHSDMG_VAL]->getPanelText()->setText(VcString::format(&str, "%d", cstat->getPhysicsDamageShort()), false);
	_pnl[PNL_LV_VAL    ]->getPanelText()->setText(VcString::format(&str, "%d", cprm->getLevel()), false);
	_pnl[PNL_UNI_VAL   ]->getPanelText()->setText(VcString::format(&str, "%d", cprm->getUnity()), false);
	_pnl[PNL_ACT_VAL   ]->getPanelText()->setText(VcString::format(&str, "%d", cprm->getActivity()), false);
	_pnl[PNL_STB_VAL   ]->getPanelText()->setText(VcString::format(&str, "%d", cprm->getStability()), false);
	_pnl[PNL_EXP_VAL   ]->getPanelText()->setText(VcString::format(&str, "%d", cprm->getExperience()), false);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
