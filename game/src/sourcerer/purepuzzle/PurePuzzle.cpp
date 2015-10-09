/***********************************************************************//**
 *	PurePuzzle.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/11.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PurePuzzle.h"

// Friends
#include "PrpzlPerfBase.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameRenderCtx.h"

// External
#include "../../tfw/Tfw.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

//==========================================================================
// PurePuzzle 定数

const f32 PurePuzzle::FCNT_ANIM_MAX	= 24.0f;			// 自動アクションのアニメーションカウント数
const f32 PurePuzzle::FCNT_FALL_MAX = 6.0f;				// 自動アクションの落下フレームカウント数
const f32 PurePuzzle::FCNT_FLUSH	= 6.0f;				// フラッシュ表示のフレームカウント数

s32 PurePuzzle::_varFixedRand		= 0xfedcba98;		// 乱数変数

#define SUBLIME_NUM					5

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PurePuzzle メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ゲームのリセット
**//*---------------------------------------------------------------------*/
void PurePuzzle::reset()
{
	_cntChain = 0;
	_score = 0;
	resetCells();				// 開始時のセルを設定する
	nextChip();					// 最初のチップの決定
	clearCellAnim();			// セルのアニメーションリセット
}

/*---------------------------------------------------------------------*//**
	次のチップ
**//*---------------------------------------------------------------------*/
void PurePuzzle::nextChip()
{
	_eltNext = u8((randFixed() & 3) + 1);
}

/*---------------------------------------------------------------------*//**
	自動アクションへ移行
**//*---------------------------------------------------------------------*/
void PurePuzzle::turnToAutoAct()
{
	_turn = TURN_AA;
	_cntChain = 0;
	clearCellAnim();			// セルのアニメーションリセット
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PurePuzzle::PurePuzzle()
	: _opdatRef(0L)
	, _perfRef(0L)
	, _aa(AA_NULL)
	, _isAaActing(false)
	, _fcntAaAnim(0.0f)
	, _turn(TURN_AA)
	, _cntChain(0)
	, _score(0)
	, _eltNext(ELT_AE)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PurePuzzle::~PurePuzzle()
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool PurePuzzle::begin(PrpzlOperationData* opdatRef, PrpzlPerfBase* perfRef)
{
	// 参照を受け取る
	_opdatRef = opdatRef;
	_perfRef = perfRef;

	// 演出モジュールの開始
	if(!_perfRef->begin(this))
	{
		return false;
	}

	// 開始時のセルを設定する
//	setupStartCellsRandom();

	// 最初のチップの決定
	nextChip();

	// 値の初期化
	_aa = AA_NULL;
	_isAaActing = false;
	//_turn = TURN_AA;
	_turn = TURN_USER;

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void PurePuzzle::end()
{
	// 演出モジュールの終了
	_perfRef->end();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::exec(ExecRes* res, const ExecCtx* ec)
{
	// 各セルのフレーム処理
	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < NUM_CELL_V; iv++)
		{
			_cellarr[ih][iv].exec(ec);
		}
	}

	switch(_turn)
	{
	case TURN_AA:
		// 自動アクション
		if(_isAaActing)
		{
			_fcntAaAnim -= ec->getDeltaFrame();
			if(_fcntAaAnim <= 0.0f)
			{
				_isAaActing = false;
				_fcntAaAnim = 0.0f;
				clearCellAnim();
			}
		}
		else
		{
			// 新規自動アクションチェック
			{
				checkCellFall();
			}
			if(!_isAaActing)
			{
				checkCellCombine();
			}
			/*
			if(!_isAaActing)
			{
				checkCellReact();
				if(_isAaActing)	{	_cntChain++;	}
			}
			*/
			if(!_isAaActing)
			{
				checkCellSublime();
				if(_isAaActing)	{	_cntChain++;	}
			}
			if(!_isAaActing)
			{
				// スコア計算
				if(_cntChain >= 2)
				{
					_score += (_cntChain - 1) * 1;
				}

				_turn = TURN_USER;
				_cntChain = 0;
			}
		}
		break;
	case TURN_USER:
		// ユーザー操作は PrpzlPerf 側で行う
		break;
	}

	_perfRef->exec(ec);

	STRACE("AA [%d]%d cnt=%f\n", _aa, _isAaActing, _fcntAaAnim);
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void PurePuzzle::render(const RenderCtx* rc)
{
	_perfRef->render(rc);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void PurePuzzle::draw(const RenderCtx* rc)
{
	_perfRef->draw(rc);
}

/*---------------------------------------------------------------------*//**
	セルアニメーションのクリア
**//*---------------------------------------------------------------------*/
void PurePuzzle::clearCellAnim()
{
	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < NUM_CELL_V; iv++)
		{
			_cellarr[ih][iv].resetAnim();
		}
	}
}

/*---------------------------------------------------------------------*//**
	セルをリセットする
**//*---------------------------------------------------------------------*/
void PurePuzzle::resetCells()
{
	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < NUM_CELL_V; iv++)
		{
			_cellarr[ih][iv]._elt = ELT_AE;
			_cellarr[ih][iv]._hi = false;
			_cellarr[ih][iv].resetAnim();
		}
	}
}

