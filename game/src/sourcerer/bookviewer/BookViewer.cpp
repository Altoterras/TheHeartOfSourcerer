/***********************************************************************//**
 *	BookViewer.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/21.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BookViewer.h"

// Friends
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameRenderCtx.h"

// External
#include "../../tfw/Tfw.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/file/File.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BookViewer メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BookViewer::BookViewer()
	: _pageCur(0)
	, _opstat(OPSTAT_STABLE)
	, _fliprateCur(0)
	, _fliprateDrag(0)
	, _hold(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BookViewer::~BookViewer()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BookViewer::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BookViewer:: destroy()
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool BookViewer::begin(Config* conf, void* objParamRef)
{
	// 設定のコピー
	_conf = *conf;

	// 値の初期化
	_pageCur = 0;

	setupDispPageTexture();	// 表次ページのテクスチャ設定

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void BookViewer::end(void* objParamRef)
{
	// テクスチャ削除
	for(int i = 0; i < NUM_DPD; i++)
	{
		if(_dparr[i]._tex != 0L)
		{
			if(_dparr[i]._tex->isCreated())
			{
				_dparr[i]._tex->destroy();
			}
			delete _dparr[i]._tex;
			_dparr[i]._tex = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void BookViewer::exec(ExecRes* res, const ExecCtx* ec)
{
	// ユーザー操作
	TouchPanel* touchp = ((GameExecCtx*)ec)->getTouchPanel();
	PointF32 ptNow;
	if(touchp->isTapTrigger(&ptNow))
	{
		if(_opstat == OPSTAT_STABLE)
		{
			if(ptNow.x() <= (Game::getGame()->getLogicalWidth() / 2))
			{
				if(_pageCur > 0)
				{
					// 新しいページへ
					_pageCur -= 2;	// ページを戻る
					if(_pageCur < 0)	{	_pageCur = 0;	}

					// ページめくりの開始
					_fliprateCur = 100;
					_opstat = OPSTAT_FLIP_L;
					setupDispPageTexture();				// 表次ページのテクスチャ設定
					setupFlipPageStartVertex(true);		// ページめくりの開始頂点設定
				}
			}
			else
			{
				if(_pageCur < (_conf._numPage - 1))
				{
					// 新しいページへ
					_pageCur += 2;	// ページを送る
					if(_pageCur > (_conf._numPage - 1))	{	_pageCur = _conf._numPage - 1;	}

					// ページめくりの開始
					_fliprateCur = 0;
					_opstat = OPSTAT_FLIP_R;
					setupDispPageTexture();				// 表次ページのテクスチャ設定
					setupFlipPageStartVertex(false);	// ページめくりの開始頂点設定
				}
			}
			_hold = true;
		}
	}
	else if(touchp->isTapRelease(0L, 0L))
	{
		_hold = false;
	}
	else if(touchp->getLastAction() == TouchPanel::ACT_DRAG)
	{
		// 手動めくり目標値
		touchp->getDragParam(0L, &ptNow, 0L);
		if((_opstat == OPSTAT_FLIP_L) || (_opstat == OPSTAT_FLIP_R))
		{
			_fliprateDrag = (u8)(100.0f * (Game::getGame()->getLogicalWidth() - ptNow.x()) / (f32)Game::getGame()->getLogicalWidth());
			if(_fliprateDrag > 100)	{	_fliprateDrag = 100;	}
		}
		_hold = true;
	}

	if(_hold)
	{
		// 手動めくり
		if(_fliprateCur > _fliprateDrag)
		{
			if(_fliprateCur > (_fliprateDrag + 10))	{	_fliprateCur -= 10;	}
			else									{	_fliprateCur--;		}
		}
		else if(_fliprateCur < _fliprateDrag)
		{
			if(_fliprateCur < (_fliprateDrag - 10))	{	_fliprateCur += 10;	}
			else									{	_fliprateCur++;		}
		}
	}
	else
	{
		// 自動めくり
		if((_opstat == OPSTAT_FLIP_L) || (_opstat == OPSTAT_FLIP_R))
		{
			if(_fliprateCur > 50)
			{
				_fliprateCur++;
				if(_fliprateCur >= 100)
				{
					// 戻した
					if(_opstat == OPSTAT_FLIP_L)
					{
						_pageCur += 2;
						if(_pageCur > (_conf._numPage - 1))	{	_pageCur = _conf._numPage - 1;	}
					}

					_opstat = OPSTAT_STABLE;
					setupDispPageTexture();				// 表次ページのテクスチャ設定
				}
			}
			else if(_fliprateCur > 0)
			{
				_fliprateCur--;
				if(_fliprateCur <= 0)
				{
					// 戻した
					if(_opstat == OPSTAT_FLIP_R)
					{
						_pageCur -= 2;
						if(_pageCur < 0)	{	_pageCur = 0;	}
					}

					_opstat = OPSTAT_STABLE;
					setupDispPageTexture();				// 表次ページのテクスチャ設定
				}
			}
			else if(_fliprateCur == 0)
			{
				_opstat = OPSTAT_STABLE;
				setupDispPageTexture();				// 表次ページのテクスチャ設定
			}
		}
	}

	// ページめくりのクロスシミュレーション
	{
		f32 thetaFlip = - (_fliprateCur / 100.0f) * TFW_PI;
		Vector3F vCnr(PAGE_W, PAGE_H, 0.0f);	// 端の位置ベクトル
		Matrix4F mtxFkip;
		mtxFkip.rotateY(thetaFlip);
		mtxFkip.mult(&vCnr, &vCnr);
		f32 vxDivHz = vCnr.x() / (f32)NUM_DIV_HZ;
		f32 vyDivVt = vCnr.y() / (f32)NUM_DIV_VT;
		f32 vzDivHz = vCnr.z() / (f32)NUM_DIV_HZ;
		f32 vzDivVt = vCnr.z() / (f32)NUM_DIV_VT;
		const f32 yStart = - PAGE_H * 0.5f;
		Vector3F vHrd(0.0f, yStart, 0.0f);		// 全く慣性が無い場合の位置ベクトル

		for(int ihz = 0; ihz < NUM_DIV_HZ + 1; ihz++)
		{
			for(int ivt = 0; ivt < NUM_DIV_VT + 1; ivt++)
			{
				// 慣性率の計算
				f32 weightNew = (ihz / (f32)NUM_DIV_HZ) * ((NUM_DIV_VT - ivt) / (f32)NUM_DIV_VT);	// 右下端は慣性が少ない
				weightNew = ::powf(weightNew, 5.0f);		// その傾向を強調する
				weightNew += 0.2f * ((NUM_DIV_HZ - ihz) / (f32)NUM_DIV_HZ);	// 内側は慣性が少ない
				weightNew += 0.2f;	// 最低硬さ
				if(weightNew > 1.0f)	{	weightNew = 1.0f;	}
				f32 weightOld = 1.0f - weightNew;

				_fvarr[ihz][ivt]._vtx.x() = (vHrd.x() * weightNew) + (_fvarr[ihz][ivt]._vtx.x() * weightOld);
				_fvarr[ihz][ivt]._vtx.y() = (vHrd.y() * weightNew) + (_fvarr[ihz][ivt]._vtx.y() * weightOld);
				_fvarr[ihz][ivt]._vtx.z() = (vHrd.z() * weightNew) + (_fvarr[ihz][ivt]._vtx.z() * weightOld);
				vHrd.y() += vyDivVt;
			}
			vHrd.x() += vxDivHz;
			vHrd.y() = yStart;
			vHrd.z() += vzDivHz;
		}
	}

	STRACE("page=%d, flipr=%d\n", _pageCur, _fliprateCur);
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void BookViewer::render(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();
	rdr->setAlphaBlend(true);							// アルファブレンドあり
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);		// アルファファンクションを通常に
	rdr->setFog(false);									// フォグを無効に
	///rdr->setCulling(false);							// カリングを無効に

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// 左ページ描画
	Vector3F v1, v2;
	if(_dparr[DPD_LEFT]._tex != 0L)
	{
		rdr->bindTexture(_dparr[DPD_LEFT]._tex);	// テクスチャを設定
		renderPage(rc, Vector3F::setout(&v1, - PAGE_W, - PAGE_H * 0.5f, 0.0f), Vector3F::setout(&v2, 0.0f, PAGE_H * 0.5f, 0.0f), 0.0f, 0.0f, 1.0f, 1.0f);
	}
	// 右ページ描画
	if(_dparr[DPD_RIGHT]._tex != 0L)
	{
		rdr->bindTexture(_dparr[DPD_RIGHT]._tex);	// テクスチャを設定
		renderPage(rc, Vector3F::setout(&v1, 0.0f, - PAGE_H * 0.5f, 0.0f), Vector3F::setout(&v2, PAGE_W, PAGE_H * 0.5f, 0.0f), 0.0f, 0.0f, 1.0f, 1.0f);
	}

	// めくり中ページ描画
	if((_opstat == OPSTAT_FLIP_L) || (_opstat == OPSTAT_FLIP_R))
	{
		if(_dparr[DPD_FLIP_LEFT]._tex != 0L)
		{
			rdr->bindTexture(_dparr[DPD_FLIP_LEFT]._tex);	// テクスチャを設定
			renderFlipPage(rc, 0.0f, 0.0f, 1.0f, 1.0f, true);
		}
		if(_dparr[DPD_FLIP_RIGHT]._tex != 0L)
		{
			rdr->bindTexture(_dparr[DPD_FLIP_RIGHT]._tex);	// テクスチャを設定
			renderFlipPage(rc, 0.0f, 0.0f, 1.0f, 1.0f, false);
		}
	}

	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);
}

/*---------------------------------------------------------------------*//**
	ページ描画

	       ↓v2
	+------+
	|      |
	|      |
	|      |
	|      |
	+------+
	↑v1
**//*---------------------------------------------------------------------*/
void BookViewer::renderPage(const RenderCtx* rc, const Vector3F* v1, const Vector3F* v2, f32 tu, f32 tv, f32 tw, f32 th)
{
	const GLfloat vtxs[] =
	{
		v1->x(),	v1->y(),	v1->z(),
		v2->x(),	v1->y(),	v2->z(),
		v1->x(),	v2->y(),	v1->z(),
		v2->x(),	v2->y(),	v2->z(),
	};
	const GLfloat uvs[] =
	{
		tu,			tv,
		tu + tw,	tv,
		tu,			tv + th,
		tu + tw,	tv + th,
	};

	::glVertexPointer(3, GL_FLOAT, 0, vtxs);
	::glTexCoordPointer(2, GL_FLOAT, 0, uvs);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*---------------------------------------------------------------------*//**
	ページめくり描画
**//*---------------------------------------------------------------------*/
void BookViewer::renderFlipPage(const RenderCtx* rc, f32 tu, f32 tv, f32 tw, f32 th, bool isFrontSide)
{
	f32 tuDivHz = tw / (f32)NUM_DIV_HZ;
	f32 tvDivVt = th / (f32)NUM_DIV_VT;
	f32 tuWk = tu;
	f32 tvWk = tv;

	GLfloat vtxs[3 * 2 * (NUM_DIV_VT + 1)];
	GLfloat uvs[2 * 2 * (NUM_DIV_VT + 1)];
	int ispV, ispT;

	if(isFrontSide)
	{
		for(int ihz = 0; ihz < NUM_DIV_HZ; ihz++)
		{
			ispV = 0;
			ispT = 0;

			for(int ivt = 0; ivt < NUM_DIV_VT; ivt++)
			{
				vtxs[ispV++] = _fvarr[ihz    ][ivt]._vtx.x();
				vtxs[ispV++] = _fvarr[ihz    ][ivt]._vtx.y();
				vtxs[ispV++] = _fvarr[ihz    ][ivt]._vtx.z();
				vtxs[ispV++] = _fvarr[ihz + 1][ivt]._vtx.x();
				vtxs[ispV++] = _fvarr[ihz + 1][ivt]._vtx.y();
				vtxs[ispV++] = _fvarr[ihz + 1][ivt]._vtx.z();
				uvs[ispT++] = tuWk;
				uvs[ispT++] = tvWk;
				uvs[ispT++] = tuWk + tuDivHz;
				uvs[ispT++] = tvWk;

				tvWk += tvDivVt;
			}

			vtxs[ispV++] = _fvarr[ihz    ][NUM_DIV_VT]._vtx.x();
			vtxs[ispV++] = _fvarr[ihz    ][NUM_DIV_VT]._vtx.y();
			vtxs[ispV++] = _fvarr[ihz    ][NUM_DIV_VT]._vtx.z();
			vtxs[ispV++] = _fvarr[ihz + 1][NUM_DIV_VT]._vtx.x();
			vtxs[ispV++] = _fvarr[ihz + 1][NUM_DIV_VT]._vtx.y();
			vtxs[ispV++] = _fvarr[ihz + 1][NUM_DIV_VT]._vtx.z();
			uvs[ispT++] = tuWk;
			uvs[ispT++] = tvWk;
			uvs[ispT++] = tuWk + tuDivHz;
			uvs[ispT++] = tvWk;
			::glVertexPointer(3, GL_FLOAT, 0, vtxs);
			::glTexCoordPointer(2, GL_FLOAT, 0, uvs);
			::glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (NUM_DIV_VT + 1));

			tuWk += tuDivHz;
			tvWk = tv;
		}
	}
	else
	{
		for(int ihz = 0; ihz < NUM_DIV_HZ; ihz++)
		{
			ispV = 0;
			ispT = 0;

			for(int ivt = 0; ivt < NUM_DIV_VT; ivt++)
			{
				vtxs[ispV++] = _fvarr[ihz + 1][ivt]._vtx.x();
				vtxs[ispV++] = _fvarr[ihz + 1][ivt]._vtx.y();
				vtxs[ispV++] = _fvarr[ihz + 1][ivt]._vtx.z();
				vtxs[ispV++] = _fvarr[ihz    ][ivt]._vtx.x();
				vtxs[ispV++] = _fvarr[ihz    ][ivt]._vtx.y();
				vtxs[ispV++] = _fvarr[ihz    ][ivt]._vtx.z();
				uvs[ispT++] = 1.0f - (tuWk + tuDivHz);
				uvs[ispT++] = tvWk;
				uvs[ispT++] = 1.0f - tuWk;
				uvs[ispT++] = tvWk;

				tvWk += tvDivVt;
			}

			vtxs[ispV++] = _fvarr[ihz + 1][NUM_DIV_VT]._vtx.x();
			vtxs[ispV++] = _fvarr[ihz + 1][NUM_DIV_VT]._vtx.y();
			vtxs[ispV++] = _fvarr[ihz + 1][NUM_DIV_VT]._vtx.z();
			vtxs[ispV++] = _fvarr[ihz    ][NUM_DIV_VT]._vtx.x();
			vtxs[ispV++] = _fvarr[ihz    ][NUM_DIV_VT]._vtx.y();
			vtxs[ispV++] = _fvarr[ihz    ][NUM_DIV_VT]._vtx.z();
			uvs[ispT++] = 1.0f - (tuWk + tuDivHz);
			uvs[ispT++] = tvWk;
			uvs[ispT++] = 1.0f - tuWk;
			uvs[ispT++] = tvWk;
			::glVertexPointer(3, GL_FLOAT, 0, vtxs);
			::glTexCoordPointer(2, GL_FLOAT, 0, uvs);
			::glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (NUM_DIV_VT + 1));

			tuWk += tuDivHz;
			tvWk = tv;
		}
	}
}

/*---------------------------------------------------------------------*//**
	ページめくりの開始頂点を設定
**//*---------------------------------------------------------------------*/
void BookViewer::setupFlipPageStartVertex(bool isLeftPage)
{
	const f32 vxDivHz = PAGE_W / (f32)NUM_DIV_HZ;
	const f32 vyDivVt = PAGE_H / (f32)NUM_DIV_VT;
	const f32 yStart = - PAGE_H * 0.5f;

	Vector3F vD(isLeftPage ? - vxDivHz : vxDivHz, vyDivVt, 0.0f);
	Vector3F vWk(0.0f, yStart, 0.0f);

	for(int ihz = 0; ihz < NUM_DIV_HZ + 1; ihz++)
	{
		for(int ivt = 0; ivt < NUM_DIV_VT + 1; ivt++)
		{
			_fvarr[ihz][ivt]._vtx = vWk;
			vWk.y() += vD.y();
		}
		vWk.x() += vD.x();
		vWk.y() = yStart;
	}
}

/*---------------------------------------------------------------------*//**
	表示ページのテクスチャを設定
**//*---------------------------------------------------------------------*/
void BookViewer::setupDispPageTexture()
{
	s32 fileidOld[NUM_DPD];
	Texture* texOld[NUM_DPD];
	for(int i = 0; i < NUM_DPD; i++)
	{
		fileidOld[i] = _dparr[i]._fileid;
		texOld[i] = _dparr[i]._tex;
		_dparr[i]._fileid = 0;
		_dparr[i]._tex = 0L;
	}

	// 表示ページのページを設定
	if(_conf._isLeftBind)
	{
	}
	else
	{
		s32 pageRight = (_pageCur + 1) & ~1;
		if(_opstat == OPSTAT_FLIP_L)
		{
			_dparr[DPD_LEFT]._page = pageRight - 1;
			_dparr[DPD_FLIP_LEFT]._page = pageRight;
			_dparr[DPD_FLIP_RIGHT]._page = pageRight + 1;
			_dparr[DPD_RIGHT]._page = pageRight + 2;
		}
		else if(_opstat == OPSTAT_FLIP_R)
		{
			_dparr[DPD_LEFT]._page = pageRight - 3;
			_dparr[DPD_FLIP_LEFT]._page = pageRight - 2;
			_dparr[DPD_FLIP_RIGHT]._page = pageRight  - 1;
			_dparr[DPD_RIGHT]._page = pageRight;
		}
		else
		{
			_dparr[DPD_LEFT]._page = pageRight - 1;
			_dparr[DPD_FLIP_LEFT]._page = - 1;
			_dparr[DPD_FLIP_RIGHT]._page = - 1;
			_dparr[DPD_RIGHT]._page = pageRight;
		}
	}

	// ファイル ID 決定
	for(int i = 0; i < NUM_DPD; i++)
	{
		if((0 <= _dparr[i]._page) && (_dparr[i]._page <= _conf._numPage))
		{
			_dparr[i]._fileid = _conf._fidFirst + _dparr[i]._page;
		}
	}

	// テクスチャ再利用と、使用しないテクスチャを削除
	for(int j = 0; j < NUM_DPD; j++)
	{
		if(texOld[j] == 0L)	{	continue;	}

		int idxReuse = -1;
		for(int i = 0; i < NUM_DPD; i++)
		{
			if(_dparr[i]._fileid == fileidOld[j])
			{
				idxReuse = i;
				break;
			}
		}
		if(idxReuse != -1)		// 再利用
		{
			_dparr[idxReuse]._tex = texOld[j];
		}
		else					// 削除
		{
			if(texOld[j]->isCreated())	{	texOld[j]->destroy();	}
			delete texOld[j];
			texOld[j] = 0L;
		}
	}

	// 新規テクスチャの読み込み
	for(int i = 0; i < NUM_DPD; i++)
	{
		if((_dparr[i]._fileid != 0) && (_dparr[i]._tex == 0L))
		{
			_dparr[i]._tex = new Texture();
			if(!_dparr[i]._tex->create(_dparr[i]._fileid, Texture::FMT_PNG))
			{
				delete _dparr[i]._tex;
				_dparr[i]._tex = 0L;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
