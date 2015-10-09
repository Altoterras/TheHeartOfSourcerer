/***********************************************************************//**
 *	TbfDefine.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_TBF_TBF_DEFINE_H_
#define _TFW_TBF_TBF_DEFINE_H_

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ZEROMEMORY(p_, s_)	{ for(int ixx = 0; ixx < (s_); ixx++) { ((u8*)(p_))[ixx] = 0; } }

////////////////////////////////////////////////////////////////////////////
// 構造体

namespace tfw
{

//==========================================================================
/*
	TBF フォーマット

	TbfHeader
	{IncludeFiles} = {TbfIncludeXxxFile} * TbfHeader._numIncludeFile
	{FileArray} = {File} * TbfHeader._numIncludeFile

	{TbfIncludeXxxFile} =
		TbfIncludeKindFile or TbfIncludeGeneralFile
		TbfHeader._typeInclude でスイッチ
*/

/*---------------------------------------------------------------------*//**
 *	TBF ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	s8			_declare[6];			// TBF 宣言
	u8			_endian;				// エンディアン（ENDIAN_LITTLE / ENDIAN_BIG）
	u8			__pad_0010[ 1 ];
	u16			_verMajor;				// メジャーバージョン番号
	u16			_verMinor;				// マイナーバージョン番号
	u8			__pad_0020[ 4 ];
	// [ 16 ]
	u32			_sizeTotal;				// TBF ファイルトータルサイズ
	u32			_timeCreate;			// 作成時間
	u8			__pad_0030[ 3 ];
	u8			_typeInclude;			// 1 = TbfIncludeKindFile / 2 = TbfIncludeGeneralFile
	u32			_numIncludeFile;		// 内包ファイル数
	// [ 32 ]
	u8			__pad_0040[ 32 ];
	// [ 64 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 64;

	static const u8		ENDIAN_LITTLE					= 1;
	static const u8		ENDIAN_BIG						= 2;

	static const u8		INCTYPE_KIND					= 1;
	static const u8		INCTYPE_ID						= 2;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfHeader));

		// tpm
		_declare[0] = 't';
		_declare[1] = 'b';
		_declare[2] = 'f';

		// endian
		_endian = ENDIAN_LITTLE;

		// ver 1.0
		_verMajor = 1;
		_verMinor = 0;
	}
};

/*---------------------------------------------------------------------*//**
 *	TBF 内包種別ファイル情報構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfIncludeKindFile
{
	//======================================================================
	// 変数

	// [ 0 ]
	s8			_kindname[8];			// ファイル種別名
	u32			_offset;				// TBF ファイル先頭からのオフセット位置
	u32			_size;					// ファイルサイズ
	// [ 16 ]
	u8			_isCompress;			// 圧縮ファイルかどうか
	u8			__pad_0010[ 15 ];
	// [ 32 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 32;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfIncludeKindFile));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBF 内包 ID ファイル情報構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfIncludeIdFile
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_fileid;				// ファイル ID
	u32			_offset;				// TBF ファイル先頭からのオフセット位置
	u32			_size;					// ファイルサイズ
	u8			_isCompress;			// 圧縮ファイルかどうか
	u8			__pad_0020[ 3 ];
	// [ 16 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 16;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfIncludeIdFile));
	}
};

//==========================================================================
/*
	シェイプ フォーマット

	TbfI1ShapeArrayHeader
	{ShapeArray} = {Shape} * TbfI1ShapeArrayHeader._numShape

	{Shape} =
		TbfI1ShapeHeader
		{VertexList} = Vector3F * TbfI1ShapeHeader._numVertex
		{NormalList} = Vector3F * TbfI1ShapeHeader._numVertex
		{UvList} = Vector2F * TbfI1ShapeHeader._numVertex
		{ColorList} = Vector4U8 * TbfI1ShapeHeader._numVertex
		{IndexArray} = (u16 * IndexNum) * TbfI1ShapeHeader._numIndex
		{VertexNumInFaceArray} = u16 * TbfI1ShapeHeader._numFace

	※	“I1”は“iOS OpenGL ES 1.1 向け”の意とする
		別プラットフォーム向けへは別名をつける

	※	「シェイプ」と「モデル」の名称について
		Enlight ゲームアプリと Maya では「シェイプ」の意味合いが違う
		Maya において「シェイプ」は 3D 形状を表すと同時にレベルエディット
		情報なども表すものとなっているが、
		Enlight ゲームアプリにおいて「シェイプ」はあくまでも 3D 形状のみを表
		し、レベルエディット情報は「モデル」という単位になる
		現状 TBF は Maya と同じ扱いでシェイプを保持するが、将来的には
		Enlight ゲームアプリと同じ扱いにした方が効率的と思われる。
*/

