/***********************************************************************//**
 *	FocusCursor.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_FOCUS_CURSOR_H_
#define _SRCR_GUI_FOCUS_CURSOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
class Texture;
class Panel;
class Renderer;

TFW_END_NS

SRCR_BEGIN_NS

class FocusArray;
class Game;
class Unit;
class FocusMenu;
class Spirit;

/*---------------------------------------------------------------------*//**
 *	移動カーソル
 *
**//*---------------------------------------------------------------------*/
class FocusCursor : public FrameExecDraw
{
	//======================================================================
	// クラス
private:
	class Focus
	{
	public:
		Unit*		_unitRef;
		Vector2F*	_posScr;	

		Focus();
		~Focus();
	};

	//======================================================================
	// 定数
public:
	// カーソルの状態
	enum State
	{
		STATE_STOP,
		STATE_WALK,
		STATE_RUN
	};

	// 最大フォーカス数
	static const s32	NUM_FOCUS_MAX	= 1;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかを得る
	inline bool		isEnable() const			{ return _isEnableOut; }
	// フォーカスを得る
	Unit*			getFocus(s32 index) const;

	// 有効かどうかを設定する
	inline void		setEnable(bool isEnable)	{ _isEnableOut = isEnable; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setFocus(s32 index, Unit* unitRef);
	void setOwnerSpirit(Spirit* sprt);
	void setPreventTapCounter(f32 framecnt);
	void resetAllFocus();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FocusCursor();
	virtual ~FocusCursor();
	bool create(Texture* texRef, FocusMenu* fcmenuRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画
	void notifyEnterGame();
	void notifyChangeMapBefore();
	void notifyDeserialize();

private:
	void setFocus(s32 index, Unit* unitRef, const Vector2F* vScrPos);
	void drawFocusCursor(Renderer* rdr, const FocusArray* fa, f32 scale, f32 rate2dr);
	void drawQuestIcon(Renderer* rdr, const Unit* unit, f32 rate2dr);
	void showFocusMenu(Unit* unit, const Vector2F* vScrPos);

	//======================================================================
	// メンバ変数

private:
	bool		_isEnableOut;
	bool		_isEnableSelf;
	f32			_fcntPreventTap;		// タップ防止カウンタ
	f32			_dangFocus;				// フォーカスカーソルの描画角度（ディグリー）
	f32			_dangFocusable;			// ターゲットカーソルの描画角度（ディグリー）
	FocusArray*	_focusarr;				// フォーカス配列
	Texture*	_texRef;
	FocusMenu*	_fcmenuRef;
	Spirit*		_sprtOwnrRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_FOCUS_CURSOR_H_
