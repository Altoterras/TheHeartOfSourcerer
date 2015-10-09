/***********************************************************************//**
 *	EdevFontSet.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _EDEV_GUI_EDEV_FONT_SET_H_
#define _EDEV_GUI_EDEV_FONT_SET_H_

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

EDEV_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォント セット
 *
**//*---------------------------------------------------------------------*/
class EdevFontSet : public FontSet
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
	EdevFontSet();
	virtual ~EdevFontSet();
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

EDEV_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _EDEV_GUI_EDEV_FONT_SET_H_
