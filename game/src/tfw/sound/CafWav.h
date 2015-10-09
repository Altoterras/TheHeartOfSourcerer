/***********************************************************************//**
 *	CafWav.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/04/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_CAF_WAV_H_
#define _TFW_SOUND_CAF_WAV_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Wave.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

/*---------------------------------------------------------------------*//**
 *	CAF Wave
 *	
**//*---------------------------------------------------------------------*/
class CafWav : public Wave
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
		WAVTYPE_LPCM,
	};

	//======================================================================
	// 構造体
private:
	struct CafFileHeader
	{
		u8	mFileType[4];
		u16	mFileVersion;
		u16	mFileFlags;
	};

	struct CafChunkHeader
	{
		u8	mChunkType[4];
		s32	mChunkSize1;
		s32	mChunkSize2;
	};

	struct CafAudioFormat
	{
		f64	mSampleRate;
		u8	mFormatID[4];
		u32	mFormatFlags;
		u32	mBytesPerPacket;
		u32	mFramesPerPacket;
		u32 mChannelsPerFrame;
		u32	mBitsPerChannel;
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
	CafWav();
	~CafWav();
	void			destroy();

	bool			readWaveData(void* pData, u32 ulDataSize, u32* pulBytesWritten);
	bool			setWaveDataOffset(u32 offset);
	bool			getWaveDataOffset(u32* pulOffset);

protected:
	bool			createFromFile(FileBase* file, u16 cflag);
	bool			parse(FileBase* file);

	//======================================================================
	// メンバ変数

private:
	WAVTYPE					_type;
	CafAudioFormat			_fmt;
	u8*						_data;
	u32						_sizeData;
	FileBase*				_file;
	u32						_offsetData;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_CAF_WAV_H_
