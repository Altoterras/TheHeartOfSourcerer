/***********************************************************************//**
 *  RiffWav.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/10/06.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "RiffWav.h"

// Friends
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../string/StringUtils.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数定義

#if !defined(WAVE_FORMAT_PCM)
	#define		WAVE_FORMAT_PCM							1
#endif
#if !defined(WAVE_FORMAT_EXTENSIBLE)
	#define		WAVE_FORMAT_EXTENSIBLE                 0xFFFE
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// RiffWav メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	チャンネル数を得る
**//*---------------------------------------------------------------------*/
u32 RiffWav::getChannelNum() const
{
	return _fmtext._format._nChannels;
}

/*---------------------------------------------------------------------*//**
	サンプルビット数を得る
**//*---------------------------------------------------------------------*/
u32 RiffWav::getBitsPerSample() const
{
	return _fmtext._format._wBitsPerSample;
}

/*---------------------------------------------------------------------*//**
	周波数を得る
**//*---------------------------------------------------------------------*/
u32 RiffWav::getFrequency() const
{
	return _fmtext._format._nSamplesPerSec;
}

/*---------------------------------------------------------------------*//**
	サンプル値の最大値を得る
**//*---------------------------------------------------------------------*/
u32 RiffWav::getMaxSampleOffset() const
{
	return _sizeData / _fmtext._format._nChannels / (_fmtext._format._wBitsPerSample / 8);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
RiffWav::RiffWav() :
	_type(WAVTYPE_NULL),
	_data(0L),
	_sizeData(0),
	_file(0L),
	_offsetData(0)
{
	memset(&_fmtext, 0, sizeof(WaveFormatExtensible));
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
RiffWav::~RiffWav()
{
	ASSERT(_data == 0L);
	ASSERT(_file == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルから作成
**//*---------------------------------------------------------------------*/
bool RiffWav::createFromFile(FileBase* file, u16 cflag)
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
				TRACELN("{RiffWav::createFromFile} read data : size=%d, ofs=%d, memp=%x", _sizeData, _offsetData, _data);
				
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
void RiffWav::destroy()
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
	オープンした RiffWav のオーディオデータを読む
**//*---------------------------------------------------------------------*/
bool RiffWav::readRiffWaveData(void* pData, u32 ulDataSize, u32* pulBytesWritten)
{
	if(_file == 0L)	{	return false;	}

	u32 offset = _file->getPosition();

	if(_sizeData < (offset - _offsetData + ulDataSize))
	{
		ulDataSize = _sizeData - (offset - _offsetData);
	}

	*pulBytesWritten = _file->read(pData, ulDataSize);

	return true;
}

/*---------------------------------------------------------------------*//**
	オープンした RiffWav のオーディオデータへのオフセットを設定する
**//*---------------------------------------------------------------------*/
bool RiffWav::setRiffWaveDataOffset(u32 offset)
{
	if(_file == 0L)	{	return false;	}

	// オーディオデータ部分にシーク
	_file->seek(_offsetData + offset, true);

	return true;
}

/*---------------------------------------------------------------------*//**
	オープンした RiffWav のオーディオデータへのオフセットを得る
**//*---------------------------------------------------------------------*/
bool RiffWav::getRiffWaveDataOffset(u32* pulOffset)
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
bool RiffWav::parse(FileBase* file)
{
	WaveFileHeader wfhead;
	RiffChunk rfchunk;
	WaveFmt wfmt;

	// ファイルヘッダを読む
	file->read(&wfhead, sizeof(WaveFileHeader));
	if(StringUtils::equalsIgnoreCaseNum((const char*)wfhead.szRIFF, "RIFF", 4) && StringUtils::equalsIgnoreCaseNum((const char*)wfhead.szWAVE, "WAVE", 4))
	{
		while(file->read(&rfchunk, sizeof(RiffChunk)) == sizeof(RiffChunk))
		{
			if(StringUtils::equalsIgnoreCaseNum((const char*)rfchunk.szChunkName, "fmt ", 4))
			{
				if(rfchunk.ulChunkSize <= sizeof(WaveFmt))
				{
					// フォーマットを得る
					file->read(&wfmt, rfchunk.ulChunkSize);
					if(wfmt.usFormatTag == WAVE_FORMAT_PCM)
					{
						_type = WAVTYPE_PCM;
						::memcpy(&_fmtext._format, &wfmt, sizeof(WaveFormatEx));
					}
					else if (wfmt.usFormatTag == WAVE_FORMAT_EXTENSIBLE)
					{
						_type = WAVTYPE_EXT;
						::memcpy(&_fmtext, &wfmt, sizeof(WaveFormatExtensible));
					}
				}
				else
				{
					file->seek(rfchunk.ulChunkSize, false);
				}
			}
			else if(StringUtils::equalsIgnoreCaseNum((const char*)rfchunk.szChunkName, "data", 4))
			{
				// オーディオデータの情報を得る
				_sizeData = rfchunk.ulChunkSize;
				_offsetData = file->getPosition();
				file->seek(rfchunk.ulChunkSize, false);
			}
			else
			{
				file->seek(rfchunk.ulChunkSize, false);
			}

			// 次のチャンクへ
			if((rfchunk.ulChunkSize & 1) != 0)
			{
				file->seek(1, false);
			}
		}

		if((_sizeData > 0) && (_offsetData > 0) && ((_type == WAVTYPE_PCM) || (_type == WAVTYPE_EXT)))
		{
			return true;
		}
	}

	return false;
}

#if 0
bool RiffWav::getAlBufferFormat(PFNALGETENUMVALUE pfnGetEnumValue, u32 *pulFormat)
{
	*pulFormat = 0;

	if(_type == WAVTYPE_PCM)
	{
		if(_fmtext.Format.nChannels == 1)
		{
			switch(_fmtext.Format.wBitsPerSample)
			{
			case 4:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO_IMA4");
				break;
			case 8:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO8");
				break;
			case 16:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO16");
				break;
			}
		}
		else if(_fmtext.Format.nChannels == 2)
		{
			switch(_fmtext.Format.wBitsPerSample)
			{
			case 4:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO_IMA4");
				break;
			case 8:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO8");
				break;
			case 16:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO16");
				break;
			}
		}
		else if((_fmtext.Format.nChannels == 4) && (_fmtext.Format.wBitsPerSample == 16))
		{
			*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
		}
	}
	else if(_type == WAVTYPE_EXT)
	{
		if(	(_fmtext.Format.nChannels == 1) &&
			((_fmtext.dwChannelMask == SPEAKER_FRONT_CENTER) ||
			(_fmtext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT)) ||
			(_fmtext.dwChannelMask == 0)))
		{
			switch(_fmtext.Format.wBitsPerSample)
			{
			case 4:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO_IMA4");
				break;
			case 8:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO8");
				break;
			case 16:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO16");
				break;
			}
		}
		else if((_fmtext.Format.nChannels == 2) && (_fmtext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT)))
		{
			switch(_fmtext.Format.wBitsPerSample)
			{
			case 4:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO_IMA4");
				break;
			case 8:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO8");
				break;
			case 16:
				*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO16");
				break;
			}
		}
		else if((_fmtext.Format.nChannels == 2) && (_fmtext.Format.wBitsPerSample == 16) && (_fmtext.dwChannelMask == (SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
		{
			*pulFormat =  pfnGetEnumValue("AL_FORMAT_REAR16");
		}
		else if ((_fmtext.Format.nChannels == 4) && (_fmtext.Format.wBitsPerSample == 16) && (_fmtext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
		{
			*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
		}
		else if ((_fmtext.Format.nChannels == 6) && (_fmtext.Format.wBitsPerSample == 16) && (_fmtext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
		{
			*pulFormat = pfnGetEnumValue("AL_FORMAT_51CHN16");
		}
		else if ((_fmtext.Format.nChannels == 7) && (_fmtext.Format.wBitsPerSample == 16) && (_fmtext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_BACK_CENTER)))
		{
			*pulFormat = pfnGetEnumValue("AL_FORMAT_61CHN16");
		}
		else if ((_fmtext.Format.nChannels == 8) && (_fmtext.Format.wBitsPerSample == 16) && (_fmtext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT)))
		{
			*pulFormat = pfnGetEnumValue("AL_FORMAT_71CHN16");
		}
	}

	if(*pulFormat == 0)
	{
		return false;
	}

	return true;
}
#endif

////////////////////////////////////////////////////////////////////////////

TFW_END_NS