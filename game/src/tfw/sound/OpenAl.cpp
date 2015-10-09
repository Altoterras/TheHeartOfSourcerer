/***********************************************************************//**
 *  OpenAl.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/10/09.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OpenAl.h"

// Friends
#include "Wave.h"
#include "../collection/List.h"
#include "../lib/Vector.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define EVERY_CHECK_ERROR	1

////////////////////////////////////////////////////////////////////////////
// 型定義

typedef ALvoid (AL_APIENTRY *alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OpenAl メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	バッファーデータ設定
**//*---------------------------------------------------------------------*/
bool OpenAl::setBufferData(s32 bufidx, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
#if USE_OPENAL
	if((bufidx < 0) || (_numBuf <= bufidx))	{	ASSERT(false);	return false;	}
	ALuint bufid = _bufid[bufidx];

	#if defined(_WINDOWS) || defined(_ANDROID)
	{
		::alBufferData(bufid, format, data, size, freq);
	}
	#elif defined(_IOS)
	{
		#define USE_STATIC	0
		#if USE_STATIC
		{
			static alBufferDataStaticProcPtr	proc = 0L;
			// 関数アドレス取得
			if(proc == 0L)
			{
				proc = (alBufferDataStaticProcPtr)::alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
			}
			if(proc != 0L)
			{
				proc(bufid, format, data, size, freq);
			}
		}
		#else
		{
			::alBufferData(bufid, format, data, size, freq);
		}
		#endif
	}
	#endif

	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::setBufferData} error! : setBufferData faild. - %x\n", error);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	バッファーデータ設定
**//*---------------------------------------------------------------------*/
bool OpenAl::setBufferData(s32 bufidx, Wave* wav)
{
#if USE_OPENAL
	return setBufferData(bufidx, wav, 0, S32_MAX);
#else	// USE_OPENAL
	return true;
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	バッファーデータ設定
**//*---------------------------------------------------------------------*/
bool OpenAl::setBufferData(s32 bufidx, Wave* wav, s32 sampleStartLoopPoint, s32 sampleEndLoopPoint)
{
#if USE_OPENAL
	// フォーマット
	ALenum fmt = AL_FORMAT_STEREO16;
	switch(wav->getChannelNum())
	{
	case 1:
		switch(wav->getBitsPerSample())
		{
		case 8:		fmt = AL_FORMAT_MONO8;		break;
		case 16:	fmt = AL_FORMAT_MONO16;		break;
		}
		break;
	case 2:
		switch(wav->getBitsPerSample())
		{
		case 8:		fmt = AL_FORMAT_STEREO8;	break;
		case 16:	fmt = AL_FORMAT_STEREO16;	break;
		}
		break;
	}

	// 周波数
	ALsizei freq = wav->getFrequency();

	// バッファに設定
	s32 rateSampleToSize = wav->getChannelNum() * (wav->getBitsPerSample() / 8);
	ALsizei sizeStartLoopPoint = sampleStartLoopPoint * rateSampleToSize;
	ALsizei sizeEndLoopPoint = (sampleEndLoopPoint == S32_MAX) ? wav->getDataSize() : sampleEndLoopPoint * rateSampleToSize;
	if(!setBufferData(bufidx, fmt, wav->data() + sizeStartLoopPoint, sizeEndLoopPoint - sizeStartLoopPoint, freq))
	{
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	バッファーデータクリア
**//*---------------------------------------------------------------------*/
bool OpenAl::clearBufferData(s32 bufidx)
{
#if USE_OPENAL
	::alBufferi(bufidx, AL_SIZE, 0);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::clearBufferData} error! : alBufferi faild. - %x\n", error);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	再生
**//*---------------------------------------------------------------------*/
bool OpenAl::play(s32 trkidx)
{
#if USE_OPENAL
	::alSourcePlay(_track[trkidx]._srcid);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::play} error! : alSourcePlay faild. - %x\n", error);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	停止
**//*---------------------------------------------------------------------*/
bool OpenAl::stop(s32 trkidx)
{
#if USE_OPENAL
	::alSourceStop(_track[trkidx]._srcid);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::stop} error! : alSourceStop faild. - %x\n", error);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	一時停止
**//*---------------------------------------------------------------------*/
bool OpenAl::pause(s32 trkidx)
{
#if USE_OPENAL
	::alSourcePause(_track[trkidx]._srcid);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::play} error! : alSourcePause faild. - %x\n", error);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	再開
**//*---------------------------------------------------------------------*/
bool OpenAl::resume(s32 trkidx)
{
	return play(trkidx);
}

/*---------------------------------------------------------------------*//**
	トラックのキューにバッファを追加する
**//*---------------------------------------------------------------------*/
bool OpenAl::queueBuffers(s32 trkidx, s32 bufidx)
{
	return queueBuffers(trkidx, &bufidx, 1);
}

/*---------------------------------------------------------------------*//**
	トラックのキューにバッファを追加する
**//*---------------------------------------------------------------------*/
bool OpenAl::queueBuffers(s32 trkidx, s32* idxarrBuf, s32 numArr)
{
	bool isSucceeded = true;
#if USE_OPENAL
	ALuint* arrbid = 0L;

	// バッファ ID 配列確保
	static const s32 numStaticArr = 8;
	static ALuint arrbidStatic[numStaticArr];
	if(numArr <= numStaticArr)
	{
		arrbid = arrbidStatic;
	}
	else
	{
		arrbid = new ALuint[numArr];
	}

	// バッファ ID 配列作成
	for(int i = 0; i < numArr; i++)
	{
		ASSERT((0 <= idxarrBuf[i]) && (idxarrBuf[i] < _numBuf));
		arrbid[i] = _bufid[idxarrBuf[i]];
	}

	// OpenAL キューにバッファを追加
	::alSourceQueueBuffers(_track[trkidx]._srcid, numArr, arrbid);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::queueBuffers} error! : alSourceQueueBuffers faild. - %x\n", error);
		isSucceeded = false;
	}

	// バッファ ID 配列解放
	if(arrbid != arrbidStatic)
	{
		delete[] arrbid;
	}
#endif	// USE_OPENAL

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	トラックのキューからバッファを削除する
**//*---------------------------------------------------------------------*/
bool OpenAl::unqueueBuffers(s32 trkidx, s32 bufidx)
{
	return unqueueBuffers(trkidx, &bufidx, 1);
}

/*---------------------------------------------------------------------*//**
	トラックのキューからバッファを削除する
**//*---------------------------------------------------------------------*/
bool OpenAl::unqueueBuffers(s32 trkidx, s32* idxarrBuf, s32 numArr)
{
	bool isSucceeded = true;
#if USE_OPENAL
	ALuint* arrbid = 0L;

	// バッファ ID 配列確保
	static const s32 numStaticArr = 8;
	static ALuint arrbidStatic[numStaticArr];
	if(numArr <= numStaticArr)
	{
		arrbid = arrbidStatic;
	}
	else
	{
		arrbid = new ALuint[numArr];
	}

	// バッファ ID 配列作成
	for(int i = 0; i < numArr; i++)
	{
		ASSERT((0 <= idxarrBuf[i]) && (idxarrBuf[i] < _numBuf));
		arrbid[i] = _bufid[idxarrBuf[i]];
	}

	// OpenAL キューからバッファを削除
	::alSourceUnqueueBuffers(_track[trkidx]._srcid, numArr, arrbid);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::unqueueBuffers} error! : alSourceUnqueueBuffers faild. - %x\n", error);
		isSucceeded = false;
	}

	// バッファ ID 配列解放
	if(arrbid != arrbidStatic)
	{
		delete[] arrbid;
	}
#endif	// USE_OPENAL

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	トラックのキューからバッファをクリア（全削除）する
**//*---------------------------------------------------------------------*/
bool OpenAl::clearQueueBuffers(s32 trkidx)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcei(_track[trkidx]._srcid, AL_BUFFER, AL_NONE);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::clearQueueBuffers} error! : alSourcei(AL_BUFFER) faild. - %x\n", error);

		ALint stat;
		::alGetSourcei(_track[trkidx]._srcid, AL_SOURCE_STATE, &stat);
		TRACE("{OpenAl::clearQueueBuffers} error! : alSourcei(AL_BUFFER) faild. - stat=%d\n", stat);

		ASSERT(false);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	自動ループを設定する
**//*---------------------------------------------------------------------*/
bool OpenAl::setAutoLoop(s32 trkidx, bool isLoop)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcei(_track[trkidx]._srcid, AL_LOOPING, isLoop);
	ALenum error;
	if((error = ::alGetError()) != AL_NO_ERROR)
	{
		TRACE("{OpenAl::setAutoLoop} error! : alSourcei(AL_LOOPING) faild. - %x\n", error);
		ASSERT(false);
		return false;
	}
#endif	// USE_OPENAL

	return true;
}

/*---------------------------------------------------------------------*//**
	再生トラックのサンプルオフセット値を得る
**//*---------------------------------------------------------------------*/
s32 OpenAl::getSampleOffset(s32 trkidx)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	ALint smpofs = 0;
	::alGetSourcei(_track[trkidx]._srcid, AL_SAMPLE_OFFSET, &smpofs);
	return smpofs;
#else	// USE_OPENAL
	return 0;
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	リスナの位置を設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setListenerPosition(const Vector3F* pos)
{
#if USE_OPENAL
	::alListenerfv(AL_POSITION, pos->_v);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setListenerPosition} error! : alListener3f(AL_POSITION) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL

	_posListener->copy(pos);	// 現在位置を保存
}

/*---------------------------------------------------------------------*//**
	リスナの向きを設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setListenerDirection(const Vector3F* dir)
{
#if USE_OPENAL
	::alListenerfv(AL_ORIENTATION, dir->_v);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setListenerDirection} error! : alListener3f(AL_ORIENTATION) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックの位置を設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackPosition(s32 trkidx, const Vector3F* pos)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcefv(_track[trkidx]._srcid, AL_POSITION, pos->_v);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setListenerDirection} error! : alListener3f(AL_ORIENTATION) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックの速度を設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackVelocity(s32 trkidx, const Vector3F* vel)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcefv(_track[trkidx]._srcid, AL_VELOCITY, vel->_v);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setListenerDirection} error! : alListener3f(AL_ORIENTATION) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックのゲインを設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackGain(s32 trkidx, f32 gain)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcef(_track[trkidx]._srcid, AL_GAIN, gain);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackGain} error! : alListener3f(AL_GAIN) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックの最小ゲインを設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackMinGain(s32 trkidx, f32 gain)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcef(_track[trkidx]._srcid, AL_MIN_GAIN, gain);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackMinGain} error! : alListener3f(AL_MIN_GAIN) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックの最大ゲインを設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackMaxGain(s32 trkidx, f32 gain)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcef(_track[trkidx]._srcid, AL_MAX_GAIN, gain);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackMaxGain} error! : alListener3f(AL_MAX_GAIN) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックの基準距離を設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackReferenceDistance(s32 trkidx, f32 dist)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcef(_track[trkidx]._srcid, AL_REFERENCE_DISTANCE, dist);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackReferenceDistance} error! : alListener3f(AL_REFERENCE_DISTANCE) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックの最大距離を設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackMaxDistance(s32 trkidx, f32 dist)
{
#if USE_OPENAL
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcef(_track[trkidx]._srcid, AL_MAX_DISTANCE, dist);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackMaxDistance} error! : alListener3f(AL_MAX_DISTANCE) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックを UI 用に設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackForUi(s32 trkidx)
{
#if USE_OPENAL
	// リスなの位置から影響を受けないように設定
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcei(_track[trkidx]._srcid, AL_SOURCE_RELATIVE, AL_FALSE);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackForUi} error! : alListener3f(AL_SOURCE_RELATIVE) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	トラックを BGM 用に設定する
**//*---------------------------------------------------------------------*/
void OpenAl::setTrackForBgm(s32 trkidx)
{
#if USE_OPENAL
	// リスなの位置から影響を受けないように設定
	ASSERT((0 <= trkidx) && (trkidx < _numTrack));
	::alSourcei(_track[trkidx]._srcid, AL_SOURCE_RELATIVE, AL_FALSE);
	#if EVERY_CHECK_ERROR
		ALenum error;
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::setTrackForUi} error! : alListener3f(AL_SOURCE_RELATIVE) faild. - %x\n", error);
			ASSERT(false);
		}
	#endif
#endif	// USE_OPENAL
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OpenAl::OpenAl()
	: _posListener(0L)
#if USE_OPENAL
	, _device(0L)
	, _context(0L)
	, _bufid(0L)
	, _numBuf(0)
	, _track(0L)
	, _numTrack(0)
#endif	// USE_OPENAL
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
OpenAl::~OpenAl()
{
	ASSERT(_posListener == 0L);
#if USE_OPENAL
	ASSERT(_context == 0L);
	ASSERT(_device == 0L);
	ASSERT(_bufid == 0L);
	ASSERT(_track == 0L);
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool OpenAl::create(s32 numBuf, s32 numTrack)
{
#if USE_OPENAL
	ALenum error;

	// OpenAL デバイスを開く
	_device = ::alcOpenDevice(0L);
	if(_device == 0L)
	{
		TRACE("{OpenAl::create} error! : alcOpenDevice faild.\n");
		return false;
	}
	TRACE("{OpenAl::create} Using OpenAL Device '%s'\n", ::alcGetString(_device, ALC_DEVICE_SPECIFIER));

	// OpenAL コンテキストの作成
	_context = ::alcCreateContext(_device, 0L);
	if(_context == 0L)
	{
		TRACE("{OpenAl::create} error! : alcCreateContext faild.\n");
		return false;
	}

	// アクティブなコンテキストを作成する
	::alcMakeContextCurrent(_context);

	// バッファの作成
	_bufid = new ALuint[numBuf];
	if(_bufid != 0L)
	{
		_numBuf = numBuf;
		::alGenBuffers(_numBuf, _bufid);
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::create} error! : alGenBuffers faild. - %x\n", error);
			return false;
		}
	}

	// トラックの作成
	_track = new Track[numTrack];
	if(_track == 0L)
	{
		TRACE("{OpenAl::create} create Track faild\n");
		return false;
	}
	_numTrack = numTrack;

	// ソースの作成
	for(int i = 0; i < _numTrack; i++)
	{
		::alGenSources(1, &_track[i]._srcid);
		if((error = ::alGetError()) != AL_NO_ERROR)
		{
			TRACE("{OpenAl::create} error! : alGenSources faild. - %x\n", error);
		}
	}

	#if 0
		ALint pv = alGetInteger(AL_DISTANCE_MODEL);
		alDistanceModel(AL_LINEAR_DISTANCE);
		pv = alGetInteger(AL_DISTANCE_MODEL);
		TRACE("%x\n", pv);
	#endif
#endif	// USE_OPENAL

	// 現在のリスナー位置ベクトルを作成
	_posListener = new Vector3F();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void OpenAl::destroy()
{
	// 現在のリスナー位置ベクトルを削除
	delete _posListener;
	_posListener = 0L;

#if USE_OPENAL
	if(_context != 0L)
	{
		// ソース・トラックの削除
		for(int i = 0; i < _numTrack; i++)
		{
			if(::alIsSource(_track[i]._srcid))
			{
				::alSourceStop(_track[i]._srcid);
				::alSourcei(_track[i]._srcid, AL_BUFFER, 0);
				::alDeleteSources(1, &_track[i]._srcid);
				_track[i]._srcid = 0;
			}
		}
		delete[] _track;
		_track = 0L;
		_numTrack = 0;

		// バッファの削除
		for(int i = 0; i < _numBuf; i++)
		{
			if(::alIsBuffer(_bufid[i]))
			{
				::alDeleteBuffers(1, &_bufid[i]);
				_bufid[i] = 0;
			}
		}
		delete[] _bufid;
		_bufid = 0L;
		_numBuf = 0;

		// OpenAL コンテキストの破棄
		::alcMakeContextCurrent(0);
		::alcDestroyContext(_context);
		_context = 0;
	}
	ASSERT(_track == 0L);

	// OpenAL デバイスを閉じる
	if(_device != 0L)
	{
		::alcCloseDevice(_device);
		_device = 0L;
	}
#endif	// USE_OPENAL
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void OpenAl::exec()
{
#if USE_OPENAL
	ALint stat;
	for(int i = 0; i < _numTrack; i++)
	{
		::alGetSourcei(_track[i]._srcid, AL_SOURCE_STATE, &stat);
		switch(stat)
		{
		case AL_INITIAL:	_track[i]._stat = STAT_INIT;	break;
		case AL_PLAYING:	_track[i]._stat = STAT_PLAY;	break;
		case AL_PAUSED:		_track[i]._stat = STAT_PAUSE;	break;
		case AL_STOPPED:	_track[i]._stat = STAT_STOP;	break;
		}
	}

	/*
	if(stat == AL_PLAYING)
	{
		ALint smpofs = 0;
		::alGetSourcei(_track._srcid, AL_SAMPLE_OFFSET, &smpofs);
		//TRACE("{OpenAl::exec} sample offset: %d\n", smpofs);

		if((smpofs >= _track._smpofsNextQueueing) && (_track._smpofsNextQueueing > 0))	// 次のキュー位置まで再生した
		{
			_track._cntLoop++;
			_track._smpofsNextQueueing += (_track._smpofsLoopEnd - _track._smpofsLoopStart);
			TRACE("{OpenAl::exec} setup loop point : current_sample_offset=%d, next_sample_offset=%d, num=%d\n", smpofs, _track._smpofsNextQueueing, _track._cntLoop);
			// その次のバッファをキューに入れる
			::alSourceQueueBuffers(_track._srcid, 1, &_bufid[1]);

			TRACE("{OpenAl::exec} alSourceQueueBuffers : %d\n", _bufid[1]);
			ALenum error;
			if((error = ::alGetError()) != AL_NO_ERROR)
			{
				TRACE("{OpenAl::exec} error! : alSourceQueueBuffers faild. - %x\n", error);
			}
		}
	}
	*/

	/*
	if(_isPlayIntro)
	{
		ALint bufid = 0;
		::alGetSourcei(_track._srcid, AL_BUFFER, &bufid);
		TRACE("{OpenAl::exec} bufid = %d\n", bufid);
		if(bufid == _bufid[1])
		{
			::alSourceUnqueueBuffers(_track._srcid, 1, &_bufid[0]);
			TRACE("{OpenAl::exec} alSourceUnqueueBuffers : %d\n", _bufid[0]);
			ALenum error;
			if((error = ::alGetError()) != AL_NO_ERROR)
			{
				TRACE("{OpenAl::exec} error! : alGenBuffers faild. - %x\n", error);
			}

			_isPlayIntro = false;
		}
	}
	*/
#endif	// USE_OPENAL
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS