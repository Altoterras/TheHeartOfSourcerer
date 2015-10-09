/***********************************************************************//**
 *  OpenSl.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/18.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OpenSl.h"

// Friends
#include "../file/File.h"

// External
#if defined(_ANDROID)
#include <AL/alslbridge.h>
#endif

// Library
#include <math.h>

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

#if USE_OPENSL
/*---------------------------------------------------------------------*//**
	バッファ再生が終了したときのコールバックハンドラ（静的関数）
**//*---------------------------------------------------------------------*/
static void callbackBufferQueuePlayerStatic(SLAndroidSimpleBufferQueueItf bq, void* context)
{
	OpenSl::Track* tr = (OpenSl::Track*)context;
	tr->_ownRef->callbackBufferQueuePlayer(tr, bq);
}
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OpenSl::Track メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OpenSl::Track::Track()
	: _ownRef(0L)
#if	USE_OPENSL
	, _objPlayer(0L)
	, _playPlayer(0L)
	, _seekPlayer(0L)
	, _volumePlayer(0L)
	, _effectPlayer(0L)
	, _bqPlayer(0L)
#endif	// USE_OPENSL
	, _bufferNext(0L)
	, _sizeNext(0)
	, _countNext(0)
{
}

//==========================================================================
// OpenSl メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	サウンド ファイルを設定する
**//*---------------------------------------------------------------------*/
bool OpenSl::createFileTrack(s32 trkidx, File* fileSound, u32 length)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	u32 start = fileSound->getPosition();
	if(length == 0)	{	length = fileSound->getLength();	}
	addr_size fd = reinterpret_cast<addr_size>(fileSound->getDescriptor());

	SLresult result;

	// オーディオソースの設定
	SLDataLocator_AndroidFD locFd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
	SLDataFormat_MIME formatMime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
	SLDataSource srcAudio = { &locFd, &formatMime };

	// オーディオシンクの設定
	SLDataLocator_OutputMix locOutmix = { SL_DATALOCATOR_OUTPUTMIX, _objOutputMix };
	SLDataSink sinkAudio = { &locOutmix, NULL };

	// オーディオプレイヤーの作成
	const SLInterfaceID ids[2] = { SL_IID_SEEK, /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME };
	const SLboolean req[2] = { SL_BOOLEAN_TRUE, /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE };
	result = (*_engineEngine)->CreateAudioPlayer(_engineEngine, &tr->_objPlayer, &srcAudio, &sinkAudio, 2, ids, req);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createFilePlayer} CreateAudioPlayer failed.");
		return false;
	}

	// オーディオプレイヤーの実体化
	result = (*tr->_objPlayer)->Realize(tr->_objPlayer, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createFilePlayer} _objPlayer->player failed.");
		return false;
	}

	// 再生インターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_PLAY, &tr->_playPlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createFilePlayer} GetInterface(SL_IID_PLAY) failed.");
		return false;
	}

	// シークインターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_SEEK, &tr->_seekPlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createFilePlayer} GetInterface(SL_IID_SEEK) failed.");
		return false;
	}

	// ボリュームインターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_VOLUME, &tr->_volumePlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createFilePlayer} GetInterface(SL_IID_VOLUME) failed.");
		return false;
	}

	// ループ設定
	result = (*tr->_seekPlayer)->SetLoop(tr->_seekPlayer, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createFilePlayer} SetLoop failed.");
		return false;
	}
#endif	// USE_OPENSL

	return true;
}

/*---------------------------------------------------------------------*//**
	バッファ キュー プレイヤーの作成
**//*---------------------------------------------------------------------*/
bool OpenSl::createBufferQueueTrack(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	SLresult result;

	// オーディオソースの設定
	SLDataLocator_AndroidSimpleBufferQueue locBufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM formatPcm =
	{
		SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_8,
		SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN
	};
	SLDataSource srcAudio = { &locBufq, &formatPcm };

	// オーディオシンクの設定
	SLDataLocator_OutputMix locOutmix = { SL_DATALOCATOR_OUTPUTMIX, _objOutputMix };
	SLDataSink sinkAudio = { &locOutmix, NULL };

	// オーディオプレイヤーの作成
	const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
	const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
	result = (*_engineEngine)->CreateAudioPlayer(_engineEngine, &tr->_objPlayer, &srcAudio, &sinkAudio, 3, ids, req);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} CreateAudioPlayer failed.");
		return false;
	}

	// オーディオプレイヤーの実体化
	result = (*tr->_objPlayer)->Realize(tr->_objPlayer, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} _objPlayer->Realize failed.");
		return false;
	}

	// 再生インターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_PLAY, &tr->_playPlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} GetInterface(SL_IID_PLAY) failed.");
		return false;
	}

	// バッファキューインターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_BUFFERQUEUE, &tr->_bqPlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} GetInterface(SL_IID_BUFFERQUEUE) failed.");
		return false;
	}

	// バッファキューコールバックの登録
	result = (*tr->_bqPlayer)->RegisterCallback(tr->_bqPlayer, callbackBufferQueuePlayerStatic, tr);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} RegisterCallback failed.");
		return false;
	}

