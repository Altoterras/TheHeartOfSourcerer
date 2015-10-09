/***********************************************************************//**
 *  HrAnimSet.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimSet.h"

// Friends
#include "HrAnimSetCtx.h"
#include "HrAnimCtx.h"
#include "HrAnim.h"
#include "HrAnimObj.h"
#include "HrAnimObjFrame.h"
#include "HrAnimPlayingFrame.h"
#include "Shape.h"
#include "../gcmn/Gcalc.h"
#include "../lib/Matrix.h"
#include "../lib/Quaternion.h"
#include "../lib/Vector.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define INTP_SPLINE			(0) && TEST_CUBIC_INTP
#define INTP_LINEAR_QTRNN	(1)
#define INTP_SLERP_QTRNN	(0)

////////////////////////////////////////////////////////////////////////////
// 定数

#define FRAME_NARROW		(1.0f)	// 補間用に同フレームでもわずかに間を開けて計算する

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimSet メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	現在のアニメーション ID を得る
**//*---------------------------------------------------------------------*/
u16 HrAnimSet::getCurAnimId(HrAnimSetCtx* asctx) const
{
	if(asctx->_animidxCur == -1)	{	return 0;	}
	return _aarrAnim[asctx->_animidxCur]->getAnimId();
}

/*---------------------------------------------------------------------*//**
	最大のアニメーション ID を得る
**//*---------------------------------------------------------------------*/
u16 HrAnimSet::getMaxAnimId(HrAnimSetCtx* asctx) const
{
	u16 animidMax = 0;
	for(s32 i = 0; i < _numAnim; i++)
	{
		if(animidMax < _aarrAnim[i]->getAnimId())
		{
			animidMax = _aarrAnim[i]->getAnimId();
		}
	}
	return animidMax;
}

/*---------------------------------------------------------------------*//**
	現在のアニメーションを得る
**//*---------------------------------------------------------------------*/
f32 HrAnimSet::getCurAnimFrame(HrAnimSetCtx* asctx) const
{
	if(asctx->_animidxCur == -1)	{	return -1.0f;	}
	return _aarrAnim[asctx->_animidxCur]->getFrame(asctx->_acarrAnimCtx[asctx->_animidxCur]);
}

