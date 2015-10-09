/***********************************************************************//**
 *  MovieFileTextureIos.cpp
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
#include "MovieFileTextureIos.h"

// Friends
#include "../Base.h"

// External

// Library
#import <AVFoundation/AVFoundation.h>

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MovieFileTextureIosDelegater クラス

static void* s_ctxMovieFileTexture_TimedMetadataObserver = &s_ctxMovieFileTexture_TimedMetadataObserver;
static void* s_ctxMovieFileTexture_RateObservation = &s_ctxMovieFileTexture_RateObservation;
static void* s_ctxMovieFileTexture_CurrentItemObservation = &s_ctxMovieFileTexture_CurrentItemObservation;
static void* s_ctxMovieFileTexture_PlayerItemStatusObserver = &s_ctxMovieFileTexture_PlayerItemStatusObserver;

/*---------------------------------------------------------------------*//**
 * ビデオカメラデリゲートクラス
 *
**//*---------------------------------------------------------------------*/
@interface MovieFileTextureIosDelegater : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
	atar::MovieFileTextureIos* _own;
}

@property atar::MovieFileTextureIos* own;

@end

@implementation MovieFileTextureIosDelegater

@synthesize own = _own;

/*---------------------------------------------------------------------*//**
	状態変更通知
**//*---------------------------------------------------------------------*/
- (void)observeValueForKeyPath:(NSString*)keyPath
	ofObject:(id)object
	change:(NSDictionary*)change
	context:(void*)ctx
{
	if(!_own->notifyChangedValueForKeyPath(ctx, &object, &keyPath))
	{
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:ctx];
	}
}

/*---------------------------------------------------------------------*//**
	再生終了通知
**//*---------------------------------------------------------------------*/
- (void)playerItemDidReachEnd:(NSNotification*)notification
{
	_own->notifyPlayerItemDidReachEnd();
}

@end	// @implementation MovieFileTextureIosDelegater

//==========================================================================
// MovieFileTextureIosInner クラス

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 * Objective-C クラス内包クラス
 *
**//*---------------------------------------------------------------------*/
class MovieFileTextureIosInner
{
public:
	AVURLAsset* _asset;
	AVPlayer* _player;
	AVPlayerItem* _item;
	AVAssetImageGenerator* _imggen;
	CGContextRef _imgctx;
	MovieFileTextureIosDelegater* _delg;
	
	MovieFileTextureIosInner(MovieFileTextureIos* own)
		: _asset(nil)
		, _player(nil)
		, _item(nil)
		, _imggen(nil)
		, _imgctx(nil)
		, _delg([[MovieFileTextureIosDelegater alloc] init])
	{
		_delg.own = own;
	}
	
	~MovieFileTextureIosInner()
	{
		#if !__has_feature(objc_arc)
			[_delg release];
		#endif
		_delg = nil;
	}
};

