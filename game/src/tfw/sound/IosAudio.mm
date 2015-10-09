/***********************************************************************//**
 *  IosAudio.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/10/08.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self

////////////////////////////////////////////////////////////////////////////
// 型定義

// Friends
#include "../Base.h"
#include "../debug/TfwDebug.h"

// External

// Library
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/ExtendedAudioFile.h>
#import <AVFoundation/AVAudioPlayer.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreFoundation/CFURL.h>
#import <UIKit/UIKit.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// Objective-C クラス

class IosAudio;

TFW_END_NS

@interface AudioDelegater : NSObject <AVAudioPlayerDelegate>
{
	tfw::IosAudio* _ia;
}

@property tfw::IosAudio* _ia;

@end

#define TYPE_IOS_AUDIO_PLAYER AVAudioPlayer
#define TYPE_IOS_AUDIO_DELEGATER AudioDelegater
#include "IosAudio.h"

@implementation AudioDelegater

@synthesize _ia;

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
- (id)init
{
	self = [super init];
	
	_ia = 0L;
	
	return self;
}

/*---------------------------------------------------------------------*//**
	曲終了のイベントハンドラ
**//*---------------------------------------------------------------------*/
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
	/*
	TRACELN("audioPlayerDidFinishPlaying");
	::tfw::TfwDebug::trace("!!! 3 !!!\n");
	[player release];
	 */
	if(_ia != 0L)
	{
		_ia->releasePlayer();
	}
}

@end

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// グローバル関数

