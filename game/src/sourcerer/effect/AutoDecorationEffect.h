/***********************************************************************//**
 *	AutoDecorationEffect.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/08.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EFFECT_AUTO_DECORATION_EFFECT_H_
#define _SRCR_EFFECT_AUTO_DECORATION_EFFECT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Array;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class ExecCtx;
class HrAnimObjCtx;
class Model;
class RenderCtx;
class ShapeModel;
 
TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

class Game;
class GameExecCtx;

/*---------------------------------------------------------------------*//**
 *	テクスチャ フェイシャル エフェクト
 *
**//*---------------------------------------------------------------------*/
class AutoDecorationEffect : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// 種別定義
	enum Kind
	{
		KIND_EYE,		// 目パチ
		KIND_MOUTH,		// 口パク
		KIND_SHADOW,	// 丸影
		NUM_KIND
	};

	// デフォルト設定値
	static const s32 RAND_EYE_NEXT_DEFAULT	= 10;			// 非表示継続フレーム後、次に目パチアニメをする標準確率（％）
	static const s32 DURF_EYE_NEXT_DEFAULT	= 10;			// 標準非表示継続フレーム
	static const s32 DURF_EYE_ANIM			= 4;			// 目パチアニメ継続フレーム数．目パチアニメはフレーム数が基本 4 で作成されている

	// setTexFacialConfig の有効変数フラグ
	static const u32 TFCONFF_ENABLE			= 0x00000001;	// isEnable
	static const u32 TFCONFF_NEXTANIMRAND	= 0x00000002;	// randNextAnim
	static const u32 TFCONFF_NEXTANIMDURF	= 0x00000004;	// durfNextAnim

	// setCircleShadowConfig の有効変数フラグ
	static const u32 CSCONFF_ENABLE			= 0x00000001;	// isEnable
	static const u32 CSCONFF_RADIUS			= 0x00000002;	// radius
	static const u32 CSCONFF_OFFSET			= 0x00000004;	// radius

private:
	// 最大対象数
	static const s32	NUM_TARGET			= 8;

	//======================================================================
	// クラス
private:
	class TargetChar;

	/*-----------------------------------------------------------------*//**
	 *	エフェクト
	 *
	**//*-----------------------------------------------------------------*/
	class Effect
	{
		//==================================================================
		// 定数
	public:
		// 無効化フラグ
		static const u8 F_EXT_DISABLE	= 0x1;	// 外部指定の無効化
		static const u8 F_INN_DISABLE	= 0x2;	// 内部指定の無効化

		//==================================================================
		// メソッド
	public:
		Effect();
		void reset();
		bool begin(u16 charid, ShapeModel* smdlOwnRef);
		void end();
		void exec(const GameExecCtx* gec, TargetChar* tc, bool isSaying, bool isReading);
		void render(const RenderCtx* rc, TargetChar* tc);

		bool createCircleShadow(u16 charid, ShapeModel* smdlOwnRef);

	private:
		void changeOnst(bool onstNew);

		//==================================================================
		// 変数
	public:
		u8 _kind;					// 種別
		bool _onst;					// 表示状態
		u8 _flags;					// フラグ
		Model* _mdlAdd;				// 追加描画モデル

		// KIND_EYE
		u8 _randNextAnimConf;		// 非表示から表示に切り替える確率設定 0 ～ 100（100 は即時）
		s32 _midurfNextAnimConf;	// 表示間隔設定（ミリ整数）
		s32 _mifcntIntvl;			// 表示・非表示ミリカウンタ

		// KIND_SHADOW
		f32 _radius;				// 半径
		Vector3F* _offsetPos;		// 位置オフセット
	};

	/*-----------------------------------------------------------------*//**
	 *	対象キャラクタ
	 *
	**//*-----------------------------------------------------------------*/
	class TargetChar
	{
		//==================================================================
		// メソッド
	public:
		TargetChar(u16 charid);
		void reset();
		bool begin();
		void exec(const GameExecCtx* gec, u16 charidSaying, bool isReading);
		void render(const RenderCtx* rc);
		
		//==================================================================
		// 変数
	public:
		u16 _charid;						// キャラクタ ID
		bool _isFixed;						// 固定キャラである
		u8 __pad_0010[ 1 ];
		HrAnimObjCtx* _haocSpotRef;			// 追加描画モデルの基点アニメオブジェ
		HrAnimObjCtx* _haocRootRef;			// 追加描画モデルのルートアニメオブジェ
		ShapeModel* _smdlOwnRef;			// 対象キャラクタモデル
		Effect _effect[NUM_KIND];			// エフェクト
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool registerChar(u16 charid, bool isFixed);
	bool setTexFacialConfig(u32 flagsSet, u16 charid, u8 kind, bool isEnable, u8 randNextAnim, s32 durfNextAnim);
	bool setCircleShadowConfig(u32 flagsSet, u16 charid, bool isEnable, f32 radius, const Vector3F* offsetPos);

	void start(u16 evidRtmvMatter);
	void end(u16 evidRtmvMatter);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AutoDecorationEffect();
	~AutoDecorationEffect();
	bool create();
	void destroy();
	void reset();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);

private:
	TargetChar* findChar(u16 charid);
	TargetChar* addChar(u16 charid);

	//======================================================================
	// 変数
private:
	Array<TargetChar*>* _arrTrgchar;	// 対象キャラ管理配列
	MessageWindow* _wndMsgRef;			// リップシンク用、メッセージウインドウ
	u16 _evidRtmv;						// 対象とするリアルタイムムービーのイベント ID
	bool _isBegan;						// 開始フラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EFFECT_AUTO_DECORATION_EFFECT_H_
