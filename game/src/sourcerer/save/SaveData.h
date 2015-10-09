/***********************************************************************//**
 *	SaveData.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SAVE_SAVE_DATA_H_
#define _SRCR_SAVE_SAVE_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/serialize/SerializeBase.h"

#include "../status/CharStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	セーブ データ
 *
**//*---------------------------------------------------------------------*/
class SaveData : public SerializeBase
{
	//======================================================================
	// 定数
public:
	// セーブフラグ
	static const u32 SVF_CLEARING		= 0x00000001;	// クリアー時
	static const u32 SVF_EVENT_INTVL	= 0x00000010;	// イベントの途中

	// シリアライザの汎用フラグ
	static const u32 SRF_RESTARTING		= 0x00000001;	// リスタート時

private:
	// 汎用データの各保持値の数
	static const s32 NUM_SFID = 2048;		// フラグ（8 の倍数である必要がある）
	static const s32 NUM_SCID = 1024;		// 文字（もしくは 8bit 数値）
	static const s32 NUM_SNID = 32;			// 数値（32bit；32bit 浮動小数点 (f32) は memcpy する）
	static const s32 NUM_PFID = 8;			// セーブされない特殊な一時フラグ（8 の倍数である必要がある）

	//======================================================================
	// メソッド
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 汎用データの取得
	inline bool	getSaveF(s32 sfid) const		{	return getU8ArrF(_flagSaveGd, sfid);	}
	inline s8	getSaveC(s32 scid) const		{	return _s8SaveGd[scid];					}
	inline s32	getSaveN(s32 snid) const		{	return _s32SaveGd[snid];				}
	inline bool	getPlayF(s32 pfid) const 		{	return getU8ArrF(_flagPlayGd, pfid);	}
	// 汎用データの設定
	void		setSaveF(s32 sfid, bool val)	{	setU8ArrF(_flagSaveGd, sfid, val);		}
	void		setSaveC(s32 scid, s8 val)		{	_s8SaveGd[scid] = val;					}
	void		setSaveN(s32 snid, s32 val)		{	_s32SaveGd[snid] = val;					}
	void		setPlayF(s32 pfid, bool val)	{	setU8ArrF(_flagPlayGd, pfid, val);		}

	// プレイフレーム数を得る
	inline u32 getPlayFrame() const { return _framePlay; }
	// ロード時セーブフラグを得る
	inline u32 getLoadTimeSaveFlags() const { return _saveflagsLoad; }

	// バージョンを得る
	u16 getMajorVersionNumber() const;
	u8 getMinorVersionNumber() const;

	// セーブフラグを設定する
	void setSaveFlags(u32 saveflags);

private:
	static bool getU8ArrF(u8* farr, s32 sid)
	{
		return (farr[sid >> 3] & (1 << (sid & 7))) != 0;
	}
	static void setU8ArrF(u8* farr, s32 sid, bool val)
	{
		farr[sid >> 3] = (farr[sid >> 3] & ~(1 << (sid & 7))) | ((val ? 1 : 0) << (sid & 7));
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void clear();
	bool newGame();
	bool restartGame();
	bool debug_enterMap(s32 storyid);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	bool isSerializableNow();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SaveData();
	~SaveData();
	bool create();
	void destroy();
	void exec(const ExecCtx* ec);	// フレーム制御
	
	//======================================================================
	// 変数
public:
	// 汎用データ
	u8* _flagSaveGd;		// 汎用フラグ
	s8* _s8SaveGd;			// 汎用文字
	s32* _s32SaveGd;		// 汎用数値（32bit）
	u8* _flagPlayGd;		// 汎用プレイフラグ（保存されない一時データ）

	// 作業データ
	f32 _frameElapsedWork;	// 経過時間作業用変数
	u32 _framePlay;			// プレイフレーム数
	u32 _cntClear;			// クリア回数

	// 一時データ
	u32 _saveflagsSave;		// セーブ時セーブフラグ
	u32 _saveflagsLoad;		// ロード時セーブフラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SAVE_SAVE_DATA_H_
