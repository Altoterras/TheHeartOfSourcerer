/***********************************************************************//**
 *	PurePuzzle.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/11.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PUREPUZZLE_PURE_PUZZLE_H_
#define _SRCR_PUREPUZZLE_PURE_PUZZLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class PrpzlOperationData;
class PrpzlPerfBase;

/*---------------------------------------------------------------------*//**
 *	パズル
 *
**//*---------------------------------------------------------------------*/
class PurePuzzle
{
	//======================================================================
	// 定数
public:
	// セル元素の種類
	static const u8 ELT_AE		= 0;
	static const u8 ELT_WI		= 1;
	static const u8 ELT_FI		= 2;
	static const u8 ELT_WA		= 3;
	static const u8 ELT_SO		= 4;

	// セルの数
	static const s32 NUM_CELL_H	= 8;
	static const s32 NUM_CELL_V	= 16;

	// 自動アクション
	enum AutoAct
	{
		AA_NULL,
		AA_FALL,		// セル落下
		AA_COMB,		// セル同種結合
		AA_REACT,		// 隣接セル反応
		AA_SUBLIM,		// セル同色昇華
	};

	// ターン
	enum Turn
	{
		TURN_AA,
		TURN_USER,
	};

	// フレームカウント
	static const f32 FCNT_ANIM_MAX;		// 自動アクションのアニメーションカウント数
	static const f32 FCNT_FALL_MAX;		// 自動アクションの落下フレームカウント数
	static const f32 FCNT_FLUSH;		// フラッシュ表示のフレームカウント数

private:
	// 汎用作業フラグ
	static const u8 GWKF_CHECKED_MASK	= 0xf0;
	static const s32 GWKF_CHECKED_SHIFT	= 4;
	static const u8 GWKF_LOCAL_MASK		= 0x0f;

	//======================================================================
	// クラス
public:
	class Cell
	{
	public:
		class Anim
		{
		public:
			u8 _eltPrev;
			bool _hiPrev;
			f32 _fcntFlush1;
			f32 _fcntFlush2;
			f32 _fcntFall;

			Anim();
			void exec(const ExecCtx* ec);
			void reset();
		};
		
		u8 _elt;
		bool _hi;
		Anim _anim;

		Cell();
		void exec(const ExecCtx* ec);
		void resetAnim();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline PrpzlOperationData* getOpData() const { return _opdatRef; }
	inline const Cell* getCell(s32 ih, s32 iv) const { return &_cellarr[ih][iv]; }
	inline Cell* cell(s32 ih, s32 iv) { return &_cellarr[ih][iv]; }
	inline Turn getTurn() const { return _turn; }
	inline f32 getAutoActFrameCount() const { return _fcntAaAnim; }
	inline s32 getChainCount() const { return _cntChain; }
	inline s32 getScore() const { return _score; }
	inline u8 getNextChipElt(s32 index) const { return _eltNext; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void reset();
	void nextChip();
	void nextChip(u8 elt) { _eltNext = elt; }
	void turnToAutoAct();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PurePuzzle();
	~PurePuzzle();
	void exec(ExecRes* res, const ExecCtx* ec);
	void render(const RenderCtx* rc);
	void draw(const RenderCtx* rc);
	bool begin(PrpzlOperationData* opdatRef, PrpzlPerfBase* perfRef);
	void end();

private:
	inline void clearCellTmpFlags() { TFW_ZERO_MEMORY(_cgwkfarr, sizeof(u8) * NUM_CELL_H * NUM_CELL_V); }
	void clearCellAnim();

	void resetCells();
	void setupStartCellsRandom();
	bool checkCellFall();
	void checkCellFall_subDive(s32* numFall, s32 ih1, s32 iv1, s32 phase);
	bool checkCellCombine();
	void checkCellCombine_subDive(bool* isChange, s32* numComb, s32 ih1, s32 iv1);
	bool checkCellReact();
	void checkCellReact_sub(bool* isChange, s32 ih1, s32 iv1, u8 eltAbout);
	bool checkCellSublime();
	void checkCellSublime_subDive(s32* numComb, s32 ih1, s32 iv1, s32 phase);

	static s32 randFixed();
	static bool isNeighbElt(u8 elt1, u8 elt2, bool isHigher, bool isIgnodeAe);

	//======================================================================
	// 変数
private:
	PrpzlOperationData* _opdatRef;
	PrpzlPerfBase* _perfRef;

	Cell _cellarr[NUM_CELL_H][NUM_CELL_V];	// セル配列
	u8 _cgwkfarr[NUM_CELL_H][NUM_CELL_V];	// セル汎用作業フラグ配列

	Turn _turn;
	AutoAct _aa;
	bool _isAaActing;
	f32 _fcntAaAnim;
	s32 _cntChain;
	s32 _score;
	u8 _eltNext;

	static s32 _varFixedRand;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PUREPUZZLE_PURE_PUZZLE_H_
