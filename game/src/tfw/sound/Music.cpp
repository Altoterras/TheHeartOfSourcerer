/***********************************************************************//**
 *  Music.cpp
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
#include "Music.h"

// Friends
#if defined(_IOS)
	#include "IosAudio.h"
#endif
#include "OpenAlMusic.h"
#include "OpenSlMusic.h"
#include "Sound.h"
#include "WaveDef.h"
#include "../collection/List.h"
#include "../framemod/ExecCtx.h"
#include "../string/VcString.h"
#include "../string/VwString.h"
#include "../txt/implement/UnicodeTxt.h"
#include "../txt/implement/Utf8Txt.h"

// External

// Library
#if defined(_WIDNOWS)
	#include <Mmsystem.h>
#endif

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

// API 定義
#define API_OAL		1	// OpenAL を使う
#define API_OSL		2	// OpenSL を使う
#define API_MCI		3	// Windows 用 MCI を使う
#define API_IAV		4	// iOS 用 AVAudioPlayer を使う

// 使用する API
#if defined(_WINDOWS)
	//#define USE_API		API_OAL
	#define USE_API		API_MCI
#elif defined(_IOS)
	#define USE_API		API_IAV
	//#define USE_API		API_OAL
#elif defined(_ANDROID)
	//#define USE_API		API_OAL
	#define USE_API		API_OSL
#endif

////////////////////////////////////////////////////////////////////////////
// 定数

#define GAIN_DEFAULT				(1.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Music メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	曲を再生しているかどうか
**//*---------------------------------------------------------------------*/
bool Music::isPlaying() const
{
	return _stat == STAT_PLAYING;
}

