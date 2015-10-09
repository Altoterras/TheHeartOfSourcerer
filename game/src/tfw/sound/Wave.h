/***********************************************************************//**
 *	Wave.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_WAVE_H_
#define _TFW_SOUND_WAVE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

/*---------------------------------------------------------------------*//**
 *	リニア PCM 波形抽象クラス
 *	
**//*---------------------------------------------------------------------*/
class Wave  
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// データを得る
	virtual const u8* getData() const = 0;
	virtual u8* data() = 0;
	// データサイズを得る
	virtual u32 getDataSize() const = 0;
	// チャンネル数を得る
	virtual u32 getChannelNum() const = 0;
	// サンプルビット数を得る
	virtual u32 getBitsPerSample() const = 0;
	// 周波数を得る
	virtual u32 getFrequency() const = 0;
	// サンプル値の最大値を得る
	virtual u32 getMaxSampleOffset() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual			~Wave() {}
	bool			create(const CStringBase* filepath, u16 cflag);
	bool			create(const char* fbuf, int size, u16 cflag);
	bool			create(u32 fileid, u16 cflag);
	virtual void	destroy() {}

protected:
	virtual bool	createFromFile(FileBase* file, u16 cflag) = 0;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_RIFF_WAV_H_
