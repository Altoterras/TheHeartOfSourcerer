/***********************************************************************//**
 *	OpenAlMusic.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/01/06.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_OPEN_AL_MUSIC_H_
#define _TFW_SOUND_OPEN_AL_MUSIC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;
class OpenAl;
class WaveDef;

/*---------------------------------------------------------------------*//**
 *	OpenAL 曲再生モジュール
 *	
**//*---------------------------------------------------------------------*/
class OpenAlMusic  
{
	//======================================================================
	// 定数
private:
	static const s32	BUFIDXARR_EL_NUM		= 3;	// OpenAL に渡すバッファインデックス配列の要素数

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 曲がキューされているかを得る
	bool isQueueing() const { return _numArrBufIdx != 0; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool play(WaveDef* wdef, s32 cntLoop);
	bool stop();
	bool pause();
	bool resume();
	bool isPlaying();
	bool isPausing();
	void setVolume(f32 volume);
	void notifyChangeListenerPosition();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OpenAlMusic();
	virtual ~OpenAlMusic();
	bool create(OpenAl* alRef);
	void destroy();
	void exec(const ExecCtx* ec);

	//======================================================================
	// メンバ変数

private:
	OpenAl* _alRef;

	s32 _cntLoopMax;					// ループ回数（-1 は無限）
	s32 _cntLoop;						// ループカウンタ
	s32 _smpofsNextLoopQue;				// ループのための次のキューのサンプルオフセット
	s32 _smpofsLoopStart;				// ループ開始サンプルオフセット
	s32 _smpofsLoopEnd;					// ループ終了サンプルオフセット

	s32 _trackOal;						// OpenAL トラックインデックス
	s32 _arrbufidx[BUFIDXARR_EL_NUM];	// OpenAL に渡すバッファインデックス配列
	s32 _numArrBufIdx;					// OpenAL に渡すバッファインデックス配列要素数
	s32 _bufidxLoop;					// OpenAL に渡すループするバッファのインデックス
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_OPEN_AL_MUSIC_H_
