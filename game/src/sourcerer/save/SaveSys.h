/***********************************************************************//**
 *	SaveSys.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SAVE_SAVE_SYS_H_
#define _SRCR_SAVE_SAVE_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;
class FileSerializer;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class CharStat;
class SaveData;
class SysConf;

/*---------------------------------------------------------------------*//**
 *	セーブ システム
 *
**//*---------------------------------------------------------------------*/
class SaveSys
{
	//======================================================================
	// 定数
public:
	// 再開セーブデータのセーブ ID
	static const s32		SVDID_RESUME				= 999;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// セーブデータの取得
	inline SaveData* getSaveData() const		{	return _svd;			}
	// システムコンフィグの取得
	inline SysConf* getSysConf() const			{	return _sysconf;		}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool load(s32 svdid, bool isRestartCase);
	bool save(s32 svdid, u32 saveflags);
	void saveResumeAuto();
	bool readSaveData(s32 svdid, void* pdata, s32 size, bool *isPsnsUserData);

	bool loadSysConf();
	bool saveSysConf();

	bool newGame();
	bool restartGame(s32 svdid);
	bool debug_enterMap(s32 storyid);

	bool checkExistsResumeSaveFile() const;
	bool checkExistsSaveFile(s32 svdid) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SaveSys();
	~SaveSys();
	bool create();
	void destroy();
	void exec(const ExecCtx* ec);	// フレーム制御

private:
	bool openGameSaveFileRead(FileSerializer* fser, s32 svdid, bool *isPsnsUserData);
	bool openGameSaveFileWrite(FileSerializer* fser, s32 svdid);

	bool makeSaveFilePath(VcString* pathSaveFile, s32 svdid, bool enablePsnsUserData) const;

	//======================================================================
	// 変数
private:
	// 専用データ
	SaveData*				_svd;				// ゲームのセーブデータ
	SysConf*				_sysconf;			// システムコンフィグデータ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SAVE_SAVE_SYS_H_
