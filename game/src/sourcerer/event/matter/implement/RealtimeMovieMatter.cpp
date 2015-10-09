/***********************************************************************//**
 *	RealtimeMovieMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "RealtimeMovieMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../EventSys.h"
#include "../../EventData.h"
#include "../../../body/Game.h"
#include "../../../body/GameNotifier.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../gui/GameGui.h"

// External
#include "../../../../etk/sg/CamSgnc.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
#include "../../../../tfw/common/ResourcePool.h"
#include "../../../../tfw/g3d/sg/node/implement/AnimCamera.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// RealtimeMovieMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void RealtimeMovieMatter::setting(u32 fileid)
{
	_fileid = fileid;
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool RealtimeMovieMatter::analyzeSettingFromXml(u32* fileid, const XmlNode* node)
{
	// ［必須］ファイル ID
	s32 wk = 0;
	if(!XmlParserUtils::analyzeAtrb(&wk, node, "file_id"))
	{
		return false;
	}
	*fileid = (u32)wk;

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
RealtimeMovieMatter::RealtimeMovieMatter()
	: EventMatter(EMTYPE_REALTIMEMOVIE)
	, _camRtmvRef(0L)
	, _camidBk(0)
	, _mdlarrRef(0L)
	, _numMdl(0)
	, _fileid(0)
	, _hintNoAnimIntp(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
RealtimeMovieMatter::~RealtimeMovieMatter()
{
	ASSERT(_mdlarrRef == 0L);
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void RealtimeMovieMatter::destroy()
{
	EventMatter::destroy();
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void RealtimeMovieMatter::begin(u32 param)
{
	EventMatter::begin(param);

	SorSgm* sgm = Game::getGame()->getSceneGraph();

	// 可変サイズスクリーンの切り替え
	Game::getGame()->changeScreenSizeMode(false);

	// モデルを読み込み、リソースプールとシーングラフに追加
	ASSERT(_fileid != 0);
	{
		ShapeModel** mdlarr = 0L;
		s32 numMdl = 0;
		if(!ModelLoader::loadModelsFromTbf(Game::getGame()->getTfw(), _fileid, TbfDefineEx::MEXF_RTMV, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model***)&mdlarr, &numMdl))
		{
			ASSERT(false);
			return;
		}
		_mdlarrRef = mdlarr;
		_numMdl = numMdl;
		// アニメーションとフラグをセット
		u16 animid = 1;
		for(int i = 0; i < _numMdl; i++)
		{
			_mdlarrRef[i]->setModelFlag(Model::MFLAG_ANIM_VIEWPOS, true);	// ビュー座標計算時にアニメーションによる移動を考慮する
			//_mdlarrRef[i]->setModelFlag(Model::MFLAG_NO_CLIP, true);		// クリップ無しに

			// アニメーションセット
			if(animid <= _mdlarrRef[i]->getMaxAnimId())
			{
				_mdlarrRef[i]->setAnimId(animid, 0, false, true, false, false, false);
			}
		}
	}

	// シーングラフ及びリソースプールにカメラを追加
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
	// リアルタイムムービー用のカメラを追加
	if((_mdlarrRef != 0L) && (_mdlarrRef[0]->getCamAnim() != 0L))	// アニメーションカメラがモデルにある場合
	{
		// アニメーションカメラ
		AnimCamera* ancam = new AnimCamera();
		if(!ancam->create())
		{
			ASSERT(false);
			return;
		}
		if(sgm->addNode(ancam, true))
		{
			// カメラ復旧用に、現在のカメラの ID を得ておく
			_camidBk = 0;
			// ただし、既存の RealtimeMovieMatter がある場合はそちらの復旧用カメラ ID をコピーする
			EventData* evdat = getOwner()->getEventData();	ASSERT(evdat != 0L);
			for(s32 i = 0; i < evdat->getEventMatterNum(); i++)
			{
				EventMatter* matter = evdat->getEventMatter(i);
				if(matter->getMatterType() == EMTYPE_REALTIMEMOVIE)	// RealtimeMovieMatter 判定
				{
					if(matter->isBegin())
					{
						if(matter->getEvId() != this->getEvId())
						{
							_camidBk = ((RealtimeMovieMatter*)matter)->_camidBk;
							break;
						}
					}
				}
			}
			// 既存の RealtimeMovieMatter が存在しない場合は現在のカメラから
			if(_camidBk == 0)
			{
				Camera* cam = sgm->getVisibleCamera();
				_camidBk = (cam != 0L) ? cam->getId() : 0;
			}

			// カメラ切替
			ancam->setAnim(_mdlarrRef[0]->getCamAnim());	// アニメーションを設定
			nodecCam->switchVisible(ancam->getId());		// カメラを有効化
			_camRtmvRef = ancam;
			TRACE("{RealtimeMovieMatter::begin} Rtmv Max Frame : %d\n", _camRtmvRef->getMaxFrame());
		}
	}

	// アニメーションの補間を禁止する．これを打ち消すのは MovieCutMatter(Mcut)
	_hintNoAnimIntp = Game::getGame()->startNoAnimInterpolation();
	
	#if 0	// test
	{
		ShapeModel** mdlarrEye = 0L;
		s32 numEyeMdl = 0;
		if(!ModelLoader::loadModelsFromTbf(Game::getGame()->getTfw(), TEST_ASA_EYE_TBMB, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model***)&mdlarrEye, &numEyeMdl))
		{
			ASSERT(false);
		}
		_mdlEye = mdlarrEye[0];

		{
			HrAnimSet* aset = _mdlEye->getAnimSet();
			HrAnimSetCtx* asetctx = _mdlEye->getAnimSetContext();
			aset->setAnimId(asetctx, 0, 0, false, false, false);
		}

		for(int i = 0; i < _numMdl; i++)
		{
			ShapeModel* mdl = _mdlarrRef[i];
			for(int j = 0; j < mdl->getShapeNum(); j++)
			{
				Shape* shape = mdl->getShape(j);
				const VcString* dname = shape->getDebugName();	//>getDefineName();
				if(dname != 0L)
				{
					TRACE(">>> [%d][%d] %s\n", i, j, dname->getRaw());
					if(dname->equals("ASA_HIGH:headShape") || dname->equals("ASA_HIGH:head"))
					{
						shape->setRenderAfterCallback(renderEyeShapeCallback, this);
					}
				}
			}
		}
		TRACE(">>> end.\n");

	}
	#endif

	// 再生状態変更通知
	Game::getGame()->getGameNotifier()->notifyChangedRtmvPlaying(true, getEvId());
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void RealtimeMovieMatter::end()
{
	// 再生状態変更通知
	Game::getGame()->getGameNotifier()->notifyChangedRtmvPlaying(false, getEvId());

	// アニメーション補間禁を解除する
	Game::getGame()->endNoAnimInterpolation(_hintNoAnimIntp);

	// シーングラフのモデルを破棄
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(_mdlarrRef != 0L)
	{
		for(int i = 0; i < _numMdl; i++)
		{
			sgm->removeNode(_mdlarrRef[i]);
		}
		delete[] _mdlarrRef;
		_mdlarrRef = 0L;
	}

	// カメラを破棄
	if(_camRtmvRef != 0L)
	{
		// カメラを戻す
		if(	sgm->getVisibleCamera() == _camRtmvRef )	// 削除しようとしているカメラが有効のとき
		{
			CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
			if(sgm->getNodeFromId(_camidBk) != 0L)			// 戻すカメラがまだ存在するときのみ（destroy 中など、無い場合もある）
			{
				nodecCam->switchVisible(_camidBk);	// カメラを戻す
			}
			else
			{
				// デフォルトカメラに戻す
				Camera* camDefault = Game::getGame()->getDefaultCamera();
				if(camDefault != 0L)
				{
					nodecCam->switchVisible(camDefault->getId());	// カメラを戻す
				}
			}
		}

		sgm->removeNode(_camRtmvRef);
		_camRtmvRef = 0L;
	}

	// リソースプールから不要なリソースをガベージする
	ResourcePool* respl = Game::getGame()->getResourcePool();
	respl->removeUnusedSgEntityResource();

	// 可変サイズスクリーンの切り替え
	Game::getGame()->changeScreenSizeMode(true);

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool RealtimeMovieMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 動作設定解析

	analyzeSettingFromXml(&_fileid, node);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void RealtimeMovieMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	if(_camRtmvRef != 0L)
	{
		if(_camRtmvRef->isDoneAnim() || !_camRtmvRef->isVisible())
		{
			// 終了
			end();
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
