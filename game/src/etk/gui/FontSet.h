/***********************************************************************//**
 *	FontSet.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/09/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_FONT_SET_H_
#define _ETK_GUI_FONT_SET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;
class FontCharDefData;
class ProportionalFontData;
class Texture;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォント セット
 *
**//*---------------------------------------------------------------------*/
class FontSet
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	virtual Font* getFont(s32 fonttype) const = 0;
	virtual Font* getMainGuiFont() const { return getFont(0); }
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_FONT_SET_H_
