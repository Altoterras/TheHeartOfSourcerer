/***********************************************************************//**
 *	TbmbManip.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/02/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_TBF_TBMB_MANIP_H_
#define _TFW_TBF_TBMB_MANIP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "TbfDefine.h"

#include "../collection/List.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class FileBase;

/*---------------------------------------------------------------------*//**
 *	TBMB : Tbf モデルデータ 操作
 *	
**//*---------------------------------------------------------------------*/
class TbmbManip  
{
	//======================================================================
	// 定数
public:
	enum Platform
	{
		PF_ALL,
		PF_WIN,
		PF_IPH,
		NUM_PF
	};

	//======================================================================
	// クラス
public:
	class Block
	{
	public:
		Block();
		~Block();
		bool build(u32 size);
		bool copy(const Block* src);

	public:
		u32 _size;
		u8*	_bytes;
	};
	class AnimPos
	{
	public:
		TbfI1HrAnimPosture* _hap;
		u32 _frame;	// 【2010/07/02】TbfI1HrAnimPosture に含まれているが、将来的に TbfI1HrAnimPosture からは _frame を外すため
		
		AnimPos() : _hap(0L), _frame(0) {}
	};
	class ShEnt
	{
	public:
		List<S32*> _listShpidx;
		List<AnimPos*> _listAp;
		TbfI1ShapeHierarchyElement _shel;
		s32 _hasetidx;
		
		ShEnt()
		: _hasetidx(0)
		{
			_listShpidx.setAutoObjectDelete(true);
			_listAp.setAutoObjectDelete(true);
			::memset(&_shel, 0, sizeof(TbfI1ShapeHierarchyElement));
		}
	};
	
	//======================================================================
	// 構造体
public:
	struct Shape
	{
		TbfI1ShapeHeader			_header;
		Block						_data;
	};

	struct ShapeArray
	{
		List<Shape*>					_list;
	};

	struct ShapeHierarchy
	{
		TbfI1ShapeHierarchyElement	_data;
	};

	struct ShapeHierarchySet
	{
		List<ShapeHierarchy*>		_list;
		TbfI1ShapeHierarchyHeader	_header;
	};

	struct Material
	{
		TbfI1Material				_data;
	};

	struct MaterialArray
	{
		List<Material*>				_list;
	};

	struct Texture
	{
		TbfI1TextureHeader			_header;
		Block						_data;
	};

	struct TextureArray
	{
		List<Texture*>				_list[NUM_PF];
	};

	struct HrAnim
	{
		TbfI1HrAnimHeader			_header;
		Block						_data;
	};

	struct HrAnimArray
	{
		u32							_setnum;
		List<HrAnim*>				_list;
	};

	struct TexAnim
	{
		TbfI1TexAnimHeader			_header;
		Block						_data;
	};

	struct TexAnimArray
	{
		List<TexAnim*>				_list;
	};

	struct Other
	{
		s8							_kindname[8];			// ファイル種別名
		Block						_data;
	};

	struct OtherArray
	{
		List<Other*>					_list;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline const ShapeArray*		getShapeArray() const			{	return &_shps;		}
	inline const ShapeHierarchySet*	getShapeHierarchySet() const	{	return &_shphs;		}
	inline const MaterialArray*		getMaterialArray() const		{	return &_mtrls;		}
	inline const TextureArray*		getTextureArray() const			{	return &_texs;		}
	inline const HrAnimArray*		getHrAnimArray() const			{	return &_hranis;	}
	inline const TexAnimArray*		getTexAnimArray() const			{	return &_texanis;	}
	inline const OtherArray*		getOtherArray() const			{	return &_othrs;		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool load(FileBase* fileOpened);
	bool save(FileBase* fileOpened, Platform pf);
	bool combine(const TbmbManip* ins);
	bool mutilate();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TbmbManip();
	~TbmbManip();

private:
	bool loadShapeArray();
	bool loadShape(u32 ishape);
	bool loadShapeHierarchy();
	bool loadMaterialArray();
	bool loadTextureArray();
	bool loadHrAnimArray();
	bool loadTexAnimArray();
	bool loadOther(const TbfIncludeKindFile* incf);
	bool saveSubIncf(u32* fposTail, u32* fposIncf, const char* name, u32 fposFile, u32 sizeFile);

	//======================================================================
	// メンバ変数
private:
	FileBase*					_fileTbf;

	ShapeArray					_shps;
	ShapeHierarchySet			_shphs;
	MaterialArray				_mtrls;
	TextureArray				_texs;
	HrAnimArray					_hranis;
	TexAnimArray				_texanis;
	OtherArray					_othrs;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_TBF_TBMB_MANIP_H_
