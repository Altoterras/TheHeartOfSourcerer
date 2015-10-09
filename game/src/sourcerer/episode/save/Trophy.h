/***********************************************************************//**
 *	Trophy.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/08/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_SAVE_TROPHY_H_
#define _SRCR_EPISODE_SAVE_TROPHY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../save/TrophyBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;
class PsnsBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スコア
 *
**//*---------------------------------------------------------------------*/
class Trophy : public TrophyBase
{
	//======================================================================
	// 定数
public:
	enum TrophyIndex
	{
		TI_NULL,											// (  0) 無効
		TI_ITEM_ADEPT_1_SUCCEEDED_SYNTHESIS,				// (  1) アイテム合成に成功した
		TI_ITEM_ADEPT_2_HEALED_BY_ITEM,						// (  2) アイテムで回復した
		TI_ITEM_ADEPT_3_EXCHANGED_FOR_EXP,					// (  3) アイテムを渡して経験値を得た
		TI_SOUMA_ADEPT_1_CUSTOMIZED_SOURCE_OF_SOUMA,		// (  4) オリジナルソウマのソースを作成した
		TI_SOUMA_ADEPT_2_HEALED_BY_CUSTOM_SOUMA,			// (  5) オリジナルソウマにより回復した
		TI_SOUMA_ADEPT_3_DEFEATED_UNGETSU_BY_CUSTOM_SOUMA,	// (  6) オリジナルソウマで雲月岩を倒した
		TI_SOUMA_ADEPT_4_DEFEATED_SARUNAHA_BY_CUSTOM_SOUMA,	// (  7) オリジナルソウマでサルナハを倒した
		TI_SOUMA_ADEPT_5_DEFEATED_KABUNAHA_BY_CUSTOM_SOUMA,	// (  8) オリジナルソウマでカブナハを倒した
		TI_SOUMA_ADEPT_6_DEFEATED_KAGEIWA_BY_CUSTOM_SOUMA,	// (  9) オリジナルソウマで影岩を倒した
		TI_EXPLORATION_ADEPT_1_FOUND_HIDDEN_MAP,			// ( 10) 隠しマップを発見した
		NUM_TI
	};

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Trophy(PsnsBase* psnsRef);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

	//======================================================================
	// 変数
public:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_SAVE_TROPHY_H_
