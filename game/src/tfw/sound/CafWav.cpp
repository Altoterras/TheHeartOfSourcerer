/***********************************************************************//**
 *	CafWav.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/04/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CafWav.h"

// Friends
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../lib/Calc.h"
#include "../string/StringUtils.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CafWav メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	チャンネル数を得る
**//*---------------------------------------------------------------------*/
u32 CafWav::getChannelNum() const
{
	return _fmt.mChannelsPerFrame;
}

/*---------------------------------------------------------------------*//**
	サンプルビット数を得る
**//*---------------------------------------------------------------------*/
u32 CafWav::getBitsPerSample() const
{
	return _fmt.mBitsPerChannel;
}

/*---------------------------------------------------------------------*//**
	周波数を得る
**//*---------------------------------------------------------------------*/
u32 CafWav::getFrequency() const
{
	return (u32)_fmt.mSampleRate;
}

/*---------------------------------------------------------------------*//**
	サンプル値の最大値を得る
**//*---------------------------------------------------------------------*/
u32 CafWav::getMaxSampleOffset() const
{
	return _sizeData / _fmt.mChannelsPerFrame / (_fmt.mBitsPerChannel / 8);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CafWav::CafWav() :
	_type(WAVTYPE_NULL),
	_data(0L),
	_sizeData(0),
	_file(0L),
	_offsetData(0)
{
	memset(&_fmt, 0, sizeof(CafAudioFormat));
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CafWav::~CafWav()
{
	ASSERT(_data == 0L);
	ASSERT(_file == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルから作成
**//*---------------------------------------------------------------------*/
bool CafWav::createFromFile(FileBase* file, u16 cflag)
{
	bool isSucceeded = true;

	if(TFW_IS_FLAG(cflag, CFLAG_OPEN_ONLY))	// ファイルを開くのみの場合
	{
		if(parse(file))
		{
			_file = file;
		}
		else
		{
			// 失敗
			file->close();
			delete file;
			isSucceeded = false;
		}
	}
	else									// ファイルを開くだけでなくデータを読み込む場合
	{
		if(parse(file))
		{
			// データ配列を確保して読み込む
			_data = new u8[_sizeData];
			if(_data != 0L)
			{
				// オーディオデータ部分までシークする
				file->seek(_offsetData, true);

				// サンプルデータを読み込む
				if(file->read(_data, _sizeData) != _sizeData)
				{
					// 失敗
					delete _data;
					_data = 0L;
					isSucceeded = false;
				}
			}
			else
			{
				isSucceeded = false;
			}
		}

		file->close();
		delete file;
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void CafWav::destroy()
{
	if(_data != 0L)
	{
		delete _data;
		_data = 0L;
	}

	if(_file != 0L)
	{
		_file->close();
		delete _file;
		_file = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	オープンした CafWav のオーディオデータを読む
**//*---------------------------------------------------------------------*/
bool CafWav::readWaveData(void* pData, u32 ulDataSize, u32* pulBytesWritten)
{
	if(_file == 0L)	{	return false;	}

	//unsigned long offset = _file->getPosition();
	u32 offset = _file->getPosition();

	if(_sizeData < (offset - _offsetData + ulDataSize))
	{
		ulDataSize = _sizeData - (offset - _offsetData);
	}

	*pulBytesWritten = _file->read(pData, ulDataSize);

	return true;
}

/*---------------------------------------------------------------------*//**
	オープンした CafWav のオーディオデータへのオフセットを設定する
**//*---------------------------------------------------------------------*/
bool CafWav::setWaveDataOffset(u32 offset)
{
	if(_file == 0L)	{	return false;	}

	// オーディオデータ部分にシーク
	_file->seek(_offsetData + offset, true);

	return true;
}

/*---------------------------------------------------------------------*//**
	オープンした CafWav のオーディオデータへのオフセットを得る
**//*---------------------------------------------------------------------*/
bool CafWav::getWaveDataOffset(u32* pulOffset)
{
	if(_file == 0L)	{	return false;	}
	if(pulOffset == 0L)	{	return false;	}

	// 現在のオーディオデータ部分のポジションを得る
	*pulOffset = _file->getPosition();
	*pulOffset -= _offsetData;

	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルを解析する
**//*---------------------------------------------------------------------*/
bool CafWav::parse(FileBase* file)
{
	CafFileHeader cafhead;
	CafChunkHeader cafchhead;
	CafAudioFormat cafmt;

	// ファイルヘッダを読む
	file->read(&cafhead, sizeof(CafFileHeader));

	if(StringUtils::equalsIgnoreCaseNum((const char*)cafhead.mFileType, "caff", 4))
	{
		while(file->read(&cafchhead, sizeof(CafChunkHeader)) == sizeof(CafChunkHeader))
		{
			s32 size = TFW_S32_CH_ENDIAN(cafchhead.mChunkSize2);	// (TFW_S32_CH_ENDIAN(cafchhead.mChunkSize1) << 32) | TFW_S32_CH_ENDIAN(cafchhead.mChunkSize2); ← 64bit まで想定するなら…

			if(StringUtils::equalsIgnoreCaseNum((const char*)cafchhead.mChunkType, "desc", 4))
			{
				///TRACE("desc\n");
				ASSERT(size == sizeof(CafAudioFormat));
				file->read(&cafmt, sizeof(CafAudioFormat));
				Calc::convEndian8((u8*)&_fmt.mSampleRate, (u8*)&cafmt.mSampleRate);
				::memcpy((u8*)&_fmt.mFormatID, (u8*)&cafmt.mFormatID, 4);
				Calc::convEndian4((u8*)&_fmt.mFormatFlags, (u8*)&cafmt.mFormatFlags);
				Calc::convEndian4((u8*)&_fmt.mBytesPerPacket, (u8*)&cafmt.mBytesPerPacket);
				Calc::convEndian4((u8*)&_fmt.mFramesPerPacket, (u8*)&cafmt.mFramesPerPacket);
				Calc::convEndian4((u8*)&_fmt.mChannelsPerFrame, (u8*)&cafmt.mChannelsPerFrame);
				Calc::convEndian4((u8*)&_fmt.mBitsPerChannel, (u8*)&cafmt.mBitsPerChannel);

				if(StringUtils::equalsIgnoreCaseNum((const char*)_fmt.mFormatID, "lpcm", 4))
				{
					_type = WAVTYPE_LPCM;
				}
			}
			else if(StringUtils::equalsIgnoreCaseNum((const char*)cafchhead.mChunkType, "data", 4))
			{
				///TRACE("data\n");

				// オーディオデータの情報を得る
				_sizeData = size;
				_offsetData = file->getPosition();
				file->seek(size, false);
			}
			else
			{
				file->seek(size, false);
			}
		}

		if((_sizeData > 0) && (_offsetData > 0))
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS