/***********************************************************************//**
 *	GameActMsgWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_GAME_ACT_MSG_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_GAME_ACT_MSG_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/implement/ActionMsgWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

#if 1
SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	対決時用メッセージ ウインドウ
 *
**//*---------------------------------------------------------------------*/
class GameActMsgWindow : public ActionMsgWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void showMessage(const Unit* unitActorRef, const VcString* text);

	static bool getUnitColor(ColorU8* color, const Unit* unit);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(Font* fontRef, Texture* texRef);
#if 0
	void execContents(ExecRes* res, const ExecCtx* ec);
#endif
};

SRCR_END_NS

#else

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class List;
class Font;
class Texture;
class StringDrawer;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	対決時用メッセージ ウインドウ
 *
**//*---------------------------------------------------------------------*/
class GameActMsgWindow : public GameActMsgWindow
{
	//======================================================================
	// 定数
private:
	#define FCNT_LIFE			(30 * 5)	// 表示継続フレーム
	#define FCNT_DISAPR			(10)		// 消滅フレーム
	#define Y_INVALID			(-10000.0f)	// 無効な Y 位置
	#define H_BDRLINE_PADDING	(8.0f)

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
	void showMessage(const Unit* unitActorRef, const VcString* text);
	void showMessageCol(const ColorU8* color, const VcString* text);
	void clearMessage();

	static bool getUnitColor(ColorU8* color, const Unit* unit);

private:
	void addWindowText(const VcString* text);
	void addWindowText(const ColorU8* color, const VcString* text);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameActMsgWindow();
	~GameActMsgWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	void onShowWindow(bool isShow);

	//======================================================================
	// メンバ変数
private:
	bool _isSelfHide;
	bool _isResident;
	bool _isBgCurtain;

	Font* _fontRef;
	List<Msg*>* _listMsg;
	f32 _yBdrlineCur;
	f32 _yBdrlineMin;
	f32 _yBdrlineDisp;
};

SRCR_END_NS
#endif

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_GAME_ACT_MSG_WINDOW_H_
