/***********************************************************************//**
 *	PcAbilityWindow.cpp
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
#include "PcAbilityWindow.h"

// Friends
#include "../../../char/CharDef.h"
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../item/ItemEquip.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../status/EquipStat.h"
#include "../../../status/StatusDrawer.h"

// External
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define X_TOTAL						(X_BASE + 170.0f)
#define Y_TOTAL						(100.0f)
#define X_EFFENT					(X_BASE + 44.0f)
#define Y_EFFENT					(194.0f)

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_SELF_ICON				928.0f / W_TEX, 136.0f / H_TEX, 48.0f / W_TEX, 40.0f / H_TEX
#define UV_EQUIP_ICON				968.0f / W_TEX, 48.0f / H_TEX, 40.0f / W_TEX, 40.0f / H_TEX
#define UV_PARTNER_ICON				976.0f / W_TEX, 136.0f / H_TEX, 48.0f / W_TEX, 40.0f / H_TEX
#define UV_HELPER_ICON				928.0f / W_TEX, 176.0f / H_TEX, 48.0f / W_TEX, 40.0f / H_TEX
#define UV_MAP_ICON					976.0f / W_TEX, 176.0f / H_TEX, 48.0f / W_TEX, 40.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcAbilityWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PcAbilityWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	if(!PcStatusWindowBase::create(fontRef, texRef))
	{
		return false;
	}

	// キャラクタの無効化
	Party* party = Game::getGame()->getMyParty();
	setEnablePc(MyParty::PC_MY, party->isEnablePc(MyParty::PC_MY));
	setEnablePc(MyParty::PC_PTN, false);
	setEnablePc(MyParty::PC_HLP, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void PcAbilityWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	PcStatusWindowBase::drawContents(rc, rectBase);

	Renderer* rdr = rc->getRenderer();
	const Party* party = Game::getGame()->getMyParty();
	const PcUnit* pcunit = (PcUnit*)party->getPcUnit(_ipc);

	f32 x;
	f32 y;
	Elene elene;
	StatusDrawer::DrawInfo di;
	RectF32 rect;

	rdr->setSolidColor(0, 0, 0, 255);
	EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_ABILITY_CREATABLE_OQL), X_TOTAL, 80.0f, 16, rc);
	if(pcunit->getEvokableElene(&elene, Unit::EVECAT_TOTAL))
	{
		x = X_TOTAL;
		y = Y_TOTAL;
		StatusDrawer::drawElene(&di, rc, _texRef, &elene, x, y, 150, 75);
		/*
		x = 365.0f;
		rdr->setSolidColor(0, 0, 0, 255);
		VcString str;
		EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", elene.getAethel() / Elene::ELENE_VALID_FIGURE), x, y +  0.0f, 16, rc);
		EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", elene.getWind()   / Elene::ELENE_VALID_FIGURE), x, y + 16.0f, 16, rc);
		EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", elene.getFire()   / Elene::ELENE_VALID_FIGURE), x, y + 32.0f, 16, rc);
		EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", elene.getWater()  / Elene::ELENE_VALID_FIGURE), x, y + 48.0f, 16, rc);
		EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", elene.getSoil()   / Elene::ELENE_VALID_FIGURE), x, y + 64.0f, 16, rc);
		*/
	}

	di.reset();
	x = X_EFFENT;
	y = Y_EFFENT;
	EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_ABILITY_EFFECTIVE_ENT), x, y, 16, rc);
	y += 16.0f;
	const RectF32 uvEeIcon[5] =
	{
		RectF32( UV_SELF_ICON ),
		RectF32( UV_EQUIP_ICON ),
		RectF32( UV_PARTNER_ICON ),
		RectF32( UV_HELPER_ICON ),
		RectF32( UV_MAP_ICON ),
	};
	for(int icat = Unit::EVECAT_SELF; icat <= Unit::EVECAT_MAP; icat++)
	{
		if(icat == Unit::EVECAT_PARTY)
		{
			if(!party->getPcUnit(MyParty::PC_PTN)->isEnable())	{	continue;	}	// メンバーがいない
		}
		if(icat == Unit::EVECAT_HELPER)
		{
			if(!party->getPcUnit(MyParty::PC_HLP)->isEnable())	{	continue;	}	// 支援モンスターがいない
		}
		if(icat == Unit::EVECAT_EQUIP)
		{
			if((pcunit->getItemEquip() != 0L) && (pcunit->getItemEquip()->getEquipStat()->getEquipItem(EquipPlace::EQP_ACCSR) == 0L))	{	continue;	}	// 装備がない
		}

		if(pcunit->getEvokableElene(&elene, (Unit::EvokableElneCategory)icat))
		{
			rdr->setSolidColor(255, 255, 255, 255);
			rdr->bindTexture(_texRef);
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, x, y, 48.0f, 40.0f), &uvEeIcon[icat - 1]);
			StatusDrawer::drawElene(&di, rc, _texRef, &elene, x, y + 42.0f, 60.0f, 30.0f);
			x += 64.0f;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
