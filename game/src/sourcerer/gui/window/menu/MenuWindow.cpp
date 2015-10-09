/***********************************************************************//**
 *	MenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MenuWindow.h"

// Friends
#include "../../menu/GameMenuCreateParam.h"
#include "../../menu/GameMenuShowParam.h"
#include "../../panel/ScrollBar.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../sound/GameSoundDef.h"

// External
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/collection/Tree.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/string/VcString.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuExecRes.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/ui/Pad.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define RATE_POS_TO_VEL			(100)
#define RATE_FLIP_TO_VEL		(RATE_POS_TO_VEL / 4)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MenuWindow 定数

const f32 MenuWindow::X_SCRBARWD = 32.0f;
const f32 MenuWindow::W_SCRBAR_PAD = 16.0f;

//==========================================================================
// MenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ウインドウを移動する
**//*---------------------------------------------------------------------*/
void MenuWindow::move(PointF32* pos)
{
	// 差を求める
	f32 xDiff = pos->x() - _ptBase->x();
	f32 yDiff = pos->y() - _ptBase->y();

	// スクロールバーの移動
	RectF32 rect = *_scrlbar->getRectangle();
	rect.x() += xDiff;
	rect.y() += yDiff;
	_scrlbar->setRectangle(&rect);

	// 要素ごとの移動
	ListIterator<Block*> it = _listBlock->iterator();
	while(it.has())
	{
		Block* block = it.next();
		block->moveDiff(xDiff, yDiff);
	}

	// このウインドウ自体の移動
	rect = *this->getRectangle();
	rect.x() += xDiff;
	rect.y() += yDiff;
	this->setRectangle(&rect);

	// 新しい位置として保存
	_ptBase->x() = pos->x();
	_ptBase->y() = pos->y();
}

/*---------------------------------------------------------------------*//**
	要素数を得る
**//*---------------------------------------------------------------------*/
s32 MenuWindow::getElementNum()
{
	return _listBlock->getCount();
}

/*---------------------------------------------------------------------*//**
	要素のデータインデックスをポイントから得る
**//*---------------------------------------------------------------------*/
s32 MenuWindow::getElementContentIndexByPoint(const PointF32* pt) const
{
	Block* block = getBlock(pt);
	if(block == 0L)	{	ASSERT(false);	return -1;	}
	return block->_index;
}

/*---------------------------------------------------------------------*//**
	要素のデータインデックスをノードから得る
**//*---------------------------------------------------------------------*/
s32 MenuWindow::getElementContentIndexByNode(const MenuTreeNode* mtnode) const
{
	for(ListIterator<Block*> it = _listBlock->iterator(); it.has(); it.next())
	{
		Block* block = it.object();
		if(block->_tnode->getObject() == mtnode)
		{
			return block->_index;
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	要素を使用不能にする
**//*---------------------------------------------------------------------*/
bool MenuWindow::setElementUnused(s32 index, bool isUnused)
{
	if((index < 0) || (_listBlock->getCount() <= (u32)index))	{	ASSERT(false);	return false;	}
	Block* block = _listBlock->getObject(index);
	#if MWBLOCK_FLAG_STATE
		TFW_SET_FLAG(block->_flags, Block::F_UNUSED, isUnused);
	#else
		block->_tnode->object()->setUnused(isUnused);
	#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	要素の描画矩形を取得する
**//*---------------------------------------------------------------------*/
bool MenuWindow::getElementRect(s32 index, RectF32* rect, RectF32* rectOfs, RectF32* rectOfsAnim)
{
	if((index < 0) || (_listBlock->getCount() <= (u32)index))	{	ASSERT(false);	return false;	}
	const Block* block = _listBlock->getObject(index);
	if(block == 0L)		{	ASSERT(false);	return false;	}
	if(rect != 0L)											{	rect->copy(block->_rect);				}
	if((rectOfs != 0L) && (block->_rectOfs))				{	rectOfs->copy(block->_rectOfs);			}
	if((rectOfsAnim != 0L) && (block->_rectOfsAnim != 0L))	{	rectOfsAnim->copy(block->_rectOfsAnim);	}
	return true;
}

/*---------------------------------------------------------------------*//**
	要素の描画矩形を設定する
**//*---------------------------------------------------------------------*/
bool MenuWindow::setElementRect(s32 index, const RectF32* rect, const RectF32* rectOfs, const RectF32* rectOfsAnim)
{
	if((index < 0) || (_listBlock->getCount() <= (u32)index))	{	ASSERT(false);	return false;	}
	Block* block = _listBlock->getObject(index);
	if(block == 0L)		{	ASSERT(false);	return false;	}
	if(rect != 0L)											{	block->_rect->copy(rect);				}
	if((rectOfs != 0L) && (block->_rectOfs))				{	block->_rectOfs->copy(rectOfs);			}
	if((rectOfsAnim != 0L) && (block->_rectOfsAnim != 0L))	{	block->_rectOfsAnim->copy(rectOfsAnim);	}
	return true;
}

/*---------------------------------------------------------------------*//**
	要素の内容を設定する
**//*---------------------------------------------------------------------*/
bool MenuWindow::setElementContent(s32 index, const VcString* str)
{
	if((index < 0) || (_listBlock->getCount() <= (u32)index))	{	ASSERT(false);	return false;	}
	Block* block = _listBlock->getObject(index);
	if(block == 0L)	{	ASSERT(false);	return false;	}

	block->_str->set(str);

	// テキストサイズの再算出
	RectF32 rectBlock, rectText;
	SizeF32 sizeStr;
	block->_strdraw->setRectangle(block->_rect);
	block->_strdraw->setString(block->_str);
	block->_strdraw->drawTest(&sizeStr);
	block->_sizeTxt->w() = (f32)((s32)(sizeStr.w() + 0.999f));
	block->_sizeTxt->h() = (f32)((s32)(sizeStr.h() + 0.999f));
	remakeBlockRect(&rectBlock, &rectText, block, block->_sizeTxt);
	if(adjustBlockString(block->_str, &rectText, block->_strdraw, &rectBlock)) { block->_strdraw->setString(block->_str); }
	block->_strdraw->setRectangle(&rectText);

	return true;
}

/*---------------------------------------------------------------------*//**
	要素の汎用パラメータを設定する
**//*---------------------------------------------------------------------*/
bool MenuWindow::setElementParam(s32 index, u64 param)
{
	if((index < 0) || (_listBlock->getCount() <= (u32)index))	{	ASSERT(false);	return false;	}
	Block* block = _listBlock->getObject(index);
	if(block == 0L)	{	ASSERT(false);	return false;	}

	block->_param = param;

	return true;
}

/*---------------------------------------------------------------------*//**
	要素の内容を置換する
**//*---------------------------------------------------------------------*/
bool MenuWindow::replaceElementContent(s32 index1, s32 index2)
{
	if(index1 == index2)	{	return false;	}
	if((index1 < 0) || (_listBlock->getCount() <= (u32)index1))	{	ASSERT(false);	return false;	}
	if((index2 < 0) || (_listBlock->getCount() <= (u32)index2))	{	ASSERT(false);	return false;	}
	Block* block1 = _listBlock->getObject(index1);
	Block* block2 = _listBlock->getObject(index2);
	if(block1 == 0L)	{	ASSERT(false);	return false;	}
	if(block2 == 0L)	{	ASSERT(false);	return false;	}

	VcString strTmp(block1->_str);
	u16 flagsTmp = block1->_flags;
	u64 paramTmp = block1->_param;
	SizeF32 sizeTxtTmp(block1->_sizeTxt);
	MenuTreeNode mtnodeTmp;
	mtnodeTmp.create(TypeUtils::getEmptyString(), false, false, false, 0, 0);
	mtnodeTmp.copy(block1->_tnode->getObject());
	RectF32 rectBlock, rectText;

	block1->set(block2->_str, block2->_flags, block2->_param, block2->_tnode->getObject());
	remakeBlockRect(&rectBlock, &rectText, block1, block2->_sizeTxt);
	if(adjustBlockString(block1->_str, &rectText, block1->_strdraw, &rectBlock)) { block1->_strdraw->setString(block1->_str); }
	block1->_rect->copy(&rectBlock);
	block1->_strdraw->setRectangle(&rectText);
	block1->_sizeTxt->copy(block2->_sizeTxt);

	block2->set(&strTmp, flagsTmp, paramTmp, &mtnodeTmp);
	remakeBlockRect(&rectBlock, &rectText, block2, &sizeTxtTmp);
	if(adjustBlockString(block2->_str, &rectText, block2->_strdraw, &rectBlock)) { block2->_strdraw->setString(block2->_str); }
	block2->_rect->copy(&rectBlock);
	block2->_strdraw->setRectangle(&rectText);
	block2->_sizeTxt->copy(&sizeTxtTmp);

	mtnodeTmp.destroy();

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MenuWindow::MenuWindow()
	: _ptBase(new PointF32())
	, _sizeMax(new SizeF32())
	, _listBlock(new List<Block*>)
	, _tnodeSrc(0L)
	, _depth(0)
	, _isAvailable(false)
	, _indexCur(-1)
	, _indexNum(0)
	, _cntAnim(0)
	, _ptScrollTrg(new PointF32())
	, _ptScrollCur(new PointF32())
	, _ptScrollTapStart(new PointF32())
	, _isScorllBarKnobDrag(false)
	, _scrlbar(0L)
	, _velFlip(0)
	, _eflags(0)
	, _fontRef(0L)
	, _fontsize(0)
	, _texRef(0L)
	, _gmcpRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MenuWindow::~MenuWindow()
{
	delete _ptBase;
	delete _sizeMax;
	delete _listBlock;
	delete _ptScrollTapStart;
	delete _ptScrollTrg;
	delete _ptScrollCur;
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MenuWindow::create(TreeNode<MenuTreeNode>* tnodeSrc, s32 depth, Font* font, s32 fontsize, Texture* tex, void* paramShow, void* paramCreate)
{
	// ウインドウ作成
	RectF32 rect;
	if(!Window::create(&rect))	// ダミーの矩形で作成
	{
		return false;
	}
	
	// 値を保存
	_tnodeSrc = tnodeSrc;
	_depth = depth;
	_gmcpRef = (GameMenuCreateParam*)paramCreate;

	// スクロールバー作成
	_scrlbar = new ScrollBar();
	if(!_scrlbar->create(RectF32::setout(&rect, 0, 0, 10, 10), tex))
	{
		delete _scrlbar;
		_scrlbar = 0L;
		return false;
	}
	_sizeMax->set(Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());

	// パラメータ取得
	if(paramShow != 0L)
	{
		GameMenuShowParam* sp = (GameMenuShowParam*)paramShow;
		_ptBase->set(sp->_xBase, sp->_yBase);
		_sizeMax->set(sp->_wMax, sp->_hMax);
	}
	_fontRef = font;
	_fontsize = fontsize;
	_texRef = tex;
	if(!make(depth))
	{
		return false;
	}

	// 変数初期化
	_indexCur = -1;
	_cntAnim = 0;
	_isAvailable = true;
	_ptScrollTapStart->set(F32_PMAX, F32_PMAX);	// タップ開始時のスクロール値をリセット
	_isScorllBarKnobDrag = false;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MenuWindow::destroy()
{
	_isAvailable = false;

	// ブロックリストの削除
	for(ListIterator<Block*> it = _listBlock->iterator(); it.has(); it.next())
	{
		Block* block = it.object();
		block->destroy();
		delete block;
	}
	_listBlock->removeAll();

	// スクロールバーの削除
	if(_scrlbar != 0L)
	{
		_scrlbar->destroy();
		delete _scrlbar;
		_scrlbar = 0L;
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	ウインドウの作成
**//*---------------------------------------------------------------------*/
bool MenuWindow::make(s32 depth)
{
	// ウインドウ内の子ノードを作成する
	if(_tnodeSrc != 0L)
	{
		s32 indexChildChild = 0;
		
		const MenuTreeNode* mtnodeThis = _tnodeSrc->getObject();
		if(mtnodeThis->getName() != 0L)
		{
			VcString strb;
			strb.add(*mtnodeThis->getName());	// 文字列追加
			
			// メニューの最初の子ノードを作成する
			Block* block = new Block();
			if(!block->create(&strb, mtnodeThis->getGeneralFlags(), mtnodeThis->getGeneralParam(), _tnodeSrc, -1))
			{
				return false;
			}
			TFW_SET_FLAG(block->_typef, Block::TF_TITLE, true);
			// 子ノードをリストに追加する
			_listBlock->addTail(block);
		}

		// 子を走査する
		TreeNode<MenuTreeNode>* tnodeChild = _tnodeSrc->child();
		while(tnodeChild != 0L)
		{
			// 子のテキストを得る
			MenuTreeNode* mtnodeChild = tnodeChild->object();
			if(mtnodeChild->getName() != 0L)
			{
				if(	(Debug::isDebugMode() || (!Debug::isDebugMode() && !mtnodeChild->isDebug())) &&	// デバッグ属性はデバッグ時のみ
					!mtnodeChild->isHidden())	// 隠し要素でない
				{
					// メニューの子ノードを作成する
					Block* block = new Block();
					if(!block->create( &(*mtnodeChild->getName()), mtnodeChild->getGeneralFlags(), mtnodeChild->getGeneralParam(), tnodeChild, indexChildChild))
					{
						return false;
					}
					#if MWBLOCK_FLAG_STATE
						// フラグ設定
						TFW_SET_FLAG(block->_flags, Block::F_DISABLE, mtnodeChild->isDisabled());
						TFW_SET_FLAG(block->_flags, Block::F_HIDE, mtnodeChild->isHidden());
					#endif
					// 子ノードをリストに追加する
					_listBlock->addTail(block);
					
					indexChildChild++;
				}
			}
			
			tnodeChild = tnodeChild->sibling();
		}
		
		_indexNum = indexChildChild;
	}

	// 各子ノードのテキストサイズを計算する
	SizeF32 sizeTxtMax(0, 0);
	{
		ListIterator<Block*> it = _listBlock->iterator();
		RectF32 rectRcm(0, 0, F32_PMAX, F32_PMAX);	// 推奨サイズ（ここでは自由な最大サイズとする）
		while(it.has())
		{
			SizeF32 sizeStr;
			Block* block = it.next();
			block->_strdraw->setRectangle(&rectRcm);				// まずはほ推奨サイズの矩形を与える
			block->_strdraw->setFont(_fontRef);						// フォントを設定
			block->_strdraw->setFontSize(_fontsize);				// フォントサイズを設定
			block->_strdraw->setTracking(-1, 0);					// トラッキングを設定
			block->_strdraw->drawTest(&sizeStr);					// 描画シミュレーションを行ってサイズを出す
			block->_sizeTxt->w() = (f32)((s32)(sizeStr.w() + 0.999f));
			block->_sizeTxt->h() = (f32)((s32)(sizeStr.h() + 0.999f));
			TFW_SET_FLAG(block->_typef, Block::TF_TAIL, !it.has());	// 最終要素
			if(sizeTxtMax.w() < block->_sizeTxt->w())	{	sizeTxtMax.w() = block->_sizeTxt->w();	}	// 最大幅
			if(sizeTxtMax.h() < block->_sizeTxt->h())	{	sizeTxtMax.h() = block->_sizeTxt->h();	}	// 最大高
		}
	}

	// 各子ノードの矩形を設定する
	RectF32 rectMenu(F32_PMAX, F32_PMAX, 0, 0);
	{
		f32 x = _ptBase->x();
		f32 y = _ptBase->y();
		f32 xMax = 0.0f;
		f32 yMax = 0.0f;
		ListIterator<Block*> it = _listBlock->iterator();
		while(it.has())
		{
			Block* block = it.next();

			RectF32 rectNode;		// ノードの矩形
			RectF32 rectText;		// テキストの矩形
			makeBlockRect(&rectNode, &rectText, block, x, y, block->_sizeTxt, &sizeTxtMax, depth);
			if(adjustBlockString(block->_str, &rectText, block->_strdraw, &rectNode)) { block->_strdraw->setString(block->_str); }

			block->_rect->copy(&rectNode);							// ノードに矩形を設定
			block->_strdraw->setRectangle(&rectText);				// テキストに矩形を設定

			// メニュー矩形の x, y 位置
			if(rectMenu.x() > rectNode.x())	{	rectMenu.x() = rectNode.x();	}	// 最小位置を採用
			if(rectMenu.y() > rectNode.y())	{	rectMenu.y() = rectNode.y();	}	// 最小位置を採用
			// 最大 x, y 位置
			if(xMax < (rectNode.x() + rectNode.w()))	{	xMax = rectNode.x() + rectNode.w();	}
			if(yMax < (rectNode.y() + rectNode.h()))	{	yMax = rectNode.y() + rectNode.h();	}

			y += rectNode.h();		// 次の y 位置
		}
		// メニュー矩形の幅，高さ
		rectMenu.w() = xMax - rectMenu.x();
		rectMenu.h() = yMax - rectMenu.y();
	}
	SizeF32 sizeRange(_sizeMax->w() - (rectMenu.x() - _ptBase->x()), _sizeMax->h() - (rectMenu.y() - _ptBase->y()));
	
	// ウインドウを設定する
	_ptScrollTrg->set(0, 0);
	f32 xScrollMax = 0;
	f32 yScrollMax = 0;
	if((_sizeMax->w() > 0) && (rectMenu.w() > sizeRange.w()))	// 最大幅が指定されており、その幅を超えている場合
	{
		xScrollMax = rectMenu.x() - sizeRange.w();
		rectMenu.w() = sizeRange.w() - rectMenu.x();
	}
	if((_sizeMax->h() > 0) && (rectMenu.h() > sizeRange.h()))	// 最大高さが指定されており、その高さを超えている場合
	{
		yScrollMax = rectMenu.h() - (_sizeMax->h() - rectMenu.y());
		rectMenu.h() = sizeRange.h();
		rectMenu.w() += W_SCRBAR_PAD + X_SCRBARWD;	// スクロールバー分の幅を加算
	}
	Window::setRectangle(&rectMenu);

	// スクロールバーを設定する
	RectF32 rectSb(rectMenu.x() + rectMenu.w() - X_SCRBARWD, rectMenu.y(), X_SCRBARWD, sizeRange.h());
	_scrlbar->setRectangle(&rectSb);
	_scrlbar->setScrollMinMax(0, yScrollMax);
	_scrlbar->setScrollPage(sizeRange.h());
	_scrlbar->setScrollPos(0);

	return true;
}

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void MenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	const f32 X_PADDING = 0;
	const f32 Y_PADDING = 0;
	if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
	{
		rectBlock->set(xBase + X_PADDING, yBase, sizeTxt->w() + (X_PADDING * 2), sizeTxt->h() + Y_PADDING);				// ノードの矩形サイズを設定（タイトル）
		rectText->set(rectBlock->x() + X_PADDING, rectBlock->y() + Y_PADDING / 2, sizeTxt->w(), sizeTxt->h());			// テキストの矩形を設定（タイトル）
	}
	else
	{
		rectBlock->set(xBase + X_PADDING, yBase, sizeAllTxtMax->w() + (X_PADDING * 2), sizeTxt->h() + (Y_PADDING * 2));	// ノードの矩形サイズを設定
		if(block->_index != -1) { rectBlock->x() += 16; }																// インデックス付要素（選択可能）はインデント下げ
		rectText->set(rectBlock->x() + X_PADDING, rectBlock->y() + Y_PADDING, sizeTxt->w(), sizeTxt->h());				// テキストの矩形を設定
	}
}

/*---------------------------------------------------------------------*//**
	要素の矩形を再作成
**//*---------------------------------------------------------------------*/
void MenuWindow::remakeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, const SizeF32* sizeTxt)
{
	rectBlock->copy(block->_rect);
	const RectF32* rectTextNow = block->_strdraw->getRectangle();
	rectText->set(rectTextNow->x(), rectTextNow->y(), sizeTxt->w(), sizeTxt->h());
}

/*---------------------------------------------------------------------*//**
	文字列を調整する
	
	@retval true 文字列を修正した
	@retval false 文字列を修正しなかった
**//*---------------------------------------------------------------------*/
bool MenuWindow::adjustBlockString(VcString* str, RectF32* rectText, StringDrawer* strdraw, const RectF32* rectBlock)
{
	return false;
}

/*---------------------------------------------------------------------*//**
	要素取得
**//*---------------------------------------------------------------------*/
MenuWindow::Block* MenuWindow::getBlock(s32 index) const
{
	ListIterator<Block*> it = _listBlock->iterator();
	while(it.has())
	{
		Block* block = it.next();
		if(block->_index == index)
		{
			return block;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	位置より要素取得
**//*---------------------------------------------------------------------*/
MenuWindow::Block* MenuWindow::getBlock(const PointF32* pt) const
{
	ListIterator<Block*> it = _listBlock->iterator();
	while(it.has())
	{
		Block* block = it.next();
		RectF32 rect = *block->_rect;
		rect.x() -= _ptScrollTrg->x();
		rect.y() -= _ptScrollTrg->y();
		if(rect.isPointIn(pt))
		{
			return block;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
void MenuWindow::onCreatePanel()
{
}

/*---------------------------------------------------------------------*//**
	破棄通知
**//*---------------------------------------------------------------------*/
void MenuWindow::onDestroyPanel()
{
	destroy();
}

/*---------------------------------------------------------------------*//**
	現在のカーソル位置へのスクロール
**//*---------------------------------------------------------------------*/
void MenuWindow::scrollCurIndex()
{
	const s32 X_PADDING = 0;
	const s32 Y_PADDING = 0;
	if((0 <= _indexCur) && ((u32)_indexCur < _listBlock->getCount()))
	{
		Block* block = getBlock(_indexCur);
		ASSERT(block != 0L);
		const RectF32* rectBl = block->_strdraw->getRectangle();
		const RectF32* rectBase = this->getRectangle();
		RectF32 rectWork(rectBase->x() + X_PADDING, rectBase->y() + Y_PADDING, rectBase->w() - X_PADDING * 2, rectBase->h() - Y_PADDING * 2);
		
		if(((s32)(rectBl->y() + rectBl->h()) - _ptScrollTrg->y()) > (rectWork.y() + rectWork.h()))
		{
			_ptScrollTrg->y() = (s32)(rectBl->y() + rectBl->h()) - (rectWork.y() + rectWork.h());
		}
		else if(((s32)rectBl->y() - _ptScrollTrg->y()) < rectWork.y())
		{
			_ptScrollTrg->y() = (s32)rectBl->y() - rectWork.y();
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MenuWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	MenuExecRes* cres = (MenuExecRes*)res;

	if(((GameExecCtx*)ec)->getTouchPanel() != 0L)
	{
		TouchPanel* tcp = ((GameExecCtx*)ec)->getTouchPanel();
		Pad* pad = 0L;

		// スクロールバーの制御
		if(!tcp->isReacted())
		{
			ScrollBar::ScrollBarExecRes sbres;
			_scrlbar->exec(&sbres, tcp);
			if(sbres.getAction() != ScrollBar::ScrollBarExecRes::ACT_NULL)
			{
				if((sbres.getAction() == ScrollBar::ScrollBarExecRes::ACT_FLIP_UP) || (sbres.getAction() == ScrollBar::ScrollBarExecRes::ACT_FLIP_DOWN))
				{
					_velFlip += sbres.getValue() * RATE_FLIP_TO_VEL;	// フリップ速度を足す
				}
				else
				{
					f32 yPos = _ptScrollTrg->y();
					switch(sbres.getAction())
					{
					case ScrollBar::ScrollBarExecRes::ACT_CLICK_UP_ARROW:		yPos -= 10;							break;
					case ScrollBar::ScrollBarExecRes::ACT_CLICK_DOWN_ARROW:		yPos += 10;							break;
					case ScrollBar::ScrollBarExecRes::ACT_CLICK_PAGE_UP:		yPos -= _scrlbar->getScrollPage();	break;
					case ScrollBar::ScrollBarExecRes::ACT_CLICK_PAGE_DOWN:		yPos += _scrlbar->getScrollPage();	break;
					case ScrollBar::ScrollBarExecRes::ACT_DRAG_KNOB:			yPos = sbres.getValue();			_isScorllBarKnobDrag = true;	break;
					}
					if(yPos < _scrlbar->getScrollMin())	{	yPos = _scrlbar->getScrollMin();	}
					if(yPos > _scrlbar->getScrollMax())	{	yPos = _scrlbar->getScrollMax();	}
					if(yPos != _ptScrollTrg->y())
					{
						_ptScrollTrg->y() = yPos;
					}
					_velFlip /= 4;	// フリップ速度の急減速
				}
			}
		}

		if(!tcp->isReacted())
		{
			PointF32 ptTouch, ptStart;
			if((tcp->getLastAction() == TouchPanel::ACT_DRAG) && _scrlbar->isShow())		// ドラッグ
			{
				PointF32 ptPrev;
				tcp->getDragParam(&ptStart, &ptTouch, &ptPrev);
				onDrag(cres, tcp, &ptTouch, &ptPrev, &ptStart);
			}
			else if((tcp->getLastAction() == TouchPanel::ACT_FLIP) && _scrlbar->isShow())	// フリップ
			{
				f32 speed;
				tcp->getFlipParam(&ptStart, &speed, &ptTouch, 0L);
				onFlip(cres, tcp, &ptTouch, &ptStart, speed);
			}
			else if(tcp->getTouchPosition(&ptTouch))	// タッチ中
			{
				onTouching(cres, tcp, &ptTouch, getBlock(&ptTouch));
			}
			else if(tcp->isDoubleClickRelease(&ptTouch))
			{
				onDoubleTapRelease(cres, tcp, &ptTouch, getBlock(&ptTouch));
			}
			else if(tcp->isTapRelease(&ptTouch, &ptStart))	// タップ後
			{
				onTapRelease(cres, tcp, &ptTouch, &ptStart, getBlock(&ptTouch));
			}
			else if((pad != 0L) && pad->isTrigger(Pad::KEY_OK))		// パッドの OK ボタン
			{
				onPadOk(cres, pad);
			}
			else if((pad != 0L) && pad->isTrigger(Pad::KEY_CANCEL))	// パッドのキャンセルボタン
			{
				onPadCancel(cres, pad);
			}
			else if((pad != 0L) && pad->isTrigger(Pad::KEY_UP))		// パッドの上ボタン
			{
				onPadUp(cres, pad);
			}
			else if((pad != 0L) && pad->isTrigger(Pad::KEY_DOWN))		// パッドの下ボタン
			{
				onPadDown(cres, pad);
			}
			else if((pad != 0L) && pad->isTrigger(Pad::KEY_LEFT))		// パッドの左ボタン
			{
				onPadLeft(cres, pad);
			}
			else if((pad != 0L) && pad->isTrigger(Pad::KEY_RIGHT))		// パッドの右ボタン
			{
				onPadRight(cres, pad);
			}
		}
	}

	// フリップスクロール
	if(_velFlip != 0)
	{
		f32 yPos = _ptScrollTrg->y() + (_velFlip / RATE_POS_TO_VEL);
		if(yPos < _scrlbar->getScrollMin())	{	yPos = _scrlbar->getScrollMin();	}
		if(yPos > _scrlbar->getScrollMax())	{	yPos = _scrlbar->getScrollMax();	}
		if(yPos != _ptScrollTrg->y())		{	_ptScrollTrg->y() = yPos;			}
		_velFlip = _velFlip * 0.8f;	// フリップ速度の減速
		if(TFW_ABS(_velFlip) <= 1)	{	_velFlip = 0;	}
	}

	// スクロール位置の制御
	{
		const f32 rateIntp = 0.5f;
		if(_ptScrollCur->x() != _ptScrollTrg->x())
		{
			if(TFW_ABS(_ptScrollCur->x() - _ptScrollTrg->x()) <= 2)	{	_ptScrollCur->x() = _ptScrollTrg->x();			}
			else	{	_ptScrollCur->x() = (_ptScrollCur->x() * (1.0f - rateIntp)) + (_ptScrollTrg->x() * rateIntp);	}
		}
		if(_ptScrollCur->y() != _ptScrollTrg->y())
		{
			if(TFW_ABS(_ptScrollCur->y() - _ptScrollTrg->y()) <= 2)	{	_ptScrollCur->y() = _ptScrollTrg->y();			}
			else	{	_ptScrollCur->y() = (_ptScrollCur->y() * (1.0f - rateIntp)) + (_ptScrollTrg->y() * rateIntp);	}
			_scrlbar->setScrollPos(_ptScrollCur->y());
		}
	}

	// ブロックのフレーム制御
	for(ListIterator<Block*> it = _listBlock->iterator(); it.has(); it.next())
	{
		it.object()->exec(ec);
	}

	_cntAnim++;
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - タッチ後の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onTapRelease(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, const PointF32* ptStart, Block* block)
{
	bool isScorllBarKnobDragBk = _isScorllBarKnobDrag;
	_isScorllBarKnobDrag = false;				// スクロールバーノブドラッグフラグのリセット
	_ptScrollTapStart->set(F32_PMAX, F32_PMAX);	// タップ開始時のスクロール値をリセット
	if(isScorllBarKnobDragBk)	// スクロールバーノブドラッグ後のタップリリースには反応しない
	{
		return;
	}

	// 子の要素によって挙動を変える
	bool isNearTap = true;	// タップ開始位置からの近さは見ない［2013/8/19］　(TFW_ABS(ptTouch->x() - ptStart->x()) <= 10) && (TFW_ABS(ptTouch->y() - ptStart->y()) <= 10);
	if(block == 0L)	// メニュー外をタップした
	{
		if((_depth != 0) || !TFW_IS_FLAG(_eflags, EF_NO_SELF_CANCEL))
		{
			TRACE("{MenuWindow::exec} Menu : CANCEL\n");
			*cres->returnValue() = MenuExecRes::ON_CANCEL;
			///ui->setReacted(true);
			Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_CANCEL, false);
			return;
		}
		else
		{
			*cres->returnValue() = MenuExecRes::ON_CLICKOUTSIDE;
			return;
		}
	}
	else if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))				// 無効な領域をタップした
	{
		TRACE("{MenuWindow::exec} Menu : NO ACTION\n");
	}
	else if(!block->isDisabled() && isNearTap)	// 要素をタップし、無効でない
	{
		TRACE("{MenuWindow::exec} Menu : ELEMENT\n");
		ASSERT(block->_tnode != 0L);
		ui->setReacted(true);
		const TreeNode<MenuTreeNode>* tnodeChild = block->_tnode->getChild();
		if(tnodeChild != 0L)
		{
			*cres->targetNode() = block->_tnode;
			_cntAnim = 0;	// アニメーションカウンタリセット
		
			const MenuTreeNode* mtnodeChild = tnodeChild->getObject();
			ASSERT(mtnodeChild != 0L);
			if(mtnodeChild->isFunction())			// ファンクションノード
			{
				if(!block->isDisabled() && !block->isUnused())	// 無効ノードや使用不能ノードは除外
				{
					*cres->returnValue() = MenuExecRes::ON_FUNC;
					Game::getGame()->getSoundManager()->playUiSe(TFW_IS_FLAG(_eflags, EF_FOR_USE) ? GameSoundDef::SE_OK : GameSoundDef::SE_CSR_MOVE, false);
				}
			}
			else if(mtnodeChild->isCustomChild())	// カスタムチャイルドノード
			{
				*cres->returnValue() = MenuExecRes::ON_CSTMCHLD;
				Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_CSR_MOVE, false);
				return;
			}
			else	// 下のノードを潜る
			{
				*cres->returnValue() = MenuExecRes::ON_DIVE;
				Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_CSR_MOVE, false);
				return;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - ダブルタッチ後の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onDoubleTapRelease(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, Block* block)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - タッチ中の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onTouching(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, Block* block)
{
	if(block != 0L)	// メニュー内をタップした
	{
		_indexCur = block->_index;	// カーソルを移動
		_cntAnim = 0;				// アニメーションカウンタリセット
		scrollCurIndex();			// スクロール処理
		if(ui->isTapTrigger(0L)) { _ptScrollTapStart->copy(_ptScrollTrg); }	// タップ開始時のスクロール値を保存
		ui->setReacted(true);
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - ドラッグ操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onDrag(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptCur, const PointF32* ptPrev, const PointF32* ptStart)
{
	if(getRectangle()->isPointIn(ptStart) && (_ptScrollTapStart->y() != F32_PMAX))
	{
		f32 yPos = _ptScrollTapStart->y() - (ptCur->y() - ptStart->y());
		if(yPos < _scrlbar->getScrollMin())	{	yPos = _scrlbar->getScrollMin();	}
		if(yPos > _scrlbar->getScrollMax())	{	yPos = _scrlbar->getScrollMax();	}
		if(yPos != _ptScrollTrg->y())
		{
			_ptScrollTrg->y() = yPos;
		}
		_velFlip /= 4;	// フリップ速度の急減速
		ui->setReacted(true);
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - フリップ操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onFlip(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptCur, const PointF32* ptStart, f32 speed)
{
	// 要素自体のドラッグ／フリップによるスクロールの反応矩形
	RectF32 rectBlockScroll(getRectangle());
	rectBlockScroll.w() -= W_SCRBAR_PAD + X_SCRBARWD;	// スクロールバーはスクロールに反応しない
	if(rectBlockScroll.isPointIn(ptStart))
	{
		// フリップ速度を足す
		if(ptCur->y() > ptStart->y())
		{
			_velFlip -= speed * RATE_FLIP_TO_VEL;
			ui->setReacted(true);
		}
		else if(ptCur->y() < ptStart->y())
		{
			_velFlip += speed * RATE_FLIP_TO_VEL;
			ui->setReacted(true);
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - パッドの OK 操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onPadOk(MenuExecRes* cres, Pad* ui)
{
	Block* block = getBlock(_indexCur);
	ASSERT(block != 0L);
	ASSERT(block->_tnode != 0L);
	const TreeNode<MenuTreeNode>* tnodeChild = block->_tnode->getChild();
	if(tnodeChild != 0L)
	{
		*cres->targetNode() = block->_tnode;
		_cntAnim = 0;	// アニメーションカウンタリセット
		
		const MenuTreeNode* mtnodeChild = tnodeChild->getObject();
		ASSERT(mtnodeChild != 0L);
		if(mtnodeChild->isFunction())			// ファンクションノード
		{
			if(!block->isDisabled())	// 無効ノードは除外
			{
				*cres->returnValue() = MenuExecRes::ON_FUNC;
			}
		}
		else if(mtnodeChild->isCustomChild())	// カスタムチャイルドノード
		{
			*cres->returnValue() = MenuExecRes::ON_CSTMCHLD;
			return;
		}
		else	// 下のノードを潜る
		{
			*cres->returnValue() = MenuExecRes::ON_DIVE;
			return;
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - パッドのキャンセル操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onPadCancel(MenuExecRes* cres, Pad* ui)
{
	*cres->returnValue() = MenuExecRes::ON_CANCEL;
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - パッドの左レバー操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onPadLeft(MenuExecRes* cres, Pad* ui)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - パッドの上レバー操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onPadUp(MenuExecRes* cres, Pad* ui)
{
	// カーソル移動処理
	if(_indexCur > 0)				{	_indexCur--;	}
	else							{	_indexCur = _indexNum - 1;	}
	_cntAnim = 0;	// アニメーションカウンタリセット
	
	scrollCurIndex();	// スクロール処理
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - パッドの右レバー操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onPadRight(MenuExecRes* cres, Pad* ui)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理 - パッドの下操作の応答
**//*---------------------------------------------------------------------*/
void MenuWindow::onPadDown(MenuExecRes* cres, Pad* ui)
{
	// カーソル移動処理
	if(_indexCur < (_indexNum - 1))	{	_indexCur++;	}
	else							{	_indexCur = 0;	}
	_cntAnim = 0;	// アニメーションカウンタリセット
	
	scrollCurIndex();	// スクロール処理
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void MenuWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	Renderer* rdr = rc->getRenderer();

	// スクロールバーの描画
	if(TFW_IS_FLAG(getFlags(), F_FRONT))	// 前面のときのみ
	{	
		_scrlbar->draw(rc);
	}

	// 要素ごとに描画
	if(TFW_IS_FLAG(_eflags, EF_REVERSE_DRAW))	// 下から描画
	{
		ListIterator<Block*> it = _listBlock->iteratorEnd();
		while(it.has())
		{
			Block* block = it.prev();

			// ウインドウ外は描画しない（単純な縦並びを想定）
			if(block->_rectOfsAnim != 0L)	// アニメーションあり
			{
				f32 y = block->_rect->y() + (s32)block->_rectOfsAnim->y() - _ptScrollCur->y();
				if(rectBase->y() > (y + block->_rect->h()))	{	continue;	}
				if((rectBase->y() + rectBase->h()) < y)		{	continue;	}
			}
			else							// アニメーションなし（※全てのブロックも同様と想定）
			{
				f32 y = block->_rect->y() - _ptScrollCur->y();
				if(rectBase->y() > (y + block->_rect->h()))	{	break;		}	// これより上はないはず
				if((rectBase->y() + rectBase->h()) < y)		{	continue;	}
			}

			// ブロック描画
			drawBlock(rc, block, (block->_index == _indexCur) && !block->isDisabled());
		}
	}
	else	// 上から描画
	{
		ListIterator<Block*> it = _listBlock->iterator();
		while(it.has())
		{
			Block* block = it.next();

			// ウインドウ外は描画しない（単純な縦並びを想定）
			if(block->_rectOfsAnim != 0L)	// アニメーションあり
			{
				f32 y = block->_rect->y() + (s32)block->_rectOfsAnim->y() - _ptScrollCur->y();
				if(rectBase->y() > (y + block->_rect->h()))	{	continue;	}
				if((rectBase->y() + rectBase->h()) < y)		{	continue;	}
			}
			else							// アニメーションなし（※全てのブロックも同様と想定）
			{
				f32 y = block->_rect->y() - _ptScrollCur->y();
				if(rectBase->y() > (y + block->_rect->h()))	{	continue;	}
				if((rectBase->y() + rectBase->h()) < y)		{	break;		}	// これより下はないはず
			}

			// ブロック描画
			drawBlock(rc, block, (block->_index == _indexCur) && !block->isDisabled());
		}
	}
}

/*---------------------------------------------------------------------*//**
	ブロック描画
**//*---------------------------------------------------------------------*/
void MenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	Renderer* rdr = rc->getRenderer();

	if(!block->isHidden())	// 隠し属性でなければ
	{
		ColorU8 col1, col2, col3, col4;
		RectF32 rectBlock((f32)(block->_rect->x() - getCurScrollPos()->x()), (f32)(block->_rect->y() - getCurScrollPos()->y()), (f32)block->_rect->w(), (f32)block->_rect->h());

		// 矩形描画
		if(isFocus)	// フォーカスされている
		{
			RendererUtils::draw2dColorRect(rdr, &rectBlock, ColorU8::setout(&col1, 255, 191, 191, 63), ColorU8::setout(&col2, 204, 188, 188, 63), ColorU8::setout(&col3, 200, 163, 163, 63), ColorU8::setout(&col4, 180, 147, 147, 63));
		}
		else
		{
			RendererUtils::draw2dColorRect(rdr, &rectBlock, ColorU8::setout(&col1, 255, 255, 255, 63), ColorU8::setout(&col2, 204, 255, 255, 63), ColorU8::setout(&col3, 200, 212, 212, 63), ColorU8::setout(&col4, 180, 202, 202, 63));
		}

		// テキスト描画
		if(block->isDisabled() || block->isUnused())
		{
			rdr->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), 255);	// 無効色
		}
		else
		{	
			rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), 255);	// 有効色
		}
		block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x(), - (f32)getCurScrollPos()->y());
	}
}

//==========================================================================
// MenuWindow::Block メソッド

/*---------------------------------------------------------------------*//**
	無効かどうかを得る
**//*---------------------------------------------------------------------*/
bool MenuWindow::Block::isDisabled() const
{
	#if MWBLOCK_FLAG_STATE
		return TFW_IS_FLAG(_flags, Block::F_DISABLED);
	#else
		return _tnode->getObject()->isDisabled();
	#endif
}

/*---------------------------------------------------------------------*//**
	使用不可かどうかを得る
**//*---------------------------------------------------------------------*/
bool MenuWindow::Block::isUnused() const
{
	#if MWBLOCK_FLAG_STATE
		return TFW_IS_FLAG(_flags, Block::F_UNUSED);
	#else
		return _tnode->getObject()->isUnused();
	#endif
}

/*---------------------------------------------------------------------*//**
	隠し中かどうかを得る
**//*---------------------------------------------------------------------*/
bool MenuWindow::Block::isHidden() const
{
	#if MWBLOCK_FLAG_STATE
		return TFW_IS_FLAG(_flags, Block::F_HIDE);
	#else
		return _tnode->getObject()->isHidden();
	#endif
}


/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MenuWindow::Block::Block()
	: _index(-1)
	, _str(0L)
	, _tnode(0L)
	, _strdraw(0L)
	, _rect(0L)
	, _sizeTxt(0L)
	, _color(0L)
	, _typef(0)
	, _flags(0)
	, _rectOfs(0L)
	, _rectOfsAnim(0L)
	, _deltaOfsAnim(0.0f)
	, _rateOfsAnim(0.0f)
	, _param(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MenuWindow::Block::~Block()
{
	ASSERT(_str == 0L);
	ASSERT(_rect == 0L);
	ASSERT(_sizeTxt == 0L);
	ASSERT(_color == 0L);
	ASSERT(_strdraw == 0L);
	ASSERT(_rectOfs == 0L);
	ASSERT(_rectOfsAnim == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MenuWindow::Block::create(const VcString* str, u32 flags, u64 param, TreeNode<MenuTreeNode>* tnode, s32 index)
{
	_strdraw = new StringDrawer();
	if(!_strdraw->create())
	{
		return false;
	}
	
	_index = index;
	_str = new VcString(str);
	_tnode = tnode;
	_strdraw->setString(_str);
	
	if((flags & COL_EXTLINK) != 0)	{	_color = new Vector4F(1.0f, 0.8f, 1.0f, 1.0f);	}
	else							{	_color = new Vector4F(1.0f, 1.0f, 1.0f, 1.0f);	}

	const MenuTreeNode* mtnode = tnode->getObject();
	#if MWBLOCK_FLAG_STATE
		TFW_SET_FLAG(_flags, F_HIDE, mtnode->isHidden());
		TFW_SET_FLAG(_flags, F_DISABLE, mtnode->isDisabled());
	#endif

	_param = param;
	
	_rect = new RectF32();
	_sizeTxt = new SizeF32();
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MenuWindow::Block::destroy()
{
	if(_strdraw != 0L)
	{
		_strdraw->destroy();
		delete _strdraw;
		_strdraw = 0L;
	}

	delete _str;
	_str = 0L;
	delete _sizeTxt;
	_sizeTxt = 0L;
	delete _rect;
	_rect = 0L;
	delete _color;
	_color = 0L;
	delete _rectOfs;
	_rectOfs = 0L;
	delete _rectOfsAnim;
	_rectOfsAnim = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MenuWindow::Block::exec(const ExecCtx* ec)
{
	// オフセットアニメ
	if((_rectOfsAnim != 0L) && (_rectOfs != 0L))
	{
		for(int i = 0; i < 4; i++)
		{
			if(_rectOfsAnim->_v[i] > _rectOfs->_v[i])
			{
				f32 diff = _rectOfsAnim->_v[i] - _rectOfs->_v[i];		// 差を計算する
				diff *= _rateOfsAnim;									// 差を比で縮める
				diff = diff - (ec->getDeltaFrame() * _deltaOfsAnim);	// 目的方向に進める
				if(diff < 0.0f)	{	_rectOfsAnim->_v[i] = (f32)_rectOfs->_v[i];		}
				else			{	_rectOfsAnim->_v[i] = _rectOfs->_v[i] + diff;	}
			}
			else if(_rectOfsAnim->_v[i] < _rectOfs->_v[i])
			{
				f32 diff = _rectOfs->_v[i] - _rectOfsAnim->_v[i];		// 差を計算する
				diff *= _rateOfsAnim;									// 差を比で縮める
				diff = diff - (ec->getDeltaFrame() * _deltaOfsAnim);	// 目的方向に進める
				if(diff < 0.0f)	{	_rectOfsAnim->_v[i] = (f32)_rectOfs->_v[i];		}
				else			{	_rectOfsAnim->_v[i] = _rectOfs->_v[i] - diff;	}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	差分移動
**//*---------------------------------------------------------------------*/
void MenuWindow::Block::moveDiff(f32 xDiff, f32 yDiff)
{
	if((xDiff != 0) || (yDiff != 0))
	{
		_rect->x() += xDiff;
		_rect->y() += yDiff;

		RectF32 rectTd = *_strdraw->getRectangle();
		rectTd.x() += xDiff;
		rectTd.y() += yDiff;
		_strdraw->setRectangle(&rectTd);
	}
}

/*---------------------------------------------------------------------*//**
	オフセットアニメ変数を有効化
**//*---------------------------------------------------------------------*/
void MenuWindow::Block::enableOffsetAnim(f32 deltaOfsAnim, f32 rateOfsAnim)
{
	if(_rectOfs == 0L)		{	_rectOfs = new RectF32();	}
	if(_rectOfsAnim == 0L)	{	_rectOfsAnim = new RectF32();	}

	_deltaOfsAnim = deltaOfsAnim;
	_rateOfsAnim = rateOfsAnim;
}

/*---------------------------------------------------------------------*//**
	内容を再設定
**//*---------------------------------------------------------------------*/
void MenuWindow::Block::set(const VcString* str, u32 flags, u64 param, const MenuTreeNode* mtnode)
{
	_str->set(str);
	_strdraw->setString(_str);

	_flags = flags;
	if((flags & COL_EXTLINK) != 0)	{	_color->set(1.0f, 0.8f, 1.0f, 1.0f);	}
	else							{	_color->set(1.0f, 1.0f, 1.0f, 1.0f);	}

	_param = param;
	
	_tnode->object()->copy(mtnode);
	#if MWBLOCK_FLAG_STATE
		TFW_SET_FLAG(_flags, F_HIDE, mtnode->isHidden());
		TFW_SET_FLAG(_flags, F_DISABLE, mtnode->isDisabled());
	#endif
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
