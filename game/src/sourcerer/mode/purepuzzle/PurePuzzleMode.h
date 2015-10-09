/***********************************************************************//**
 *	PurePuzzleMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_PUREPUZZLE_PURE_PUZZLE_H_
#define _SRCR_MODE_PUREPUZZLE_PURE_PUZZLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
class Camera;

TFW_END_NS

SRCR_BEGIN_NS

class PrpzlOperationData;
class PrpzlPerfBase;
class PurePuzzle;

/*---------------------------------------------------------------------*//**
 *	パズル ビュー モード
 *
**//*---------------------------------------------------------------------*/
class PurePuzzleMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PurePuzzleMode();
	~PurePuzzleMode();
	bool create();
	void destroy();

protected:
	bool notifyBegin(void* objParamRef);
	short getDefaultChildMode();

	//======================================================================
	// 変数
private:
	PrpzlOperationData* _opdat;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_PUREPUZZLE_PURE_PUZZLE_H_