//==========================================================================
// MovieFileTextureIos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	プレイヤー取得
**//*---------------------------------------------------------------------*/
PTR_MOVIE_FILE_TEXTURE_IOS_AV_PLAYER MovieFileTextureIos::getPlayer() const
{
	return _inner->_player;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ムービー再生中かどうかを得る
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::isPlaying() const
{
	if(_inner->_player == nil)	{	return false;	}
	return _inner->_player.rate != 0.0;
}

/*---------------------------------------------------------------------*//**
	ムービーが修了したかどうかを得る
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::isFinished() const
{
	return _playstat == PLAYSTAT_FINISHED;
}

/*---------------------------------------------------------------------*//**
	ムービーが再生準備完了しているかどうかを得る
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::isPrepared() const
{
	return _playstat == PLAYSTAT_PREPARED;
}

/*---------------------------------------------------------------------*//**
	ファイル再生準備
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::prepareFile(CStringBase* pathFile)
{
	TRACELN("{MovieFileTextureIos::prepare} %s", pathFile->getRaw());

	// ムービーファイルの再生準備
	NSString* filepathAsset = [[NSString alloc] initWithCString:pathFile->getRaw() encoding:NSUTF8StringEncoding];
	NSURL* urlAsset = [NSURL fileURLWithPath:filepathAsset];
	NSDictionary* dictAssetOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
	AVURLAsset* asset = [[AVURLAsset alloc] initWithURL:urlAsset options:dictAssetOptions];
#if 1
	AVPlayerItem* item = [AVPlayerItem playerItemWithAsset:asset];
	[item addObserver:_inner->_delg forKeyPath:@"status" options:0 context:s_ctxMovieFileTexture_PlayerItemStatusObserver];
	[[NSNotificationCenter defaultCenter] addObserver:_inner->_delg selector:@selector(playerItemDidReachEnd:) name:AVPlayerItemDidPlayToEndTimeNotification object:item];
	AVPlayer* player = [[AVPlayer alloc] initWithPlayerItem:item];
#else
	[asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:@"tracks"]
		completionHandler:^{
			NSError* error = nil;
			AVKeyValueStatus status = [asset statusOfValueForKey:@"tracks" error:&error];
			if (status == AVKeyValueStatusLoaded)
			{
				_inner->_item = [[AVPlayerItem alloc] initWithAsset:asset];
				[_inner->_item addObserver:_delg forKeyPath:@"status" options:0 context:s_ctxMovieFileTexture_PlayerItemStatusObserver];
				[[NSNotificationCenter defaultCenter] addObserver:_inner->_delg selector:@selector(playerItemDidReachEnd:) name:AVPlayerItemDidPlayToEndTimeNotification object:_inner->_item];
				_inner->_player = [[AVPlayer alloc] initWithPlayerItem:_inner->_item];
			}
			else
			{
				TRACELN("{MovieFileTextureIos::prepareFile} %s", [[error localizedDescription] cStringUsingEncoding:NSASCIIStringEncoding]);
			}
		}
	];
#endif
	
#if 1
	if(![asset tracksWithMediaCharacteristic:AVMediaTypeVideo])
	{
		TRACELN("{MovieFileTextureIos::prepareFile} tracksWithMediaCharacteristic failed.");
		return false;
	}
	AVAssetImageGenerator* imggen = [AVAssetImageGenerator assetImageGeneratorWithAsset:asset];
#else
	AVAssetTrack* trackSrc = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
	AVMutableComposition* comp = [AVMutableComposition composition];
	AVMutableCompositionTrack* ctrack = [comp addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
	NSError* error = nil;
	BOOL ok = [ctrack insertTimeRange:trackSrc.timeRange ofTrack:trackSrc atTime:kCMTimeZero error:&error];
	if(!ok)
	{
		TRACE("%@", [error localizedDescription]);
		return false;
	}
	AVAssetImageGenerator* imggen = [[AVAssetImageGenerator alloc] initWithAsset:comp];
#endif

	_inner->_asset = asset;
	_inner->_player = player;
	_inner->_item = item;
	_inner->_imggen = imggen;
	_inner->_imgctx = 0L;
	
	_playstat = PLAYSTAT_PREPARING;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ムービー再生
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::play()
{
	if(_inner->_player == nil)	{	return false;	}
	_playstat = PLAYSTAT_PLAYING;
	[_inner->_player play];
	return true;
}

/*---------------------------------------------------------------------*//**
	ムービー停止
**//*---------------------------------------------------------------------*/
void MovieFileTextureIos::pause()
{
	if(_inner->_player == nil)	{	return;			}
	_playstat = PLAYSTAT_PAUSING;
	[_inner->_player pause];
}

/*---------------------------------------------------------------------*//**
	ムービー画像生成
**//*---------------------------------------------------------------------*/
u8* MovieFileTextureIos::generateMovieBitmap()
{
	if(_inner->_player == nil)
	{
		TRACELN("{MovieFileTextureIos::generateMovieBitmap} no player.");
		return 0L;
	}

	// ムービー画像を取得する
#if 0
	Float64 durationSeconds = CMTimeGetSeconds([_inner->_asset duration]);
	CMTime midpoint = CMTimeMakeWithSeconds(durationSeconds / 2.0, 600);
	CMTime actualTime;
	NSError* error = nil;
	CGImageRef imageMov = [_inner->_imggen copyCGImageAtTime:midpoint actualTime:&actualTime error:&error];
#else
	CMTime time = [_inner->_player currentTime];
	CMTime actualTime;
	NSError* error = nil;
	CGImageRef imageMov = [_inner->_imggen copyCGImageAtTime:time actualTime:&actualTime error:&error];
#endif
	if (imageMov == nil)
	{
		TRACELN("{MovieFileTextureIos::generateMovieBitmap} copyCGImageAtTime failed: %s", [[error localizedDescription] cStringUsingEncoding:NSASCIIStringEncoding]);
		return 0L;
	}
	
	size_t width = CGImageGetWidth(imageMov);
	size_t height = CGImageGetHeight(imageMov);
	
	// 画像がない場合は作成する
	if(_inner->_imgctx == NULL)
	{
		CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Big;
		switch (CGImageGetAlphaInfo(imageMov))
		{
		case kCGImageAlphaPremultipliedLast:
		case kCGImageAlphaPremultipliedFirst:
		case kCGImageAlphaLast:
		case kCGImageAlphaFirst:
			bitmapInfo |= kCGImageAlphaPremultipliedLast;
			break;
		default:
			bitmapInfo |= kCGImageAlphaNoneSkipLast;
			break;
		}
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		_inner->_imgctx = CGBitmapContextCreate(_bits, _width, _height, 8, _width * 4, colorSpace, bitmapInfo);
		CGColorSpaceRelease(colorSpace);
		if ((width != _width) || (height != _height))
		{
			CGContextScaleCTM(_inner->_imgctx, _width / (f32)width, _height / (f32)height);
		}
	}

	// 描画を行う
	CGRect rect = CGRectMake(0, 0, width, height);
	CGContextDrawImage(_inner->_imgctx, rect, imageMov);

	// ムービー画像を解放する
	CGImageRelease(imageMov);
	
	return _bits;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MovieFileTextureIos::MovieFileTextureIos()
	: _bits(0L)
	, _width(0)
	, _height(0)
	, _playstat(PLAYSTAT_NULL)
	, _inner(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MovieFileTextureIos::~MovieFileTextureIos()
{
	ASSERT(_inner == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::create(s32 widthTexture, s32 heightTexture)
{
	// 内部インスタンスの作成
	_inner = new MovieFileTextureIosInner(this);

	// 値取得
	_width = widthTexture;
	_height = heightTexture;

	// バッファ作成
	_bits = new u8[_width * _height * 4];
		
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MovieFileTextureIos::destroy()
{
	// 再生停止
	if(isPlaying())	{	pause();	}

	// インスタンス解放
	if(_inner != 0L)
	{
		#if !__has_feature(objc_arc)
			[_inner->_imgctx release];
			[_inner->_imggen release];
			[_inner->_item release];
			[_inner->_player release];
			[_inner->_asset release];
		#endif
		_inner->_imgctx = 0L;
		_inner->_imggen = 0L;
		_inner->_item = 0L;
		_inner->_player = 0L;
		_inner->_asset = 0L;
	}
	
	// バッファ解放
	delete[] _bits;
	_bits = 0L;

	// 内部インスタンスの解放
	delete _inner;
	_inner = 0L;
}

/*---------------------------------------------------------------------*//**
	状態変更通知
**//*---------------------------------------------------------------------*/
bool MovieFileTextureIos::notifyChangedValueForKeyPath(void* context, void* object, void* key)
{
	if(context == s_ctxMovieFileTexture_PlayerItemStatusObserver)
	{
		_playstat = PLAYSTAT_PREPARED;
		
#if 0
		//if((_inner->_item.status == AVPlayerStatusReadyToPlay) && (_inner->_player.status == AVPlayerStatusReadyToPlay))
		if((_inner->_item.status == AVPlayerStatusReadyToPlay) && (_inner->_player.rate == 0))
		{
			for(AVPlayerItemTrack* itemTrack in _inner->_item.tracks)
			{
				if(![itemTrack.assetTrack hasMediaCharacteristic:AVMediaCharacteristicAudible])
				{
					itemTrack.enabled = NO;
				}
			}
		}

		//[_inner->_player play];
#endif
	}
	else
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	再生終了通知
**//*---------------------------------------------------------------------*/
void MovieFileTextureIos::notifyPlayerItemDidReachEnd()
{
	_playstat = PLAYSTAT_FINISHED;
	[_inner->_player seekToTime:kCMTimeZero];
}

ATAR_END_NS