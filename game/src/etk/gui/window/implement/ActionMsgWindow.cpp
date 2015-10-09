/***********************************************************************//**
 *	ActionMsgWindow.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/12/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ActionMsgWindow.h"

// Friends
#include "../../../body/EtkBody.h"
#include "../../../body/EtkExecCtx.h"

// External

// Library
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/string/VcString.h"
#include "../../../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置
#define PADDING_X		(16)
#define PADDING_Y		(8)

// テクスチャ座標値
#define TEX_W			(1024.0f)
#define TEX_H			(1024.0f)
#define TEX_UV_L		(0.0f / TEX_W)
#define TEX_UV_T		(0.0f / TEX_H)
#define TEX_UV_W		(443.0f / TEX_W)
#define TEX_UV_H		(106.0f / TEX_H)

#define FRAME_DISP			(5.0 * EtkBody::getBody()->getBasicFrameRate())		// テキスト表示フレーム数
#define FCNT_LIFE			(5.0f * EtkBody::getBody()->getBasicFrameRate())	// 表示継続フレーム
#define FCNT_DISAPR			(10)		// 消滅フレーム
#define Y_INVALID			(-10000.0f)	// 無効な Y 位置
#define H_BDRLINE_PADDING	(8.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ActionMsgWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	矩形の設定
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::setRectangle(const RectF32* rectWnd, const RectF32* rectScreen)
{
	_rectScreen->copy(rectScreen);

	MessageWindow::setRectangle(rectWnd);
}
/*---------------------------------------------------------------------*//**
	メッセージ表示
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::showMessageCol(const ColorU8* color, const VcString* text)
{
	/*
	VcString msg(VcString::format("{style color=#%02x%02x%02x}", color->r(), color->g(), color->b()));
	msg.add(text);
	addWindowText(color, &msg);
	*/
	addWindowText(color, text);
}

/*---------------------------------------------------------------------*//**
	メッセージクリア
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::clearMessage()
{
	_listMsg->removeAll();
}

/*---------------------------------------------------------------------*//**
	メッセージ追加
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::addWindowText(const VcString* text)
{
	ColorU8 col(255, 255, 255, 255);
	return addWindowText(&col, text);
}

/*---------------------------------------------------------------------*//**
	メッセージ追加
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::addWindowText(const ColorU8* color, const VcString* text)
{
	RectF32 rect(0, Y_INVALID, (f32)_rectScreen->width(), (f32)_rectScreen->height());
	SizeF32 size;

	// メッセージ追加
	Msg* msg = new Msg(text, color, _fontRef, &rect);
	msg->_frmcnt = FCNT_LIFE;
	msg->_strdraw->drawTest(&size);
	msg->_hStr = size.h();
	msg->_strdraw->rectangle()->h() = msg->_hStr;
	_listMsg->addTail(msg);

	// ウインドウを自動的に表示する
	if(!isShowWindow())
	{
		showWindow(true);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ActionMsgWindow::ActionMsgWindow()
	: _isSelfHide(false)
	, _isResident(false)
	, _isBgCurtain(false)
	, _rectScreen(0L)
	, _fontRef(0L)
	, _listMsg(0L)
	, _yBdrlineCur(0.0f)
	, _yBdrlineMin(0.0f)
	, _yBdrlineDisp(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ActionMsgWindow::~ActionMsgWindow()
{
	ASSERT(_rectScreen == 0L);
	ASSERT(_listMsg == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ActionMsgWindow::create(const RectF32* rectWnd, const RectF32* rectScreen, Font* fontRef, Texture* texRef)
{
	// スクリーン矩形の保存
	_rectScreen = new RectF32(*rectScreen);

	// ウインドウの作成
	RectF32 rect;
	ColorU8 col;
	if(!MessageWindow::create(fontRef, texRef, rectWnd, ColorU8::setout(&col, 255, 255, 255, 255), PADDING_X, PADDING_X, PADDING_Y, PADDING_Y, true))
	{
		return false;
	}
	///getPanelText()->setVerticalScrollAnimation(true, 1.0f);

	// フォントの保存
	_fontRef = fontRef;

	// リストの作成
	_listMsg = new List<Msg*>(true);

	// ボーダーライン設定
	_yBdrlineMin = (f32)_rectScreen->height() * 0.5f;
	_yBdrlineCur = (f32)_rectScreen->height();
	_yBdrlineDisp = (f32)_rectScreen->height();

	// 背景幕を描画するか否か
	_isBgCurtain = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::destroy()
{
	// リストの削除
	delete _listMsg;
	_listMsg = 0L;
	// スクリーン矩形の削除
	delete _rectScreen;
	_rectScreen = 0L;

	MessageWindow::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;

	// 操作ロック時は処理を進めない
	bool isHide = eec->isLockOperating();	// 操作ロック時
	if(_isSelfHide != isHide)		// 状態が変更された
	{
		_isSelfHide = isHide;
	}
	if(_isSelfHide)
	{
		return;
	}

	if(true)	// ユーザーの入力を受け付けるモードの場合
	{
		TouchPanel* ui = eec->getTouchPanel(); 
		PointF32 ptN, ptS;
		if(!ui->isReacted() && ui->isTapRelease(&ptN, &ptS))
		{
			RectF32 rect(0, _yBdrlineDisp, _rectScreen->width(), _rectScreen->height() - _yBdrlineDisp);
			if(rect.isPointIn(&ptN) && rect.isPointIn(&ptS))
			{
				// メッセージのフレームカウントをゼロにして消滅を促す
				ListIterator<Msg*> it;
				for(ListIterator<Msg*> it = _listMsg->iterator(); it.has(); it.next())
				{
					Msg* msg = it.object();
					if(	(msg->_yLogical > Y_INVALID)	// 出現済みのメッセージ
						&& (msg->_frmcnt > 0.0f)	)	// フレームカウンタを修正
					{
						msg->_frmcnt = 0.0f;
						break;
					}
				}
			}
		}
	}

	// メッセージ更新
	{
		// 未出現メッセージの出現処理
		f32 y = (f32)_rectScreen->height();
		ListIterator<Msg*> itWk;
		ListIterator<Msg*> itAppearingTop;
		for(itWk = _listMsg->iteratorEnd(); itWk.has(); itWk.prev())
		{
			Msg* msg = itWk.object();
			if(msg->_yLogical <= Y_INVALID)	// 未出現のメッセージ
			{
				itAppearingTop = itWk;
			}
			else
			{
				y = msg->_yLogical + msg->_hStr;
				break;
			}
		}
		if(itAppearingTop.has())
		{
			for(itWk = itAppearingTop; itWk.has(); itWk.next())
			{
				Msg* msg = itWk.object();
				msg->_strdraw->rectangle()->y() = msg->_yLogical = y;
				y += msg->_hStr;
			}
		}
		// メッセージ送りスピード
		f32 speed = ((y <= _rectScreen->height()) ? 1.0f : (y / (f32)_rectScreen->height())) * 2.0f;	//(y <= (3 * Game::getGame()->getLogicalHeight() / 2)) ? 1.5f : (y / (f32)Game::getGame()->getLogicalHeight());
		// メッセージ位置更新
		f32 yDif = _rectScreen->height() - y;
		f32 yMov = ec->getDeltaFrame() * speed;
		f32 yAdv = - yMov;
		if(yAdv < yDif)	{	yAdv = yDif;	}
		itWk = _listMsg->iterator();
		while(itWk.has())
		{
			Msg* msg = itWk.object();
			// フレームカウンタ更新
			if((msg->_strdraw->rectangle()->y() + msg->_hStr) <= _rectScreen->height())	// 表示中
			{
				msg->_frmcnt -= ec->getDeltaFrame();
			}
			// 位置更新
			if(yAdv < 0.0f)
			{
				msg->_yLogical += yAdv;
			}
			itWk.next();
		}
		// ボーダーライン算出
		itWk = _listMsg->iterator();
		if(itWk.has())
		{
			Msg* msg = itWk.object();
			if(msg->_frmcnt < 0.0f)
			{
				_yBdrlineCur += yMov;
			}
			else
			{
				_yBdrlineCur = itWk.object()->_yLogical;
			}
			if(_yBdrlineCur < _yBdrlineMin)
			{
				_yBdrlineCur = _yBdrlineMin;
			}
			_yBdrlineDisp = _yBdrlineCur - H_BDRLINE_PADDING;
		}
		else
		{
			_yBdrlineCur = (f32)_rectScreen->height();
			_yBdrlineDisp += yMov;
		}
		// クリップ処理と消滅処理
		itWk = _listMsg->iterator();
		while(itWk.has())
		{
			Msg* msg = itWk.object();
			// クリップ処理
			if(msg->_yLogical < _yBdrlineCur)
			{
				yDif = _yBdrlineCur - msg->_yLogical;
				if(yDif > msg->_hStr)	{	yDif = msg->_hStr;	}
				msg->_strdraw->rectangle()->y() = _yBdrlineCur;
				msg->_strdraw->rectangle()->h() = msg->_hStr - yDif;
				msg->_strdraw->setScrollPosition(- yDif);
			}
			else
			{
				msg->_strdraw->rectangle()->y() = msg->_yLogical;	// メッセージ位置更新
			}
			// 消滅処理
			if((msg->_yLogical + msg->_hStr) <= _yBdrlineCur)
			{
				ListIterator<Msg*> itTmp = itWk;
				itTmp.prev();
				_listMsg->removeNode(itWk.node());
				itWk = itTmp;
				continue;
			}
			itWk.next();
		}
	}

	// ウインドウを自動的に消す
	if(isShowWindow())
	{
		if(_yBdrlineDisp >= _rectScreen->width())
		{
			showWindow(false);
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::draw(const RenderCtx* rc)
{
	if(_isSelfHide) { return; }
	// 非表示時は抜ける
	if(!isShowWindow())	{ return; }
	// 一時非表示中は抜ける
	if(isTemporaryHiding()) { return; }

	if(_isBgCurtain && (_yBdrlineDisp < _rectScreen->width()))
	{
		rc->getRenderer()->setSolidColor(0, 0, 0, 63);
		RendererUtils::draw2dRect(rc->getRenderer(), 0.0f, _yBdrlineDisp, (f32)_rectScreen->width(), _rectScreen->height() - _yBdrlineDisp);
	}

	for(ListIterator<Msg*> it = _listMsg->iterator(); it.has(); it.next())
	{
		Msg* msg = it.object();
		if(msg->_yLogical <= Y_INVALID)	// 未出現のメッセージ
		{
			continue;
		}
		
		//ASSERT(msg->_strdraw->getRectangle()->w() >= msg->_strdraw->getCharDefaultWidth());
		//ASSERT(msg->_strdraw->getTargetString() != 0L);
		//ASSERT((msg->_strdraw->getTargetString() != 0L) && (msg->_strdraw->getTargetString()->getLength() > 0));
		rc->getRenderer()->setSolidColor(msg->_col);	// 色の設定
		msg->_strdraw->draw(rc);
	}

}

#if 0
/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	if(_isSelfHide)		{	return;	}

	Renderer* rdr = rc->getRenderer();
	PanelText* ptxt = getPanelText();	ASSERT(ptxt != 0L);

	// 背景描画
	rdr->setSolidColor(0, 0, 0, 127);
	rdr->bindTexture(0L);
	RendererUtils::draw2dRect(rdr, (f32)rectBase->x(), (f32)rectBase->y(), (f32)rectBase->w(), (f32)rectBase->h());

	// パネル機能による文字描画
	Panel::draw(rc);
}
#endif

/*---------------------------------------------------------------------*//**
	ウインドウ表示通知
**//*---------------------------------------------------------------------*/
void ActionMsgWindow::onShowWindow(bool isShow)
{
	if(!isShow)
	{
		// 非表示時に以前のメッセージを削除
		setWindowText(TypeUtils::getEmptyString());
	}

	MessageWindow::onShowWindow(isShow);
}


//==========================================================================
// ActionMsgWindow::Msg メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ActionMsgWindow::Msg::Msg(const VcString* str, const ColorU8* col, Font* fontRef, const RectF32* rectString)
	: _str(new VcString(str))
	, _col(new ColorU8(col))
	, _strdraw(new StringDrawer())
	, _frmcnt(0.0f)
	, _yLogical(Y_INVALID)
	, _hStr(0.0f)
{
	if(!_strdraw->create(fontRef, 16, rectString, _str))
	{
		ASSERT(false);
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ActionMsgWindow::Msg::~Msg()
{
	if(_strdraw != 0L)
	{
		_strdraw->destroy();
		delete _strdraw;
	}

	delete _str;
	delete _col;
}


////////////////////////////////////////////////////////////////////////////

ETK_END_NS
