/***********************************************************************//**
 *	MovieFileTextureIos.h
 *	Terras Frameork
 *	
 *	Created by Ryoutarou Kishi on 2013/06/07.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_MOVIE_FILE_TEXTURE_IOS_H_
#define _ATAR_BODY_MOVIE_FILE_TEXTURE_IOS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

#if defined(__OBJC__)
	@class AVPlayer;
	#define PTR_MOVIE_FILE_TEXTURE_IOS_AV_PLAYER AVPlayer*
#else
	#define PTR_MOVIE_FILE_TEXTURE_IOS_AV_PLAYER void*
#endif

ATAR_BEGIN_NS

class MovieFileTextureIosInner;

/*---------------------------------------------------------------------*//**
 * ビデオカメラ（iOS）
 *
**//*---------------------------------------------------------------------*/
class MovieFileTextureIos
{
	//======================================================================
	// 定数
private:
	// ビットマップフォーマット
	enum BitmapFormat
	{
		BMPFORM_NULL,
		BMPFORM_BGRA,
	};
	
	// 再生状態
	enum PlayingState
	{
		PLAYSTAT_NULL,
		PLAYSTAT_PREPARING,
		PLAYSTAT_PREPARED,
		PLAYSTAT_PLAYING,
		PLAYSTAT_PAUSING,
		PLAYSTAT_FINISHED,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	u8* getBitmapBits() const { return _bits; }
	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	BitmapFormat getFormat() const { return BMPFORM_BGRA; }	// 現在固定
	PTR_MOVIE_FILE_TEXTURE_IOS_AV_PLAYER getPlayer() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool isPlaying() const;
	bool isFinished() const;
	bool isPrepared() const;
	bool prepareFile(CStringBase* pathFile);
	bool play();
	void pause();
	u8* generateMovieBitmap();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MovieFileTextureIos();
	~MovieFileTextureIos();
	bool create(s32 widthTexture, s32 heightTexture);
	void destroy();
	bool notifyChangedValueForKeyPath(void* context, void* object, void* key);
	void notifyPlayerItemDidReachEnd();
	
	//======================================================================
	// 変数
private:
	u8* _bits;
	s32 _width;
	s32 _height;
	PlayingState _playstat;
	MovieFileTextureIosInner* _inner;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_MOVIE_FILE_TEXTURE_IOS_H_