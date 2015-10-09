/***********************************************************************//**
 *  ModelLoader_tbf.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/07.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ModelLoader_tbf.h"

// Friends
#include "ModelLoader.h"
#include "ShapeModel.h"
#include "BillboardModel.h"
#include "../../../Shape.h"
#include "../../../Material.h"
#include "../../../HrAnim.h"
#include "../../../HrAnimSet.h"
#include "../../../HrAnimObj.h"
#include "../../../HrAnimObjFrame.h"
#include "../../../CamAnim.h"
#include "../../../CamAnimFrame.h"
#include "../../../../Tfw.h"
#include "../../../../collection/List.h"
#include "../../../../collection/Tree.h"
#include "../../../../collision/Gcol.h"
#include "../../../../common/ResourcePool.h"
#include "../../../../file/File.h"
#if defined(_ANDROID)
#include "../../../../file/FileAndroid.h"
#endif	// #if defined(_ANDROID)
#include "../../../../file/IdFile.h"
#include "../../../../gcmn/Texture.h"
#include "../../../../gcmn/TexAnim.h"
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"
#include "../../../../lib/TmpBuf.h"
#include "../../../../string/VcString.h"
#include "../../../../string/CcString.h"
#include "../../../../string/StringUtils.h"
#include "../../../../tbf/TbfDefine.h"
#include "../../../../tbf/TbmbManip.h"			// for test

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#if 0
#define TLOG	TRACE
#else
#define	TLOG	//
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ModelLoader_tbf メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ModelLoader_tbf::ModelLoader_tbf(Tfw* tfwRef)
	: _tfwRef(tfwRef)
	, _fileTbf(0L)
	, _tarrTex(0L)
	, _numTex(0)
	, _usedTex(0L)
	, _taarrTexAnim(0L)
	, _numTexAnim(0)
	, _marrMtrl(0L)
	, _numMtrl(0)
	, _sarrShape(0L)
	, _siarrShexi(0L)
	, _numShape(0)
	, _haobjTree(0L)
	, _hoiHaoexi(0L)
	, _numHrAnimObjTreeTop(0)
	, _numHrAnimObjTotal(0)
	, _haarrHrAnim(0L)
	, _haiarrHaexi(0L)
	, _numHrAnim(0)
	, _hasarrHaset(0L)
	, _hasiarrHasexi(0L)
	, _numHaset(0)
	, _camanim(0L)
	, _garrGcol(0L)
	, _numGcol(0)
	, _marrMdl(0L)
	, _numMdl(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ModelLoader_tbf::~ModelLoader_tbf()
{
	// 専用の変数を削除する
	if(_fileTbf != 0L)
	{
		delete _fileTbf;
	}
	if(_usedTex != 0L)
	{	
		delete[] _usedTex;
	}
	if(_siarrShexi != 0L)
	{
		for(int i = 0; i < _numShape; i++)			{	delete _siarrShexi[i];		}
		delete[] _siarrShexi;
	}
	if(_haiarrHaexi != 0L)
	{
		for(int i = 0; i < _numHrAnim; i++)			{	delete _haiarrHaexi[i];		}
		delete[] _haiarrHaexi;
	}
	if(_hoiHaoexi != 0L)
	{
		for(int i = 0; i < _numHrAnimObjTotal; i++)	{	delete _hoiHaoexi[i];		}
		delete[] _hoiHaoexi;
	}
	if(_hasiarrHasexi != 0L)
	{
		for(int i = 0; i < _numHaset; i++)			{	delete _hasiarrHasexi[i];	}
		delete[] _hasiarrHasexi;
	}

	// モデルに管理を委譲しなかった場合（モデルの読み込みに失敗した場合等）のインスタンスを削除する
	if(_tarrTex != 0L)
	{
		for(int i = 0; i < _numTex; i++)				{	if(_tarrTex[i] != 0L)		{	_tarrTex[i]->destroy();			delete _tarrTex[i];			}	}
		delete[] _tarrTex;
	}
	if(_taarrTexAnim != 0L)
	{
		for(int i = 0; i < _numTexAnim; i++)			{	if(_taarrTexAnim[i] != 0L)	{	_taarrTexAnim[i]->destroy();	delete _taarrTexAnim[i];	}	}
		delete[] _taarrTexAnim;
	}
	if(_marrMtrl != 0L)
	{
		for(int i = 0; i < _numMtrl; i++)				{	if(_marrMtrl[i] != 0L)		{	_marrMtrl[i]->destroy();		delete _marrMtrl[i];		}	}
		delete[] _marrMtrl;
	}
	if(_sarrShape != 0L)
	{
		for(int i = 0; i < _numShape; i++)				{	if(_sarrShape[i] != 0L)		{	_sarrShape[i]->destroy();		delete _sarrShape[i];		}	}
		delete[] _sarrShape;
	}
	if(_haobjTree != 0L)
	{
		for(int i = 0; i < _numHrAnimObjTreeTop; i++)	{	if(_haobjTree[i] != 0L)		{	_haobjTree[i]->destroy();		delete _haobjTree[i];		}	}
		delete[] _haobjTree;
	}
	if(_haarrHrAnim != 0L)
	{
		for(int i = 0; i < _numHrAnim; i++)				{	if(_haarrHrAnim[i] != 0L)	{	_haarrHrAnim[i]->destroy();		delete _haarrHrAnim[i];		}	}
		delete[] _haarrHrAnim;
	}
	if(_hasarrHaset != 0L)
	{
		for(int i = 0; i < _numHaset; i++)				{	if(_hasarrHaset[i] != 0L)	{	_hasarrHaset[i]->destroy();		delete _hasarrHaset[i];		}	}
		delete[] _hasarrHaset;
	}
	if(_camanim != 0L)
	{	
		if(_camanim != 0L)								{	_camanim->destroy();			delete _camanim;			}
	}
	if(_garrGcol != 0L)
	{
		for(int i = 0; i < _numGcol; i++)				{	if(_garrGcol[i] != 0L)		{	_garrGcol[i]->destroy();		delete _garrGcol[i];		}	}
		delete[] _garrGcol;
	}

	// モデルポインタ配列を削除する（モデルインスタンスは削除しない）
	delete[] _marrMdl;
}

/*---------------------------------------------------------------------*//**
	TBF モデルをファイルから読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::load(const CStringBase* filename, bool isForceOneModel, bool isBillboard)
{
	// ファイルモジュールの作成
#if defined(_ANDROID)
	File* file = new FileAndroid(FileAndroid::FAF_ASSET);
#else
	File* file = new File();
#endif

	// ファイルの拡張子特定
	VcString filepath = VcString(filename) + ".tbmb";
	if(!file->isExist(&filepath))	{	filepath = VcString(filename) + ".tbm";	}
	if(!file->isExist(&filepath))	{	ASSERTM(false, "{ModelLoader_tbf::load} file does no exists.");	return 0L;	}
	
	// ファイルを開く
	if(!file->open(&filepath, File::MODE_READ))
	{
		ASSERT(false);
		delete file;
		return 0L;
	}
	_fileTbf = file;

	// 読み込み処理
	return load(isForceOneModel, isBillboard);
}

/*---------------------------------------------------------------------*//**
	TBF モデルをパックファイルから読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::load(u32 fileid, bool isForceOneModel, bool isBillboard)
{
	// ファイルを開く
	ASSERTM(IdFile::isIdFileExist(fileid), VcString::format("{ModelLoader_tbf::Load}\n file not exist!\n file_id=%d, p2=%d, p3=%d", fileid, isForceOneModel, isBillboard));
	
	IdFile* file = new IdFile(fileid);
	if(!file->isOpened())
	{
		ASSERT(false);
		delete file;
		return 0L;
	}
	_fileTbf = file;

	// 読み込み処理
	return load(isForceOneModel, isBillboard);
}

/*---------------------------------------------------------------------*//**
	TBF モデルを読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::load(bool isForceOneModel, bool isBillboard)
{
	bool isSuccess = true;

	// TbmbManip test
	#if 0
	{
		TbmbManip tbmbm;
		tbmbm.load(_fileTbf);
		_fileTbf->seek(0, true);
	}
	#endif

	// TBF ヘッダ読み込み
	TbfHeader head;
	_fileTbf->read(&head, sizeof(TbfHeader));
	///ASSERT(head->_typeInclude == TbfHeader::INCTYPE_KIND);

	// 内包ファイル情報読み込み
	List<TbfIncludeKindFile*> listIncf(true);
	for(int ifile = 0; ifile < (int)head._numIncludeFile; ifile++)
	{
		// 内包情報読み込み
		TbfIncludeKindFile* incf = new TbfIncludeKindFile;
		_fileTbf->read(incf, sizeof(TbfIncludeKindFile));
		listIncf.addTail(incf);
	}

	// 各内包ファイル読み込み
	for(ListIterator<TbfIncludeKindFile*> it = listIncf.iterator(); it.has(); it.next())
	{
		const TbfIncludeKindFile* incf = it.object();

		// ファイルシーク
		_fileTbf->seek(incf->_offset, true);

		if(::strcmp((const char*)incf->_kindname, "TexI1") == 0)
		{
			// テクスチャ配列読み込み
			TLOG("# loadTextureArray >>>\n");
			if(!loadTextureArray(incf->_size))		{	isSuccess = false;	}
			TLOG("# >>> loadTextureArray.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "MtrlI1") == 0)
		{
			// マテリアル配列読み込み
			TLOG("# loadMaterialArray >>>\n");
			if(!loadMaterialArray(incf->_size))		{	isSuccess = false;	}
			TLOG("# >>> loadMaterialArray.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "ShpsI1") == 0)
		{
			// シェイプ配列読み込み
			TLOG("# loadShapeArray >>>\n");
			if(!loadShapeArray(incf->_size))		{	isSuccess = false;	}
			TLOG("# >>> loadShapeArray.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "ShpHrI1") == 0)
		{
			// シェイプ階層読み込み
			TLOG("# loadShapeHierarchy >>>\n");
			if(!loadShapeHierarchy(incf->_size))	{	isSuccess = false;	}
			TLOG("# >>> loadShapeHierarchy.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "HrAnmI1") == 0)
		{
			// 階層アニメ読み込み
			TLOG("# loadHrAnim >>>\n");
			if(!loadHrAnim(incf->_size))			{	isSuccess = false;	}
			TLOG("# >>> loadHrAnim.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "TxAnmI1") == 0)
		{
			// テクスチャアニメ読み込み
			TLOG("# loadTexAnim >>>\n");
			if(!loadTexAnim(incf->_size))			{	isSuccess = false;	}
			TLOG("# >>> loadTexAnim.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "CmAnmI1") == 0)
		{
			// カメラアニメ読み込み
			TLOG("# loadCamAnim >>>\n");
			if(!loadCamAnim(incf->_size))			{	isSuccess = false;	}
			TLOG("# >>> loadCamAnim.\n");
		}
		else if(::strcmp((const char*)incf->_kindname, "GcolI1") == 0)
		{
			// 地面コリジョン読み込み
			TLOG("# loadGcol >>>\n");
			if(!loadGcol(incf->_size))				{	isSuccess = false;	}
			TLOG("# >>> loadGcol.\n");
		}
	}

	// ファイルを閉じる
	_fileTbf->close();

	// アニメーションセット作成
	if(isSuccess && (_numHaset > 0))
	{
		_hasarrHaset = new HrAnimSet*[_numHaset];
		for(int ihaset = 0; ihaset < _numHaset; ihaset++)
		{
			_hasarrHaset[ihaset] = new HrAnimSet();
			if(!_hasarrHaset[ihaset]->create(_hasiarrHasexi[ihaset]->_haarrHrAnim, _hasiarrHasexi[ihaset]->_numHrAnim, _hasiarrHasexi[ihaset]->_haobjTree, _hasiarrHasexi[ihaset]->_numHrAnimObjTreeTop, _hasiarrHasexi[ihaset]->_numHrAnimObjTotal, _hasiarrHasexi[ihaset]->_numAnimTagShape))
			{
				delete[] _hasiarrHasexi[ihaset]->_haarrHrAnim;
				delete[] _hasiarrHasexi[ihaset]->_haobjTree;
				isSuccess = false;
			}

			// アニメとアニメ階層はアニメセットに管理を委譲した、もしくは失敗して削除された
			_hasiarrHasexi[ihaset]->_haarrHrAnim = 0L;
			_hasiarrHasexi[ihaset]->_numHrAnim = 0;
			_hasiarrHasexi[ihaset]->_haobjTree = 0L;
			_hasiarrHasexi[ihaset]->_numHrAnimObjTreeTop = 0;
			_hasiarrHasexi[ihaset]->_numHrAnimObjTotal = 0;
			_hasiarrHasexi[ihaset]->_numAnimTagShape = 0;
		}
	}

	// モデル作成
	if(isSuccess)
	{
		// シェイプ共有
		int numShareHrAnimSet = 0;	// 共有階層アニメセット数
		for(int ishape = 0; ishape < _numShape; ishape++)
		{
			if(_siarrShexi[ishape]->_isRefer)		// 他とシェイプを共有する参照型シェイプ
			{
				ASSERT(_sarrShape[ishape] == 0L);
				if(_siarrShexi[ishape]->_shapeRef != 0L)
				{
					_sarrShape[ishape] = _siarrShexi[ishape]->_shapeRef;
				}
				else
				{
					CcString cstrDefName(_siarrShexi[ishape]->_defname);
					_sarrShape[ishape] = _siarrShexi[ishape]->_shapeRef = findExistedShape(&cstrDefName, false);
				}
				ASSERT(_sarrShape[ishape] != 0L);

				// 関連する階層アニメセットがあるか調べ、ある場合は階層アニメも共有する
				HrAnimSet* haset = findExistedHrAnimSet(_sarrShape[ishape], false);
				if(haset != 0L)
				{
					_siarrShexi[ishape]->_isAnimTrg = true;		// アニメーションに関連するシェイプである
					_siarrShexi[ishape]->_hasetRef = haset;
					numShareHrAnimSet++;
				}
				else
				{
					_siarrShexi[ishape]->_isAnimTrg = false;	// アニメーションに関連するシェイプではない
				}
			}
		}

		// モデル作成
		if(isForceOneModel)	// 強制１モデル
		{
			_numMdl = 1;
			_marrMdl = new Model*[_numMdl];
			// モデルを作成する
			if(isBillboard)
			{
				_marrMdl[0] = makeBillboardModel(_sarrShape, _siarrShexi, _numShape);
			}
			else
			{
				_marrMdl[0] = makeShapeModel(_sarrShape, _siarrShexi, _numShape, (_numHaset > 0 ? _hasarrHaset[0] : 0L), (_numGcol > 0 ? _garrGcol[0] : 0L));
			}
			if(_marrMdl[0] == 0L)
			{
				delete[] _marrMdl;
				_marrMdl = 0L;
				_numMdl = 0;
				isSuccess = false;
			}
			// カメラアニメーションがある場合はセットする
			if(_camanim != 0L)
			{
				_marrMdl[0]->setCamAnim(_camanim);
			}
		}
		else				// アニメーション単位にモデルを作成する
		{
			// モデル数の確定
			int cntShape = 0;
			// まずはアニメーションに関連付けられていないシェイプの数を得る
			for(int ishape = 0; ishape < _numShape; ishape++)
			{
				if(!_siarrShexi[ishape]->_isAnimTrg)	// アニメーションに関連しないシェイプ
				{
					cntShape++;
				}
			}
			// モデルの総数はアニメーションセットの数 + アニメーションに関連付けられていないシェイプの数
			_numMdl = _numHaset + numShareHrAnimSet + cntShape;

			// 配列を作成する
			_marrMdl = new Model*[_numMdl];

			// まずはアニメーションに関連付けられていないシェイプに対してのモデルを作成する
			int idxModel = 0;
			for(int ishape = 0; ishape < _numShape; ishape++)
			{
				if(!_siarrShexi[ishape]->_isAnimTrg)	// アニメーションに関連しないシェイプ
				{
					// モデルを作成する
					if(!loadSubMakeModel(isBillboard, &idxModel, &_sarrShape[ishape], &_siarrShexi[ishape], 1, 0L, (_numGcol > 0 ? _garrGcol[0] : 0L)))
					{
						isSuccess = false;
						break;
					}
				}
			}

			// アニメーションセット単位でモデルを作成する
			if(isSuccess)
			{
				for(int iaset = 0; iaset < _numHaset; iaset++)
				{
					// 該当シェイプ配列を作成する
					cntShape = 0;
					for(int ishape = 0; ishape < _numShape; ishape++)
					{
						if(_hasiarrHasexi[iaset]->_isAnimTrgShape[ishape])	{	cntShape++;	}
					}
					if(cntShape == 0)	{	continue;	}	// おそらくテクスチャアニメーションのみ場合など
					Shape** sarrShape = new Shape*[cntShape];
					ShapeExInfo** siarr = new ShapeExInfo*[cntShape];
					cntShape = 0;
					for(int ishape = 0; ishape < _numShape; ishape++)
					{
						if(_hasiarrHasexi[iaset]->_isAnimTrgShape[ishape])
						{
							sarrShape[cntShape] = _sarrShape[ishape];
							siarr[cntShape] = _siarrShexi[ishape];
							cntShape++;
						}
					}

					// モデルを作成する
					if(!loadSubMakeModel(isBillboard, &idxModel, sarrShape, siarr, cntShape, _hasarrHaset[iaset], (_numGcol > 0 ? _garrGcol[0] : 0L)))
					{
						isSuccess = false;
						break;
					}

					// 階層アニメセット共有に対応
					for(int ishape = 0; ishape < _numShape; ishape++)
					{
						if(_siarrShexi[ishape]->_hasetRef == _hasarrHaset[iaset])	// 共有している
						{
							// 拡張情報だけ付け替える
							for(int iswk = 0; iswk < cntShape; iswk++)
							{
								if(_siarrShexi[ishape]->_shapeRef == sarrShape[iswk])
								{
									siarr[iswk] = _siarrShexi[ishape];
									break;
								}
							}
							
							// モデルを作成する
							if(!loadSubMakeModel(isBillboard, &idxModel, sarrShape, siarr, cntShape, _hasarrHaset[iaset], (_numGcol > 0 ? _garrGcol[0] : 0L)))
							{
								isSuccess = false;
								break;
							}
						}
					}

					// シェイプ配列を削除する
					delete[] sarrShape;
					delete[] siarr;
				}
			}
			ASSERT(idxModel == _numMdl);

			// カメラアニメーションがある場合はセットする
			if(_camanim != 0L)
			{
				for(int i = 0; i < _numMdl; i++)
				{
					_marrMdl[i]->setCamAnim(_camanim);
				}
			}
		}
	}

	if(isSuccess)
	{
		// リソースプールにオブジェクトの管理を委譲
		ResourcePool* respl = _tfwRef->getResourcePool();
		if(_numShape > 0)
		{
			respl->add(_sarrShape, _numShape);

			for(int i = 0; i < _numShape; i++)			{	delete _siarrShexi[i];		}
			delete[] _siarrShexi;
			_siarrShexi = 0L;
			for(int i = 0; i < _numHrAnimObjTotal; i++)	{	delete _hoiHaoexi[i];		}
			delete[] _hoiHaoexi;
			_hoiHaoexi = 0L;

			_sarrShape = 0L;
			_numShape = 0;
		}
		if(_numTex > 0)
		{
			respl->add(_tarrTex, _numTex);
			_tarrTex = 0L;
			_numTex = 0;
		}
		if(_numTexAnim > 0)
		{
			respl->add(_taarrTexAnim, _numTexAnim);
			_taarrTexAnim = 0L;
			_numTexAnim = 0;
		}
		if(_numMtrl > 0)
		{
			respl->add(_marrMtrl, _numMtrl);
			_marrMtrl = 0L;
			_numMtrl = 0;
		}
		if(_numHaset > 0)
		{
			respl->add(_hasarrHaset, _numHaset);

			for(int i = 0; i < _numHaset; i++)			{	delete _hasiarrHasexi[i];	}
			delete[] _hasiarrHasexi;
			_hasiarrHasexi = 0L;

			_hasarrHaset = 0L;
			_numHaset = 0;
		}
		if(_camanim != 0L)
		{
			respl->add(_camanim);
			_camanim = 0L;
		}
		if(_numGcol > 0)
		{
			respl->add(_garrGcol, _numGcol);
			_garrGcol = 0L;
			_numGcol = 0;
		}
	}

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	シェイプモデルの作成
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadSubMakeModel(bool isBillboard, s32* idxModel, Shape** sarr, ShapeExInfo** shexiarr, int numShape, HrAnimSet* aset, Gcol* gcol)
{
	// モデルを作成する
	if(isBillboard)
	{
		_marrMdl[*idxModel] = makeBillboardModel(sarr, shexiarr, numShape);
	}
	else
	{
		_marrMdl[*idxModel] = makeShapeModel(sarr, shexiarr, numShape, aset, gcol);
	}

	if(_marrMdl[*idxModel] == 0L)
	{
		for(int i = *idxModel - 1; i >= 0; i--)	{	delete _marrMdl[i];	}
		delete[] _marrMdl;
		_marrMdl = 0L;
		_numMdl = 0;
		return false;
	}

	(*idxModel)++;
	return true;
}

/*---------------------------------------------------------------------*//**
	シェイプモデルの作成
**//*---------------------------------------------------------------------*/
ShapeModel* ModelLoader_tbf::makeShapeModel(Shape** sarr, ShapeExInfo** shexiarr, int numShape, HrAnimSet* aset, Gcol* gcol)
{
	if(numShape <= 0)	{	ASSERT(false);	return 0L;	}	// エラー

	// モデル作成
	ShapeModel* mdl = new ShapeModel();
	if(mdl->create( sarr, numShape, aset, gcol ))
	{
		// 初期アニメオブジェクト姿勢の設定
		if(_hoiHaoexi != 0L)
		{
			for(int i = 0; i < _numHrAnimObjTotal; i++)
			{
				HrObjExInfo* hoei = _hoiHaoexi[i];
				mdl->setAnimObjPosture((u16)hoei->_idxObj, hoei->_trs, hoei->_rot, hoei->_scl);
			}
		}

		// 追加モデル情報を付加
		if(shexiarr != 0L)
		{
			u16 rendorder = 0;
			u16 flagsEx = 0;
			u16 spid = 0;
			u8 deftype = TbfI1ShapeHeader::DEFTYPE_NULL;
			u8 flagsRenderOpt = 0;
			bool isSameDefType = true;
			for(int i = 0; i < numShape; i++)
			{
				ASSERT(sarr[i] != 0L);
				ASSERT(shexiarr[i] != 0L);

				// 最大の描画順
				if(rendorder < shexiarr[i]->_rendorder)
				{
					rendorder = shexiarr[i]->_rendorder;
				}

				// フラグを合わせる
				flagsEx |= shexiarr[i]->_flag;
				flagsRenderOpt |= shexiarr[i]->_flagsRenderOpt;

				// SPID は最初のものを
				if((shexiarr[i]->_spid != 0) && (spid == 0))
				{
					spid = shexiarr[i]->_spid;
				}

				// 定義タイプは全て同じならばそれ、同じでないなら DEFTYPE_NULL
				if(deftype != shexiarr[i]->_deftype)
				{
					if(deftype != TbfI1ShapeHeader::DEFTYPE_NULL)
					{
						isSameDefType = false;
					}
					deftype = shexiarr[i]->_deftype;
				}
			}
			if(!isSameDefType)	{	deftype = TbfI1ShapeHeader::DEFTYPE_NULL;	}

			// 描画順の設定
			mdl->setRenderOrder(rendorder);

			// フラグの設定
			mdl->setExFlag(flagsEx, true);
			mdl->setRenderOptFlag(flagsRenderOpt, true);

			// SPID の設定
			mdl->setSpid(spid);

			// 定義タイプが DEFTYPE_DECLARE, DEFTYPE_ALTERNATIVE の場合は表示しない
			switch(deftype)
			{
			case TbfI1ShapeHeader::DEFTYPE_DECLARE:
				mdl->setDefType(ShapeModel::DEFTYPE_DCL);		// 定義タイプを設定
				mdl->setModelFlag(Model::MFLAG_NO_USE, true);	// 使用しない
				mdl->setShow(false);							// 表示しない
				break;
			case TbfI1ShapeHeader::DEFTYPE_ALTERNATIVE:
				mdl->setDefType(ShapeModel::DEFTYPE_ALT);		// 定義タイプを設定
				mdl->setModelFlag(Model::MFLAG_NO_USE, true);	// 使用しない
				mdl->setShow(false);							// 表示しない
				break;
			}

			#if 1
				// 配置オブジェクトは座標をモデルの指定する
				switch(deftype)
				{
				case TbfI1ShapeHeader::DEFTYPE_INSTANCE:
				case TbfI1ShapeHeader::DEFTYPE_REFER:
					mdl->setPosture(shexiarr[0]->_trs, shexiarr[0]->_rot, shexiarr[0]->_scl);
					break;
				default:
					{
						Vector3F vZero(0.0f, 0.0f, 0.0f);
						Vector3F vOne(1.0f, 1.0f, 1.0f);
						mdl->setPosture(&vZero, &vZero, &vOne);	// 初期配置
					}
					break;
				}
			#else
				if((numShape == 1) && (aset == 0L))			// ひとつのシェイプでアニメーションがない場合
				{
					mdl->setPosture(shexiarr[0]->_trs, shexiarr[0]->_rot, shexiarr[0]->_scl);
				}
				else	// 複数シェイプ、もしくはアニメーションがある場合
				{
					Vector3F vZero(0.0f, 0.0f, 0.0f);
					Vector3F vOne(1.0f, 1.0f, 1.0f)
					mdl->setPosture(&vZero, &vZero, &vOne);	// 初期配置
				}
			#endif
		}
	}
	else
	{
		delete mdl;
		mdl = 0L;
	}

	return mdl;
}

