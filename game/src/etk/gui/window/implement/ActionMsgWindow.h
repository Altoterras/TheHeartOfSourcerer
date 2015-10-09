/***********************************************************************//**
 *	ActionMsgWindow.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/05/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_WINDOW_IMPLEMENT_CONFRONT_MSG_WINDOW_H_
#define _ETK_GUI_WINDOW_IMPLEMENT_CONFRONT_MSG_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MessageWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class List;
class Font;
class Texture;
class StringDrawer;

TFW_END_NS

ETK_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	対決時用メッセージ ウインドウ
 *
**//*---------------------------------------------------------------------*/
class ActionMsgWindow : public MessageWindow
{
	//======================================================================
	// 定数
private:

	//======================================================================
	// クラス
private:
	class Msg
	{
	public:
		VcString* _str;
		ColorU8* _col;
		StringDrawer* _strdraw;
		f32 _frmcnt;
		f32 _yLogical;
		f32 _hStr;

		Msg(const VcString* str, const ColorU8* col, Font* fontRef, const RectF32* rectString);
		~Msg();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 常に表示設定（対決時など）
	inline void setResident(bool is) {	_isResident = is; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setRectangle(const RectF32* rectWnd, const RectF32* rectScreen);
	void showMessageCol(const ColorU8* color, const VcString* text);
	void clearMessage();

private:
	void addWindowText(const VcString* text);
	void addWindowText(const ColorU8* color, const VcString* text);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ActionMsgWindow();
	~ActionMsgWindow();
	bool create(const RectF32* rectWnd, const RectF32* rectScreen, Font* fontRef, Texture* texRef);
	void destroy();
	virtual void execContents(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	virtual void onShowWindow(bool isShow);

	//======================================================================
	// メンバ変数
private:
	bool _isSelfHide;
	bool _isResident;
	bool _isBgCurtain;

	RectF32* _rectScreen;
	Font* _fontRef;
	List<Msg*>* _listMsg;
	f32 _yBdrlineCur;
	f32 _yBdrlineMin;
	f32 _yBdrlineDisp;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_WINDOW_IMPLEMENT_CONFRONT_MSG_WINDOW_H_
