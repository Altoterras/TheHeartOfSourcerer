/***********************************************************************//**
 *  OpenAlMusic.cpp
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
#include "OpenAlMusic.h"

// Friends
#include "CafWav.h"
#include "OpenAl.h"
#include "RiffWav.h"
#include "Sound.h"
#include "WaveDef.h"
#include "WaveFormat.h"
#include "../string/VcString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define GAIN_DEFAULT				(1.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OpenAlMusic メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	再生
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::play(WaveDef* wdef, s32 cntLoop)
{
	// 値のリセット
	_cntLoop = 0;
	_cntLoopMax = 0;
	_smpofsNextLoopQue = 0;
	_smpofsLoopStart = 0;
	_smpofsLoopEnd = 0;
	_numArrBufIdx = 0;
	_bufidxLoop = -1;
	
	// Wave を読み込む
	Wave* wav = 0L;
	if(wdef->_fileid != 0)	// ファイル ID で指定
	{
		if(wdef->_wfmt == WaveFormat::RIFF)
		{
			wav = new RiffWav();
		}
		else if(wdef->_wfmt == WaveFormat::CAF)
		{
			wav = new CafWav();
		}
		if(wav == 0L)
		{
			ASSERT(false);
			return false;
		}
		if(!wav->create(wdef->_fileid, 0))
		{
			delete wav;
			ASSERT(false);
			return false;
		}
	}
	else
	{
		// 波形ファイルのパスを作成する
		VcString pathWav;
		if(wdef->_pathDir != 0L)	{	pathWav += VcString(wdef->_pathDir) + "/";			}
		if(wdef->_filename != 0L)	{	pathWav += VcString(wdef->_filename);				}
		if(wdef->_type != 0L)		{	pathWav += VcString(".") + VcString(wdef->_type);	}

		// RiffWav を読み込む
		wav = new RiffWav();
		if(!wav->create(&pathWav, 0))
		{
			delete wav;
			ASSERT(false);
			return false;
		}
	}

	bool isSucceeded = false;
	do
	{
		TRACE("{OpenAlMusic::play} Frequency=%d, Bits=%d\n", wav->getFrequency(), wav->getBitsPerSample());

		// OpenAL バッファに設定
		s32 smplWavEnd = wav->getMaxSampleOffset();
		s32 cntBufArr = 0;
		s32 cntBufIdx = Sound::OAL_BUFIDX_MUSIC_START;
		// イントロ
		if(wdef->_smplLoopStart > 0)
		{
			if(!_alRef->setBufferData(cntBufIdx, wav, 0, wdef->_smplLoopStart))
			{
				ASSERT(false);
				break;
			}
			_arrbufidx[cntBufArr++] = (cntBufIdx++);
		}
		// ループ区間
		{
			if(!_alRef->setBufferData(cntBufIdx, wav, wdef->_smplLoopStart, wdef->_smplLoopEnd))
			{
				ASSERT(false);
				break;
			}
			_bufidxLoop = ((cntLoop > 0) || (cntLoop == -1)) ? cntBufIdx : -1;	// ループする場合のバッファインデックス
			_arrbufidx[cntBufArr++] = (cntBufIdx++);
		}
		// エンディング
		if(wdef->_smplLoopEnd < smplWavEnd)
		{
			if(!_alRef->setBufferData(cntBufIdx, wav, wdef->_smplLoopEnd, smplWavEnd))
			{
				ASSERT(false);
				break;
			}
			_arrbufidx[cntBufArr++] = (cntBufIdx++);
		}
		ASSERT(cntBufIdx <= Sound::OAL_BUFIDX_MUSIC_END);

		// ループ設定
		_smpofsNextLoopQue = ((cntLoop > 0) || (cntLoop == -1)) ? wdef->_smplLoopStart : -1;	// 最初のキュー追加位置の設定
		_smpofsLoopStart = wdef->_smplLoopStart;
		_smpofsLoopEnd = wdef->_smplLoopEnd == S32_MAX ? wav->getMaxSampleOffset() : wdef->_smplLoopEnd;

		// キューにバッファを追加する
		if(!_alRef->queueBuffers(_trackOal, _arrbufidx, cntBufArr))
		{
			ASSERT(false);
			break;
		}
		_numArrBufIdx = cntBufArr;

		// 再生
		if(!_alRef->play(_trackOal))
		{
			ASSERT(false);
			break;
		}

		isSucceeded = true;
	}
	while(false);

	wav->destroy();
	delete wav;
	if(!isSucceeded)	{	return false;	}
	
	// 設定保存・リセット
	_cntLoopMax = cntLoop;

	return true;
}

/*---------------------------------------------------------------------*//**
	停止
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::stop()
{
	bool isSucceeded = true;

	// 停止
	if(!_alRef->stop(_trackOal))
	{
		ASSERT(false);
		isSucceeded = false;
	}

	// キューからバッファを全削除する
	if(_numArrBufIdx != 0)
	{
		if(!_alRef->clearQueueBuffers(_trackOal))
		{
			ASSERT(false);
			isSucceeded = false;
		}
		_numArrBufIdx = 0;	// 削除済み
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	一時停止
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::pause()
{
	// 停止
	if(!_alRef->pause(_trackOal))
	{
		return true;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	再開
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::resume()
{
	if(!isPausing())
	{
		return false;
	}

	// 再生
	if(!_alRef->resume(_trackOal))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	再生中かを得る
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::isPlaying()
{
	// OpenAL の状態を見て判断する
	u8 stat = _alRef->getTrackStat(_trackOal);
	if(stat == OpenAl::STAT_PLAY)
	{
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	一時停止中かを得る
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::isPausing()
{
	// OpenAL の状態を見て判断する
	u8 stat = _alRef->getTrackStat(_trackOal);
	if(stat == OpenAl::STAT_PAUSE)
	{
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	ボリュームを設定する
**//*---------------------------------------------------------------------*/
void OpenAlMusic::setVolume(f32 volume)
{
	_alRef->setTrackMinGain(_trackOal, volume);
	_alRef->setTrackMaxGain(_trackOal, volume);
	_alRef->setTrackGain(_trackOal, volume);
}

