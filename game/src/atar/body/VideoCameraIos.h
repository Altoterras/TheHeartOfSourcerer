/***********************************************************************//**
 *	VideoCameraIos.h
 *	Terras Frameork
 *	
 *	Created by Ryoutarou Kishi on 2013/06/07.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_VIDEO_CAMERA_IOS_H_
#define _ATAR_BODY_VIDEO_CAMERA_IOS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ATAR_BEGIN_NS

class VideoCameraIosInner;

/*---------------------------------------------------------------------*//**
 * ビデオカメラ（iOS）
 *
**//*---------------------------------------------------------------------*/
class VideoCameraIos
{
	//======================================================================
	// 定数
public:
	// ビデオ解像度
	enum VideoResolution
	{
		VRESO_NULL,
		VRESO_192x144,
		VRESO_352x288,
		VRESO_640x480,
	};
	
	// ビットマップフォーマット
	enum BitmapFormat
	{
		BMPFORM_NULL,
		BMPFORM_BGRA,
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void startCapture();
	void stopCapture();
	bool isCapturing();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	VideoCameraIos();
	~VideoCameraIos();
	bool create(VideoResolution resolution);
	void destroy();
	void setBitmapFromDelegater(s32 width, s32 height, s32 bytesPerRow, u8* bits);

	//======================================================================
	// 変数
private:
	u8* _bits;
	s32 _width;
	s32 _height;
	VideoCameraIosInner* _inner;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_VIDEO_CAMERA_IOS_H_