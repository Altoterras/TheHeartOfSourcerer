/***********************************************************************//**
 *	OpenSl.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/12/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_OPEN_SL_H_
#define _TFW_SOUND_OPEN_SL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#if defined(_ANDROID)
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#elif defined(_WINDOWS)
#include <OpenSL/OpenSLES.h>
#endif

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_ANDROID)
#define USE_OPENSL		1
#else
#define USE_OPENSL		0
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class File;

#if !defined(_ANDROID)
typedef void* SLAndroidSimpleBufferQueueItf;
#endif

/*---------------------------------------------------------------------*//**
 *	OpenSL モジュール
 *		※ Sound クラスのサブモジュール
 *	
**//*---------------------------------------------------------------------*/
class OpenSl  
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	トラック
	 *	
	**//*-----------------------------------------------------------------*/
	class Track
	{
	public:
		OpenSl* _ownRef;
		
#if USE_OPENSL
		SLObjectItf _objPlayer;
		SLPlayItf _playPlayer;
		SLSeekItf _seekPlayer;
		SLVolumeItf _volumePlayer;
		SLEffectSendItf _effectPlayer;
		SLAndroidSimpleBufferQueueItf _bqPlayer;
#endif

		short* _bufferNext;
		unsigned _sizeNext;
		int _countNext;

		Track();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool createBufferQueueTrack(s32 trkidx);
	bool createFileTrack(s32 trkidx, File* fileSound, u32 length);
	bool destroyTrack(s32 trkidx);

	bool play(s32 trkidx);
	bool stop(s32 trkidx);
	bool pause(s32 trkidx);
	bool resume(s32 trkidx);
	bool isPlaying(s32 trkidx);
	bool isPausing(s32 trkidx);
	bool setVolume(s32 trkidx, f32 volume);
	bool setCurrentLoop(s32 trkidx, bool isLoop);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OpenSl();
	virtual ~OpenSl();
	bool create(s32 numTrack);
	void destroy();
	void exec();

	void callbackBufferQueuePlayer(Track* tr, SLAndroidSimpleBufferQueueItf bq);

	//======================================================================
	// 変数
private:
#if USE_OPENSL
	SLObjectItf _objEngine;
	SLEngineItf _engineEngine;
	SLObjectItf _objOutputMix;
	SLEnvironmentalReverbItf _revbOutputMix;
	Track* _track;
	s32 _numTrack;
#endif	// USE_OPENSL
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_OPEN_SL_H_
