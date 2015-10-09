/***********************************************************************//**
 *	StatGauge.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/02/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StatGauge.h"

// Friends
#include "../status/CharStat.h"
#include "../status/StatusDrawer.h"

// External

// Library
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Rect.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define FCNT_CHANGE_ANIM_NUM	(1.0f * FRAMERATE)

////////////////////////////////////////////////////////////////////////////
// StatGauge クラス

//==========================================================================
// StatGauge メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	キャラクターステータスを関連付ける
**//*---------------------------------------------------------------------*/
void StatGauge::attachCharStat(const CharStat* cstatRef)
{
	if(_cstatRef != cstatRef)
	{
		_cstatRef = cstatRef;
		resetAllGaugeAnim();	// 全アニメーションリセット
	}

	if(_cstatRef == 0L)
	{
		return;
	}

	// 値の同期
	_lvPrev = _cstatRef->getLevel();
	_lfenePrev = _cstatRef->getEnergy();
	_depthConcPrev = _cstatRef->getConcentrateDepth();
	_cntChargePrev = _cstatRef->getChargeCounter();
}

/*---------------------------------------------------------------------*//**
	生命エネルギー変化通知
**//*---------------------------------------------------------------------*/
void StatGauge::onLifeChange(s32 cur, s32 prev)
{
	GaugeAnim* ga = getEmptySlot();
	if(ga == 0L) { return; }
	bool damage = cur < prev;

	f32 rateMax = _rect->w() / _cstatRef->getMaxEnergy();
	f32 w = (damage ? prev - cur : cur - prev) * rateMax;
	f32 x = cur * rateMax;
	if(w < 0.1f) { return; }
	if(w > _rect->w()) { w = _rect->w(); }
	if(x > _rect->w()) { x = _rect->w(); }

	ga->_kind = damage ? GAK_LFE_DMG : GAK_LFE_RCV;
	ga->_rect->set(
		damage ? _rect->x() + x : _rect->x() + x - w,
		_rect->y() + StatusDrawer::H_NAME_DEFAULT,
		w,
		StatusDrawer::H_LFGAUGE_DEFAULT);
	ga->_fcntAnim = 0.0f;
}

/*---------------------------------------------------------------------*//**
	意識深度変化通知
**//*---------------------------------------------------------------------*/
void StatGauge::onConcentrateChange(s32 cur, s32 prev)
{
	GaugeAnim* ga = getEmptySlot();
	if(ga == 0L) { return; }
	bool damage = cur < prev;

	f32 rateMax = _rect->w() / (f32)CharStat::CONCD_HENL_MIN;
	f32 w = (damage ? prev - cur : cur - prev) * rateMax;
	f32 x = cur * rateMax;
	if(w < 0.1f) { return; }
	if(w > _rect->w()) { w = _rect->w(); }
	if(x > _rect->w()) { x = _rect->w(); }

	ga->_kind = damage ? GAK_CNS_DMG : GAK_CNS_RCV;
	ga->_rect->set(
		damage ? _rect->x() + x : _rect->x() + x - w,
		_rect->y() + StatusDrawer::H_NAME_DEFAULT + StatusDrawer::H_LFGAUGE_DEFAULT + 1.0f,
		w,
		StatusDrawer::H_CONSGAUGE_DEFAULT);
	ga->_fcntAnim = 0.0f;
}

/*---------------------------------------------------------------------*//**
	ため変化通知
**//*---------------------------------------------------------------------*/
void StatGauge::onChargeChange(s32 cur, s32 prev)
{
	GaugeAnim* ga = getEmptySlot();
	if(ga == 0L) { return; }
	bool damage = cur < prev;

	f32 rateMax = _rect->w() / (f32)CharStat::CHARGECNT_MAX;
	f32 w = (damage ? prev - cur : cur - prev) * rateMax;
	f32 x = cur * rateMax;
	if(w < 0.1f) { return; }
	if(w > _rect->w()) { w = _rect->w(); }
	if(x > _rect->w()) { x = _rect->w(); }

	ga->_kind = damage ? GAK_CHG_DMG : GAK_CHG_RCV;
	ga->_rect->set(
		damage ? _rect->x() + x : _rect->x() + x - w,
		_rect->y() + StatusDrawer::H_NAME_DEFAULT + StatusDrawer::H_LFGAUGE_DEFAULT + StatusDrawer::H_MPGAUGE_DEFAULT - StatusDrawer::H_CHARGEGAUGE_DEFAULT,
		w,
		StatusDrawer::H_CHARGEGAUGE_DEFAULT);
	ga->_fcntAnim = 0.0f;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StatGauge::StatGauge()
	: _cstatRef(0L)
	, _texGuiRef(0L)
	, _rect(0L)
	, _lvPrev(0)
	, _lfenePrev(0)
	, _depthConcPrev(0)
	, _cntChargePrev(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StatGauge::~StatGauge()
{
	ASSERT(_rect == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool StatGauge::create(Texture* texGuiRef, const RectF32* rect)
{
	_texGuiRef = texGuiRef;

	// 矩形の作成
	_rect = new RectF32(*rect);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StatGauge::destroy()
{
	// 矩形の削除
	delete _rect;
	_rect = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StatGauge::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_cstatRef == 0L)	
	{
		return;
	}

	// 値の更新
	s32 lv = _cstatRef->getLevel();
	if(_lvPrev != _cstatRef->getLevel())
	{
		_lvPrev = lv;
		resetAllGaugeAnim();	// 全アニメーションリセット
	}
	else
	{
		s32 lfene = _cstatRef->getEnergy();
		if(_lfenePrev != lfene)
		{
			onLifeChange(lfene, _lfenePrev);
			_lfenePrev = lfene;
		}
		s32 depthConc = _cstatRef->getConcentrateDepth();
		if(_depthConcPrev != depthConc)
		{
#if 1
			onConcentrateChange(depthConc, _depthConcPrev);
			_depthConcPrev = depthConc;
#else
			s32 n = (s32)((depthConc - _depthConcDisp) * 0.0625f * ec->getDeltaFrame());
			_depthConcDisp += n;
#endif
		}
		u16 cntCharge = _cstatRef->getChargeCounter();
		if(_cntChargePrev != cntCharge)
		{
#if 1
			onChargeChange(cntCharge, _cntChargePrev);
			_cntChargePrev = cntCharge;
#else
			s32 n = (s32)((cntCharge - _cntChargeDisp) * 0.0625f * ec->getDeltaFrame());
			_cntChargeDisp += n;
#endif
		}
	}

	// アニメーション制御
	for(int i = 0; i < NUM_SLOTS; i++)
	{
		if(_slotGa[i]._kind == GAK_NULL) { continue; }
		_slotGa[i]._fcntAnim += ec->getDeltaFrame();
		if(_slotGa[i]._fcntAnim >= FCNT_CHANGE_ANIM_NUM)
		{
			_slotGa[i]._fcntAnim = -1.0f;
			if(_slotGa[i]._fcntAnim <= 0.0f)
			{
				_slotGa[i]._kind = GAK_NULL;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void StatGauge::draw(const RenderCtx* rc)
{
	if(_cstatRef == 0L)
	{
		return;
	}
	Renderer* rdr = rc->getRenderer();

	// ゲージ描画
	StatusDrawer::drawCharStat(rc, _texGuiRef, _cstatRef, _rect->x(), _rect->y(), _rect->w(), _rect->h(), true, true);

	// アニメーション描画
	ColorU8 col1, col2, col3, col4;
	for(int i = 0; i < NUM_SLOTS; i++)
	{
		if(_slotGa[i]._kind == GAK_NULL) { continue; }
		u8 alpha1 = 241;
		u8 alpha2 = 127;
		f32 arate = (FCNT_CHANGE_ANIM_NUM - _slotGa[i]._fcntAnim) / (f32)FCNT_CHANGE_ANIM_NUM;
		alpha1 = (u8)(alpha1 * arate);
		alpha2 = (u8)(alpha2 * arate);
		if((_slotGa[i]._kind == GAK_LFE_DMG) || (_slotGa[i]._kind == GAK_CNS_DMG) || (_slotGa[i]._kind == GAK_CHG_DMG))
		{
			col1.set(255, 91, 27, alpha1);
			col2.set(255, 88, 27, alpha1);
			col3.set(255, 63, 27, alpha2);
			col4.set(255, 47, 27, alpha2);
		}
		else
		{
			col1.set(95, 191, 255, alpha1);
			col2.set(95, 188, 255, alpha1);
			col3.set(95, 163, 255, alpha2);
			col4.set(95, 147, 255, alpha2);
		}
		RendererUtils::draw2dColorRect(rc->getRenderer(), _slotGa[i]._rect, &col1, &col2, &col3, &col4);
	}
}

/*---------------------------------------------------------------------*//**
	空のスロットを得る
**//*---------------------------------------------------------------------*/
StatGauge::GaugeAnim* StatGauge::getEmptySlot()
{
	for(int i = 0; i < NUM_SLOTS; i++)
	{
		if(_slotGa[i]._kind == GAK_NULL)
		{
			return &_slotGa[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	全アニメーションをリセットする
**//*---------------------------------------------------------------------*/
void StatGauge::resetAllGaugeAnim()
{
	for(int i = 0; i < NUM_SLOTS; i++)
	{
		_slotGa[i].reset();
	}

}

////////////////////////////////////////////////////////////////////////////
// StatGauge::GaugeAnim クラス

//==========================================================================
// StatGauge::GaugeAnim メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StatGauge::GaugeAnim::GaugeAnim()
	: _kind(GAK_NULL)
	, _rect(new RectF32())
	, _fcntAnim(-1.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StatGauge::GaugeAnim::~GaugeAnim()
{
	delete _rect;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void StatGauge::GaugeAnim::reset()
{
	_kind = GAK_NULL;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