/*---------------------------------------------------------------------*//**
 *	TBM シェイプ配列ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1ShapeArrayHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numShape;				// シェイプ数
	u8			__pad_0010[ 4 ];
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1ShapeArrayHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM シェイプ ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1ShapeHeader
{
	//======================================================================
	// 定数

	static const int	LEN_DEFNAME		= 12;

	//======================================================================
	// 変数

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 形状情報

	// [ 0 ]
	u8			_flagType;				// タイプフラグ
	u8			__pad_0010[ 3 ];
	s8			_defname[LEN_DEFNAME];	// シェイプ名
	// [ 16 ]
	u32			_numVertex;				// 頂点数
	u32			_numIndex;				// インデックス数
	u32			_numFace;				// 面数
	u16			_flagShape;				// 形状フラグ
	u16			_mtrlidx;				// マテリアルインデックス
	// [ 32 ]
	f32			_bboxMin[3];			// バウンディングボックス最小値
	f32			_bboxMax[3];			// バウンディングボックス最大値
	// [ 56 ]

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// レベルエディット情報

	u16			_spid;					// SPID
	u16			_flagAtrb;				// 汎用属性フラグ（ATRBFLAG_*）
	u16			_rendorder;				// 描画順
	u8			_deftype;				// 定義タイプ（DEFTYPE_*）
	u8			_flagsRenderOpt;		// 描画オプションフラグ
	// [ 64 ]
	u8			_dbgname[24];			// デバッグ用ネーム
	// [ 88 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 88;

	static const u8		TYPEFLAG_IOS_OPENGLES_11		= 0x01;		// iOS OpenGL ES 1.1 用

	static const u16	SHAPEFLAG_INDEXSHAPE			= 0x01;		// インデックスシェイプ
	static const u16	SHAPEFLAG_NORMAL				= 0x02;		// 法線あり
	static const u16	SHAPEFLAG_TEXUV					= 0x04;		// テクスチャ座標あり
	static const u16	SHAPEFLAG_VCOLOR				= 0x08;		// 頂点カラーあり

	static const u8		DEFTYPE_NULL					= 0;		// 無効値
	static const u8		DEFTYPE_INSTANCE				= 1;		// 実例（配置され、シェイプの形状は同名の最初の物を使う定義）
	static const u8		DEFTYPE_DECLARE					= 2;		// 宣言（配置されず、シェイプの形状のみの定義）
	static const u8		DEFTYPE_REFER					= 3;		// 参照（配置され、シェイプの形状は同名の他の物を使う定義）
	static const u8		DEFTYPE_ALTERNATIVE				= 4;		// 代替（配置されず、シェイプの形状のみ DECLARE の代替として定義）

	static const u8		ROPTFLAG_VTXCOL					= 0x01;		// 頂点カラー有り
	static const u8		ROPTFLAG_VTXNRM					= 0x02;		// 頂点法線有り
	static const u8		ROPTFLAG_SCLANIM				= 0x04;		// スケールアニメーション有り
	static const u8		ROPTFLAG_TRANS_NML				= 0x10;		// 通常半透明
	static const u8		ROPTFLAG_TRANS_ADD				= 0x20;		// 加算半透明
	static const u8		ROPTFLAG_TRANS_SUB				= 0x40;		// 減算半透明

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1ShapeHeader));
	}
};

//==========================================================================
/*
	シェイプ階層フォーマット

	TbfI1ShapeHierarchyHeader
	{ShapeHierarchyArray} = TbfI1ShapeHierarchyElement * TbfI1ShapeArrayHeader._numShape
*/

/*---------------------------------------------------------------------*//**
 *	TBM シェイプ階層ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1ShapeHierarchyHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numTopLevel;			// トップレベル要素数
	u32			_numShape;				// 階層情報内のシェイプ数
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1ShapeArrayHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM シェイプ階層要素構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1ShapeHierarchyElement
{
	//======================================================================
	// 変数

	// [ 0 ]
	u16			_depth;					// 階層の深さ
	u16			_shapeidx;				// シェイプインデックス
	u8			_hasetidx;				// アニメーションセットインデックス
	u8			__pad_0010[ 1 ];
	u16			_grpidx;				// グループインデックス
	u16			__reserved__type;		// (予約) タイプ
	u8			__pad_0020[ 6 ];
	// [ 16 ]
	f32			_translate[3];			// 平行移動ベクトル
	f32			_angle[3];				// 回転ベクトル
	f32			_scale[3];				// スケール ベクトル
	u8			__pad_0030[ 4 ];
	// [ 56 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 56;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1ShapeHierarchyElement));
	}
};

//==========================================================================
/*
	マテリアル フォーマット

	TbfI1MaterialArrayHeader
	TbfI1Material * TbfMaterialArrayHeader._numMaterial
*/

/*---------------------------------------------------------------------*//**
 *	TBM マテリアル配列ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1MaterialArrayHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numMaterial;			// マテリアル数
	u8			__pad_0010[ 4 ];
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1MaterialArrayHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM マテリアル構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1Material
{
	//======================================================================
	// 変数

	// [ 0 ]
	u8			_flagType;				// タイプフラグ
	u8			__pad_0010[ 3 ];
	u32			_flagAtrb;				// 属性フラグ
	u8			__pad_0020[ 4 ];
	u32			_texuid;				// テクスチャユニーク ID
	// [ 16 ]
	f32			_colorDiffuse[3];		// DIFFUSE カラー
	f32			_colorSpecular[3];		// SPECULAR カラー
	f32			_colorAmbient[3];		// AMBIENT カラー
	f32			_colorEmission[3];		// ？
	// [ 64 ]
	f32			_opacity;				// 透明度
	f32			_shininess;				// ？
	// [ 72 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 72;

	static const u8		TYPEFLAG_IOS_OPENGLES_11		= 0x01;		// iOS OpenGL ES 1.1 用

	static const u32	ATRBFLAG_NULL					= 0x0000;	// NULL

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1Material));
	}
};

//==========================================================================
/*
	テクスチャ フォーマット

	TbfI1TextureArrayHeader
	{TextureArray} = {Texture} * TbfI1TextureArrayHeader._numTexture

	{Texture} =
		TbfI1TextureHeader
		{TextureData}

	{TextureData} =
		# _flagAtrb | ATRBFLAG_INCLUDE_FILE の場合
			{テクスチャ画像データ}
		# _flagAtrb | ATRBFLAG_EXTERNAL_FILE の場合
			{テクスチャのファイル名文字列}
*/

/*---------------------------------------------------------------------*//**
 *	TBM テクスチャ配列ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1TextureArrayHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numTexture;			// テクスチャ数
	u8			__pad_0010[ 4 ];
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1TextureArrayHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM テクスチャ ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1TextureHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u8			_flagType;				// タイプフラグ
	s8			_format[ 3 ];			// 画像フォーマットを表現する３文字（主に拡張子の先頭３文字）
	u32			_flagAtrb;				// 属性フラグ
	u32			_sizeData;				// データサイズ
	u32			_texuid;				// テクスチャユニーク ID
	// [ 16 ]

	// テクスチャデータは、属性フラグが ATRBFLAG_INCLUDE_FILE の場合、
	// 　テクスチャ画像バイナリデータ
	// 属性フラグが ATRBFLAG_EXTERNAL_FILE の場合、
	// 　テクスチャのファイル名文字列
	// を指す

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 16;

	static const u8		TYPEFLAG_IOS_OPENGLES_11		= 0x01;		// iOS OpenGL ES 1.1 用

	static const u32	ATRBFLAG_INCLUDE_FILE			= 0x0001;	// ファイルを内包
	static const u32	ATRBFLAG_EXTERNAL_FILE			= 0x0002;	// 外部参照ファイル
	static const u32	ATRBFLAG_FILE_MASK				= 0x000f;	// ファイル属性マスク

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1TextureHeader));
	}
};

//==========================================================================
/*
	階層アニメーション フォーマット

	TbfI1HrAnimArrayHeader
	{HrAnimArray} = {HrAnim} * TbfI1HrAnimArrayHeader._numHrAnim

	{HrAnimArray} =
		TbfI1HrAnimHeader
		{HrAnimFrameArray} = {HrAnimFrame} * TbfI1HrAnimHeader._numKeyFrame

	{HrAnimFrame} =
		TbfI1HrAnimFrameHeader
		TbfI1HrAnimPosture * TbfI1HrAnimFrameHeader._numPosture
*/

/*---------------------------------------------------------------------*//**
 *	TBM 階層アニメ配列ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1HrAnimArrayHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numHrAnim;				// 階層アニメ数
	u8			_numHaset;				// 階層アニメセット数
	u8			__pad_0010[ 3 ];
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1HrAnimArrayHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM 階層アニメ ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1HrAnimHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numFrame;				// フレーム数
	u16			_numKeyFrame;			// キーフレーム数
	u8			_animid;				// アニメーション ID
	u8			_hasetidx;				// アニメーションセットインデックス
	// [ 8 ]
	//	※ 【2010/02/04 r-kishi】
	//		_animid と _idxAnimSet はサイズ的に窮屈だ．
	//		どこか決定的に互換性が保てなくなり、
	//		やむを得ず変更をするときのついでのタイミングで u16 に拡張したい…

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1HrAnimHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM 階層アニメ フレーム ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1HrAnimFrameHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numPosture;			// 姿勢情報数
	u32			_frame;					// フレーム番号　【2010/07/02】こちらにも TbfI1HrAnimPosture と同様の値を入れることにした
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1HrAnimFrameHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM 階層アニメ姿勢情報構造体
 *	※		データ構造的には TbfI1HrAnimFrameHeader に _frame があるのが
 *			自然ではある…．
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1HrAnimPosture
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_frame;					// フレーム番号（※１）
	u16			_shapeidx;				// シェイプインデックス
	u8			__pad_0010[ 2 ];
	u8			__reserved_0010[ 4 ];	// HrAnimObjFrame では _aobjTrgRef にあたる
	f32			_translate[3];			// 平行移動ベクトル
	f32			_angle[3];				// 回転ベクトル
	f32			_scale[3];				// スケール ベクトル
	// [ 48 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 48;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1HrAnimPosture));
	}
};

//==========================================================================
/*
	テクスチャ アニメーション フォーマット

	TbfI1TexAnimArrayHeader
	{TexAnimArray} = {TexAnim} * TbfI1TexAnimArrayHeader._numTexAnim

	{TexAnim} =
		TbfI1TexAnimHeader
		{TbfI1TexAnimKeyFrameArray} = TbfI1TexAnimKeyFrame * TbfI1TexAnimHeader._numKeyFrame
*/

/*---------------------------------------------------------------------*//**
 *	TBM テクスチャ アニメ配列ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1TexAnimArrayHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numTexAnim;			// テクスチャアニメ数
	u8			__pad_0010[ 4 ];
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1TexAnimArrayHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM テクスチャアニメ ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1TexAnimHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numFrame;			// フレーム数
	u16			_numKeyFrame;		// キーフレーム数
	u16			_animid;			// アニメーション ID
	// [ 8 ]
	u32			_texuid;			// テクスチャユニーク ID
	u8			__pad_0010[ 4 ];
	// [ 16 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 16;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1TexAnimHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM テクスチャアニメ キーフレーム 構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1TexAnimKeyFrame
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_frame;					// フレーム番号
	f32			_translate[2];			// 平行移動ベクトル
	// [ 12 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 12;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1TexAnimKeyFrame));
	}
};

//==========================================================================
/*
	カメラ アニメーション フォーマット

	TbfI1CamAnimHeader
	{TbfI1CamAnimKeyFrameArray} = TbfI1CamAnimKeyFrame * TbfI1CamAnimHeader._numKeyFrame
*/

/*---------------------------------------------------------------------*//**
 *	TBM カメラアニメ ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1CamAnimHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_numFrame;			// フレーム数
	u16			_numKeyFrame;		// キーフレーム数
	u8			__pad_0010[ 2 ];
	f32			_fovy;				// 視野角［DEGREE］
	f32			_near;				// ニアクリップ値
	// [ 16 ]
	f32			_far;				// ファークリップ値
	u8			__pad_0020[ 4 ];
	// [ 24 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 24;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1CamAnimHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM カメラアニメ キーフレーム 構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1CamAnimKeyFrame
{
	//======================================================================
	// 変数

	// [ 0 ]
	u32			_frame;					// フレーム番号
	f32			_location[3];			// 位置ベクトル
	f32			_aim[3];				// 注視点
	f32			_lookup[3];				// 上方ベクトル
	u8			__pad_0010[ 8 ];
	// [ 48 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 48;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1CamAnimKeyFrame));
	}
};

//==========================================================================
/*
	地面コリジョン フォーマット

	TbfI1GcolHeader
	{PlaneArray} = TbfI1GcolPlane * TbfI1GcolHeader._numPlane
	{VertexArray} = Vector3F * TbfI1GcolHeader._numVertex
	{CoeffArray} = Vector4F * TbfI1GcolHeader._numCoeff
*/

/*---------------------------------------------------------------------*//**
 *	TBM 地面コリジョン ヘッダ構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1GcolHeader
{
	//======================================================================
	// 変数

	// [ 0 ]
	u16			_numPlane;				// 三角形の数
	u16			_numVertex;				// 三角形の頂点数
	u16			_numCoeff;				// 平面方程式係数の数
	u8			__pad_0010[ 2 ];
	// [ 8 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 8;

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1GcolHeader));
	}
};

/*---------------------------------------------------------------------*//**
 *	TBM 地面コリジョン平面情報構造体
 *	
**//*---------------------------------------------------------------------*/
struct TbfI1GcolPlane
{
	//======================================================================
	// 変数

	// [ 0 ]
	u16			_vtxidx1;				// 三角形の頂点インデックス１
	u16			_vtxidx2;				// 三角形の頂点インデックス２
	u16			_vtxidx3;				// 三角形の頂点インデックス３
	u16			_coeffidx;				// 係数インデックス
	// [ 8 ]
	u16			_plnidxNeib1;			// 隣接平面の平面インデックス１
	u16			_plnidxNeib2;			// 隣接平面の平面インデックス２
	u16			_plnidxNeib3;			// 隣接平面の平面インデックス３
	u16			_flagAtrb;				// 属性フラグ
	// [ 16 ]

	//======================================================================
	// 定数

	static const s32	SIZE_THIS						= 16;

	static const u16	ATRBF_WALL						= 0x0001;	// 壁当たり
	static const u16	ATRBF_KEEPOUT					= 0x0002;	// 進入禁止
	static const u16	ATRBF_ABSOLUTE_KEEPOUT			= 0x0003;	// 絶対進入禁止（壁と進入禁止の合わせ）
	static const u16	ATRBF_ENTRANCE					= 0x0004;	// 入口（マップ開始位置）
	static const u16	ATRBF_MOVE_MAP_01				= 0x0010;	// マップ移動１
	static const u16	ATRBF_MOVE_MAP_02				= 0x0020;	// マップ移動２
	static const u16	ATRBF_MOVE_MAP_03				= 0x0040;	// マップ移動３
	static const u16	ATRBF_MOVE_MAP_04				= 0x0080;	// マップ移動４
	static const u16	ATRBF_EVENTID_MASK				= 0x0f00;	// イベント ID マスク
	static const u16	ATRBF_SHADOW_S					= 0x1000;	// 影量 - 小
	static const u16	ATRBF_SHADOW_M					= 0x2000;	// 影量 - 中
	static const u16	ATRBF_SHADOW_L					= 0x3000;	// 影量 - 大
	static const u16	ATRBF_SHADOW_MASK				= 0x3000;	// 影量マスク

	//======================================================================
	// メソッド

	void setInitValue()
	{
		ZEROMEMORY(this, sizeof(TbfI1GcolPlane));
		_plnidxNeib1 = 0xff;
		_plnidxNeib2 = 0xff;
		_plnidxNeib3 = 0xff;
	}
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////

#endif // _TFW_TBF_TBF_DEFINE_H_
