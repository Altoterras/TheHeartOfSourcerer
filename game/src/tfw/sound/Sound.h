/***********************************************************************//**
 *	Sound.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/10/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_SOUND_H_
#define _TFW_SOUND_SOUND_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;
class OpenAl;
class OpenSl;
class Music;
class Se;

/*---------------------------------------------------------------------*//**
 *	サウンド
 *	
**//*---------------------------------------------------------------------*/
class Sound
{
	//======================================================================
	// 定数
public:
	static const s32 OAL_BUFIDX_MUSIC_START	= 0;
	static const s32 OAL_BUFIDX_MUSIC_END	= 2;
	static const s32 OAL_BUFIDX_SE_START	= 3;
	static const s32 OAL_TRACK_MUSIC_START	= 0;
	static const s32 OAL_TRACK_MUSIC_END	= 0;
	static const s32 OAL_TRACK_SE_START		= 1;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// OpenSL を得る
	inline OpenSl* getOpenSl() { return _osl; }
	// OpenAL を得る
	inline OpenAl* getOpenAl() { return _oal; }
	// 曲再生を得る
	inline Music* getMusic() { return _music; }
	// SE 再生を得る
	inline Se* getSe() { return _se; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Sound();
	~Sound();
	bool create(s32 numSePolyphony, s32 numMaxSeDefine, s32 numWaveBuffer, bool isSeLoadOnPlay);
	void destroy();
	void exec(const ExecCtx* ec);

private:

	//======================================================================
	// 変数
private:
	OpenSl* _osl;
	OpenAl* _oal;
	Music* _music;
	Se* _se;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SOUND_SOUND_H_