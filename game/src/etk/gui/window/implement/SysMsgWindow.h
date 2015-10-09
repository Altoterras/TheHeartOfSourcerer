/***********************************************************************//**
 *	SysMsgWindow.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_WINDOW_IMPLEMENT_SYS_MSG_WINDOW_H_
#define _ETK_GUI_WINDOW_IMPLEMENT_SYS_MSG_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MessageWindow.h"

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
class SysMsgWindow : public MessageWindow
{
	//======================================================================
	// 定数
public:
	enum Button
	{
		BTN_YES,
		BTN_NO,
		NUM_BTN
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 結果値を得る
	inline const Result* getResult() const { return &_res; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setRectangle(const RectF32* rectWnd, const RectF32* rectScreen);
	void showMessage(const VcString* msg, s32 hintCtrl);	// 外部制御用
	void showStdMsg(const VcString* msg);					// 自立制御用
	void setModal(bool on) { _confModal = on; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SysMsgWindow();
	~SysMsgWindow();
	bool create(const RectF32* rectWnd, const RectF32* rectScreen, Font* fontRef, Texture* texRef, const VcString* strYesButton, const VcString* strNoButton);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	virtual void onShowWindow(bool isShow);

protected:
	void changeLanguage(const VcString* strYesButton, const VcString* strNoButton);

	//======================================================================
	// メンバ変数
private:
	RectF32* _rectScreen;
	Panel* _pnlBtn[NUM_BTN];
	Result _res;
	bool _confSelfClose;	// 終了時に自ら閉じる
	bool _confDarkCurtain;	// 暗転する
	bool _confModal;		// UI 応答を独占する
	bool _isDoneReserved;	// 終了予約
	f32 _fcntFade;			// フェードフレームカウンタ
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_WINDOW_IMPLEMENT_SYS_MSG_WINDOW_H_
