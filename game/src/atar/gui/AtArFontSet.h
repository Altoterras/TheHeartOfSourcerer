/***********************************************************************//**
 *	AtArFontSet.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_GUI_AT_AR_FONT_SET_H_
#define _ATAR_GUI_AT_AR_FONT_SET_H_

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

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォント セット
 *
**//*---------------------------------------------------------------------*/
class AtArFontSet : public FontSet
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
	// プロパティ
public:
	Font* getFont(s32 fonttype) const;
	Texture* getTexture(s32 fonttype) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AtArFontSet();
	virtual ~AtArFontSet();
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

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_GUI_AT_AR_FONT_SET_H_