#if 0
	// エフェクト送信インターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_EFFECTSEND, &tr->_effectPlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} GetInterface(SL_IID_EFFECTSEND) failed.");
		return false;
	}
#endif

	// ボリュームインターフェイスの取得
	result = (*tr->_objPlayer)->GetInterface(tr->_objPlayer, SL_IID_VOLUME, &tr->_volumePlayer);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::createBufferQueuePlayer} GetInterface(SL_IID_VOLUME) failed.");
		return false;
	}
#endif	// USE_OPENSL

	return true;
}

/*---------------------------------------------------------------------*//**
	プレイヤー破棄
**//*---------------------------------------------------------------------*/
bool OpenSl::destroyTrack(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_objPlayer != NULL)
	{
		(*tr->_objPlayer)->Destroy(tr->_objPlayer);
		tr->_objPlayer = 0L;
		tr->_playPlayer = 0L;
		tr->_seekPlayer = 0L;
		tr->_volumePlayer = 0L;
		tr->_effectPlayer = 0L;
		tr->_bqPlayer = 0L;
	}
#endif	// USE_OPENSL

	return true;
}

/*---------------------------------------------------------------------*//**
	再生
**//*---------------------------------------------------------------------*/
bool OpenSl::play(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_playPlayer == 0L)	{	return false;	}

	SLresult result = (*tr->_playPlayer)->SetPlayState(tr->_playPlayer, SL_PLAYSTATE_PLAYING);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
#endif	// USE_OPENSL
	
	return true;
}

/*---------------------------------------------------------------------*//**
	停止
**//*---------------------------------------------------------------------*/
bool OpenSl::stop(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_playPlayer == 0L)	{	return false;	}

	SLresult result = (*tr->_playPlayer)->SetPlayState(tr->_playPlayer, SL_PLAYSTATE_STOPPED);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
#endif	// USE_OPENSL
	
	return true;
}

/*---------------------------------------------------------------------*//**
	一時停止
**//*---------------------------------------------------------------------*/
bool OpenSl::pause(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_playPlayer == 0L)	{	return false;	}

	SLresult result = (*tr->_playPlayer)->SetPlayState(tr->_playPlayer, SL_PLAYSTATE_PAUSED);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
#endif	// USE_OPENSL
	
	return true;
}

/*---------------------------------------------------------------------*//**
	再開
**//*---------------------------------------------------------------------*/
bool OpenSl::resume(s32 trkidx)
{
	return play(trkidx);
}

/*---------------------------------------------------------------------*//**
	再生しているかどうか
**//*---------------------------------------------------------------------*/
bool OpenSl::isPlaying(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_playPlayer == 0L)	{	return false;	}

	SLuint32 state;
	SLresult result = (*tr->_playPlayer)->GetPlayState(tr->_playPlayer, &state);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
	
	return (state == SL_PLAYSTATE_PLAYING);
#else	// USE_OPENSL
	return false;
#endif	// USE_OPENSL
}

/*---------------------------------------------------------------------*//**
	一時停止しているかどうか
**//*---------------------------------------------------------------------*/
bool OpenSl::isPausing(s32 trkidx)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_playPlayer == 0L)	{	return false;	}

	SLuint32 state;
	SLresult result = (*tr->_playPlayer)->GetPlayState(tr->_playPlayer, &state);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
	
	return (state == SL_PLAYSTATE_PAUSED);
#else	// USE_OPENSL
	return false;
#endif	// USE_OPENSL
}

/*---------------------------------------------------------------------*//**
	ボリュームの設定
**//*---------------------------------------------------------------------*/
bool OpenSl::setVolume(s32 trkidx, f32 volume)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_volumePlayer == 0L)	{	return false;	}

	f32 attenuation = (volume < 0.01f) ? -96.0f : 20 * log10f(volume);
	SLmillibel millibel = (SLmillibel)(attenuation * 1000.0f);

	SLresult result = (*tr->_volumePlayer)->SetVolumeLevel(tr->_volumePlayer, millibel);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
#endif	// USE_OPENSL

	return true;
}

