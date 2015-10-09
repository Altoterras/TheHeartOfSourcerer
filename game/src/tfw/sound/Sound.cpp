/***********************************************************************//**
 *  Sound.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/10/06.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Sound.h"

// Friends
#include "OpenAl.h"
#include "OpenSl.h"
#include "Music.h"
#include "Se.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define IS_NOCROSS(vt_, v1_, v2_)	( ( (( (v2_)->_v[0] - (v1_)->_v[0] ) * ( (vt_)->_v[2] - (v1_)->_v[2] )) - (( (v2_)->_v[2] - (v1_)->_v[2] ) * ( (vt_)->_v[0] - (v1_)->_v[0] )) ) >= 0.0f )

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Sound メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Sound::Sound()
	: _osl(0L)
	, _oal(0L)
	, _music(0L)
	, _se(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Sound::~Sound()
{
	ASSERT(_osl == 0L);
	ASSERT(_oal == 0L);
	ASSERT(_music == 0L);
	ASSERT(_se == 0L);
}

/*---------------------------------------------------------------------*//**
	作成

	@param numSePolyphony	SE の同時発音数
	@param numMaxSeDefine	SE の最大定義数
	@param numWaveBuffer	音ファイルバッファの数
							numMaxSeDefine + 3 あれば安心
							+ 3 は曲再生（Music）モジュールが使う最大数
	@param isSeLoadOnPlay	SE を再生時に読み込むかどうか
**//*---------------------------------------------------------------------*/
bool Sound::create(s32 numSePolyphony, s32 numMaxSeDefine, s32 numWaveBuffer, bool isSeLoadOnPlay)
{
	// OpenSL を作成する
	OpenSl* osl = new OpenSl();
	if((osl == 0L) || (!osl->create(1)))
	{
		delete osl;
		ASSERTM(false, "{Sound::create} OpenSl::create failed.");
		return false;
	}
	_osl = osl;

	// OpenAL を作成する
	OpenAl* oal = new OpenAl();
	if((oal == 0L) || (!oal->create(numWaveBuffer, numSePolyphony + 1)))	// + 1, は曲（Music）再生分
	{
		delete oal;
		ASSERTM(false, "{Sound::create} OpenAl::create failed.");
		return false;
	}
	_oal = oal;

	// 曲再生モジュールを作成する
	Music* music = new Music();
	if(!music->create(this))
	{
		delete music;
		ASSERTM(false, "{Sound::create} Music::create failed.");
		return false;
	}
	_music = music;

	// SE 再生モジュールを作成する
	Se* se = new Se();
	if(!se->create(this, numSePolyphony, numMaxSeDefine, isSeLoadOnPlay))
	{
		delete se;
		ASSERTM(false, "{Sound::create} Se::create failed.");
		return false;
	}
	_se = se;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Sound::destroy()
{
	// SE 再生モジュールを削除する
	if(_se != 0L)
	{
		_se->destroy();
		delete _se;
		_se = 0L;
	}

	// 曲再生モジュールを削除する
	if(_music != 0L)
	{
		_music->destroy();
		delete _music;
		_music = 0;
	}

	// OpenAL を削除する
	if(_oal != 0L)
	{
		_oal->destroy();
		delete _oal;
		_oal = 0L;
	}

	// OpenSL を削除する
	if(_osl != 0L)
	{
		_osl->destroy();
		delete _osl;
		_osl = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Sound::exec(const ExecCtx* ec)
{
	if(_osl != 0L)		{	_osl->exec();		}
	if(_oal != 0L)		{	_oal->exec();		}
	if(_se != 0L)		{	_se->exec();		}
	if(_music != 0L)	{	_music->exec(ec);	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS