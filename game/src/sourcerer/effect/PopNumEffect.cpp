/***********************************************************************//**
 *	PopNumEffect.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/12.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// PopNum

// Self
#include "PopNumEffect.h"

// Friends
#include "../body/GameExecCtx.h"
#include "../gui/GameGui.h"
#include "../unit/Unit.h"

// External
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// ポップ情報の表示フレーム数
#define FRAMECNT_POP_DISP			(2 * FRAMERATE)	// 2 秒
#define FRAMECNT_POP_DISP_DTS		(1 * FRAMERATE)	// アルファで消え始めるフレーム：1 秒

// ポップ数字の UV 情報
#define W_TEX				1024.0f
#define H_TEX				1024.0f
#define W_TEX_INV			(1.0f / W_TEX)
#define H_TEX_INV			(1.0f / W_TEX)
#define W_POPNUM			10
#define W_POPNUM_MISS		33
#define H_POPNUM			11
#define XPAD_POPNUM			(-2)
#define UV_POPNUM_U			41
#define UV_POPNUM_V			884
#define UV_POPNUM_W			10
#define UV_POPNUM_W_PAD		1
#define UV_POPNUM_H			11
#define UV_POPNUM_MISS_W	33
static const f32 s_uvPopNum[11][4] =
{
	{	(UV_POPNUM_U + ( 0 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 1 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 2 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 3 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 4 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 5 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 6 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 7 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 8 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + ( 9 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
	{	(UV_POPNUM_U + (10 * (UV_POPNUM_W + UV_POPNUM_W_PAD))) * W_TEX_INV, UV_POPNUM_V * H_TEX_INV, UV_POPNUM_MISS_W * W_TEX_INV, UV_POPNUM_H * H_TEX_INV	},
};

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Conductor メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	数値ポップをキューに貯める
**//*---------------------------------------------------------------------*/
void PopNumEffect::on(s32 num, PopNumEffect::Kind kind, const Unit* unitTrg)
{
	// ポップ情報を作成
	PopNum* pop = new PopNum();
	pop->_num = num;
	pop->_kind = (Kind)kind;
	pop->_unitTrg = unitTrg;

	// リストに追加
	_listPop->addTail(pop);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PopNumEffect::PopNumEffect()
	: _listPop(0L)
	, _isSelfHide(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PopNumEffect::~PopNumEffect()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PopNumEffect::create()
{
	// リストの作成
	ASSERT(_listPop == 0L);
	_listPop = new List<PopNum*>(true);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PopNumEffect::destroy()
{
	// リストの削除
	delete _listPop;
	_listPop = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void PopNumEffect::exec(const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// 操作ロック時は処理を進めない
	bool isHide = gec->isLockOperating();	// 操作ロック時
	if(_isSelfHide != isHide)		// 状態が変更された
	{
		_isSelfHide = isHide;

		// 自動隠しの場合、表示中の数値は終える
		for(ListIterator<PopNum*> it = _listPop->iterator(); it.has(); it.next())
		{
			PopNum* pop = it.object();
			if(pop->_pos->x() == F32_PMAX)	{	continue;	}
			pop->_fcnt = FRAMECNT_POP_DISP;
		}
	}
	if(_isSelfHide)
	{
		return;
	}

	// ポップリストの処理
	Tfw* tfw = Game::getGame()->getTfw();						ASSERT(tfw != 0L);
	View* view = tfw->getView();								ASSERT(view != 0L);
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();					ASSERT(sgm != 0L);
	Camera* cam = sgm->getVisibleCamera();
	if(cam != 0L)
	{
		ListIterator<PopNum*> itCur, itNext;
		itCur = _listPop->iterator();
		while(itCur.has())
		{
			// 削除に対応するため、次を得ておく
			itNext = itCur;
			itNext.next();

			PopNum* pop = itCur.object();

			// フレームカウンタ更新
			pop->_fcnt += ec->getDeltaFrame();
			if(pop->_fcnt >= FRAMECNT_POP_DISP)
			{
				// リストから削除する
				_listPop->removeNode(itCur.node());
			}
			else
			{
				// スクリーン座標を計算
				do
				{
					//pop->_pos->x() = F32_PMAX;
					if(pop->_unitTrg == 0L)				{	break;	}
					if(!pop->_unitTrg->isEnable())		{	break;	}	// 無効なユニットは除外
					if(!pop->_unitTrg->isEnableFocus())	{	break;	}	// フォーカスされないユニットは除外
					if(pop->_unitTrg->isHidden())		{	break;	}	// 隠し属性ユニットは除外
					if(pop->_unitTrg->getUnitType() == UNITTYPE_EQUIPMENT_ITEM)	{	break;	}	// 所持品は除外
					if(pop->_unitTrg->getCenterPos()->z() >= cam->getLoc()->z())	{	break;	}	// カメラより手前は除外
					Vector2F vScr;
					Gcalc::conv3dPosToScreenPos(&vScr, pop->_unitTrg->getCenterPos(), cam, view);
					const f32 W_NUM = (W_POPNUM + XPAD_POPNUM) * 5;
					const f32 H_NUM = H_POPNUM;
					if(vScr.x() < W_NUM) { vScr.x() = W_NUM; }
					else if(vScr.x() > (Game::getGame()->getLogicalWidth() - W_NUM)) { vScr.x() = Game::getGame()->getLogicalWidth() - W_NUM; }
					if(vScr.y() < W_NUM) { vScr.y() = H_NUM; }
					else if(vScr.y() > (Game::getGame()->getLogicalHeight() - H_NUM)){ vScr.y() = Game::getGame()->getLogicalHeight() - H_NUM; }
					pop->_pos->copy(&vScr);
				}
				while(false);
			}

			// 次のリストノードへ
			itCur = itNext;
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void PopNumEffect::draw(const RenderCtx* rc)
{
	if(_isSelfHide) { return; }

	Texture* texGui = Game::getGame()->getGui()->getGuiTexture();
	for(ListIterator<PopNum*> it = _listPop->iterator(); it.has(); it.next())
	{
		const PopNum* pop = it.object();
		if(pop->_pos->x() == F32_PMAX)	{	continue;	}

		u8 alpha = 255;
		if(pop->_fcnt > FRAMECNT_POP_DISP_DTS)
		{
			alpha = (u8)(255.0f - ((pop->_fcnt - FRAMECNT_POP_DISP_DTS) / (f32)(FRAMECNT_POP_DISP - FRAMECNT_POP_DISP_DTS)) * 255.0f);
		}

		f32 x = pop->_pos->x();
		f32 y = pop->_pos->y();

		// 画面外判定
		if(x < - (W_POPNUM + XPAD_POPNUM) * 5)		{	continue;	}
		if(x >= Game::getGame()->getLogicalWidth())	{	continue;	}
		if(y < - H_POPNUM)							{	continue;	}
		if(y >= Game::getGame()->getLogicalHeight()){	continue;	}

		// 種別ごとの描画
		switch(pop->_kind)
		{
		case K_NULL:
		case K_ENE_DMG:
			///rc->getRenderer()->setSolidColor(255, 240, 201, alpha);
			rc->getRenderer()->setSolidColor(250, 234, 142, alpha);
			y += - 5.0f + (pop->_fcnt - (FRAMECNT_POP_DISP / 2)) * 0.4f;
			drawPopNum(rc, texGui, x, y, pop->_num, ALIGN_RIGHT);
			break;
		case K_ENE_RECV:
			rc->getRenderer()->setSolidColor(100, 205, 100, alpha);
			x -= 10.0f;
			y -= 5.0f + pop->_fcnt * 0.2f;
			drawPopNum(rc, texGui, x, y, pop->_num, ALIGN_RIGHT);
			break;
		case K_PHYS_DMG:
			rc->getRenderer()->setSolidColor(227, 55, 55, alpha);
			y += 20.0f + (pop->_fcnt - (FRAMECNT_POP_DISP / 2)) * 0.5f;
			drawPopNum(rc, texGui, x, y, pop->_num, ALIGN_LEFT);
			break;
		case K_PHYS_RECV:
			rc->getRenderer()->setSolidColor(154, 220, 245, alpha);
			y -= - 20.0f + pop->_fcnt * 0.3f;
			drawPopNum(rc, texGui, x, y, pop->_num, ALIGN_LEFT);
			break;
		case K_MISS:
			rc->getRenderer()->setSolidColor(255, 255, 255, alpha);
			drawPopNumMiss(rc, texGui, x, y, ALIGN_CENTER);
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	ポップ数字の 2D 描画
**//*---------------------------------------------------------------------*/
void PopNumEffect::drawPopNum(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, u32 num, PopNumEffect::Align align)
{
	rc->getRenderer()->bindTexture(texGui);
	u32 dbuf[16];
	s32 p = 0;
	do
	{
		u32 d = num % 10;
		dbuf[p++] = d;
		num /= 10;
		if(p >= 16)	{	break;	}
	}
	while(num > 0);
	switch(align)
	{
	case ALIGN_RIGHT:	x -= p * (W_POPNUM + XPAD_POPNUM);			break;
	case ALIGN_CENTER:	x -= (p * (H_POPNUM + XPAD_POPNUM)) / 2;	break;
	}
	for(int i = 0; i < p; i++)
	{
		const f32* uv = s_uvPopNum[dbuf[p - i - 1]];
		RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, W_POPNUM, H_POPNUM, uv[0], uv[1], uv[2], uv[3]);
		x += UV_POPNUM_W + XPAD_POPNUM;
	}
}

/*---------------------------------------------------------------------*//**
	ポップ MISS 表示の 2D 描画
**//*---------------------------------------------------------------------*/
void PopNumEffect::drawPopNumMiss(const RenderCtx* rc, Texture* texGui, f32 x, f32 y, PopNumEffect::Align align)
{
	rc->getRenderer()->bindTexture(texGui);
	switch(align)
	{
	case ALIGN_RIGHT:	x -= UV_POPNUM_MISS_W;		break;
	case ALIGN_CENTER:	x -= UV_POPNUM_MISS_W / 2;	break;
	}
	const f32* uv = s_uvPopNum[10];
	RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, W_POPNUM_MISS, H_POPNUM, uv[0], uv[1], uv[2], uv[3]);
}

//==========================================================================
// Conductor::PopNum メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PopNumEffect::PopNum::PopNum()
	: _num(0)
	, _kind(K_NULL)
	, _fcnt(0.0f)
	, _unitTrg(0L)
	, _pos(new Vector2F(F32_PMAX, F32_PMAX))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PopNumEffect::PopNum::~PopNum()
{
	delete _pos;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
