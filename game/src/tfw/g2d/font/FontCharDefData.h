/***********************************************************************//**
 *	FontCharDefData.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/03/27.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G2D_FONT_FONT_CHAR_DEF_DATA_H_
#define _TFW_G2D_FONT_FONT_CHAR_DEF_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

/*---------------------------------------------------------------------*//**
 *	フォント文字定義データ
 *
**//*---------------------------------------------------------------------*/
class FontCharDefData
{
	//======================================================================
	// クラス
public:
	class ProportionalCharInfo
	{
	public:
		u8 _xStart;
		u8 _xEnd;
		
	public:
		ProportionalCharInfo() : _xStart(0), _xEnd(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	u32 getCharIndex(const u16 c) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FontCharDefData();
	~FontCharDefData();
	bool load(FileBase* file);

	//======================================================================
	// 変数
public:
	u32 _countCharDef;
	u16* _sarrCharDef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G2D_FONT_FONT_CHAR_DEF_DATA_H_

