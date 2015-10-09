/***********************************************************************//**
 *	AutoDecorationEffect.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/08.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AutoDecorationEffect.h"

// Friends
#include "../file/GameFileIdDef.h"
#include "../char/CharDef.h"
#include "../char/CharDefTbl.h"
#include "../common/CalcUtils.h"
#include "../episode/data/FixedData.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/SubtitlesWindow.h"
#include "../event/EventData.h"
#include "../event/EventSys.h"
#include "../event/matter/implement/ActMatter.h"
#include "../event/matter/implement/RealtimeMovieMatter.h"
#include "../event/matter/implement/TalkMatter.h"
#include "../event/matter/implement/act/Act.h"
#include "../event/matter/implement/act/implement/Msg.h"
#include "../event/matter/EventMatterType.h"
#include "../model/CircleShadow.h"
#include "../model/spid/Spid.h"

// External
#include "../../etk/sg/TbfDefineEx.h"
#include "../../tfw/collection/Array.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/evsys/MsgData.h"
#include "../../tfw/evsys/MsgDataSet.h"
#include "../../tfw/g3d/HrAnimObjCtx.h"
#include "../../tfw/g3d/HrAnimSet.h"
#include "../../tfw/g3d/HrAnimSetCtx.h"
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AutoDecorationEffect メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	キャラクタを登録する
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::registerChar(u16 charid, bool isFixed)
{
	TargetChar* tc = addChar(charid);
	if(tc == 0L)	{	return false;	}
	tc->_isFixed = isFixed;
	return true;
}

/*---------------------------------------------------------------------*//**
	テクスチャフェイシャル設定変更
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::setTexFacialConfig(u32 flagsSet, u16 charid, u8 kind, bool isEnable, u8 randNextAnim, s32 durfNextAnim)
{
	if(charid == 0L)							{	return false;	}
	if(_arrTrgchar == 0L)						{	return false;	}
	if(kind >= NUM_KIND)						{	return false;	}

	// 対象を探す
	TargetChar* tc = findChar(charid);
	if(tc == 0L)								{	return false;	}

	// 設定変更
	if(TFW_IS_FLAG(flagsSet, TFCONFF_ENABLE))
	{
		TFW_SET_FLAG(tc->_effect[kind]._flags, Effect::F_EXT_DISABLE, !isEnable);
	}
	if(TFW_IS_FLAG(flagsSet, TFCONFF_NEXTANIMRAND))
	{	
		tc->_effect[kind]._randNextAnimConf = randNextAnim;
	}
	if(TFW_IS_FLAG(flagsSet, TFCONFF_NEXTANIMDURF))
	{
		tc->_effect[kind]._midurfNextAnimConf = durfNextAnim * 1024;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	丸影設定変更
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::setCircleShadowConfig(u32 flagsSet, u16 charid, bool isEnable, f32 radius, const Vector3F* offsetPos)
{
	if(charid == 0L)							{	return false;	}
	if(_arrTrgchar == 0L)						{	return false;	}

	// 対象を探す
	TargetChar* tc = findChar(charid);
	if(tc == 0L)								{	return false;	}
	Effect* e = &tc->_effect[KIND_SHADOW];

	// 設定変更
	if(TFW_IS_FLAG(flagsSet, CSCONFF_ENABLE))
	{
		TFW_SET_FLAG(e->_flags, Effect::F_EXT_DISABLE, !isEnable);
	}
	if(TFW_IS_FLAG(flagsSet, CSCONFF_RADIUS))
	{
		e->_radius = radius;

		// モデルへ反映
		if(e->_mdlAdd != 0L)
		{
			((CircleShadow*)e->_mdlAdd)->setRadius(e->_radius);
		}
		else
		{	
			e->createCircleShadow(tc->_charid, tc->_smdlOwnRef);
		}
	}
	if(TFW_IS_FLAG(flagsSet, CSCONFF_OFFSET) && (offsetPos != 0L))
	{
		if(e->_offsetPos != 0L)
		{
			e->_offsetPos->copy(offsetPos);
		}
		else
		{
			e->_offsetPos = new Vector3F(offsetPos);
		}

		// モデルへ反映
		if(e->_mdlAdd != 0L)
		{
			((CircleShadow*)e->_mdlAdd)->setOffsetPos(e->_offsetPos->x(), e->_offsetPos->z());
		}
		else
		{
			e->createCircleShadow(tc->_charid, tc->_smdlOwnRef);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	エフェクト制御開始
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::start(u16 evidRtmvMatter)
{
	if(_arrTrgchar == 0L)						{	return;	}
	if(_arrTrgchar->getCount() <= 0)			{	return;	}

	// 開始済みの場合は強制終了
	if(_isBegan)
	{
		end(0);
	}

	// リアルタイムムービーマターおよびアクトマターを得る
	EventData* evdat = Game::getGame()->getEventSys()->getEventData();	ASSERT(evdat != 0L);
	RealtimeMovieMatter* matterRtmv = 0L;
	ActMatter* matterAct = 0L;
	{
		EventMatter* matterWk1 = evdat->getEventMatterFromEvid(evidRtmvMatter);
		if((matterWk1->getMatterType() == EMTYPE_REALTIMEMOVIE) && matterWk1->isBegin())	// 開始済み RealtimeMovieMatter 判定
		{
			if(matterWk1->getParentEvId() != 0)
			{
				EventMatter* matterWk2 = evdat->getEventMatterFromEvid(matterWk1->getParentEvId());
				if(matterWk2 != 0L)
				{
					if((matterWk2->getMatterType() == EMTYPE_ACT) && matterWk2->isBegin())	// 開始済み ActMatter 判定
					{
						matterRtmv = (RealtimeMovieMatter*)matterWk1;
						matterAct = (ActMatter*)matterWk2;
					}
				}
			}
		}
	}
	if((matterRtmv == 0L) || (matterAct == 0L))	{	return;	}

	// リアルタイム ムービー マターのイベント ID を保存
	_evidRtmv = matterRtmv->getEvId();

	// ムービーに内在するキャラとアニメオブジェを登録する
	{
		List<Spid::EnumRes*> listSpid(true);
		for(int i = 0; i < matterRtmv->getShapeModelNum(); i++)
		{
			ShapeModel* smdl = (matterRtmv->shapeModelArray())[i];

			// モデル中の SPID とアニメオブジェを検索・列挙する
			Spid::enumerateHrAnimObjCtx(&listSpid, smdl);

			for(ListIterator<Spid::EnumRes*> it = listSpid.iterator(); it.has(); it.next())
			{
				Spid::EnumRes* eres = it.object();
				if((Spid::SPID_LC_CHAR_HEAD__START_ <= eres->_spid) && (eres->_spid <= Spid::SPID_LC_CHAR_HEAD__END_))
				{
					u16 charid = eres->_spid - Spid::SPID_LC_CHAR_HEAD__START_;
					TargetChar* tc = addChar(charid);		// 登録追加
					if(tc != 0L)	// 登録成功
					{
						tc->_haocSpotRef = eres->_haobjctx;			// アニメオブジェを解決
						tc->_smdlOwnRef = smdl;						// 対象キャラモデルを解決
						Spid::findRoot(&tc->_haocRootRef, smdl);	// ルートアニメオブジェを解決
					}
				}
			}
			listSpid.removeAll();
		}
	}

	// アクトマター中のメッセージを全て確認し、登録済みキャラの口パクアニメを有効化する
	List<Act*>* listAct = matterAct->getAllActList();
	if(listAct != 0L)
	{
		const MsgDataSet* msgdatset = evdat->getMsgDataSet(); ASSERT(msgdatset != 0L);
		MsgData::FindHint hint;
		for(ListIterator<Act*> it = listAct->iterator(); it.has(); it.next())
		{
			if(it.object()->_type == ACT_MSG)
			{
				const Msg* msgact = (Msg*)it.object();
				const MsgData::Msg* msg = msgdatset->findMessageFromMsgid(msgact->_setting._msgid, &hint);
				u16 charid = (u16)msg->_param1;
				TargetChar* tc = findChar(charid);	// 登録済検索
				if(tc != 0L)	// 検索
				{
					// 口パクの内部有効化
					TFW_SET_FLAG(tc->_effect[KIND_MOUTH]._flags, Effect::F_INN_DISABLE, false);
				}
			}
		}
	}

	// 各対象キャラの開始処理
	for(u32 itrg = 0; itrg < _arrTrgchar->getCount(); itrg++)
	{
		_arrTrgchar->getAt(itrg)->begin();
	}

	// 台詞ウインドウの取得
	_wndMsgRef = Game::getGame()->getGui()->getSubtitlesWindow();
	ASSERT(_wndMsgRef != 0L);

	// 開始した
	_isBegan = true;
}

/*---------------------------------------------------------------------*//**
	エフェクト制御終了

	@param evidRtmvMatter RTMV マターのイベント ID
				※ 合致しない RTMV に対する制御終了は受け付けない
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::end(u16 evidRtmvMatter)
{
	if((evidRtmvMatter > 0) && (_evidRtmv > 0) && (_evidRtmv != evidRtmvMatter))
	{
		return;
	}

	reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AutoDecorationEffect::AutoDecorationEffect()
	: _arrTrgchar(0L)
	, _wndMsgRef(0L)
	, _evidRtmv(0)
	, _isBegan(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AutoDecorationEffect::~AutoDecorationEffect()
{
	ASSERT(_arrTrgchar == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::create()
{
	// 管理配列を作成する
	_arrTrgchar = new Array<TargetChar*>(false, true);
	if(!_arrTrgchar->alloc(NUM_TARGET))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::destroy()
{
	reset();

	// 管理配列を削除する
	delete _arrTrgchar;
	_arrTrgchar = 0L;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::reset()
{
	if(_arrTrgchar == 0L)	{	return;	}

	for(u32 itrg = 0; itrg < _arrTrgchar->getCount(); itrg++)
	{
		_arrTrgchar->getAt(itrg)->reset();
	}
	_arrTrgchar->removeAll(true);

	_evidRtmv = 0;
	_isBegan = false;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isBegan)	{	return;	}	// 開始していない

	GameExecCtx* gec = (GameExecCtx*)ec;
	EventData* evdat = Game::getGame()->getEventSys()->getEventData(); ASSERT(evdat != 0L);

	// ムービーが続行中かチェック
	if(_evidRtmv != 0L)	// 継続中
	{
		RealtimeMovieMatter* matterRtmv = (RealtimeMovieMatter*)evdat->getEventMatterFromEvid(_evidRtmv);
		if((matterRtmv == 0L) || matterRtmv->isEnd())	// 終了している場合
		{
			// 固定キャラ以外を削除
			u32 itrg = 0;
			while(itrg < _arrTrgchar->getCount())
			{
				TargetChar* tc = _arrTrgchar->getAt(itrg);
				if(tc->_isFixed)
				{
					tc->reset();	// 固定キャラはリセット
				}
				else
				{
					_arrTrgchar->remove(itrg);	// 削除
					continue;
				}
				itrg++;
			}
			// 自動終了
			end(0);
			return;
		}
	}
	else
	{
		ASSERT(false);
		return;
	}

	// 現在しゃべっているキャラクタを得る
	u16 charidSaying = 0;
	bool isReading = false;
	if(_wndMsgRef != 0L)
	{
		u32 evidMsg = _wndMsgRef->getGeneralParameter();
		if(evidMsg != 0)
		{
			EventMatter* matterWk = evdat->getEventMatterFromEvid(evidMsg);
			if((matterWk != 0L) && (matterWk->getMatterType() == EMTYPE_TALK))
			{
				TalkMatter* matterTalk = (TalkMatter*)matterWk;
				TalkMatter::Data* data = matterTalk->getData(0);
				charidSaying = data->_charid;
				isReading = _wndMsgRef->isReadingAnimation();
			}
		}
	}

	// 各対象キャラを制御
	for(u32 itrg = 0; itrg < _arrTrgchar->getCount(); itrg++)
	{
		_arrTrgchar->getAt(itrg)->exec(gec, charidSaying, isReading);
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::render(const RenderCtx* rc)
{
	if(_evidRtmv == 0)	{	return;	}	// ムービーを捕らえてない

	rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);	// 通常合成に設定

	for(u32 itrg = 0; itrg < _arrTrgchar->getCount(); itrg++)
	{
		_arrTrgchar->getAt(itrg)->render(rc);
	}
}

/*---------------------------------------------------------------------*//**
	登録キャラクタを検索する
**//*---------------------------------------------------------------------*/
AutoDecorationEffect::TargetChar* AutoDecorationEffect::findChar(u16 charid)
{
	if(charid == 0L)							{	return 0L;	}
	if(_arrTrgchar == 0L)						{	return 0L;	}

	for(u32 itrg = 0; itrg < _arrTrgchar->getCount(); itrg++)
	{
		TargetChar* tc = _arrTrgchar->getAt(itrg);
		if(tc->_charid == charid)
		{
			// 登録済み
			return tc;
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	キャラクタを登録する
**//*---------------------------------------------------------------------*/
AutoDecorationEffect::TargetChar* AutoDecorationEffect::addChar(u16 charid)
{
	if(charid == 0)								{	return 0L;	}
	if(_arrTrgchar == 0L)						{	return 0L;	}
	if(_arrTrgchar->getCount() >= NUM_TARGET)	{	return 0L;	}

	// 重複チェック
	TargetChar* tc = findChar(charid);
	if(tc != 0L)
	{
		// 登録済み
		return tc;
	}
	
	// 配列に追加
	tc = new TargetChar(charid);
	_arrTrgchar->add(tc);

	return tc;
}

//==========================================================================
// AutoDecorationEffect::Effect メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AutoDecorationEffect::Effect::Effect()
	: _kind(NUM_KIND)
	, _onst(false)
	, _flags(0)
	, _mdlAdd(0L)
	, _randNextAnimConf(0)
	, _midurfNextAnimConf(0)
	, _mifcntIntvl(0)
	, _radius(F32_PMAX)
	, _offsetPos(0L)
{
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::Effect::reset()
{
	// 追加描画モデルを削除
	if(_mdlAdd != 0L)
	{
		if(_kind == KIND_SHADOW)
		{
			_mdlAdd->destroy();
			delete _mdlAdd;
			_mdlAdd = 0L;
		}
		else
		{
			_mdlAdd->setUsed(false);	// リソースプールに使用終了を伝える
		}
		_mdlAdd = 0L;
	}

	// オフセット位置を削除
	delete _offsetPos;
	_offsetPos = 0L;

	_onst = false;
	_mifcntIntvl = 0;
	_radius = F32_PMAX;
}

/*---------------------------------------------------------------------*//**
	開始処理
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::Effect::begin(u16 charid, ShapeModel* smdlOwnRef)
{
	// 値初期化
	_mifcntIntvl = 0;
	_onst = false;

	// 追加描画モデルを読み込む
	if(_kind == KIND_SHADOW)
	{
		if(createCircleShadow(charid, smdlOwnRef))
		{
			// 基本的に表示し続ける
			_onst = true;
		}
	}
	else
	{
		u32 fileidAddMdl = 0;
		switch(_kind)
		{
		case KIND_EYE:		fileidAddMdl = FILEID_FROM_CHAR_EYE_MODEL(charid);		break;
		case KIND_MOUTH:	fileidAddMdl = FILEID_FROM_CHAR_MOUTH_MODEL(charid);	break;
		}
		if(fileidAddMdl == 0)
		{
			ASSERT(false);
			return false;
		}
		if(!IdFile::isIdFileExist(fileidAddMdl))
		{
			return false;
		}
		ShapeModel** mdlarrAdd = 0L;
		s32 numAddMdl = 0;
		if(!ModelLoader::loadModelsFromTbf(Game::getGame()->getTfw(), fileidAddMdl, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_NULL, (Model***)&mdlarrAdd, &numAddMdl))
		{
			ASSERT(false);
			return false;
		}
		_mdlAdd = mdlarrAdd[0];
		ASSERT(_mdlAdd != 0L);
		if(_mdlAdd == 0L)	{	return false;	}
		delete[] mdlarrAdd;

		_mdlAdd->setUsed(true);	// リソースプールに使用中であることを伝える
	}

	// 追加描画モデルの設定
	if(_mdlAdd != 0L)	{	_mdlAdd->setModelFlag(Model::MFLAG_NO_CLIP, true);	}	// とりあえず描画クリップ処理無し

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::Effect::exec(const GameExecCtx* gec, TargetChar* tc, bool isSaying, bool isReading)
{
	if(_mdlAdd == 0L)									{	return;	}	// 制御不要
	if(TFW_IS_FLAG(_flags, Effect::F_EXT_DISABLE))		{	return;	}	// 無効化中
	if(TFW_IS_FLAG(_flags, Effect::F_INN_DISABLE))		{	return;	}	// 無効化中

	// ON / OFF 制御
	switch(_kind)
	{
	case KIND_MOUTH:
		// 口パクの ON / OFF 制御
		{
			if(!isSaying)	// 対象キャラは話していない
			{
				changeOnst(false);	// OFF にする
				break;
			}
			changeOnst(isReading);	// 読み上げていれば ON にする
		}
		break;
	case KIND_EYE:
		// 目パチの ON / OFF 制御
		if(_mifcntIntvl > 0)	// 維持フレーム
		{
			_mifcntIntvl -= gec->getMilliDeltaFrame();
			if(_mifcntIntvl < 0)	{	_mifcntIntvl = 0;	}
		}
		if(_mifcntIntvl <= 0)	// 変更フレーム
		{
			if(_onst)	// 非表示→表示
			{
				changeOnst(false);	// OFF にする
				_mifcntIntvl = _midurfNextAnimConf;
			}
			else		// 表示→非表示
			{
				if((u8)(CalcUtils::randF() * 100.0f) <= _randNextAnimConf)
				{
					changeOnst(true);	// ON にする
					_mifcntIntvl = DURF_EYE_ANIM * 1024;
				}
				else
				{
					_mifcntIntvl = _midurfNextAnimConf;
				}
			}
		}
		break;
	case KIND_SHADOW:
		break;
	}

	// 追加描画モデルの制御
	_mdlAdd->exec(0L, gec);
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::Effect::render(const RenderCtx* rc, TargetChar* tc)
{
	if(_mdlAdd == 0L)									{	return;	}	// 制御不要
	if(TFW_IS_FLAG(_flags, Effect::F_EXT_DISABLE))		{	return;	}	// 無効化中
	if(TFW_IS_FLAG(_flags, Effect::F_INN_DISABLE))		{	return;	}	// 無効化中

	if(_onst)
	{
		::glPushMatrix();

		switch(_kind)
		{
		case KIND_MOUTH:
		case KIND_EYE:
			{
				const Matrix4F* mtxMv = tc->_haocSpotRef->getModelViewMatrix(); ASSERT(mtxMv != 0L);
				if(mtxMv != 0L)
				{
					rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);	// アルファファンクションを通常に
					rc->getRenderer()->setSolidColor(255, 255, 255, 255);			// カラーを基本色に

					::glLoadMatrixf(mtxMv->ptr());
					_mdlAdd->render(rc);
				}
			}
			break;
		case KIND_SHADOW:
			{
				const Matrix4F* mtxMv = tc->_haocRootRef->getModelViewMatrix(); ASSERT(mtxMv != 0L);
				if(mtxMv != 0L)
				{
					::glLoadMatrixf(mtxMv->ptr());
					_mdlAdd->render(rc);
				}
			}
			break;
		}

		::glPopMatrix();
	}
}

/*---------------------------------------------------------------------*//**
	丸影の作成
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::Effect::createCircleShadow(u16 charid, ShapeModel* smdlOwnRef)
{
	if(_mdlAdd != 0L)	{	return true;	}

	const CharDef* chrdef = FixedData::getCharDefTable()->getDef(charid);

	// モデルの半径を得る
	f32 r = 0.0f;
	if(chrdef != 0L)
	{
		r = (f32)chrdef->getShadowRadius();
	}
	if(_radius != F32_PMAX)	// 特別に指定値がある場合
	{
		r = _radius;
	}
	if(r == 0.0f)
	{
		smdlOwnRef->getAllShapesBoundingBox(0L, &r);
		r = r * 0.6f * 1.2f;	// CharUnit::create に準拠
	}

	// 一定大きさ以上なら丸影モデルを作成する
	if(r > 10.0f)
	{
		CircleShadow* mdlCrcShdw = new CircleShadow();
		if(mdlCrcShdw->create())
		{
			_mdlAdd = mdlCrcShdw;			// 参照を保存しておく

			// オフセット位置の設定
			f32 xOffsetPos = 0.0f;
			f32 zOffsetPos = 0.0f;
			if(chrdef != 0L)
			{
				xOffsetPos = (f32)chrdef->getShadowOffsetX();
				zOffsetPos = (f32)chrdef->getShadowOffsetZ();
			}
			if(_offsetPos != 0L)	// 特別に指定値がある場合
			{
				xOffsetPos = _offsetPos->x();
				zOffsetPos = _offsetPos->z();
			}
			mdlCrcShdw->setOffsetPos(xOffsetPos, zOffsetPos);
			// 半径を設定
			mdlCrcShdw->setRadius(r);
			// 不透明度設定
			mdlCrcShdw->setOpacity(153);	// 255 × 0.6 の濃さに．CharUnit::create に準拠
		}
		else
		{
			delete mdlCrcShdw;
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	表示状態の変更
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::Effect::changeOnst(bool onstNew)
{
	if(_onst != onstNew)
	{
		_onst = onstNew;

		// テクスチャアニメーションフレームのリセット
		if(onstNew && ((_kind == KIND_EYE) || (_kind == KIND_MOUTH)))
		{
			((ShapeModel*)_mdlAdd)->resetTexAnimFrame();
		}
	}
}

//==========================================================================
// AutoDecorationEffect::TargetChar メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AutoDecorationEffect::TargetChar::TargetChar(u16 charid)
	: _charid(charid)
	, _isFixed(false)
	, _haocSpotRef(0L)
	, _haocRootRef(0L)
	, _smdlOwnRef(0L)
{
	_effect[KIND_EYE]._kind = KIND_EYE;
	_effect[KIND_MOUTH]._kind = KIND_MOUTH;
	_effect[KIND_SHADOW]._kind = KIND_SHADOW;

	// デフォルトパラメータ
	_effect[KIND_EYE]._randNextAnimConf = RAND_EYE_NEXT_DEFAULT;
	_effect[KIND_EYE]._midurfNextAnimConf = DURF_EYE_NEXT_DEFAULT * 1024;

	// デフォルトでは目パチは有効、口パクは無効
	// 口パクは AutoDecorationEffect::begin 中のメッセージ検索にて有効化する
	TFW_SET_FLAG(_effect[KIND_EYE]._flags, Effect::F_INN_DISABLE, false);	// 目パチ有効
	TFW_SET_FLAG(_effect[KIND_MOUTH]._flags, Effect::F_INN_DISABLE, true);	// 口パク無効
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::TargetChar::reset()
{
	_haocSpotRef = 0L;
	_smdlOwnRef = 0L;

	for(int i = 0; i < NUM_KIND; i++)
	{
		_effect[i].reset();
	}
}

/*---------------------------------------------------------------------*//**
	開始処理
**//*---------------------------------------------------------------------*/
bool AutoDecorationEffect::TargetChar::begin()
{
	if(_haocSpotRef == 0L)	{	return false;	}	// 制御不要

	// 行列を保存するように設定
	_haocSpotRef->setFlags(HrAnimObjCtx::F_SAVE_MV_MTX, true);
	_haocRootRef->setFlags(HrAnimObjCtx::F_SAVE_MV_MTX, true);

	// エフェクトを開始
	for(int i = 0; i < NUM_KIND; i++)
	{
		_effect[i].begin(_charid, _smdlOwnRef);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::TargetChar::exec(const GameExecCtx* gec, u16 charidSaying, bool isReading)
{
	if(_haocSpotRef == 0L)	{	return;	}	// 制御不要

	for(int i = 0; i < NUM_KIND; i++)
	{
		_effect[i].exec(gec, this, _charid == charidSaying, isReading);
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void AutoDecorationEffect::TargetChar::render(const RenderCtx* rc)
{
	if(_haocSpotRef == 0L)	{	return;	}	// 制御不要

	// 対象キャラモデルの描画結果が非表示の場合はエフェクトも描画しない
	ASSERT(_smdlOwnRef != 0L);
	if(TFW_IS_FLAG(_smdlOwnRef->getRenderResFlags(), Model::RRESFLAG_CLIPPED))
	{
		return;	// クリップされた
	}

	for(int i = 0; i < NUM_KIND; i++)
	{
		_effect[i].render(rc, this);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
