/***********************************************************************//**
 *	IosAudio.h
 *	Terras Frameork
 *	
 *	Created by Ryoutarou Kishi on 2009/10/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_IOS_AUDIO_H_
#define _TFW_SOUND_IOS_AUDIO_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#import <OpenAL/al.h>
#import <OpenAL/alc.h>

////////////////////////////////////////////////////////////////////////////
// 型定義

#if !defined(TYPE_IOS_AUDIO_PLAYER)
	#define TYPE_IOS_AUDIO_PLAYER void
#endif
#if !defined(TYPE_IOS_AUDIO_DELEGATER)
	#define TYPE_IOS_AUDIO_DELEGATER void
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	iOS オーディオ
 *	
**//*---------------------------------------------------------------------*/
class IosAudio
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool playMusic(const char* name, const char* type, s32 cntLoop);
	bool stopMusic();
	bool pauseMusic();
	bool resumeMusic();
	bool isPlayingMusic();
	inline bool isPausingMusic() const { return _isPausing; }
	void setMusicVolume(f32 volume);
	void setCurrentLoopCount(s32 cntLoop);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	IosAudio();
	~IosAudio();
	bool create();
	void destroy();
	void releasePlayer();

	//======================================================================
	// 変数
private:
	TYPE_IOS_AUDIO_PLAYER* _playerMusic;
	TYPE_IOS_AUDIO_DELEGATER* _audiodelg;
	bool _isInitPlayer;
	bool _isPausing;
	f32 _volume;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
// グローバル関数

TFW_BEGIN_NS

#if 0
extern bool IosAudio_loadAudioData(const char* nameFile, const char* typeFile, void** data, ALsizei* sizeData, ALenum* format, ALsizei* samplerate);
#endif
	
extern void IosAudio_alBufferDataStatic(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SOUND_IOS_AUDIO_H_