/***********************************************************************//**
 *	FocusCursor.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FocusCursor.h"

// Friends
#include "FocusMenu.h"
#include "GameFontSet.h"
#include "../common/CalcUtils.h"
#include "../episode/party/MyParty.h"
#include "../event/EventIdInfoArray.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameRenderCtx.h"
#include "../item/Item.h"
#include "../magic/MagicCluster.h"	// for Souma's target
#include "../magic/MagicSys.h"		// for Souma's target
#include "../map/PlacementObj.h"
#include "../spirit/Spirit.h"
#include "../unit/Unit.h"
#include "../unit/UnitManager.h"
#include "../unit/char/CharUnit.h"
#include "../unit/char/npc/NpcCharUnit.h"
#include "../unit/item/PutItemUnit.h"
#include "../unit/item/EquipmentItemUnit.h"
#include "../status/ActStat.h"
#include "../status/CharStat.h"
#include "../status/StatusDrawer.h"
#include "../status/FocusArray.h"

// External
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/EasyStringDrawer.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/gui/Panel.h"
#include "../../tfw/lib/Calc.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 制御情報
#define RANGE_TOTRG				(30.0f)						// クリック位置とターゲットの許容範囲

// 回転角度
#define DANG_FOCUS_DELTA		(2.5f)
#define DANG_FOCUSABLE_DELTA	(0.5f)

// テクスチャ情報
#define W_TEX					(1024.0f)						// テクスチャ全体の幅
#define H_TEX					(1024.0f)						// テクスチャ全体の高さ
#define W_FOCUS_CIRC			(64)							// フォーカスカーソルの幅
#define H_FOCUS_CIRC			(64)							// フォーカスカーソルの高さ
#define UV_FOCUS				288.0f, 960.0f, 64.0f, 64.0f	// フォーカスカーソルのテクスチャ座標
#define UV_FOCUSABLE			352.0f, 960.0f, 64.0f, 64.0f	// フォーカスカーソルのテクスチャ座標
#define W_QUEST_ICON			(8)								// クエストアイコンの幅
#define H_QUEST_ICON			(8)								// クエストアイコンの高さ
#define UV_QUEST_ACCEPTABLE		168.0f, 984.0f, 8.0f, 8.0f		// クエスト受諾可能
#define UV_QUEST_CANCELABLE		176.0f, 984.0f, 8.0f, 8.0f		// クエスト中止可能
#define UV_QUEST_FINISHABLE		184.0f, 984.0f, 8.0f, 8.0f		// クエスト終了可能

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FocusCursor::Focus メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FocusCursor::Focus::Focus()
	: _unitRef(0L)
	, _posScr(new Vector2F(F32_PMAX, F32_PMAX))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FocusCursor::Focus::~Focus()
{
	delete _posScr;
}

//==========================================================================
// FocusCursor メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フォーカスを得る
**//*---------------------------------------------------------------------*/
Unit* FocusCursor::getFocus(s32 index) const
{
	return _focusarr->unit(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フォーカス設定（主に外部からの指定）
**//*---------------------------------------------------------------------*/
void FocusCursor::setFocus(s32 index, Unit* unitRef)
{
	setFocus(index, unitRef, 0L);
}

/*---------------------------------------------------------------------*//**
	オーナー スピリットの設定
**//*---------------------------------------------------------------------*/
void FocusCursor::setOwnerSpirit(Spirit* sprt)
{
	_sprtOwnrRef = sprt;
}

/*---------------------------------------------------------------------*//**
	タップ防止カウンタの設定
**//*---------------------------------------------------------------------*/
void FocusCursor::setPreventTapCounter(f32 framecnt)
{
	_fcntPreventTap = framecnt;
	if(_fcntPreventTap < 0.0f)	{	_fcntPreventTap = 0.0f;	}
}

/*---------------------------------------------------------------------*//**
	全てのフォーカスを外す
**//*---------------------------------------------------------------------*/
void FocusCursor::resetAllFocus()
{
	for(s32 i = 0; i < NUM_FOCUS_MAX; i++)
	{
		setFocus(i, 0L, 0L);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FocusCursor::FocusCursor()
	: _isEnableOut(false)
	, _isEnableSelf(false)
	, _fcntPreventTap(0.0f)
	, _dangFocus(0.0f)
	, _dangFocusable(0.0f)
	, _focusarr(0L)
	, _texRef(0L)
	, _fcmenuRef(0L)
	, _sprtOwnrRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FocusCursor::~FocusCursor()
{
	ASSERT(_focusarr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FocusCursor::create(Texture* texRef, FocusMenu* fcmenuRef)
{
	// フォーカス配列の作成
	_focusarr = new FocusArray(NUM_FOCUS_MAX, true);

	_isEnableSelf = true;	// 自身を有効に
	_texRef = texRef;		// テクスチャポインタを保持
	_fcmenuRef = fcmenuRef;	// フォーカスメニューポインタを保持

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FocusCursor::destroy()
{
	_isEnableSelf = false;	// 自身に無効に
	_texRef = 0L;			// テクスチャポインタをクリア
	_fcmenuRef = 0L;		// フォーカスメニューポインタをクリア

	// フォーカス配列の削除
	delete _focusarr;
	_focusarr = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void FocusCursor::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isEnableOut)	{	return;	}	// 外部からの無効化

	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 

	// フォーカスの有効性更新
	_focusarr->updateValidity();
	// フォーカスの座標値をリセット
	for(s32 i = 0; i < NUM_FOCUS_MAX; i++)
	{
		_focusarr->invalidateScreenPos(i);
	}

	// 有効性判定
	_isEnableSelf = !gec->isLockOperating();
	bool isEnableUiReact = _isEnableSelf && !gec->isOpenedLockGui() && !ui->isReacted() && (_fcntPreventTap == 0.0f);

	// タッチ操作による処理
	if(isEnableUiReact) 
	{
		PointF32 ptTouch, ptStart;
		bool isTap = false;
		bool isDblTapRel = false;
		if(!ui->isReacted())
		{
			const f32 DIFF_TAP_RANGE = 20.0f;
			if(ui->isTapRelease(&ptTouch, &ptStart)) { isTap = ptTouch.isNear(&ptStart, DIFF_TAP_RANGE, DIFF_TAP_RANGE); }	// 面積が狭いので、isClickRelease だとストレスに感じる
			if(ui->isDoubleClickRelease(&ptTouch)) { isDblTapRel = true; }
		}
		if(isTap || isDblTapRel)
		{
			Tfw* tfw = Game::getGame()->getTfw();						ASSERT(tfw != 0L);
			View* view = tfw->getView();								ASSERT(view != 0L);
			SorSgm* sgm = (SorSgm*)tfw->getSgManager();					ASSERT(sgm != 0L);
			Camera* cam = sgm->getVisibleCamera();						ASSERT(cam != 0L);

			UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
			for(int i = 0; i < unitmng->getUnitNum(); i++)
			{
				Unit* unit = unitmng->getUnitFromIndex(i);				ASSERT(unit != 0L);
				if(!unit->isEnable())		{	continue;	}	// 無効なユニットは除外
				if(!unit->isEnableFocus())	{	continue;	}	// フォーカスされないユニットは除外
				if(unit->isHidden())		{	continue;	}	// 隠し属性ユニットは除外
				if(unit->getUnitType() == UNITTYPE_EQUIPMENT_ITEM)	{	continue;	}	// 所持品は除外
				if(unit->getCenterPos()->z() >= cam->getLoc()->z())	{	continue;	}	// カメラより手前は除外

				// スクリーン座標に変換
				Vector2F vScr;
				Gcalc::conv3dPosToScreenPos(&vScr, unit->getCenterPos(), cam, view);

				if(isDblTapRel)	// ダブルクリック時は直接フォーカスをセットする
				{
					// 一度フォーカスをリセット
					for(int j = 0; j < NUM_FOCUS_MAX; j++)
					{
						setFocus(j, 0L, 0L);
					}
					// 範囲内かどうか判定して、範囲内だったらフォーカス設定する
					if(TFW_IS_NEAR(vScr.x(), ptTouch.x(), RANGE_TOTRG) && TFW_IS_NEAR(vScr.y(), ptTouch.y(), RANGE_TOTRG))
					{
						// フォーカスをセット（上部でリセットしているので、[0] 決めうち）
						setFocus(0, unit, &vScr);
						// UI に反応済みフラグを立てる
						ui->setReacted(true);
						break;
					}
				}
				else if(isTap)	// タップ時はメニューを表示
				{
					// 範囲内かどうか判定して、範囲内だったらメニューを表示する
					TRACELN("{FocusCursor::exec} unit touch?; scr_x=%f, scr_y=%f, click_x=%d, click_y=%d", vScr.x(), vScr.y(), ptTouch.x(), ptTouch.y());
					if(TFW_IS_NEAR(vScr.x(), ptTouch.x(), RANGE_TOTRG) && TFW_IS_NEAR(vScr.y(), ptTouch.y(), RANGE_TOTRG))
					{
						showFocusMenu(unit, &vScr);		// フォーカスメニュー表示
						ui->setReacted(true);	// UI に反応済みフラグを立てる
					}
				}
			}
		}
	}

	// フォーカスカーソルの画面位置を計算
	for(s32 i = 0; i < NUM_FOCUS_MAX; i++)
	{
		// 無効になったユニットは外す
		const Unit* unitFocused = _focusarr->getUnit(i);
		if(unitFocused == 0L)	{	continue;	}
		if(!unitFocused->isEnable())
		{
			setFocus(i, 0L, 0L);
		}

		// スクリーン座標値が未計算の場合、計算する
		if(!_focusarr->isValidScreenPos(i))
		{
			_focusarr->calcScreenPos(i);
		}
	}

	// マジッククラスターのフォーカスの画面位置を計算
	MagicSys* mgsys = Game::getGame()->getMagicSys();
	for(int i = 0; i < mgsys->getMaxClusterNum(); i++)
	{
		MagicCluster* mc = mgsys->getClusterFromIndex(i);
		if(mc == 0L)		{	continue;	}
		if(!mc->isValid())	{	continue;	}
		mc->focusArray()->calcScreenPosAll();
	}

	// タップ防止カウンタの更新
	if(_fcntPreventTap > 0.0f)
	{
		_fcntPreventTap -= ec->getDeltaFrame();
		if(_fcntPreventTap < 0.0f)	{	_fcntPreventTap = 0.0f;	}
	}

	// カーソル回転角度の更新
	_dangFocus += DANG_FOCUS_DELTA;
	if(_dangFocus > 180.0f)		{	_dangFocus -= 360.0f;		}
	_dangFocusable += DANG_FOCUSABLE_DELTA;
	if(_dangFocusable > 180.0f)	{	_dangFocusable -= 360.0f;	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void FocusCursor::draw(const RenderCtx* rc)
{
	if(!_isEnableOut || !_isEnableSelf)	{	return;	}	// 無効化中
	
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = rc->getRenderer();
	RectF32 uv;

	// テクスチャ設定
	rdr->bindTexture(_texRef);
	// 加算アルファ合成
	rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);
	// 2D 解像描画比
	f32 rate2dr = Env_get2drRate();
	// 頂点カラーを基本色に
	rdr->setSolidColor(255, 255, 255, 255);

	// クリック対象を描画
	{
		Tfw* tfw = Game::getGame()->getTfw();						ASSERT(tfw != 0L);
		View* view = tfw->getView();								ASSERT(view != 0L);
		SorSgm* sgm = (SorSgm*)tfw->getSgManager();					ASSERT(sgm != 0L);
		Camera* cam = sgm->getVisibleCamera();
		if(cam != 0L)
		{
			UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
			for(int i = 0; i < unitmng->getUnitNum(); i++)
			{
				const Unit* unit = unitmng->getUnitFromIndex(i); ASSERT(unit != 0L);
				if(!unit->isEnable())				{	continue;	}	// 無効なユニットは除外
				if(!unit->isEnableFocus())			{	continue;	}	// フォーカスされないユニットは除外
				if(unit->isHidden())				{	continue;	}	// 隠し属性ユニットは除外
				if(unit->isHiddenClick())			{	continue;	}	// クリック選択不可
				if(unit->getUnitType() == UNITTYPE_EQUIPMENT_ITEM)	{	continue;	}	// 所持品は除外
				if(unit->getCenterPos()->z() >= cam->getLoc()->z())	{	continue;	}	// カメラより手前は除外

				// 既にフォーカスがあるユニットは不要
				bool isHadFocus = false;
				for(s32 j = 0; j < NUM_FOCUS_MAX; j++)
				{
					if(_focusarr->getUnit(j) == unit)
					{
						isHadFocus = true;
						break;
					}
				}
				if(isHadFocus)						{	continue;	}

				// スクリーン座標に変換
				Vector2F vScr;
				Gcalc::conv3dPosToScreenPos(&vScr, unit->getCenterPos(), cam, view);

				// 画面外チェック
				if(	((vScr.x() + W_FOCUS_CIRC) < 0) ||
					((vScr.x() - W_FOCUS_CIRC) >= Game::getGame()->getLogicalWidth()) ||
					((vScr.y() + H_FOCUS_CIRC) < 0) ||
					((vScr.y() - H_FOCUS_CIRC) >= Game::getGame()->getLogicalHeight()) )
				{
					continue;
				}

				// 描画
				::glPushMatrix();	// 行列を保存
				::glTranslatef(vScr.x(), vScr.y(), 0.0f);
				drawQuestIcon(rdr, unit, rate2dr);	// クエストアイコン表示
				::glRotatef(_dangFocusable, 0.0f, 0.0f, 1.0f);
				// フォーカス可能アイコン表示
				RectF32 vtx(- W_FOCUS_CIRC / 2, - H_FOCUS_CIRC / 2, W_FOCUS_CIRC, H_FOCUS_CIRC);
				RendererUtils::draw2dTextureRect(rdr, &vtx, Gcalc::calcTexUv(&uv, UV_FOCUSABLE, W_TEX, H_TEX, rate2dr));
				::glPopMatrix();	// 行列を戻す
			}
		}
	}

	// フォーカスカーソル表示
	drawFocusCursor(rdr, _focusarr, 1.0f, rate2dr);

	// マジッククラスターのフォーカスを表示
	MagicSys* mgsys = Game::getGame()->getMagicSys();
	for(int i = 0; i < mgsys->getMaxClusterNum(); i++)
	{
		MagicCluster* mc = mgsys->getClusterFromIndex(i);
		if(mc == 0L)			{	continue;	}
		if(!mc->isValid())		{	continue;	}
		if(!TFW_IS_FLAG(mc->getConfFlags(), MagicCluster::CONFF_SHOW_FC))	{	continue;	}
		rdr->setSolidColor(63, 0, 255, 255);
		drawFocusCursor(rdr, mc->getFocusArray(), 0.5f, rate2dr);
	}

	// 通常アルファ合成
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);

	// フォーカスゲージ表示
	for(s32 i = 0; i < NUM_FOCUS_MAX; i++)
	{
		const Unit* unitFocused = _focusarr->getUnit(i);
		if(unitFocused == 0L)	{	continue;	}	// フォーカスがある場合のみ以下を処理する
		const Vector2F* posScr = _focusarr->getScreenPos(i);

		f32 x = posScr->x() + W_FOCUS_CIRC * 0.5f;
		f32 y = posScr->y();

		// ゲージ表示
		if(unitFocused->isCategory(Unit::UNITCAT_CHAR))
		{
			const CharUnit* cunit = (CharUnit*)unitFocused;
			const CharStat* cstat = cunit->getCharStat();
			if(cstat != 0L)
			{
				StatusDrawer::drawCharStat(rc, _texRef, cstat, x, y, StatusDrawer::W_GAUGE_DEFAULT, StatusDrawer::H_GAUGE_DEFAULT, false, false);
			}
		}
		else if(unitFocused->getUnitType() == UNITTYPE_EQUIPMENT_ITEM)
		{
			const EquipmentItemUnit* iunit = (EquipmentItemUnit*)unitFocused;
			const Item* item = iunit->getItem();
			if(item != 0L)
			{
				// 名前表示
				const VcString* name = item->getItemDef()->getName(Env_getLangId());
				if(name != 0L)
				{
					rdr->setSolidColor(255, 255, 255, 255);
					Font* fontJp = grc->getFontSet()->getFont(GameFontSet::JP);
					EasyStringDrawer::draw(fontJp, name, x, y, 8, rc);
					y += 8.0f;
				}

				const EleneStat* eestat = item->getEleneStat();
				if(eestat != 0L)
				{
					StatusDrawer::drawEleneStat(rc, _texRef, eestat, x, y, 60.0f, StatusDrawer::H_ENEGAUGE_DEFAULT, false);
				}
			}
		}
		else if(unitFocused->getUnitType() == UNITTYPE_PUT_ITEM)
		{
			const PutItemUnit* diunit = (PutItemUnit*)unitFocused;
			const ItemDef* itmdf = diunit->getItemDef();
			if(itmdf != 0L)
			{
				// 名前表示
				const VcString* name = itmdf->getName(Env_getLangId());
				if(name != 0L)
				{
					rdr->setSolidColor(255, 255, 255, 255);
					Font* fontJp = grc->getFontSet()->getFont(GameFontSet::JP);
					EasyStringDrawer::draw(fontJp, name, x, y, 8, rc);
					y += 8.0f;
				}
			}
			const EleneStat* eestat = diunit->getEleneStat();
			if(eestat != 0L)
			{
				StatusDrawer::drawEleneStat(rc, _texRef, eestat, x, y, 60.0f, StatusDrawer::H_ENEGAUGE_DEFAULT, false);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	ゲーム開始通知
**//*---------------------------------------------------------------------*/
void FocusCursor::notifyEnterGame()
{
	// フォーカスをリセット
	resetAllFocus();
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void FocusCursor::notifyChangeMapBefore()
{
	// フォーカスをリセット
	resetAllFocus();
}

/*---------------------------------------------------------------------*//**
	デシリアライズ（セーブデータ読み込み）通知
**//*---------------------------------------------------------------------*/
void FocusCursor::notifyDeserialize()
{
	// フォーカスを復帰
	UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unit = unitmng->getUnitFromIndex(i); ASSERT(unit != 0L);
		if(unit->isGuiFocused())
		{
			setFocus(0, unit);
		}
	}
}

/*---------------------------------------------------------------------*//**
	フォーカス設定（主に内部での指定）
**//*---------------------------------------------------------------------*/
void FocusCursor::setFocus(s32 index, Unit* unitRef, const Vector2F* vScrPos)
{
	if(unitRef == 0L)
	{
		// フォーカスとスクリーン座標をクリア
		Unit* unitFocused = _focusarr->unit(index);
		if(unitFocused != 0L)	{	unitFocused->setGuiFocused(false);	}
		_focusarr->setUnit(index, 0L);
		_focusarr->invalidateScreenPos(index);
	}
	else
	{
		if(!unitRef->isEnableFocus())	{	ASSERT(false);	return;	}	// フォーカス設定を許可されていない場合は抜ける

		// フォーカス設定
		_focusarr->setUnit(index, unitRef);
		unitRef->setGuiFocused(true);

		// スクリーン座標を設定
		if(vScrPos != 0L)
		{
			_focusarr->setCalculatedScreenPos(index, vScrPos);
		}
		else
		{
			_focusarr->calcScreenPos(index);
		}
	}

	// スピリットのフォーカスも変更
	if(_sprtOwnrRef != 0L)	// オーナーが設定されているときのみ
	{
		_sprtOwnrRef->setFocusUnit(unitRef, true);
	}
}

/*---------------------------------------------------------------------*//**
	フォーカス配列からフォーカスカーソルの描画
**//*---------------------------------------------------------------------*/
void FocusCursor::drawFocusCursor(Renderer* rdr, const FocusArray* fa, f32 scale, f32 rate2dr)
{
	if(!fa->isCalculateScreenPos())	{	return;	}

	RectF32 vtx(- W_FOCUS_CIRC / 2, - H_FOCUS_CIRC / 2, W_FOCUS_CIRC, H_FOCUS_CIRC);
	RectF32 uv;

	if(scale != 1.0f)
	{
		vtx *= scale;
	}

	// フォーカスカーソル表示
	for(s32 i = 0; i < fa->getCount(); i++)
	{
		const Unit* unitFocused = fa->getUnit(i);
		if(unitFocused == 0L)			{	continue;	}	// フォーカスがある場合のみ以下を処理する
		const Vector2F* posScr = fa->getScreenPos(i);

		// 画面外チェック
		if(	((posScr->x() + W_FOCUS_CIRC) < 0) ||
			((posScr->x() - W_FOCUS_CIRC) >= Game::getGame()->getLogicalWidth()) ||
			((posScr->y() + H_FOCUS_CIRC) < 0) ||
			((posScr->y() - H_FOCUS_CIRC) >= Game::getGame()->getLogicalHeight()) )
		{
			continue;
		}

		// 描画
		::glPushMatrix();	// 行列を保存
		::glTranslatef((f32)posScr->x(), (f32)posScr->y(), 0.0f);
		drawQuestIcon(rdr, unitFocused, rate2dr);	// クエストアイコン表示
		::glRotatef(_dangFocus, 0.0f, 0.0f, 1.0f);
		// フォーカスカーソル表示
		RendererUtils::draw2dTextureRect(
			rdr,
			&vtx,
			Gcalc::calcTexUv(&uv, UV_FOCUS, W_TEX, H_TEX, rate2dr)	);
		::glPopMatrix();	// 行列を戻す
	}
}

/*---------------------------------------------------------------------*//**
	クエストアイコンの描画
**//*---------------------------------------------------------------------*/
void FocusCursor::drawQuestIcon(Renderer* rdr, const Unit* unit, f32 rate2dr)
{
	if(unit->isCategory(Unit::UNITCAT_CHAR))
	{
		const s32 PADDING = -4;
		u16 qsflags = ((CharUnit*)unit)->getQuestFlags();
		RectF32 vtx;
		RectF32 uv;
		if(TFW_IS_FLAG(qsflags, CharUnit::QSF_ACCEPTABLE))
		{
			vtx.set(- (W_FOCUS_CIRC / 2) + W_FOCUS_CIRC + PADDING, - (H_FOCUS_CIRC / 2) - PADDING, W_QUEST_ICON, H_QUEST_ICON);
			Gcalc::calcTexUv(&uv, UV_QUEST_ACCEPTABLE, W_TEX, H_TEX, rate2dr);
		}
		else if(TFW_IS_FLAG(qsflags, CharUnit::QSF_CANCELABLE))
		{
			vtx.set(- (W_FOCUS_CIRC / 2) + W_FOCUS_CIRC + PADDING, - (H_FOCUS_CIRC / 2) - PADDING, W_QUEST_ICON, H_QUEST_ICON);
			Gcalc::calcTexUv(&uv, UV_QUEST_CANCELABLE, W_TEX, H_TEX, rate2dr);
		}
		else if(TFW_IS_FLAG(qsflags, CharUnit::QSF_FINISHABLE))
		{
			vtx.set(- (W_FOCUS_CIRC / 2) + W_FOCUS_CIRC + PADDING, - (H_FOCUS_CIRC / 2) - PADDING, W_QUEST_ICON, H_QUEST_ICON);
			Gcalc::calcTexUv(&uv, UV_QUEST_FINISHABLE, W_TEX, H_TEX, rate2dr);
		}
		RendererUtils::draw2dTextureRect(rdr, &vtx, &uv);
	}
}

/*---------------------------------------------------------------------*//**
	フォーカスメニューを開く
**//*---------------------------------------------------------------------*/
void FocusCursor::showFocusMenu(Unit* unit, const Vector2F* vScrPos)
{
	const s32 MTNODEID_DROPITEM_FOCUS		= 21001;
	const s32 MTNODEID_ENEMY_FOCUS			= 22001;
	const s32 MTNODEID_EXTRA_FOCUS			= 23001;
	const s32 MTNODEID_EXTRA_CHECK			= 23002;
	const s32 MTNODEID_MAPOBJ_FOCUS			= 24001;
	const s32 MTNODEID_MAPOBJ_CHECK			= 24002;

	// メニュー種別と準備
	FocusMenu::MenuKind mkind = FocusMenu::NUM_MENU;
	if(unit->getUnitType() == UNITTYPE_ENEMY_CHAR)
	{
		mkind = FocusMenu::MENU_ENEMY;

		// フォーカス
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_ENEMY_FOCUS, getFocus(0) != unit);
	}
	else if(unit->getUnitType() == UNITTYPE_EXTRA_CHAR)
	{
		mkind = FocusMenu::MENU_EXTRA;

		// フォーカス
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_EXTRA_FOCUS, getFocus(0) != unit);
	}
	else if(unit->getUnitType() == UNITTYPE_MAPOBJ)
	{	
		mkind = FocusMenu::MENU_MAPOBJ;

		// フォーカス
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_MAPOBJ_FOCUS, getFocus(0) != unit);

		// チェック
		bool isEnableCheck = false;
		if((unit != 0L) && TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
		{
			const PlacementObj* pobj = ((NpcCharUnit*)unit)->getPlacementObj();
			const EventIdInfoArray::EventIdInfo* cii = pobj->getCallEvi()->getInfo(pobj->getActiveCallEviIndex());
			if(cii != 0L)
			{
				isEnableCheck = true;
			}
		}
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_MAPOBJ_CHECK, isEnableCheck);
	}
	else if(unit->getUnitType() == UNITTYPE_PUT_ITEM)
	{
		mkind = FocusMenu::MENU_DROPITEM;

		// フォーカス
		_fcmenuRef->setMenuElementEnable(mkind, MTNODEID_DROPITEM_FOCUS, getFocus(0) != unit);
	}

	// メニューを開く
	if(mkind != FocusMenu::NUM_MENU)
	{
		PointF32 pt(vScrPos->x(), vScrPos->y());
		_fcmenuRef->showMenu(mkind, &pt, unit, true);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

