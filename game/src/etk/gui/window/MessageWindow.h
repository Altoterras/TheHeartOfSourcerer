/***********************************************************************//**
 *	MessageWindow.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_WINDOW_MESSAGE_WINDOW_H_
#define _ETK_GUI_WINDOW_MESSAGE_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;
class Texture;
class StringDrawer;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージウインドウ
 *
**//*---------------------------------------------------------------------*/
class MessageWindow : public Window
{
	//======================================================================
	// 定数
public:
	enum Kind
	{
		KIND_NULL,
		KIND_TALK,	// ユーザーの応答を受け付けるモード
		KIND_SAY,	// イベント制御などでユーザーの応答を受け付けないモード
		KIND_YESNO,	// YES or NO の応答を受け付けるモード
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	結果値
	 *
	**//*-----------------------------------------------------------------*/
	class Result
	{
		//==================================================================
		// 定数
	private:
		static const u32 F_YES	= 0x00000001;
		static const u32 F_NO	= 0x00000002;

		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// YES ボタンをクリックしたかを得る
		inline bool isClickYesButton() const { return TFW_IS_FLAG(_flag, F_YES); }
		// NO ボタンをクリックしたかを得る
		inline bool isClickNoButton() const { return TFW_IS_FLAG(_flag, F_NO); }

		// YES ボタンをクリックしたかを設定する
		inline void setClickYesButton() { TFW_SET_FLAG(_flag, F_YES, true); }
		// NO ボタンをクリックしたかを設定する
		inline void setClickNoButton() { TFW_SET_FLAG(_flag, F_NO, true); }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		Result() : _flag(0) {}
		void reset() { _flag = 0; }

		//==================================================================
		// 変数
	private:
		u32 _flag;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 次へカーソルを有効化する
	inline void setEnableNextCursor(bool isEnable) { _isEnableNextCursor = isEnable; }
	// 強制的に次へカーソルを表示するかどうかを設定する
	inline void setForceNextCursor(bool isEnable) { _isForceNextCursor = isEnable; }
	// オートスクロールするかどうかを設定する
	void setAutoScroll(bool isEnable);
	// 動作を停止しているかどうかを得る
	inline bool isDone() { return _isDone; }
	// 読み上げアニメーション中かを得る
	bool isReadingAnimation() const;
	// SAY ウェイトモードかを得る
	inline bool isWaitSayMode() const { return _isWaitSay; }
	// 結果値を得る
	inline virtual const Result* getResult() const { return 0L; }
	// 汎用パラメータを得る
	inline u32 getGeneralParameter() const { return _gparam; }

	// SAY ウェイトモードかを設定する
	inline void setWaitSayMode(bool isWaitSay) { _isWaitSay = isWaitSay; }
	// 汎用パラメータを設定する
	inline void setGeneralParameter(u32 gparam) { _gparam = gparam; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setRectangle(const RectF32* rect);
	virtual void setWindowText(const VcString* text);
	virtual void addWindowText(const VcString* text);
	virtual void showMessage(const VcString* msg, s32 hintCtrl);
	void nextPage(s32 hintCtrl);
	void setDone(s32 hintCtrl)				{	if(hintCtrl == _hintCtrl) { _isDone = true; }	}
	void setKind(Kind kind)					{	_kind = kind;									}
	void setReadingAnimation(bool isAnim);
	void setVerticalScrollAnimation(bool isAnim);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MessageWindow();
	virtual ~MessageWindow();
	virtual bool create(Font* fontRef, Texture* texRef, const RectF32* rectWnd, const ColorU8* colorTxt, s16 xPaddingLeft, s16 xPaddingRight, s16 yPaddingTop, s16 yPaddingBottom, bool isAutoScroll);
	virtual void destroy();

protected:
	virtual void execContents(ExecRes* res, const ExecCtx* ec);
	virtual void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	virtual void onShowWindow(bool isShow);

	//======================================================================
	// 変数
protected:
	bool _isEnableNextCursor;
	bool _isForceNextCursor;
	bool _isDone;
	bool _isWaitSay;
	Kind _kind;
	s32 _hintCtrl;
	u32 _gparam;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_WINDOW_IMPLEMENT_MESSAGE_WINDOW_H_
