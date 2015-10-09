/***********************************************************************//**
 *  ResourcePool.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/22.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COMMON_RESOURCE_POOL_H_
#define _TFW_COMMON_RESOURCE_POOL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class Shape;
class Texture;
class ImageBase;
class TexAnim;
class Material;
class HrAnimObj;
class HrAnimSet;
class CamAnim;
class Gcol;
class SgEntity;
class IdManager;

/*---------------------------------------------------------------------*//**
 *	リソース プール クラス
 *
**//*---------------------------------------------------------------------*/
class ResourcePool
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	IdManager*		getIdManager()	{	return _idmngrRef;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	Shape*			findShape(const CStringBase* defname) const;
	HrAnimSet*		findHrAnimSet(Shape* shape) const;

	void			add(Shape** shapearrEntr, s32 numShape);
	void			add(Texture** texarrEntr, s32 numTex);
	void			add(ImageBase* imgEntr);
	void			add(TexAnim** taarrEntr, s32 numTexAnim);
	void			add(Material** mtrlarrEntr, s32 numMtrl);
	void			add(HrAnimObj** haobjarrEntr, s32 numAobj);
	void			add(HrAnimSet** hasetarrEntr, s32 numAset);
	void			add(Gcol** gcolarrEntr, s32 numGcol);
	void			add(SgEntity** sgearrEntr, s32 numEntity);
	void			add(CamAnim* camanimEntr);

	void			removeUnusedSgEntityResource();
	void			removeAll();

	void			traceInfo();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ResourcePool();
	~ResourcePool();
	bool			create(IdManager* idmngrRef);
	void			destroy();

private:
	void			checkUsedShape(const Shape* shapeUsed, bool* isUsedShape, bool* isUsedMtrl, bool* isUsedTex, bool* isUsedImg, bool* isUsedTexAnim);
	void			checkUsedHrAnimObjDive(const HrAnimObj* haobjUsed, bool* isUsedHrAobj);

	//======================================================================
	// 変数
private:
	List<Shape*>*		_listShape;
	List<Texture*>*		_listTex;
	List<ImageBase*>*	_listImg;
	List<TexAnim*>*		_listTexAnim;
	List<Material*>*	_listMtrl;
	List<HrAnimObj*>*	_listHrAobj;
	List<HrAnimSet*>*	_listHrAset;
	List<CamAnim*>*		_listCamA;
	List<Gcol*>*		_listGcol;
	List<SgEntity*>*	_listSgEnt;

	IdManager*			_idmngrRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COMMON_RESOURCE_POOL_H_