/***********************************************************************//**
 *	StatusDrawer.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/16.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StatusDrawer.h"

// Friends
#include "CharStat.h"
#include "Elene.h"
#include "EquipPlace.h" 
#include "../body/GameRenderCtx.h"
#include "../gui/GameFontSet.h"
#include "../unit/Unit.h"

// External
#include "../../tfw/framemod/ExecCtx.h"
#include "../../tfw/framemod/RenderCtx.h"
#include "../../tfw/gcmn/EasyStringDrawer.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
//#include "../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX					1024.0f
#define H_TEX					1024.0f

// バー テクスチャ UV
#define W_GAUGE_AE				4
#define UV_GAUGE_AE				116.0f, 875.0f, (f32)W_GAUGE_AE, 3.0f, W_TEX, H_TEX
#define W_GAUGE_WI				8
#define UV_GAUGE_WI				99.0f, 875.0f, (f32)W_GAUGE_WI, 3.0f, W_TEX, H_TEX
#define W_GAUGE_FI				16
#define UV_GAUGE_FI				66.0f, 875.0f, (f32)W_GAUGE_FI, 3.0f, W_TEX, H_TEX
#define W_GAUGE_WA				32
#define UV_GAUGE_WA				1.0f, 875.0f, (f32)W_GAUGE_WA, 3.0f, W_TEX, H_TEX
#define W_GAUGE_SO				64
#define UV_GAUGE_SO				1.0f, 866.0f, (f32)W_GAUGE_SO, 8.0f, W_TEX, H_TEX
#define W_GAUGE_LF				128
#define UV_GAUGE_LF				1.0f, 849.0f, (f32)W_GAUGE_SO, 8.0f, W_TEX, H_TEX

// 元素アイコン
#define W_ELM					12.0f
#define H_ELM					12.0f
#define UV_ELM_SO_C				642.0f, 930.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_WA_C				658.0f, 930.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_FI_C				674.0f, 930.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_WI_C				690.0f, 930.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_AE_C				706.0f, 930.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_SO_N				642.0f, 946.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_WA_N				658.0f, 946.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_FI_N				674.0f, 946.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_WI_N				690.0f, 946.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_AE_N				706.0f, 946.0f, W_ELM, H_ELM, W_TEX, H_TEX
#define UV_ELM_WRAP				760.0f, 928.0f, 24.0f, 24.0f, W_TEX, H_TEX
#define UV_ELM_BARR				736.0f, 928.0f, 24.0f, 24.0f, W_TEX, H_TEX
#define UV_ELM_EXCEED			720.0f, 928.0f, 16.0f, 16.0f, W_TEX, H_TEX

// 意識集中度テクスチャ UV
#define UV_CONS_PLOT			800.0f, 952.0f, 5.0f, 5.0f, W_TEX, H_TEX

// 極小数字 UV
#define UV_TINYNUM_U			1
#define UV_TINYNUM_V			884
#define UV_TINYNUM_W			4
#define UV_TINYNUM_H			6

// 元素量極小数字描画の角度
#define XROT_TINYNUM			(48.0f)

// 特殊ステータス
#if 0	// 16x16
	#define W_SPSTICON			16.0f
	#define H_SPSTICON			16.0f
	#define UV_SPSTAT_DOWN		672.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_HEAVY		640.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_SLEEP		656.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_CONFU		688.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_TRANS		640.0f, 976.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
#else	// 8x8
	#define W_SPSTICON			8.0f
	#define H_SPSTICON			8.0f
	#define UV_SPSTAT_DOWN		720.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_HEAVY		704.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_SLEEP		712.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_CONFU		728.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
	#define UV_SPSTAT_TRANS		736.0f, 960.0f, W_SPSTICON, H_SPSTICON, W_TEX, H_TEX
#endif

////////////////////////////////////////////////////////////////////////////
// 静的変数

#define IDX_LF					Elene::NUM_EL
#define IDX_CONS				1	// 以前の空ゲージ用アニメテクスチャの流用、地，火，風，空の４つは現在使っていない
static f32 s_framecnt = 0.0f;
static f32 s_ofsAnimElLf[Elene::NUM_EL + 1];		// +1 は HP ゲージ用
static RectF32 s_uvElLf[Elene::NUM_EL + 1];
static RectF32 s_uvElmCql[Elene::NUM_EL];
static RectF32 s_uvElmNql[Elene::NUM_EL];
static RectF32 s_uvElmVeil[EleneStat::NUM_VEIL + 1];	// +1 は増悪表現用
static RectF32 s_uvConsPlot;
static RectF32 s_uvTinyNum[10];
static RectF32 s_uvTinySign[2];
static RectF32 s_uvSpStatDown;
static RectF32 s_uvSpStatHeavy;
static RectF32 s_uvSpStatSleep;
static RectF32 s_uvSpStatConfu;
static RectF32 s_uvSpStatTrans;

////////////////////////////////////////////////////////////////////////////
// 静的関数

/*---------------------------------------------------------------------*//**
	元素エネルギー描画時のスケール情報を得る
**//*---------------------------------------------------------------------*/
inline void procureDrawInfo(StatusDrawer::DrawInfo* di, s32 eeMax, f32 wEl, f32 hEl)
{
	#if 1	// Ver3.x～
	{
		di->_scaleEne = wEl / (f32)eeMax;
	}
	#elif 1
		if(eeMax < 1000)		{	di->set(   0.001f,  0.7f, 127,  95, hEl * 0.30f);	}
		else if(eeMax < 5000)	{	di->set(  0.0002f, 0.75f, 143, 111, hEl * 0.25f);	}
		else if(eeMax < 10000)	{	di->set(  0.0001f,  0.8f, 159, 127, hEl * 0.20f);	}
		else if(eeMax < 50000)	{	di->set( 0.00002f, 0.85f, 175, 143, hEl * 0.15f);	}
		else if(eeMax < 100000)	{	di->set( 0.00001f,  0.9f, 191, 159, hEl * 0.10f);	}
		else if(eeMax < 500000)	{	di->set(0.000002f, 0.95f, 207, 175, hEl * 0.05f);	}
		else					{	di->set(0.000001f,  1.0f, 223, 191,       0.0f);	}	// 100000 ～ 1000000
	#elif 1
		if(eeMax < 1000)		{	di->set(   0.001f, 0.7f, 127,  95, hEl * 0.6f);		}
		else if(eeMax < 10000)	{	di->set(  0.0001f, 0.8f, 159, 127, hEl * 0.4f);		}
		else if(eeMax < 100000)	{	di->set( 0.00001f, 0.9f, 191, 159, hEl * 0.2f);		}
		else					{	di->set(0.000001f, 1.0f, 223, 191,       0.0f);		}	// 100000 ～ 1000000
	#else	// Ver1.3.1
		if(eeMax < 20000)
		{
			di->set(0.00005f, 0.9f, 127, 63, hEl * 0.7f);
		}
		else if((20000 <= eeMax) && (eeMax < 50000))
		{
			di->set(0.00002f, 0.95f, 127, 63, hEl * 0.45f);
		}
		else if((50000 <= eeMax) && (eeMax < 100000))
		{
			di->seT(0.00001f, 1.0f, 127, 63, hEl * 0.2f);
		}
		else if((100000 <= eeMax) && (eeMax < 200000))
		{
			di->set(0.000005f, 1.1f, 127, 127, 0.0f);
		}
		else if((200000 <= eeMax) && (eeMax < 400000))
		{
			di->set(0.0000025f, 1.4f, 159, 159, 0.0f);
		}
		else if((400000 <= eeMax) && (eeMax < 800000))
		{
			di->set(0.00000125f, 1.8f, 191, 191, 0.0f);
		}
		else	// 800000 ～ 1000000
		{	
			di->set(0.000001f, 2.0f, 223, 223, 0.0f);
		}
	#endif
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StatusDrawer 定数

const f32 StatusDrawer::W_GAUGE_DEFAULT = 80.0f;			// 標準のゲージ幅
const f32 StatusDrawer::H_GAUGE_DEFAULT = H_NAME_DEFAULT + H_LFGAUGE_DEFAULT + H_MPGAUGE_DEFAULT + H_ENEGAUGE_DEFAULT;	// 標準のゲージ高さ
const f32 StatusDrawer::H_NAME_DEFAULT = 10.0f;				// 標準の名前表示分高さ
const f32 StatusDrawer::H_LFGAUGE_DEFAULT = 8.0f;			// 標準の生命ゲージ高さ
const f32 StatusDrawer::H_MPGAUGE_DEFAULT = 6.0f;			// 標準の精神ゲージ高さ
const f32 StatusDrawer::H_CONSGAUGE_DEFAULT = 4.0f;			// 標準の意識ゲージ高さ
const f32 StatusDrawer::H_CHARGEGAUGE_DEFAULT = 1.0f;		// 標準のためゲージ高さ
const f32 StatusDrawer::H_ENEGAUGE_DEFAULT = 36.0f;			// 標準の元素ゲージ高さ

//==========================================================================
// StatusDrawer メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
void StatusDrawer::init()
{
	// 元素バー UV の作成
	Gcalc::calcTexUv(&s_uvElLf[Elene::EL_AE], UV_GAUGE_AE, 1.0f);
	Gcalc::calcTexUv(&s_uvElLf[Elene::EL_WI], UV_GAUGE_WI, 1.0f);
	Gcalc::calcTexUv(&s_uvElLf[Elene::EL_FI], UV_GAUGE_FI, 1.0f);
	Gcalc::calcTexUv(&s_uvElLf[Elene::EL_WA], UV_GAUGE_WA, 1.0f);
	Gcalc::calcTexUv(&s_uvElLf[Elene::EL_SO], UV_GAUGE_SO, 1.0f);
	Gcalc::calcTexUv(&s_uvElLf[IDX_LF], UV_GAUGE_LF, 1.0f);
	// 元素アイコン UV の作成
	Gcalc::calcTexUvNoAa(&s_uvElmCql[Elene::EL_AE], UV_ELM_AE_C);
	Gcalc::calcTexUvNoAa(&s_uvElmCql[Elene::EL_WI], UV_ELM_WI_C);
	Gcalc::calcTexUvNoAa(&s_uvElmCql[Elene::EL_FI], UV_ELM_FI_C);
	Gcalc::calcTexUvNoAa(&s_uvElmCql[Elene::EL_WA], UV_ELM_WA_C);
	Gcalc::calcTexUvNoAa(&s_uvElmCql[Elene::EL_SO], UV_ELM_SO_C);
	Gcalc::calcTexUvNoAa(&s_uvElmNql[Elene::EL_AE], UV_ELM_AE_N);
	Gcalc::calcTexUvNoAa(&s_uvElmNql[Elene::EL_WI], UV_ELM_WI_N);
	Gcalc::calcTexUvNoAa(&s_uvElmNql[Elene::EL_FI], UV_ELM_FI_N);
	Gcalc::calcTexUvNoAa(&s_uvElmNql[Elene::EL_WA], UV_ELM_WA_N);
	Gcalc::calcTexUvNoAa(&s_uvElmNql[Elene::EL_SO], UV_ELM_SO_N);
	Gcalc::calcTexUvNoAa(&s_uvElmVeil[EleneStat::VEIL_WP], UV_ELM_WRAP);
	Gcalc::calcTexUvNoAa(&s_uvElmVeil[EleneStat::VEIL_BR], UV_ELM_BARR);
	Gcalc::calcTexUvNoAa(&s_uvElmVeil[EleneStat::NUM_VEIL], UV_ELM_EXCEED);
	// 意識集中度 UV の作成
	Gcalc::calcTexUv(&s_uvConsPlot, UV_CONS_PLOT, 1.0f);
	// 極小数字 UV の作成
	{
		f32 wiTex = 1.0f / W_TEX;
		//f32 hiTex = 1.0f / H_TEX;
		f32 vTinyNum = UV_TINYNUM_V * wiTex;
		f32 wTinyNum = UV_TINYNUM_W * wiTex;
		f32 hTinyNum = UV_TINYNUM_H * wiTex;
		for(int i = 0; i < 10; i++)
		{
			s_uvTinyNum[i].set((UV_TINYNUM_U + (i * UV_TINYNUM_W)) * wiTex, vTinyNum, wTinyNum, hTinyNum);
		}
		for(int i = 0; i < 2; i++)
		{
			s_uvTinySign[i].set((UV_TINYNUM_U + (i * UV_TINYNUM_W)) * wiTex, vTinyNum + hTinyNum, wTinyNum, hTinyNum);
		}
	}
	// 特殊ステータスアイコン UV の作成
	Gcalc::calcTexUvNoAa(&s_uvSpStatDown, UV_SPSTAT_DOWN);
	Gcalc::calcTexUvNoAa(&s_uvSpStatHeavy, UV_SPSTAT_HEAVY);
	Gcalc::calcTexUvNoAa(&s_uvSpStatSleep, UV_SPSTAT_SLEEP);
	Gcalc::calcTexUvNoAa(&s_uvSpStatConfu, UV_SPSTAT_CONFU);
	Gcalc::calcTexUvNoAa(&s_uvSpStatTrans, UV_SPSTAT_TRANS);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StatusDrawer::exec(const ExecCtx* ec)
{
	s_framecnt += ec->getDeltaFrame();
	s32 icnt = (s32)s_framecnt / 2;
	f32 wi = 1.0f / (f32)W_TEX;
	s_ofsAnimElLf[Elene::EL_AE] = (f32)(W_GAUGE_AE - (icnt & (W_GAUGE_AE - 1))) * wi;
	s_ofsAnimElLf[Elene::EL_WI] = (f32)(W_GAUGE_WI - (icnt & (W_GAUGE_WI - 1))) * wi;
	s_ofsAnimElLf[Elene::EL_FI] = (f32)(W_GAUGE_FI - (icnt & (W_GAUGE_FI - 1))) * wi;
	s_ofsAnimElLf[Elene::EL_WA] = (f32)(W_GAUGE_WA - (icnt & (W_GAUGE_WA - 1))) * wi;
	s_ofsAnimElLf[Elene::EL_SO] = (f32)(W_GAUGE_SO - (icnt & (W_GAUGE_SO - 1))) * wi;
	s_ofsAnimElLf[IDX_LF] = (f32)(W_GAUGE_LF - (icnt & (W_GAUGE_LF - 1))) * wi;
}

/*---------------------------------------------------------------------*//**
	キャラクタ ステータス 2D 描画
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawCharStat(const RenderCtx* rc, Texture* texGui, const CharStat* cstat, f32 x, f32 y, f32 w, f32 h, bool drawMpGauge, bool drawDifEleneNum)
{
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = grc->getRenderer();
	Font* fontEn = grc->getFontSet()->getFont(GameFontSet::EN);
	ColorU8 colBase(0, 0, 0, 63);
	f32 wEneDmg = 0.0f;
	f32 wPhsDmg = 0.0f;
	f32 xWk;
	f32 yWk = y;
	f32 yLf;
	f32 wrfRest = 1.0f;
	CharStat::Stat stat = cstat->getStat();
	u16 spstat = cstat->getSpStatFlags();
	bool exstat = (stat == CharStat::STAT_DOWN) || (spstat != 0);

	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成

	// 名前表示
	if((cstat->getOwnerUnit() != 0L) && !exstat)
	{
		const VcString* name = cstat->getOwnerUnit()->getMainName();
		if(name != 0L)
		{
			rdr->setSolidColor(255, 255, 255, 255);
			Font* fontJp = grc->getFontSet()->getFont(GameFontSet::JP);
			EasyStringDrawer::draw(fontJp, name, x, yWk, 8, rc);
		}
	}

	// 特殊ステータス
	if(exstat)
	{
		const f32 W_ICONPAD = 2.0f;
		xWk = x;
		rdr->bindTexture(texGui);
		rdr->setSolidColor(255, 255, 255, 255);
		if(stat == CharStat::STAT_DOWN)
		{
			RendererUtils::draw2dTextureRect(rdr, xWk, y, W_SPSTICON, H_SPSTICON, s_uvSpStatDown.x(), s_uvSpStatDown.y(), s_uvSpStatDown.w(), s_uvSpStatDown.h());
			xWk += W_SPSTICON + W_ICONPAD;
		}
		if(TFW_IS_FLAG(spstat, CharStat::SPSTAT_HEAVY))
		{
			RendererUtils::draw2dTextureRect(rdr, xWk, y, W_SPSTICON, H_SPSTICON, s_uvSpStatHeavy.x(), s_uvSpStatHeavy.y(), s_uvSpStatHeavy.w(), s_uvSpStatHeavy.h());
			xWk += W_SPSTICON + W_ICONPAD;
		}
		if(TFW_IS_FLAG(spstat, CharStat::SPSTAT_SLEEP))
		{
			RendererUtils::draw2dTextureRect(rdr, xWk, y, W_SPSTICON, H_SPSTICON, s_uvSpStatSleep.x(), s_uvSpStatSleep.y(), s_uvSpStatSleep.w(), s_uvSpStatSleep.h());
			xWk += W_SPSTICON + W_ICONPAD;
		}
		if(TFW_IS_FLAG(spstat, CharStat::SPSTAT_CONFU))
		{
			RendererUtils::draw2dTextureRect(rdr, xWk, y, W_SPSTICON, H_SPSTICON, s_uvSpStatConfu.x(), s_uvSpStatConfu.y(), s_uvSpStatConfu.w(), s_uvSpStatConfu.h());
			xWk += W_SPSTICON + W_ICONPAD;
		}
		if(TFW_IS_FLAG(spstat, CharStat::SPSTAT_TRANS))
		{
			RendererUtils::draw2dTextureRect(rdr, xWk, y, W_SPSTICON, H_SPSTICON, s_uvSpStatTrans.x(), s_uvSpStatTrans.y(), s_uvSpStatTrans.w(), s_uvSpStatTrans.h());
			xWk += W_SPSTICON + W_ICONPAD;
		}
	}

	// 生命エネルギーゲージ
	yWk = y + H_NAME_DEFAULT;
	yLf = yWk;
	if(cstat->getMaxEnergy() > 0)
	{
		// 生命エネルギーゲージの地
		f32 rateMax = 1.0f / (f32)cstat->getMaxEnergy();
		wrfRest = (cstat->getMaxEnergy() - cstat->getEnergyDamage() - cstat->getPhysicsDamage()) * rateMax;
		rateMax *= w;
		wEneDmg = cstat->getEnergyDamage() * rateMax;
		wPhsDmg = cstat->getPhysicsDamage() * rateMax;
		RectF32 vtx(x, yLf, w, H_LFGAUGE_DEFAULT);
		RendererUtils::draw2dColorRect(rdr, &vtx, &colBase, &colBase, &colBase, &colBase);

		// 直接テクスチャアニメーションする
		::glMatrixMode(GL_TEXTURE);
		::glLoadIdentity();
		::glTranslatef(s_ofsAnimElLf[IDX_LF], 0.0f, 0.0f);
		rdr->bindTexture(texGui);

		// 残りエネルギー値表示
		s32 lfene = (cstat->getEnergy() + CharStat::LFENE_VALID_FIGURE - 1) / CharStat::LFENE_VALID_FIGURE;
		if(lfene > 0)
		{
			f32 wWk = w - wEneDmg - wPhsDmg;
			if(wWk < 1.0f)	{	wWk = 1.0f;	}
			if(TFW_IS_FLAG(cstat->getCreateFlags(), CharStat::CF_NEGATIVE_ATRB))	// 反属性
			{
				static const ColorU8 col1(191, 95, 191, 241);
				static const ColorU8 col2(191, 95, 188, 241);
				static const ColorU8 col3(123, 95, 255, 127);
				static const ColorU8 col4(111, 95, 255, 127);
				RendererUtils::draw2dColorTextureRect(rdr, x, yLf, wWk, H_LFGAUGE_DEFAULT, s_uvElLf[IDX_LF].x(), s_uvElLf[IDX_LF].y(), s_uvElLf[IDX_LF].w() * wrfRest, s_uvElLf[IDX_LF].h(), &col1, &col2, &col3, &col4);
			}
			else
			{
				static const ColorU8 col1(127, 255, 191, 241);
				static const ColorU8 col2(127, 255, 188, 241);
				static const ColorU8 col3(127, 163, 255, 127);
				static const ColorU8 col4(127, 147, 255, 127);
				RendererUtils::draw2dColorTextureRect(rdr, x, yLf, wWk, H_LFGAUGE_DEFAULT, s_uvElLf[IDX_LF].x(), s_uvElLf[IDX_LF].y(), s_uvElLf[IDX_LF].w() * wrfRest, s_uvElLf[IDX_LF].h(), &col1, &col2, &col3, &col4);
			}
		}

		// 直接テクスチャアニメーションをリセット
		::glLoadIdentity();
		::glMatrixMode(GL_MODELVIEW);

		// 物理ダメージ表示
		if(cstat->getPhysicsDamage() > 0)
		{
			// 物理ダメージ
			{
				RectF32 vtx(x + w - wPhsDmg, yLf, wPhsDmg, H_LFGAUGE_DEFAULT);
				ColorU8 col(255, 0, 0, 127);
				RendererUtils::draw2dColorRect(rdr, &vtx, &col, &col, &col, &col);
			}
			// 物理ダメージライン
			{
				RectF32 vtx(x + w - wPhsDmg, yLf - 1.0f, 1.0f, H_LFGAUGE_DEFAULT + 2.0f);
				ColorU8 col(255, 0, 0, 255);
				RendererUtils::draw2dColorRect(rdr, &vtx, &col, &col, &col, &col);
			}
		}

		// エネルギー値数値表示
		VcString str;
		rdr->setSolidColor(0, 0, 0, 255);
		EasyStringDrawer::draw(fontEn, VcString::format(&str, "%d", lfene), x + 2.0f, yLf - 7.0f, w, 14, 14, EasyStringDrawer::F_ALIGN_RIGHT, rc);
		rdr->setSolidColor(255, 255, 255, 255);
		EasyStringDrawer::draw(fontEn, VcString::format(&str, "%d", lfene), x + 1.0f, yLf - 7.0f, w, 14, 14, EasyStringDrawer::F_ALIGN_RIGHT, rc);

		yWk += H_LFGAUGE_DEFAULT;
	}

	if(drawMpGauge)
	{
		// 地
		RendererUtils::draw2dColorRect(rdr, x, yWk, w, H_MPGAUGE_DEFAULT, &colBase, &colBase, &colBase, &colBase);

		// 意識深さ表示
		if(cstat->getConcentrateDepth() > 0)
		{
			// 直接テクスチャアニメーションする
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();
			::glTranslatef(s_ofsAnimElLf[IDX_CONS], 0.0f, 0.0f);
			rdr->bindTexture(texGui);

			wrfRest = cstat->getConcentrateDepth() / (f32)CharStat::CONCD_HENL_MIN;
			if(wrfRest > 1.0f) { wrfRest = 1.0f; }
			f32 wWk = wrfRest * w;
			if(wWk >= 1.0f)
			{
				f32 ir = 1.0f - wrfRest;
				u8 r = (u8)(255.0f - (ir * 32.0f));
				u8 g = (u8)(255.0f - (ir * 226.0f));
				const ColorU8 col1(r, g, (u8)(255.0f - (ir * 45.0f)), 255);
				const ColorU8 col2(r, g, (u8)(255.0f - (ir * 86.0f)), 255);
				const ColorU8 col3(r, g, (u8)(255.0f - (ir * 104.0f)), 127);
				const ColorU8 col4(r, g, (u8)(255.0f - (ir * 114.0f)), 127);
				rdr->bindTexture(texGui);
				RendererUtils::draw2dColorTextureRect(rdr, x, yWk + 1.0f, wWk, H_CONSGAUGE_DEFAULT, s_uvElLf[IDX_CONS].x(), s_uvElLf[IDX_CONS].y(), s_uvElLf[IDX_CONS].w() * wrfRest, s_uvElLf[IDX_CONS].h(), &col1, &col2, &col3, &col4);
				///RendererUtils::draw2dColorRect(rdr, x, yWk + 1.0f, wWk, H_CONSGAUGE_DEFAULT - 2.0f, &col1, &col2, &col3, &col4);
			}

			// 直接テクスチャアニメーションをリセット
			::glLoadIdentity();
			::glMatrixMode(GL_MODELVIEW);
		}

		// ため表示
		if(cstat->getChargeCounter() > 0)
		{
			f32 wCc = cstat->getChargeCounter() / (f32)CharStat::CHARGECNT_MAX * w;
			if(wCc >= 1.0f)
			{
				ColorU8 colCcT(255, 255 - (cstat->getChargeCounter() >> 8), 0, 127);
				ColorU8 colCcB(255, colCcT.g(), 0, 223);
				RendererUtils::draw2dColorRect(rdr, x, yWk + H_MPGAUGE_DEFAULT - H_CHARGEGAUGE_DEFAULT, wCc, H_CHARGEGAUGE_DEFAULT, &colCcT, &colCcT, &colCcB, &colCcB);
			}
		}

		yWk += H_MPGAUGE_DEFAULT;
	}

	// 元素エネルギー表示
	ASSERT(cstat->getEleneStat() != 0L);
	StatusDrawer::drawEleneStat(rc, texGui, cstat->getEleneStat(), x, yWk, w, h - (yWk - y), drawDifEleneNum);

#if 0
	// 元素エネルギー表示
	ASSERT(cstat->getEleneStat() != 0L);
	StatusDrawer::draw(rc, texGui, cstat->getEleneStat(), x, yWk, w, H_ENEGAUGE_DEFAULT);

	// 意識深さ表示
	if(isConsPlot)
	{
		f32 yCd = (H_ENEGAUGE_DEFAULT / (f32)Elene::NUM_EL) * cstat->getConcentrateDepth() / (f32)CharStat::CONCD_EL_MAX;
		if(yCd >= H_ENEGAUGE_DEFAULT)
		{
			rdr->setSolidColor(255, 0, 0, 255);
			yCd = (f32)H_ENEGAUGE_DEFAULT;
		}
		else
		{
			rdr->setSolidColor(255, 255, 255, 255);
		}
		rdr->bindTexture(texGui);
		RectF32 rect(x - 5.0f, yWk + H_ENEGAUGE_DEFAULT - yCd - 3.0f, 5.0f, 5.0f);
		RendererUtils::draw2dTextureRect(rdr, &rect, &s_uvConsPlot);
	}

	// ため表示
	if(cstat->getChargeCounter() > 0)
	{
		f32 yCc = cstat->getChargeCounter() / (f32)CharStat::CHARGECNT_MAX * H_ENEGAUGE_DEFAULT;
		if(yCc >= 1.0f)
		{
			rdr->setSolidColor(255, 255 - (cstat->getChargeCounter() >> 8), 0, 255);
			RendererUtils::draw2dLine(rdr, x, yWk + H_ENEGAUGE_DEFAULT, x, yWk + H_ENEGAUGE_DEFAULT - yCc);
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	元素エネルギー状態 2D 描画
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawEleneStat(DrawInfo* di, const RenderCtx* rc, Texture* tex, const EleneStat* eestat, f32 x, f32 y, f32 w, f32 h, bool drawDifEleneNum)
{
	Renderer* rdr = rc->getRenderer();

	// 要素の幅と高さ
	const f32 FL_PAD_X = 3;	// 全体枠に対するパディング
	const f32 FL_PAD_Y = 5;
	f32 padElX = 2;			// 要素間のパディング（デフォルト値）
	f32 wEl = ((w - FL_PAD_X - FL_PAD_X) / Elene::NUM_EL) - padElX - padElX;
	f32 hEl = h - FL_PAD_Y - FL_PAD_Y;
	if(wEl < hEl) { hEl = wEl; }
	else { wEl = hEl; }

	// 地
	ColorU8 col(0, 0, 0, 51);
	RendererUtils::draw2dColorRect(rdr, x, y, w, h, &col);
	rdr->setSolidColor(10, 10, 10, 33);
	RendererUtils::draw2dRectLine(rdr, x, y, w, h);
	//rdr->setSolidColor(255, 255, 255, 255);

	// 最大値を得る
	s32 eeMax = S32_MIN;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		//if(eeMax < eestat->getAfv()->get(i))	{	eeMax = eestat->getAfv()->get(i);	}
		if(eeMax < eestat->getNql()->get(i))	{ eeMax = eestat->getNql()->get(i); }
	}

	// スケールを得る
	if(di->_scaleEne == 0.0f)	// 未計算
	{
		procureDrawInfo(di, eeMax, wEl, hEl);
	}

	rdr->bindTexture(tex);
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成
	::glMatrixMode(GL_MODELVIEW);

	// 各元素エネルギーの幅・高さや位置の計算と現在量の描画
	padElX = (((w - FL_PAD_X - FL_PAD_X) / Elene::NUM_EL) - wEl) * 0.5f;
	f32 xElLeft = x + FL_PAD_X + padElX;
	f32 xEl = xElLeft;
	f32 yEl = y + FL_PAD_Y;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		f32 cql = (f32)eestat->getDfv()->get(i);
		f32 nql = (f32)eestat->getNql()->get(i);
		f32 hCql = cql * di->_scaleEne;			// 現在量の高さ
		f32 wNql = nql * di->_scaleEne;			// 原質量の幅
		if(wNql < 2.0f) { wNql = 2.0f; }
		else if(wNql > wEl) { wNql = wEl; }
		f32 hNql = wNql;						// 原質量の高さ
		f32 hRem = hCql - hNql;					// 原質量に対する余り高さ
		f32 xElU = xEl + ((wEl - wNql) * 0.5f);
		f32 yElU = yEl + ((hEl - hNql) * 0.5f);
		#if 0	// 五角形テスト
			f32 r = h * 0.5f;
			f32 theta = TFW_DPI * (((3 - i) / 5.0f)) - 0.314159f;
			xElU = x + (w * 0.5f) + r * cosf(theta);
			yElU = y + (h * 0.5f) + r * sinf(theta);
		#endif

		// 原質量
		if(cql < nql)	// 現在量が原質量に足りていないときのみ描画
		{
			RendererUtils::draw2dTextureRect(rdr, xElU, yElU, wNql, hNql, s_uvElmNql[i].x(), s_uvElmNql[i].y(), s_uvElmNql[i].w(), s_uvElmNql[i].h());
		}

		// 現在値（２巡目以降は増悪量）
		ColorU8 colWork(223, 223, 223, 255);
		while(hCql > 0.0f)
		{
			f32 hc = hCql < hNql ? hCql : hNql;
			f32 hr = hc / hNql;
			RendererUtils::draw2dColorTextureRect(rdr, xElU, yElU + hNql - hc, wNql, hc, s_uvElmCql[i].x(), s_uvElmCql[i].y() + (H_ELM / H_TEX * (1.0f - hr)), s_uvElmCql[i].w(), s_uvElmCql[i].h() * hr, &colWork, &colWork, &colWork, &colWork);
			hCql -= hNql;
			colWork._v[0] = (colWork._v[0] + 63) / 2;
			colWork._v[1] = (colWork._v[1] + 63) / 2;
			colWork._v[2] = (colWork._v[2] + 63) / 2;
		}

		// 増悪
		f32 rexc = ((cql / nql) - 1.0f) * 0.5f;	// 100% ～ 300% が増悪
		if(rexc > 0.0f)
		{
			if(rexc > 1.0f)	{ rexc = 1.0f; }
			rdr->setAlphaBlendFunc(Renderer::AFUNC_SUB);		// 減算合成
			s32 c = (s32)(rexc * 255.0f);
			ColorU8 colVeil(255, 255, 255, (u8)c);
			RendererUtils::draw2dColorTextureRect(rdr, xElU - 2, yElU - 2, wNql + 4, hNql + 4, s_uvElmVeil[EleneStat::NUM_VEIL].x(), s_uvElmVeil[EleneStat::NUM_VEIL].y(), s_uvElmVeil[EleneStat::NUM_VEIL].w(), s_uvElmVeil[EleneStat::NUM_VEIL].h(), &colVeil, &colVeil, &colVeil, &colVeil);
		}

		// ベール量
		s32 eeVeil = eestat->getVeil(EleneStat::VEIL_WP)->getElene()->get(i);
		if(eeVeil > 0)
		{
			rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);		// 加算合成
			s32 c = (s32)(eeVeil * 255.0f / (f32)Elene::ELEN_DEFAULT);
			if(c > 255) { c = 255; }
			ColorU8 colVeil(255, 255, 255, (u8)c);
			RendererUtils::draw2dColorTextureRect(rdr, xElU - 6.0f, yElU - 6.0f, wNql + 12.5f, hNql + 12.5f, s_uvElmVeil[EleneStat::VEIL_WP].x(), s_uvElmVeil[EleneStat::VEIL_WP].y(), s_uvElmVeil[EleneStat::VEIL_WP].w(), s_uvElmVeil[EleneStat::VEIL_WP].h(), &colVeil, &colVeil, &colVeil, &colVeil);
		}
		eeVeil = eestat->getVeil(EleneStat::VEIL_BR)->getElene()->get(i);
		if(eeVeil > 0)
		{
			rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);		// 加算合成
			s32 c = (s32)(eeVeil * 255.0f / (f32)Elene::ELEN_DEFAULT);
			if(c > 255) { c = 255; }
			ColorU8 colVeil(255, 255, 255, (u8)c);
			RendererUtils::draw2dColorTextureRect(rdr, xElU - 6.0f, yElU - 6.0f, wNql + 12.5f, hNql + 12.5f, s_uvElmVeil[EleneStat::VEIL_BR].x(), s_uvElmVeil[EleneStat::VEIL_BR].y(), s_uvElmVeil[EleneStat::VEIL_BR].w(), s_uvElmVeil[EleneStat::VEIL_BR].h(), &colVeil, &colVeil, &colVeil, &colVeil);
		}

#if 0
		// 数値描画
		rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成
		rdr->setSolidColor(255, 255, 255, 127);
#if 0
		drawTinyNumRightAlign(rc, tex, xEl + wEl + 1, (i & 1) ? yEl + hEl + 7 : yEl + hEl + 1, 1.0f, eestat->getAfv()->get(i) / Elene::ELENE_VALID_FIGURE);
#else
		f32 xt = xEl + wEl;
		f32 yt = y + h - (UV_TINYNUM_H * 0.7f);
		::glTranslatef(xt, yt, 0.0f);
		::glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
		drawTinyNumRightAlign(rc, tex, 0.0f, 0.0f, 1.0f, eestat->getDfv()->get(i) / Elene::ELENE_VALID_FIGURE);
		::glRotatef(-60.0f, 0.0f, 0.0f, 1.0f);
		::glTranslatef(-xt, -yt, 0.0f);
#endif
		rdr->setSolidColor(255, 255, 255, 255);
#endif

		rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成

		xEl += wEl + padElX + padElX;
	}

	f32 hTnum = h - hEl;
	if(hTnum >= (UV_TINYNUM_W * 4.0f))
	{
		f32 xEl = xElLeft;
		f32 yEl = y + FL_PAD_Y;
		f32 yt = y + h - (UV_TINYNUM_H * 0.7f);
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			// 数値描画
			rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成
			rdr->setSolidColor(255, 255, drawDifEleneNum ? 127 : 255, 127);
			f32 xt = xEl + wEl;
			::glTranslatef(xt, yt, 0.0f);
			::glRotatef(XROT_TINYNUM, 0.0f, 0.0f, 1.0f);
			s32 ene = eestat->getDfv()->get(i) + eestat->calcVeilSubtotalElene(i);
			if(drawDifEleneNum) { ene -= eestat->getNql()->get(i); }
			drawTinyNumRightAlign(rc, tex, 0.0f, 0.0f, 1.0f, ene / Elene::ELENE_VALID_FIGURE, drawDifEleneNum);
			::glRotatef(-XROT_TINYNUM, 0.0f, 0.0f, 1.0f);
			::glTranslatef(-xt, -yt, 0.0f);
//			rdr->setSolidColor(255, 255, 255, 255);

			xEl += wEl + padElX + padElX;
		}
	}

#if 0	// 縦テストバージョン
	// 要素の幅と高さ
	f32 PAD = 1;
	f32 wEl = (w / Elene::NUM_EL) - PAD - PAD;
	f32 hEl = h - 12 - PAD - PAD; //; h < wEl ? h : wEl;

	// 地
	ColorU8 col(0, 0, 0, 191);
	RendererUtils::draw2dColorRect(rdr, (f32)x, (f32)y, (f32)w, (f32)h, &col);

	// 最大値を得る
	s32 eeMax = S32_MIN;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(eeMax < eestat->getAfv()->get(i))	{	eeMax = eestat->getAfv()->get(i);	}
		if(eeMax < eestat->getNql()->get(i))	{	eeMax = eestat->getNql()->get(i);	}
	}

	// スケールを得る
	if(di->_scaleEne == 0.0f)	// 未計算
	{
		procureDrawInfo(di, eeMax, wEl, hEl);
	}

	const ColorU8 colImp(64, 64, 64, 64);
	const ColorU8 colVeil(191, 191, 191, 127);

	// 各元素エネルギーの幅・高さや位置の計算と現在量の描画
	f32 rateEleneMax = wEl * di->_scaleEne;
	f32 xEl = x + PAD;
	f32 yEl = y + PAD; //y + ((h - hEl) / 2) + PAD;
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		s32 afv = eestat->getAfv()->get(i);
		s32 nql = eestat->getNql()->get(i);
		//f32 hrCur = afv * di->_scaleEne;	// 現在量の高さレート
		f32 hCur = afv * rateEleneMax;		// 現在量の高さ
		f32 hNql = nql * rateEleneMax;		// 原質量の高さ
		f32 hRem = hCur - hNql;				// 原質量に対する余り高さ
		f32 yNql = yEl + hEl - hNql;		// 原質量の Y 位置
		f32 yRem = yNql;					// 余り高さの Y 位置

		// 現在量
		const ColorU8* colDef = getElementColor((Elene::ElKind)i);
		RendererUtils::draw2dColorRect(rdr, xEl, yEl + hEl - hCur, wEl, hCur, colDef);

		// 増悪量
		s32 eeImp = eestat->getCql()->get(i) - nql;
		if(eeImp > 0)
		{
			f32 hImp = eeImp * rateEleneMax;	// 増悪量の幅
			if(hImp > hRem)
			{
				hImp = hRem;
			}
			if(hImp > 0.0f)
			{
				RendererUtils::draw2dColorRect(rdr, xEl, yRem, wEl, hImp, &colImp, &colImp, &colImp, &colImp);
				yRem += hImp;
				hRem -= hImp;
			}
		}

		// ベール量
		s32 eeVeil = eestat->getVeil(EleneStat::VEIL_WP)->getElene()->get(i) + eestat->getVeil(EleneStat::VEIL_EF)->getElene()->get(i);
		if((eeVeil > 0) && (hRem > 0.0f))
		{
			f32 hVeil = eeVeil * rateEleneMax;	// ベール量の幅
			if(hVeil > hRem)
			{
				hVeil = hRem;
			}
			if(hVeil > 0.0f)
			{
				RendererUtils::draw2dColorRect(rdr, xEl, hRem, wEl, hVeil, &colVeil, &colVeil, &colVeil, &colVeil);
			}
		}

		// 原質量
		const ColorU8* colLit = getElementColorLight((Elene::ElKind)i);
		rdr->setSolidColor(colLit);
		RendererUtils::draw2dLine(rdr, xEl, yNql, xEl + wEl, yNql);

		// 数値描画
		rdr->setSolidColor(255, 255, 255, 127);
		drawTinyNumRightAlign(rc, tex, xEl + wEl + 1, (i & 1) ? yEl + hEl + 7 : yEl + hEl + 1, eestat->getAfv()->get(i) / Elene::ELENE_VALID_FIGURE);

		xEl += wEl + PAD + PAD;
	}

#elif 0	// 旧バージョン（～ ver2.x）
	// 要素の幅と高さ
	f32 hEl = h / Elene::NUM_EL;
	f32 wEl = w;

	// 地
	ColorU8 col(0, 0, 0, 191);
	RendererUtils::draw2dColorRect(rdr, (f32)x, (f32)y, (f32)w, (f32)h, &col);

	// 最大値を得る
	s32 eeMax = S32_MIN;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(eeMax < eestat->getAfv()->get(i))	{	eeMax = eestat->getAfv()->get(i);	}
		if(eeMax < eestat->getNql()->get(i))	{	eeMax = eestat->getNql()->get(i);	}
	}
	
	// スケールを得る
	if(di->_scaleEne == 0.0f)	// 未計算
	{
		procureDrawInfo(di, eeMax, wEl, hEl);
	}

	// 直接テクスチャアニメーションする
	::glMatrixMode(GL_TEXTURE);
	::glLoadIdentity();
	f32 ofsAnimBk = 0.0f;

	// 各元素エネルギーの幅・高さや位置の計算と現在量の描画
	f32 rateEleneMax = wEl * di->_scaleEne;
	f32 yElMax = y + hEl * (Elene::NUM_EL - 1);
	f32 yEl = yElMax;
	f32 yElS = yEl + di->_yOfs;
	f32 hElS = hEl - (di->_yOfs * 2.0f);
	const ColorU8 colImp(64, 64, 64, 64);
	const ColorU8 colVeil(191, 191, 191, 127);
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		s32 afv = eestat->getAfv()->get(i);
		s32 nql = eestat->getNql()->get(i);
		f32 wrCur = afv * di->_scaleEne;	// 現在量の横幅レート
		f32 wCur = afv * rateEleneMax;		// 現在量の幅
		f32 wNql = nql * rateEleneMax;		// 原質量の幅
		f32 wRem = wCur - wNql;				// 原質量に対する余り幅
		f32 xNql = x + wNql;				// 原質量の X 位置
		f32 xRem = xNql;					// 余り幅の X 位置

		// テクスチャアニメーション
		::glTranslatef(s_ofsAnimElLf[i] - ofsAnimBk, 0.0f, 0.0f);
		ofsAnimBk = s_ofsAnimElLf[i];

		// 現在量
		const ColorU8* colDef = getElementColor((Elene::ElKind)i);
		ColorU8 colRt;
		RendererUtils::boostColor(&colRt, colDef, di->_scaleCol);
		ColorU8 colLt(colRt.r(), colRt.g(), colRt.b(), di->_alphaH);
		ColorU8 colRb(colRt.r(), colRt.g(), colRt.b(), di->_alphaV);
		ColorU8 colLb(colRt.r(), colRt.g(), colRt.b(), di->_alphaH);
		rdr->bindTexture(tex);
		RendererUtils::draw2dColorTextureRect(rdr, x, yElS, wCur, hElS, s_uvElLf[i].x(), s_uvElLf[i].y(), s_uvElLf[i].w() * wrCur, s_uvElLf[i].h(), &colLt, &colRt, &colLb, &colRb);

		// 増悪量
		s32 eeImp = eestat->getCql()->get(i) - nql;
		if(eeImp > 0)
		{
			f32 wImp = eeImp * rateEleneMax;	// 増悪量の幅
			if(wImp > wRem)
			{
				wImp = wRem;
			}
			if(wImp > 0.0f)
			{
				RendererUtils::draw2dColorRect(rdr, xRem, yElS, wImp, hElS, &colImp, &colImp, &colImp, &colImp);
				xRem += wImp;
				wRem -= wImp;
			}
		}

		// ベール量
		s32 eeVeil = eestat->getVeil(EleneStat::VEIL_WP)->getElene()->get(i) + eestat->getVeil(EleneStat::VEIL_EF)->getElene()->get(i);
		if((eeVeil > 0) && (wRem > 0.0f))
		{
			f32 wVeil = eeVeil * rateEleneMax;	// ベール量の幅
			if(wVeil > wRem)
			{
				wVeil = wRem;
			}
			if(wVeil > 0.0f)
			{
				RendererUtils::draw2dColorRect(rdr, xRem, yElS, wVeil, hElS, &colVeil, &colVeil, &colVeil, &colVeil);
			}
		}

		// 原質量
		const ColorU8* colLit = getElementColorLight((Elene::ElKind)i);
		rdr->setSolidColor(colLit);
		RendererUtils::draw2dLine(rdr, xNql, yEl, xNql, yEl + hEl);

		yEl -= hEl;
		yElS -= hEl;
	}

	// 直接テクスチャアニメーションをリセット
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);

	// 数値描画
	if(true)
	{
		rdr->setSolidColor(255, 255, 255, 127);
		yEl = yElMax;
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			drawTinyNumRightAlign(rc, tex, x + w, yEl, eestat->getAfv()->get(i) / Elene::ELENE_VALID_FIGURE);
			yEl -= hEl;
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	元素エネルギー状態 2D 描画
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawEleneStat(const RenderCtx* rc, Texture* tex, const EleneStat* eestat, f32 x, f32 y, f32 w, f32 h, bool drawDifEleneNum)
{
	DrawInfo di;
	drawEleneStat(&di, rc, tex, eestat, x, y, w, h, drawDifEleneNum);
}

/*---------------------------------------------------------------------*//**
	元素エネルギー 2D 描画
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawElene(DrawInfo* di, const RenderCtx* rc, Texture* tex, const Elene* elene, f32 x, f32 y, f32 w, f32 h)
{
	Renderer* rdr = rc->getRenderer();

	// 要素の幅と高さ
	f32 FL_PAD_X = 3;	// 全体枠に対するパディング
	f32 FL_PAD_Y = 5;
	f32 EL_PAD_X = 2;	// 要素間のパディング
	f32 wEl = ((w - FL_PAD_X - FL_PAD_X) / Elene::NUM_EL) - EL_PAD_X - EL_PAD_X;
	f32 hEl = wEl;

	// ベースの色
	ColorU8 col(0, 0, 0, 31);
	RendererUtils::draw2dColorRect(rdr, (f32)x, (f32)y, (f32)w, (f32)h, &col);

	// 最大値を得る
	s32 eeMax = S32_MIN;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(eeMax < elene->get(i))	{	eeMax = elene->get(i);	}
	}
	
	// スケールを得る
	if(di->_scaleEne == 0.0f)	// 未計算
	{
		procureDrawInfo(di, eeMax, wEl, hEl);
	}

	rdr->bindTexture(tex);
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// 通常合成
	::glMatrixMode(GL_MODELVIEW);

	// 各元素エネルギー
	f32 xEl = x + (w - ((wEl + FL_PAD_X + EL_PAD_X) * Elene::NUM_EL)) * 0.5f;
	f32 yEl = y + FL_PAD_Y;
	f32 scaleTinyNum = w / 60.0f;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		s32 cql = elene->get(i);
		f32 wCql = cql * di->_scaleEne;
		if(wCql < 2.0f) { wCql = 2.0f; }
		else if(wCql > wEl) { wCql = wEl; }
		f32 hCql = wCql;
		f32 xElU = xEl + ((wEl - wCql) / 2);
		f32 yElU = yEl + ((hEl - hCql) / 2);

		// 元素量
		ColorU8 colWork(223, 223, 223, 255);
		if(hCql > 0.0f)
		{
			RendererUtils::draw2dColorTextureRect(rdr, xElU, yElU, wCql, hCql, s_uvElmCql[i].x(), s_uvElmCql[i].y(), s_uvElmCql[i].w(), s_uvElmCql[i].h(), &colWork, &colWork, &colWork, &colWork);
		}

		// 数値描画
		rdr->setSolidColor(255, 255, 255, 127);
		f32 xt = xEl + wEl;
		f32 yt = y + h - (UV_TINYNUM_H * scaleTinyNum * 0.7f);
		::glTranslatef(xt, yt, 0.0f);
		::glRotatef(XROT_TINYNUM, 0.0f, 0.0f, 1.0f);
		drawTinyNumRightAlign(rc, tex, 0.0f, 0.0f, scaleTinyNum, cql / Elene::ELENE_VALID_FIGURE, false);
		::glRotatef(-XROT_TINYNUM, 0.0f, 0.0f, 1.0f);
		::glTranslatef(-xt, -yt, 0.0f);
		rdr->setSolidColor(255, 255, 255, 255);

		xEl += wEl + EL_PAD_X + EL_PAD_X;
	}

#if 0	// 旧バージョン（～ ver2.x）
	// 要素の幅と高さ
	f32 hEl = h / Elene::NUM_EL;
	f32 wEl = w;

	// 地
	ColorU8 col(0, 0, 0, 191);
	RendererUtils::draw2dColorRect(rdr, (f32)x, (f32)y, (f32)w, (f32)h, &col);

	// 最大値を得る
	s32 eeMax = S32_MIN;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(eeMax < elene->get(i))	{	eeMax = elene->get(i);	}
	}

	// スケールを得る
	if(di->_scaleEne == 0.0f)	// 未計算
	{
		procureDrawInfo(di, eeMax, hEl);
	}

	// 直接テクスチャアニメーションする
	::glMatrixMode(GL_TEXTURE);
	::glLoadIdentity();
	f32 ofsAnimBk = 0.0f;

	// 各元素エネルギー
	const f32 rateEleneMax = wEl * di->_scaleEne;
	f32 yElS = y + (hEl * (Elene::NUM_EL - 1)) + di->_yOfs;
	f32 hElS = hEl - (di->_yOfs * 2.0f);
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		f32 wCur = elene->get(i) * rateEleneMax;					// 現在量の幅

		// テクスチャアニメーション
		::glTranslatef(s_ofsAnimElLf[i] - ofsAnimBk, 0.0f, 0.0f);
		ofsAnimBk = s_ofsAnimElLf[i];

		// 現在量
		const ColorU8* colDef = getElementColor((Elene::ElKind)i);
		ColorU8 colRt;
		RendererUtils::boostColor(&colRt, colDef, di->_scaleCol);
		ColorU8 colLt(colRt.r(), colRt.g(), colRt.b(), di->_alphaH);
		ColorU8 colRb(colRt.r(), colRt.g(), colRt.b(), di->_alphaV);
		ColorU8 colLb(colRt.r(), colRt.g(), colRt.b(), di->_alphaH);
		rdr->bindTexture(tex);
		RendererUtils::draw2dColorTextureRect(rdr, x, yElS, wCur, hElS, s_uvElLf[i].x(), s_uvElLf[i].y(), s_uvElLf[i].w(), s_uvElLf[i].h(), &colLt, &colRt, &colLb, &colRb);

		yElS -= hEl;
	}

	// 直接テクスチャアニメーションをリセット
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);

	// 数値描画
	if(true)
	{
		rdr->setSolidColor(255, 255, 255, 127);
		f32 yEl = (f32)y + hEl * (Elene::NUM_EL - 1);
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			drawTinyNumRightAlign(rc, tex, x + w, yEl, elene->get(i) / Elene::ELENE_VALID_FIGURE);
			yEl -= hEl;
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	元素エネルギー 2D 描画
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawElene(const RenderCtx* rc, Texture* tex, const Elene* elene, f32 x, f32 y, f32 w, f32 h)
{
	DrawInfo di;
	drawElene(&di, rc, tex, elene, x, y, w, h);
}

/*---------------------------------------------------------------------*//**
	極小数字 2D 描画（左寄せ）
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawTinyNumLeftAlign(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, f32 scale, s32 num, bool sign)
{
	rc->getRenderer()->bindTexture(texGui);
	s32 dbuf[16];
	bool minus = false;
	if(num < 0)
	{
		minus = true;
		num = - num;
	}
	s32 p = 0;
	do
	{
		s32 d = num % 10;
		dbuf[p++] = d;
		num /= 10;
		if(p >= 16)	{	break;	}
	}
	while(num > 0);
	f32 w = UV_TINYNUM_W * scale;
	f32 h = UV_TINYNUM_H * scale;
	if(sign)
	{
		const RectF32* uv = &s_uvTinySign[minus ? 1 : 0];
		RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, w, h, uv->x(), uv->y(), uv->w(), uv->h());
		x += w;
	}
	for(int i = 0; i < p; i++)
	{
		const RectF32* uv = &s_uvTinyNum[dbuf[p - i - 1]];
		RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, w, h, uv->x(), uv->y(), uv->w(), uv->h());
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	極小数字 2D 描画（右寄せ）
**//*---------------------------------------------------------------------*/
void StatusDrawer::drawTinyNumRightAlign(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, f32 scale, s32 num, bool sign)
{
	rc->getRenderer()->bindTexture(texGui);
	s32 dbuf[16];
	bool minus = false;
	if(num < 0)
	{
		minus = true;
		num = - num;
	}
	s32 p = 0;
	do
	{
		s32 d = num % 10;
		dbuf[p++] = d;
		num /= 10;
		if(p >= 16)	{	break;	}
	}
	while(num > 0);
	f32 w = UV_TINYNUM_W * scale;
	f32 h = UV_TINYNUM_H * scale;
	x -= w;
	for(int i = 0; i < p; i++)
	{
		const RectF32* uv = &s_uvTinyNum[dbuf[i]];
		RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, w, h, uv->x(), uv->y(), uv->w(), uv->h());
		x -= w;
	}
	if(sign)
	{
		const RectF32* uv = &s_uvTinySign[minus ? 1 : 0];
		RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, w, h, uv->x(), uv->y(), uv->w(), uv->h());
	}
}

/*---------------------------------------------------------------------*//**
	パーティクル用の元素色を計算する
**//*---------------------------------------------------------------------*/
void StatusDrawer::calcEleneColorForParticle(ColorU8* col, const Elene* elene)
{
	s32 m = Elene::ELEN_DEFAULT;
	for(s32 i = Elene::EL_WA; i <= Elene::EL_WI; i++)
	{
		if(m < elene->get(i))
		{
			m = elene->get(i);
		}
	}
	f32 ive = 255.0f / (f32)m;
	u8 r = (u8)(elene->getFire() * ive);
	u8 g = (u8)(elene->getWind() * ive);
	u8 b = (u8)(elene->getWater() * ive);
	col->set(r, g, b, 255);
	/*
	f32 ive = 127.0f / (f32)(Elene::ELEN_DEFAULT * 16);
	s32 r = 127 + (s32)(elene->getFire() * ive);
	s32 g = 127 + (s32)(elene->getWind() * ive);
	s32 b = 127 + (s32)(elene->getWater() * ive);
	if(r > 255) { r = 255; }
	if(g > 255) { g = 255; }
	if(b > 255) { b = 255; }
	col->set((u8)r, (u8)g, (u8)b, 255);
	const s32 COL_BASE = 77;
	const s32 COL_REST = 255 - COL_BASE;
	col->set((u8)COL_BASE, (u8)COL_BASE, (u8)COL_BASE, 255);
	f32 ive = 1.0f / (f32)(Elene::ELEN_DEFAULT * 64);
	s32 caddR = (s32)(COL_REST * elene->getFire() * ive);
	s32 caddG = (s32)(COL_REST * elene->getWind() * ive);
	s32 caddB = (s32)(COL_REST * elene->getWater() * ive);
	if(caddR > COL_REST)
	{
		s32 caddH = (caddR - COL_REST) / 2;
		caddG += caddH;
		caddB += caddH;
	}
	if(caddG > COL_REST)
	{
		s32 caddH = (caddG - COL_REST) / 2;
		caddR += caddH;
		caddB += caddH;
	}
	if(caddB > COL_REST)
	{
		s32 caddH = (caddB - COL_REST) / 2;
		caddR += caddH;
		caddG += caddH;
	}
	if(caddR > COL_REST)	{	caddR = COL_REST;	}
	if(caddG > COL_REST)	{	caddG = COL_REST;	}
	if(caddB > COL_REST)	{	caddB = COL_REST;	}
	col->r() += (u8)caddR;
	col->g() += (u8)caddG;
	col->b() += (u8)caddB;
	*/
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
