/***********************************************************************//**
 *	AtArFontSet.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AtArFontSet.h"

// Friends
#include "../file/AtArFileIdDef.h"

// External
#include "../../tfw/g2d/font/FontCharDefData.h"
#include "../../tfw/g2d/font/ProportionalFontData.h"
#include "../../tfw/g2d/font/implement/BitmapFontAscii.h"
#include "../../tfw/g2d/font/implement/BitmapFontMultiByte.h"
#include "../../tfw/g2d/font/implement/OsFont.h"
#include "../../tfw/gcmn/Texture.h"

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AtArFontSet プロパティ

/*---------------------------------------------------------------------*//**
	指定フォントを得る
**//*---------------------------------------------------------------------*/
Font* AtArFontSet::getFont(s32 fonttype) const
{
	if((fonttype < 0) || (fonttype >= NUM_))	{	return 0L;	}
	return _fontarr[fonttype];
}

/*---------------------------------------------------------------------*//**
	指定フォントに使われているテクスチャを得る
**//*---------------------------------------------------------------------*/
Texture* AtArFontSet::getTexture(s32 fonttype) const
{
	if((fonttype < 0) || (fonttype >= NUM_))	{	return 0L;	}
	return _texarrBmpFont[fonttype];
}

//==========================================================================
// AtArFontSet メソッド
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AtArFontSet::AtArFontSet()
	: _fontarr(0L)
	, _texarrBmpFont(0L)
	, _pfdatarrFont(0L)
	, _fcddJpFont(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AtArFontSet::~AtArFontSet()
{
	ASSERT(_fontarr == 0L);
	ASSERT(_texarrBmpFont == 0L);
	ASSERT(_pfdatarrFont == 0L);
	ASSERT(_fcddJpFont == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool AtArFontSet::create()
{
	// フォント配列作成
	_fontarr = new Font*[NUM_];
	if(_fontarr == 0L)	{	return false;	}
	::memset(_fontarr, 0, sizeof(Font*) * NUM_);

	// テクスチャ配列作成
	_texarrBmpFont = new Texture*[NUM_];
	if(_texarrBmpFont == 0L)	{	return false;	}
	::memset(_texarrBmpFont, 0, sizeof(Texture*) * NUM_);

	// プロポーショナルフォントデータ配列作成
	_pfdatarrFont = new ProportionalFontData*[NUM_];
	if(_pfdatarrFont == 0L)	{	return false;	}
	::memset(_pfdatarrFont, 0, sizeof(ProportionalFontData*) * NUM_);

	// 日本語フォントの文字定義データ作成
	_fcddJpFont = new FontCharDefData;

	// インスタンス作成
	_texarrBmpFont[EN] = new Texture(); 
	_texarrBmpFont[JP] = new Texture();
#if 0
	_texarrBmpFont[JP_MOVIE] = new Texture();
#endif
	_pfdatarrFont[JP] = new ProportionalFontData();
#if 0
	_pfdatarrFont[JP_MOVIE] = new ProportionalFontData();
#endif
	_fontarr[EN] = new BitmapFontAscii();
	_fontarr[JP] = new BitmapFontMultiByte();
#if 0
	_fontarr[JP_MOVIE] = new BitmapFontMultiByte();
#endif

	// 読み込みとフォント作成処理
	if(!load())
	{
		destroy();
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void AtArFontSet::destroy()
{
	// 日本語フォントの文字定義データ削除
	delete _fcddJpFont;
	_fcddJpFont = 0L;

	// プロポーショナルフォントデータ配列の削除
	if(_pfdatarrFont != 0L)
	{
		for(int i = 0; i < NUM_; i++)
		{
			if(_pfdatarrFont[i] != 0L)
			{
				delete _pfdatarrFont[i];
			}
		}
		delete[] _pfdatarrFont;
		_pfdatarrFont = 0L;
	}

	// テクスチャ配列の削除
	if(_texarrBmpFont != 0L)
	{
		for(int i = 0; i < NUM_; i++)
		{
			if(_texarrBmpFont[i] != 0L)
			{
				_texarrBmpFont[i]->destroy();
				delete _texarrBmpFont[i];
			}
		}
		delete[] _texarrBmpFont;
		_texarrBmpFont = 0L;
	}

	// フォント配列の削除
	if(_fontarr != 0L)
	{
		for(int i = 0; i < NUM_; i++)
		{
			if(_fontarr[i] != 0L)
			{
				_fontarr[i]->destroy();
				delete _fontarr[i];
			}
		}
		delete[] _fontarr;
		_fontarr = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フォント再読込（デバッグ用）
**//*---------------------------------------------------------------------*/
void AtArFontSet::debug_reload()
{
	// テクスチャ配列の削除
	if(_texarrBmpFont != 0L)
	{
		for(int i = 0; i < NUM_; i++)
		{
			if(_texarrBmpFont[i] != 0L)
			{
				_texarrBmpFont[i]->destroy();
			}
		}
	}

	// フォント配列の一時削除
	if(_fontarr != 0L)
	{
		for(int i = 0; i < NUM_; i++)
		{
			_fontarr[i]->destroy();
		}
	}

	load();
}

/*---------------------------------------------------------------------*//**
	フォント読込
**//*---------------------------------------------------------------------*/
bool AtArFontSet::load()
{
	const int fileidFontEnTex = FILEID_CONV_RD(SYSTEM_FONT_EN01_SD_PNG);
	const int fileidFontJpCharDefData = SYSTEM_FONTJPCHARDEF_DAT;
	const int fileidFontJpGothicTex = FILEID_CONV_RD(SYSTEM_FONT_JP01G_SD_PNG);
	const int fileidFontJpGothicPtbl = FILEID_CONV_RD(SYSTEM_FONT_JP01G_SD_PTBL);
#if 0
	const int fileidFontJpMovieTex = FILEID_CONV_RD(SYSTEM_FONT_JP01M_SD_PNG);
	const int fileidFontJpMoviePtbl = FILEID_CONV_RD(SYSTEM_FONT_JP01M_SD_PTBL);
#endif
	f32 scale = Env_isRdResolution() ? 0.25f : Env_isHdResolution() ? 0.5f : 1.0f;
	
	// 英語ビットマップフォントテクスチャ読み込み
	if(!_texarrBmpFont[EN]->create(fileidFontEnTex, Texture::FMT_PNG))
	{
		ASSERT(false);
		return false;
	}
	// 英語ビットマップフォント作成
	if(!((BitmapFontAscii*)_fontarr[EN])->create(_texarrBmpFont[EN], 16, 0.75f, scale))
	{
		ASSERT(false);
		return false;
	}

	// 日本語フォントの文字定義データ読み込み
	{
		IdFile file(fileidFontJpCharDefData);
		if(file.isOpened())
		{
			if(!_fcddJpFont->load(&file))
			{
				ASSERT(false);
				return false;
			}
		}
	}

	// 日本語ビットマップフォント（ゴシック）テクスチャ読み込み
	if(!_texarrBmpFont[JP]->create(fileidFontJpGothicTex, Texture::FMT_PNG))
	{
		ASSERT(false);
		return false;
	}
	// 日本語ビットマップフォント（ゴシック）プロポーショナルフォントデータ読み込み
	if(_pfdatarrFont[JP] != 0L)
	{
		IdFile file(fileidFontJpGothicPtbl);
		if(file.isOpened())
		{
			if(file.read(_pfdatarrFont[JP], sizeof(ProportionalFontData)) != sizeof(ProportionalFontData))
			{
				delete _pfdatarrFont[JP];
				_pfdatarrFont[JP] = 0L;
			}
		}
	}
	// 日本語ビットマップフォント（ゴシック）作成
	if(!((BitmapFontMultiByte*)_fontarr[JP])->create(_texarrBmpFont[JP], _pfdatarrFont[JP], _fcddJpFont, 16, 0.5f, scale))
	{
		ASSERT(false);
		return false;
	}

#if 0
	// 日本語ビットマップフォント（字幕）テクスチャ読み込み
	if(!_texarrBmpFont[JP_MOVIE]->create(fileidFontJpMovieTex, Texture::FMT_PNG))
	{
		ASSERT(false);
		return false;
	}
	// 日本語ビットマップフォント（字幕）プロポーショナルフォントデータ読み込み
	if(_pfdatarrFont[JP_MOVIE] != 0L)
	{
		IdFile file(fileidFontJpMoviePtbl);
		if(file.isOpened())
		{
			if(file.read(_pfdatarrFont[JP_MOVIE], sizeof(ProportionalFontData)) != sizeof(ProportionalFontData))
			{
				delete _pfdatarrFont[JP_MOVIE];
				_pfdatarrFont[JP_MOVIE] = 0L;
			}
		}
	}
	// 日本語ビットマップフォント（字幕）作成
	if(!((BitmapFontMultiByte*)_fontarr[JP_MOVIE])->create(_texarrBmpFont[JP_MOVIE], _pfdatarrFont[JP_MOVIE], _fcddJpFont, 16, 0.5f, scale))
	{
		ASSERT(false);
		return false;
	}
#endif

	return true;
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS
