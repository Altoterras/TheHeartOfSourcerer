/***********************************************************************//**
 *	MaterialManager.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/07.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Self
#include "MaterialManager.h"

// Friends

// External
#include "../../tfw/collection/Array.h"
#include "../../tfw/downloader/DownloaderIos.h"
#include "../../tfw/file/File.h"
#include "../../tfw/lib/TfwTime.h"
#include "../../tfw/lib/XmlParser.h"
#include "../../tfw/lib/XmlParserUtils.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////
// 定数

ATAR_BEGIN_NS

#define FNAME_XML					"material.xml"

#define MSEC_TIMEOUT_XML			(10000)		// 10 秒
#define MSEC_TIMEOUT_PIC			(60000)		// 1 分
#define MSEC_TIMEOUT_MOV			(300000)	// 5 分

#define DISTANCE_THRESHOLD_DEFAULT	(45.0f)		// 一致距離のしきい値


////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MaterialManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	起動時ダウンロード開始
**//*---------------------------------------------------------------------*/
bool MaterialManager::startFirstDownload()
{
	_status = STATUS_DOWNLOADING;
	_step = STEP_FIRSTLOAD_XML_DOWNLOADING;
	_stateDownloaderPrev = Downloader::STATE_NULL;
	VcString urlNetworkFile(*_urlDownloadDirRef);
	urlNetworkFile.add(FNAME_XML);
	VcString pathLocalFile(*_pathTmp);
	pathLocalFile.add(FNAME_XML);
	TRACELN("{MaterialManager::startFirstDownload} %s -> %s", urlNetworkFile.getRaw(), pathLocalFile.getRaw());
	_downloader->registerFile(&pathLocalFile, &urlNetworkFile, MSEC_TIMEOUT_XML);
	return _downloader->startDownload();
}

/*---------------------------------------------------------------------*//**
	起動時ダウンロード開始
**//*---------------------------------------------------------------------*/
bool MaterialManager::startOneMovieDownload(s32 indexMaterial)
{
	const ArMaterialInfo* armi = getArMaterial(indexMaterial);
	if(armi == 0L)					{	return false;	}
	if(armi->_id->getLength() <= 0)	{	return false;	}
	_indexOneMovDlMaterial = indexMaterial;
	_status = STATUS_DOWNLOADING;
	_step = STEP_ONEMOVLOAD_MATERIAL_MOV_DOWNLOADING;
	_stateDownloaderPrev = Downloader::STATE_NULL;
	VcString filename(*armi->_id + FEXT_MOV);
	VcString urlNetworkFile(*_urlDownloadDirRef);
	urlNetworkFile.add(&filename);
	VcString pathLocalFile(*_pathTmp);
	pathLocalFile.add(&filename);
	TRACELN("{MaterialManager::startOneMovieDownload} %s -> %s", urlNetworkFile.getRaw(), pathLocalFile.getRaw());
	_downloader->registerFile(&pathLocalFile, &urlNetworkFile, MSEC_TIMEOUT_MOV);
	return _downloader->startDownload();
}

