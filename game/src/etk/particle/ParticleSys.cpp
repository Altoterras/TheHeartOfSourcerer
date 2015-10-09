/***********************************************************************//**
 *	ParticleSys.cpp
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
#include "ParticleSys.h"

// Friends
#include "Particle.h"
#include "ParticleDefTbl.h"

// External
#include "../../tfw/file/IdFile.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/lib/Vector.h"

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// マクロ

// パーティクルモデルを先に読み込む
#define PRELOAD_PM		0		// 0 の場合はその場で（強引に）読み込む

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ
	
/*---------------------------------------------------------------------*//**
	パーティクルを得る
**//*---------------------------------------------------------------------*/
Particle* ParticleSys::getParticle(u16 ptclid) const
{
	ASSERT((1 <= ptclid) && (ptclid <= NUM_PARTICLE_MAX));
	if((ptclid < 1) || (NUM_PARTICLE_MAX < ptclid))	{	return 0L;	}
	return &_ptclarrSlot[ptclid - 1];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	パーティクルを出現させる
**//*---------------------------------------------------------------------*/
u16 ParticleSys::appearParticle(s32 ptcldid, const Vector3F* pos, const ColorU8* color, u32 cflags)
{
	// 定義を得る
	const ParticleDef* def = 0L;
#if 0
	if(!TFW_IS_FLAG(cflags, ParticleDef::CF_CUSTOM_BEHAVIOR))
	{
		def = _ptcldftblRef->getDef(ptcldid);
	}
#else
	def = _ptcldftblRef->getDef(ptcldid);
	if(def == 0L)
	{
		return 0;
	}
#endif

	// パーティクルモデルを探す
#if 0
	s32 pmidx = findParticleModel(def != 0L ? def->getModelId() : ptcldid);
#else
	s32 pmidx = findParticleModel(def->getModelId());
#endif
	if(pmidx < 0)
	{
		return 0;
	}

	// 空きのパーティクススロットを得る
	s32 ptclidx = findUnusedParticleSlot();
	if(ptclidx < 0)
	{
		TRACE("{ParticleSys::appearParticle} particle num over.\n");
		return 0;	// 失敗．最大値を超えている
	}
	ASSERT((0 <= ptclidx) && (ptclidx < NUM_PARTICLE_MAX));

	// パーティクル生成
	Particle* ptcl = &_ptclarrSlot[ptclidx];
	ptcl->appear(_pmarr[pmidx]._model, pos, def);
	ptcl->setCtrlFlags(cflags);
	if(color != 0L)
	{
		ptcl->setColor(color);
	}

	return (s16)(ptclidx + 1);
}

/*---------------------------------------------------------------------*//**
	指定パーティクルと同じパーティクルを出現させる
**//*---------------------------------------------------------------------*/
u16 ParticleSys::appearParticle(u16 ptclidSrc)
{
	// 空きのパーティクススロットを得る
	s32 ptclidxNew = findUnusedParticleSlot();
	if(ptclidxNew < 0)
	{
		TRACE("{ParticleSys::appearParticle} particle num over.\n");
		return 0;	// 失敗．最大値を超えている
	}
	ASSERT((0 <= ptclidxNew) && (ptclidxNew < NUM_PARTICLE_MAX));

	// 複製元パーティクルを得る
	const Particle* ptclSrc = getParticle(ptclidSrc);
	if(ptclSrc == 0L)
	{
		return 0;	// 指定パーティクルが存在しない
	}

	// パーティクル生成
	Particle* ptclNew = &_ptclarrSlot[ptclidxNew];
	ptclNew->appear(ptclSrc->getModel(), ptclSrc->getPosition(), ptclSrc->getDef());
	ptclNew->setCtrlFlags(ptclSrc->getCtrlFlags());

	return (s16)(ptclidxNew + 1);
}

/*---------------------------------------------------------------------*//**
	パーティクルを消滅させる
**//*---------------------------------------------------------------------*/
void ParticleSys::disappearParticle(u16 ptclid)
{
	ASSERT((1 <= ptclid) && (ptclid <= NUM_PARTICLE_MAX));
	Particle* ptcl = &_ptclarrSlot[ptclid - 1];
	if(ptcl->isValid())
	{
		ptcl->disappear();
	}
}

/*---------------------------------------------------------------------*//**
	全てのパーティクルを消滅させる
**//*---------------------------------------------------------------------*/
void ParticleSys::disappearAllParticle()
{
	for(int i = 0; i < NUM_PARTICLE_MAX; i++)
	{
		Particle* ptcl = &_ptclarrSlot[i];
		if(ptcl->isValid())
		{
			ptcl->disappear();
		}
	}
}

/*---------------------------------------------------------------------*//**
	シーングラフに登録
**//*---------------------------------------------------------------------*/
void ParticleSys::regSg(Particle* ptcl)
{
	// シーングラフに追加
	_sgncRef->addChildNode(ptcl->getModel());
}

/*---------------------------------------------------------------------*//**
	シーングラフから登録抹消
**//*---------------------------------------------------------------------*/
void ParticleSys::unregSg(Particle* ptcl)
{
	// シーングラフから削除
	_sgncRef->removeChildNode(ptcl->getModel());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ParticleSys::ParticleSys()
	: _ptclarrSlot(0L)
	, _idxPtclModelLoaded(0)
	, _ptcldftblRef(0L)
	, _sgncRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ParticleSys::~ParticleSys()
{
	ASSERT(_ptclarrSlot == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ParticleSys::create(const ParticleDefTbl* ptcldftblRef, SgNodeChain* sgncTrgRef)
{
	// 参照保存
	_ptcldftblRef = ptcldftblRef;
	ASSERT(_ptcldftblRef != 0L);
	_sgncRef = sgncTrgRef;
	ASSERT(_sgncRef != 0L);

	// パーティクル配列を作成
	_ptclarrSlot = new Particle[NUM_PARTICLE_MAX];
	if(_ptclarrSlot == 0L)		{	return false;	}
	for(int i = 0; i < NUM_PARTICLE_MAX; i++)
	{
		if(!_ptclarrSlot[i].create(this))	{	goto Error;	}
	}

	#if PRELOAD_PM
		// オリジナルパーティクルモデルの読み込み
		loadParticleModelRange(1, 27);
		loadParticleModelRange(301, 307);
		loadParticleModelRange(501, 507);
	#endif

	return true;

Error:
	if(_ptclarrSlot != 0L)
	{
		delete[] _ptclarrSlot;
		_ptclarrSlot = 0L;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ParticleSys::destroy()
{
	// オリジナルパーティクルモデルの削除
	for(int i = 0; i < NUM_PARTICLE_MODEL; i++)
	{
		if(_pmarr[i]._model != 0L)
		{
			_pmarr[i]._model->setUsed(false);	// リソースプールが削除できるように使用中フラグを下ろす
			_pmarr[i]._model = 0L;
			_pmarr[i]._pmdlid = 0;
		}
	}

	// パーティクル配列を削除
	if(_ptclarrSlot != 0L)
	{
		for(int i = 0; i < NUM_PARTICLE_MAX; i++)
		{
			_ptclarrSlot[i].destroy();
		}
		delete[] _ptclarrSlot;
		_ptclarrSlot = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void ParticleSys::exec(ExecRes* res, const ExecCtx* ec)
{
	for(int i = 0; i < NUM_PARTICLE_MAX; i++)
	{
		if(_ptclarrSlot[i].isValid())	{	_ptclarrSlot[i].exec(ec);	}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ParticleSys::render(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void ParticleSys::notifyChangedMap()
{
	// パーティクルを全て消す
	for(int i = 0; i < NUM_PARTICLE_MAX; i++)
	{
		if(_ptclarrSlot[i].isValid())
		{
			_ptclarrSlot[i].disappear();
		}
	}

	// 読み捨てモデルを破棄する
	for(int i = 0; i < NUM_PARTICLE_MODEL; i++)
	{
		if(_pmarr[i]._model != 0L)
		{
			if(_pmarr[i]._pmdlid >= PMDLID_TEMP__START_)
			{
				_pmarr[i]._model->setUsed(false);	// リソースプールが削除できるように使用中フラグを下ろす
				_pmarr[i]._model = 0L;
				_pmarr[i]._pmdlid = 0;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	パーティクルモデルを範囲読み込む
**//*---------------------------------------------------------------------*/
bool ParticleSys::loadParticleModelRange(s32 ptcldidStart, s32 ptcldidEnd)
{
	bool isSucceeded = true;

	for(s32 ptcldid = ptcldidStart; ptcldid <= ptcldidEnd; ptcldid++)
	{
		if(!loadParticleModel(ptcldid))
		{
			isSucceeded = false;
		}
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	パーティクルモデルを読み込む
**//*---------------------------------------------------------------------*/
s32 ParticleSys::loadParticleModel(s32 pmdlid)
{
	Model* model = loadModel(pmdlid);
	if(model == 0L)
	{
		ASSERT(false);
		return -1;
	}

	s32 idx = _idxPtclModelLoaded;
	ASSERT(idx < NUM_PARTICLE_MODEL);
	_pmarr[idx]._model = model;
	_pmarr[idx]._pmdlid = pmdlid;
	_pmarr[idx]._model->setUsed(true);	// 使用中フラグを立てる
	_idxPtclModelLoaded++;
	return idx;
}

/*---------------------------------------------------------------------*//**
	パーティクルモデルを探す
**//*---------------------------------------------------------------------*/
s32 ParticleSys::findParticleModel(s32 pmdlid)
{
	if(pmdlid == 0)	{	return -1;	}

	for(int i = 0; i < NUM_PARTICLE_MODEL; i++)
	{
		if(_pmarr[i]._pmdlid == pmdlid)
		{
			return i;
		}
	}

	#if PRELOAD_PM
		// 先読みの場合は見つからなかったら無効値を返す
		return -1;
	#else
		// 見つからなかったらその場で読む
		s32 idx = loadParticleModel(pmdlid);
		return idx;
	#endif
}

/*---------------------------------------------------------------------*//**
	未使用のパーティクルスロットのインデックスを得る
**//*---------------------------------------------------------------------*/
s32 ParticleSys::findUnusedParticleSlot() const
{
	for(int i = 0; i < NUM_PARTICLE_MAX; i++)
	{
		if(!_ptclarrSlot[i].isValid())	{	return i;	}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
