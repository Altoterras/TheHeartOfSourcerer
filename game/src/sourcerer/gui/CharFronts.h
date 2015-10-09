/***********************************************************************//**
 *	CharFronts.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_CHAR_FRONTS_H_
#define _SRCR_GUI_CHAR_FRONTS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Point;
typedef Point<f32> PointF32;
template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Texture;
class Panel;

TFW_END_NS

SRCR_BEGIN_NS

class ActStat;
class CharStat;
class CharUnit;
class FocusCursor;
class FocusMenu;
class Game;
class Item;
class Spirit;
class StatGauge;
class Unit;

/*---------------------------------------------------------------------*//**
 *	キャラクタ GUI
 *
**//*---------------------------------------------------------------------*/
class CharFronts : public FrameExecDraw
{
	//======================================================================
	// 定数
private:
	enum PartyChar
	{
		PC_1,
		PC_2,
		PC_3,
		NUM_PC
	};
	enum CharGauge
	{
		CGG_PC1,
		CGG_PC2,
		CGG_PC3,
		CGG_TRG,
		NUM_CGG
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	キャラクター単位の状態
	 *
	**//*-----------------------------------------------------------------*/
	class CharInfo
	{
	public:
		Panel* _pnlMain;
		Panel* _pnlSub;
		f32 _hMain;
		f32 _hSub;
		CharUnit* _cunit;
		const Spirit* _sprt;
		const Item* _item;
		Unit* _wunit;
		CharInfo() : _pnlMain(0L), _pnlSub(0L), _hMain(0.0f), _hSub(0.0f), _cunit(0L), _sprt(0L), _item(0L), _wunit(0L) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 外部有効かどうかを得る
	inline bool isEnable() const { return _isEnableOut; }
	// 外部有効かどうかをs設定する
	void setEnable(bool isEnable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool getPcStatRect(RectF32* rectOut, s32 ipc) const;
#if 0	// GaugeAnimEffect によるダメージゲージアニメーションは StatGauge に任せるように変更した	【2015/02/24 r-kishi】
	bool getPcGaugePos(PointF32* ptOut, s32 ipc) const;
	bool getTargetGaugePos(PointF32* ptOut, const Unit* unitTrg) const;
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharFronts();
	virtual ~CharFronts();
	bool create(Texture* texGuiRef, FocusCursor* fccsrRef, FocusMenu* fcmenuRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画
	void notifyChangeMapBefore();

private:
	void acquireCharPtr();
	void showFocusMenu(s32 imchar, bool isChar);
	CharUnit* getTargetUnit() const;
	void drawBehaviorActing(const RenderCtx* rc, const Spirit* sprt, f32 x, f32 y);

	//======================================================================
	// 変数
private:
	bool			_isEnableOut;
	bool			_isEnableSelf;
	CharInfo		_ci[NUM_PC];
	const CharStat*	_cstatTrgRef;
	f32				_dfcntTrgClearDelay;
	StatGauge*		_sg[NUM_CGG];
	RectF32*		_uvwhLeaderMark;
	RectF32*		_uvwhTargetIcon;
	RectF32*		_uvwhWhiteDot;
	FocusCursor*	_fccsrRef;
	FocusMenu*		_fcmenuRef;
	Texture*		_texGuiRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_CHAR_FRONTS_H_
