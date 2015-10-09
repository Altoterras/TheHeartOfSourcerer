/***********************************************************************//**
 *	OpenAl.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/10/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_OPEN_AL_H_
#define _TFW_SOUND_OPEN_AL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#if defined(_ANDROID)
#include <AL/al.h>
#include <AL/alc.h>
#else
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_ANDROID)
#define USE_OPENAL		1
#else
#define USE_OPENAL		1
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Wave;

/*---------------------------------------------------------------------*//**
 *	OpenAL モジュール
 *		※ Sound クラスのサブモジュール
 *	
**//*---------------------------------------------------------------------*/
class OpenAl  
{
	//======================================================================
	// 定数
public:
	static const u8 STAT_NULL		= 0;
	static const u8 STAT_INIT		= 1;
	static const u8 STAT_PLAY		= 2;
	static const u8 STAT_PAUSE		= 3;
	static const u8 STAT_STOP		= 4;
	static const u8 STAT_TO_PLAY	= 5;
	static const u8 STAT_TO_PAUSE	= 6;
	static const u8 STAT_TO_STOP	= 7;

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	トラック
	 *	
	**//*-----------------------------------------------------------------*/
	class Track
	{
	public:
		ALuint _srcid;
		u8 _stat;
		Track() : _srcid(0), _stat(STAT_NULL) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のリスナー位置を得る
	inline const Vector3F* getListenerPosition() const { return _posListener; }
#if USE_OPENAL
	// バッファの数を得る
	inline s32 getBufferNum() const { return _numBuf; }
	// トラックの数を得る
	inline s32 getTrackNum() const { return _numTrack; }
	// トラックの状態を得る
	inline u8 getTrackStat(s32 trkidx) const { ASSERT((0 <= trkidx) && (trkidx < _numTrack)); return _track[trkidx]._stat; }
#else
	inline s32 getBufferNum() const { return 0; }
	inline s32 getTrackNum() const { return 0; }
	inline u8 getTrackStat(s32 trkidx) const { return STAT_NULL; }
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setBufferData(s32 bufidx, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
	bool setBufferData(s32 bufidx, Wave* wav);
	bool setBufferData(s32 bufidx, Wave* wav, s32 sampleStartLoopPoint, s32 sampleEndLoopPoint);
	bool clearBufferData(s32 bufidx);
	bool play(s32 trkidx);
	bool stop(s32 trkidx);
	bool pause(s32 trkidx);
	bool resume(s32 trkidx);
	bool queueBuffers(s32 trkidx, s32 bufidx);
	bool queueBuffers(s32 trkidx, s32* idxarrBuf, s32 numArr);
	bool unqueueBuffers(s32 trkidx, s32 bufidx);
	bool unqueueBuffers(s32 trkidx, s32* idxarrBuf, s32 numArr);
	bool clearQueueBuffers(s32 trkidx);
	bool setAutoLoop(s32 trkidx, bool isLoop);
	s32 getSampleOffset(s32 trkidx);

	void setListenerPosition(const Vector3F* pos);
	void setListenerDirection(const Vector3F* dir);
	void setTrackPosition(s32 trkidx, const Vector3F* pos);
	void setTrackVelocity(s32 trkidx, const Vector3F* vel);

	void setTrackGain(s32 trkidx, f32 gain);
	void setTrackMinGain(s32 trkidx, f32 gain);
	void setTrackMaxGain(s32 trkidx, f32 gain);
	void setTrackReferenceDistance(s32 trkidx, f32 dist);
	void setTrackMaxDistance(s32 trkidx, f32 dist);
	void setTrackForUi(s32 trkidx);
	void setTrackForBgm(s32 trkidx);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OpenAl();
	virtual ~OpenAl();
	bool create(s32 numBuf, s32 numTrack);
	void destroy();
	void exec();

private:

	//======================================================================
	// 変数
private:
	Vector3F* _posListener;
#if USE_OPENAL
	ALCdevice* _device;
	ALCcontext* _context;
	ALuint* _bufid;
	s32 _numBuf;
	Track* _track;
	s32 _numTrack;
#endif	// USE_OPENAL
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_OPEN_AL_H_