/*---------------------------------------------------------------------*//**
	リスナー位置変更通知
	※OpenAL のリスナー位置が変更したときに任意で呼び出す
	　（フレームワークが自動的には呼び出さないので注意）
**//*---------------------------------------------------------------------*/
void OpenAlMusic::notifyChangeListenerPosition()
{
	_alRef->setTrackPosition(_trackOal, _alRef->getListenerPosition());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OpenAlMusic::OpenAlMusic()
	: _alRef(0L)
	, _cntLoopMax(0)
	, _cntLoop(0)
	, _smpofsNextLoopQue(-1)
	, _smpofsLoopStart(0)
	, _smpofsLoopEnd(0)
	, _trackOal(Sound::OAL_TRACK_MUSIC_START)
	, _numArrBufIdx(0)
	, _bufidxLoop(-1)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
OpenAlMusic::~OpenAlMusic()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool OpenAlMusic::create(OpenAl* alRef)
{
	// 参照を保存
	_alRef = alRef;

	// 専用トラック設定
	_alRef->setTrackForBgm(_trackOal);
	_alRef->setTrackMinGain(_trackOal, GAIN_DEFAULT);
	_alRef->setTrackMaxGain(_trackOal, GAIN_DEFAULT);
	_alRef->setTrackGain(_trackOal, GAIN_DEFAULT);
	_alRef->setTrackMaxDistance(_trackOal, 0.0f);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void OpenAlMusic::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void OpenAlMusic::exec(const ExecCtx* ec)
{
	// OpenAL のバッファループ処理
	if(_bufidxLoop >= 0)	// ループがある
	{
		s32 smpofs = _alRef->getSampleOffset(_trackOal);
		if((smpofs >= _smpofsNextLoopQue) && (_smpofsNextLoopQue >= 0))	// 次のキュー位置まで再生した
		{
			s32 cntLoopNext = _cntLoop + 1;
			if((cntLoopNext > _cntLoopMax) && (_cntLoopMax > 0))	// 有限回ループが最大値まで達した
			{
				// ループ終了
				_smpofsNextLoopQue = -1;
				_bufidxLoop = -1;
			}
			else
			{
				TRACE("{OpenAlMusic::exec} next que : current_sample_offset=%d, next_sample_offset=%d, num=%d\n", smpofs, _smpofsNextLoopQue, _cntLoop);
				// ループ処理
				_cntLoop = cntLoopNext;
				_alRef->queueBuffers(_trackOal, _bufidxLoop);	// 次のバッファをキューに入れる
				_smpofsNextLoopQue += (_smpofsLoopEnd - _smpofsLoopStart);	// 次に追加するタイミング
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS