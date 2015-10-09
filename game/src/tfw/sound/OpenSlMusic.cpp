/***********************************************************************//**
 *  OpenSlMusic.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2013/01/06.
 *  Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OpenSlMusic.h"

// Friends
#include "OpenSl.h"
#include "WaveDef.h"
#include "../file/File.h"
#if defined(_ANDROID)
#include "../file/FileAndroid.h"
#endif
#include "../string/VcString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OpenSlMusic メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	再生
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::play(WaveDef* wdef, s32 cntLoop)
{
	FTRACE("{OpenSlMusic::play} 000.\n");
	VcString pathWav;
	if(wdef->_filename != 0L)	{	pathWav += VcString(wdef->_filename);				}
	if(wdef->_type != 0L)		{	pathWav += VcString(".") + VcString(wdef->_type);	}
	FTRACE("{OpenSlMusic::play} 001. pathWav=%s\n", pathWav.getRaw());

	// ファイルを開く
	#if defined(_ANDROID)
		_file = new FileAndroid(FileAndroid::FAF_ASSET, &pathWav, File::MODE_READ);
	#else
		_file = new File(&pathWav, File::MODE_READ);
	#endif
	if(!_file->isOpened())	// 開けなかった
	{
		// 失敗
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{OpenSlMusic::play} file-open faild. - fname=%s", pathWav.getRaw()));
		return false;
	}
	FTRACE("{OpenSlMusic::play} 002.\n");

	// トラック作成
	if(!_slRef->createFileTrack(TRCKIDX_MUSIC, _file, 0))
	{
		ASSERTM(false, "{OpenSlMusic::play} createFileTrack faild");
		return false;
	}

	FTRACE("{OpenSlMusic::play} 003.\n");
	// 再生
	if(!_slRef->play(TRCKIDX_MUSIC))
	{
		ASSERTM(false, "{OpenSlMusic::play} play faild");
		return false;
	}

	// ループ設定
	setCurrentLoop(cntLoop > 0);
	
	FTRACE("{OpenSlMusic::play} 999.\n");
	return true;
}

/*---------------------------------------------------------------------*//**
	停止
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::stop()
{
	// 停止
	if(!_slRef->stop(TRCKIDX_MUSIC))
	{
		ASSERTM(false, "{OpenSlMusic::stop}  faild");
		return false;
	}

	// トラック削除
	_slRef->destroyTrack(TRCKIDX_MUSIC);

	// ファイルを閉じる
	if(_file != 0L)
	{
		delete _file;
		_file = 0L;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	一時停止
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::pause()
{
	return _slRef->pause(TRCKIDX_MUSIC);
}

/*---------------------------------------------------------------------*//**
	再開
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::resume()
{
	return _slRef->resume(TRCKIDX_MUSIC);
}

/*---------------------------------------------------------------------*//**
	再生しているかどうか
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::isPlaying()
{
	return _slRef->isPlaying(TRCKIDX_MUSIC);
}

/*---------------------------------------------------------------------*//**
	一時停止しているかどうか
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::isPausing()
{
	return _slRef->isPausing(TRCKIDX_MUSIC);
}

/*---------------------------------------------------------------------*//**
	ボリュームの設定
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::setVolume(f32 volume)
{
	return _slRef->setVolume(TRCKIDX_MUSIC, volume);
}

/*---------------------------------------------------------------------*//**
	現在の再生曲のループ数の設定
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::setCurrentLoop(bool isLoop)
{
	return _slRef->setCurrentLoop(TRCKIDX_MUSIC, isLoop);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OpenSlMusic::OpenSlMusic()
	: _slRef(0L)
	, _file(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
OpenSlMusic::~OpenSlMusic()
{
	ASSERT(_slRef == 0L);
	ASSERT(_file == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool OpenSlMusic::create(OpenSl* slRef)
{
	// 参照を保存
	_slRef = slRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void OpenSlMusic::destroy()
{
	// ファイルを閉じる
	if(_file != 0L)
	{
		delete _file;
		_file = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void OpenSlMusic::exec()
{
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
