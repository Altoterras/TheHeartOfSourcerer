/***********************************************************************//**
 *	Particle.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/11/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Particle.h"

// Friends
#include "ParticleDef.h"
#include "ParticleSys.h"

// External
#include "../../tfw/g3d/Material.h"
#include "../../tfw/g3d/Shape.h"
#include "../../tfw/g3d/sg/node/implement/BillboardModel.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/gcmn/TexAnim.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/gcmn/Renderer.h"

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 静的定数

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	持続フレーム数を設定する
**//*---------------------------------------------------------------------*/
void Particle::setLifeFrame(s32 frame)
{
	_lifeframe = frame;

	// テクスチャアニメーションのフレーム値を利用する場合は、フレーム数を取得・上書き
	if(_lifeframe == LIFEFRAME_TEXANIM)
	{
		if(_model != 0L)
		{
			_lifeframe = getTexAnimFrameNum();
		}
	}
}

/*---------------------------------------------------------------------*//**
	遅延フレーム数を設定する
**//*---------------------------------------------------------------------*/
void Particle::setDelayFrame(s32 frame)
{
	_cntFrame -= frame;	// 現在のフレームから減算する
}

/*---------------------------------------------------------------------*//**
	速度を設定する
**//*---------------------------------------------------------------------*/
void Particle::setVelocity(const Vector3F* vel)
{
	*_vel = *vel;
}

/*---------------------------------------------------------------------*//**
	色を設定する
**//*---------------------------------------------------------------------*/
void Particle::setColor(const ColorU8* color)
{
	if(_color->equals(color))	{	return;	}

	*_color = *color;
	_alphaOrg = color->a();

	if(_alphaMul <= 0.0f)
	{
		_color->a() = 0;
	}
	else if(_alphaMul < 1.0f)
	{
		_color->a() = (u8)(_alphaOrg * _alphaMul);
	}

	TFW_SET_FLAG(_fflags, FF_CHANGE_COLOR, true);
}

