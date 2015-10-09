/***********************************************************************//**
 *	DownloaderIos.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/06/17.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_DOWNLOADER_DOWNLOADER_IOS_H_
#define _TFW_DOWNLOADER_DOWNLOADER_IOS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Downloader.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class DownloaderIosInner;

/*---------------------------------------------------------------------*//**
 *	IOS 用ダウンローダー クラス
 *		※※※ 要 ARC(Automatic Reference Counting)　※※※
 *
**//*---------------------------------------------------------------------*/
class DownloaderIos : public Downloader
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	DownloaderIos();
	virtual ~DownloaderIos();
	virtual bool create();
	virtual void destroy();
	void onDoneDownload(bool succeeded, s32 idxFileInfo);

protected:
	virtual bool startDownloadFile(s32 idxFileInfo);
	virtual void doneAllDownloadFile();
	virtual void terminateAllDownloadFile();
	virtual void failedAllDownloadFile();

	//======================================================================
	// 変数
protected:
	DownloaderIosInner* _inner;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_DOWNLOADER_DOWNLOADER_IOS_H_

