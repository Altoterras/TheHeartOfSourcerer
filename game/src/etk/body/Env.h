/***********************************************************************//**
 *	Env.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_ENV_H_
#define _ETK_BODY_ENV_H_

////////////////////////////////////////////////////////////////////////////
// 定数

namespace etk
{

// 言語 ID
enum LangId
{
	LANGID_EN,
	LANGID_JP,
	NUM_LANGID
};

// プラットフォーム ID
enum PlatformId
{
	PLATFORMID_WIN,
	PLATFORMID_IOS,
	PLATFORMID_ANDROID,
	NUM_PLATFORMID
};
	
// OS コンポーネント ID
enum OsComponentId
{
	OSCMPID_SOURCE_TEXT_FIELD = 1,
};

}	// namespace enlt

////////////////////////////////////////////////////////////////////////////
// グローバル関数

namespace etk
{

// 言語 ID を得る
LangId Env_getLangId();

// プラットフォーム ID を得る
inline PlatformId Env_getPlatformId()
{
	// Windows である
	#if defined(_WINDOWS)
		return PLATFORMID_WIN;
	#endif

	// iOS である
	#if defined(_IOS)
		return PLATFORMID_IOS;
	#endif

	// Android である
	#if defined(_ANDROID)
		return PLATFORMID_ANDROID;
	#endif
}

// 高解像度かを得る
bool Env_isHdResolution();
// Retina 解像度かを得る
bool Env_isRdResolution();
#if 0
// ワイド解像度かを得る
bool Env_isWdResolution();
#endif
// 2D 解像描画比を得る
float Env_get2drRate();

}	// namespace et

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_BODY_ENV_H_
