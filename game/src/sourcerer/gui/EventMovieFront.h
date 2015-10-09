/***********************************************************************//**
 *	EventMovieFront.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_EVENT_MOVIE_FRONT_H_
#define _SRCR_GUI_EVENT_MOVIE_FRONT_H_

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
 *	イベントムービー中のフロントエンド GUI
 *
**//*---------------------------------------------------------------------*/
class EventMovieFront : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// パネル種別
	enum ButtonKind
	{
		BTN_SKIP,
		NUM_BTN
	};

private:
	// 操作種別
	enum EventOp
	{
		EOP_PAUSE,		// ポーズ
		EOP_PLAY,		// ポーズ解除
		EOP_SKIP,		// スキップ
		EOP_ENDWAIT,	// 台詞待ち終了
		EOP_CHECKSKIP,	// スキップ可能かを調べる
		NUM_EOP,
	};

	//======================================================================
	// クラス
private:
	class Button
	{
	public:
		Panel* _pnl;
		bool _isEnable;
	public:
		Button();
		~Button();
		bool create(Texture* texGuiRef, const RectF32* rectPanel, const RectF32* uvPanel);
		void destroy();
		void changeEnable(bool isEnable);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 外部有効かどうかを得る
	inline bool getEnable() const			{	return _isEnableOut;		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setEnable(bool isEnable);
	void setEneblePanel(ButtonKind btnk, bool isEnable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EventMovieFront();
	virtual ~EventMovieFront();
	bool create(Texture* texGuiRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画

private:
	bool operationToEvent(EventOp uop);

	//======================================================================
	// 変数
private:
	bool _isEnableOut;
	bool _isEnableSelf;
	bool _isAskYesNo;
	Button _btn[NUM_BTN];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_EVENT_MOVIE_FRONT_H_
