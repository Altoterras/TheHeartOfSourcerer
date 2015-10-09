/***********************************************************************//**
 *	Downloader.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/06/17.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Downloader.h"

// Friends
#include "../collection/Array.h"
#include "../string/VcString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Downloader メソッド

/*---------------------------------------------------------------------*//**
	（登録された）ファイル数を得る
**//*---------------------------------------------------------------------*/
s32 Downloader::getFileNum() const
{
	return _arrFiles->getCount();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ダウンロードするファイルを登録する
**//*---------------------------------------------------------------------*/
bool Downloader::registerFile(const CStringBase* pathLocalFile, const CStringBase* urlNetworkFile, s32 msecTimeout)
{
	FileInfo* fi = new FileInfo();
	fi->_pathLocalFile->copy(pathLocalFile);
	fi->_urlNetworkFile->copy(urlNetworkFile);
	fi->_msecTimeout = msecTimeout;
	_arrFiles->add(fi);
	return true;
}

/*---------------------------------------------------------------------*//**
	登録ファイルをクリアする
**//*---------------------------------------------------------------------*/
void Downloader::clearRegisterFile()
{
	_arrFiles->removeAll();
}

/*---------------------------------------------------------------------*//**
	ダウンロードを開始する
**//*---------------------------------------------------------------------*/
bool Downloader::startDownload()
{
	// 実行中の場合は一度停止
	switch(_stat)
	{
	case STATE_DOWNLOADING:
		terminateAllDownloadFile();
		break;
	case STATE_PAUSED:
		return resumeDownload();	// 再開
	}
	
	// ファイルがない場合は即終了
	if(_arrFiles->getCount() <= 0)
	{
		setStatus(STATE_SUCCEEDED);
		return true;	// 失敗ではない、とする
	}

	// 全てのファイルをキュー状態に
	for(int i = 0; i < _arrFiles->getCount(); i++)
	{
		FileInfo* fi = _arrFiles->getAt(i);
		fi->_phase = FileInfo::PHASE_QUEREING;
	}
	
	// インデックス 0 からダウンロード開始
	_cntDownloading = 0;
	if(!startDownloadFile(_cntDownloading))
	{
		return false;
	}

	setStatus(STATE_DOWNLOADING);
	return true;
}

/*---------------------------------------------------------------------*//**
	ダウンロードを一時停止する
**//*---------------------------------------------------------------------*/
void Downloader::pauseDownload()
{
	setStatus(STATE_PAUSED);
	
	// デフォルトの実装では、ダウンロード中のファイルには手を付けない
}

/*---------------------------------------------------------------------*//**
	ダウンロードをキャンセルする
**//*---------------------------------------------------------------------*/
void Downloader::cancelDownload()
{
	if((_stat != STATE_STABLE) && (_stat != STATE_SUCCEEDED))
	{
		terminateAllDownloadFile();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Downloader::Downloader()
	: _stat(STATE_NULL)
	, _arrFiles(0L)
	, _cntDownloading(0)
	, _cbChStat(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Downloader::~Downloader()
{
	ASSERT(_arrFiles == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Downloader::create()
{
	// ファイル情報配列の作成
	_arrFiles = new Array<FileInfo*>(true, true);

	// 使用可能な状態に
	setStatus(STATE_STABLE);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Downloader::destroy()
{
	// ファイル情報配列の削除
	delete _arrFiles;
	_arrFiles = 0L;
}

/*---------------------------------------------------------------------*//**
	ファイル情報を得る
**//*---------------------------------------------------------------------*/
Downloader::FileInfo* Downloader::getFileInfo(s32 idxFileInfo)
{
	if((idxFileInfo < 0) || (_arrFiles->getCount() <= idxFileInfo))
	{
		return 0L;
	}
	return _arrFiles->getAt(idxFileInfo);
}

/*---------------------------------------------------------------------*//**
	ステータス変更
**//*---------------------------------------------------------------------*/
void Downloader::setStatus(State stat)
{
	_stat = stat;
	if(_cbChStat != 0L)	{	_cbChStat(_stat);	}
}

/*---------------------------------------------------------------------*//**
	ファイルダウンロード開始
**//*---------------------------------------------------------------------*/
bool Downloader::startDownloadFile(s32 idxFileInfo)
{
	if((idxFileInfo < 0) || (_arrFiles->getCount() <= idxFileInfo))
	{
		failedAllDownloadFile();
		return false;
	}
	
	// ファイルをダウンロード中へ変更
	FileInfo* fi = _arrFiles->getAt(idxFileInfo);
	fi->_phase = FileInfo::PHASE_DOWNLOADING;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルダウンロードを再開する
**//*---------------------------------------------------------------------*/
bool Downloader::resumeDownload()
{
	// 既にダウンロード中のものがあればそのまま抜ける
	for(int i = 0; i < _arrFiles->getCount(); i++)
	{
		const FileInfo* fi = _arrFiles->getAt(i);
		if(fi->_phase == FileInfo::PHASE_DOWNLOADING)
		{
			return true;
		}
	}

	// 次のファイルへ
	if(!nextDownloadFile())
	{
		return false;
	}
	
	setStatus(STATE_DOWNLOADING);
	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルのダウンロードが完了し、次のファイルをダウンロードする
**//*---------------------------------------------------------------------*/
bool Downloader::doneAndNextDownloadFile(s32 idxFileInfoDone)
{
	if((idxFileInfoDone < 0) || (_arrFiles->getCount() <= idxFileInfoDone))
	{
		failedAllDownloadFile();
		return false;
	}

	// ファイルをダウンロード済みへ変更
	FileInfo* fi = _arrFiles->getAt(idxFileInfoDone);
	fi->_phase = FileInfo::PAHSE_DONE;
	
	// 次のファイルへ
	return nextDownloadFile();
}

/*---------------------------------------------------------------------*//**
	次のファイルをダウンロードする
**//*---------------------------------------------------------------------*/
bool Downloader::nextDownloadFile()
{
	_cntDownloading++;
	if(_cntDownloading < _arrFiles->getCount())
	{
		// ポーズ中は次ぎにいかない
		if(_stat == STATE_PAUSED)
		{
			return true;
		}
	
		if(!startDownloadFile(_cntDownloading))
		{
			failedAllDownloadFile();
			return false;
		}
	}
	else
	{
		// 全てのファイルをダウンロード完了
		doneAllDownloadFile();
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	全てのファイルがダウンロード完了した
**//*---------------------------------------------------------------------*/
void Downloader::doneAllDownloadFile()
{
	setStatus(STATE_SUCCEEDED);
}

/*---------------------------------------------------------------------*//**
	全てのファイルのダウンロードを強制終了する
**//*---------------------------------------------------------------------*/
void Downloader::terminateAllDownloadFile()
{
	setStatus(STATE_CANCELED);
}

/*---------------------------------------------------------------------*//**
	ファイルダウンロードが失敗した
**//*---------------------------------------------------------------------*/
void Downloader::failedAllDownloadFile()
{
	setStatus(STATE_FAILED);
}

//==========================================================================
// Downloader メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Downloader::FileInfo::FileInfo()
	: _phase(PHASE_NULL)
	, _pathLocalFile(new VcString())
	, _urlNetworkFile(new VcString())
	, _msecTimeout(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Downloader::FileInfo::~FileInfo()
{
	delete _pathLocalFile;
	delete _urlNetworkFile;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