/*---------------------------------------------------------------------*//**
	乗算アルファカラー値を設定する
**//*---------------------------------------------------------------------*/
void Particle::setMulAlphaColor(f32 alphaMul)
{
	if(_alphaMul == alphaMul)	{	return;	}

	_alphaMul = alphaMul;

	if(_alphaMul <= 0.0f)
	{
		_color->a() = 0;
	}
	else if(_alphaMul < 1.0f)
	{
		_color->a() = (u8)(_alphaOrg * _alphaMul);
	}

	TFW_SET_FLAG(_fflags, FF_CHANGE_COLOR, true);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	出現させる
**//*---------------------------------------------------------------------*/
bool Particle::appear(const Model* modelRef, const Vector3F* pos, const ParticleDef* def)
{
	// 念のため再リセット
	reset();

	// モデルを複製
	_model = modelRef->clone();
	ASSERT(_model != 0L);
	if(_model == 0L)	{	return false;	}

	// 値を初期化
	_pos->set(0.0f, 0.0f, 0.0f);
	_vel->set(0.0f, 0.0f, 0.0f);
	_scale = 1.0f;
	_color->set(255, 255, 255, 255);
	_cntFrame = 1.0f;

	// 位置が指定されている場合は初期位置を設定する
	if(pos != 0L)
	{
		_pos->copy(pos);
	}

	// パーティクル定義がある場合は定義に従う
	_def = def;
	if(_def != 0L)
	{
		// 速度
		_vel->set((f32)_def->getInitialVelX(), (f32)_def->getInitialVelY(), (f32)_def->getInitialVelZ());
		// 生存時間
		_lifeframe = _def->getLifeFrame();
		// 半透明合成式
		_model->setRenderOptFlag(_def->getAlphaFuncFlag(), true);
		// 色を設定
		if(TFW_IS_FLAG(_def->getColorFlag(), ParticleDef::COLF_COL1))
		{
			_color->copy(_def->getColor1());
		}
	}

	// モデルの色を設定
	_model->setColor(_color);
	// モデル位置設定
	reflectPosture(true);
	
	// シーングラフに追加する
	_sysRef->regSg(this);

	// テクスチャアニメーションのフレーム値を利用する場合は、フレーム数を取得・上書き
	if(_lifeframe == LIFEFRAME_TEXANIM)
	{
		_lifeframe = getTexAnimFrameNum();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	消滅させる
**//*---------------------------------------------------------------------*/
void Particle::disappear()
{
	// シーングラフから削除する
	_sysRef->unregSg(this);

	// リセット
	reset();
}

/*---------------------------------------------------------------------*//**
	位置を反映する
**//*---------------------------------------------------------------------*/
void Particle::reflectPosture(bool isForceModelCalcMatrix)
{
	// モデル位置設定
	if(_rotz != 0.0f)
	{
		Vector3F vrot(0.0f, 0.0f, _rotz);
		_model->setPosition(_pos, &vrot);
	}
	else
	{
		_model->setPosition(_pos, 0L);
	}
	if(_scale != 1.0f)
	{
		Vector3F vscl(_scale, _scale, _scale);
		_model->setScale(&vscl);
	}

	// モデルの姿勢行列を強制計算
	if(isForceModelCalcMatrix)
	{
		_model->calcPostureMatrixByManual();
	}

	// 色の反映
	if(TFW_IS_FLAG(_cflags, CF_EXT_COLOR) && TFW_IS_FLAG(_fflags, FF_CHANGE_COLOR))
	{
		_model->setColor(_color);
		TFW_SET_FLAG(_fflags, FF_CHANGE_COLOR, false);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Particle::Particle()
	: _model(0L)
	, _sysRef(0L)
	, _pos(0L)
	, _vel(0L)
	, _scale(1.0f)
	, _rotz(0.0f)
	, _color(0L)
	, _alphaOrg(0)
	, _alphaMul(0.0f)
	, _cflags(0)
	, _fflags(0)
	, _lifeframe(0)
	, _cntFrame(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Particle::~Particle()
{
	ASSERT(_model == 0L);
	ASSERT(_pos == 0L);
	ASSERT(_vel == 0L);
	ASSERT(_color == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Particle::create(ParticleSys* ptclsysRef)
{
	// システムへの参照を保存
	_sysRef = ptclsysRef;

	// モデルが残っている場合は削除
	if(_model != 0L)
	{
		_model->destroy();
		delete _model;
		_model = 0L;
	}

	// 位置ベクトルを作成する
	_pos = new Vector3F();
	// 速度ベクトルを作成する
	_vel = new Vector3F();
	// 色を作成する
	_color = new ColorU8(255, 255, 255, 255);
	_alphaOrg = 255;
	_alphaMul = 1.0f;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Particle::destroy()
{
	// 残っている場合は消滅処理
	if(isValid())
	{
		disappear();
	}

	// モデルが残っている場合は削除
	if(_model != 0L)
	{
		_model->destroy();
		delete _model;
		_model = 0L;
	}

	// 色を削除する
	delete _color;
	_color = 0L;
	// 位置ベクトルを削除する
	delete _pos;
	_pos = 0L;
	// 速度ベクトルを削除する
	delete _vel;
	_vel = 0L;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void Particle::reset()
{
	// 複製したモデルを削除
	if(_model != 0L)
	{
		_model->destroy();
		delete _model;
		_model = 0L;
	}

	// 値のクリア
	_pos->set(0.0f, 0.0f, 0.0f);
	_vel->set(0.0f, 0.0f, 0.0f);
	_scale = 1.0f;
	_rotz = 0.0f;
	_color->set(255, 255, 255, 255);
	_alphaOrg = 255;
	_alphaMul = 1.0f;
	_cflags = 0;
	_fflags = 0;
	_lifeframe = 0;
	_cntFrame = 0.0f;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void Particle::exec(const ExecCtx* ec)
{
	if(_model == 0L)	{	return;	}

	// フレーム進行と表示・非表示制御
	if(_cntFrame < 1.0f)	// 遅延フレーム中
	{
		// フレームを進める
		_cntFrame += ec->getDeltaFrame();

		// 実行開始
		if(_cntFrame > 0.0f)
		{
			_cntFrame = 1.0f;

			// 表示に設定
			_model->setShow(true);
		}
		else if(_model->isShow())
		{
			// 非表示に設定
			_model->setShow(false);
		}
	}
	else	// 表示中
	{
		// フレームを進める
		_cntFrame += ec->getDeltaFrame();
		///STRACELN("%d:%f/%f", _def->getParticleDefId(), _cntFrame, _lifeframe);

		// 生存時間による自動消滅
		if((_lifeframe >= 0) && (_cntFrame > _lifeframe))
		{
			disappear();
		}
	}

	if(_cntFrame >= 1.0f)	// 実行中（遅延フレーム解消後）
	{
		// 位置更新
		if(!TFW_IS_FLAG(_cflags, ParticleDef::CF_NO_CALC_VEL))	// 速度計算を行う
		{
			// 速度を適用
			_pos->x() += _vel->x() * ec->getDeltaFrame();
			_pos->y() += _vel->y() * ec->getDeltaFrame();
			_pos->z() += _vel->z() * ec->getDeltaFrame();
		}

		// 色を指定
		if(TFW_IS_FLAG(_cflags, CF_EXT_COLOR))	// 外部からの色指定に従う
		{
			if(TFW_IS_FLAG(_fflags, FF_CHANGE_COLOR))	// 色が変更されたときのみ
			{
				_model->setColor(_color);
				TFW_SET_FLAG(_fflags, FF_CHANGE_COLOR, false);
			}
		}
		else if((_def != 0L) && TFW_IS_FLAG(_def->getColorFlag(), ParticleDef::COLF_COL2))
		{
			if(_lifeframe > 0)
			{
				f32 rate = _cntFrame / (f32)_lifeframe;
				Gcalc::interpolate(_color, _def->getColor1(), _def->getColor2(), rate);
				_model->setColor(_color);
			}
		}

		// モデル位置設定
		reflectPosture(false);	// モデルの姿勢行列計算はモデルの exec に任せる

		// フレームフラグをクリア
		_fflags = 0;
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Particle::render(const RenderCtx* rc)
{
	// モデルの描画はシーングラフにて行う
}

/*---------------------------------------------------------------------*//**
	モデルから、テクスチャアニメーションのフレーム数を得る
**//*---------------------------------------------------------------------*/
s32 Particle::getTexAnimFrameNum() const
{
	if(_model == 0L)	{	return 0;	}

	// テクスチャアニメーションを取得する
	const Shape* shape = 0L;
	switch(_model->getModelClass())
	{
	case Model::MODELCLASS_BILLBOARD:
		shape = ((BillboardModel*)_model)->getShape();
		break;
	case Model::MODELCLASS_SHAPE:
		shape = (((ShapeModel*)_model)->getShapeNum() > 0) ? ((ShapeModel*)_model)->getShape(0) : 0L;
		break;
	}
	if(shape == 0L)		{	return 0;	}
	const Material* mtrl = shape->getMaterial();
	if(mtrl == 0L)		{	return 0;	}
	const Texture* tex = mtrl->getTexture();
	if(tex == 0L)		{	return 0;	}
	const TexAnim* texanim = tex->getTexAnim();
	if(texanim == 0L)	{	return 0;	}

	return (s32)texanim->getFrameNum();
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