/*---------------------------------------------------------------------*//**
	ビルボードモデルの作成
**//*---------------------------------------------------------------------*/
BillboardModel* ModelLoader_tbf::makeBillboardModel(Shape** sarr, ShapeExInfo** shexiarr, int numShape)
{
	if(numShape <= 0)	{	ASSERT(false);	return 0L;	}	// エラー

	// モデル作成
	BillboardModel* mdl = new BillboardModel();
	if(mdl->create( sarr[0] ))
	{
		// 追加モデル情報を付加
		if(shexiarr != 0L)
		{
			// 最大の描画順を採用
			u16 rendorder = 0;
			for(int i = 0; i < numShape; i++)
			{
				ASSERT(sarr[i] != 0L);
				ASSERT(shexiarr[i] != 0L);

				if(rendorder < shexiarr[i]->_rendorder)
				{
					rendorder = shexiarr[i]->_rendorder;
				}
			}
			mdl->setRenderOrder(rendorder);

			// フラグ
			mdl->setExFlag(shexiarr[0]->_flag, true);

			// SPID の設定
			mdl->setSpid(shexiarr[0]->_spid);

			// 姿勢は最初に見つけたものを採用
			if((shexiarr[0]->_trs != 0L) && (shexiarr[0]->_rot != 0L) && (shexiarr[0]->_scl != 0L))
			{
				mdl->setPosture(shexiarr[0]->_trs, shexiarr[0]->_rot, shexiarr[0]->_scl);
			}
		}
	}
	else
	{
		delete mdl;
		mdl = 0L;
	}

	return mdl;
}