/*---------------------------------------------------------------------*//**
	登録数を得る
**//*---------------------------------------------------------------------*/
s32 Music::getRegisterMusicNum() const
{
	return _listWave->getCount();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	再生

	@param id 曲 ID
	@param volume ボリューム
	@param cntLoop ループ回数（-1 で無限ループ）
	@param isForceRestart 強制的に最初から再生する
	@param frameFadeOut フェードアウトのフレーム数
	@param frameFadeIn フェードインのフレーム数
**//*---------------------------------------------------------------------*/
void Music::play(u16 id, f32 volume, s32 cntLoop, bool isForceRestart, s32 frameFadeOut, s32 frameFadeIn)
{
	// 同じ曲の場合の処理
	if(_id == id)
	{
		// 強制リスタート時以外はボリュームとループを設定して抜ける
		if(!isForceRestart)
		{
			// ボリューム設定
			setFadeIn(0.0f, volume);	// フェードイン機構に任せて設定

			// 現在の再生曲のループ数を設定
			#if (USE_API == API_IAV)
			{
				_player->setCurrentLoopCount(cntLoop);
			}
			#elif /*(USE_API == API_OAL) ||*/ (USE_API == API_OSL)
			{
				// 仮実装
				_player->setCurrentLoop(cntLoop != 0);
			}
			#endif

			return;
		}
	}

	// 予約
	_idReserved = id;
	_cntLoopMaxReserved = cntLoop;

	// フェードアウトセット
	//	※	現在再生中ではないときや、フェードアウト無しでも、
	//		フェードアウト処理後に予約再生を開始する設計のため、フェードアウトを設定する
	if(_id == 0)
	{
		setFadeOut((f32)0.0f);
	}
	else
	{
		setFadeOut((f32)frameFadeOut);
	}
	// フェードインセット
	//	※	フェードイン無しでも、
	//		現在のボリュームを設定するためには必要なため、フェードインを設定する
	//		（ただし、現在の設計では、１フレーム分ボリューム設定が遅れる）
	setFadeIn((f32)frameFadeIn, volume);
}

/*---------------------------------------------------------------------*//**
	停止

	@param frameFadeOut フェードアウトのフレーム数
**//*---------------------------------------------------------------------*/
void Music::stop(s32 frameFadeOut)
{
	// 予約
	_idReserved = 0;
	_cntLoopMaxReserved = 0;

	// フェードアウトセット
	if(_id != 0)	// 現在再生中の場合のみ
	{
		setFadeOut((f32)frameFadeOut);
	}
}

/*---------------------------------------------------------------------*//**
	一時停止

**//*---------------------------------------------------------------------*/
void Music::pause()
{
	TRACELN("{Music::pause}　pause");
	#if (USE_API == API_OAL) || (USE_API == API_OSL)
	{
		_player->pause();
	}
	#elif (USE_API == API_IAV)
	{
		_player->pauseMusic();
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	（一時停止からの）再開

**//*---------------------------------------------------------------------*/
void Music::resume()
{
	TRACELN("{Music::pause}　pause");
	#if (USE_API == API_OAL) || (USE_API == API_OSL)
	{
		_player->resume();
	}
	#elif (USE_API == API_IAV)
	{
		_player->resumeMusic();
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	曲を登録する
**//*---------------------------------------------------------------------*/
bool Music::registerMusic(u16 id, const CStringBase* pathDir, const CStringBase* filename, const CStringBase* type, s32 smplLoopStart, s32 smplLoopEnd)
{
	WaveDef* wdef = new WaveDef(
		id,
		pathDir,
		filename,
		type,
		smplLoopStart,
		smplLoopEnd		);
	_listWave->addTail(wdef);
	return true;
}

/*---------------------------------------------------------------------*//**
	曲を登録する
**//*---------------------------------------------------------------------*/
bool Music::registerMusic(u16 id, u32 fileid, u8 wfmt, s32 smplLoopStart, s32 smplLoopEnd)
{
	WaveDef* wdef = new WaveDef(
		id,
		fileid,
		wfmt,
		smplLoopStart,
		smplLoopEnd		);
	_listWave->addTail(wdef);
	return true;
}

/*---------------------------------------------------------------------*//**
	曲の登録を解除する
**//*---------------------------------------------------------------------*/
bool Music::unregisterMusic(u16 id)
{
	for(ListIterator<WaveDef*> it = _listWave->iterator(); it.has(); it.next())
	{
		WaveDef* wdef = it.object();
		if(wdef->_id == id)
		{
			_listWave->removeNode(it.node());
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	ボリュームを設定する
**//*---------------------------------------------------------------------*/
void Music::setVolume(f32 volume)
{
	if(_volume == volume)	{	return;	}

	_volume = volume;

	#if (USE_API == API_OAL) || (USE_API == API_OSL)
	{
		_player->setVolume(volume);
	}
	#elif (USE_API == API_IAV)
	{
		_player->setMusicVolume(volume);
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	リスナー位置変更通知
	※OpenAL のリスナー位置が変更したときに任意で呼び出す
	　（フレームワークが自動的には呼び出さないので注意）
**//*---------------------------------------------------------------------*/
void Music::notifyChangeListenerPosition()
{
	#if (USE_API == API_OAL)
	{
		_player->notifyChangeListenerPosition();
	}
	#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Music::Music()
	: _listWave(0L)
	, _player(0L)
	, _stat(STAT_NULL)
	, _id(0)
	, _volume(GAIN_DEFAULT)
	, _cntLoopRest(0)
	, _idReserved(0)
	, _cntLoopMaxReserved(0)
	, _fcntFadeOutCur(-1.0f)
	, _fcntFadeOutMax(0.0f)
	, _volumeFadeOutStart(0.0f)
	, _fcntFadeInCur(-1.0f)
	, _fcntFadeInMax(0.0f)
	, _volumeFadeInEnd(0.0f)
	, _sndRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Music::~Music()
{
	ASSERT(_player == 0L);
	ASSERT(_listWave == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Music::create(Sound* sndRef)
{
	// 参照を保存
	_sndRef = sndRef;

	#if (USE_API == API_OAL)
	{
		// OpenAL プレイヤーを作成する
		OpenAlMusic* alm = new OpenAlMusic();
		if(!alm->create(_sndRef->getOpenAl()))
		{
			delete alm;
			ASSERT(false);
			return false;
		}
		_player = alm;
	}
	#elif (USE_API == API_OSL)
	{
		// OpenSL プレイヤーを作成する
		OpenSlMusic* slm = new OpenSlMusic();
		if(!slm->create(_sndRef->getOpenSl()))
		{
			delete slm;
			ASSERT(false);
			return false;
		}
		_player = slm;
	}
	#elif (USE_API == API_IAV)
	{
		// iOS オーディオ プレイヤーを作成する
		IosAudio* ia = new IosAudio();
		if(!ia->create())
		{
			delete ia;
			ASSERT(false);
			return false;
		}
		_player = ia;
	}
	#endif

	// 音定義リストを作成
	_listWave = new List<WaveDef*>(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Music::destroy()
{
	#if (USE_API == API_MCI)
	{
		// MCI を全て閉じる
		::mciSendString(_T("close all"), 0L, 0, 0L);
	}
	#else
	{
		// オーディオ プレイヤーを削除する
		if(_player != 0L)
		{
			_player->destroy();
			delete _player;
			_player = 0L;
		}
	}
	#endif

	// 音定義リストを削除
	if(_listWave != 0L)
	{
		delete _listWave;
		_listWave = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Music::exec(const ExecCtx* ec)
{
	checkStatus();

	// フェード処理と予約曲再生／停止
	if(_fcntFadeOutCur >= 0.0f)
	{
		_fcntFadeOutCur += ec->getDeltaFrame();
		f32 volumeNew = _volumeFadeOutStart * (1.0f - (_fcntFadeOutCur / _fcntFadeOutMax));
		if(_fcntFadeOutCur >= _fcntFadeOutMax)
		{
			volumeNew = 0.0f;
			_fcntFadeOutCur = -1.0f;	// フェード終了

			// 次の曲へ
			if(_idReserved)
			{
				playNowReserved();	// 予約曲の再生
			}
			else
			{
				stopNow();	// 曲の停止
			}
		}
		setVolume(volumeNew);
	}
	else if(_fcntFadeInCur >= 0.0f)
	{
		_fcntFadeInCur += ec->getDeltaFrame();
		f32 volumeNew = _volumeFadeInEnd * _fcntFadeInCur / _fcntFadeInMax;
		if(_fcntFadeInCur >= _fcntFadeInMax)
		{
			volumeNew = _volumeFadeInEnd;
			_fcntFadeInCur = -1.0f;		// フェード終了
		}
		setVolume(volumeNew);
	}

	if(_stat == STAT_PLAYING)
	{
		// OpenAL のバッファループ処理
		#if (USE_API == API_OAL)
		{
			_player->exec(ec);
		}
		#endif
	}
}

/*---------------------------------------------------------------------*//**
	音定義を得る
**//*---------------------------------------------------------------------*/
WaveDef* Music::getWaveDef(u16 id)
{
	for(ListIterator<WaveDef*> it = _listWave->iterator(); it.has(); it.next())
	{
		WaveDef* wdef = it.object();
		if(wdef->_id == id)
		{
			return wdef;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	現在の状態を得る
**//*---------------------------------------------------------------------*/
void Music::checkStatus()
{
	if(_stat != STAT_NULL)
	{
		#if (USE_API == API_OAL) || (USE_API == API_OSL)
		{
			// OpenAL の状態を見て判断する
			if(_player->isPlaying() || _player->isPausing())
			{
				_stat = STAT_PLAYING;
			}
			else
			{
				if(_stat == STAT_PLAYING)	{	stopNow();	}	// 停止処理
				_stat = STAT_STOPPED;
			}
		}
		#elif (USE_API == API_IAV)
		{
			if(_player->isPlayingMusic() || _player->isPausingMusic())
			{
				_stat = STAT_PLAYING;
			}
			else
			{
				if(_stat == STAT_PLAYING)	{	stopNow();	}	// 停止処理
				_stat = STAT_STOPPED;
			}
		}
		#elif (USE_API == API_MCI)
		{
			// MCI の状態を見て判断する
			TCHAR buf[256];
			MCIERROR ret = ::mciSendString(_T("status tfw_music mode"), buf, sizeof(buf), 0L);
			if(ret == 0)
			{
				if(::_tcscmp(buf, _T("stopped")) == 0)
				{
					if(_stat == STAT_PLAYING)	{	stopNow();	}	// 停止処理
					_stat = STAT_STOPPED;

					// ループ処理
					if(_cntLoopRest > 0)
					{
						playNow(_id, _cntLoopRest - 1);	// 再生
					}
				}
				else if(::_tcscmp(buf, _T("playing")) == 0)
				{
					_stat = STAT_PLAYING;
				}
				else
				{
					TRACE("{Music::exec} MCI status : %s\n", buf);
				}
			}
		}
		#endif
	}
}

/*---------------------------------------------------------------------*//**
	フェードインの設定
**//*---------------------------------------------------------------------*/
void Music::setFadeIn(f32 fcntMax, f32 volume)
{
	_fcntFadeInCur = 0.0f;			// フェードイン開始
	_fcntFadeInMax = fcntMax;
	_volumeFadeInEnd = volume;
}

/*---------------------------------------------------------------------*//**
	フェードアウトの設定
**//*---------------------------------------------------------------------*/
void Music::setFadeOut(f32 fcntMax)
{
	_fcntFadeOutCur = 0.0f;			// フェードアウト開始
	_fcntFadeOutMax = fcntMax;
	_volumeFadeOutStart = _volume;
}

/*---------------------------------------------------------------------*//**
	予約曲の再生
**//*---------------------------------------------------------------------*/
bool Music::playNowReserved()
{
	bool succeeded = playNow(_idReserved, _cntLoopMaxReserved);

	// 失敗時に繰り返しトライしないように予約値をクリアする
	_idReserved = 0;
	_cntLoopMaxReserved = 0;

	return succeeded;
}

/*---------------------------------------------------------------------*//**
	再生
**//*---------------------------------------------------------------------*/
bool Music::playNow(u16 id, s32 cntLoop)
{
	// 音定義を得る
	WaveDef* wdef = getWaveDef(id);
	if(wdef == 0L)
	{
		ASSERT(false);
		return false;
	}
	
	// 状態の確認
	checkStatus();
	
	// 再生中、もしくは（OpenAL 使用時に）キューにバッファが設定されている場合
	bool queueing = false;
	#if (USE_API == API_OAL)
	{
		queueing = _player->isQueueing();
	}
	#endif
	if((_stat == STAT_PLAYING) || (_stat == STAT_TO_PLAY) || queueing)
	{
		stopNow();
	}

	// 無音再生の場合
	if(id == 0)
	{
		_id = 0;
		return true;
	}

	// 値のリセット
	_id = 0;
	
	#if (USE_API == API_OAL) || (USE_API == API_OSL)
	{
		if(!_player->play(wdef, cntLoop))
		{
			return false;
		}
	}
	#elif (USE_API == API_MCI)
	{
		// パスを作成する
		VcString pathWav;
		if(wdef->_pathDir != 0L)	{	pathWav += VcString(wdef->_pathDir) + "/";			}
		if(wdef->_filename != 0L)	{	pathWav += VcString(wdef->_filename);				}
		if(wdef->_type != 0L)		{	pathWav += VcString(".") + VcString(wdef->_type);	}

		 // MCI による再生
		VcString strCmd = VcString::format("open %s alias tfw_music", pathWav.getRaw());
		#if defined(_UNICODE)
			Utf8Txt utf8txt;
			UnicodeTxt unitxt;
			utf8txt.setSingleString(&strCmd);
			unitxt.copy(&utf8txt);
			MCIERROR ret = ::mciSendString(unitxt.getRawWideString()->getRaw(), 0L, 0, 0L);
		#else
			MCIERROR ret = ::mciSendString(strCmd, 0L, 0, 0L);
		#endif
		if(ret != 0)
		{
			TCHAR buf[256];
			::mciGetErrorString(ret, buf, sizeof(buf));
			TRACE(buf);
			return false;
		}
		ret = ::mciSendString(_T("play tfw_music"), 0L, 0, 0L);
		if(ret != 0)
		{
			TCHAR buf[256];
			::mciGetErrorString(ret, buf, sizeof(buf));
			TRACE(buf);
			return false;
		}

		_cntLoopRest = cntLoop;
	}
	#elif (USE_API == API_IAV)
	{
		// 再生
		TRACELN("{Music::play}　play : %s", wdef->_filename->getRaw());
		_player->playMusic(*wdef->_filename, *wdef->_type, cntLoop);
	}
	#endif
	
	// 状態の再確認
	_stat = STAT_TO_PLAY;
	checkStatus();
	
	// 設定保存・リセット
	_id = id;

	return true;
}

/*---------------------------------------------------------------------*//**
	停止
**//*---------------------------------------------------------------------*/
bool Music::stopNow()
{
	bool isSucceeded = true;

	#if (USE_API == API_OAL) || (USE_API == API_OSL)
	{
		// 停止
		if((_stat == STAT_PLAYING) || (_stat == STAT_TO_PLAY))
		{
			if(!_player->stop())
			{
				ASSERT(false);
				isSucceeded = false;
			}
		}
	}
	#elif (USE_API == API_MCI)
	{
		// MCI による停止
		MCIERROR ret = ::mciSendString(_T("stop tfw_music"), 0L, 0, 0L);
		if(ret != 0)
		{
			TCHAR buf[256];
			::mciGetErrorString(ret, buf, sizeof(buf));
			TRACE(buf);
			isSucceeded = false;
		}
		ret = ::mciSendString(_T("close tfw_music"), 0L, 0, 0L);
		if(ret != 0)
		{
			TCHAR buf[256];
			::mciGetErrorString(ret, buf, sizeof(buf));
			TRACE(buf);
			isSucceeded = false;
		}
	}
	#elif (USE_API == API_IAV)
	{
		// 停止
		_player->stopMusic();
	}
	#endif

	// 状態の確認
	_stat = STAT_TO_STOP;
	checkStatus();

	return isSucceeded;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS