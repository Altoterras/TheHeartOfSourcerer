/***********************************************************************//**
 *	RiffWav.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/10/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_RIFF_WAV_H_
#define _TFW_SOUND_RIFF_WAV_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Wave.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

/*---------------------------------------------------------------------*//**
 *	RIFF Wave
 *	
**//*---------------------------------------------------------------------*/
class RiffWav : public Wave
{
	//======================================================================
	// 定数
public:
	// 作成フラグ
	static const u16	CFLAG_OPEN_ONLY	= 0x0001;	// ファイルを開くのみ

	// 波形タイプ
	enum WAVTYPE
	{
		WAVTYPE_NULL	= 0,
		WAVTYPE_PCM,
		WAVTYPE_EXT,
	};

	//======================================================================
	// 構造体
private:
	struct WaveFormatEx
	{
		u16 _wFormatTag;
		u16 _nChannels;
		u32 _nSamplesPerSec;
		u32 _nAvgBytesPerSec;
		u16 _nBlockAlign;
		u16 _wBitsPerSample;
		u16 _cbSize;
	};

	struct Guid
	{
		u32 _data1;
		u16 _data2;
		u16 _data3;
		u8 _data4[ 8 ];
	};

	struct WaveFormatExtensible
	{
		WaveFormatEx _format;
		union
		{
			u16 _wValidBitsPerSample;	// bits of precision
			u16 _wSamplesPerBlock;		// valid if wBitsPerSample == 0
			u16 _wReserved;				// If neither applies, set to zero.
		} Samples;
		u32 _dwChannelMask;		// which channels are present in stream
		Guid _subFormat;
	};

	struct WaveFileHeader
	{
		char szRIFF[4];
		u32 ulRIFFSize;
		char szWAVE[4];
	};

	struct RiffChunk
	{
		char szChunkName[4];
		u32 ulChunkSize;	//unsigned long	ulChunkSize;
	};

	struct WaveFmt
	{
		u16	usFormatTag;
		u16	usChannels;
		u32	ulSamplesPerSec;
		u32	ulAvgBytesPerSec;
		u16	usBlockAlign;
		u16	usBitsPerSample;
		u16	usSize;
		u16	usReserved;
		u32	ulChannelMask;
		Guid guidSubFormat;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// データを得る
	inline const u8* getData() const { return _data; }
	inline u8* data() { return _data; }
	// データサイズを得る
	inline u32 getDataSize() const { return _sizeData; }
	// チャンネル数を得る
	u32 getChannelNum() const;
	// サンプルビット数を得る
	u32 getBitsPerSample() const;
	// 周波数を得る
	u32 getFrequency() const;
	// サンプル値の最大値を得る
	u32 getMaxSampleOffset() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	RiffWav();
	~RiffWav();
	void			destroy();

	bool			readRiffWaveData(void* pData, u32 ulDataSize, u32* pulBytesWritten);
	bool			setRiffWaveDataOffset(u32 offset);
	bool			getRiffWaveDataOffset(u32* pulOffset);

protected:
	bool			createFromFile(FileBase* file, u16 cflag);
	bool			parse(FileBase* file);

	//======================================================================
	// メンバ変数

private:
	WAVTYPE					_type;
	WaveFormatExtensible	_fmtext;
	u8*						_data;
	u32						_sizeData;
	FileBase*				_file;
	u32						_offsetData;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_RIFF_WAV_H_