/*---------------------------------------------------------------------*//**
	現在の再生曲のループ数の設定
**//*---------------------------------------------------------------------*/
bool OpenSl::setCurrentLoop(s32 trkidx, bool isLoop)
{
#if USE_OPENSL
	Track* tr = &_track[trkidx];

	if(tr->_seekPlayer == 0L)	{	return false;	}

	SLresult result = (*tr->_seekPlayer)->SetLoop(tr->_seekPlayer, (SLboolean)isLoop, 0, SL_TIME_UNKNOWN);
	if(result != SL_RESULT_SUCCESS)
	{
		return false;
	}
#endif	// USE_OPENSL

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OpenSl::OpenSl()
#if USE_OPENSL
	: _objEngine(0L)
	, _engineEngine(0L)
	, _objOutputMix(0L)
	, _revbOutputMix(0L)
	, _track(0L)
	, _numTrack(0)
#endif	// USE_OPENSL
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
OpenSl::~OpenSl()
{
#if USE_OPENSL
	ASSERT(_objEngine == 0L);
	ASSERT(_engineEngine == 0L);
	ASSERT(_objOutputMix == 0L);
	ASSERT(_revbOutputMix == 0L);
	ASSERT(_track == 0L);
#endif	// USE_OPENSL
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool OpenSl::create(s32 numTrack)
{
#if USE_OPENSL
	SLresult result;

	// OpenSL エンジンの作成
	result = slCreateEngine(&_objEngine, 0, NULL, 0, NULL, NULL);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::create} slCreateEngine failed.");
		return false;
	}

	// OpenSL エンジンの実体化
	result = (*_objEngine)->Realize(_objEngine, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::create} _objEngine->Realize failed.");
		return false;
	}

	// OpenSL エンジンのインターフェイスを取得
	result = (*_objEngine)->GetInterface(_objEngine, SL_IID_ENGINE, &_engineEngine);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::create} GetInterface(SL_IID_ENGINE) failed.");
		return false;
	}

	// 出力ミキサーの作成（環境リバーブの取得は必須ではない）
	const SLInterfaceID ifid[1] = { SL_IID_ENVIRONMENTALREVERB };
	const SLboolean req[1] = { SL_BOOLEAN_FALSE };
	result = (*_engineEngine)->CreateOutputMix(_engineEngine, &_objOutputMix, 1, &SL_IID_ENVIRONMENTALREVERB, req);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::create} CreateOutputMix failed.");
		return false;
	}

	// 出力ミキサーの実体化
	result = (*_objOutputMix)->Realize(_objOutputMix, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		ASSERTM(SL_RESULT_SUCCESS == result, "{OpenSl::create} _objOutputMix->Realize failed.");
		return false;
	}

#if 0
	// 環境リバーブのインターフェイスを取得
	result = (*_objOutputMix)->GetInterface(_objOutputMix, SL_IID_ENVIRONMENTALREVERB, &_revbOutputMix);
	if(SL_RESULT_SUCCESS == result)
	{
		result = (*_revbOutputMix)->SetEnvironmentalReverbProperties(_revbOutputMix, &_reverbSettings);
	}
#endif

	// トラックの作成
	_track = new Track[numTrack];
	if(_track == 0L)
	{
		TRACE("{OpenSl::create} create Track faild\n");
		return false;
	}
	_numTrack = numTrack;
	for(int i = 0; i < _numTrack; i++)
	{
		_track[i]._ownRef = this;
	}

	#if defined(_ANDROID) && USE_EXTERNAL_ENGINE_INSTANCE
		// OpenAL backend にインターフェイス登録
		alslbridge_setOpenSlInterfaceToOpenAlBackend(_objEngine, _engineEngine, _objOutputMix);
	#endif
#endif	// USE_OPENSL

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void OpenSl::destroy()
{
#if USE_OPENSL
	// トラックの削除
	for(int i = 0; i < _numTrack; i++)
	{
		destroyTrack(i);
	}
	delete[] _track;
	_track = 0L;
	_numTrack = 0;

	#if defined(_ANDROID) && USE_EXTERNAL_ENGINE_INSTANCE
		// OpenAL backend からインターフェイス抹消
		alslbridge_setOpenSlInterfaceToOpenAlBackend(NULL, NULL, NULL);
	#endif

	// 出力ミキサーの削除
	if(_objOutputMix != NULL)
	{
		(*_objOutputMix)->Destroy(_objOutputMix);
		_objOutputMix = NULL;
		_revbOutputMix = NULL;
	}

	// OpenSL エンジンの削除
	if(_objEngine != NULL)
	{
		(*_objEngine)->Destroy(_objEngine);
		_objEngine = NULL;
		_engineEngine = NULL;
	}
#endif	// USE_OPENSL
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void OpenSl::exec()
{
}

/*---------------------------------------------------------------------*//**
	バッファ再生が終了したときのコールバック ハンドラ
**//*---------------------------------------------------------------------*/
void OpenSl::callbackBufferQueuePlayer(Track* tr, SLAndroidSimpleBufferQueueItf bq)
{
#if USE_OPENSL
	ASSERT(bq == tr->_bqPlayer);

	// 次のバッファが設定されていたらキューに置く
	tr->_countNext--;
	if((tr->_countNext > 0) && (tr->_bufferNext != 0L) && (tr->_sizeNext != 0))
	{
		SLresult result =(*tr->_bqPlayer)->Enqueue(tr->_bqPlayer, tr->_bufferNext, tr->_sizeNext);
		ASSERT(SL_RESULT_SUCCESS == result);	// SL_RESULT_BUFFER_INSUFFICIENT が出る可能性が高い
	}
#endif	// USE_OPENSL
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
