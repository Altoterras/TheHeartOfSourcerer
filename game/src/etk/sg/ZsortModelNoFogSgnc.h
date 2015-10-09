/***********************************************************************//**
 *  ZsortModelNoFogSgnc.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2011/12/16.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_ZSORT_MODEL_NO_FOG_SGNC_H_
#define _ETK_SG_IMPLEMENT_SOR_ZSORT_MODEL_NO_FOG_SGNC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "ZsortModelSgnc.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

// 全ての描画を Z ソートするテスト
#define TEST_ALL_ZSORT		0

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	半透明背景 ノード連結クラス
 *
**//*---------------------------------------------------------------------*/
class ZsortModelNoFogSgnc : public ZsortModelSgnc
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_MDL_MAX	= 256;

	//======================================================================
	// 構造体
private:
	struct Qsdat
	{
		f32		_z;
		Model*	_mdl;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ZsortModelNoFogSgnc();

	virtual void previseImplements(const RenderCtx* rc);
	virtual void renderImplements(const RenderCtx* rc);

	static int compare(const Qsdat* a, const Qsdat* b);

	//======================================================================
	// 変数
private:
	Qsdat	_qsdat[NUM_MDL_MAX];
	s32		_numMdl;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_ZSORT_MODEL_NO_FOG_SGNC_H_
