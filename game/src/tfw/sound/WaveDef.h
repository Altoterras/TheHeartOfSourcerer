/***********************************************************************//**
 *	WaveDef.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/10/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SOUND_WAVE_DEF_H_
#define _TFW_SOUND_WAVE_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

/*---------------------------------------------------------------------*//**
 *	波形定義
 *	
**//*---------------------------------------------------------------------*/
class WaveDef  
{
public:
	u16			_id;			// 曲 ID
	u8			_wfmt;			// 波形フォーマット（WaveFormat::*）
	u32			_fileid;		// ファイル ID 読みではない場合は 0 で、下記のパスを使う
	VcString*	_pathDir;		// 曲ファイルのディレクトリパス
	VcString*	_filename;		// ファイル名（拡張子を含まない）
	VcString*	_type;			// "mp3" など
	s32			_smplLoopStart;	// 開始ループポイントサンプル値
	s32			_smplLoopEnd;	// 終了ループポイントサンプル値（曲終端までの場合は S32_MAX 指定可能）

	WaveDef(u16 id, const CStringBase* pathDir, const CStringBase* filename, const CStringBase* type, s32 smplLoopStart, s32 smplLoopEnd);
	WaveDef(u16 id, u32 fileid, u8 wfmt, s32 smplLoopStart, s32 smplLoopEnd);
	~WaveDef();
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_SOUND_WAVE_DEF_H_
