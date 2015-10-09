/***********************************************************************//**
 *	AriaWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AriaWindow.h"

// Friends
#include "../../CharFronts.h"
#include "../../GameGui.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../magic/MagicCluster.h"
#include "../../../magic/MagicSys.h"
#include "../../../souma/Souma.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusDrawer.h"
#include "../../../unit/char/CharUnit.h"

// External
#include "../../../../etk/particle/Particle.h"
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/g2d/TiledRectDraw.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/g3d/sg/node/implement/Model.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/txtbox/TxtBox.h"
#include "../../../../tfw/string/StringUtils.h"
#include "../../../../tfw/txt/implement/SjisTxt.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 位置
#define X_LEFTP		18	//50
#define Y_LEFTP		70
#define X_RIGHTP	270
#define Y_RIGHTP	70

// テキストのパディング
#define L_PAD_TXT	10
#define T_PAD_TXT	10
#define R_PAD_TXT	10
#define B_PAD_TXT	20

// テクスチャチップの大きさ
#define W_TEX		(1024.0f)
#define H_TEX		(1024.0f)
#define W_TEX_CHIP	(16)
#define H_TEX_CHIP	(16)

// 地
#define UV_BG_BASE_U	0.0f
#define UV_BG_BASE_V	992.0f
#define UV_BG_BASE_W	32.0f
#define UV_BG_BASE_H	32.0f
#define W_BG_BASE	16.0f
#define H_BG_BASE	16.0f
#define UV_BG_SIDE_U	32.0f
#define UV_BG_SIDE_V	992.0f
#define UV_BG_SIDE_W	32.0f
#define UV_BG_SIDE_H	32.0f
#define W_BG_SIDE	16.0f
#define H_BG_SIDE	16.0f
#define UV_BG_CRNR_U	64.0f
#define UV_BG_CRNR_V	992.0f
#define UV_BG_CRNR_W	32.0f
#define UV_BG_CRNR_H	32.0f
#define W_BG_CRNR	16.0f
#define H_BG_CRNR	16.0f
#define UV_BG_HORN_U	96.0f
#define UV_BG_HORN_V	992.0f
#define UV_BG_HORN_W	32.0f
#define UV_BG_HORN_H	32.0f
#define W_BG_HORN	32.0f
#define H_BG_HORN	32.0f

// ブレイクボタン
#define L_PAD_BBTN		(-16)
#define T_PAD_BBTN		(-8)
#define W_BBTN			(24)
#define H_BBTN			(24)
#define UV_BBTN_NRM		128.0f / W_TEX, 992.0f / H_TEX, 32.0f / W_TEX, 32.0f / H_TEX
#define UV_BBTN_HVR		160.0f / W_TEX, 992.0f / H_TEX, 32.0f / W_TEX, 32.0f / H_TEX
#define MARGIN_H_BBTN	(8)
#define MARGIN_V_BBTN	(8)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AriaWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	モードを設定する
**//*---------------------------------------------------------------------*/
void AriaWindow::setMode(ModeKind mode)
{
	if(_mode == mode)	{	return;	}
	_mode = mode;

	// モード変更
	if(_mode == MODE_RAW)
	{
		deleteSpellModeObject();
		createRawModeObject();
		Game::getGame()->setOptionFlags(Game::OPTF_ARIA_WINDOUW_RAW_SRC, true);
	}
	else if(_mode == MODE_SPELL)
	{
		deleteRawModeObject();
		createSpellModeObject();
		Game::getGame()->setOptionFlags(Game::OPTF_ARIA_WINDOUW_RAW_SRC, false);
	}
}

