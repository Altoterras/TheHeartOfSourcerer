/***********************************************************************//**
 *	PrpzlPerf_Enlight.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/11.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PrpzlPerf_Enlight.h"

// Friends
#include "PrpzlOperationData.h"
#include "PurePuzzle.h"
#include "../common/CalcUtils.h"
#include "../file/GameFileIdDef.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameRenderCtx.h"
#include "../gui/GameFontSet.h"

// External
#include "../../tfw/Tfw.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../tfw/gcmn/EasyStringDrawer.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

//==========================================================================
// PrpzlPerf_Enlight 定数

const f32 PrpzlPerf_Enlight::CELL_XW		= 100.0f;			// セルの横幅
const f32 PrpzlPerf_Enlight::CELL_YH		= 100.0f;			// セルの縦幅

#define X_RESET_BTN		360
#define Y_RESET_BTN		200
#define W_RESET_BTN		60
#define H_RESET_BTN		20

#define X_PRESET_BTN	360
#define Y_PRESET_BTN	230
#define W_PRESET_BTN	60
#define H_PRESET_BTN	20

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PrpzlPerf_Enlight メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PrpzlPerf_Enlight::PrpzlPerf_Enlight()
	: _pzlRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PrpzlPerf_Enlight::~PrpzlPerf_Enlight()
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool PrpzlPerf_Enlight::begin(PurePuzzle* pzlRef)
{
	// 参照を得る
	_pzlRef = pzlRef;

	// モード別開始処理
	switch(_pzlRef->getOpData()->_conf._pzlmode)
	{
	case PrpzlOperationData::PZLMODE_QUEST:
		// プリセット配列読み込み
		presetCells();
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::end()
{
}

/*---------------------------------------------------------------------*//**
	ユーザー操作処理
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::exec(const ExecCtx* ec)
{
	// デバッグインターフェイス
	{
		TouchPanel* touchp = ((GameExecCtx*)ec)->getTouchPanel();
		PointF32 ptNow;
		if(!touchp->isReacted() && touchp->isTapRelease(&ptNow, 0L))
		{
			// ゲームリセット
			if((X_RESET_BTN <= ptNow.x()) && (Y_RESET_BTN <= ptNow.y()) && (ptNow.x() <= (X_RESET_BTN + W_RESET_BTN)) && (ptNow.y() <= (Y_RESET_BTN + H_RESET_BTN)))
			{
				_pzlRef->reset();
				return;
			}
			// プリセット
			else if((X_PRESET_BTN <= ptNow.x()) && (Y_PRESET_BTN <= ptNow.y()) && (ptNow.x() <= (X_PRESET_BTN + W_PRESET_BTN)) && (ptNow.y() <= (Y_PRESET_BTN + H_PRESET_BTN)))
			{
				_pzlRef->reset();
				presetCells();
				return;
			}
		}
	}

	if(_pzlRef->getTurn() == PurePuzzle::TURN_USER)
	{
		TouchPanel* touchp = ((GameExecCtx*)ec)->getTouchPanel();
		PointF32 ptiNow;
		if(!touchp->isReacted() && touchp->isTapRelease(&ptiNow, 0L))
		{
			Vector3F pos3d;
			Vector2F ptfNow((f32)ptiNow.x(), (f32)ptiNow.y());
			CalcUtils::calcScreenPosAnd3dZTo3dPos(&pos3d, &ptfNow, 0.0f);
			TRACE("pos {%f, %f, %f}\n", pos3d.x(), pos3d.y(), pos3d.z());

			s32 ih = (s32)((pos3d.x() + (PurePuzzle::NUM_CELL_H / 2 * CELL_XW)) / CELL_XW);
			s32 iv = (s32)((pos3d.y() + (PurePuzzle::NUM_CELL_V / 2 * CELL_YH)) / CELL_YH);
			if((0 <= ih) && (ih < PurePuzzle::NUM_CELL_H) && (0 <= iv) && (iv < PurePuzzle::NUM_CELL_V))
			{
				PurePuzzle::Cell* c = _pzlRef->cell(ih, iv);
				if(c->_elt == PurePuzzle::ELT_AE)
				{
					c->_elt = _pzlRef->getNextChipElt(0);
					c->_hi = false;
					c->resetAnim();

					_pzlRef->nextChip();		// 次のチップを決定
					_pzlRef->turnToAutoAct();	// 自動アクションへ移行
				}
			}
		}
	}

}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::render(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();
	rdr->setAlphaBlend(true);							// アルファブレンドあり
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// アルファファンクションを通常に
	rdr->setFog(false);									// フォグを無効に
	///rdr->setCulling(false);							// カリングを無効に

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_COLOR_ARRAY);

	f32 x, y;
	ColorU8 col1, col2;
	for(int ih = 0; ih < PurePuzzle::NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < PurePuzzle::NUM_CELL_V; iv++)
		{
			x = CELL_XW * (ih - (PurePuzzle::NUM_CELL_H / 2));
			y = CELL_YH * (iv - (PurePuzzle::NUM_CELL_V / 2));

			// ベースタイル描画
			{
				u8 colTile = ((ih + iv) & 1) ? 255 : 191;
				renderSubBlock(x, y, 0.0f, CELL_XW, CELL_YH, colTile, colTile, colTile, 255);
			}

			const PurePuzzle::Cell* c = _pzlRef->getCell(ih, iv);
			if((c->_elt == PurePuzzle::ELT_AE) && (c->_anim._eltPrev == PurePuzzle::ELT_AE))	{	continue;	}

			y += c->_anim._fcntFall * (CELL_YH / PurePuzzle::FCNT_FALL_MAX);

			// 色などの算出
			getEltColor(&col1, c->_elt, c->_hi);
			if((c->_hi != c->_anim._hiPrev) || (c->_elt != c->_anim._eltPrev))
			{
				getEltColor(&col2, c->_anim._eltPrev, c->_anim._hiPrev);
				f32 rate = _pzlRef->getAutoActFrameCount() / PurePuzzle::FCNT_ANIM_MAX;
				f32 rrate = 1.0f - rate;
				for(int i = 0; i < 3; i++)
				{
					s32 v = (s32)((rrate * col1._v[i]) + (rate * col2._v[i]));
					if(v < 0)			{	v = 0;		}
					else if(v > 255)	{	v = 255;	}
					col1._v[i] = (u8)v;
				}
			}

			// ブロック描画
			{
				f32 m = c->_hi ? 2.0f : 12.0f;
				renderSubBlock(x + m, y + m, 0.1f, CELL_XW - m - m, CELL_YH - m - m, col1.r(), col1.g(), col1.b(), col1.a());
			}

			// フラッシュ
			u8 b = (c->_anim._eltPrev == PurePuzzle::ELT_WI) ? 0 : 255;
			if(c->_anim._fcntFlush1 > 0.0f)
			{
				f32 rate = c->_anim._fcntFlush1 / PurePuzzle::FCNT_FLUSH;
				renderSubBlock(x, y, 0.2f, CELL_XW, CELL_YH, 255, 255, b, (u8)(255.0f * rate));
			}
			if(c->_anim._fcntFlush2 > 0.0f)
			{
				f32 rate = c->_anim._fcntFlush2 / PurePuzzle::FCNT_FLUSH;
				renderSubBlock(x, y, 0.2f, CELL_XW, CELL_YH, 255, 0, b, (u8)(255.0f * rate));
			}
		}
	}

	::glDisableClientState(GL_COLOR_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::draw(const RenderCtx* rc)
{
	VcString str;
	Font* font = ((GameRenderCtx*)rc)->getFontSet()->getFont(GameFontSet::JP);

	// 連鎖
	EasyStringDrawer::draw(font, VcString::format(&str, "Chain: %d", _pzlRef->getChainCount()), 360, 80, 16, rc);

	// スコア
	EasyStringDrawer::draw(font, VcString::format(&str, "Score: %d", _pzlRef->getScore()), 360, 100, 16, rc);

	// 次のブロック
	{
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);
		EasyStringDrawer::draw(font, "Next:", 360, 130, 16, rc);
		ColorU8 col;
		getEltColor(&col, _pzlRef->getNextChipElt(0), false);
		rc->getRenderer()->setSolidColor(&col);
		RendererUtils::draw2dRect(rc->getRenderer(), 395, 124, 20, 20);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);
	}

	// 待て
	switch(_pzlRef->getTurn())
	{
	case PurePuzzle::TURN_AA:	EasyStringDrawer::draw(font, "*** Wait... ***", 360, 150, 16, rc);	break;
	case PurePuzzle::TURN_USER:	EasyStringDrawer::draw(font, "> User Op.", 360, 150, 16, rc);		break;
	}

	// リセットボタン
	{
		rc->getRenderer()->setSolidColor(127, 127, 127, 255);
		RendererUtils::draw2dRect(rc->getRenderer(), X_RESET_BTN, Y_RESET_BTN, W_RESET_BTN, H_RESET_BTN);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);
		EasyStringDrawer::draw(font, "reset", X_RESET_BTN + 4, Y_RESET_BTN, 16, rc);
	}

	// プリセットボタン
	{
		rc->getRenderer()->setSolidColor(127, 127, 127, 255);
		RendererUtils::draw2dRect(rc->getRenderer(), X_PRESET_BTN, Y_PRESET_BTN, W_PRESET_BTN, H_PRESET_BTN);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);
		EasyStringDrawer::draw(font, "preset", X_PRESET_BTN + 4, Y_PRESET_BTN, 16, rc);
	}
}

/*---------------------------------------------------------------------*//**
	3D ブロック描画
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::renderSubBlock(f32 x, f32 y, f32 z, f32 w, f32 h, u8 r, u8 g, u8 b, u8 a)
{
	const GLfloat vtxs[] =
	{
		x,		y,		z,
		x + w,	y,		z,
		x,		y + h,	z,
		x + w,	y + h,	z,
	};
	const GLubyte cols[] =
	{
		r,	g,	b,	a,
		r,	g,	b,	a,
		r,	g,	b,	a,
		r,	g,	b,	a,
	};
	::glVertexPointer(3, GL_FLOAT, 0, vtxs);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, cols);
	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*---------------------------------------------------------------------*//**
	元素タイプの色を得る
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::getEltColor(ColorU8* col, u8 elt, bool isHi)
{
	switch(elt)
	{
	case PurePuzzle::ELT_AE:
		if(isHi)	{	col->set(255, 255, 255, 255);	}	else	{	col->set(191, 191, 191, 255);	}
		break;
	case PurePuzzle::ELT_WI:
		if(isHi)	{	col->set(150, 222,  92, 255);	}	else	{	col->set( 81, 210,  77, 255);	}
		break;
	case PurePuzzle::ELT_FI:
		if(isHi)	{	col->set(220,  25,  25, 255);	}	else	{	col->set(170,  34,  34, 255);	}
		break;
	case PurePuzzle::ELT_WA:
		if(isHi)	{	col->set( 45,  96, 160, 255);	}	else	{	col->set( 58,  70, 128, 255);	}
		break;
	case PurePuzzle::ELT_SO:
		if(isHi)	{	col->set(125, 108, 100, 255);	}	else	{	col->set( 95,  78,  31, 255);	}
		break;
	default:
		col->set( 0,  0,  0, 255);
		break;
	}
}

/*---------------------------------------------------------------------*//**
	プリセット配列呼び出し
**//*---------------------------------------------------------------------*/
void PrpzlPerf_Enlight::presetCells()
{
	#define CELL(_ih_, _iv_, _elt_) _pzlRef->cell((_ih_), (_iv_))->_elt = _elt_

	// ファイルを読み込む
	u32 fileid = PUREPUZZLE_PZLQ_TEST_TXT;
	switch(_pzlRef->getOpData()->_conf._pzlmode)
	{
	case PrpzlOperationData::PZLMODE_QUEST:
		fileid = PUREPUZZLE_PZLQ01_TXT + _pzlRef->getOpData()->_conf._questNo - 1;
		break;
	}
	if(fileid == 0)
	{
		return;
	}
	IdFile file;
	if(!file.open(fileid))
	{
		return;
	}

	// セル配列データを読み込む
	char c = 0;
	for(int iv = PurePuzzle::NUM_CELL_V - 1; iv >= 0; iv--)
	{
		for(int ih = 0; ih < PurePuzzle::NUM_CELL_H; ih++)
		{
			file.read(&c, sizeof(char));
			CELL(ih, iv, (u8)(c - '0'));
		}

		do{	file.read(&c, sizeof(char)); }while(c != '\n');
	}

	do{	file.read(&c, sizeof(char)); }while(c != '\n');

	// 落下チップ配列データを読み込む
	file.read(&c, sizeof(char));
	_pzlRef->nextChip((u8)(c - '0'));
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