#if 0
/*---------------------------------------------------------------------*//**
	オーディオデータ読み込み
**//*---------------------------------------------------------------------*/
bool IosAudio_loadAudioData(const char* nameFile, const char* typeFile, void** data, ALsizei* sizeData, ALenum* format, ALsizei* samplerate)
{
	bool							isSucceeded = false;
	OSStatus						err = noErr;	
	SInt64							propFileLengthInFrames = 0;
	AudioStreamBasicDescription		propFileFormat;
	UInt32							sizeProperty;
	ExtAudioFileRef					extrefFile = 0L;
	void*							dataNew = 0L;
	UInt32							sizeDataNew = 0;
	AudioStreamBasicDescription		descOutputFormat;
	NSURL*							url;
	
	// URL を作成する
	NSString* strFileName = [[NSString alloc] initWithUTF8String: nameFile];
	NSString* strFileType = [[NSString alloc] initWithUTF8String: typeFile];
	NSString* path = [[NSBundle mainBundle] pathForResource:strFileName ofType:strFileType];
	if(path == nil) { TRACE("{IosAudio_loadAudioData} NSBundle pathForResource FAILED, path is nil\n"); goto Exit; }
	url = [[NSURL alloc] initFileURLWithPath: path];
	if(url == nil) { TRACE("{IosAudio_loadAudioData} NSURL initFileURLWithPath FAILED, url is nil\n"); goto Exit; }
	
	// ファイルを開く
	err = ExtAudioFileOpenURL((CFURLRef)url, &extrefFile);
	if(err) { TRACE("{IosAudio_loadAudioData} ExtAudioFileOpenURL FAILED, Error = %ld\n", err); goto Exit; }
	
	// オーディオデータフォーマットを得る
	sizeProperty = sizeof(propFileFormat);
	err = ExtAudioFileGetProperty(extrefFile, kExtAudioFileProperty_FileDataFormat, &sizeProperty, &propFileFormat);
	if(err) { TRACE("{IosAudio_loadAudioData} ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %ld\n", err); goto Exit; }
	if(propFileFormat.mChannelsPerFrame > 2) { TRACE("{IosAudio_loadAudioData} - Unsupported Format, channel count is greater than stereo\n"); goto Exit; }

	// クライアント出力フォーマット
	descOutputFormat.mSampleRate = propFileFormat.mSampleRate;
	descOutputFormat.mChannelsPerFrame = propFileFormat.mChannelsPerFrame;
	descOutputFormat.mFormatID = kAudioFormatLinearPCM;
	descOutputFormat.mBytesPerPacket = 2 * descOutputFormat.mChannelsPerFrame;
	descOutputFormat.mFramesPerPacket = 1;
	descOutputFormat.mBytesPerFrame = 2 * descOutputFormat.mChannelsPerFrame;
	descOutputFormat.mBitsPerChannel = 16;	// 16 bit
	descOutputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;	// エンディアン等
	
	// クライアント出力データフォーマットを設定する
	err = ExtAudioFileSetProperty(extrefFile, kExtAudioFileProperty_ClientDataFormat, sizeof(descOutputFormat), &descOutputFormat);
	if(err) { TRACE("MyGetOpenALAudioData: ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) FAILED, Error = %ld\n", err); goto Exit; }

	// トータルフレーム数を得る
	sizeProperty = sizeof(propFileLengthInFrames);
	err = ExtAudioFileGetProperty(extrefFile, kExtAudioFileProperty_FileLengthFrames, &sizeProperty, &propFileLengthInFrames);
	if(err) { TRACE("MyGetOpenALAudioData: ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %ld\n", err); goto Exit; }

	// メモリにデータを読み込む
	sizeDataNew = propFileLengthInFrames * descOutputFormat.mBytesPerFrame;
	dataNew = new char[sizeDataNew];
	if(dataNew != 0L)
	{
		// オーディオバッファリストにデータを読み込む
		AudioBufferList	buflistData;
		buflistData.mNumberBuffers = 1;
		buflistData.mBuffers[0].mDataByteSize = sizeDataNew;
		buflistData.mBuffers[0].mNumberChannels = descOutputFormat.mChannelsPerFrame;
		buflistData.mBuffers[0].mData = dataNew;
		err = ExtAudioFileRead(extrefFile, (UInt32*)&propFileLengthInFrames, &buflistData);
		if(err == noErr)
		{
			// 成功
			*sizeData = (ALsizei)sizeDataNew;
			*format = (descOutputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
			*samplerate = (ALsizei)descOutputFormat.mSampleRate;
			*data = dataNew;
			isSucceeded = true;
		}
		else 
		{ 
			// 失敗
			delete[] dataNew;
			TRACE("MyGetOpenALAudioData: ExtAudioFileRead FAILED, Error = %ld\n", err);
			goto Exit;
		}	
	}
	
Exit:
	// ファイルを閉じる
	if(extrefFile) { ExtAudioFileDispose(extrefFile); }

	[url release];
	[strFileName release];
	[strFileType release];
	
	return isSucceeded;
}
#endif

/*---------------------------------------------------------------------*//**
	iPhone 最適化版 OpenAL バッファ送信
**//*---------------------------------------------------------------------*/
typedef ALvoid (AL_APIENTRY *alBufferDataStaticPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
void IosAudio_alBufferDataStatic(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	static alBufferDataStaticPtr	proc = 0L;
    
	// 関数アドレス取得
	if(proc == 0L)
	{
		proc = (alBufferDataStaticPtr)::alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
	}
    
	if(proc != 0L)
	{
		proc(bid, format, data, size, freq);
	}
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IosAudio メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	音楽再生
**//*---------------------------------------------------------------------*/
bool IosAudio::playMusic(const char* name, const char* type, s32 cntLoop)
{
	// URL を作成する
	NSString* strName = [[NSString alloc] initWithUTF8String: name];
	NSString* strType = [[NSString alloc] initWithUTF8String: type];
	NSURL* url = [[NSURL alloc] initFileURLWithPath: [[NSBundle mainBundle] pathForResource:strName ofType:strType]];
	if(!url) { return false; }
	
	// プレイヤーを解放
	releasePlayer();
	
	// プレイヤーを初期化
	NSError* error = nil;
	AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
	if(error)
	{
		#if !__has_feature(objc_arc)
			[error release];
		#endif
		return false;
	}
	
	// 解放用のデリゲータを設定
	player.delegate = _audiodelg;
	
	// ループ設定
	player.numberOfLoops = cntLoop;
	
	// ボリューム設定
	player.volume = _volume;
	
	// 再生処理
	[player play];
	
	#if !__has_feature(objc_arc)
		[url release];
		[strName release];
		[strType release];
	#endif
	
	_playerMusic = player;
	_isInitPlayer = true;
	_isPausing = false;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	音楽停止
**//*---------------------------------------------------------------------*/
bool IosAudio::stopMusic()
{
	if(!_isInitPlayer)	{	return false;	}
	
	// 停止処理
	[_playerMusic stop];
	
	// プレイヤーを解放
	releasePlayer();

	_isPausing = false;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	音楽一時停止
**//*---------------------------------------------------------------------*/
bool IosAudio::pauseMusic()
{
	if(!_isInitPlayer)	{	return false;	}
	
	// 一時停止処理
	[_playerMusic pause];
	
	_isPausing = true;
	
	return true;
}


/*---------------------------------------------------------------------*//**
	音楽再開
**//*---------------------------------------------------------------------*/
bool IosAudio::resumeMusic()
{
	if(!_isInitPlayer)	{	return false;	}
	
	// 再開処理
	[_playerMusic play];

	_isPausing = false;
	
	return true;
}


/*---------------------------------------------------------------------*//**
	音楽を再生しているかどうか
**//*---------------------------------------------------------------------*/
bool IosAudio::isPlayingMusic()
{
	if(!_isInitPlayer)	{	return false;	}

	// 再生中かどうか
	return _playerMusic.playing;
}

/*---------------------------------------------------------------------*//**
	ボリュームの設定
**//*---------------------------------------------------------------------*/
void IosAudio::setMusicVolume(f32 volume)
{
	_volume = volume;
	
	// 初期化後は即座に反映
	if(_isInitPlayer)
	{
		_playerMusic.volume = _volume;
	}
}

/*---------------------------------------------------------------------*//**
	現在の再生曲のループ数の設定
**//*---------------------------------------------------------------------*/
void IosAudio::setCurrentLoopCount(s32 cntLoop)
{
	if(_isInitPlayer)
	{
		_playerMusic.numberOfLoops = cntLoop;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IosAudio::IosAudio()
	: _playerMusic(0L)
	, _audiodelg(0L)
	, _isInitPlayer(false)
	, _isPausing(false)
	, _volume(1.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IosAudio::~IosAudio()
{
	ASSERT(_playerMusic == 0L);
	ASSERT(_audiodelg == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool IosAudio::create()
{
	_audiodelg = [[AudioDelegater alloc] init];
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void IosAudio::destroy()
{
	// プレイヤーを解放
	releasePlayer();
	
	#if !__has_feature(objc_arc)
		[_audiodelg release];
	#endif
	_audiodelg = 0L;
}

/*---------------------------------------------------------------------*//**
  プレイヤーの解放
**//*---------------------------------------------------------------------*/
void IosAudio::releasePlayer()
{
	if(_playerMusic != 0L)
	{
		if(_isInitPlayer)
		{
			#if !__has_feature(objc_arc)
				[_playerMusic release];
			#endif
		}
		
		_playerMusic = 0L;
	}
	_isInitPlayer = false;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS