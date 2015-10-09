/***********************************************************************//**
 *	SaveSys.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SaveSys.h"

// Friends
#include "SaveData.h"
#include "SysConf.h"
#include "../event/EventSys.h"
#include "../body/Game.h"
#include "../body/GameNotifier.h"
#include "../mode/GameMode.h"

// External
#include "../../etk/body/OsDepMod.h"
#include "../../tfw/file/File.h"
#include "../../tfw/file/FileSerializer.h"
#include "../../tfw/psns/PsnsBase.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define SIZE_SERIAL_GENEDAT	(1 + (NUM_SID_F / 8) + (NUM_SID_C * 2) + (NUM_SID_SN * 2) + (NUM_SID_N * 4))

// ファイル名
#if defined(_ENLIGHT_V01)
	#define FNAME_GAME_RESUME	"sourcerer01_save_resume%s.dat"
	#define FNAME_GAME_FID		"sourcerer01_save_f%02d%s.dat"
#elif defined(_ENLIGHT_V02)
	#define FNAME_GAME_RESUME	"sourcerer02_save_resume%s.dat"
	#define FNAME_GAME_FID		"sourcerer02_save_f%02d%s.dat"
#else
	#define FNAME_GAME_RESUME	"enlight_save_resume%s.dat"
	#define FNAME_GAME_FID		"enlight_save_f%02d%s.dat"
#endif
#define FNAME_SYSCONF		"sysconf.dat"

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SaveSys メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ロード
**//*---------------------------------------------------------------------*/
bool SaveSys::load(s32 svdid, bool isRestartCase)
{
	bool isPsnsUserData = false;

	// ファイルを開く
	FileSerializer fser;
	if(!openGameSaveFileRead(&fser, svdid, &isPsnsUserData))
	{
		return false;
	}

	// リスタートフラグを立てる
	if(isRestartCase)
	{
		fser.setGeneralFlags(SaveData::SRF_RESTARTING, true);
	}

	// セーブデータをデシリアライズ
	if(!_svd->deserialize(&fser))
	{
		return false;
	}

	// 各モジュールに通知
	Game::getGame()->getGameNotifier()->notifyDeserialize();

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブ
**//*---------------------------------------------------------------------*/
bool SaveSys::save(s32 svdid, u32 saveflags)
{
	TRACE("{SaveSys::save}  svdid=%d, saveflags=%d\n",  svdid, saveflags);

	// システムコンフィグの保存
	_sysconf->collectDataForSave();
	if(_sysconf->isChanged())	// 変更があるときのみ
	{
		saveSysConf();
	}

	// ファイルを開く
	FileSerializer fser;
	if(!openGameSaveFileWrite(&fser, svdid))
	{
		return false;
	}

	// セーブデータをシリアライズ
	_svd->setSaveFlags(saveflags);
	return _svd->serialize(&fser);
}

/*---------------------------------------------------------------------*//**
	再開用の自動保存
**//*---------------------------------------------------------------------*/
void SaveSys::saveResumeAuto()
{
	GameMode::ModeKind gmkind = Game::getGame()->getGameMode()->getChildEndModeKind();
	if(gmkind == GameMode::MODE_GAME_INGAME_WALKING_NORMAL)
	{
		if(!Game::getGame()->getEventSys()->isBegunEvent())
		{
			save(SVDID_RESUME, 0);		// 中断データの自動保存
		}
	}
}

/*---------------------------------------------------------------------*//**
	任意サイズ分のセーブデータを読み込む
**//*---------------------------------------------------------------------*/
bool SaveSys::readSaveData(s32 svdid, void* pdata, s32 size, bool *isPsnsUserData)
{
	// ファイルを開く
	FileSerializer fser;
	if(!openGameSaveFileRead(&fser, svdid, isPsnsUserData))
	{
		return false;
	}

	// データを読み込む
	return fser.restore(pdata, size);
}

/*---------------------------------------------------------------------*//**
	システムコンフィグをロードする
**//*---------------------------------------------------------------------*/
bool SaveSys::loadSysConf()
{
	// ファイルパス
	VcString path;
	if(!Game::getGame()->getOsDepMod()->getDocumentDirPath(&path, true))	{	return false;	}
	path += FNAME_SYSCONF;
	TRACE("{SaveSys::loadSysConf} conf-path: %s\n", path.getRaw());

	// ファイルの存在確認
	if(!File::isFileExist(&path))	{	return false;	}

	// ファイルを開く
	File file(&path, File::MODE_READ);
	if(!file.isOpened())	{	return false;	}

	// データを読み込む
	_sysconf->load(&file);

	// ファイルを閉じる
	file.close();

	return true;
}

/*---------------------------------------------------------------------*//**
	システムコンフィグをセーブする
**//*---------------------------------------------------------------------*/
bool SaveSys::saveSysConf()
{
	// ファイルパス
	VcString path;
	if(!Game::getGame()->getOsDepMod()->getDocumentDirPath(&path, true))	{	return false;	}
	path += FNAME_SYSCONF;
	TRACE("{SaveSys::saveSysConf} conf-path: %s\n", path.getRaw());

	// ファイルを開く
	File file(&path, File::MODE_WRITE);
	if(!file.isOpened())	{	return false;	}

	// データを保存
	_sysconf->collectDataForSave();
	_sysconf->save(&file);

	// ファイルを閉じる
	file.close();

	return true;
}

/*---------------------------------------------------------------------*//**
	新規ゲーム
**//*---------------------------------------------------------------------*/
bool SaveSys::newGame()
{
	return _svd->newGame();
}

/*---------------------------------------------------------------------*//**
	ストーリーをはじめからやり直す
**//*---------------------------------------------------------------------*/
bool SaveSys::restartGame(s32 svdid)
{
	if(!load(svdid, true))
	{
		return false;
	}
	return _svd->restartGame();
}

/*---------------------------------------------------------------------*//**
	マップに直接入る（デバッグ用）
**//*---------------------------------------------------------------------*/
bool SaveSys::debug_enterMap(s32 storyid)
{
#if defined(_DEBUG)
	return _svd->debug_enterMap(storyid);
#else
	return true;
#endif
}

/*---------------------------------------------------------------------*//**
	再開ファイルの存在チェック
**//*---------------------------------------------------------------------*/
bool SaveSys::checkExistsResumeSaveFile() const
{
	VcString pathSaveFile;
	if(makeSaveFilePath(&pathSaveFile, SVDID_RESUME, true) && File::isFileExist(&pathSaveFile))
	{
		return true;
	}
	if(makeSaveFilePath(&pathSaveFile, SVDID_RESUME, false) && File::isFileExist(&pathSaveFile))
	{
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	セーブファイルの存在チェック
**//*---------------------------------------------------------------------*/
bool SaveSys::checkExistsSaveFile(s32 svdid) const
{
	VcString pathSaveFile;
	if(makeSaveFilePath(&pathSaveFile, svdid, true) && File::isFileExist(&pathSaveFile))
	{
		return true;
	}
	if(makeSaveFilePath(&pathSaveFile, svdid, false) && File::isFileExist(&pathSaveFile))
	{
		return true;
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SaveSys::SaveSys()
	: _svd(0L)
	, _sysconf(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SaveSys::~SaveSys()
{
	ASSERT(_svd == 0L);
	ASSERT(_sysconf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SaveSys::create()
{
	// セーブデータの作成
	_svd = new SaveData();
	if(!_svd->create())
	{
		delete _svd;
		_svd = 0L;
		return false;
	}

	// システムコンフィグデータの作成
	_sysconf = new SysConf();
	if(!_sysconf->create())
	{
		delete _sysconf;
		_sysconf = 0L;
		return false;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SaveSys::destroy()
{
	// システムコンフィグデータの削除
	if(_sysconf != 0L)
	{
		_sysconf->destroy();
		delete _sysconf;
		_sysconf = 0L;
	}

	// セーブデータの削除
	if(_svd != 0L)
	{
		_svd->destroy();
		delete _svd;
		_svd = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SaveSys::exec(const ExecCtx* ec)
{
	if(_svd != 0L)
	{
		_svd->exec(ec);
	}
}

/*---------------------------------------------------------------------*//**
	読み込み用ゲームセーブファイルを開く
**//*---------------------------------------------------------------------*/
bool SaveSys::openGameSaveFileRead(FileSerializer* fser, s32 svdid, bool *isPsnsUserData)
{
	// セーブファイルパスの作成と存在チェック
	VcString pathSaveFile;
	if(makeSaveFilePath(&pathSaveFile, svdid, true) && File::isFileExist(&pathSaveFile))
	{
		*isPsnsUserData = true;
	}
	else if(makeSaveFilePath(&pathSaveFile, svdid, false) && File::isFileExist(&pathSaveFile))
	{
		*isPsnsUserData = false;
	}
	else
	{
		return false;
	}

	// ファイルを開く
	File* file = new File(&pathSaveFile, File::MODE_READ);
	if(!file->isOpened())
	{
		delete file;
		return false;
	}

	// シリアライザにファイルを設定
	fser->setFile(file, true);	// ファイルオブジェクト削除の委譲

	// シリアライザヘッダの読み込み
	fser->restoreHeader();
	u32 mjverDat = (fser->getMajorVersionNumber() << 16) + fser->getMinorVersionNumber();
	u32 mjverSys = (_svd->getMajorVersionNumber() << 16) + _svd->getMinorVersionNumber();
	if(mjverDat > mjverSys)
	{
		//ASSERTM(false, "{SaveSys::load} Invalid Save-Data Version.");
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	書き込み用ゲームセーブファイルを開く
**//*---------------------------------------------------------------------*/
bool SaveSys::openGameSaveFileWrite(FileSerializer* fser, s32 svdid)
{
	// セーブ可能かどうかを調べる
	if(!_svd->isSerializableNow())
	{
		return false;
	}

	// セーブファイルパスの作成と存在チェック
	VcString pathSaveFile;
	if(!makeSaveFilePath(&pathSaveFile, svdid, true))
	{
		if(!makeSaveFilePath(&pathSaveFile, svdid, false))
		{
			return false;
		}
	}

	// ファイルを開く
	File* file = new File(&pathSaveFile, File::MODE_WRITE);
	if(!file->isOpened())
	{
		delete file;
		return false;
	}

	// シリアライザにファイルを設定
	fser->setFile(file, true);	// ファイルオブジェクト削除の委譲

	// シリアライザヘッダの書き込み
	fser->setupHeader(_svd->getMajorVersionNumber(), _svd->getMinorVersionNumber(), true);	// ストラクチャモード
	if(!fser->storeHeader())
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブファイルのパスを作成する
**//*---------------------------------------------------------------------*/
bool SaveSys::makeSaveFilePath(VcString* pathSaveFile, s32 svdid, bool enablePsnsUserData) const
{
	// PSNS ユーザー ID を得る
	VcString idstr;
	if(enablePsnsUserData)
	{
		if(Game::getGame()->getPsns()->getCurrentPlayerId(&idstr))
		{
			idstr.insert(0, '_');
		}
		else
		{
			return false;
		}
	}

	// ファイルパスを作成
	if(!Game::getGame()->getOsDepMod()->getDocumentDirPath(pathSaveFile, false))
	{
		return false;
	}
	if(svdid == SVDID_RESUME)
	{
		*pathSaveFile += VcString::format(FNAME_GAME_RESUME, idstr.getRaw());
	}
	else
	{
		*pathSaveFile += VcString::format(FNAME_GAME_FID, svdid, idstr.getRaw());
	}
	TRACE("{SaveSys::makeSaveFilePath} save-data-path: %s\n", pathSaveFile->getRaw());

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
