/***********************************************************************//**
 *	MaterialManager.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/07.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_MATERIAL_STORAGE_H_
#define _ATAR_BODY_MATERIAL_STORAGE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExec.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#define DOWNLOAD_MOV_FIRSTLOAD	(0)	// 起動時ダウンロードで映像もダウンロードする

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Array;
class DownloaderIos;

TFW_END_NS

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	素材管理
 *		※ 要 ARC(Automatic Reference Counting)
 *
**//*---------------------------------------------------------------------*/
class MaterialManager : public FrameExec
{
	//======================================================================
	// 定数
public:
	// 状態
	enum Status
	{
		STATUS_NULL,
		STATUS_DOWNLOADING,
		STATUS_FAILED_DOWNDLOAD,
		STATUS_SUCCEEDED_DOWNDLOAD,
	};

	// ファイル拡張し
	#define FEXT_PIC		".png"
	#define FEXT_MOV		".mp4"
	
private:
	// AR 素材の最大値
	static const s32 NUM_AR_MAX = 16;

	// ステップ
	enum Step
	{
		STEP_NULL,
		STEP_FIRSTLOAD_XML_DOWNLOADING,
		STEP_FIRSTLOAD_MATERIAL_PIC_DOWNLOADING,
		STEP_ONEMOVLOAD_MATERIAL_MOV_DOWNLOADING,
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	AR 素材情報
	 *
	**//*-----------------------------------------------------------------*/
	class ArMaterialInfo
	{
	public:
		u64 _timevLastUpdate;
		VcString* _id;
		VcString* _url;
		f32 _distanceThreshold;

		ArMaterialInfo();
		~ArMaterialInfo();
		void copy(const ArMaterialInfo* src);
	};
	
private:
	/*-----------------------------------------------------------------*//**
	 *	素材情報
	 *
	**//*-----------------------------------------------------------------*/
	class MaterialInfo
	{
	public:
		u64 _timevLastUpdate;
		ArMaterialInfo _armi[NUM_AR_MAX];
		
		MaterialInfo();
		~MaterialInfo();
		void copy(const MaterialInfo* src);
		ArMaterialInfo* findArMaterial(const VcString* id);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	// 状態を得る
	inline Status getStatus() const { return _status; }
	
	// 最大 AR 素材数
	inline s32 getArMaterialMaxNum() const { return NUM_AR_MAX; }
	// AR 素材を得る
	inline const ArMaterialInfo* getArMaterial(s32 index) const { return ((0 <= index) && (index < NUM_AR_MAX)) ? &_mtrliCur._armi[index] : 0L; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool startFirstDownload();
	bool startOneMovieDownload(s32 indexMaterial);
	bool isMovieFileExists(s32 indexMaterial);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MaterialManager();
	~MaterialManager();
	bool create(const CStringBase* pathDocs, const CStringBase* pathTmp);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	
private:
	bool doLoadingStep();
	bool doNextMaterialLoading();
	bool loadLocalXml(MaterialInfo* mtrli, const CStringBase* pathXml);
	static bool convTimeStringToValue(u64* timev, const CStringBase* str);
	static bool moveFile(const CStringBase* pathFileDst, const CStringBase* pathFileSrc);
	static bool deleteFileIfExists(const CStringBase* pathFile);

	//======================================================================
	// 変数
private:
	DownloaderIos* _downloader;
	Array<VcString*>* _arrDownloadDirUrl;
	VcString* _pathDocs;
	VcString* _pathTmp;
	const VcString* _urlDownloadDirRef;
	Status _status;
	Step _step;
	MaterialInfo _mtrliCur;
	MaterialInfo _mtrliDownload;
	s32 _stateDownloaderPrev;
	s32 _indexOneMovDlMaterial;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_MATERIAL_STORAGE_H_
