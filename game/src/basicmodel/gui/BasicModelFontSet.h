/***********************************************************************//**
 *	BasicModelFontSet.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_GUI_BASICMODEL_FONT_SET_H_
#define _BASICMODEL_GUI_BASICMODEL_FONT_SET_H_

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

BASICMODEL_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォント セット
 *
**//*---------------------------------------------------------------------*/
class BasicModelFontSet : public FontSet
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
	BasicModelFontSet();
	virtual ~BasicModelFontSet();
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

BASICMODEL_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_GUI_BASICMODEL_FONT_SET_H_
