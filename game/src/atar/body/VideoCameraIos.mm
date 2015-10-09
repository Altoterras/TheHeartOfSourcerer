/***********************************************************************//**
 *  VideoCameraIos.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2013/06/07.
 *  Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "VideoCameraIos.h"

// Friends
#include "../Base.h"

// External

// Library
#import <AVFoundation/AVCaptureDevice.h>
#import <AVFoundation/AVCaptureOutput.h>
#import <AVFoundation/AVFoundation.h>

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// VideoCameraIosDelegater クラス

ATAR_BEGIN_NS

class VideoCameraIos;

ATAR_END_NS

/*---------------------------------------------------------------------*//**
 * ビデオカメラデリゲートクラス
 *
**//*---------------------------------------------------------------------*/
@interface VideoCameraIosDelegater : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
	atar::VideoCameraIos* _own;
}

@property atar::VideoCameraIos* own;

@end

@implementation VideoCameraIosDelegater

@synthesize own = _own;

/*---------------------------------------------------------------------*//**
	カメラバッファ取得
**//*---------------------------------------------------------------------*/
- (void)captureOutput:(AVCaptureOutput *)captureOutput
  didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
		 fromConnection:(AVCaptureConnection *)connection
{
	CVPixelBufferRef pixbuff = CMSampleBufferGetImageBuffer(sampleBuffer);
	if(CVPixelBufferLockBaseAddress(pixbuff, 0) == kCVReturnSuccess)
	{
		void* base = CVPixelBufferGetBaseAddress(pixbuff);
		size_t width = CVPixelBufferGetWidth(pixbuff);
		size_t height = CVPixelBufferGetHeight(pixbuff);
		size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixbuff);
		_own->setBitmapFromDelegater((s32)width, (s32)height, (s32)bytesPerRow, (u8*)base);

		CVPixelBufferUnlockBaseAddress(pixbuff, 0);
	}
}

@end	// @implementation VideoCameraIosDelegater

//==========================================================================
// VideoCameraIosInner クラス

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 * Objective-C クラス内包クラス
 *
**//*---------------------------------------------------------------------*/
class VideoCameraIosInner
{
public:
	AVCaptureSession* _session;
	VideoCameraIosDelegater* _delg;
	
	VideoCameraIosInner(VideoCameraIos* own)
		: _session(nil)
		, _delg([[VideoCameraIosDelegater alloc] init])
	{
		_delg.own = own;
	}
	
	VideoCameraIosInner()
	{
		#if !__has_feature(objc_arc)
			[_delg release];
		#endif
		_delg = nil;
	}
};

//==========================================================================
// VideoCameraIos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	キャプチャ開始
**//*---------------------------------------------------------------------*/
void VideoCameraIos::startCapture()
{
	if(_inner->_session == nil)
	{
		return;
	}
	// セッション開始
	[_inner->_session startRunning];
}

/*---------------------------------------------------------------------*//**
	キャプチャ停止
**//*---------------------------------------------------------------------*/
void VideoCameraIos::stopCapture()
{
	if(_inner->_session == nil)
	{
		return;
	}
	// セッション停止
	[_inner->_session stopRunning];
}

/*---------------------------------------------------------------------*//**
	キャプチャ動作中かを得る
**//*---------------------------------------------------------------------*/
bool VideoCameraIos::isCapturing()
{
	if(_inner->_session == nil)
	{
		return false;
	}
	return _inner->_session.running;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
VideoCameraIos::VideoCameraIos()
	: _bits(0L)
	, _width(0)
	, _height(0)
	, _inner(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
VideoCameraIos::~VideoCameraIos()
{
	ASSERT(_inner == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool VideoCameraIos::create(VideoResolution resolution)
{
	// 内包クラスの作成
	_inner = new VideoCameraIosInner(this);

	// カメラデバイス取得
	AVCaptureDevice* deviceCapture = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];

	// 解像度判定
	NSString* strVideoQuality;
	bool retry = false;
	do
	{
		switch(resolution)
		{
			case VRESO_192x144:
				_width = 192;
				_height = 144;
				strVideoQuality = AVCaptureSessionPresetLow;
				break;
			case VRESO_352x288:
				_width = 352;
				_height = 288;
				strVideoQuality = AVCaptureSessionPreset352x288;
				break;
			case VRESO_640x480:
				_width = 640;
				_height = 480;
				strVideoQuality = AVCaptureSessionPreset640x480;
				break;
			default:
				return false;
		}
		// デバイスがその解像度をサポートしているか確認する
		if([deviceCapture supportsAVCaptureSessionPreset:strVideoQuality])
		{
			retry = false;
		}
		else
		{
			// サポートをしていない場合は解像度を下げてリトライする
			switch(resolution)
			{
				case VRESO_192x144:
					return false;
				case VRESO_352x288:
					resolution = VRESO_192x144;
					break;
				case VRESO_640x480:
					resolution = VRESO_192x144;
					break;
			}
			retry = true;
		}
	}
	while (retry);
	s32 bytesPerPixel = 4;	// 現在固定

	// バッファ作成
	_bits = new u8[_width * _height * bytesPerPixel];

	
	// 入力の初期化
	NSError* error = nil;
	AVCaptureDeviceInput* deviceInput = [AVCaptureDeviceInput deviceInputWithDevice:deviceCapture error:&error];

	// 画像への出力を作成
	AVCaptureVideoDataOutput* videoOutput = [[AVCaptureVideoDataOutput alloc] init];
	videoOutput.videoSettings = [NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA] forKey:(NSString *)kCVPixelBufferPixelFormatTypeKey];	// kCVPixelFormatType_32RGBA は対応外
	[videoOutput setAlwaysDiscardsLateVideoFrames:YES];

    // 1秒あたり4回画像をキャプチャ
	AVCaptureConnection* videoConnection = [videoOutput connectionWithMediaType:AVMediaTypeVideo];
	videoConnection.videoMinFrameDuration = CMTimeMake(1, 4);

	// デリゲータのセット
	dispatch_queue_t queue = dispatch_queue_create("jp.co.altoterras.atar.myQueue", NULL);
	[videoOutput setSampleBufferDelegate:_inner->_delg queue:queue];
	dispatch_release(queue);
	
	// セッション初期化
	_inner->_session = [[AVCaptureSession alloc] init];
	[_inner->_session beginConfiguration];
	_inner->_session.sessionPreset = strVideoQuality;
	[_inner->_session commitConfiguration];
	[_inner->_session addInput:deviceInput];
	[_inner->_session addOutput:videoOutput];
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void VideoCameraIos::destroy()
{
	// キャプチャセッションを解放
	if(_inner->_session != nil)
	{
		for (AVCaptureOutput* output in _inner->_session.outputs)
		{
			[_inner->_session removeOutput:output];
		}
		for (AVCaptureInput* input in _inner->_session.inputs)
		{
			[_inner->_session removeInput:input];
		}
		_inner->_session = nil;
	}
	
	// バッファ解放
	delete[] _bits;
	_bits = 0L;

	// 内包クラスの削除
	delete _inner;
	_inner = 0L;
}


/*---------------------------------------------------------------------*//**
	音楽再生
**//*---------------------------------------------------------------------*/
void VideoCameraIos::setBitmapFromDelegater(s32 width, s32 height, s32 bytesPerRow, u8* bits)
{
	if(_bits == 0L)			{	return;	}	// エラー
	if(_width < width)		{	return;	}	// エラー
	if(_height < height)	{	return;	}	// エラー
	memcpy(_bits, bits, bytesPerRow * height);
}

ATAR_END_NS