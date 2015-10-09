/***********************************************************************//**
 *	GaugeAnimEffect.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/25.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GaugeAnimEffect.h"

// Friends
#include "../common/CalcUtils.h"
#include "../episode/party/MyParty.h"
#include "../body/Game.h"
#include "../gui/CharFronts.h"
#include "../gui/GameGui.h"
#include "../mode/GameMode.h"
#include "../status/CharStat.h"
#include "../status/StatusDrawer.h"
#include "../unit/char/CharUnit.h"

// External
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// GaugeAnimEffect クラス

//==========================================================================
// GaugeAnimEffect 定数

#define FCNT_CHANGE_ANIM_NUM	(1.0f * FRAMERATE)

//==========================================================================
// GaugeAnimEffect メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ソウマ作成開始
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::startSoumaMaking(const CharUnit* unitTrg)
{
	Effect* ef = getEmptySlot();
	if(ef == 0L) { return; }
	ef->_unitTrg = unitTrg;
	ef->_kind = Effect::KIND_MAKING;
}

/*---------------------------------------------------------------------*//**
	ソウマ作成中更新通知
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::updateSoumaMaking(const CharUnit* unitTrg, f32 x, f32 y, s32 cur, s32 max, s32 preset)
{
	Effect* ef = getTargetSlot(unitTrg, Effect::KIND_MAKING);
	if(ef == 0L) { return; }

	f32 rateMax = StatusDrawer::W_GAUGE_DEFAULT / (f32)max;
	f32 w = cur * rateMax;
	f32 p = preset * rateMax;

	ef->_rect->set(x, y, w, StatusDrawer::H_LFGAUGE_DEFAULT);
	ef->_wPreset = p;
}

/*---------------------------------------------------------------------*//**
	ソウマ作成終了
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::endSoumaMaking(const CharUnit* unitTrg)
{
	Effect* ef = getTargetSlot(unitTrg, Effect::KIND_MAKING);
	if(ef == 0L) { return; }

	ef->reset();
}

/*---------------------------------------------------------------------*//**
	ダメージ通知
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::onDamage(const CharUnit* unitTrg, s32 cur, s32 max)
{
#if 0	// StatGauge に任せるように変更した	【2015/02/24 r-kishi】
	Effect* ef = getEmptySlot();
	if(ef == 0L) { return; }
	ef->_unitTrg = unitTrg;
	ef->_kind = Effect::KIND_DAMAGE;

	// ゲージの基準位置を得る
	UnitType utype = ef->_unitTrg->getUnitType();
	s32 ipc = -1;
	switch(utype)
	{
	case UNITTYPE_MY_CHAR:		ipc = MyParty::PC_MY;	break;
	case UNITTYPE_PARTNER_CHAR:	ipc = MyParty::PC_PTN;	break;
	case UNITTYPE_HELPER_CHAR:	ipc = MyParty::PC_HLP;	break;
	}

	CharFronts* chrfrnt = Game::getGame()->getGui()->getCharFronts();
	if(chrfrnt->isEnable())
	{
		PointF32 pt(F32_PMAX, F32_PMAX);
		if(ipc != -1)
		{
			chrfrnt->getPcGaugePos(&pt, ipc);
		}
		else
		{
			chrfrnt->getTargetGaugePos(&pt, unitTrg);
		}
		if(pt.x() != F32_PMAX)
		{
			f32 w = cur * (StatusDrawer::W_GAUGE_DEFAULT / (f32)max);
			if((0.0f < w) && (w < 1.0f)) { w = 1.0f; }
			else if(w > StatusDrawer::W_GAUGE_DEFAULT) { w = StatusDrawer::W_GAUGE_DEFAULT; }

			ef->_xBase = pt.x();
			ef->_rect->set(pt.x(), pt.y(), w, StatusDrawer::H_LFGAUGE_DEFAULT);
			updateChangeGaugePos(ef);
		}
	}
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GaugeAnimEffect::GaugeAnimEffect()
	: _slotsEf(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GaugeAnimEffect::~GaugeAnimEffect()
{
	ASSERT(_slotsEf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GaugeAnimEffect::create()
{
	_slotsEf = new Effect[NUM_EFSLOTS];

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::destroy()
{
	delete[] _slotsEf;
	_slotsEf = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::exec(ExecRes* res, const ExecCtx* ec)
{
#if 0	// StatGauge に任せるように変更した	【2015/02/24 r-kishi】
	for(int i = 0; i < NUM_EFSLOTS; i++)
	{
		if(_slotsEf[i]._kind == Effect::KIND_NULL)
		{
			continue;
		}
		Effect* ef = &_slotsEf[i];

		switch(ef->_kind)
		{
		case Effect::KIND_DAMAGE:
			// フレーム更新
			ef->_fcntAnim += ec->getDeltaFrame();	// フレームカウンタ更新
			if(ef->_fcntAnim >= FCNT_CHANGE_ANIM_NUM)
			{
				ef->reset();	// 終了
				break;
			}
			// 矩形更新
			updateChangeGaugePos(ef);
			break;
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::draw(const RenderCtx* rc)
{
	if(Game::getGame()->getGameMode()->getChildEndModeKind() == GameMode::MODE_GAME_INGAME_WORLDMAP)
	{
		return;		// ワールドマップ時は描かない
	}
	
	for(int i = 0; i < NUM_EFSLOTS; i++)
	{
		if(_slotsEf[i]._kind == Effect::KIND_NULL)
		{
			continue;
		}
		Effect* ef = &_slotsEf[i];

		switch(ef->_kind)
		{
#if 0	// StatGauge に任せるように変更した	【2015/02/24 r-kishi】
		case Effect::KIND_DAMAGE:
			// キャラステータスへの矩形描画
			{
				u8 alpha1 = 241;
				u8 alpha2 = 127;
				{
					f32 arate = (FCNT_CHANGE_ANIM_NUM - ef->_fcntAnim) / (f32)FCNT_CHANGE_ANIM_NUM;
					alpha1 = (u8)(alpha1 * arate);
					alpha2 = (u8)(alpha2 * arate);
				}
				if(ef->_rect->w() > 0.0f)
				{
					ColorU8 col1(255, 91, 27, alpha1);
					ColorU8 col2(255, 88, 27, alpha1);
					ColorU8 col3(255, 63, 27, alpha2);
					ColorU8 col4(255, 47, 27, alpha2);
					RendererUtils::draw2dColorRect(rc->getRenderer(), ef->_rect, &col1, &col2, &col3, &col4);
				}
			}
			break;
#endif

		case Effect::KIND_MAKING:
			// 作成中ステータスへの矩形描画
			if(ef->_rect->w() > 0.0f)
			{
				if(ef->_wPreset > 0.0f)
				{
					const ColorU8 colP1(200, 200, 200, 240);
					const ColorU8 colP2(220, 220, 220, 220);
					RendererUtils::draw2dColorRect(rc->getRenderer(), ef->_rect->x(), ef->_rect->y(), ef->_wPreset, ef->_rect->h(), &colP1, &colP1, &colP2, &colP2);
				}
				const ColorU8 col1(255, 91, 27, 241);
				const ColorU8 col2(255, 88, 27, 241);
				const ColorU8 col3(255, 63, 27, 127);
				const ColorU8 col4(255, 47, 27, 127);
				RendererUtils::draw2dColorRect(rc->getRenderer(), ef->_rect, &col1, &col2, &col3, &col4);
			}
			break;
		}
	}
}

#if 0	// StatGauge に任せるように変更した	【2015/02/24 r-kishi】
/*---------------------------------------------------------------------*//**
	ダメージゲージの位置更新
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::updateChangeGaugePos(Effect* ef)
{
	const CharStat* cstatTrg = ef->_unitTrg->getCharStat();
	f32 rateMax = StatusDrawer::W_GAUGE_DEFAULT / cstatTrg->getMaxEnergy();
	f32 wEne = cstatTrg->getEnergy() * rateMax;
	ef->_rect->x() = ef->_xBase + wEne;
}
#endif

/*---------------------------------------------------------------------*//**
	スロットから空き構造体を得る
**//*---------------------------------------------------------------------*/
GaugeAnimEffect::Effect* GaugeAnimEffect::getEmptySlot()
{
	for(int i = 0; i < NUM_EFSLOTS; i++)
	{
		if(_slotsEf[i]._kind == Effect::KIND_NULL)
		{
			return &_slotsEf[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	スロットから対象の構造体を得る
**//*---------------------------------------------------------------------*/
GaugeAnimEffect::Effect* GaugeAnimEffect::getTargetSlot(const CharUnit* unitTrg, u8 step)
{
	for(int i = 0; i < NUM_EFSLOTS; i++)
	{
		if((_slotsEf[i]._unitTrg == unitTrg) && (_slotsEf[i]._kind == step))
		{
			return &_slotsEf[i];
		}
	}
	return 0L;
}

////////////////////////////////////////////////////////////////////////////
// GaugeAnimEffect::Effect クラス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GaugeAnimEffect::Effect::Effect()
	: _rect(new RectF32())
{
	reset();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GaugeAnimEffect::Effect::~Effect()
{
	delete _rect;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void GaugeAnimEffect::Effect::reset()
{
	_kind = KIND_NULL;
	_rect->set(0.0f, 0.0f, 0.0f, 0.0f);
	_unitTrg = 0L;
	_wPreset = 0.0f;
	_fcntAnim = 0.0f;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
