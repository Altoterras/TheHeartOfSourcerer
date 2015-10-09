/***********************************************************************//**
 *	SoundMngr.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SOUND_SOUND_MNGR_H_
#define _ETK_SOUND_SOUND_MNGR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class ExecCtx;
class Sound;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント
 *
**//*---------------------------------------------------------------------*/
class SoundMngr
{
	//======================================================================
	// 定数
public:
	// 無限回時のループ値
	static const s32	LOOP_INFINITY			= -1;

	// デフォルトのフェードフレーム数
	static const s32	FRAME_FADE_IN_DEFAULT	= 0;
	static const s32	FRAME_FADE_OUT_DEFAULT	= 15;

	// デフォルトの音量
	static const f32	VOLUME_BGM_DEFAULT;
	static const f32	VOLUME_UI_SE_DEFAULT;
	static const f32	VOLUME_3D_SE_DEFAULT;
	static const f32	VOLUME_3D_SE_MIN;
	static const f32	VOLUME_3D_SE_MAX;
	
	//======================================================================
	// クラス
public:
	class BgmDef
	{
	public:
		const s16 _bgmid;
		u32 _fileid;
		const s32 _smplLoopStart;
		const s32 _smplLoopEnd;
		BgmDef(s16 bgmid, u32 fileid, s32 smplLoopStart, s32 smplLoopEnd)
			: _bgmid(bgmid)
			, _fileid(fileid)
			, _smplLoopStart(smplLoopStart)
			, _smplLoopEnd(smplLoopEnd)
		{
		}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 曲の再生がステレオかどうかを得る
	inline bool isStereoMusic() const { return _isStereoMusic;	}
	// 現在再生中の曲 ID を得る
	u16 getCurrentPlayMusicId() const;
	// 予約された曲 ID を得る
	u16 getReservedMusicId() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void playBgm(u16 bgmid, s32 cntLoop, f32 volume, bool isForceRestart, s32 frameFadeOut, s32 frameFadeIn, s32 ctrlkeyBgm);
	inline void playBgm(u16 bgmid, bool isLoop) { playBgm(bgmid, LOOP_INFINITY, VOLUME_BGM_DEFAULT, false, FRAME_FADE_OUT_DEFAULT, FRAME_FADE_IN_DEFAULT, 0); }
	void stopBgm(s32 frameFadeOut);
	inline void stopBgm() { stopBgm(FRAME_FADE_OUT_DEFAULT); }
	void pauseBgm();
	void resumeBgm();
	s32 playUiSe(u16 seid, bool isLoop);
	s32 playSe(u16 seid, bool isLoop, f32 volume, const Vector3F* pos, const Vector3F* vel);
	void stopSe(s32 sech);
	void stopAllSe();

	void setListener(const Vector3F* pos, const Vector3F* dir);
	void setStereoMusic(bool isStereo);
	bool registerBgms(const BgmDef* bgmarr, s32 numBgm);
	bool registerSeIngame();

	void restrictControlBgm(s32 ctrlkeyBgm);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SoundMngr();
	~SoundMngr();
	bool create();
	void destroy();
	void exec(const ExecCtx* ec);

private:
	bool registerMusics();
	bool registerSeUi();
	bool registerSe(u32 fileidRangeStart, u32 fileidRangeEnd);

	//======================================================================
	// 変数
private:
	Sound* _sound;
	bool _isStereoMusic;
	bool _isRegIngameSe;
	s32 _ctrlkeyBgm;
	const BgmDef* _bgmarr;
	s32 _numBgm;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SOUND_SOUND_MNGR_H_
