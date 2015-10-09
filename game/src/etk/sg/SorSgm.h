/***********************************************************************//**
 *  SorSgm.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/04/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
 *	描画順が固定的なシンプルな標準シーングラフ
 *		基本的な戦略として、
 *			不透明 → キャラクタ → 二値抜き → 半透明 → エフェクト
 *		の順で描画する単純なルールのシーングラフ
 *		不透明オブジェクトは Z ソートを行わない
 *		かなりアプリケーションよりのモジュールだが、
 *		標準的な手法としてフレームワークに入れておく
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_SOR_SGM_H_
#define _ETK_SG_IMPLEMENT_SOR_SOR_SGM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/g3d/sg/SgManager.h"
#include "../../tfw/g3d/sg/node/SgNodeType.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Camera;
class SgNode;
class RenderCtx;
class Model;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	静的描画順（Static Orderd Render）シーングラフ管理
 *
**//*---------------------------------------------------------------------*/
class SorSgm : public SgManager
{
	//======================================================================
	// 定数
public:
	// 要素カテゴリ（描画順に定義）
	enum Ek		// Entity-Kind
	{
		EK_CAM	= 0,	// カメラ
		EK_OPCTY_NOFOG,	// フォグなし不透明モデル
		EK_ZSORT_NOFOG,	// フォグなしZ ソート半透明モデル
		EK_OPCTY,		// 不透明モデル
		EK_BITRNS,		// 二値透明モデル
		EK_STUCK_TRANS,	// 密着半透明モデル（不透明描画の直後に描く Z ソートしない半透明モデル）
		EK_ZSORT,		// Z ソート半透明モデル
		NUM_EK
	};

public:
	// 描画順の範囲定義
	static const u16	RO_FARBG_OPCTY_MIN	= 1;
	static const u16	RO_FARBG_OPCTY_MAX	= 200;
	static const u16	RO_FARBG_TRANS_MIN	= 201;
	static const u16	RO_FARBG_TRANS_MAX	= 500;
	static const u16	RO_OPCTY_MIN		= 501;
	static const u16	RO_OPCTY_MAX		= 1000;
	static const u16	RO_BITRNS_MIN		= 1001;
	static const u16	RO_BITRNS_MAX		= 1500;
	static const u16	RO_STUCK_TRANS_MIN	= 1501;
	static const u16	RO_STUCK_TRANS_MAX	= 2000;
	static const u16	RO_TRANS_MIN		= 2001;
	static const u16	RO_TRANS_MAX		= 3000;
	static const u16	RO_EFF_MDL_MIN		= 3001;
	static const u16	RO_EFF_MDL_MAX		= 4000;
	static const u16	RO_EFF_PTCL_MIN		= 4001;
	static const u16	RO_EFF_PTCL_MAX		= 5000;
	static const u16	RO_ZSORT_MIN		= 5001;
	static const u16	RO_ZSORT_MAX		= 6000;

public:
	static const u16	SGNTYPE_DIRECT		= SGNTYPE_CUSTOM + 100;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	Camera* getVisibleCamera() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool addNode(SgEntity* sgnode, bool isAddResourcePool);
	bool removeNode(SgNode* sgnode);
	bool addNode(SgNode* sgnode);
	SgNodeChain* getKindChain(Ek ek) const;

	void showBg(bool isShow);

	void debug_print();

protected:
	static Ek convSgTypeToEntityKind(SgNode* sgnode);
	static Ek convRenderOrderToEntityKind(SgNode* sgnode, u16 rendorder);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(Tfw* tfwOwnerRef);
	void render(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_SOR_SGM_H_