/*---------------------------------------------------------------------*//**
	指定素材の映像ファイルが Documents ディレクトリにあるかどうかを判定する
**//*---------------------------------------------------------------------*/
bool MaterialManager::isMovieFileExists(s32 indexMaterial)
{
	const ArMaterialInfo* armi = getArMaterial(indexMaterial);
	if(armi == 0L)					{	return false;	}
	if(armi->_id->getLength() <= 0)	{	return false;	}
	VcString cpathFile(*_pathDocs);
	cpathFile.add(armi->_id);
	cpathFile.add(FEXT_MOV);
	NSString* pathFile = [[NSString alloc] initWithCString:cpathFile.getRaw() encoding:NSUTF8StringEncoding];
	if(![[NSFileManager defaultManager] fileExistsAtPath:pathFile])
	{
		return false;
	}
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MaterialManager::MaterialManager()
	: _downloader(0L)
	, _arrDownloadDirUrl(0L)
	, _pathDocs(0L)
	, _pathTmp(0L)
	, _urlDownloadDirRef(0L)
	, _status(STATUS_NULL)
	, _step(STEP_NULL)
	, _stateDownloaderPrev(0)
	, _indexOneMovDlMaterial(-1)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MaterialManager::~MaterialManager()
{
	ASSERT(_downloader == 0L);
	ASSERT(_arrDownloadDirUrl == 0L);
	ASSERT(_pathDocs == 0L);
	ASSERT(_pathTmp == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MaterialManager::create(const CStringBase* pathDocs, const CStringBase* pathTmp)
{
	// ダウンローダの作成
	DownloaderIos* downloader = new DownloaderIos();
	if((downloader == 0L) || !downloader->create())
	{
		ASSERT(false);
		delete downloader;
		return false;
	}
	_downloader = downloader;
	
	// デフォルトのダウンロードディレクトリ URL 配列の作成
	_arrDownloadDirUrl = new Array<VcString*>(true, true);
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_01));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_02));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_03));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_04));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_05));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_06));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_07));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_08));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_09));
	_arrDownloadDirUrl->add(new VcString(URL_BASE_DEFAULT_10));
	for(u32 i = 0; i < _arrDownloadDirUrl->getCount(); i++)
	{
		NSString* dir = NSLocalizedString(@"server_dir", nil);
		_arrDownloadDirUrl->getAt(i)->add([dir cStringUsingEncoding:NSUTF8StringEncoding]);
		_arrDownloadDirUrl->getAt(i)->add(URL_SUB_DIR);
	}
	
	// パスの保存
	_pathDocs = new VcString(*pathDocs);
	_pathTmp = new VcString(*pathTmp);
	if(_pathDocs->getLength() < 1)	{	return false;	}
	if(_pathTmp->getLength() < 1)	{	return false;	}
	char c = _pathDocs->getAt(_pathDocs->getLength() - 1);
	if((c != '/') && (c != '\\'))	{	_pathDocs->add('/');	}
	c = _pathTmp->getAt(_pathTmp->getLength() - 1);
	if((c != '/') && (c != '\\'))	{	_pathTmp->add('/');		}
	
	// ドキュメントディレクトリのクリア
	#if defined(_DEBUG) && 0
	{
		NSFileManager* fm = [NSFileManager defaultManager];
		NSString* pathDir = [[NSString alloc] initWithCString:_pathDocs->getRaw() encoding:NSUTF8StringEncoding];
		for(NSString* filename in [fm contentsOfDirectoryAtPath:pathDir error:nil])
		{
			[fm removeItemAtPath:[pathDir stringByAppendingString:filename] error:nil];
		}
	}
	#endif

	// 現在の設定 XML を読む
	VcString pathDocsXmlFile = *_pathDocs + FNAME_XML;
	loadLocalXml(&_mtrliCur, &pathDocsXmlFile);
	#if defined(_DEBUG)
	{
		TfwTime ttime;
		TfwTime::convU64ToTime(&ttime, _mtrliCur._timevLastUpdate);
		TRACELN("{MaterialManager::create} current xml: %d/%d/%d %d:%d:%d", ttime._year, ttime._mon, ttime._mday, ttime._hour, ttime._min, ttime._sec);
	}
	#endif

	// ダウンロードディレクトリ URL の特定
	u32 idxUrl = ::rand() % _arrDownloadDirUrl->getCount();
	ASSERT(idxUrl < _arrDownloadDirUrl->getCount());
	_urlDownloadDirRef = _arrDownloadDirUrl->getAt(idxUrl);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MaterialManager::destroy()
{
	// パスの削除
	delete _pathDocs;
	_pathDocs = 0L;
	delete _pathTmp;
	_pathTmp = 0L;

	// ダウンロードディレクトリ URL 配列の削除
	delete _arrDownloadDirUrl;
	_arrDownloadDirUrl = 0L;

	// ダウンローダーの削除
	if(_downloader != 0L)
	{
		_downloader->destroy();
		delete _downloader;
		_downloader = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MaterialManager::exec(ExecRes* res, const ExecCtx* ec)
{
	s32	statDownloader = (s32)_downloader->getStatus();
	if(statDownloader != _stateDownloaderPrev)
	{
		switch(statDownloader)
		{
		case Downloader::STATE_SUCCEEDED:
			if(!doLoadingStep())
			{
				_status = STATUS_FAILED_DOWNDLOAD;
			}
			break;
		case Downloader::STATE_FAILED:
			_status = STATUS_FAILED_DOWNDLOAD;
			break;
		}
		_stateDownloaderPrev = statDownloader;
	}
}

/*---------------------------------------------------------------------*//**
	ローディングのステップ実行
**//*---------------------------------------------------------------------*/
bool MaterialManager::doLoadingStep()
{
	_downloader->clearRegisterFile();
	
	switch(_step)
	{
	case STEP_FIRSTLOAD_XML_DOWNLOADING:
		{
			VcString pathDownloadFile = *_pathTmp + FNAME_XML;
			if(!loadLocalXml(&_mtrliDownload, &pathDownloadFile))
			{
				TRACELN("{MaterialManager::doStep} load-xml failed.");
				return false;
			}
			#if 1
			{
				// 最初の素材のダウンロードへ
				if(!doNextMaterialLoading())
				{
					return false;
				}
			}
			#else
			{
				// 更新日時が新しいものがあれば素材データをダウンロードする
				if(_mtrliDownload._timevLastUpdate > _mtrliCur._timevLastUpdate)
				{
					// 最初の素材のダウンロードへ
					if(!doNextMaterialLoading())
					{
						return false;
					}
				}
				else	// 最新の XML なのでダウンロード処理を終了する
				{
					TRACELN("{MaterialManager::doStep} xml is latest.");
					[_delg releaseDownload];
					_status = STATUS_SUCCEEDED_DOWNDLOAD;
					return true;
				}
			}
			#endif
		}
		break;
	case STEP_FIRSTLOAD_MATERIAL_PIC_DOWNLOADING:
	case STEP_ONEMOVLOAD_MATERIAL_MOV_DOWNLOADING:
		if(!doNextMaterialLoading())
		{
			return false;
		}
		break;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	次の素材ローディングへ
**//*---------------------------------------------------------------------*/
bool MaterialManager::doNextMaterialLoading()
{
	switch(_step)
	{
	case STEP_FIRSTLOAD_XML_DOWNLOADING:
		{
			s32 cntDownloadFile = 0;
			for(int iarm = 0; iarm < NUM_AR_MAX; iarm++)
			{
				const VcString* id = _mtrliDownload._armi[iarm]._id;
				if(id->getLength() <= 0)
				{
					continue;	// ID 未定義
				}
				ArMaterialInfo* armiCur = _mtrliCur.findArMaterial(id);
				if((armiCur != 0L) && (_mtrliDownload._armi[iarm]._timevLastUpdate <= armiCur->_timevLastUpdate))
				{
					continue;	// 更新不要
				}
				
				// AR 認識画像のダウンロード登録
				VcString filename = *id + FEXT_PIC;
				VcString urlNetworkFile = *_urlDownloadDirRef + filename;
				VcString pathLocalFile = *_pathTmp + filename;
				_downloader->registerFile(&pathLocalFile, &urlNetworkFile, MSEC_TIMEOUT_PIC);
				#if DOWNLOAD_MOV_FIRSTLOAD
				{
					// 映像の登録
					filename = *id + FEXT_MOV;
					urlNetworkFile = *_urlDownloadDir + filename;
					pathLocalFile = *_pathTmp + filename;
					_downloader->registerFile(&pathLocalFile, &urlNetworkFile, MSEC_TIMEOUT_MOV);
				}
				#else
				{
					// ここで映像ファイルを Documents から削除しておく（遅延ダウンロード時は更新時間を見れず更新できなくなってしまうため）
					filename = *id + FEXT_MOV;
					pathLocalFile = *_pathDocs + filename;
					deleteFileIfExists(&pathLocalFile);
				}
				#endif
				cntDownloadFile++;
			}
			_step = STEP_FIRSTLOAD_MATERIAL_PIC_DOWNLOADING;
			if(cntDownloadFile > 0)
			{
				_downloader->startDownload();
				break;
			}
			// ダウンロードファイルがない場合はそのまま STEP_FIRSTLOAD_MATERIAL_PIC_DOWNLOADING へ
		}
		// break しない
	case STEP_FIRSTLOAD_MATERIAL_PIC_DOWNLOADING:
		{
			VcString pathTempFile, pathDocsFile, filename;
			// ファイルを一時ディレクトリからドキュメントディレクトリに移動
			pathTempFile = *_pathTmp + FNAME_XML;
			pathDocsFile = *_pathDocs + FNAME_XML;
			if(!moveFile(&pathDocsFile, &pathTempFile))
			{
				TRACELN("{MaterialManager::doNextMaterialDownload} move xml-file failed.");
				return false;
			}
			// 存在しなくなったファイルを削除する
			for(int iarm = 0; iarm < NUM_AR_MAX; iarm++)
			{
				const VcString* id = _mtrliCur._armi[iarm]._id;
				if(id->getLength() <= 0)
				{
					continue;	// ID 未定義
				}
				ArMaterialInfo* armiDownload = _mtrliDownload.findArMaterial(id);
				if(armiDownload != 0L)
				{
					continue;	// 削除不要
				}
				
				// 新しいリストには存在しないファイルは削除する
				filename = *id + FEXT_PIC;
				pathDocsFile = *_pathDocs + filename;
				deleteFileIfExists(&pathDocsFile);
				filename = *id + FEXT_MOV;
				pathDocsFile = *_pathDocs + filename;
				deleteFileIfExists(&pathDocsFile);
			}
			// ダウンロードされたファイルをドキュメントフォルダにコピーする
			for(int iarm = 0; iarm < NUM_AR_MAX; iarm++)
			{
				const VcString* id = _mtrliDownload._armi[iarm]._id;
				if(id->getLength() <= 0)
				{
					continue;	// ID 未定義
				}
				ArMaterialInfo* armiCur = _mtrliCur.findArMaterial(id);
				if((armiCur != 0L) && (_mtrliDownload._armi[iarm]._timevLastUpdate <= armiCur->_timevLastUpdate))
				{
					continue;	// 更新不要
				}
				
				// AR 認識画像の移動
				filename = *id + FEXT_PIC;
				pathTempFile = *_pathTmp + filename;
				pathDocsFile = *_pathDocs + filename;
				if(!moveFile(&pathDocsFile, &pathTempFile))
				{
					TRACELN("{MaterialManager::doNextMaterialDownload} move pic-file failed.");
					return false;
				}
				#if DOWNLOAD_MOV_FIRSTLOAD
				{
					// 映像の移動
					filename = *id + FEXT_MOV;
					pathTempFile = *_pathTmp + filename;
					pathDocsFile = *_pathDocs + filename;
					if(!moveFile(&pathDocsFile, &pathTempFile))
					{
						TRACELN("{MaterialManager::doNextMaterialDownload} move mov-file failed.");
						return false;
					}
				}
				#endif
			}
			
			// 素材情報を現在値にコピー
			_mtrliCur.copy(&_mtrliDownload);
			
			// 全て成功
			TRACE("{MaterialManager::doNextMaterialDownload} done download.");
			_status = STATUS_SUCCEEDED_DOWNDLOAD;
		}
		break;
	case STEP_ONEMOVLOAD_MATERIAL_MOV_DOWNLOADING:
		{
			if(_indexOneMovDlMaterial == -1)
			{
				return false;
			}
			const ArMaterialInfo* armi = getArMaterial(_indexOneMovDlMaterial);
			if(armi == 0L)
			{
				return false;
			}
			const VcString* id = armi->_id;
			if(id->getLength() <= 0)
			{
				return false;
			}
				
			// 映像の移動
			VcString filename = *id + FEXT_MOV;
			VcString pathTempFile = *_pathTmp + filename;
			VcString pathDocsFile = *_pathDocs + filename;
			if(!moveFile(&pathDocsFile, &pathTempFile))
			{
				return false;
			}
			
			// 全て成功
			TRACE("{MaterialManager::doNextMaterialDownload} done download.");
			_status = STATUS_SUCCEEDED_DOWNDLOAD;
		}
		break;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ローカルの XML ファイルを読み込む
**//*---------------------------------------------------------------------*/
bool MaterialManager::loadLocalXml(MaterialInfo* mtrli, const CStringBase* pathXml)
{
	// ファイルの存在確認
	if(!File::isFileExist(pathXml))	{	return false;	}
	// ファイルを開く
	File file(pathXml, File::MODE_READ);
	if(!file.isOpened())	{	return false;	}
	// ファイルを読み込む
	VcString strXml;
	file.readString(&strXml, file.getLength());
	// ファイルを閉じる
	file.close();

	VcString strWk;
	f32 fWk;
	
	// XML 解析
	XmlParser xmlp;
	const XmlNode* nodeRoot = xmlp.parseXmlDocument(&strXml);
	if(nodeRoot == 0L)	{	return false;	}
	
	if(!nodeRoot->getName()->equals("setting"))	{	return false;	}
	const XmlNode* nodeSetting = nodeRoot;

	// 最終更新日を得る
	if(XmlParserUtils::analyzeAtrb(&strWk, nodeSetting, "last_update"))
	{
		convTimeStringToValue(&mtrli->_timevLastUpdate, &strWk);
	}
	
	// AR 素材情報を得る
	const XmlNode* nodeArInfo = nodeRoot->findSiblingNode("ar_info");
	if(nodeArInfo == 0L)	{	return false;	}
	s32 idxAr = 0;
	const XmlNode* nodeAr = nodeArInfo->getChildNode();
	while(nodeAr != 0L)
	{
		if(XmlParserUtils::analyzeAtrb(&strWk, nodeAr, "id"))
		{
			mtrli->_armi[idxAr]._id->copy(&strWk);
		}
		if(XmlParserUtils::analyzeAtrb(&strWk, nodeAr, "last_update"))
		{
			convTimeStringToValue(&mtrli->_armi[idxAr]._timevLastUpdate, &strWk);
		}
		if(XmlParserUtils::analyzeAtrb(&strWk, nodeAr, "url"))
		{
			mtrli->_armi[idxAr]._url->copy(&strWk);
		}
		if(XmlParserUtils::analyzeAtrb(&fWk, nodeAr, "threshold"))
		{
			mtrli->_armi[idxAr]._distanceThreshold = fWk;
		}
	
		idxAr++;
		nodeAr = nodeAr->getSiblingNode();
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	時間文字列を数値に変換する
**//*---------------------------------------------------------------------*/
bool MaterialManager::convTimeStringToValue(u64* timev, const CStringBase* str)
{
	VcString strWk(*str);
	StringUtils::trim(&strWk);
	VcStringArray saYmdAndTime = StringUtils::split(&strWk, ' ');
	if(saYmdAndTime.getCount() < 2)	{	return false;	}
	VcStringArray saYmd = StringUtils::split(saYmdAndTime.get(0), '/');
	if(saYmd.getCount() < 3)		{	return false;	}
	VcStringArray saTime = StringUtils::split(saYmdAndTime.get(1), ':');
	if(saTime.getCount() < 3)		{	return false;	}
	TfwTime ttime;
	ttime._year = StringUtils::toInteger(saYmd.get(0));
	ttime._mon = StringUtils::toInteger(saYmd.get(1));
	ttime._mday = StringUtils::toInteger(saYmd.get(2));
	ttime._hour = StringUtils::toInteger(saTime.get(0));
	ttime._min = StringUtils::toInteger(saTime.get(1));
	ttime._sec = StringUtils::toInteger(saTime.get(2));
	TfwTime::convTimeToU64(timev, &ttime);
	
#if defined(_DEBUG) && 1
	TfwTime ttimeCheck;
	TfwTime::convU64ToTime(&ttimeCheck, *timev);
	ASSERT(ttime._year == ttimeCheck._year);
	ASSERT(ttime._mon == ttimeCheck._mon);
	ASSERT(ttime._mday == ttimeCheck._mday);
	ASSERT(ttime._hour == ttimeCheck._hour);
	ASSERT(ttime._min == ttimeCheck._min);
	ASSERT(ttime._sec == ttimeCheck._sec);
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルの移動（上書き）
**//*---------------------------------------------------------------------*/
bool MaterialManager::moveFile(const CStringBase* pathFileDst, const CStringBase* pathFileSrc)
{
	NSFileManager* fm = [NSFileManager defaultManager];
	NSError* error;
	
	NSString* pathDst = [[NSString alloc] initWithCString:pathFileDst->getRaw() encoding:NSUTF8StringEncoding];
	NSString* pathSrc = [[NSString alloc] initWithCString:pathFileSrc->getRaw() encoding:NSUTF8StringEncoding];
	
	// 既存ファイルの存在チェック
	if(![fm fileExistsAtPath:pathSrc])	// コピー元がない
	{
		TRACELN("{MaterialManager::moveFile} src-file no exists: %s", pathFileSrc->getRaw());
		return false;
	}
	if([fm fileExistsAtPath:pathDst])	// コピー先に既にファイルが存在している
	{
		if(![fm removeItemAtPath:pathDst error:&error])
		{
			TRACELN("{MaterialManager::moveFile} delete dst-file failed: %s", pathFileDst->getRaw());
			return false;
		}
	}
	
	// ファイルの移動
	if(![fm moveItemAtPath:pathSrc toPath:pathDst error:&error])
	{
		TRACELN("{MaterialManager::moveFile} move file failed: %s <- %s", pathFileDst->getRaw(), pathFileSrc->getRaw());
		return false;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルの削除
**//*---------------------------------------------------------------------*/
bool MaterialManager::deleteFileIfExists(const CStringBase* pathFile)
{
	NSFileManager* fm = [NSFileManager defaultManager];
	NSError* error;
	
	NSString* path = [[NSString alloc] initWithCString:pathFile->getRaw() encoding:NSUTF8StringEncoding];
	
	// 古いファイルの削除
	if([fm fileExistsAtPath:path])	// 古いファイルが存在している
	{
		if(![fm removeItemAtPath:path error:&error])
		{
			TRACELN("{MaterialManager::deleteFile} delete file failed: %s", pathFile->getRaw());
			return false;
		}
	}
	
	return true;
}

//==========================================================================
// MaterialManager::ArMaterialInfo メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MaterialManager::ArMaterialInfo::ArMaterialInfo()
	: _timevLastUpdate(0)
	, _id(new VcString())
	, _url(new VcString())
	, _distanceThreshold(DISTANCE_THRESHOLD_DEFAULT)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MaterialManager::ArMaterialInfo::~ArMaterialInfo()
{
	delete _id;
	delete _url;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void MaterialManager::ArMaterialInfo::copy(const MaterialManager::ArMaterialInfo* src)
{
	_timevLastUpdate = src->_timevLastUpdate;
	_id->copy(src->_id);
	_url->copy(src->_url);
	_distanceThreshold = src->_distanceThreshold;
}

//==========================================================================
// MaterialManager::MaterialInfo メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MaterialManager::MaterialInfo::MaterialInfo()
	: _timevLastUpdate(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MaterialManager::MaterialInfo::~MaterialInfo()
{
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void MaterialManager::MaterialInfo::copy(const MaterialManager::MaterialInfo* src)
{
	_timevLastUpdate = src->_timevLastUpdate;
	for(int i = 0; i < NUM_AR_MAX; i++)
	{
		_armi[i].copy(&src->_armi[i]);
	}
}

/*---------------------------------------------------------------------*//**
	ID による AR 素材検索
**//*---------------------------------------------------------------------*/
MaterialManager::ArMaterialInfo* MaterialManager::MaterialInfo::findArMaterial(const VcString* id)
{
	for(int i = 0; i < NUM_AR_MAX; i++)
	{
		if(_armi[i]._id->equals(id))
		{
			return &_armi[i];
		}
	}
	return 0L;
}

ATAR_END_NS