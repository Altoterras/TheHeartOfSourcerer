/***********************************************************************//**
 *  Se.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/04/12.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Se.h"

// Friends
#include "OpenAl.h"
#include "Sound.h"
#include "CafWav.h"
#include "RiffWav.h"
#include "../collection/List.h"
#include "../lib/Vector.h"
#include "../string/VcString.h"

// External

// Library
#if defined(_WIDNOWS)
	#include <Mmsystem.h>
#endif

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define IS_WIN_USE_MCI		0

////////////////////////////////////////////////////////////////////////////
// 定数

#define CH_UI						(0)

#define GAIN_UI_DEFAULT				(1.0f)
#define DIST_3D_REF_DEFAULT			(1.0f)
#define DIST_3D_MAX_DEFAULT			(1000.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Se メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	UI 用 SE 再生
**//*---------------------------------------------------------------------*/
s32 Se::playUi(u16 id, bool isLoop)
{
	return play(id, CH_UI, isLoop, -1.0f, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	3D 空間用 SE 再生
**//*---------------------------------------------------------------------*/
s32 Se::play(u16 id, bool isLoop, f32 volume, const Vector3F* pos, const Vector3F* vel)
{
	return play(id, -1, isLoop, volume, pos, vel);
}

/*---------------------------------------------------------------------*//**
	SE 再生
**//*---------------------------------------------------------------------*/
s32 Se::play(u16 id, s32 ich, bool isLoop, f32 volume, const Vector3F* pos, const Vector3F* vel)
{
	// チャンネルを得る
	Channel* ch = 0L;
	if(ich >= 0)
	{
		ASSERT(ich < _numCh);
		ch = &_arrchCh[ich];

		// 既に同フレーム中にプレイを実行した場合は失敗
		if(TFW_IS_FLAG(ch->_flags, Channel::F_PFRAME))
		{
			return -1;
		}

		// 強制停止
		stop(ich);
	}
	else
	{
		// 空きチャンネルを得る
		ich = -1;
		for(int ichWk = CH_UI + 1; ichWk < _numCh; ichWk++)	// 0 チャンネルは UI 用に固定
		{
			ch = &_arrchCh[ichWk];
			if((ch->_stat == OpenAl::STAT_INIT) || (ch->_stat == OpenAl::STAT_STOP))
			{
				ich = ichWk;
				break;
			}
		}
		if(ich == -1)
		{
			TRACE("{Se::play} channel is full !\n");
			return -1;
		}
	}

	// 音定義を得る
	WaveDef* wdef = getWaveDef(id);
	if(wdef == 0L)
	{
		//ASSERT(false);
		TRACE("{Se::play} no define se : %d\n", id);
		return -1;
	}

	// 波形が未定義で再生時に読み込む場合はここで読み込む
	if(_isLoadOnPlay && (wdef->_bufidx == -1))
	{
		if(!loadWave(wdef))
		{
			return -1;
		}
	}

	// 自動ループ設定
	if(!_sndRef->getOpenAl()->setAutoLoop(ch->_track, isLoop))
	{
		ASSERT(false);
		return -1;
	}

	// 音量の設定
	if(volume > 0.0f)
	{
		_sndRef->getOpenAl()->setTrackGain(ch->_track, volume);
	}
	else if(volume == 0.0f)
	{
		return -1;
	}

	// 位置・速度の設定
	if(pos != 0L)
	{
		_sndRef->getOpenAl()->setTrackPosition(ch->_track, pos);
	}
	else
	{
		_sndRef->getOpenAl()->setTrackPosition(ch->_track, _sndRef->getOpenAl()->getListenerPosition());	// OpenAL リスナーの位置で鳴らす
	}
	if(vel != 0L)
	{
		_sndRef->getOpenAl()->setTrackVelocity(ch->_track, vel);
	}

	// キューにバッファを追加する
	if(!_sndRef->getOpenAl()->queueBuffers(ch->_track, wdef->_bufidx))
	{
		ASSERT(false);
		return -1;
	}

	// 再生
	if(!_sndRef->getOpenAl()->play(ch->_track))
	{
		ASSERT(false);
		return -1;
	}

	// チャンネルに情報を保存する
	ch->_id = id;
	ch->_bufidx = wdef->_bufidx;
	ch->_stat = OpenAl::STAT_TO_PLAY;
	TFW_SET_FLAG(ch->_flags, Channel::F_LOOP, isLoop);
	TFW_SET_FLAG(ch->_flags, Channel::F_PFRAME, true);

	return ich;
}

/*---------------------------------------------------------------------*//**
	SE 停止
**//*---------------------------------------------------------------------*/
bool Se::stop(s32 ich)
{
	bool isSucceeded = true;

	// 停止
	ASSERT((0 <= ich) && (ich < _numCh));
	Channel* ch = &_arrchCh[ich];
	if((ch->_stat == OpenAl::STAT_PLAY) || (ch->_stat == OpenAl::STAT_PAUSE))
	{
		if(!_sndRef->getOpenAl()->stop(ch->_track))
		{
			ASSERT(false);
			isSucceeded = false;
		}
	}

	// キューからバッファを削除する
	if(ch->_bufidx >= Sound::OAL_BUFIDX_SE_START)
	{
		if(!_sndRef->getOpenAl()->unqueueBuffers(ch->_track, ch->_bufidx))
		{
			ASSERT(false);
			isSucceeded = false;
		}
	}

	// チャンネル情報をクリア
	ch->_id = 0;
	ch->_bufidx = 0;
	ch->_stat = OpenAl::STAT_TO_STOP;
	TFW_SET_FLAG(ch->_flags, Channel::F_LOOP, false);

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	SE 全停止
**//*---------------------------------------------------------------------*/
void Se::stopAll()
{
	for(s32 ich = 0; ich < _numCh; ich++)
	{
		Channel* ch = &_arrchCh[ich];
		if((ch->_stat == OpenAl::STAT_PLAY) || (ch->_stat == OpenAl::STAT_PAUSE))
		{
			stop(ich);
		}
	}
}
	
/*---------------------------------------------------------------------*//**
	SE を登録する
**//*---------------------------------------------------------------------*/
bool Se::registerSe(u16 id, u32 fileid, u8 wfmt)
{
	// SE 定義の空きを得る
	WaveDef* wdef = 0L;
	for(int iwd = 0; iwd < _numWaveDef; iwd++)
	{
		if(_arrwdWave[iwd]._id == 0)
		{
			wdef = &_arrwdWave[iwd];
			break;
		}
	}
	if(wdef == 0L)
	{
		ASSERTM(false, "{Se::registerMusic} wave-def is full !\n");
		return false;
	}
	
	// SE 定義に情報を保存する
	wdef->_id = id;
	wdef->_fileid = fileid;
	wdef->_wfmt = wfmt;

	// 登録時に先読みする場合は波形を読み込む
	if(!_isLoadOnPlay)
	{
		if(!loadWave(wdef))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	SE の登録を解除する
**//*---------------------------------------------------------------------*/
bool Se::unregisterSe(u16 id)
{
	// SE 定義を得る
	WaveDef* wdef = getWaveDef(id);
	if(wdef == 0L)
	{
		VcString str;
		ASSERTM(false, VcString::format(&str, "{Se::unregisterMusic} id=%d is no exist !\n", id));
		return false;
	}

	// OpenAL バッファをクリアする
	if(!_sndRef->getOpenAl()->clearBufferData(wdef->_bufidx))
	{
		ASSERT(false);
		return false;
	}
	
	// SE 定義の情報をクリア
	wdef->_id = 0;
	wdef->_bufidx = 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	UI 用 SE のボリュームを設定する
**//*---------------------------------------------------------------------*/
void Se::setUiVolume(f32 volume)
{
	if(_volumeUi == volume)	{	return;	}

	_volumeUi = volume;

	Channel* chUi = &_arrchCh[CH_UI];
	_sndRef->getOpenAl()->setTrackMinGain(chUi->_track, _volumeUi);
	_sndRef->getOpenAl()->setTrackMaxGain(chUi->_track, _volumeUi);
	_sndRef->getOpenAl()->setTrackGain(chUi->_track, _volumeUi);
}

/*---------------------------------------------------------------------*//**
	3D 空間用 SE の基準距離を設定する
**//*---------------------------------------------------------------------*/
void Se::setStandardDistance(f32 distRefer, f32 distMax)
{
	if((_distRefer == distRefer) && (_distMax == distMax))	{	return;	}

	_distRefer = distRefer;
	_distMax = distMax;

	for(int ich = CH_UI + 1; ich < _numCh; ich++)
	{
		Channel* ch = &_arrchCh[ich];
		_sndRef->getOpenAl()->setTrackReferenceDistance(ch->_track, _distRefer);
		_sndRef->getOpenAl()->setTrackMaxDistance(ch->_track, _distMax);
	}
}

/*---------------------------------------------------------------------*//**
	3D 空間用 SE のボリュームを設定する
**//*---------------------------------------------------------------------*/
void Se::setVolume(f32 volume, f32 volumeMin, f32 volumeMax)
{
	for(int ich = CH_UI + 1; ich < _numCh; ich++)
	{
		Channel* ch = &_arrchCh[ich];
		_sndRef->getOpenAl()->setTrackGain(ch->_track, volume);
		_sndRef->getOpenAl()->setTrackMinGain(ch->_track, volumeMin);
		_sndRef->getOpenAl()->setTrackMaxGain(ch->_track, volumeMax);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Se::Se()
	: _arrwdWave(0L)
	, _numWaveDef(0)
	, _arrchCh(0L)
	, _numCh(0)
	, _cntBufIdx(0)
	, _isLoadOnPlay(false)
	, _volumeUi(GAIN_UI_DEFAULT)
	, _distRefer(DIST_3D_REF_DEFAULT)
	, _distMax(DIST_3D_MAX_DEFAULT)
	, _sndRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Se::~Se()
{
	ASSERT(_arrwdWave == 0L);
	ASSERT(_arrchCh == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Se::create(Sound* sndRef, s32 numSePolyphony, s32 numMaxSeDefine, bool isLoadOnPlay)
{
	// 参照を保存
	_sndRef = sndRef;

	// 読み込み設定を保存
	_isLoadOnPlay = isLoadOnPlay;

	// 音定義配列を作成
	_numWaveDef = numMaxSeDefine;
	_arrwdWave = new WaveDef[_numWaveDef];

	// チャンネル配列を作成
	_numCh = numSePolyphony;
	_arrchCh = new Channel[_numCh];
	for(int ich = 0; ich < _numCh; ich++)
	{
		_arrchCh[ich]._track = Sound::OAL_TRACK_SE_START + ich;
	}

	// OpenAL バッファインデックスカウンタの初期化
	_cntBufIdx = Sound::OAL_BUFIDX_SE_START;

	// UI 用チャンネル設定
	Channel* chUi = &_arrchCh[CH_UI];
	_sndRef->getOpenAl()->setTrackForUi(chUi->_track);
	_sndRef->getOpenAl()->setTrackMinGain(chUi->_track, _volumeUi);
	_sndRef->getOpenAl()->setTrackMaxGain(chUi->_track, _volumeUi);
	_sndRef->getOpenAl()->setTrackGain(chUi->_track, _volumeUi);
	// 3D 空間用チャンネル設定
	for(int ich = CH_UI + 1; ich < _numCh; ich++)
	{
		Channel* ch = &_arrchCh[ich];
		_sndRef->getOpenAl()->setTrackReferenceDistance(ch->_track, _distRefer);
		_sndRef->getOpenAl()->setTrackMaxDistance(ch->_track, _distMax);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Se::destroy()
{
	// チャンネル配列を削除
	if(_arrchCh != 0L)
	{
		delete[] _arrchCh;
		_arrchCh = 0L;
		_numCh = 0;
	}

	// 音定義配列を削除
	if(_arrwdWave != 0L)
	{
		delete[] _arrwdWave;
		_arrwdWave = 0L;
		_numWaveDef = 0;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Se::exec()
{
	// チャンネルのフレーム制御
	for(s32 ich = 0; ich < _numCh; ich++)
	{
		Channel* ch = &_arrchCh[ich];
		// プレイフラグをおろす
		if(TFW_IS_FLAG(ch->_flags, Channel::F_PFRAME))
		{
			TFW_SET_FLAG(ch->_flags, Channel::F_PFRAME, false);
		}
	}

	checkStatus();
}

/*---------------------------------------------------------------------*//**
	波形を読み込む
**//*---------------------------------------------------------------------*/
bool Se::loadWave(WaveDef* wdef)
{
	if(wdef->_wfmt == WaveFormat::RIFF)
	{
		// RiffWav を読み込む
		RiffWav wav;
		if(!wav.create(wdef->_fileid, 0))
		{
			ASSERT(false);
			return false;
		}

		// OpenAL バッファに設定する
		if(!_sndRef->getOpenAl()->setBufferData(_cntBufIdx, &wav))
		{
			ASSERT(false);
			wav.destroy();
			return false;
		}

		wav.destroy();
	}
	else if(wdef->_wfmt == WaveFormat::CAF)
	{
		// CafWav を読み込む
		CafWav wav;
		if(!wav.create(wdef->_fileid, 0))
		{
			ASSERT(false);
			return false;
		}

		// OpenAL バッファに設定する
		if(!_sndRef->getOpenAl()->setBufferData(_cntBufIdx, &wav))
		{
			ASSERT(false);
			wav.destroy();
			return false;
		}

		wav.destroy();
	}
	
	// SE 定義にバッファインデックスを保存する
	wdef->_bufidx = _cntBufIdx;
	_cntBufIdx++;	// OpenAL バッファインデックスカウンタを進める

	return true;
}

/*---------------------------------------------------------------------*//**
	音定義を得る
**//*---------------------------------------------------------------------*/
Se::WaveDef* Se::getWaveDef(u16 id)
{
	for(int iwd = 0; iwd < _numWaveDef; iwd++)
	{
		if(_arrwdWave[iwd]._id == id)
		{
			return &_arrwdWave[iwd];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	現在の状態を得る
**//*---------------------------------------------------------------------*/
void Se::checkStatus()
{
	for(int ich = 0; ich < _numCh; ich++)
	{
		u8 stat = _sndRef->getOpenAl()->getTrackStat(_arrchCh[ich]._track);
		if(stat == OpenAl::STAT_STOP)
		{
			if((_arrchCh[ich]._stat == OpenAl::STAT_PLAY) || (_arrchCh[ich]._stat == OpenAl::STAT_PAUSE))
			{
				// 自動的に止まった
				stop(ich);
			}
		}
		_arrchCh[ich]._stat = stat;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS