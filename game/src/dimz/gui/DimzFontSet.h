/***********************************************************************//**
 *	DimzFontSet.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _DIMZ_GUI_DIMZ_FONT_SET_H_
#define _DIMZ_GUI_DIMZ_FONT_SET_H_

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

DIMZ_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォント セット
 *
**//*---------------------------------------------------------------------*/
class DimzFontSet : public FontSet
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
	DimzFontSet();
	virtual ~DimzFontSet();
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

DIMZ_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _DIMZ_GUI_DIMZ_FONT_SET_H_
