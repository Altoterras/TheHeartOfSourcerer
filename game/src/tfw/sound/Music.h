/***********************************************************************//**
 *	Music.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/10/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_MUSIC_H_
#define _TFW_SOUND_MUSIC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "WaveFormat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class ExecCtx;
class Sound;
class WaveDef;

#if defined(_IOS)
class IosAudio;
#elif defined(_ANDROID)
class OpenSlMusic;
#else
class OpenAlMusic;
#endif

/*---------------------------------------------------------------------*//**
 *	曲再生モジュール（主に BGM として利用される）
 *		※ Sound クラスのサブモジュール
 *	
**//*---------------------------------------------------------------------*/
class Music  
{
	//======================================================================
	// 定数
public:
	static const s32	LOOP_INFINITY			= -1;	// 無限回時のループ値

private:
	static const u8		STAT_NULL				= 0;
	static const u8		STAT_STOPPED			= 1;	// 停止中
	static const u8		STAT_PLAYING			= 2;	// 再生中
	static const u8		STAT_TO_STOP			= 3;	// 停止へ
	static const u8		STAT_TO_PLAY			= 4;	// 再生へ

	static const s32	BUFIDXARR_EL_NUM		= 3;	// OpenAL に渡すバッファインデックス配列の要素数

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// プレイ中かどうかを得る
	bool isPlaying() const;
	// 現在の曲 ID を得る
	inline u16 getCurrentMusicId() const { return _id; }
	// 予約された曲 ID を得る
	inline u16 getReservedMusicId() const { return _idReserved; }
	// 登録数を得る
	s32 getRegisterMusicNum() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void play(u16 id, f32 volume, s32 cntLoop, bool isForceRestart, s32 frameFadeOut, s32 frameFadeIn);
	void stop(s32 frameFadeOut);
	void pause();
	void resume();

	bool registerMusic(u16 id, const CStringBase* pathDir, const CStringBase* filename, const CStringBase* type, s32 smplLoopStart, s32 smplLoopEnd);
	bool registerMusic(u16 id, u32 fileid, u8 wfmt, s32 smplLoopStart, s32 smplLoopEnd);
	bool unregisterMusic(u16 id);

	void setVolume(f32 volume);
	void notifyChangeListenerPosition();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Music();
	virtual ~Music();
	bool create(Sound* sndRef);
	void destroy();
	void exec(const ExecCtx* ec);

private:
	WaveDef* getWaveDef(u16 id);
	void checkStatus();
	void setFadeIn(f32 fcntMax, f32 volume);
	void setFadeOut(f32 fcntMax);

	bool playNowReserved();
	bool playNow(u16 id, s32 cntLoop);
	bool stopNow();

	//======================================================================
	// メンバ変数

private:
	List<WaveDef*>* _listWave;
#if defined(_IOS)
	IosAudio* _player;
#elif defined(_ANDROID)
	OpenSlMusic* _player;
#else
	OpenAlMusic* _player;
#endif

	u8 _stat;							// 状態（OpenAl::STAT_*）
	u16 _id;							// 曲 ID
	f32 _volume;						// ボリューム
	s32 _cntLoopRest;					// ループ残回数

	u16	_idReserved;					// 予約された曲 ID
	s32 _cntLoopMaxReserved;			// 予約されたループ回数（-1 は無限）

	f32 _fcntFadeOutCur;				// フェードアウトフレームカウンタ
	f32 _fcntFadeOutMax;				// フェードアウトフレームカウンタ最大値
	f32 _volumeFadeOutStart;			// フェードアウト開始ボリューム
	f32 _fcntFadeInCur;					// フェードインフレームカウンタ
	f32 _fcntFadeInMax;					// フェードインフレームカウンタ最大値
	f32 _volumeFadeInEnd;				// フェードイン終了ボリューム

	Sound* _sndRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_MUSIC_H_