/*---------------------------------------------------------------------*//**
	ソースコードが空かを得る
**//*---------------------------------------------------------------------*/
bool AriaWindow::isEmptySourceCode() const
{
	if(_mode == MODE_RAW)
	{
		if(_txtboxSrc == 0L)	{	return true;	}
		return _txtboxSrc->getText()->getRawCount() <= 0;
	}
	else if(_mode == MODE_SPELL)
	{
		if(_strSpell == 0L)		{	return true;	}
		return _strSpell->getLength() <= 0;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ソーステキストを設定する
**//*---------------------------------------------------------------------*/
void AriaWindow::setSourceCode(const VcString* str, bool isCursorReset)
{
	if(_mode == MODE_RAW)
	{
		if(_txtboxSrc == 0L)	{	return;		}
		Txt* txt = _txtboxSrc->text(); ASSERT(txt != 0L);
		txt->setSingleString(str);
		_txtboxSrc->setText(txt, isCursorReset);
	}
	else if(_mode == MODE_SPELL)
	{
		_strSpell->emptyEasy();
		_strdrawSpell->setString(_strSpell);
	}
}

/*---------------------------------------------------------------------*//**
	アクティブコード位置を設定する
**//*---------------------------------------------------------------------*/
void AriaWindow::setActiveCode(u32 idxBegin, u32 idxEnd)
{
	if(_mode == MODE_RAW)
	{
		if(_txtboxSrc == 0L)		{	return;		}
		_txtboxSrc->setActiveText(idxBegin, idxEnd);
	}
	else if(_mode == MODE_SPELL)
	{
		if(_strdrawSpell == 0L)		{	return;		}
		const VcString* code = getSourceCode();
		VcString str;
		if(code != 0L)
		{
			str = code->substring(idxBegin, idxEnd);
		}
		if(str.getLength() <= 0)
		{
			return;
		}
		_strSpell->add(&str);
		_strSpell->add(" ");
		// 文字列描画にコード文字列を設定する
		s32 indexDrawStart = _strdrawSpell->getDrawStartStringIndex();	// スクロール結果としての文字列描画開始インデックス
		if((_strdrawSpell->isVerticalScrollAnimationEnd()) && (indexDrawStart > 0))	// アニメーションしておらず描画していない行がある
		{
			_strSpell->remove(0, indexDrawStart);							// 不要な行を削除する
			_strdrawSpell->setString(_strSpell);							// 文字列再設定
		}
		else
		{
			_strdrawSpell->updateString();
		}
		// スクロール処理
		SizeF32 size;
		_strdrawSpell->drawTest(&size);
		f32 sh = _strdrawSpell->getRectangle()->h();
		if(sh < size.h())
		{
			f32 ch = (f32)_strdrawSpell->getCharDefaultHeight();
			s32 numOverLine = (s32)((size.h() + ch - 1 - sh) / ch);
			_strdrawSpell->setDrawStartStrLine(numOverLine);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ソウマを設定する
**//*---------------------------------------------------------------------*/
void AriaWindow::setSouma(Souma* souma)
{
	_soumaRef = souma;
}

/*---------------------------------------------------------------------*//**
	矩形設定
**//*---------------------------------------------------------------------*/
void AriaWindow::setRectangle(const RectF32* rect)
{
	Panel::setRectangle(rect);

	// 文字列描画も移動
	RectF32 restStr(*rect);
	restStr.x() += L_PAD_TXT;
	restStr.y() += T_PAD_TXT;
	restStr.w() -= L_PAD_TXT + R_PAD_TXT;
	restStr.h() -= T_PAD_TXT + B_PAD_TXT;
	if(_txtboxSrc != 0L)			{	_txtboxSrc->setRectangle(&restStr);		}
	else if(_strdrawSpell != 0L)	{	_strdrawSpell->setRectangle(&restStr);	}

	// 中断ボタンも移動
	if(_pnlBreak != 0L)
	{
		RectF32 rectBb(*rect);
		rectBb.x() = rectBb.x() + rectBb.w() + L_PAD_BBTN;
		rectBb.y() = rectBb.y() + T_PAD_BBTN;
		rectBb.w() = W_BBTN;
		rectBb.h() = H_BBTN;
		_pnlBreak->setRectangle(&rectBb);
	}

	// ゲージ位置も移動
	_ptGaugePos->set(rect->x() + rect->w() - (s32)StatusDrawer::W_GAUGE_DEFAULT, rect->y() + rect->h());
}

/*---------------------------------------------------------------------*//**
	位置設定
**//*---------------------------------------------------------------------*/
void AriaWindow::setPosition(const PointF32* pt)
{
	Panel::setPosition(pt);

	const RectF32* rectThis = this->getRectangle();

	// 文字列描画も移動
	RectF32 restStr(*rectThis);
	restStr.x() += L_PAD_TXT;
	restStr.y() += T_PAD_TXT;
	restStr.w() -= L_PAD_TXT + R_PAD_TXT;
	restStr.h() -= T_PAD_TXT + B_PAD_TXT;
	if(_txtboxSrc != 0L)			{	_txtboxSrc->setRectangle(&restStr);		}
	else if(_strdrawSpell != 0L)	{	_strdrawSpell->setRectangle(&restStr);	}

	// 中断ボタンも移動
	if(_pnlBreak != 0L)
	{
		RectF32 rectBb(*rectThis);
		rectBb.x() = rectBb.x() + rectBb.w() + L_PAD_BBTN;
		rectBb.y() = rectBb.y() + T_PAD_BBTN;
		rectBb.w() = W_BBTN;
		rectBb.h() = H_BBTN;
		_pnlBreak->setRectangle(&rectBb);
	}

	// ゲージ位置も移動
	_ptGaugePos->set(rectThis->x() + rectThis->w() - (s32)StatusDrawer::W_GAUGE_DEFAULT, rectThis->y() + rectThis->h());
}

/*---------------------------------------------------------------------*//**
	中断ボタンの有効／無効化
**//*---------------------------------------------------------------------*/
void AriaWindow::setEnableBreakButton(bool isEnable)
{
	if(_pnlBreak != 0L)
	{
		_pnlBreak->setEnable(isEnable);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AriaWindow::AriaWindow()
	: _mode(MODE_NULL)
	, _texGuiRef(0L)
	, _fontRef(0L)
	, _trdrawBg(0L)
	, _txtboxSrc(0L)
	, _strdrawSpell(0L)
	, _pnlBreak(0L)
	, _strSpell(0L)
	, _isDone(false)
	, _isInputDone(false)
	, _isUserOp(false)
	, _ptTapStartPos(0L)
	, _ptGaugePos(0L)
	, _hnumChip(0)
	, _vnumChip(0)
	, _soumaRef(0L)
#if OLD_ARIAWND_POSITION
	, _isLeftPosition(false)
	, _ecntChPosition(0)
#endif
	, _isDefaultPosition(false)
	, _alphaFadeCol(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AriaWindow::~AriaWindow()
{
	ASSERT(_txtboxSrc == 0L);
	ASSERT(_pnlBreak == 0L);
	ASSERT(_ptTapStartPos == 0L);
	ASSERT(_ptGaugePos == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool AriaWindow::create(Font* fontRef, Texture* texRef, ModeKind mode)
{
	// チップ数を決定する
	_hnumChip = 11;
	_vnumChip = 8;

	RectF32 rect(X_LEFTP, Y_LEFTP, (f32)(_hnumChip * W_TEX_CHIP), (f32)(_vnumChip* H_TEX_CHIP));
	RectF32 uv;

	// ウインドウの作成
	if(!Window::create(&rect))
	{
		return false;
	}

	// 背景タイルの作成
	TiledRectDraw* trdraw = new TiledRectDraw();
	if(!trdraw->create(texRef))
	{
		delete trdraw;
		ASSERT(false);
		return false;
	}
	_trdrawBg = trdraw;
	_trdrawBg->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BG_CRNR_U, UV_BG_CRNR_V, UV_BG_CRNR_W, UV_BG_CRNR_H, W_TEX, H_TEX, 1.0f), W_BG_CRNR, H_BG_CRNR, TFW_DISPF_RANG_0);
	_trdrawBg->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BG_SIDE_U, UV_BG_SIDE_V, UV_BG_SIDE_W, UV_BG_SIDE_H, W_TEX, H_TEX, 1.0f), W_BG_SIDE, H_BG_SIDE, TFW_DISPF_RANG_0);
	_trdrawBg->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BG_CRNR_U, UV_BG_CRNR_V, UV_BG_CRNR_W, UV_BG_CRNR_H, W_TEX, H_TEX, 1.0f), W_BG_CRNR, H_BG_CRNR, TFW_DISPF_RANG_90);
	_trdrawBg->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BG_SIDE_U, UV_BG_SIDE_V, UV_BG_SIDE_W, UV_BG_SIDE_H, W_TEX, H_TEX, 1.0f), W_BG_SIDE, H_BG_SIDE, TFW_DISPF_RANG_270);
	_trdrawBg->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BG_BASE_U, UV_BG_BASE_V, UV_BG_BASE_W, UV_BG_BASE_H, W_TEX, H_TEX, 1.0f), W_BG_BASE, H_BG_BASE, TFW_DISPF_RANG_0);
	_trdrawBg->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BG_SIDE_U, UV_BG_SIDE_V, UV_BG_SIDE_W, UV_BG_SIDE_H, W_TEX, H_TEX, 1.0f), W_BG_SIDE, H_BG_SIDE, TFW_DISPF_RANG_90);
	_trdrawBg->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BG_CRNR_U, UV_BG_CRNR_V, UV_BG_CRNR_W, UV_BG_CRNR_H, W_TEX, H_TEX, 1.0f), W_BG_CRNR, H_BG_CRNR, TFW_DISPF_RANG_270);
	_trdrawBg->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BG_SIDE_U, UV_BG_SIDE_V, UV_BG_SIDE_W, UV_BG_SIDE_H, W_TEX, H_TEX, 1.0f), W_BG_SIDE, H_BG_SIDE, TFW_DISPF_RANG_180);
	_trdrawBg->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BG_CRNR_U, UV_BG_CRNR_V, UV_BG_CRNR_W, UV_BG_CRNR_H, W_TEX, H_TEX, 1.0f), W_BG_CRNR, H_BG_CRNR, TFW_DISPF_RANG_180);

	// 子パネル作成
	_pnlBreak = new Panel();
	_pnlBreak->create(&rect);
	_pnlBreak->setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, UV_BBTN_NRM));
	_pnlBreak->setStateInfo(STATE_HOVER, texRef, RectF32::setout(&uv, UV_BBTN_HVR));
	_pnlBreak->setHorizontalHitMargin(MARGIN_H_BBTN);
	_pnlBreak->setVerticalHitMargin(MARGIN_V_BBTN);

	// タップ開始時ウインドウ位置の作成
	_ptTapStartPos = new PointF32(F32_PMAX, F32_PMAX);
	// ゲージ表示位置の作成
	_ptGaugePos = new PointF32;

	_texGuiRef = texRef;		// テクスチャへの参照を保存
	_fontRef = fontRef;			// フォントへの参照を保存
	_isDone = false;			// 終了フラグをクリア
	_isInputDone = false;		// 入力フラグをクリア
#if OLD_ARIAWND_POSITION
	_isLeftPosition = false;	// デフォルトは右表示
#endif
	_alphaFadeCol = 255;		// フェードアルファ値

	setRectangle(&rect);		// 矩形を再設定
	setMode(mode);				// 動作モード設定

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void AriaWindow::destroy()
{
	// ゲージ表示位置の削除
	delete _ptGaugePos;
	_ptGaugePos = 0L;

	// タップ開始時ウインドウ位置の削除
	delete _ptTapStartPos;
	_ptTapStartPos = 0L;

	// 子パネル削除
	_pnlBreak->destroy();
	delete _pnlBreak;
	_pnlBreak = 0L;

	// 背景タイルの削除
	if(_trdrawBg != 0L)
	{
		_trdrawBg->destroy();
		delete _trdrawBg;
		_trdrawBg = 0L;
	}

	// 各モードのオブジェクト削除
	deleteSpellModeObject();
	deleteRawModeObject();

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	内容のフレーム制御
**//*---------------------------------------------------------------------*/
void AriaWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel();
	PointF32 ptTouch, ptClick, ptStart;

	// ユーザー移動
	if(!ui->isReacted() && (ui->getLastAction() == TouchPanel::ACT_DRAG))
	{
		ui->getDragParam(&ptStart, &ptTouch, 0L);
		// ウインドウ内をドラッグしたとき、ウインドウを移動させる
		if(_ptTapStartPos->x() == F32_PMAX)
		{
			if(getRectangle()->isPointIn(&ptStart))
			{
				_ptTapStartPos->set(getRectangle()->x(), getRectangle()->y());
				ui->setReacted(true);
			}
		}
		else
		{
			_isUserOp = true;
			ui->setReacted(true);
			// ドラッグ移動
			PointF32 pt(ptTouch.x() - ptStart.x() + _ptTapStartPos->x(), ptTouch.y() - ptStart.y() + _ptTapStartPos->y());
			this->setPosition(&pt);
			_isDefaultPosition = false;
		}
	}
	else if(_isUserOp && (ui->getTouchingNum() == 0) && (_ptTapStartPos->x() != F32_PMAX))
	{
		_ptTapStartPos->set(F32_PMAX, F32_PMAX);
	}
	// ユーザーによるモード変更
	if(!ui->isReacted())
	{
		if(ui->isDoubleClickRelease(&ptTouch))
		{
			if(getRectangle()->isPointIn(&ptTouch))
			{
				switch(_mode)
				{
				case MODE_RAW:		setMode(MODE_SPELL);	break;
				case MODE_SPELL:	setMode(MODE_RAW);		break;
				}
			}
		}
	}

	// フェード透明度計算
	_alphaFadeCol = 255;
	f32 fcntFade = getFadeFrameCount();
	if(fcntFade != 0.0f)
	{
		if(fcntFade > 0.0f)
		{
			_alphaFadeCol = (u8)((FCNT_WINDOWS_FADE - fcntFade) / (f32)FCNT_WINDOWS_FADE * 255.0f);
		}
		else
		{
			_alphaFadeCol = (u8)(- fcntFade / (f32)FCNT_WINDOWS_FADE * 255.0f);
		}
	}

	// 文字列描画制御
	if(_txtboxSrc != 0L)
	{
		_txtboxSrc->exec(res, ec);
		_txtboxSrc->textColor()->a() = _alphaFadeCol;
		_txtboxSrc->activeTextColor()->a() = _alphaFadeCol;
	}
	else if(_strdrawSpell != 0L)
	{
		_strdrawSpell->exec(ec->getDeltaFrame());
	}

	// ソース表示
	if(_soumaRef != 0L)
	{
#if OLD_ARIAWND_POSITION
		// 自動位置変更
		if(_ecntChPosition <= 0)
		{
			u16 mcid = _soumaRef->getCurClusterId();
			if(mcid > 0)
			{
				MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
				MagicCluster* mc = mgcsys->getCluster(mcid); ASSERT(mc != 0L);
				if(mc != 0L)
				{
					u16 ptclid = mc->getParticleId(0);
					if(ptclid > 0)
					{
						ParticleSys* ptclsys = Game::getGame()->getParticleSys(); ASSERT(ptclsys != 0L);
						Particle* ptcl = ptclsys->getParticle(ptclid); ASSERT(ptcl != 0L);
						if(ptcl != 0L)
						{
							Model* mdl = ptcl->getModel(); ASSERT(mdl != 0L);
							if(mdl != 0L)
							{
								const Vector3F*	vpos = mdl->getViewPosition(); ///ASSERT(vpos != 0L);
								if(vpos != 0L)
								{
									bool isLeftPosionNew = (vpos->x() >= 0.0f);	// 画面中央より右にクラスタが描画されてるとき、ウインドウを左位置に表示
									// 位置変更
									if(isLeftPosionNew != _isLeftPosition)
									{
										_isLeftPosition = isLeftPosionNew;
										_ecntChPosition = 30;	// このフレーム間は再判定しない
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			_ecntChPosition--;
		}
#endif

		// 位置移動
		if(!_isUserOp)	// ユーザー移動していないときのみ
		{
			CharFronts* chrfrnt = Game::getGame()->getGui()->getCharFronts();
			if(chrfrnt->isEnable())
			{
				RectF32 rect;
				chrfrnt->getPcStatRect(&rect, 0);
				PointF32 pt(rect.x(), rect.y() + rect.h() + 2);
				if((this->getRectangle()->x() != pt.x()) || (this->getRectangle()->y() != pt.y()))
				{
					this->setPosition(&pt);
				}
				_isDefaultPosition = true;
			}

#if OLD_ARIAWND_POSITION
			const RectF32* rectWnd = this->getRectangle();
			if(_isLeftPosition && ((rectWnd->x() != X_LEFTP) || (rectWnd->y() != Y_LEFTP)))
			{
				PointF32 pt((rectWnd->x() + X_LEFTP) / 2, (rectWnd->y() + Y_LEFTP) / 2);
				this->setPosition(&pt);
			}
			else if(!_isLeftPosition && ((rectWnd->x() != X_RIGHTP) || (rectWnd->y() != Y_RIGHTP)))
			{
				PointF32 pt((rectWnd->x() + X_RIGHTP) / 2, (rectWnd->y() + Y_RIGHTP) / 2);
				this->setPosition(&pt);
			}
#endif
		}
	}

#if 0
	// 子パネル制御
	ptTouch.set(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? /*ui->isClickRelease(&ptClick)*/ui->isTapRelease(&ptClick, 0L) : false;
	if(isClick)
	{
		if(_pnlBreak->hit(&ptClick, true))
		{
			// ソウマ強制終了
			if(_soumaRef != 0L)
			{
				_soumaRef->terminate();
			}

			ui->setReacted(true);
		}
	}
	else
	{
		_pnlBreak->hit(&ptTouch, isTouch);
	}
	_pnlBreak->exec(res, ec);
#endif
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void AriaWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	Renderer* rdr = rc->getRenderer();
	rc->getRenderer()->setSolidColor(255, 255, 255, 255);

	const RectF32* rectWnd = this->getRectangle();

	// 背景描画
	rc->getRenderer()->setSolidColor(255, 255, 255, _alphaFadeCol);		// 背景描画の色を設定
	_trdrawBg->draw(rc, rectWnd);

	// 吹き出しの角を描画する
	if(_isDefaultPosition)
	{
		RectF32 vtx, uv;
		rdr->bindTexture(_texGuiRef);
		RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, rectWnd->x() + 42.0f, rectWnd->y(), - W_BG_HORN, - H_BG_HORN), Gcalc::calcTexUv(&uv, UV_BG_HORN_U, UV_BG_HORN_V, UV_BG_HORN_W, UV_BG_HORN_H, W_TEX, H_TEX, 1.0f));
	}

	// テキスト描画
	rc->getRenderer()->setSolidColor(0, 0, 0, _alphaFadeCol);			// テキストの色
	if(_txtboxSrc != 0L)			{	_txtboxSrc->draw(rc);		}
	else if(_strdrawSpell != 0L)	{	_strdrawSpell->draw(rc);	}
	rc->getRenderer()->setSolidColor(255, 255, 255, _alphaFadeCol);		// 色を戻す

	// マジッククラスタの元素エネルギーを表示する
	if(_soumaRef != 0L)
	{
		u16 mcid = _soumaRef->getCurClusterId();
		if(mcid > 0)
		{
			MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
			MagicCluster* mc = mgcsys->getCluster(mcid); ASSERT(mc != 0L);
			if(mc != 0L)
			{
				const TransStat* tstat = mc->getTransStat();
				if(tstat != 0L)
				{
					#if 0
						const Unit* unitOwner = tstat->getOwnerUnit();
						if((unitOwner != 0L) && unitOwner->isCategory(Unit::UNITCAT_CHAR))
						{
							const CharStat* cstat = ((CharUnit*)unitOwner)->getCharStat();
							s32 lfeneMax = cstat->getMaxEnergy();
							s32 lfeneCost = tstat->getEleneStat()->getAfv()->getSoumaMaterializeLifeEnergy();

							// 消費エネルギーバー表示
							f32 w = 80.0f;
							f32 rateMax = w / (f32)lfeneMax;
							f32 wCost = lfeneCost * rateMax;
							if(wCost > 0.0f)
							{
								if(wCost < 1.0f)	{	wCost = 1.0f;	}
								RectF32 rect;
								ColorU8 col1, col2, col3, col4;
								RendererUtils::draw2dColorRect(
									rdr,
									RectF32::setout(
										&rect,
										_ptGaugePos->x(),
										_ptGaugePos->y() - 10.0f,
										/*
										rectWnd->x() + rectWnd->w() - 80.0f,
										rectWnd->y() + rectWnd->h() - 10.0f, 
										*/
										wCost,
										8.0f	),
									ColorU8::setout(&col1, 255,  91,  27, 241),
									ColorU8::setout(&col2, 255,  88,  27, 241),
									ColorU8::setout(&col3, 255,  63,  27, 127),
									ColorU8::setout(&col4, 255,  47,  27, 127) );
							}
						}
					#endif

					const EleneStat* eestat = tstat->getEleneStat();
					StatusDrawer::drawEleneStat(rc, _texGuiRef, eestat, (f32)_ptGaugePos->x(), (f32)_ptGaugePos->y(), StatusDrawer::W_GAUGE_DEFAULT, StatusDrawer::H_ENEGAUGE_DEFAULT, false);
				}
			}
		}
	}

	if(_alphaFadeCol != 255)	{	rc->getRenderer()->setSolidColor(255, 255, 255, 255);	}		// 色を戻す

#if 0
	// 子パネル描画
	_pnlBreak->draw(rc);
#endif
}

/*---------------------------------------------------------------------*//**
	ソースモードのオブジェクト作成
**//*---------------------------------------------------------------------*/
bool AriaWindow::createRawModeObject()
{
	// テキスト作成
	SjisTxt* txt = new SjisTxt();	///Txt* txt = new AsciiTxt();
	if(txt == 0L)
	{
		ASSERT(false);
		return false;
	}
	// テキストエディタの作成
	TxtBox* txtbox = new TxtBox();
	RectF32 restStr(getRectangle());
	restStr.x() += L_PAD_TXT;
	restStr.y() += T_PAD_TXT;
	restStr.w() -= L_PAD_TXT + R_PAD_TXT;
	restStr.h() -= T_PAD_TXT + B_PAD_TXT;
	if(!txtbox->create(txt, _fontRef, 10, _texGuiRef, &restStr, TxtBox::MODE_SRCREAD, 0))
	{
		delete txtbox;
		ASSERT(false);
		return false;
	}
	_txtboxSrc = txtbox;
	// テキストをテキストエディタに設定する
	const VcString* strCode = getSourceCode();
	if(strCode != 0L)	{	setSourceCode(strCode, false);	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ソースモードのオブジェクト削除
**//*---------------------------------------------------------------------*/
void AriaWindow::deleteRawModeObject()
{
	// テキストエディタの削除
	if(_txtboxSrc != 0L)
	{
		_txtboxSrc->destroy();
		delete _txtboxSrc;
		_txtboxSrc = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	呪文モードのオブジェクト作成
**//*---------------------------------------------------------------------*/
bool AriaWindow::createSpellModeObject()
{
	// 呪文文字列作成
	_strSpell = new VcString();
	if(_strSpell == 0L)
	{
		ASSERT(false);
		return false;
	}
	// 文字列描画の作成
	StringDrawer* strdraw = new StringDrawer();
	RectF32 restStr(
		(f32)getRectangle()->x() + L_PAD_TXT,
		(f32)getRectangle()->y() + T_PAD_TXT,
		(f32)getRectangle()->w() - (L_PAD_TXT + R_PAD_TXT),
		(f32)getRectangle()->h() - (T_PAD_TXT + B_PAD_TXT)	);
	if(!strdraw->create(_fontRef, 10, &restStr, _strSpell))
	{
		delete strdraw;
		ASSERT(false);
		return false;
	}
	_strdrawSpell = strdraw;
	_strdrawSpell->setVerticalScrollAnimation(true, 2.0f);
	return true;
}

/*---------------------------------------------------------------------*//**
	呪文モードのオブジェクト削除
**//*---------------------------------------------------------------------*/
void AriaWindow::deleteSpellModeObject()
{
	// 文字列描画の削除
	if(_strdrawSpell != 0L)
	{
		_strdrawSpell->destroy();
		delete _strdrawSpell;
		_strdrawSpell = 0L;
	}
	// 呪文文字列の削除
	delete _strSpell;
	_strSpell = 0L;
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void AriaWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// 値をクリア
		_isDone = false;		// 終了フラグをクリア
		_isInputDone = false;	// 入力フラグをクリア
		_isUserOp = false;		// ユーザー移動フラグをクリア
		_alphaFadeCol = 255;	// フェードアルファ値をリセット
		_ptTapStartPos->set(F32_PMAX, F32_PMAX);
	}
}

/*---------------------------------------------------------------------*//**
	ソースコードを得る
**//*---------------------------------------------------------------------*/
const VcString* AriaWindow::getSourceCode() const
{
	if(_soumaRef == 0L)	{	return 0L;	}
	const VcString* strCode = _soumaRef->getCode();
	if(strCode == 0L)	{	return 0L;	}
	return strCode;
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
