/***********************************************************************//**
 *	OpenSlMusic.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/01/06.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_OPEN_SL_MUSIC_H_
#define _TFW_SOUND_OPEN_SL_MUSIC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class File;
class OpenSl;
class WaveDef;

/*---------------------------------------------------------------------*//**
 *	OpenSL 曲再生モジュール
 *	
**//*---------------------------------------------------------------------*/
class OpenSlMusic  
{
	//======================================================================
	// 定数
private:
	static const int	TRCKIDX_MUSIC	= 0;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool play(WaveDef* wdef, s32 cntLoop);
	bool stop();
	bool pause();
	bool resume();
	bool isPlaying();
	bool isPausing();
	bool setVolume(f32 volume);
	bool setCurrentLoop(bool isLoop);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OpenSlMusic();
	virtual ~OpenSlMusic();
	bool create(OpenSl* slRef);
	void destroy();
	void exec();

	//======================================================================
	// 変数
private:
	OpenSl* _slRef;
	File* _file;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_OPEN_SL_H_