/*---------------------------------------------------------------------*//**
	ルートオブジェクトの行列を計算する
**//*---------------------------------------------------------------------*/
void HrAnimSet::calcObjRootMatrix(s32 idxRoot, Matrix4F* mtx, HrAnimSetCtx* asctx) const
{
	ASSERT((0 <= idxRoot) && (idxRoot < _numRootAobj));
	_aobjRoot[idxRoot]->calcMatrix(mtx, asctx->_aocaObjCtxArray);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アニメーションオブジェクトの姿勢を直接設定する
**//*---------------------------------------------------------------------*/
bool HrAnimSet::setAnimObjPosture(HrAnimSetCtx* asctx, u16 indexObj, const Vector3F* trs, const Vector3F* rot, const Vector3F* scl)
{
	bool isSucceeded = false;

	HrAnimObjPosture aop(trs, rot, 0L, scl, false);
	for(int i = 0; i < _numRootAobj; i++)
	{
		if(_aobjRoot[i]->setPosture(asctx->_aocaObjCtxArray, indexObj, &aop))	{	isSucceeded = true;	}
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	アニメーション切り替え
**//*---------------------------------------------------------------------*/
void HrAnimSet::setAnimId(HrAnimSetCtx* asctx, u16 animid, s32 frameIntp, bool replaceHeadKeyFrameByCurPos, bool isLoop, bool isChangeFirstFrame, bool isChangeEndFrame)
{
	if(isChangeFirstFrame)		// １フレーム目で切り替える
	{
		asctx->_animidRsvFirst = animid;				// アニメーション ID を予約する
		asctx->_frameIntpRsvFirst = frameIntp;
	}
	else if(isChangeEndFrame)	// 最終フレームで切り替える（現状、１フレーム目切替とは排他）
	{
		asctx->_animidRsvEnd = animid;				// アニメーション ID を予約する
		asctx->_frameIntpRsvEnd = frameIntp;
	}
	else						// 即時切り替える
	{
		setAnimIdImmediately(asctx, animid, frameIntp, replaceHeadKeyFrameByCurPos);
		asctx->_animidRsvFirst = ANIMID_INVALID;		// 予約解除
		asctx->_animidRsvEnd = ANIMID_INVALID;		// 予約解除
	}

	TFW_SET_FLAG(asctx->_flags, HrAnimSetCtx::F_LOOP, isLoop);	// ループ設定フラグ
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimSet::HrAnimSet()
	: _aarrAnim(0L)
	, _numAnim(0)
	, _numShape(0)
	, _aobjRoot(0L)
	, _numRootAobj(0)
	, _numAobjTotal(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnimSet::~HrAnimSet()
{
	ASSERT(_aarrAnim == 0L);
	ASSERT(_aobjRoot == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HrAnimSet::create(HrAnim** animarrDlg, u16 numAnim, HrAnimObj** aobjRootEntr, u16 numRootAobj, u16 numAobjTotal, u16 numShape)
{
	// アニメ配列を受け取る（配列・インスタンスともに委譲はされている）
	_aarrAnim = animarrDlg;
	_numAnim = numAnim;

	// オブジェクト階層配列を受け取る（配列・インスタンスともに委譲はされている）
	_aobjRoot = aobjRootEntr;
	_numRootAobj = numRootAobj;
	_numAobjTotal = numAobjTotal;

	// シェイプ数を保存
	_numShape = numShape;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HrAnimSet::destroy()
{
	// 委譲されたインスタンスの削除
	{
		for(int i = 0; i < _numRootAobj; i++)
		{
			_aobjRoot[i]->destroy();
			delete _aobjRoot[i];
		}
		delete[] _aobjRoot;
		_aobjRoot = 0L;
		_numRootAobj = 0;
		_numAobjTotal = 0;
	}
	{
		for(int i = 0; i < _numAnim; i++)
		{
			_aarrAnim[i]->destroy();
			delete _aarrAnim[i];
		}
		delete[] _aarrAnim;
		_aarrAnim = 0L;
		_numAnim = 0;
	}

	_numShape = 0;
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool HrAnimSet::createContext(HrAnimSetCtx* asctx, Shape** sarr, ShapeCtx** scarr)
{
	return asctx->create(_aarrAnim, _numAnim, _aobjRoot, _numRootAobj, _numAobjTotal, _numShape, sarr, scarr);
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void HrAnimSet::destroyContext(HrAnimSetCtx* asctx)
{
	asctx->destroy(_aarrAnim, _numAnim, _aobjRoot, _numRootAobj, _numShape);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void HrAnimSet::exec(const ExecCtx* ec, HrAnimSetCtx* asctx)
{
	if(TFW_IS_FLAG(ec->getExecFlags(), ExecCtx::EF_PAUSE_ANIM))	{	return;	}	// 一時停止中

	HrAnim* anim;
	HrAnimCtx* animctx;
	f32 frame;

	// アニメーションの進行
	if(asctx->_frameAnimIntpMax > 0.0f)		// アニメーション間補間中
	{
		asctx->_frameAnimIntp += ec->getDeltaFrame();	// 補間フレームを進める
		if(asctx->_frameAnimIntp >= asctx->_frameAnimIntpMax)
		{
			// 補間終了
			asctx->_frameAnimIntp = asctx->_frameAnimIntpMax;
			asctx->_frameAnimIntpMax = 0.0f;
		}

		frame = asctx->_frameAnimIntp;	// 線形的に新しいアニメーション姿勢に移行するフレーム値
	}
	else if(asctx->_animidxCur == -1)		// アニメーションが設定されていない場合
	{
		if(asctx->_animidRsvFirst != ANIMID_INVALID)	// １フレーム目切り替え予約がある場合
		{
			// アニメーション切り替えを行う
			setAnimIdImmediately(asctx, asctx->_animidRsvFirst, asctx->_frameIntpRsvFirst, false);
			asctx->_animidRsvFirst = ANIMID_INVALID;	// 予約解除
		}
		else if(asctx->_animidRsvEnd != ANIMID_INVALID)	// 最終フレーム切り替え予約がある場合
		{
			// アニメーション切り替えを行う
			setAnimIdImmediately(asctx, asctx->_animidRsvEnd, asctx->_frameIntpRsvEnd, false);
			asctx->_animidRsvEnd = ANIMID_INVALID;	// 予約解除
		}

		frame = 1.0f;
	}
	else									// ▼ 通常アニメーション中
	{
		anim = _aarrAnim[asctx->_animidxCur];
		animctx = asctx->_acarrAnimCtx[asctx->_animidxCur];
		// 通常アニメーションフレームを進める
		animctx->_frame += ec->getDeltaFrame();
		if(animctx->_frame > anim->getMaxFrameNumber())
		{
			animctx->_frame = (f32)anim->getMaxFrameNumber();
		}
		bool eof = anim->getFrameIndex(asctx->_apfState->_idxarrAofTmpShapes, animctx->_frame);
		// 新しいフレーム列を求める
		#if TEST_CUBIC_INTP
			setShapesAnimPlayingFrame(asctx, asctx->_apfState->_idxarrAofTmpShapes, -1, HrAnimPlayingFrame::KP_M1);
		#else
			setShapesAnimPlayingFrame(asctx, asctx->_apfState->_idxarrAofTmpShapes, 0, HrAnimPlayingFrame::KP_P0);
		#endif
		// ループや予約処理
		if(eof)
		{
			// ループ処理
			if(TFW_IS_FLAG(asctx->_flags, HrAnimSetCtx::F_LOOP))
			{
				animctx->_frame = 1.0f;
				asctx->_replcIntpHeadKf = false;	// 先頭フレーム置換はアニメーション間補間の最初のループのみ
			}

			// 予約解消
			if(asctx->_animidRsvFirst != ANIMID_INVALID)	// １フレーム目切り替え予約がある場合
			{
				// アニメーション切り替えを行う
				setAnimIdImmediately(asctx, asctx->_animidRsvFirst, asctx->_frameIntpRsvFirst, false);
				asctx->_animidRsvFirst = ANIMID_INVALID;	// 予約解除
			}
			else if(asctx->_animidRsvEnd != ANIMID_INVALID)	// 最終フレーム切り替え予約がある場合
			{
				// アニメーション切り替えを行う
				setAnimIdImmediately(asctx, asctx->_animidRsvEnd, asctx->_frameIntpRsvEnd, false);
				asctx->_animidRsvEnd = ANIMID_INVALID;		// 予約解除
			}
		}

		frame = animctx->_frame;
	}

	// 再生しているアニメーションが無い場合は抜ける
	if(asctx->_animidxCur == -1)	{	return;	}
	anim = _aarrAnim[asctx->_animidxCur];
	animctx = asctx->_acarrAnimCtx[asctx->_animidxCur];

	// 補間
	bool isIntp = !TFW_IS_FLAG(ec->getExecFlags(), ExecCtx::EF_NO_ANIM_INTERP);	// 補間は有効か
	for(int ishape = 0; ishape < (int)_numShape; ishape++)
	{
		HrAnimPlayingFrame::HrAnimObjPlayingFrame* aopf = &asctx->_apfState->_aopfShapes[ishape];
		const HrAnimObjFrame* aofP0 = aopf->_aof[HrAnimPlayingFrame::KP_P0];
		// キーフレーム無し
		if(aofP0 == 0L)
		{
			continue;
		}

		HrAnimObjPosture aopRes;
		const HrAnimObjFrame* aofP1 = aopf->_aof[HrAnimPlayingFrame::KP_P1];
		f32 frameP0 = (f32)aofP0->getFrame();

		if(isIntp && (aofP0 != 0L) && (aofP1 != 0L) && (frame > frameP0))	// ２点間補間
		{
			f32 rframeP0 = aopf->_rframe[HrAnimPlayingFrame::KP_P0];
			f32 ratioIntp = (frame - frameP0) / rframeP0;
			#if defined(_DEBUG)
				if(ratioIntp > 1.0f)
				{
					ASSERT(false);
					ratioIntp = 1.0f;
				}
			#endif

			#if INTP_SPLINE					// 平行移動スプライン補間 + クォータニオン球面線形補間
				const HrAnimObjFrame* aofM1 = aopf->_aof[HrAnimPlayingFrame::KP_M1];
				const HrAnimObjFrame* aofP2 = aopf->_aof[HrAnimPlayingFrame::KP_P2];
				f32 rframeM1;
				f32 rframeP1;
				if(aofM1 == 0L)
				{
					aofM1 = aofP0;
					rframeM1 = FRAME_NARROW;
				}
				else
				{
					rframeM1 = aopf->_rframe[HrAnimPlayingFrame::KP_M1];
				}
				if(aofP2 == 0L)
				{
					aofP2 = aofP1;
					rframeP1 = FRAME_NARROW;
				}
				else
				{
					rframeP1 = aopf->_rframe[HrAnimPlayingFrame::KP_P1];
				}
///				ASSERT((rframeM1 >= FRAME_NARROW) && (rframeP0 >= FRAME_NARROW) && (rframeP1 >= FRAME_NARROW));
				f32 xP0 = frameP0;
				f32 xM1 = frameP0 - rframeM1;
				f32 xP1 = frameP0 + rframeP0;
				f32 xP2 = xP1 + rframeP1;
				Gcalc::splineintrp(aopRes.translate(), frame, xM1, xP0, xP1, xP2, aofM1->getPos()->getTranslate(), aofP0->getPos()->getTranslate(), aofP1->getPos()->getTranslate(), aofP2->getPos()->getTranslate());
				Vector4F qtrnP0, qtrnP1, qtrnRes;
				aofP0->getPos()->calcQuaternion(&qtrnP0);
				aofP1->getPos()->calcQuaternion(&qtrnP1);
				Quaternion::slerp(&qtrnRes, &qtrnP0, &qtrnP1, ratioIntp);
				aopRes.setQuaternion(&qtrnRes);
			#elif INTP_LINEAR_QTRNN			// クォータニオンによる線形補間
				Gcalc::interpolate(aopRes.translate(), aofP0->getPos()->getTranslate(), aofP1->getPos()->getTranslate(), ratioIntp);
				Vector4F qtrnP0, qtrnP1, qtrnRes;
				aofP0->getPos()->calcQuaternion(&qtrnP0);
				aofP1->getPos()->calcQuaternion(&qtrnP1);
				Quaternion::lerp(&qtrnRes, &qtrnP0, &qtrnP1, ratioIntp);
				aopRes.setQuaternion(&qtrnRes);
			#elif INTP_SLERP_QTRNN			// クォータニオンによる球面補間
				Gcalc::interpolate(aopRes.translate(), aofP0->getPos()->getTranslate(), aofP1->getPos()->getTranslate(), ratioIntp);
				Vector4F qtrnP0, qtrnP1, qtrnRes;
				aofP0->getPos()->calcQuaternion(&qtrnP0);
				aofP1->getPos()->calcQuaternion(&qtrnP1);
				Quaternion::slerp(&qtrnRes, &qtrnP0, &qtrnP1, ratioIntp);
				aopRes.setQuaternion(&qtrnRes);
			#else							// 線形補間
				Gcalc::interpolate(aopRes.translate(), aofP0->getPos()->getTranslate(), aofP1->getPos()->getTranslate(), ratioIntp);
				Gcalc::interpolate(aopRes.angle(), aofP0->getPos()->getAngle(), aofP1->getPos()->getAngle(), ratioIntp);
				//Gcalc::interpolate(aopRes.scale(), aofP0->getScale(), aofP1->getScale(), ratioIntp);
			#endif
		}
		else	// 補間なし
		{
			aopRes.copy(aofP0->getPos());
		}

		#if (INTP_LINEAR_QTRNN) && 0	// オイラー角の強制クォータニオン計算
		{
			if(!aopRes.hasQuaternion())
			{
				Vector4F qtrnWk;
				Quaternion::convFromEulerAngles(&qtrnWk, aopRes.getAngle());
				aopRes.setQuaternion(&qtrnWk);
			}
		}
		#endif

		// 姿勢設定
		aofP0->getObj()->setPosture(asctx->_aocaObjCtxArray, &aopRes);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void HrAnimSet::render(const RenderCtx* rc, HrAnimSetCtx* asctx)
{
	for(int i = 0; i < (int)_numRootAobj; i++)
	{
		_aobjRoot[i]->render(rc, asctx->_aocaObjCtxArray);
	}
}

/*---------------------------------------------------------------------*//**
	アニメーションインデックスの検索
**//*---------------------------------------------------------------------*/
s32 HrAnimSet::findAnimIndex(u16 animid)
{
	for(s32 i = 0; i < _numAnim; i++)
	{
		if(_aarrAnim[i]->getAnimId() == animid)
		{
			return i;
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	アニメーションの即時切り替え
**//*---------------------------------------------------------------------*/
void HrAnimSet::setAnimIdImmediately(HrAnimSetCtx* asctx, u16 animid, s32 frameIntp, bool replaceHeadKeyFrameByCurPos)
{
	/*

	アニメーション間補間時のキーフレーム列の例

	A01     A03     A05             A09 A10			← アニメーション A のフレーム列
	             C									← 現在の姿勢（例：フレーム 4）
	B01 B02         B05 B06     B08     B10			← アニメーション B のフレーム列

		↓↓↓　補間例（isExcludeNextHeadFrame: false, frameIntp: 3）
	A03  C          B01 B02 ...			← 補間後のフレーム列
	-1   0          3   4   ...			← 差分フレーム

		↓↓↓　補間例（isExcludeNextHeadFrame: true, frameIntp: 2）
	A03  C     B02         B05 ...		← 補間後のフレーム列
	-1   0     2           5   ...		← 差分フレーム

	*/

	bool isIntp = frameIntp > 0.0f;				// アニメーション間補間をする

	#if TEST_CUBIC_INTP
		// 変更前のアニメーションフレーム値をバックアップする
		f32 frameOld = 1.0f;
		if(asctx->_animidxCur != -1)
		{
			frameOld = asctx->_acarrAnimCtx[asctx->_animidxCur]->_frame;
		}
	#endif

	// 全キーフレーム列の繰下げとクリア
	for(u32 ishape = 0; ishape < _numShape; ishape++)
	{
		HrAnimPlayingFrame::HrAnimObjPlayingFrame* aopf = &asctx->_apfState->_aopfShapes[ishape];
		for(s32 kp = 0; kp < HrAnimPlayingFrame::KP_P0; kp++)
		{
			aopf->_rframe[kp] = aopf->_rframe[kp + 1];
			aopf->_aof[kp] = aopf->_aof[kp + 1];
		}
		for(s32 kp = HrAnimPlayingFrame::KP_P0; kp < HrAnimPlayingFrame::NUM_KP; kp++)
		{
			aopf->_aof[kp] = 0L;
			aopf->_rframe[kp] = 0.0f;
		}
	}
	asctx->_replcIntpHeadKf = false;

	// 新しいアニメーション ID をセット
	asctx->_animidxCur = findAnimIndex(animid);
	if(asctx->_animidxCur == -1)
	{
		return;	// 存在しないアニメーションであった
	}
	HrAnim* anim = _aarrAnim[asctx->_animidxCur];
	HrAnimCtx* animctx = asctx->_acarrAnimCtx[asctx->_animidxCur];

	// 新しいアニメーションのキーフレーム数が複数ない場合は先頭キーフレーム置換はできない
	if(replaceHeadKeyFrameByCurPos && (anim->getAnimObjFrameMaxNum() <= 1))
	{
		replaceHeadKeyFrameByCurPos = false;
	}

	// 新しいアニメーションの初期姿勢を得る
	anim->getHeadFrameIndex(asctx->_apfState->_idxarrAofTmpShapes);
	animctx->_frame = 1.0f;
	asctx->_frameAnimIntp = 0.0f;
	asctx->_frameAnimIntpMax = 0.0f;

	// 新しいキーフレーム列を求める
	s32 kpStart = (isIntp && !replaceHeadKeyFrameByCurPos) ? HrAnimPlayingFrame::KP_P1 : HrAnimPlayingFrame::KP_P0;	// アニメーション間補間時は +0 キーフレームは現在の姿勢になる
	setShapesAnimPlayingFrame(
		asctx,
		asctx->_apfState->_idxarrAofTmpShapes,
		0,
		kpStart	);

	// 現在の姿勢保持とカウンタセット
	if(isIntp || replaceHeadKeyFrameByCurPos)
	{
		// 現在の姿勢を +0 キーフレームとして挿入する
		for(u32 ishape = 0; ishape < _numShape; ishape++)
		{
			HrAnimPlayingFrame::HrAnimObjPlayingFrame* aopf = &asctx->_apfState->_aopfShapes[ishape];
			const HrAnimObjFrame* aofHead = anim->getAnimObjFrame(ishape, 0); ASSERT(aofHead != 0L);
			// 現在のシェイプの姿勢を _aofIntpShapes に保存し、+0 キーフレーム（HrAnimObjFrame）とする
			HrAnimObjFrame* aofIntp = &asctx->_apfState->_aofIntpShapes[ishape];
			aofHead->getObj()->getPosture(asctx->_aocaObjCtxArray, aofIntp->pos());	// 現在の姿勢を保存
			aofIntp->setFrame(1);
			aofIntp->setObj(aofHead->getObj());
			aopf->_aof[HrAnimPlayingFrame::KP_P0] = aofIntp;
			if(!replaceHeadKeyFrameByCurPos)
			{
				aopf->_rframe[HrAnimPlayingFrame::KP_P0] = (f32)frameIntp;
			}

			#if TEST_CUBIC_INTP
				// 以前のアニメーションにおける +0 キーフレームからの経過差分フレームを -1 差分フレーム値に設定
				const HrAnimObjFrame* aofOldP0 = aopf->_aof[HrAnimPlayingFrame::KP_M1];
				if(aofOldP0 != 0L)
				{
					f32 rframeM1 = frameOld - aofOldP0->getFrame();
					if(rframeM1 <= 0.0f)	{	rframeM1 = FRAME_NARROW;	}
					aopf->_rframe[HrAnimPlayingFrame::KP_M1] = rframeM1;
				}
			#endif
		}

		// アニメーション間補間フレーム値を設定
		if(replaceHeadKeyFrameByCurPos)
		{
			asctx->_replcIntpHeadKf = true;	// 先頭キーフレームを置換する
		}
		else
		{
			asctx->_frameAnimIntp = 1.0f;
			asctx->_frameAnimIntpMax = 1.0f + (f32)frameIntp;
		}
	}
}

/*---------------------------------------------------------------------*//**
	全シェイプのキーフレーム列枠を設定する
**//*---------------------------------------------------------------------*/
void HrAnimSet::setShapesAnimPlayingFrame(HrAnimSetCtx* asctx, const u16* idxarrAof, s32 idxAofOffset, s32 kpStart)
{
	HrAnim* anim = _aarrAnim[asctx->_animidxCur];
	bool isLoop = TFW_IS_FLAG(asctx->_flags, HrAnimSetCtx::F_LOOP);
	bool replaceHead = asctx->_replcIntpHeadKf;

	for(u32 ishape = 0; ishape < _numShape; ishape++)
	{
		s32 kp = kpStart;
		HrAnimPlayingFrame::HrAnimObjPlayingFrame* aopf = &asctx->_apfState->_aopfShapes[ishape];
		s32 numAof = anim->getAnimObjFrameNum(ishape);
		s32 idxAofWkL = idxarrAof[ishape];

		// キーフレームがない場合は全て NULL 化
		if(idxAofWkL == HrAnim::AOFIDX_INVALID)
		{
			while(kp < HrAnimPlayingFrame::NUM_KP)
			{
				aopf->_aof[kp] = 0L;
				aopf->_rframe[kp] = 0.0f;
				kp++;
			}
			continue;
		}

		// キーフレームが１つしかない場合は固定的に全てを先頭キーフレームを設定
		if(numAof <= 1)
		{
			while(kp < HrAnimPlayingFrame::NUM_KP)
			{
				if(!asctx->_replcIntpHeadKf)
				{
					aopf->_aof[kp] = anim->getAnimObjFrame(ishape, 0);
					aopf->_rframe[kp] = FRAME_NARROW;
				}
				kp++;
			}
			continue;
		}

		// インデックスにオフセットが設定されている場合は加算する
		if(idxAofOffset != 0)
		{
			idxAofWkL += idxAofOffset;
		}

		// キーフレームを枠に抜き出す
		if(kp < HrAnimPlayingFrame::NUM_KP)
		{
			s32 idxAofWkLogicL = idxAofWkL;
			while(true)
			{
				idxAofWkL = normalizeIndex(idxAofWkLogicL, numAof, isLoop);
				ASSERT((0 <= idxAofWkL) && (idxAofWkL < numAof));
				const HrAnimObjFrame* aofL = anim->getAnimObjFrame(ishape, idxAofWkL);

				f32 rframeWk;
				s32 idxAofWkR = normalizeIndex(idxAofWkLogicL + 1, numAof, isLoop);
				if(idxAofWkR < idxAofWkL)	// おそらく、ループして左右反転している
				{
					rframeWk = (f32)(anim->getMaxFrameNumber() - aofL->getFrame());
				}
				else
				{
					const HrAnimObjFrame* aofR = anim->getAnimObjFrame(ishape, idxAofWkR);
					rframeWk = (f32)(aofR->getFrame() - aofL->getFrame());
				}
				if(rframeWk <= 0.0f)	{	rframeWk = FRAME_NARROW;	}

				if(replaceHead && (idxAofWkLogicL <= 0))
				{
					if(idxAofWkL == 0)	// 頭のフレームを置換する
					{
						HrAnimObjFrame* aofIntp = &asctx->_apfState->_aofIntpShapes[ishape];
						aopf->_aof[kp] = aofIntp;
						aopf->_rframe[kp] = rframeWk;
					}
				}
				else
				{
					aopf->_aof[kp] = aofL;
					aopf->_rframe[kp] = rframeWk;
				}

				kp++;
				if(kp >= HrAnimPlayingFrame::NUM_KP)
				{
					break;
				}
				idxAofWkLogicL++;
				if(replaceHead && (idxAofWkLogicL >= numAof))
				{
					replaceHead = false;
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	インデックス値を正規化する
**//*---------------------------------------------------------------------*/
s32 HrAnimSet::normalizeIndex(s32 index, s32 num, bool isLoop) const
{
	if(index < 0)
	{
		if(isLoop)	{	while(index < 0)	{	index += num;	}	}
		else		{	index = 0;									}
	}
	else if(index >= num)
	{
		if(isLoop)	{	while(index >= num)	{	index -= num;	}	}
		else		{	index = num - 1;							}
	}
	return index;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS