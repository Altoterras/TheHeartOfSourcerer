/***********************************************************************//**
 *	IngameFront.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_INGAME_FRONT_H_
#define _SRCR_GUI_INGAME_FRONT_H_

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
typedef Rect<f32> RectF32;
class Texture;
class Panel;

TFW_END_NS

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	インゲーム（通常操作画面）のフロントエンド GUI
 *
**//*---------------------------------------------------------------------*/
class IngameFront : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// パネル種別
	enum ButtonKind
	{
		BTN_MENU,
		BTN_SOUMA,
		BTN_MEDITATION,
		BTN_GUARDCHARGE,
		BTN_ATTRACT,
		BTN_CURE,
		NUM_BTN
	};

	// パネル状態
	static const u8 BSTAT_OFF = 0;
	static const u8 BSTAT_ON = 1;
	static const u8 BSTAT_BREAKABLE = 2;

	//======================================================================
	// クラス
private:
	class Button
	{
	public:
		Panel* _pnl;
		u8 _bstat;
		bool _isEnable;
	public:
		Button();
		~Button();
		bool create(Texture* texGuiRef, const RectF32* rectPanel, const RectF32* uvPanel);
		void destroy();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 外部有効かどうかを得る
	inline bool isEnable() const			{ return _isEnableOut; }

	// 外部有効かどうかを設定する
	inline void setEnable(bool isEnable)	{ _isEnableOut = isEnable; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setEneblePanel(ButtonKind btnk, bool isEnable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	IngameFront();
	virtual ~IngameFront();
	bool create(Texture* texGuiRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画
	void notifyResizeView();
	void notifyChangeMode(s32 gmode);
	void notifyChangeLeader();

	//======================================================================
	// 変数
private:
	bool		_isEnableOut;
	bool		_isEnableSelf;
	Button		_btn[NUM_BTN];
	Panel*		_pnlBreak;
	s32			_ipcLeaderCur;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_INGAME_FRONT_H_
