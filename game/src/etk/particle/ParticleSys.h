/***********************************************************************//**
 *	ParticleSys.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/11/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_PARTICLE_PARTICLE_SYS_H_
#define _ETK_PARTICLE_PARTICLE_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

#include "ParticleDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Model;
class SgNodeChain;

TFW_END_NS

ETK_BEGIN_NS

class Game;
class Particle;
class ParticleDefTbl;
class SorSgm;

/*---------------------------------------------------------------------*//**
 *	パーティクル システム
 *
**//*---------------------------------------------------------------------*/
class ParticleSys : public FrameExecDraw
{
	//======================================================================
	// 定数
protected:
	static const u16		NUM_PARTICLE_MAX	= 128;		// 最大表示パーティクル数
	static const u16		NUM_PARTICLE_MODEL	= 64;		// 最大パーティクルモデル数

	static const s32		PMDLID_PROG__START_	= 20000;	// プログラム制御パーティクルモデルの開始 ID
	static const s32		PMDLID_PROG__END_	= 29999;	// プログラム制御パーティクルモデルの終了 ID
	static const s32		PMDLID_TEMP__START_	= 30000;	// 読み捨てパーティクルモデルの開始 ID

	//======================================================================
	// クラス
private:
	class PtclModel
	{
	public:
		s32		_pmdlid;	// パーティクルモデル ID
		Model*	_model;

		PtclModel() : _pmdlid(0), _model(0L) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// パーティクルを得る
	Particle* getParticle(u16 ptclid) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	u16 appearParticle(s32 ptcldid, const Vector3F* pos, const ColorU8* color, u32 cflags);
	inline u16 appearParticle(s32 ptcldid, const Vector3F* pos, u32 cflags) { return appearParticle(ptcldid, pos, 0L, cflags); }
	u16 appearParticle(u16 ptclidSrc);
	void disappearParticle(u16 ptclid);
	void disappearAllParticle();

public:
	void regSg(Particle* ptcl);
	void unregSg(Particle* ptcl);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ParticleSys();
	virtual ~ParticleSys();
	virtual bool create(const ParticleDefTbl* ptcldftblRef, SgNodeChain* sgncTrgRef);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	virtual void render(const RenderCtx* rc);			// 描画
	virtual void notifyChangedMap();

protected:
	virtual Model* loadModel(s32 pmdlid) = 0;

private:
	bool loadParticleModelRange(s32 ptcldidStart, s32 ptcldidEnd);
	s32 loadParticleModel(s32 pmdlid);
	s32 findParticleModel(s32 pmdlid);
	s32 findUnusedParticleSlot() const;

	//======================================================================
	// 変数
private:
	Particle* _ptclarrSlot;
	PtclModel _pmarr[NUM_PARTICLE_MODEL];
	s32 _idxPtclModelLoaded;

	const ParticleDefTbl* _ptcldftblRef;
	SgNodeChain* _sgncRef;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_PARTICLE_PARTICLE_SYS_H_
