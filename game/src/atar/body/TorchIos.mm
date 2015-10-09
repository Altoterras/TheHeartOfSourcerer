/***********************************************************************//**
 *  TorchIos.cpp
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
#include "TorchIos.h"

// Friends
#include "../Base.h"

// External

// Library
#import <AVFoundation/AVCaptureDevice.h>
#import <AVFoundation/AVFoundation.h>

////////////////////////////////////////////////////////////////////////////
// クラス

ATAR_BEGIN_NS

//==========================================================================
// TorchIos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	点灯
**//*---------------------------------------------------------------------*/
void TorchIos::startLighting()
{
	AVCaptureDevice* deviceCapture = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
 	if(deviceCapture == nil)
	{
		return;
	}
	if([deviceCapture respondsToSelector:@selector(torchActive)] && !deviceCapture.torchActive)
	{
		return;
	}
	if(!deviceCapture.torchAvailable)
	{
		return;
	}
	// トーチ開始
	NSError* error = nil;
	[deviceCapture lockForConfiguration:&error];
	deviceCapture.torchMode = AVCaptureTorchModeOn;
	[deviceCapture unlockForConfiguration];
	_on = true;
}

/*---------------------------------------------------------------------*//**
	消灯
**//*---------------------------------------------------------------------*/
void TorchIos::stopLighting()
{
	AVCaptureDevice* deviceCapture = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
 	if(deviceCapture == nil)
	{
		return;
	}
	if([deviceCapture respondsToSelector:@selector(torchActive)] && !deviceCapture.torchActive)
	{
		return;
	}
	// トーチ終了
	NSError* error = nil;
	[deviceCapture lockForConfiguration:&error];
	deviceCapture.torchMode = AVCaptureTorchModeOff;
	[deviceCapture unlockForConfiguration];
	_on = false;
}

/*---------------------------------------------------------------------*//**
	懐中電灯デバイスを持っているかどうか
**//*---------------------------------------------------------------------*/
bool TorchIos::hasDevice() const
{
	AVCaptureDevice* deviceCapture = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
 	if(deviceCapture == nil)
	{
		return false;
	}
	return deviceCapture.hasTorch == TRUE;
}

/*---------------------------------------------------------------------*//**
	デバイスの点灯状態が変数値と合うように更新する
**//*---------------------------------------------------------------------*/
void TorchIos::updateDevice()
{
	AVCaptureDevice* deviceCapture = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
 	if(deviceCapture == nil)
	{
		return;
	}
	if(_on)
	{
		if(deviceCapture.torchMode != AVCaptureTorchModeOn)
		{
			startLighting();
		}
	}
	else
	{
		if(deviceCapture.torchMode == AVCaptureTorchModeOn)
		{
			stopLighting();
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TorchIos::TorchIos()
	: _on(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TorchIos::~TorchIos()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TorchIos::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TorchIos::destroy()
{
}

ATAR_END_NS