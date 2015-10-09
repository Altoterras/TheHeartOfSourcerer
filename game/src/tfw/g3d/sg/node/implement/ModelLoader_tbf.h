/***********************************************************************//**
 *  ModelLoader_tbf.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/07.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_LOADER_TBF_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_LOADER_TBF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

#include "../../../../tbf/TbfDefine.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

struct TbfI1ShapeHierarchyElement;
template<class TYPE> class TreeNode;
template<class TYPE> class Tree;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
template<class TYPE> class List;
class Tfw;
class Matrix4F;
class Shape;
class HrAnimObj;
class HrAnimObjFrame;
class HrAnim;
class HrAnimSet;
class CamAnim;
class FileBase;
class Material;
class Anim;
class Texture;
class TexAnim;
class Gcol;
class Model;
class ShapeModel;
class BillboardModel;

/*---------------------------------------------------------------------*//**
 *	TBF モデル ローダー クラス
 *
**//*---------------------------------------------------------------------*/
class ModelLoader_tbf
{
	//======================================================================
	// クラス

private:
	class ShapeExInfo
	{
	public:
		u16			_rendorder;				// 描画順
		u16			_flag;					// フラグ
		u16			_spid;					// SPID
		u8			_deftype;				// 定義タイプ
		u8			_flagsRenderOpt;		// 描画オプションフラグ
		bool		_isAnimTrg;				// アニメーション対象かどうか
		bool		_isRefer;				// DEFTYPE_INSTANCE や DEFTYPE_REFER などでシェイプを参照しているかどうか
		Shape*		_shapeRef;				// 共有参照しているシェイプ
		HrAnimSet*	_hasetRef;				// 共有参照している階層アニメセット
		char		_defname[TbfI1ShapeHeader::LEN_DEFNAME];
		Vector3F*	_trs;
		Vector3F*	_rot;
		Vector3F*	_scl;

		ShapeExInfo();
		~ShapeExInfo();

		void setPosture(const Vector3F* trs, const Vector3F* rot, const Vector3F* scl);
	};
	class HrObjExInfo
	{
	public:
		HrAnimObj*	_aobj;
		int			_idxObj;
		int			_idxAnimSet;
		u16			_depth;
		Vector3F*	_trs;
		Vector3F*	_rot;
		Vector3F*	_scl;
		int			_wk;

		HrObjExInfo();
		~HrObjExInfo();

		void set(HrAnimObj* aobj, int idxObj, int idxAnimSet, u16 depth, const Vector3F* trs, const Vector3F* rot, const Vector3F* scl);
	};
	class HrAnimExInfo
	{
	public:
		u8			_animid;
		int			_idxAnimSet;

		HrAnimExInfo();
		~HrAnimExInfo();

		void set(u8 animid, int idxAnimSet);
	};
	class HrAnimSetExInfo
	{
	public:
		HrAnim**	_haarrHrAnim;
		int			_numHrAnim;
		HrAnimObj**	_haobjTree;
		int			_numHrAnimObjTreeTop;
		int			_numHrAnimObjTotal;
		bool*		_isAnimTrgShape;
		int			_numAnimTagShape;

		HrAnimSetExInfo(int numShape);
		~HrAnimSetExInfo();

		void set(int numAnimTagShape);
	};

	//======================================================================
	// アクセサ

public:
	Model**				getLoadedModels() const		{	return _marrMdl; 	}
	int					getLoadedModelNum() const	{	return _numMdl;		}

	//======================================================================
	// メソッド

public:
	ModelLoader_tbf(Tfw* tfwRef);
	~ModelLoader_tbf();

	bool				load(const CStringBase* filename, bool isForceOneModel, bool isBillboard);
	bool				load(u32 fileid, bool isForceOneModel, bool isBillboard);

private:
	bool				load(bool isForceOneModel, bool isBillboard);
	bool				loadSubMakeModel(bool isBillboard, s32* idxModel, Shape** sarr, ShapeExInfo** shexiarr, int numShape, HrAnimSet* aset, Gcol* gcol);
	ShapeModel*			makeShapeModel(Shape** sarr, ShapeExInfo** shexiarr, int numShape, HrAnimSet* aset, Gcol* gcol);
	BillboardModel*		makeBillboardModel(Shape** sarr, ShapeExInfo** shexiarr, int numShape);
	bool				loadTextureArray(u32 size);
	bool				loadMaterialArray(u32 size);
	bool				loadShapeArray(u32 size);
	bool				loadShape(int ishape);
	Shape*				findExistedShape(CStringBase* name, bool isFindResourcePool);
	HrAnimSet*			findExistedHrAnimSet(Shape* shape, bool isFindResourcePool);
	bool				loadShapeHierarchy(u32 size);
	HrAnimObj*			makeHrAnimObjTree(TbfI1ShapeHierarchyElement* shel, u32* cntShape, const u32 numShape, u16 depth, List<HrObjExInfo*>* listObjExi);
	bool				loadHrAnim(u32 size);
	void				divideEachHrAnimSetObjTree(List<HrAnimObjFrame*>** listObjFrame, int numHrAnim);
	void				divideEachHrAnimSetAnim();
	bool				loadTexAnim(u32 size);
	bool				loadCamAnim(u32 size);
	bool				loadGcol(u32 size);

	//======================================================================
	// メンバ変数

private:
	Tfw*				_tfwRef;
	FileBase*			_fileTbf;

	Texture**			_tarrTex;
	int					_numTex;
	bool*				_usedTex;

	TexAnim**			_taarrTexAnim;
	int					_numTexAnim;

	Material**			_marrMtrl;
	int					_numMtrl;

	Shape**				_sarrShape;
	ShapeExInfo**		_siarrShexi;
	int					_numShape;

	HrAnimObj**			_haobjTree;
	HrObjExInfo**		_hoiHaoexi;
	int					_numHrAnimObjTreeTop;
	int					_numHrAnimObjTotal;

	HrAnim**			_haarrHrAnim;
	HrAnimExInfo**		_haiarrHaexi;
	int					_numHrAnim;

	HrAnimSet**			_hasarrHaset;
	HrAnimSetExInfo**	_hasiarrHasexi;
	int					_numHaset;

	CamAnim*			_camanim;

	Gcol**				_garrGcol;
	int					_numGcol;

	Model**				_marrMdl;
	int					_numMdl;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_LOADER_TBF_H_
