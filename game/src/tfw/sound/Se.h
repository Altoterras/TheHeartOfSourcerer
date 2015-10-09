/***********************************************************************//**
 *	Se.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/04/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_SE_H_
#define _TFW_SOUND_SE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "WaveFormat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Sound;

/*---------------------------------------------------------------------*//**
 *	SE 再生モジュール
 *		※ Sound クラスのサブモジュール
 *	
**//*---------------------------------------------------------------------*/
class Se  
{
private:
	static const s32	BUFIDXARR_EL_NUM		= 3;	// OpenAL に渡すバッファインデックス配列の要素数

	//======================================================================
	// クラス

public:
	/*-----------------------------------------------------------------*//**
	 *	SE 定義
	 *	
	**//*-----------------------------------------------------------------*/
	class WaveDef
	{
	public:
		u16			_id;			// SE ID
		u8			_wfmt;			// ファイルフォーマット
		u32			_fileid;		// ファイル ID
		s32			_bufidx;		// OpenAL バッファインデックス

		WaveDef() : _id(0), _bufidx(-1), _fileid(0), _wfmt(0) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	発音チャンネル
	 *	
	**//*-----------------------------------------------------------------*/
	class Channel
	{
	public:
		static const u8 F_LOOP		= 0x01;	// ループするかどうか
		static const u8 F_PFRAME	= 0x10;	// フレーム中に既にプレイを実行した

		u16			_id;			// SE ID
		u8			_stat;			// 状態（OpenAl::STAT_*）
		u8			_flags;			// フラグ
		s32			_track;			// OpenAL トラックインデックス
		s32			_bufidx;		// OpenAL バッファインデックス

		Channel() : _id(0), _stat(0), _flags(0), _track(0), _bufidx(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 playUi(u16 id, bool isLoop);
	s32 play(u16 id, bool isLoop, f32 volume, const Vector3F* pos, const Vector3F* vel);
	bool stop(s32 ich);
	void stopAll();

	bool registerSe(u16 id, u32 fileid, u8 wfmt);
	bool unregisterSe(u16 id);

	void setUiVolume(f32 volume);
	void setStandardDistance(f32 distRefer, f32 distMax);
	void setVolume(f32 volume, f32 volumeMin, f32 volumeMax);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Se();
	virtual ~Se();
	bool create(Sound* sndRef, s32 numSePolyphony, s32 numMaxSeDefine, bool isLoadOnPlay);
	void destroy();
	void exec();

private:
	bool loadWave(WaveDef* wdef);
	WaveDef* getWaveDef(u16 id);
	void checkStatus();
	s32 play(u16 id, s32 ich, bool isLoop, f32 volume, const Vector3F* pos, const Vector3F* vel);

	//======================================================================
	// メンバ変数

private:
	WaveDef* _arrwdWave;	// SE 定義配列
	s32 _numWaveDef;		// 最大 SE 定義数
	Channel* _arrchCh;		// チャンネル配列
	s32 _numCh;				// チャンネル数
	s32 _cntBufIdx;			// OpenAL バッファインデックスカウンタ
	bool _isLoadOnPlay;		// 再生時に読み込む

	f32 _volumeUi;			// UI 用 SE のボリューム
	f32 _distRefer;			// 3D 空間用 SE の基準距離
	f32 _distMax;			// 3D 空間用 SE の最大距離

	Sound* _sndRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_SE_H_
