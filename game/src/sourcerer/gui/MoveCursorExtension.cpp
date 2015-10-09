/***********************************************************************//**
 *	MoveCursorExtension.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MoveCursorExtension.h"

// Friends
#include "../body/GameExecCtx.h"
#include "../body/Party.h"
#include "../unit/Unit.h"

// External

// Library
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Vector.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define RANGE_TOTRG				(30.0f)						// クリック位置とターゲットの許容範囲

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MoveCursorExtension メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	有効かどうかを設定する
**//*---------------------------------------------------------------------*/
void MoveCursorExtension::registerHandlingableUnit(const Unit* unit)
{
	_listUnit->addTail(unit);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MoveCursorExtension::MoveCursorExtension()
	: _listUnit(0L)
	, _unitHandled(0L)
	, _unitDefault(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MoveCursorExtension::~MoveCursorExtension()
{
	ASSERT(_listUnit == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MoveCursorExtension::create()
{
	// リストを作成する
	_listUnit = new List<const Unit*>;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MoveCursorExtension::destroy()
{
	// リストを削除する
	delete _listUnit;
	_listUnit = 0L;
}

/*---------------------------------------------------------------------*//**
	タップ開始
**//*---------------------------------------------------------------------*/
void MoveCursorExtension::onTapStart(const PointF32* ptTouch)
{
#if 0	// 仕様廃止	【2012/03/25 r-kishi】
	Tfw* tfw = Game::getGame()->getTfw();			ASSERT(tfw != 0L);
	View* view = tfw->getView();					ASSERT(view != 0L);
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();
	if(sgm == 0L)	{	return;		}
	Camera* cam = sgm->getVisibleCamera();
	if(cam == 0L)	{	return;		}
	for(ListIterator<const Unit*> it = _listUnit->iterator(); it.has(); it.next())
	{
		const Unit* unit = it.object();		ASSERT(unit != 0L);
		if(!unit->isEnable())		{	continue;	}	// 無効なユニットは除外
		if(unit == _unitDefault)	{	continue;	}	// 標準操作ユニットは除外

		// スクリーン座標に変換
		Vector2F vScr;
		Gcalc::conv3dPosToScreenPos(&vScr, unit->getCenterPos(), cam, view);

		// タップされていたら操作ユニットに設定
		if(TFW_IS_NEAR(vScr.x(), ptTouch->x(), RANGE_TOTRG) && TFW_IS_NEAR(vScr.y(), ptTouch->y(), RANGE_TOTRG))
		{
			_unitHandled = unit;
			break;
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	タップ終了
**//*---------------------------------------------------------------------*/
void MoveCursorExtension::onTapEnd()
{
	// 操作ユニットを除外
	_unitHandled = 0L;
}

/*---------------------------------------------------------------------*//**
	リーダー変更通知
**//*---------------------------------------------------------------------*/
void MoveCursorExtension::notifyChangeLeader()
{
	// 標準操作ユニットを設定する
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	_unitDefault = (const Unit*)party->getLeaderPcUnit();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

