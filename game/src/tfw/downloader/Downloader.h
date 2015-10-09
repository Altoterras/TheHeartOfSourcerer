/***********************************************************************//**
 *	Downloader.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/06/17.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_DOWNLOADER_DOWNLOADER_H_
#define _TFW_DOWNLOADER_DOWNLOADER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Array;
class VcString;

/*---------------------------------------------------------------------*//**
 *	ダウンローダー基底クラス
 *	
**//*---------------------------------------------------------------------*/
class Downloader
{
	//======================================================================
	// 定数
public:
	enum State
	{
		STATE_NULL,
		STATE_STABLE,
		STATE_DOWNLOADING,
		STATE_PAUSED,
		STATE_CANCELED,
		STATE_FAILED,
		STATE_SUCCEEDED,
	};
	
	//======================================================================
	// 型定義
public:
	typedef void (* ChangeStatusCallback)(State stat);
	
	//======================================================================
	// クラス
protected:
	class FileInfo
	{
	public:
		enum Phase
		{
			PHASE_NULL,
			PHASE_QUEREING,
			PHASE_DOWNLOADING,
			PAHSE_DONE,
		};
	
		Phase _phase;
		VcString* _pathLocalFile;
		VcString* _urlNetworkFile;
		s32 _msecTimeout;
		
		FileInfo();
		~FileInfo();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 状態を得る
	inline State getStatus() const { return _stat; }
	// （登録された）ファイル数を得る
	s32 getFileNum() const;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool registerFile(const CStringBase* pathLocalFile, const CStringBase* urlNetworkFile, s32 msecTimeout);
	virtual void clearRegisterFile();
	virtual bool startDownload();
	virtual void pauseDownload();
	virtual void cancelDownload();
	virtual void setChangeStatusCallback(ChangeStatusCallback cbStat) { _cbChStat = cbStat; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Downloader();
	virtual ~Downloader();
	virtual bool create();
	virtual void destroy();
	
protected:
	FileInfo* getFileInfo(s32 idxFileInfo);
	virtual void setStatus(State stat);
	virtual bool startDownloadFile(s32 idxFileInfo) = 0;
	virtual bool resumeDownload();
	virtual bool doneAndNextDownloadFile(s32 idxFileInfoDone);
	virtual bool nextDownloadFile();
	virtual void doneAllDownloadFile();
	virtual void terminateAllDownloadFile();
	virtual void failedAllDownloadFile();

	//======================================================================
	// 変数
private:
	State _stat;
	Array<FileInfo*>* _arrFiles;	// ファイル情報配列
	s32 _cntDownloading;
	ChangeStatusCallback _cbChStat;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_DOWNLOADER_DOWNLOADER_H_