/*---------------------------------------------------------------------*//**
	開始時のセルを乱数により設定する
**//*---------------------------------------------------------------------*/
void PurePuzzle::setupStartCellsRandom()
{
	resetCells();

	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < 3 * NUM_CELL_V / 4; iv++)
		{
			_cellarr[ih][iv]._elt = (randFixed() & 3) + 1;
		}
	}
}

/*---------------------------------------------------------------------*//**
	セルの落下チェック
**//*---------------------------------------------------------------------*/
bool PurePuzzle::checkCellFall()
{
	clearCellTmpFlags();	// セル一時フラグクリア

	bool isAct = false;
	// 判定ループ
	for(u8 eltAbout = ELT_SO; eltAbout >= ELT_WI; eltAbout--)
	{
		//bool isChange = false;
		for(int ih = 0; ih < NUM_CELL_H; ih++)
		{
			for(int iv = 0; iv < NUM_CELL_V; iv++)
			{
				if(_cellarr[ih][iv]._elt == ELT_AE)	{	continue;	}

				s32 numFall = S32_MAX;
				checkCellFall_subDive(&numFall, ih, iv, 1);
				if((0 < numFall) && (numFall < S32_MAX))
				{
					checkCellFall_subDive(&numFall, ih, iv, 2);
					isAct = true;
				}
			}
		}
	}

	// 変更ループ
	s32 numFallMax = 0;
	if(isAct)
	{
		for(int ih = 0; ih < NUM_CELL_H; ih++)
		{
			for(int iv = 0; iv < NUM_CELL_V; iv++)
			{
				s32 numFall = _cgwkfarr[ih][iv] & GWKF_LOCAL_MASK;
				if(numFall <= 0)			{	continue;	}
				if(numFallMax < numFall)	{	numFallMax = numFall;	}
				ASSERT((iv - numFall) >= 0);
				Cell* c1 = &_cellarr[ih][iv];
				Cell* c2 = &_cellarr[ih][iv - numFall];	// 落ちる先
				c2->_elt = c2->_anim._eltPrev = c1->_elt;
				c2->_hi = c1->_hi;
				c2->_anim._fcntFall = FCNT_FALL_MAX * numFall;
				c1->_elt = c1->_anim._eltPrev = ELT_AE;
				c1->_hi = false;
			}
		}
	}

	if(isAct)
	{
		_aa = AA_FALL;
		_isAaActing = true;
		_fcntAaAnim = FCNT_FALL_MAX * numFallMax;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	セルの落下チェック - セル再帰処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::checkCellFall_subDive(s32* numFall, s32 ih1, s32 iv1, s32 phase)
{
	if((_cgwkfarr[ih1][iv1] & (1 << (GWKF_CHECKED_SHIFT + phase))) != 0)	{	return;	}

	//Cell* c1 = &_cellarr[ih1][iv1];
	_cgwkfarr[ih1][iv1] |= 1 << (GWKF_CHECKED_SHIFT + phase);

	// 落下セル数を汎用作業フラグにマークする
	if(phase == 2)
	{
		_cgwkfarr[ih1][iv1] |= (*numFall) & GWKF_LOCAL_MASK;
	}

	#define ENABLE_YOKOKUTTSUKI 0

	// 上隣
	if(iv1 < (NUM_CELL_V - 1))
	{
		if(_cellarr[ih1][iv1 + 1]._elt != ELT_AE)
		{
			checkCellFall_subDive(numFall, ih1, iv1 + 1, phase);
		}
	}
	#if ENABLE_YOKOKUTTSUKI
		// 右隣
		if(ih1 < (NUM_CELL_H - 1))
		{
			if(_cellarr[ih1 + 1][iv1]._elt != ELT_AE)
			{
				checkCellFall_subDive(numFall, ih1 + 1, iv1, phase);
			}
		}
	#endif
	// 下隣
	if(iv1 > 0)
	{
		if(_cellarr[ih1][iv1 - 1]._elt != ELT_AE)
		{
			checkCellFall_subDive(numFall, ih1, iv1 - 1, phase);
		}
		else
		{
			// 下方の空きの数を調べる
			if(phase == 1)
			{
				s32 cntUnderAe = 1;
				for(int iv = iv1 - 2; iv >= 0; iv--)
				{
					if(_cellarr[ih1][iv]._elt != ELT_AE)	{	break;	}
					cntUnderAe++;
				}
				if((*numFall) > cntUnderAe)
				{
					(*numFall) = cntUnderAe;
				}
			}
		}
	}
	else
	{
		// 下辺についている
		if(phase == 1)
		{
			(*numFall) = 0;	
		}
	}
	#if ENABLE_YOKOKUTTSUKI
		// 左隣
		if(ih1 > 0)
		{
			if(_cellarr[ih1 - 1][iv1]._elt != ELT_AE)
			{
				checkCellFall_subDive(numFall, ih1 - 1, iv1, phase);
			}
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	セルの同種結合チェック
**//*---------------------------------------------------------------------*/
bool PurePuzzle::checkCellCombine()
{
	clearCellTmpFlags();	// セル一時フラグクリア

	bool isAct = false;
	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < NUM_CELL_V; iv++)
		{
			bool isChanged = false;
			s32 numComb = 0;

			checkCellCombine_subDive(&isChanged, &numComb, ih, iv);

			if((numComb == 1) && _cellarr[ih][iv]._hi)
			{
				_cellarr[ih][iv]._hi = false;	// 戻す
				isChanged = true;
			}

			if(isChanged && !isAct)
			{
				isAct = true;
			}
		}
	}

	if(isAct)
	{
		_aa = AA_COMB;
		_isAaActing = true;
		_fcntAaAnim = FCNT_ANIM_MAX;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	セルの同種結合チェック - セル再帰処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::checkCellCombine_subDive(bool* isChange, s32* numComb, s32 ih1, s32 iv1)
{
	if((_cgwkfarr[ih1][iv1] & GWKF_CHECKED_MASK) != 0)	{	return;	}

	Cell* c1 = &_cellarr[ih1][iv1];
	(*numComb)++;
	_cgwkfarr[ih1][iv1] |= 1 << GWKF_CHECKED_SHIFT;

	// 上隣
	if(iv1 < (NUM_CELL_V - 1))
	{
		Cell* c2 = &_cellarr[ih1][iv1 + 1];
		if(c2->_elt == c1->_elt)
		{
			if(!c2->_hi)
			{
				c2->_hi = true;
				(*isChange) = true;
			}
			checkCellCombine_subDive(isChange, numComb, ih1, iv1 + 1);
		}
	}
	// 右隣
	if(ih1 < (NUM_CELL_H - 1))
	{
		Cell* c2 = &_cellarr[ih1 + 1][iv1];
		if(c2->_elt == c1->_elt)
		{
			if(!c2->_hi)
			{
				c2->_hi = true;
				(*isChange) = true;
			}
			checkCellCombine_subDive(isChange, numComb, ih1 + 1, iv1);
		}
	}
	// 下隣
	if(iv1 > 0)
	{
		Cell* c2 = &_cellarr[ih1][iv1 - 1];
		if(c2->_elt == c1->_elt)
		{
			if(!c2->_hi)
			{
				c2->_hi = true;
				(*isChange) = true;
			}
			checkCellCombine_subDive(isChange, numComb, ih1, iv1 - 1);
		}
	}
	// 左隣
	if(ih1 > 0)
	{
		Cell* c2 = &_cellarr[ih1 - 1][iv1];
		if(c2->_elt == c1->_elt)
		{
			if(!c2->_hi)
			{
				c2->_hi = true;
				(*isChange) = true;
			}
			checkCellCombine_subDive(isChange, numComb, ih1 - 1, iv1);
		}
	}

	// 自身
	if(*isChange)
	{
		c1->_hi = true;
	}
}

/*---------------------------------------------------------------------*//**
	隣接セル囲いによる反応チェック
**//*---------------------------------------------------------------------*/
bool PurePuzzle::checkCellReact()
{
	clearCellTmpFlags();	// セル一時フラグクリア

	bool isAct = false;

	// 判定ループ
	for(u8 eltAbout = ELT_SO; eltAbout >= ELT_WI; eltAbout--)
	{
		bool isChange = false;
		for(int ih = 0; ih < NUM_CELL_H; ih++)
		{
			for(int iv = 0; iv < NUM_CELL_V; iv++)
			{
				if(_cellarr[ih][iv]._elt == ELT_AE)	{	continue;	}
				if(_cellarr[ih][iv]._elt > ELT_SO)	{	continue;	}

				checkCellReact_sub(&isChange, ih, iv, eltAbout);
				if(isChange)
				{
					isAct = true;
				}
			}
		}
	}

	// 変更ループ
	if(isAct)
	{
		for(int ih = 0; ih < NUM_CELL_H; ih++)
		{
			for(int iv = 0; iv < NUM_CELL_V; iv++)
			{
				if((_cgwkfarr[ih][iv] & GWKF_CHECKED_MASK) == 0)	{	continue;	}
				Cell* c = &_cellarr[ih][iv];
				c->_elt--;
				c->_hi = false;
				c->_anim._fcntFlush2 = FCNT_FLUSH;
				isAct = true;
			}
		}
	}

	if(isAct)
	{
		_aa = AA_REACT;
		_isAaActing = true;
		_fcntAaAnim = FCNT_ANIM_MAX;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	隣接セル囲いによる反応チェック - セル処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::checkCellReact_sub(bool* isChange, s32 ih1, s32 iv1, u8 eltAbout)
{
	Cell* c1 = &_cellarr[ih1][iv1];
	if(c1->_elt != eltAbout)	{	return;	}

	s32 cntNeibElt = 0;

	#define ENABLE_NANAME 0

	// 上隣
	if(iv1 < (NUM_CELL_V - 1))
	{
		if(isNeighbElt(_cellarr[ih1][iv1 + 1]._elt, c1->_elt, true, false))
		{
			cntNeibElt++;
		}
	}
	#if ENABLE_NANAME
		// 右上隣
		if((iv1 < (NUM_CELL_V - 1)) && (ih1 < (NUM_CELL_H - 1)))
		{
			if(isNeighbElt(_cellarr[ih1 + 1][iv1 + 1]._elt, c1->_elt, true, false))
			{
				cntNeibElt++;
			}
		}
	#endif
	// 右隣
	if(ih1 < (NUM_CELL_H - 1))
	{
		if(isNeighbElt(_cellarr[ih1 + 1][iv1]._elt, c1->_elt, true, false))
		{
			cntNeibElt++;
		}
	}
	#if ENABLE_NANAME
		// 右下隣
		if((iv1 < (NUM_CELL_V - 1)) && (iv1 > 0))
		{
			if(isNeighbElt(_cellarr[ih1 + 1][iv1 - 1]._elt, c1->_elt, true, false))
			{
				cntNeibElt++;
			}
		}
	#endif
	// 下隣
	if(iv1 > 0)
	{
		if(isNeighbElt(_cellarr[ih1][iv1 - 1]._elt, c1->_elt, true, false))
		{
			cntNeibElt++;
		}
	}
	#if ENABLE_NANAME
		// 左下隣
		if((ih1 > 0) && (iv1 > 0))
		{
			if(isNeighbElt(_cellarr[ih1 - 1][iv1 - 1]._elt, c1->_elt, true, false))
			{
				cntNeibElt++;
			}
		}
	#endif
	// 左隣
	if(ih1 > 0)
	{
		if(isNeighbElt(_cellarr[ih1 - 1][iv1]._elt, c1->_elt, true, false))
		{
			cntNeibElt++;
		}
	}
	#if ENABLE_NANAME
		// 左上隣
		if((ih1 > 0) && (iv1 < (NUM_CELL_V - 1)))
		{
			if(isNeighbElt(_cellarr[ih1 - 1][iv1 + 1]._elt, c1->_elt, true, false))
			{
				cntNeibElt++;
			}
		}
	#endif

	// 反応
	if(cntNeibElt >= 4)
	{
		_cgwkfarr[ih1][iv1] |= 1 << GWKF_CHECKED_SHIFT;
		(*isChange) = true;
	}
}

/*---------------------------------------------------------------------*//**
	セル同色昇華の反応チェック
**//*---------------------------------------------------------------------*/
bool PurePuzzle::checkCellSublime()
{
	clearCellTmpFlags();	// セル一時フラグクリア

	// 判定ループ
	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < NUM_CELL_V; iv++)
		{
			if(_cellarr[ih][iv]._elt == ELT_AE)	{	continue;	}
			if(_cellarr[ih][iv]._elt > ELT_SO)	{	continue;	}

			s32 numComb = 0;
			checkCellSublime_subDive(&numComb, ih, iv, 1);
			
			// 昇華
			if(numComb >= SUBLIME_NUM)
			{
				numComb = 0;
				checkCellSublime_subDive(&numComb, ih, iv, 2);
			}
		}
	}

	// 変更ループ
	bool isAct = false;
	for(int ih = 0; ih < NUM_CELL_H; ih++)
	{
		for(int iv = 0; iv < NUM_CELL_V; iv++)
		{
			u8 chflag = _cgwkfarr[ih][iv] & GWKF_LOCAL_MASK;
			if(chflag == 1)			// 上昇
			{
				Cell* c = &_cellarr[ih][iv];
				c->_elt--;
				c->_hi = false;
				c->_anim._fcntFlush1 = FCNT_FLUSH;
				isAct = true;
			}
			else if(chflag == 2)	// 下降
			{
#if 1	// 下降時に消える
				Cell* c = &_cellarr[ih][iv];
				c->_elt = ELT_AE;
				c->_hi = false;
				c->_anim._fcntFlush1 = FCNT_FLUSH;
				isAct = true;
#else
				Cell* c = &_cellarr[ih][iv];
				if(c->_elt < ELT_SO)
				{
					c->_elt++;
					c->_hi = false;
					c->_anim._fcntFlush1 = FCNT_FLUSH;
					isAct = true;
				}
#if 1	// 土→空 反応
				else if(c->_elt == ELT_SO)
				{
					c->_elt = ELT_AE;
					c->_hi = false;
					c->_anim._fcntFlush1 = FCNT_FLUSH;
					isAct = true;
				}
#endif
#endif
			}
		}
	}

	if(isAct)
	{
		_aa = AA_SUBLIM;
		_isAaActing = true;
		_fcntAaAnim = FCNT_ANIM_MAX;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	セル同色昇華の反応チェック - セル再帰処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::checkCellSublime_subDive(s32* numComb, s32 ih1, s32 iv1, s32 phase)
{
	if((_cgwkfarr[ih1][iv1] & (1 << (GWKF_CHECKED_SHIFT + phase))) != 0)	{	return;	}

	Cell* c1 = &_cellarr[ih1][iv1];
	(*numComb)++;
	_cgwkfarr[ih1][iv1] |= 1 << (GWKF_CHECKED_SHIFT + phase);

	s32 cntNeibSame = 0;

	// 上隣
	if(iv1 < (NUM_CELL_V - 1))
	{
		if(_cellarr[ih1][iv1 + 1]._elt == c1->_elt)
		{
			cntNeibSame++;
			checkCellSublime_subDive(numComb, ih1, iv1 + 1, phase);
		}
	}
	// 右隣
	if(ih1 < (NUM_CELL_H - 1))
	{
		if(_cellarr[ih1 + 1][iv1]._elt == c1->_elt)
		{
			cntNeibSame++;
			checkCellSublime_subDive(numComb, ih1 + 1, iv1, phase);
		}
	}
	// 下隣
	if(iv1 > 0)
	{
		if(_cellarr[ih1][iv1 - 1]._elt == c1->_elt)
		{
			cntNeibSame++;
			checkCellSublime_subDive(numComb, ih1, iv1 - 1, phase);
		}
	}
	// 左隣
	if(ih1 > 0)
	{
		if(_cellarr[ih1 - 1][iv1]._elt == c1->_elt)
		{
			cntNeibSame++;
			checkCellSublime_subDive(numComb, ih1 - 1, iv1, phase);
		}
	}

	if(phase == 2)
	{
		if(cntNeibSame >= 1)
		{
			ASSERT(c1->_elt > 0);
			_cgwkfarr[ih1][iv1] |= (cntNeibSame >= 2) ? 1 : 2;	// 変更フラグを立てる
		}
	}
}

/*---------------------------------------------------------------------*//**
	固定擬似乱数
**//*---------------------------------------------------------------------*/
s32 PurePuzzle::randFixed()
{
	u32 seed1 = 19770328;
	u32 seed2 = 555555555;
	u32 val = ((_varFixedRand + seed2) * seed1) & 0xffffffff;
	_varFixedRand = ((val << 16) & 0xffff0000) | ((val >> 16) & 0x0000ffff);
	return _varFixedRand;
}


/*---------------------------------------------------------------------*//**
	隣接元素タイプかを判定する
**//*---------------------------------------------------------------------*/
bool PurePuzzle::isNeighbElt(u8 elt1, u8 elt2, bool isHigher, bool isIgnodeAe)
{
	if(isIgnodeAe ? (elt2 > ELT_WI) : (elt2 > ELT_AE))
	{
		if((elt2 - 1) == elt1)	{	return true;	}
	}
	if(!isHigher && (elt2 < ELT_SO))
	{
		if((elt2 + 1) == elt1)	{	return true;	}
	}
	return false;
}

//==========================================================================
// PurePuzzle::Cell メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PurePuzzle::Cell::Cell()
	: _elt(ELT_AE)
	, _hi(false)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::Cell::exec(const ExecCtx* ec)
{
	_anim.exec(ec);
}

/*---------------------------------------------------------------------*//**
	アニメーションリセット
**//*---------------------------------------------------------------------*/
void PurePuzzle::Cell::resetAnim()
{
	_anim.reset();
	_anim._eltPrev = _elt;
	_anim._hiPrev = _hi;
}

//==========================================================================
// PurePuzzle::Cell::Anim メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PurePuzzle::Cell::Anim::Anim()
{
	reset();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void PurePuzzle::Cell::Anim::exec(const ExecCtx* ec)
{
	// カウンタ更新
	_fcntFlush1 -= ec->getDeltaFrame();
	if(_fcntFlush1 <= 0.0f)
	{
		_fcntFlush1 = 0.0f;
	}
	_fcntFlush2 -= ec->getDeltaFrame();
	if(_fcntFlush2 <= 0.0f)
	{
		_fcntFlush2 = 0.0f;
	}
	_fcntFall -= ec->getDeltaFrame();
	if(_fcntFall <= 0.0f)
	{
		_fcntFall = 0.0f;
	}
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void PurePuzzle::Cell::Anim::reset()
{
	_eltPrev = ELT_AE;
	_hiPrev = false;
	_fcntFlush1 = 0.0f;
	_fcntFlush2 = 0.0f;
	_fcntFall = 0.0f;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