/*---------------------------------------------------------------------*//**
	テクスチャ配列読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadTextureArray(u32 size)
{
	const u32 SIZE_SBUF = 256;
	u8 sbuf[SIZE_SBUF];

	// テクスチャ配列ヘッダ読み込み
	TbfI1TextureArrayHeader ahead;
	_fileTbf->read(&ahead, sizeof(TbfI1TextureArrayHeader));

	// テクスチャ読み込み
	List<Texture*> listTex(false);	// 配列化のための一時リスト
	for(int itex = 0; itex < (int)ahead._numTexture; itex++)
	{
		TbfI1TextureHeader thead;
		_fileTbf->read(&thead, sizeof(TbfI1TextureHeader));
		#if defined(_IOS)
			bool isOldFormat = (thead._format[0] == 0);
		#endif

		TmpBuf tmpbuf(thead._sizeData, sbuf, SIZE_SBUF);
		char* buf = (char*)tmpbuf.get();
		_fileTbf->read(buf, thead._sizeData);

		// テクスチャ作成
		Texture* tex = 0L;
		if(TFW_IS_FLAG(thead._flagAtrb, TbfI1TextureHeader::ATRBFLAG_EXTERNAL_FILE))
		{
			tex = new Texture();
			VcString fname(buf);
			fname += ".tga";
			if(!tex->create(&fname))
			{
				delete tex;
				tex = 0L;
			}
		}
		else if(TFW_IS_FLAG(thead._flagAtrb, TbfI1TextureHeader::ATRBFLAG_INCLUDE_FILE))
		{
			Texture::Format format = Texture::getFormatFrom3Char(thead._format[0], thead._format[1], thead._format[2]);
	
			if(format == Texture::FMT_NULL)	{	format = Texture::FMT_TGA;	}	// 移行期の暫定処理	［r-kishi 2009/09/02］
			#if defined(_IOS)
				if(!isOldFormat)
				{
					if(format == Texture::FMT_TGA)	{	format = Texture::FMT_NULL;	}
					///if(format == Texture::FMT_PVR)	{	format = Texture::FMT_NULL;	}
				}
			#endif
			
			if(format != Texture::FMT_NULL)
			{
				tex = new Texture();
				if(!tex->create(buf, thead._sizeData, format))
				{
					delete tex;
					tex = 0L;
				}
			}
		}
		else
		{
			ASSERT(false);
		}

		if(tex != 0L)
		{
			// ユニーク ID をセット
			tex->setUniqueId(thead._texuid);
			// リストに追加
			listTex.addTail(tex);
		}
	}

	// 配列情報を得る
	_numTex = listTex.getCount();
	_tarrTex = listTex.toArray();
	_usedTex = new bool[_numTex];
	::memset(_usedTex, 0, sizeof(bool) * _numTex);

	return true;
}

/*---------------------------------------------------------------------*//**
	マテリアル配列読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadMaterialArray(u32 size)
{
	// マテリアル配列ヘッダ読み込み
	TbfI1MaterialArrayHeader mhead;
	_fileTbf->read(&mhead, sizeof(TbfI1MaterialArrayHeader));

	// 配列情報を得る
	_numMtrl = (int)mhead._numMaterial;
	_marrMtrl = new Material*[_numMtrl];
	::memset(_marrMtrl, 0, sizeof(Material*) * _numMtrl);

	// マテリアル読み込み
	for(int imat = 0; imat < _numMtrl; imat++)
	{
		TbfI1Material material;
		_fileTbf->read(&material, sizeof(TbfI1Material));

		// テクスチャを得る
		Texture* texTrg = 0L;
		int itexTrg = -1;
		if(material._texuid != 0)
		{
			for(int itex = 0; itex < _numTex; itex++)
			{
				if(material._texuid == _tarrTex[itex]->getUniqueId())
				{
					texTrg = _tarrTex[itex];
					itexTrg = itex;
					break;
				}
			}
		}
		else	// 旧バージョン対応	［r-kishi 2009/09/03］
		{
			itexTrg = ((material.__pad_0020[ 3 ] << 24) & 0xff000000) | ((material.__pad_0020[ 2 ] << 16) & 0x00ff0000) | ((material.__pad_0020[ 1 ] << 8) & 0x0000ff00) | ((material.__pad_0020[ 0 ] << 0) & 0x000000ff);
			texTrg = ((0 <= itexTrg) && (itexTrg < _numTex)) ? _tarrTex[itexTrg] : 0L;
		}

		// マテリアル作成
		Vector3F* colorDiffuse = new Vector3F((Vector3F*)material._colorDiffuse);
		Vector3F* colorSpecular = new Vector3F((Vector3F*)material._colorSpecular);
		Vector3F* colorAmbient = new Vector3F((Vector3F*)material._colorAmbient);
		Material* mtrl = new Material();
		if(!mtrl->create(colorDiffuse, colorSpecular, colorAmbient, material._opacity, texTrg))
		{
			delete colorDiffuse;
			delete colorSpecular;
			delete colorAmbient;
			delete mtrl;
			mtrl = 0L;
			texTrg = 0L;
			itexTrg = -1;
		}
		
		// テクスチャは使われたかどうか
		if((0 <= itexTrg) && (itexTrg < _numTex))
		{
			ASSERT(itexTrg != -1);
			_usedTex[itexTrg] = true;
		}

		// 配列にセット
		_marrMtrl[imat] = mtrl;
	}

	delete[] _usedTex;
	_usedTex = 0L;

	return true;
}

/*---------------------------------------------------------------------*//**
	シェイプ配列読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadShapeArray(u32 size)
{
	bool isSucceeded = true;

	// シェイプ配列ヘッダ読み込み
	TbfI1ShapeArrayHeader sahead;
	_fileTbf->read(&sahead, sizeof(TbfI1ShapeArrayHeader));
	const u32 posShapeTop = _fileTbf->getPosition();

	// 配列を作成する
	_numShape = (int)sahead._numShape;
	_sarrShape = new Shape*[_numShape];
	::memset(_sarrShape, 0, sizeof(Shape*) * _numShape);
	_siarrShexi = new ShapeExInfo*[_numShape];
	for(int ishape = 0; ishape < _numShape; ishape++)	{	_siarrShexi[ishape] = new ShapeExInfo();	}

	// シェイプ読み込み
	for(int ishape = 0; ishape < _numShape; ishape++)
	{
		if(!loadShape(ishape))
		{
			ASSERT(false);
			isSucceeded = false;
		}
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	シェイプ読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadShape(int ishape)
{
	TbfI1ShapeHeader shead;
	_fileTbf->read(&shead, sizeof(TbfI1ShapeHeader));

	bool isSkipNext = false;
	
	// レベルエディットによるシェイプ共有機構.
	if(shead._deftype == TbfI1ShapeHeader::DEFTYPE_INSTANCE)
	{
		// 現在のシェイプの中から検索
		CcString cstrDefName((const char*)shead._defname);
		Shape* shape = findExistedShape(&cstrDefName, false);
		if(shape != 0L)	// 見つかった場合は２つ目以降の DEFTYPE_INSTANCE なので参照として動作（１つ目は実体として動作）
		{
			_siarrShexi[ishape]->_isRefer = true;
			_siarrShexi[ishape]->_shapeRef = shape;
			::memcpy(_siarrShexi[ishape]->_defname, shead._defname, sizeof(char) * TbfI1ShapeHeader::LEN_DEFNAME);
			isSkipNext = true;
		}
	}
	else if(shead._deftype == TbfI1ShapeHeader::DEFTYPE_REFER)
	{
		_siarrShexi[ishape]->_isRefer = true;
		::memcpy(_siarrShexi[ishape]->_defname, shead._defname, sizeof(char) * TbfI1ShapeHeader::LEN_DEFNAME);
		isSkipNext = true;
	}

	if(isSkipNext)
	{
		u32 offsetNext = 0;
		offsetNext += sizeof(Vector3F) * shead._numVertex;
		offsetNext += TFW_IS_FLAG(shead._flagShape, TbfI1ShapeHeader::SHAPEFLAG_NORMAL) ? (sizeof(Vector3F) * shead._numVertex) : 0;
		offsetNext += TFW_IS_FLAG(shead._flagShape, TbfI1ShapeHeader::SHAPEFLAG_TEXUV) ? (sizeof(Vector2F) * shead._numVertex) : 0;
		offsetNext += TFW_IS_FLAG(shead._flagShape, TbfI1ShapeHeader::SHAPEFLAG_VCOLOR) ? (sizeof(Vector4U8) * shead._numVertex) : 0;
		offsetNext += sizeof(u16) * shead._numIndex;
		offsetNext += sizeof(u16) * shead._numFace;
		_fileTbf->seek(offsetNext, false);
	}
	else
	{
		Shape* shape = 0L;

		// シェイプ読み込み
		u32 numVertex = shead._numVertex;
		Vector3F* vCrd = new Vector3F[shead._numVertex];
		Vector3F* vNrm = TFW_IS_FLAG(shead._flagShape, TbfI1ShapeHeader::SHAPEFLAG_NORMAL) ? new Vector3F[numVertex] : 0L;
		Vector2F* vTex = TFW_IS_FLAG(shead._flagShape, TbfI1ShapeHeader::SHAPEFLAG_TEXUV) ? new Vector2F[numVertex] : 0L;
		Vector4U8* vCol = TFW_IS_FLAG(shead._flagShape, TbfI1ShapeHeader::SHAPEFLAG_VCOLOR) ? new Vector4U8[numVertex] : 0L;
		u16 numIndex = shead._numIndex;
		u16* arrindexs = new u16[numIndex];
		u16 numVnumInFace = shead._numFace;
		u16* arrVnumInFace = new u16[numVnumInFace];

		_fileTbf->read(vCrd, sizeof(Vector3F) * numVertex);
		if(vNrm != 0L)			{	_fileTbf->read(vNrm, sizeof(Vector3F) * numVertex);			}
		if(vTex != 0L)			{	_fileTbf->read(vTex, sizeof(Vector2F) * numVertex);			}
		if(vCol != 0L)			{	_fileTbf->read(vCol, sizeof(Vector4U8) * numVertex);		}
		if(arrindexs != 0L)		{	_fileTbf->read(arrindexs, sizeof(u16) * numIndex);			}
		if(arrVnumInFace != 0L)	{	_fileTbf->read(arrVnumInFace, sizeof(u16) * numVnumInFace);	}

		// シェイプ作成
		shape = new Shape;
		#if defined(_DEBUG)
			CcString cstrDbgName((char*)shead._dbgname);
			shape->setDebugName(&cstrDbgName);
		#endif
		if(!shape->create(
			vCrd,
			vNrm,
			vCol,
			vTex,
			numVertex,
			arrindexs,
			numIndex,
			arrVnumInFace,
			numVnumInFace	))
		{
			delete shape;
			shape = 0L;
			return false;
		}

		// バウンディングボックスの設定
		shape->setBoundingBox((Vector3F*)shead._bboxMin, (Vector3F*)shead._bboxMax);

		// 定義名設定
		CcString cstrDefName((const char*)shead._defname);
		shape->setDefineName(&cstrDefName);
		///TRACE(VcString::format("--- %s (%d)\n", shead._name, shead._deftype));

		// SPID の設定
		shape->setSpid(shead._spid);

		// マテリアルのバインド
		if((0 <= shead._mtrlidx) && (shead._mtrlidx < _numMtrl))
		{
			shape->bindMaterial(_marrMtrl[shead._mtrlidx]);
		}

		_sarrShape[ishape] = shape;
	}

	// モデル情報の保存
	_siarrShexi[ishape]->_rendorder = shead._rendorder;				// 描画順
	_siarrShexi[ishape]->_flag = shead._flagAtrb;					// 属性フラグ
	_siarrShexi[ishape]->_spid = shead._spid;						// SPID
	_siarrShexi[ishape]->_deftype = shead._deftype;					// 定義タイプ
	_siarrShexi[ishape]->_flagsRenderOpt = shead._flagsRenderOpt;	// 描画フラグ

	return true;
}

/*---------------------------------------------------------------------*//**
	存在済みのシェイプを検索して取得する
**//*---------------------------------------------------------------------*/
Shape* ModelLoader_tbf::findExistedShape(CStringBase* name, bool isFindResourcePool)
{
	// この TBF 内の作成済みシェイプから検索
	for(int ishape = 0; ishape < _numShape; ishape++)
	{
		if(_sarrShape[ishape] == 0L)	{	continue;	}
		const VcString* nameWk = _sarrShape[ishape]->getDefineName();
		if(nameWk == 0L)				{	continue;	}
		if(nameWk->equals(name))		{	return  _sarrShape[ishape];	}
	}

	if(isFindResourcePool)
	{
		// リソースプールから検索
		ResourcePool* respl = _tfwRef->getResourcePool();
		return respl->findShape(name);
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	存在済みの階層アニメセットを検索して取得する
**//*---------------------------------------------------------------------*/
HrAnimSet* ModelLoader_tbf::findExistedHrAnimSet(Shape* shape, bool isFindResourcePool)
{
	#if 0	// 低速
		// この TBF 内の作成済み階層アニメセットから検索
		for(int ihaset = 0; ihaset < _numHaset; ihaset++)
		{
			HrAnimSet* haset = _hasarrHaset[ihaset];
			for(int ihaobj = 0; ihaobj < haset->getObjRootNum(); ihaobj++)
			{
				HrAnimObj* haobj = haset->getObjRoot(ihaobj);
				if(HrAnimObj::find(haobj, shape) != 0L)
				{
					return haset;
				}
			}
		}
	#else
		// 階層アニメセット拡張情報から検索
		for(int ishape = 0; ishape < _numShape; ishape++)
		{
			if(_sarrShape[ishape] == shape)	// 拡張情報参照のために、シェイプインデックスを求める
			{
				for(int ihaset = 0; ihaset < _numHaset; ihaset++)
				{
					if(_hasiarrHasexi[ihaset]->_isAnimTrgShape[ishape])
					{
						return _hasarrHaset[ihaset];
					}
				}
				break;
			}
		}
	#endif

	if(isFindResourcePool)
	{
		// リソースプールから検索
		ResourcePool* respl = _tfwRef->getResourcePool();
		return respl->findHrAnimSet(shape);
	}
	
	return 0L;
}

/*---------------------------------------------------------------------*//**
	シェイプ階層読み込み
**//*---------------------------------------------------------------------*/
#if 0	// デバッグ出力をするかどうか
	#define SHPH_TRACE	TRACE
#else
	#define SHPH_TRACE	//
#endif
bool ModelLoader_tbf::loadShapeHierarchy(u32 size)
{
	// シェイプ階層ヘッダ読み込み
	TbfI1ShapeHierarchyHeader shhead;
	_fileTbf->read(&shhead, sizeof(TbfI1ShapeHierarchyHeader));

	// HrAnimObj ツリー基本情報を得る
	_numHrAnimObjTreeTop = (int)shhead._numTopLevel;
	_haobjTree = new HrAnimObj*[_numHrAnimObjTreeTop];

	// オブジェクト拡張情報を作成する
	List<HrObjExInfo*>* listObjExi = new List<HrObjExInfo*>;

	SHPH_TRACE("{ModelLoader_tbf::loadShapeHierarchy} shape hierarchy\n");
	// 階層情報より HrAnimObj ツリーを作成する
	u32 cntShape = 0;
	TbfI1ShapeHierarchyElement shel;
	_fileTbf->read(&shel, sizeof(TbfI1ShapeHierarchyElement));	// 最初の要素を読み込む
	for(int itop = 0; itop < _numHrAnimObjTreeTop; itop++)
	{
		HrAnimObj* aobjRoot = makeHrAnimObjTree(&shel, &cntShape, shhead._numShape, 0, listObjExi);
		if(aobjRoot == 0L)
		{
			// 失敗
			delete[] _haobjTree;
			_haobjTree = 0L;
			_numHrAnimObjTreeTop = 0;

			for(ListIterator<HrObjExInfo*> it = listObjExi->iterator(); it.has(); it.next())
			{
				delete it.object()->_aobj;
				delete it.object();
			}
			delete listObjExi;

			return false;
		}

		_haobjTree[itop] = aobjRoot;
	}

	// 拡張情報を配列化
	_hoiHaoexi = listObjExi->toArray();
	delete listObjExi;

	return true;
}

/*---------------------------------------------------------------------*//**
	アニメ オブジェクト ツリーの作成
**//*---------------------------------------------------------------------*/
HrAnimObj* ModelLoader_tbf::makeHrAnimObjTree(TbfI1ShapeHierarchyElement* shel, u32* cntShape, const u32 numShape, u16 depth, List<HrObjExInfo*>* listObjExi)
{
	// 初期姿勢ベクトル
	const Vector3F* trs = (Vector3F*)shel->_translate;
	const Vector3F* rot = (Vector3F*)shel->_angle;
	const Vector3F* scl = (Vector3F*)shel->_scale;

	// 配置モデルの初期姿勢を保存
	if(	(_siarrShexi[shel->_shapeidx]->_deftype == TbfI1ShapeHeader::DEFTYPE_INSTANCE) ||
		(_siarrShexi[shel->_shapeidx]->_deftype == TbfI1ShapeHeader::DEFTYPE_REFER)	)
	{
		_siarrShexi[shel->_shapeidx]->setPosture(trs, rot, scl);
	}

	// アニメーションオブジェクト作成
	HrAnimObj* aobj = 0L;
	if((0 <= shel->_shapeidx) && (shel->_shapeidx < _numShape))
	{
		Shape* shape = _sarrShape[shel->_shapeidx];	// 対象シェイプを取得
		//if(shape != 0L)
		{
			// アニメオブジェ作成
			aobj = new HrAnimObj();

			if(!aobj->create(_numHrAnimObjTotal, shape))
			{
				delete aobj;
				return 0L;
			}

			// オブジェクトの初期姿勢を保存
			HrObjExInfo* hoei = new HrObjExInfo();
			hoei->set(aobj, _numHrAnimObjTotal, shel->_hasetidx, depth, trs, rot, scl);
			listObjExi->addTail(hoei);

			// 総オブジェクト数をカウント
			_numHrAnimObjTotal++;
		}
	}
	#if defined(_DEBUG)
		for(int i = 0; i < depth; i++)	{	SHPH_TRACE(" ");	}	SHPH_TRACE("[%d] a=%d, p=%d\n", shel->_shapeidx, shel->_hasetidx, (addr_size)aobj);
	#endif

	// シェイプ数をカウント
	(*cntShape)++;

	// 次の階層情報を得る
	if((*cntShape) < numShape)
	{
		_fileTbf->read(shel, sizeof(TbfI1ShapeHierarchyElement));	// 次の要素を読み込む
		while((*cntShape) < numShape)
		{
			if(shel->_depth == (depth + 1))		// 子
			{
				HrAnimObj* aobjChild = makeHrAnimObjTree(shel, cntShape, numShape, depth + 1, listObjExi);

				if(aobj != 0L)
				{
					if(aobj->getChild() == 0L)
					{
						aobj->setChild(aobjChild);
					}
					else
					{
						HrAnimObj* aobjChildSibling = aobj->child();
						while(aobjChildSibling->sibling() != 0L)	{	aobjChildSibling = aobjChildSibling->sibling();	}
						aobjChildSibling->setSibling(aobjChild);
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	return aobj;
}

/*---------------------------------------------------------------------*//**
	階層アニメ読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadHrAnim(u32 size)
{
	// 階層アニメ配列ヘッダ読み込み
	TbfI1HrAnimArrayHeader haahead;
	_fileTbf->read(&haahead, sizeof(TbfI1HrAnimArrayHeader));

	// 配列を作成する
	_numHrAnim = (int)haahead._numHrAnim;
	_numHaset = (int)haahead._numHaset;		/*フォーマット移行時の処置【2010/02/05 r-kishi】>>>*/if(_numHaset <= 0) { _numHaset = 1; }/*<<<*/
	_haarrHrAnim = new HrAnim*[_numHrAnim];
	::memset(_haarrHrAnim, 0, sizeof(HrAnim*) * _numHrAnim);
	_haiarrHaexi = new HrAnimExInfo*[_numHrAnim];
	for(int ianim = 0; ianim < _numHrAnim; ianim++)	{	_haiarrHaexi[ianim] = new HrAnimExInfo();			}
	_hasiarrHasexi = new HrAnimSetExInfo*[_numHaset];
	for(int iaset = 0; iaset < _numHaset; iaset++)	{	_hasiarrHasexi[iaset] = new HrAnimSetExInfo(_numShape);	}

	List<HrAnimObjFrame*>** listObjFrame = new List<HrAnimObjFrame*>*[haahead._numHrAnim];

	// 各アニメーションデータをファイルから読み込む
	for(int ianim = 0; ianim < _numHrAnim; ianim++)
	{
		listObjFrame[ianim] = new List<HrAnimObjFrame*>[_numShape];

		// 階層アニメヘッダ読み込み
		TbfI1HrAnimHeader hahead;
		_fileTbf->read(&hahead, sizeof(TbfI1HrAnimHeader));

		// 拡張情報の保存
		u8 animid = hahead._animid;
		if((animid == 0) && (_numHrAnim == _numHaset))	{	animid = 1;	}	// アニメーションが（アニメーションセットにつき）１つでアニメーション ID が 0 の場合は強制的にアニメーション ID を 1 にセットする
		_haiarrHaexi[ianim]->set(animid, hahead._hasetidx);

		// フレーム情報読み込み
		for(int ikfrm = 0; ikfrm < (int)hahead._numKeyFrame; ikfrm++)
		{
			TbfI1HrAnimFrameHeader hafhead;
			_fileTbf->read(&hafhead, sizeof(TbfI1HrAnimFrameHeader));

			for(int ipos = 0; ipos < (int)hafhead._numPosture; ipos++)
			{
				TbfI1HrAnimPosture hapos;
				_fileTbf->read(&hapos, sizeof(TbfI1HrAnimPosture));

				// 該当シェイプを得る
				if((hapos._shapeidx < 0) && (_numShape <= hapos._shapeidx))	{	continue;	}
				int ishape = (int)hapos._shapeidx;
				Shape* shapeTrg = _sarrShape[ishape];
				if(shapeTrg == 0L)	{	continue;	}
				// 該当 HrAnimObj を得る
				HrAnimObj* haobjTrg = 0L;
				for(int iaobj = 0; iaobj < _numHrAnimObjTreeTop; iaobj++)
				{
					HrAnimObj* haobj = HrAnimObj::find(_haobjTree[iaobj], shapeTrg);
					if(haobj != 0L)
					{
						haobjTrg = haobj;
						break;
					}
				}
				if(haobjTrg == 0L)	{	continue;	}

				// アニメーションフレーム作成
				HrAnimObjFrame* haof = new HrAnimObjFrame();
				haof->set(&hapos, haobjTrg);

				listObjFrame[ianim][ishape].addTail(haof);
			}
		}
	}

	// それぞれの階層アニメセットにオブジェ階層を分割する
	divideEachHrAnimSetObjTree(listObjFrame, _numHrAnim);

	// アニメを作成する
	for(int ianim = 0; ianim < _numHrAnim; ianim++)
	{
		int hasetidx = _haiarrHaexi[ianim]->_idxAnimSet;	// 該当階層アニメセットインデックス

		TLOG("# {loadHrAnim} *1* ianim=%d\n", ianim);
		// HrAnimObjFrame 配列を作成する
		HrAnimObjFrame** arrAof = new HrAnimObjFrame*[ _hasiarrHasexi[hasetidx]->_numAnimTagShape ];
		u16* numAof = new u16[_hasiarrHasexi[hasetidx]->_numAnimTagShape];
		TFW_ZERO_MEMORY(arrAof, sizeof(HrAnimObjFrame*) * _hasiarrHasexi[hasetidx]->_numAnimTagShape);
		TFW_ZERO_MEMORY(numAof, sizeof(u16) * _hasiarrHasexi[hasetidx]->_numAnimTagShape);
		int iats = 0;
		for(int ishape = 0; ishape < _numShape; ishape++)
		{
			if(!_hasiarrHasexi[hasetidx]->_isAnimTrgShape[ishape])	{	continue;	}
			numAof[iats] = (u16)listObjFrame[ianim][ishape].getCount();
			arrAof[iats] = new HrAnimObjFrame[numAof[iats]];
			int idxAof = 0;
			for(ListIterator<HrAnimObjFrame*> it = listObjFrame[ianim][ishape].iterator(); it.has(); it.next())
			{
				const HrAnimObjFrame* aof = it.object();
				arrAof[iats][idxAof].copy(aof);
				idxAof++;
				delete aof;
			}
			iats++;
		}

		TLOG("# {loadHrAnim} *2*\n");
		// アニメ作成
		HrAnim* hranim = new HrAnim();
		if(!hranim->create(_haiarrHaexi[ianim]->_animid, arrAof, numAof, _hasiarrHasexi[hasetidx]->_numAnimTagShape))
		{
			delete[] arrAof;
			delete[] numAof;
			delete hranim;
			delete[] listObjFrame;
			return false;
		}

		// 配列にセット
		_haarrHrAnim[ianim] = hranim;

		// リストの削除
		delete[] listObjFrame[ianim];
	}

	// リストの削除
	delete[] listObjFrame;

	// 階層アニメセットごとに関わる HrAnim を分割
	divideEachHrAnimSetAnim();

	return true;
}

/*---------------------------------------------------------------------*//**
	それぞれの階層アニメセットにオブジェ階層を分割する
**//*---------------------------------------------------------------------*/
void ModelLoader_tbf::divideEachHrAnimSetObjTree(List<HrAnimObjFrame*>** listObjFrame, int numHrAnim)
{
	// それぞれのシェイプはアニメーション対象かどうかを調査する
	for(int ishape = 0; ishape < _numShape; ishape++)
	{
		bool isAnimTrg = false;
		// キーフレーム情報から対象になっているか調査する
		for(int ianim = 0; ianim < numHrAnim; ianim++)
		{
			if(listObjFrame[ianim][ishape].getCount() > 0)
			{
				isAnimTrg = true;
				break;
			}
		}
		// 拡張情報に格納する
		_siarrShexi[ishape]->_isAnimTrg = isAnimTrg;
	}

	bool* isUsedObjTreeTop = new bool[_numHrAnimObjTreeTop];	// 全アニメーションにおけるオブジェクトのアニメーション対象チェックフラグ配列
	TFW_ZERO_MEMORY(isUsedObjTreeTop, sizeof(bool) * _numHrAnimObjTreeTop);
	bool* isAnimTrgObjTree = new bool[_numHrAnimObjTreeTop];	// アニメーションごとのオブジェクトのアニメーション対象チェックフラグ配列

	for(int ihaset = 0; ihaset < _numHaset; ihaset++)
	{
		// 使用するアニメオブジェを抽出する
		int numAnimTrgObjTreeTop = 0;
		int numAnimTrgObjTotal = 0;

		// オブジェクトのアニメーション対象チェックフラグ配列をクリア
		TFW_ZERO_MEMORY(isAnimTrgObjTree, sizeof(bool) * _numHrAnimObjTreeTop);

		int iaot = 0;
		for(int iaobj = 0; iaobj < _numHrAnimObjTotal; iaobj++)
		{
			if(_hoiHaoexi[iaobj]->_idxAnimSet == ihaset)	// 該当階層アニメセットのみ
			{
				numAnimTrgObjTotal++;

				if(_hoiHaoexi[iaobj]->_depth == 0)	// トップレベルのみ
				{
					HrAnimObj* haobj = _hoiHaoexi[iaobj]->_aobj;
					ASSERT(iaot < _numHrAnimObjTreeTop);

					// このトップツリー内にアニメ対象オブジェクト（シェイプ）はあるか
					for(int ishape = 0; ishape < _numShape; ishape++)
					{
						if(_sarrShape[ishape] == 0L)	{	continue;	}
						// このオブジェツリーのシェイプかを調べてアニメーション対象であればフラグを立てる
						// ※このアルゴリズムは無駄が多い．直接再帰処理に入った方がおそらく効率が良いのではないだろうか…
						if(HrAnimObj::find(haobj, _sarrShape[ishape]) != 0L)
						{
							if(_siarrShexi[ishape]->_isAnimTrg)
							{
								// ひとつでもある場合はフラグを立てる
								isAnimTrgObjTree[iaot] = true;
								isUsedObjTreeTop[iaot] = true;
								_hasiarrHasexi[ihaset]->_isAnimTrgShape[ishape] = true;
								break;
							}
						}
					}
					// 全てのツリー内のシェイプを上記で調べたアニメ対象に設定する
					for(int ishape = 0; ishape < _numShape; ishape++)
					{
						if(_sarrShape[ishape] == 0L)	{	continue;	}
						if(HrAnimObj::find(haobj, _sarrShape[ishape]) != 0L)
						{
							_hasiarrHasexi[ihaset]->_isAnimTrgShape[ishape] = isAnimTrgObjTree[iaot];
						}
					}
					if(isAnimTrgObjTree[iaot])	{	numAnimTrgObjTreeTop++;	}

					iaot++;
				}
			}
			else
			{
				if(_hoiHaoexi[iaobj]->_depth == 0)	// トップレベルのみ
				{
					iaot++;
				}
			}
		}

		// 階層情報を階層アニメセット毎に再構築する
		{
			HrAnimObj** haobjTreeNew = new HrAnimObj*[numAnimTrgObjTreeTop];
			iaot = 0;
			int iaotNew = 0;
			u16 indexCnt = 0;
			for(int iaobj = 0; iaobj < _numHrAnimObjTotal; iaobj++)
			{
				if(_hoiHaoexi[iaobj]->_idxAnimSet == ihaset)	// 該当階層アニメセットのみ
				{
					// インデックスを振りなおす
					HrAnimObj* haobj = _hoiHaoexi[iaobj]->_aobj;
					haobj->setIndex(indexCnt++);

					if(_hoiHaoexi[iaobj]->_depth == 0)	// トップレベルのみ
					{
						HrAnimObj* haobj = _hoiHaoexi[iaobj]->_aobj;
						ASSERT(iaot < _numHrAnimObjTreeTop);

						if(isAnimTrgObjTree[iaot])
						{
							// 新しいツリーでも使われる
							haobjTreeNew[iaotNew] = haobj;
							iaotNew++;
						}
						else
						{
							// 不要なツリー

							// 姿勢を設定して削除する → 廃止
							//
							// 不要なツリーについても階層通りの姿勢を与えることは技術的に可能だが、
							// その場合、（Shape 自体には姿勢ベクトルや行列を持たないので）
							// 子の頂点位置を全て移動しなけれならない．
							// この処理はそれなりのコストを要するため行わないことにする．
							//
							// そのため、以下のいずれかの制作上の制限を注意しなければならない
							// (1) アニメーションしないオブジェクトは階層を持たない（※１）
							// (2) アニメーションしないオブジェクトであっても階層を持ちたい場合は、
							// 　　階層中のいずれかのオブジェクトをわずかでも動かす（※２）
							// ※１：
							//		結合の結果アニメーションする場合は問題ない．
							//		静止のようなアニメーションは単体では読み込み不能であるが、
							//		結合の結果、静止以外のアニメーションが
							//		オブジェクトに対するキーフレームを持てば問題ない．
							//		逆に、階層を持たない場合は、絶対座標に配置すれば問題ない．
							// ※２：
							//		不要なツリー判定が、
							//		“トップの下にぶらさがっているオブジェクト階層が、
							//		子を含めいずれのオブジェクトもキーフレームが打たれていない場合は削除する”
							//		というものだからである．
							//		この条件であれば、キーフレームを打てば問題ないが、
							//		TbfExport が姿勢に変化のないキーフレームを削除してしまうため、
							//		このような制限が発生する．
							//		（TbfExport を使わない場合はその限りではない）
						}

						iaot++;
					}
				}
				else
				{
					if(_hoiHaoexi[iaobj]->_depth == 0)	// トップレベルのみ
					{
						iaot++;
					}
				}
			}
			_hasiarrHasexi[ihaset]->_haobjTree = haobjTreeNew;
			_hasiarrHasexi[ihaset]->_numHrAnimObjTreeTop = numAnimTrgObjTreeTop;
			_hasiarrHasexi[ihaset]->_numHrAnimObjTotal = numAnimTrgObjTotal;
		}

		// アニメーション対象オブジェクト数をカウントする
		int numAnimTrgShape = 0;
		for(int ishape = 0; ishape < _numShape; ishape++)
		{
			if(_hasiarrHasexi[ihaset]->_isAnimTrgShape[ishape])	{	numAnimTrgShape++;	}
		}
		_hasiarrHasexi[ihaset]->_numAnimTagShape = numAnimTrgShape;
	}

	// 不要なオブジェと既存のオブジェクト配列を削除する
	for(int iaot = 0; iaot < _numHrAnimObjTreeTop; iaot++)
	{
		if(!isUsedObjTreeTop[iaot])
		{
			// 参照先の HrAnimObj インスタンスも削除
			_haobjTree[iaot]->destroy();
			delete _haobjTree[iaot];
		}
	}
	delete[] _haobjTree;		// 以降、this->_haobjTree は使用せず
	_haobjTree = 0L;
	_numHrAnimObjTreeTop = 0;

	// フラグ配列の削除
	delete[] isAnimTrgObjTree;
	delete[] isUsedObjTreeTop;
}

/*---------------------------------------------------------------------*//**
	それぞれの階層アニメセットにアニメーションを分割する
**//*---------------------------------------------------------------------*/
void ModelLoader_tbf::divideEachHrAnimSetAnim()
{
	for(int ihaset = 0; ihaset < _numHaset; ihaset++)
	{
		int cntHrAnim = 0;
		for(int ihanim = 0; ihanim < _numHrAnim; ihanim++)
		{
			if(_haiarrHaexi[ihanim]->_idxAnimSet == ihaset)
			{
				cntHrAnim++;
			}
		}
		int numHrAnim = cntHrAnim;
		cntHrAnim = 0;
		HrAnim** haarrHrAnim = new HrAnim*[numHrAnim];
		for(int ihanim = 0; ihanim < _numHrAnim; ihanim++)
		{
			if(_haiarrHaexi[ihanim]->_idxAnimSet == ihaset)
			{
				haarrHrAnim[cntHrAnim++] = _haarrHrAnim[ihanim];
			}
		}
		ASSERT(cntHrAnim == numHrAnim);
		_hasiarrHasexi[ihaset]->_haarrHrAnim = haarrHrAnim;
		_hasiarrHasexi[ihaset]->_numHrAnim = numHrAnim;
	}

	// 既存の HrAnim 配列を削除
	delete[] _haarrHrAnim;	// 参照先の HrAnim インスタンスは生きる
	_haarrHrAnim = 0L;
	for(int i = 0; i < _numHrAnim; i++)			{	delete _haiarrHaexi[i];		}
	delete[] _haiarrHaexi;
	_haiarrHaexi = 0L;
	_numHrAnim = 0;
}

/*---------------------------------------------------------------------*//**
	テクスチャアニメ読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadTexAnim(u32 size)
{
	// テクスチャアニメ配列ヘッダ読み込み
	TbfI1TexAnimArrayHeader tahead;
	_fileTbf->read(&tahead, sizeof(TbfI1TexAnimArrayHeader));
	int numTexAnim = (int)tahead._numTexAnim;

	List<TexAnim*> listTexAnim;

	for(int ianim = 0; ianim < numTexAnim; ianim++)
	{
		bool isSucceeded = true;

		// テクスチャアニメヘッダ読み込み
		TbfI1TexAnimHeader tahead;
		_fileTbf->read(&tahead, sizeof(TbfI1TexAnimHeader));

		// アニメ作成
		TexAnim* texanim = new TexAnim();
		if(!texanim->create(tahead._numFrame, tahead._numKeyFrame))
		{
			isSucceeded = false;
		}

		// キーフレーム情報読み込み
		for(int ikfrm = 0; ikfrm < (int)tahead._numKeyFrame; ikfrm++)
		{
			TbfI1TexAnimKeyFrame tafkf;
			_fileTbf->read(&tafkf, sizeof(TbfI1TexAnimKeyFrame));

			if(isSucceeded)
			{
				if(!texanim->createKeyFrame(ikfrm, tafkf._frame, tafkf._translate[0], tafkf._translate[1]))
				{
					isSucceeded = false;
					break;
				}
			}
		}

		// 失敗時はアニメを削除して続行
		if(!isSucceeded)
		{
			delete texanim;
			continue;
		}

		// テクスチャにセット
		for(int itex = 0; itex < _numTex; itex++)
		{
			if(tahead._texuid == _tarrTex[itex]->getUniqueId())
			{
				_tarrTex[itex]->bindTexAnim(texanim);
				listTexAnim.addTail(texanim);
				texanim = 0L;
				break;
			}
		}

		// テクスチャにセットできなかったアニメは削除する
		if(texanim != 0L)
		{
			texanim->destroy();
			delete texanim;
		}
	}

	// 配列情報を得る
	_taarrTexAnim = listTexAnim.toArray();
	_numTexAnim = listTexAnim.getCount();

	return true;
}

/*---------------------------------------------------------------------*//**
	カメラアニメ読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadCamAnim(u32 size)
{
	ASSERT(_camanim == 0L);

	// カメラアニメヘッダ読み込み
	TbfI1CamAnimHeader cahead;
	_fileTbf->read(&cahead, sizeof(TbfI1CamAnimHeader));

	bool isSucceeded = true;

	// キーフレーム情報読み込み
	CamAnimFrame* cafarr = new CamAnimFrame[cahead._numKeyFrame];
	for(int ikfrm = 0; ikfrm < (int)cahead._numKeyFrame; ikfrm++)
	{
		TbfI1CamAnimKeyFrame cakf;
		_fileTbf->read(&cakf, sizeof(TbfI1CamAnimKeyFrame));

		if(isSucceeded)
		{
			cafarr[ikfrm].set(cakf._frame, (Vector3F*)cakf._location, (Vector3F*)cakf._aim, (Vector3F*)cakf._lookup);
		}
	}

	// アニメ作成
	CamAnim* camanim = new CamAnim();
	if(!camanim->create(cafarr, cahead._numKeyFrame, cahead._fovy, cahead._near, cahead._far))
	{
		isSucceeded = false;
	}

	// 失敗時はアニメを削除して抜ける
	if(!isSucceeded)
	{
		delete[] cafarr;
		delete camanim;
		return false;
	}

	_camanim = camanim;

	return true;
}

/*---------------------------------------------------------------------*//**
	地面コリジョン読み込み
**//*---------------------------------------------------------------------*/
bool ModelLoader_tbf::loadGcol(u32 size)
{
	// 地面コリジョン ヘッダ読み込み
	TbfI1GcolHeader ghead;
	_fileTbf->read(&ghead, sizeof(TbfI1GcolHeader));

	Gcol::Plane*	parr = new Gcol::Plane[ghead._numPlane];
	for(int iplane = 0; iplane < ghead._numPlane; iplane++)
	{
		TbfI1GcolPlane gplane;
		_fileTbf->read(&gplane, sizeof(TbfI1GcolPlane));
		parr[iplane]._vtxidx1 = gplane._vtxidx1;
		parr[iplane]._vtxidx2 = gplane._vtxidx2;
		parr[iplane]._vtxidx3 = gplane._vtxidx3;
		parr[iplane]._coeffidx = gplane._coeffidx;
		parr[iplane]._plnidxNeib1 = gplane._plnidxNeib1;
		parr[iplane]._plnidxNeib2 = gplane._plnidxNeib2;
		parr[iplane]._plnidxNeib3 = gplane._plnidxNeib3;
		parr[iplane]._flagAtrb = gplane._flagAtrb;
	}

	Vector3F*	varrVtx = new Vector3F[ghead._numVertex];
	_fileTbf->read(varrVtx, sizeof(Vector3F) * ghead._numVertex);

	Vector4F*	varrCoeff = new Vector4F[ghead._numCoeff];
	_fileTbf->read(varrCoeff, sizeof(Vector4F) * ghead._numCoeff);

	// 地面コリジョン作成
	Gcol* gcol = new Gcol();
	if(!gcol->create(parr, ghead._numPlane, varrVtx, ghead._numVertex, varrCoeff, ghead._numCoeff))
	{
		delete[] parr;
		delete[] varrVtx;
		delete[] varrCoeff;
		return false;
	}
	
	// 地面コリジョン配列作成
	_numGcol = 1;	// 現状１つに固定
	_garrGcol = new Gcol*[_numGcol];
	_garrGcol[0] = gcol;

	return true;
}

//==========================================================================
// ModelLoader_tbf::ShapeExInfo メソッド

ModelLoader_tbf::ShapeExInfo::ShapeExInfo()
	: _rendorder(0)
	, _flag(0)
	, _spid(0)
	, _deftype(0)
	, _flagsRenderOpt(0)
	, _isAnimTrg(false)
	, _isRefer(false)
	, _shapeRef(0L)
	, _hasetRef(0L)
	, _trs(0L)
	, _rot(0L)
	, _scl(0L)
{
}

ModelLoader_tbf::ShapeExInfo::~ShapeExInfo()
{
	delete _trs;
	delete _rot;
	delete _scl;
}

void ModelLoader_tbf::ShapeExInfo::setPosture(const Vector3F* trs, const Vector3F* rot, const Vector3F* scl)
{
	if(_trs == 0L)	{	_trs = new Vector3F();	}
	if(_rot == 0L)	{	_rot = new Vector3F();	}
	if(_scl == 0L)	{	_scl = new Vector3F();	}
	_trs->copy(trs);
	_rot->copy(rot);
	_scl->copy(scl);
}

//==========================================================================
// ModelLoader_tbf::HrObjExInfo メソッド

ModelLoader_tbf::HrObjExInfo::HrObjExInfo()
	: _aobj(0L)
	, _idxObj(0)
	, _idxAnimSet(0)
	, _depth(0)
	, _trs(0L)
	, _rot(0L)
	, _scl(0L)
	, _wk(0)
{
}

ModelLoader_tbf::HrObjExInfo::~HrObjExInfo()
{
	delete _trs;
	delete _rot;
	delete _scl;
}

void ModelLoader_tbf::HrObjExInfo::set(HrAnimObj* aobj, int idxObj, int idxAnimSet, u16 depth, const Vector3F* trs, const Vector3F* rot, const Vector3F* scl)
{
	_aobj = aobj;
	_idxObj = idxObj;
	_idxAnimSet = idxAnimSet;
	_depth = depth;
	if(_trs == 0L)	{	_trs = new Vector3F();	}
	if(_rot == 0L)	{	_rot = new Vector3F();	}
	if(_scl == 0L)	{	_scl = new Vector3F();	}
	_trs->copy(trs);
	_rot->copy(rot);
	_scl->copy(scl);
}

//==========================================================================
// ModelLoader_tbf::HrAnimExInfo メソッド

ModelLoader_tbf::HrAnimExInfo::HrAnimExInfo()
	: _animid(0)
	, _idxAnimSet(0)
{
}

ModelLoader_tbf::HrAnimExInfo::~HrAnimExInfo()
{
}

void ModelLoader_tbf::HrAnimExInfo::set(u8 animid, int idxAnimSet)
{
	_animid = animid;
	_idxAnimSet = idxAnimSet;
}

//==========================================================================
// ModelLoader_tbf::HrAnimSetExInfo メソッド

ModelLoader_tbf::HrAnimSetExInfo::HrAnimSetExInfo(int numShape)
	: _haarrHrAnim(0L)
	, _numHrAnim(0)
	, _haobjTree(0L)
	, _numHrAnimObjTreeTop(0)
	, _numHrAnimObjTotal(0)
	, _isAnimTrgShape(0L)
	, _numAnimTagShape(0)
{
	_isAnimTrgShape = new bool[numShape];
	TFW_ZERO_MEMORY(_isAnimTrgShape, sizeof(bool) * numShape);
}

ModelLoader_tbf::HrAnimSetExInfo::~HrAnimSetExInfo()
{
	delete[] _haarrHrAnim;
	delete[] _haobjTree;
	delete[] _isAnimTrgShape;
}

void ModelLoader_tbf::HrAnimSetExInfo::set(int numAnimTagShape)
{
	_numAnimTagShape = numAnimTagShape;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS