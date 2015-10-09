/***********************************************************************//**
 *	GameFontSet.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_GAME_FONT_SET_H_
#define _SRCR_GUI_GAME_FONT_SET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/gui/FontSet.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;
class FontCharDefData;
class ProportionalFontData;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォント セット
 *
**//*---------------------------------------------------------------------*/
class GameFontSet : public FontSet
{
	//======================================================================
	// 定数
public:
	enum FontType
	{
		EN = 0,
		JP,
		JP_MOVIE,
		NUM_
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	Font* getFont(s32 fonttype) const;
	Font* getMainGuiFont() const { return getFont(JP); }
	Texture* getTexture(s32 fonttype) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameFontSet();
	virtual ~GameFontSet();
	bool create();
	void destroy();
	void debug_reload();

private:
	bool load();

	//======================================================================
	// 変数
private:
	Font** _fontarr;
	Texture** _texarrBmpFont;
	ProportionalFontData** _pfdatarrFont;
	FontCharDefData* _fcddJpFont;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_GAME_FONT_SET_H_
