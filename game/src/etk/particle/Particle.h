/***********************************************************************//**
 *	Particle.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/11/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_PARTICLE_PARTICLE_H_
#define _ETK_PARTICLE_PARTICLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Matrix4F;
class Model;
class ExecCtx;
class RenderCtx;

TFW_END_NS

ETK_BEGIN_NS

class ParticleDef;
class ParticleSys;

/*---------------------------------------------------------------------*//**
 *	パーティクル
 *
**//*---------------------------------------------------------------------*/
class Particle
{
	//======================================================================
	// 定数
public:
	// 生成時設定フラグ（ここでのフラグのマスクは 0xffff0000．残りのビットは ParticleDef::CF_* で定義）
	static const u32	CF_EXT_COLOR			= 0x00010000;	// 外部の色指定に従う

	// 生成フレーム特殊値
	static const s32	LIFEFRAME_INFINITY		= -1;			// 無限（手動消滅）
	static const s32	LIFEFRAME_TEXANIM		= -2;			// テクスチャアニメーションのフレーム数を利用

private:
	// フレームフラグ
	static const u32	FF_CHANGE_COLOR			= 0x00000001;	// 色が変更された

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかの判定（出現遅延中も有効）
	inline bool isValid() const { return _model != 0L; }
	// 出現遅延中かどうかの判定
	inline bool isDelay() const { return _cntFrame < 0.0f; }
	// 位置を得る
	inline const Vector3F* getPosition() const { return _pos; }
	inline Vector3F* position() { return _pos; }
	// 速度を得る
	inline const Vector3F* getVelocity() const { return _vel; }
	inline Vector3F* velocity() { return _vel; }
	// モデルを取得する
	inline Model* getModel() const { return _model; }
	// パーティクル定義を得る
	inline const ParticleDef* getDef() const { return _def; }
	// 動作指定フラグを得る
	inline u32 getCtrlFlags() const { return _cflags; }

	// 動作フラグを設定する
	inline void setCtrlFlags(u32 flag) { _cflags |= flag; }
	// 持続フレーム数を設定する
	void setLifeFrame(s32 frame);
	// 遅延フレーム数を設定する
	void setDelayFrame(s32 frame);
	// 速度を設定する
	void setVelocity(const Vector3F* vel);
	// スケールを設定する
	inline void setScale(f32 scale) { _scale = scale; }
	// Z 軸回転を設定する
	inline void setRotateZ(f32 rotz) { _rotz = rotz; }
	// 色を設定する
	void setColor(const ColorU8* color);
	// 乗算アルファカラー値を設定する
	void setMulAlphaColor(f32 alphaMul);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool appear(const Model* modelRef, const Vector3F* pos, const ParticleDef* def);
	void disappear();
	void reflectPosture(bool isForceModelCalcMatrix);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Particle();
	~Particle();
	bool create(ParticleSys* ptclsysRef);
	void destroy();
	void reset();
	virtual void exec(const ExecCtx* ec);		// フレーム制御
	virtual void render(const RenderCtx* rc);	// 描画

private:
	s32 getTexAnimFrameNum() const;

	//======================================================================
	// 変数
private:
	Model* _model;				// モデルポインタ
	ParticleSys* _sysRef;		// システムへの参照
	const ParticleDef* _def;	// パーティクル定義へのポインタ
	Vector3F* _pos;				// 位置
	Vector3F* _vel;				// 速度
	f32 _scale;					// スケール
	f32 _rotz;					// Z 軸回転
	ColorU8* _color;			// 色
	u8 _alphaOrg;				// オリジナルアルファカラー値
	f32 _alphaMul;				// 乗算アルファカラー値（1.0f で不透明）
	u32 _cflags;				// 動作指定フラグ
	u32 _fflags;				// フレーム制御フラグ
	s32 _lifeframe;				// 生存フレーム数
	f32 _cntFrame;				// フレームカウンタ
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_PARTICLE_PARTICLE_H_
