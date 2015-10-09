/***********************************************************************//**
 *  TbmbManip.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/02/01.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TbmbManip.h"

// Friends
#include "../file/FileBase.h"
#include "../lib/Vector.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TbmbManip::Block メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TbmbManip::Block::Block()
	: _size(0)
	, _bytes(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TbmbManip::Block::~Block()
{
	delete _bytes;
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TbmbManip::Block::build(u32 size)
{
	_size = size;
	_bytes = new u8[_size];
	if(_bytes == 0L)	{	return false;	}

	TFW_ZERO_MEMORY(_bytes, sizeof(u8) * _size);

	return true;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
bool TbmbManip::Block::copy(const Block* src)
{
	delete this->_bytes;

	this->_size = src->_size;
	this->_bytes = new u8[this->_size];
	if(this->_bytes == 0L)	{	return false;	}

	TFW_COPY_MEMORY(this->_bytes, src->_bytes, sizeof(u8) * _size);

	return true;
}

//==========================================================================
// TbmbManip メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::load(FileBase* fileOpened)
{
	_fileTbf = fileOpened;

	// TBF ヘッダ読み込み
	TbfHeader head;
	_fileTbf->read(&head, sizeof(TbfHeader));

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
			loadTextureArray();
		}
		else if(::strcmp((const char*)incf->_kindname, "MtrlI1") == 0)
		{
			// マテリアル配列読み込み
			loadMaterialArray();
		}
		else if(::strcmp((const char*)incf->_kindname, "ShpsI1") == 0)
		{
			// シェイプ配列読み込み
			loadShapeArray();
		}
		else if(::strcmp((const char*)incf->_kindname, "ShpHrI1") == 0)
		{
			// シェイプ階層読み込み
			loadShapeHierarchy();
		}
		else if(::strcmp((const char*)incf->_kindname, "HrAnmI1") == 0)
		{
			// 階層アニメ読み込み
			loadHrAnimArray();
		}
		else if(::strcmp((const char*)incf->_kindname, "TxAnmI1") == 0)
		{
			// テクスチャアニメ読み込み
			loadTexAnimArray();
		}
		else if(::strcmp((const char*)incf->_kindname, "CmAnmI1") == 0)
		{
			// カメラアニメ読み込み
			loadOther(incf);
		}
		else if(::strcmp((const char*)incf->_kindname, "GcolI1") == 0)
		{
			// 地面コリジョン読み込み
			loadOther(incf);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	保存
**//*---------------------------------------------------------------------*/
bool TbmbManip::save(FileBase* fileOpened, Platform pf)
{
	_fileTbf = fileOpened;

	// TBF ヘッダ仮書き込み（後で seek して上書きする）
	TbfHeader tbfhead;
	tbfhead.setInitValue();
	tbfhead._typeInclude = TbfHeader::INCTYPE_KIND;
	_fileTbf->write(&tbfhead, sizeof(TbfHeader));

	// 内包ファイル数カウント
	u32 numIncf = 0;
	numIncf += (_shps._list.getCount() > 0) ? 1 : 0;
	numIncf += (_shphs._list.getCount() > 0) ? 1 : 0;
	numIncf += (_mtrls._list.getCount() > 0) ? 1 : 0;
	numIncf += (_texs._list[pf].getCount() > 0) ? 1 : 0;
	numIncf += (_hranis._list.getCount() > 0) ? 1 : 0;
	numIncf += (_texanis._list.getCount() > 0) ? 1 : 0;
	numIncf += _othrs._list.getCount();

	// 内包ファイル情報を仮書き込み（後で seek して上書きする）
	u32 fposIncfCur = _fileTbf->getPosition();		// 内包ファイル情報のファイル位置を得ておく
	for(u32 iincf = 0; iincf < numIncf; iincf++)
	{
		TbfIncludeKindFile tincf;
		tincf.setInitValue();
		_fileTbf->write(&tincf, sizeof(TbfIncludeKindFile));
	}
	u32 fposTailCur = _fileTbf->getPosition();		// 末尾のファイル位置を得ておく

	// テクスチャ配列の書き込み
	if(_texs._list[pf].getCount() > 0)
	{
		u32 fposFile = fposTailCur;
		u32 sizeFile = 0;
		// テクスチャ配列ヘッダの書き込み
		TbfI1TextureArrayHeader ahead;
		ahead.setInitValue();
		ahead._numTexture = (u32)_texs._list[pf].getCount();
		_fileTbf->write(&ahead, sizeof(TbfI1TextureArrayHeader));
		sizeFile += sizeof(TbfI1TextureArrayHeader);
		// テクスチャの書き込み
		for(ListIterator<Texture*> it = _texs._list[pf].iterator(); it.has(); it.next())
		{
			Texture* tex = it.object();
			_fileTbf->write(&tex->_header, sizeof(TbfI1TextureHeader));
			_fileTbf->write(tex->_data._bytes, tex->_data._size);
			sizeFile += sizeof(TbfI1TextureHeader) + tex->_data._size;
		}
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, "TexI1", fposFile, sizeFile);
	}

	// マテリアル配列の書き込み
	if(_mtrls._list.getCount() > 0)
	{
		u32 fposFile = fposTailCur;
		u32 sizeFile = 0;
		// マテリアル配列ヘッダの書き込み
		TbfI1MaterialArrayHeader ahead;
		ahead.setInitValue();
		ahead._numMaterial = (u32)_mtrls._list.getCount();
		_fileTbf->write(&ahead, sizeof(TbfI1MaterialArrayHeader));
		sizeFile += sizeof(TbfI1MaterialArrayHeader);
		// マテリアルの書き込み
		for(ListIterator<Material*> it = _mtrls._list.iterator(); it.has(); it.next())
		{
			Material* mtrl = it.object();
			_fileTbf->write(&mtrl->_data, sizeof(TbfI1Material));
			sizeFile += sizeof(TbfI1Material);
		}
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, "MtrlI1", fposFile, sizeFile);
	}

	// シェイプ配列の書き込み
	if(_shps._list.getCount() > 0)
	{
		u32 fposFile = fposTailCur;
		u32 sizeFile = 0;
		// シェイプ配列ヘッダの書き込み
		TbfI1ShapeArrayHeader ahead;
		ahead.setInitValue();
		ahead._numShape = (u32)_shps._list.getCount();
		_fileTbf->write(&ahead, sizeof(TbfI1ShapeArrayHeader));
		sizeFile += sizeof(TbfI1ShapeArrayHeader);
		// シェイプの書き込み
		for(ListIterator<Shape*> it = _shps._list.iterator(); it.has(); it.next())
		{
			Shape* shape = it.object();
			_fileTbf->write(&shape->_header, sizeof(TbfI1ShapeHeader));
			_fileTbf->write(shape->_data._bytes, shape->_data._size);
			sizeFile += sizeof(TbfI1ShapeHeader) + shape->_data._size;
		}
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, "ShpsI1", fposFile, sizeFile);
	}

	// シェイプ階層の書き込み
	if(_shphs._list.getCount() > 0)
	{
		u32 fposFile = fposTailCur;
		u32 sizeFile = 0;
		// シェイプ階層ヘッダの書き込み
		_fileTbf->write(&_shphs._header, sizeof(TbfI1ShapeHierarchyHeader));
		sizeFile += sizeof(TbfI1ShapeHierarchyHeader);
		// シェイプ階層の書き込み
		for(ListIterator<ShapeHierarchy*> it = _shphs._list.iterator(); it.has(); it.next())
		{
			ShapeHierarchy* sh = it.object();
			_fileTbf->write(&sh->_data, sizeof(TbfI1ShapeHierarchyElement));
			sizeFile += sizeof(TbfI1ShapeHierarchyElement);
		}
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, "ShpHrI1", fposFile, sizeFile);
	}

	// 階層アニメの書き込み
	if(_hranis._list.getCount() > 0)
	{
		u32 fposFile = fposTailCur;
		u32 sizeFile = 0;
		// 階層アニメヘッダの書き込み
		TbfI1HrAnimArrayHeader ahead;
		ahead.setInitValue();
		ahead._numHrAnim = (u32)_hranis._list.getCount();
		ahead._numHaset = (u8)_hranis._setnum;
		_fileTbf->write(&ahead, sizeof(TbfI1HrAnimArrayHeader));
		sizeFile += sizeof(TbfI1HrAnimArrayHeader);
		// 階層アニメの書き込み
		for(ListIterator<HrAnim*> it = _hranis._list.iterator(); it.has(); it.next())
		{
			HrAnim* hrani = it.object();
			_fileTbf->write(&hrani->_header, sizeof(TbfI1HrAnimHeader));
			_fileTbf->write(hrani->_data._bytes, hrani->_data._size);
			sizeFile += sizeof(TbfI1HrAnimHeader) + hrani->_data._size;
		}
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, "HrAnmI1", fposFile, sizeFile);
	}

	// テクスチャアニメの書き込み
	if(_texanis._list.getCount() > 0)
	{
		u32 fposFile = fposTailCur;
		u32 sizeFile = 0;
		// テクスチャアニメヘッダの書き込み
		TbfI1TexAnimArrayHeader ahead;
		ahead.setInitValue();
		ahead._numTexAnim = (u32)_texanis._list.getCount();
		_fileTbf->write(&ahead, sizeof(TbfI1TexAnimArrayHeader));
		sizeFile += sizeof(TbfI1TexAnimArrayHeader);
		// テクスチャアニメの書き込み
		for(ListIterator<TexAnim*> it = _texanis._list.iterator(); it.has(); it.next())
		{
			TexAnim* texani = it.object();
			_fileTbf->write(&texani->_header, sizeof(TbfI1TexAnimHeader));
			_fileTbf->write(texani->_data._bytes, texani->_data._size);
			sizeFile += sizeof(TbfI1TexAnimHeader) + texani->_data._size;
		}
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, "TxAnmI1", fposFile, sizeFile);
	}

	// その他情報の書き込み
	for(ListIterator<Other*> it = _othrs._list.iterator(); it.has(); it.next())
	{
		u32 fposFile = fposTailCur;
		// その他情報の書き込み
		Other* oth = it.object();
		_fileTbf->write(oth->_data._bytes, oth->_data._size);
		// 内包ファイル情報の本書き込み
		saveSubIncf(&fposTailCur, &fposIncfCur, (const char*)oth->_kindname, fposFile, oth->_data._size);
	}

	// TBF ヘッダの書き込み
	_fileTbf->seek(0, true);
	tbfhead._sizeTotal = fposTailCur;
	tbfhead._numIncludeFile = numIncf;
	_fileTbf->write(&tbfhead, sizeof(TbfHeader));

	return true;
}

/*---------------------------------------------------------------------*//**
	保存時補助関数 - 内包ファイル情報の書き込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::saveSubIncf(u32* fposTail, u32* fposIncf, const char* name, u32 fposFile, u32 sizeFile)
{
	*fposTail = _fileTbf->getPosition();					// 末尾のファイル位置を得ておく
	_fileTbf->seek(*fposIncf, true);						// ファイル位置を内包ファイル情報へ移動
	TbfIncludeKindFile tincf;
	tincf.setInitValue();
	::strcpy((char*)tincf._kindname, name);
	tincf._offset = fposFile;
	tincf._size = sizeFile;
	_fileTbf->write(&tincf, sizeof(TbfIncludeKindFile));
	*fposIncf = _fileTbf->getPosition();					// 内包ファイル情報のファイル位置を得ておく
	_fileTbf->seek(*fposTail, true);						// ファイル位置を末尾に戻す

	return true;
}

/*---------------------------------------------------------------------*//**
	前方に結合
	※追加分のデータは既存データの前方に挿入される
**//*---------------------------------------------------------------------*/
bool TbmbManip::combine(const TbmbManip* ins)
{
	// シェイプ階層グループインデックス追加挿入分の使用済みインデックスを求める
	u16 grpidxUnused = 0;
	for(ListIterator<ShapeHierarchy*> it = ins->_shphs._list.iterator(); it.has(); it.next())
	{
		const ShapeHierarchy* shIns = it.object();
		if(grpidxUnused <= shIns->_data._grpidx)	{	grpidxUnused = shIns->_data._grpidx + 1;	}	// 使用していないグループインデックスを求める
	}

	// シェイプインデックス、グループインデックス、階層アニメセットインデックスが追加挿入分ずれる
	u32 numAddShapes = ins->_shps._list.getCount();
	for(ListIterator<HrAnim*> it = this->_hranis._list.iterator(); it.has(); it.next())
	{
		HrAnim* hra = it.object();
		u8* wk = hra->_data._bytes;
		for(u32 ikfrm = 0; ikfrm < hra->_header._numKeyFrame; ikfrm++)
		{
			TbfI1HrAnimFrameHeader* hafhead = (TbfI1HrAnimFrameHeader*)wk;
			wk += sizeof(TbfI1HrAnimFrameHeader);
			for(u32 ipos = 0; ipos < hafhead->_numPosture; ipos++)
			{
				TbfI1HrAnimPosture* hapos = (TbfI1HrAnimPosture*)wk;
				wk += sizeof(TbfI1HrAnimPosture);
				hapos->_shapeidx += numAddShapes;	// シェイプインデックスがずれる
			}
		}
	}
	for(ListIterator<ShapeHierarchy*> it = this->_shphs._list.iterator(); it.has(); it.next())
	{
		ShapeHierarchy* sh = it.object();
		if(ins->_hranis._list.getCount() > 0)	{	sh->_data._hasetidx++;	}	// 階層アニメセットインデックスがずれる
		sh->_data._shapeidx += numAddShapes;	// シェイプインデックスがずれる
		sh->_data._grpidx += grpidxUnused;		// グループインデックスがずれる
	}

	// マテリアルインデックスが追加挿入分ずれる
	u32 numAddMtrls = ins->_mtrls._list.getCount();
	for(ListIterator<Shape*> it = this->_shps._list.iterator(); it.has(); it.next())
	{
		Shape* shape = it.object();
		shape->_header._mtrlidx += numAddMtrls;	// マテリアルインデックスがずれる
	}

	// 階層アニメ情報内の階層アニメセットインデックスをずらす
	if(ins->_hranis._list.getCount() > 0)
	{
		this->_hranis._setnum++;	// セット数を増やす
		for(ListIterator<HrAnim*> it = this->_hranis._list.iterator(); it.has(); it.next())
		{
			HrAnim* hra = it.object();
			hra->_header._hasetidx++;	// 階層アニメセットインデックスをずらす
		}
	}

	// シェイプを追加
	for(ListIterator<Shape*> it = ins->_shps._list.iterator(); it.has(); it.next())
	{
		const Shape* shapeIns = it.object();
		Shape* shapeNew = new Shape;

		TFW_COPY_MEMORY(&shapeNew->_header, &shapeIns->_header, sizeof(TbfI1ShapeHeader));
		shapeNew->_data.copy(&shapeIns->_data);

		if(this->_shps._list.getCount() == 0)	{	this->_shps._list.addTail()->setObject(shapeNew);	}
		else									{	this->_shps._list.insertBefore(this->_shps._list.getHeadNode())->setObject(shapeNew);	}
	}

	// シェイプ階層を追加
	this->_shphs._header._numTopLevel += ins->_shphs._header._numTopLevel;
	this->_shphs._header._numShape += ins->_shphs._header._numShape;
	for(ListIterator<ShapeHierarchy*> it = ins->_shphs._list.iterator(); it.has(); it.next())
	{
		const ShapeHierarchy* shIns = it.object();
		ShapeHierarchy* shNew = new ShapeHierarchy;

		TFW_COPY_MEMORY(&shNew->_data, &shIns->_data, sizeof(TbfI1ShapeHierarchyElement));

		if(this->_shphs._list.getCount() == 0)	{	this->_shphs._list.addTail()->setObject(shNew);	}
		else									{	this->_shphs._list.insertBefore(this->_shphs._list.getHeadNode())->setObject(shNew);	}
	}

	// マテリアルを追加
	for(ListIterator<Material*> it = ins->_mtrls._list.iterator(); it.has(); it.next())
	{
		const Material* mtrlIns = it.object();
		Material* mtrlNew = new Material;

		TFW_COPY_MEMORY(&mtrlNew->_data, &mtrlIns->_data, sizeof(TbfI1Material));

		if(this->_mtrls._list.getCount() == 0)	{	this->_mtrls._list.addTail()->setObject(mtrlNew);	}
		else									{	this->_mtrls._list.insertBefore(this->_mtrls._list.getHeadNode())->setObject(mtrlNew);	}
	}

	// テクスチャを追加
	for(int ipf = 0; ipf < NUM_PF; ipf++)
	{
		for(ListIterator<Texture*> itIns = ins->_texs._list[ipf].iterator(); itIns.has(); itIns.next())
		{
			const Texture* texIns = itIns.object();

			// 重複テクスチャチェック
			bool isOverlap = false;
			for(ListIterator<Texture*> itThs = this->_texs._list[ipf].iterator(); itThs.has(); itThs.next())
			{
				const Texture* texThs = itThs.object();
				if(texThs->_header._texuid == texIns->_header._texuid)
				{
					isOverlap = true;
					break;
				}
			}

			// 重複しない場合は追加する
			if(!isOverlap)
			{
				Texture* texNew = new Texture;

				TFW_COPY_MEMORY(&texNew->_header, &texIns->_header, sizeof(TbfI1TextureHeader));
				texNew->_data.copy(&texIns->_data);

				if(this->_texs._list[ipf].getCount() == 0)	{	this->_texs._list[ipf].addTail()->setObject(texNew);	}
				else										{	this->_texs._list[ipf].insertBefore(this->_texs._list[ipf].getHeadNode())->setObject(texNew);	}
			}
		}
	}

	// 階層アニメを追加
	for(ListIterator<HrAnim*> it = ins->_hranis._list.iterator(); it.has(); it.next())
	{
		const HrAnim* hraIns = it.object();
		HrAnim* hraNew = new HrAnim;

		TFW_COPY_MEMORY(&hraNew->_header, &hraIns->_header, sizeof(TbfI1HrAnimHeader));
		hraNew->_data.copy(&hraIns->_data);

		if(this->_hranis._list.getCount() == 0)	{	this->_hranis._list.addTail()->setObject(hraNew);	}
		else									{	this->_hranis._list.insertBefore(this->_hranis._list.getHeadNode())->setObject(hraNew);	}
	}

	// テクスチャアニメを追加
	for(ListIterator<TexAnim*> it = ins->_texanis._list.iterator(); it.has(); it.next())
	{
		const TexAnim* texaIns = it.object();
		TexAnim* texaNew = new TexAnim;

		TFW_COPY_MEMORY(&texaNew->_header, &texaIns->_header, sizeof(TbfI1TexAnimHeader));
		texaNew->_data.copy(&texaIns->_data);

		if(this->_texanis._list.getCount() == 0)	{	this->_texanis._list.addTail()->setObject(texaNew);	}
		else										{	this->_texanis._list.insertBefore(this->_texanis._list.getHeadNode())->setObject(texaNew);	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	階層情報を元に、モデルを分割
	※	１アニメーションモデルにのみ対応
	※	現状はリアルタイムムービー用である
**//*---------------------------------------------------------------------*/
bool TbmbManip::mutilate()
{
	// シェイプ階層情報を解析し、参照しやすいリストに変換する
	List<ShEnt*> _listShe(true);
	ShEnt* she = 0L;
	for(ListIterator<ShapeHierarchy*> itSh = _shphs._list.iterator(); itSh.has(); itSh.next())
	{
		ShapeHierarchy* sh = itSh.object();
		if(sh->_data._depth == 0)
		{
			she = new ShEnt();
			::memcpy(&she->_shel, &sh->_data, sizeof(TbfI1ShapeHierarchyElement));
			_listShe.addTail(she);
		}
		ASSERT(she != 0L);
		she->_listShpidx.addTail(new S32(sh->_data._shapeidx));
	}

	// アニメーション情報を、参照しやすいリストに変換する
	List<TbfI1HrAnimPosture*> _listHapAll(true);
	TbfI1HrAnimHeader haheadBk;
	for(ListIterator<HrAnim*> itHa = _hranis._list.iterator(); itHa.has(); itHa.next())
	{
		HrAnim* ha = itHa.object();
		::memcpy(&haheadBk, &ha->_header, sizeof(TbfI1HrAnimHeader));	// 結果的に最後のヘッダが残る．このあたりが複数アニメーション対応できてないところ
		unsigned char* b = (unsigned char*)ha->_data._bytes;
		for(int ikfrm = 0; ikfrm < ha->_header._numKeyFrame; ikfrm++)
		{
			TbfI1HrAnimFrameHeader* hafhead = (TbfI1HrAnimFrameHeader*)b;
			b += sizeof(TbfI1HrAnimFrameHeader);

			for(int ipos = 0; ipos < (int)hafhead->_numPosture; ipos++)
			{
				TbfI1HrAnimPosture* hapos = (TbfI1HrAnimPosture*)b;
				b += sizeof(TbfI1HrAnimPosture);

				TbfI1HrAnimPosture* haposNew = new TbfI1HrAnimPosture;
				::memcpy(haposNew, hapos, sizeof(TbfI1HrAnimPosture));
				_listHapAll.addTail(haposNew);
			}
		}
	}

	// シェイプ階層情報を元に、アニメーションセットの数を算出する
	for(ListIterator<TbfI1HrAnimPosture*> itHap = _listHapAll.iterator(); itHap.has(); itHap.next())
	{
		TbfI1HrAnimPosture* hapos = itHap.object();

		// アニメーション姿勢情報がどの階層情報に属するかを調べ、属する階層情報にアニメーション姿勢情報を追加する
		for(ListIterator<ShEnt*> itShe = _listShe.iterator(); itShe.has(); itShe.next())
		{
			ShEnt* she = itShe.object();

			for(ListIterator<S32*> itShpidx = she->_listShpidx.iterator(); itShpidx.has(); itShpidx.next())
			{
				S32* shpidx = itShpidx.object();

				if(shpidx->_v == hapos->_shapeidx)
				{
					AnimPos* ap = new AnimPos();
					ap->_hap = hapos;
					ap->_frame = hapos->_frame;	// 【2010/07/02】将来的には TbfI1HrAnimHeader からとるべきである
					she->_listAp.addTail(ap);
				}
			}
		}
	}

	// 今までのアニメーション情報をクリア
	_hranis._setnum = 0;
	_hranis._list.removeAll();

	// 新しいアニメーションセット数をカウント
	s32 hasetidxCnt = 0;
	for(ListIterator<ShEnt*> itShe = _listShe.iterator(); itShe.has(); itShe.next())
	{
		ShEnt* she = itShe.object();

		if(she->_listAp.getCount() > 0)
		{
			she->_hasetidx = hasetidxCnt;
			hasetidxCnt++;
		}
	}
	_hranis._setnum = hasetidxCnt;

	// アニメーション情報を作成
	hasetidxCnt = 0;
	for(ListIterator<ShEnt*> itShe = _listShe.iterator(); itShe.has(); itShe.next())
	{
		ShEnt* she = itShe.object();

		s32 numAp = she->_listAp.getCount();
		if(she->_listAp.getCount() > 0)
		{
			// キーフレームの数をカウント
			u32 frameLast = 0;
			s32 numKeyFrame = 0;
			for(ListIterator<AnimPos*> itAp = she->_listAp.iterator(); itAp.has(); itAp.next())
			{
				AnimPos* ap = itAp.object();
				if(frameLast != ap->_frame)	// 新しいキーフレーム
				{
					numKeyFrame++;
					frameLast = ap->_frame;
				}
			}

			// それぞれのキーフレーム内の姿勢数をカウントする
			s32* posnumKeyFrame = new s32[numKeyFrame];
			::memset(posnumKeyFrame, 0, sizeof(s32) * numKeyFrame);
			s32 ikfrm = -1;
			frameLast = 0;
			for(ListIterator<AnimPos*> itAp = she->_listAp.iterator(); itAp.has(); itAp.next())
			{
				AnimPos* ap = itAp.object();
				if(frameLast != ap->_frame)
				{
					ikfrm++;
					frameLast = ap->_frame;
				}
				ASSERT((0 <= ikfrm) && (ikfrm < numKeyFrame));
				posnumKeyFrame[ikfrm]++;
			}

			HrAnim* ha = new HrAnim();

			// ヘッダ作成
			ha->_header._animid = haheadBk._animid;
			ha->_header._hasetidx = hasetidxCnt;
			ha->_header._numFrame = haheadBk._numFrame;
			ha->_header._numKeyFrame = numKeyFrame;

			// データ作成
			ha->_data.build((sizeof(TbfI1HrAnimFrameHeader) * numKeyFrame) + (sizeof(TbfI1HrAnimPosture) * numAp));
			unsigned char* b = (unsigned char*)ha->_data._bytes;
			ikfrm = -1;
			frameLast = 0;
			for(ListIterator<AnimPos*> itAp = she->_listAp.iterator(); itAp.has(); itAp.next())
			{
				AnimPos* ap = itAp.object();

				if(frameLast != ap->_frame)	// 新しいキーフレーム
				{
					ikfrm++;
					ASSERT((0 <= ikfrm) && (ikfrm < numKeyFrame));

					TbfI1HrAnimFrameHeader* hafh = (TbfI1HrAnimFrameHeader*)b;
					b += sizeof(TbfI1HrAnimFrameHeader);
					hafh->_numPosture = posnumKeyFrame[ikfrm];
					hafh->_frame = ap->_frame;

					frameLast = ap->_frame;
				}

				TbfI1HrAnimPosture* hapNew = (TbfI1HrAnimPosture*)b;
				b += sizeof(TbfI1HrAnimPosture);

				::memcpy(hapNew, ap->_hap, sizeof(TbfI1HrAnimPosture));
			}

			_hranis._list.addTail(ha);
			hasetidxCnt++;

			delete[] posnumKeyFrame;
		}
	}

	// 階層情報のアニメーションセットインデックスを書き換え
	ListNode<ShEnt*>* lnodeShe = 0L;
	she = 0L;
	hasetidxCnt = 0;
	for(ListIterator<ShapeHierarchy*> itSh = _shphs._list.iterator(); itSh.has(); itSh.next())
	{
		ShapeHierarchy* sh = itSh.object();
		if(sh->_data._depth == 0)
		{
			lnodeShe = (lnodeShe == 0L) ? _listShe.getHeadNode() : lnodeShe->getNext();
			she = lnodeShe->getObject();
			hasetidxCnt++;
		}

		ASSERT(she != 0L);
		sh->_data._hasetidx = she->_hasetidx;
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TbmbManip::TbmbManip()
	: _fileTbf(0L)
{
	_shps._list.setAutoObjectDelete(true);

	_shphs._list.setAutoObjectDelete(true);
	TFW_ZERO_MEMORY(&_shphs._header, sizeof(TbfI1ShapeHierarchyHeader));

	_mtrls._list.setAutoObjectDelete(true);

	_texs._list[PF_ALL].setAutoObjectDelete(true);
	_texs._list[PF_WIN].setAutoObjectDelete(false);	// PF_ALL で削除しているので、
	_texs._list[PF_IPH].setAutoObjectDelete(false);	// 　こちらは参照のみ

	_hranis._list.setAutoObjectDelete(true);
	_hranis._setnum = 0;

	_texanis._list.setAutoObjectDelete(true);

	_othrs._list.setAutoObjectDelete(true);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TbmbManip::~TbmbManip()
{
}

/*---------------------------------------------------------------------*//**
	シェイプ配列の読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadShapeArray()
{
	bool isSucceeded = true;

	// シェイプ配列ヘッダ読み込み
	TbfI1ShapeArrayHeader sahead;
	_fileTbf->read(&sahead, sizeof(TbfI1ShapeArrayHeader));

	// シェイプ読み込み
	for(u32 ishape = 0; ishape < sahead._numShape; ishape++)
	{
		if(!loadShape(ishape))		{	ASSERT(false);	isSucceeded = false;	}
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	シェイプの読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadShape(u32 ishape)
{
	Shape* shape = new Shape;

	// ヘッダの読み込み
	_fileTbf->read(&shape->_header, sizeof(TbfI1ShapeHeader));

	// データの読み込み
	u32 sizeData = 0;
	sizeData += sizeof(Vector3F) * shape->_header._numVertex;
	sizeData += TFW_IS_FLAG(shape->_header._flagShape, TbfI1ShapeHeader::SHAPEFLAG_NORMAL) ? (sizeof(Vector3F) * shape->_header._numVertex) : 0;
	sizeData += TFW_IS_FLAG(shape->_header._flagShape, TbfI1ShapeHeader::SHAPEFLAG_TEXUV) ? (sizeof(Vector2F) * shape->_header._numVertex) : 0;
	sizeData += TFW_IS_FLAG(shape->_header._flagShape, TbfI1ShapeHeader::SHAPEFLAG_VCOLOR) ? (sizeof(Vector4U8) * shape->_header._numVertex) : 0;
	sizeData += sizeof(u16) * shape->_header._numIndex;
	sizeData += sizeof(u16) * shape->_header._numFace;
	shape->_data.build(sizeData);
	_fileTbf->read(shape->_data._bytes, shape->_data._size);

	// リストに追加
	_shps._list.addTail(shape);

	return true;
}

/*---------------------------------------------------------------------*//**
	シェイプ階層の読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadShapeHierarchy()
{
	// シェイプ階層ヘッダ読み込み
	_fileTbf->read(&_shphs._header, sizeof(TbfI1ShapeHierarchyHeader));

	for(u32 ishape = 0; ishape < _shphs._header._numShape; ishape++)
	{
		ShapeHierarchy* sh = new ShapeHierarchy;

		// データの読み込み
		_fileTbf->read(&sh->_data, sizeof(TbfI1ShapeHierarchyElement));

		// リストに追加
		_shphs._list.addTail(sh);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マテリアル配列の読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadMaterialArray()
{
	// マテリアル配列ヘッダ読み込み
	TbfI1MaterialArrayHeader mhead;
	_fileTbf->read(&mhead, sizeof(TbfI1MaterialArrayHeader));

	for(u32 imtrl = 0; imtrl < mhead._numMaterial; imtrl++)
	{
		Material* mtrl = new Material;

		// データの読み込み
		_fileTbf->read(&mtrl->_data, sizeof(TbfI1Material));

		// リストに追加
		_mtrls._list.addTail(mtrl);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	テクスチャ配列の読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadTextureArray()
{
	// テクスチャアニメ配列ヘッダ読み込み
	TbfI1TextureArrayHeader tahead;
	_fileTbf->read(&tahead, sizeof(TbfI1TextureArrayHeader));

	for(u32 itex = 0; itex < tahead._numTexture; itex++)
	{
		Texture* tex = new Texture;

		// ヘッダの読み込み
		_fileTbf->read(&tex->_header, sizeof(TbfI1TextureHeader));

		// データの読み込み
		tex->_data.build(tex->_header._sizeData);
		_fileTbf->read(tex->_data._bytes, tex->_data._size);

		// リストに追加
		_texs._list[PF_ALL].addTail(tex);
		if( ((tex->_header._format[0] == 'P') && (tex->_header._format[1] == 'N') && (tex->_header._format[2] == 'G')) || ((tex->_header._format[0] == 'T') && (tex->_header._format[1] == 'G') && (tex->_header._format[2] == 'A')) )
		{
			_texs._list[PF_WIN].addTail(tex);
		}
		else if( (tex->_header._format[0] == 'P') && (tex->_header._format[1] == 'V') && (tex->_header._format[2] == 'R') )
		{
			_texs._list[PF_IPH].addTail(tex);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	階層アニメ配列の読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadHrAnimArray()
{
	// 階層アニメ配列ヘッダ読み込み
	TbfI1HrAnimArrayHeader haahead;
	_fileTbf->read(&haahead, sizeof(TbfI1HrAnimArrayHeader));

	// アニメーションセット数を得る
	_hranis._setnum = haahead._numHaset;	/*フォーマット移行時の処置【2010/02/05 r-kishi】>>>*/if(_hranis._setnum <= 0) { _hranis._setnum = 1; }/*<<<*/

	// 各アニメーションデータをファイルから読み込む
	for(u32 ianim = 0; ianim < haahead._numHrAnim; ianim++)
	{
		HrAnim* hranim = new HrAnim;

		// 階層アニメヘッダ読み込み
		_fileTbf->read(&hranim->_header, sizeof(TbfI1HrAnimHeader));

		// データ読み込み
		const u32 posDataTop = _fileTbf->getPosition();
		u32 sizeData = 0;
		for(u32 ifrm = 0; ifrm < hranim->_header._numKeyFrame; ifrm++)
		{
			TbfI1HrAnimFrameHeader hafhead;
			_fileTbf->read(&hafhead, sizeof(TbfI1HrAnimFrameHeader));
			u32 sizePostureData = (sizeof(TbfI1HrAnimPosture) * hafhead._numPosture);
			_fileTbf->seek(sizePostureData, false);
			sizeData += sizeof(TbfI1HrAnimFrameHeader) + sizePostureData;
		}
		_fileTbf->seek(posDataTop, true);
		hranim->_data.build(sizeData);
		_fileTbf->read(hranim->_data._bytes, hranim->_data._size);

		// リストに追加
		_hranis._list.addTail(hranim);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	テクスチャアニメ配列の読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadTexAnimArray()
{
	// テクスチャアニメ配列ヘッダ読み込み
	TbfI1TexAnimArrayHeader tahead;
	_fileTbf->read(&tahead, sizeof(TbfI1TexAnimArrayHeader));

	for(u32 ianim = 0; ianim < tahead._numTexAnim; ianim++)
	{
		TexAnim* texa = new TexAnim();

		// テクスチャアニメヘッダ読み込み
		_fileTbf->read(&texa->_header, sizeof(TbfI1TexAnimHeader));

		// データの読み込み
		u32 sizeData = texa->_header._numKeyFrame * sizeof(TbfI1TexAnimKeyFrame);
		texa->_data.build(sizeData);
		_fileTbf->read(texa->_data._bytes, texa->_data._size);

		// リストに追加
		_texanis._list.addTail(texa);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	その他のデータの読み込み
**//*---------------------------------------------------------------------*/
bool TbmbManip::loadOther(const TbfIncludeKindFile* incf)
{
	Other* other = new Other;

	TFW_COPY_MEMORY(other->_kindname, incf->_kindname, sizeof(u8) * 8);

	other->_data.build(incf->_size);
	_fileTbf->read(other->_data._bytes, other->_data._size);

	// リストに追加
	_othrs._list.addTail(other);

	return true;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS