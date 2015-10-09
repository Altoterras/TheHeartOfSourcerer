/***********************************************************************//**
 *	ParticleDef.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/01/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_PARTICLE_PARTICLE_DEF_H_
#define _ETK_PARTICLE_PARTICLE_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/lib/Color.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	パーティクル定義
 *
**//*---------------------------------------------------------------------*/
class ParticleDef
{
	//======================================================================
	// 定数
public:
	// 生成時設定フラグ（ここでのフラグのマスクは 0x0000ffff．残りのビットは Particle::CF_* で定義）
	///static const u32		CF_CUSTOM_BEHAVIOR	= 0x00000001;	// カスタム制御（PartileDef の標準パラメータに従わない）
	static const u32		CF_NO_CALC_VEL		= 0x00000010;	// 速度計算を行う

	// 描画フラグ（＜注＞値をそのまま使うので TbfI1ShapeHeader::ROPTFLAG_TRANS_* や、Model::ROPTFLAG_TRANS_* と同じにすること）
	static const u8			ROPTFLAG_TRANS_NML	= 0x10;			// 通常半透明
	static const u8			ROPTFLAG_TRANS_ADD	= 0x20;			// 加算半透明
	static const u8			ROPTFLAG_TRANS_SUB	= 0x40;			// 減算半透明

	// 色フラグ
	static const u8			COLF_NULL			= 0x00;			// 無し
	static const u8			COLF_COL1			= 0x01;			// 色１有効
	static const u8			COLF_COL2			= 0x02;			// 色２有効

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// パーティクル定義 ID を得る
	inline u16 getParticleDefId() const { return _ptcldid; }
	// モデル ID を得る
	inline s32 getModelId() const { return _modelid; }
	// 生存フレームを得る
	inline s32 getLifeFrame() const { return (_frameLife == 0xffff) ? -1 : (_frameLife == 0xfffe) ? -2 : _frameLife; }
	// 初速度 X を得る
	inline u16 getInitialVelX() const { return _xInitVel; }
	// 初速度 Y を得る
	inline u16 getInitialVelY() const { return _yInitVel; }
	// 初速度 Z を得る
	inline u16 getInitialVelZ() const { return _zInitVel; }
	// 汎用パラメータ１を得る
	inline u16 getParam1() const { return _param1; }
	// 汎用パラメータ２を得る
	inline u16 getParam2() const { return _param2; }
	// 汎用パラメータ３を得る
	inline u16 getParam3() const { return _param3; }
	// 半透明合成式を得る
	inline u16 getAlphaFuncFlag() const { return (u16)_alphafunc; }
	// 色フラグを得る
	inline u8 getColorFlag() const { return _colflags; }
	// 色 1 を得る
	inline const ColorU8* getColor1() const { return &_col1; }
	// 色 2 を得る
	inline const ColorU8* getColor2() const { return &_col2; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ParticleDef();
	bool loadFromDataFile(FileBase* file);
	void cleanup();

	//======================================================================
	// 変数
private:
	u16			_ptcldid;			// 定義 ID
	u16			_modelid;			// モデル ID
	u16			_frameLife;			// 生存フレーム
	u16			_xInitVel;			// 初速度 X
	u16			_yInitVel;			// 初速度 Y
	u16			_zInitVel;			// 初速度 Z
	u16			_param1;			// 汎用パラメータ１
	u16			_param2;			// 汎用パラメータ２
	u16			_param3;			// 汎用パラメータ３

	u8			_alphafunc;			// 半透明合成式（ROPTFLAG_TRANS_*）
	u8			_colflags;			// 色フラグ（COLF_*）
	ColorU8		_col1;				// 色 1
	ColorU8		_col2;				// 色 2
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_PARTICLE_PARTICLE_DEF_H_
