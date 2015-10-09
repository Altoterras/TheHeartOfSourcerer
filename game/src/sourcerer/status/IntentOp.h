/***********************************************************************//**
 *	IntentOp.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _GAME_STATUS_INTENT_OP_H_
#define _GAME_STATUS_INTENT_OP_H_

////////////////////////////////////////////////////////////////////////////
// クラス

GAME_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ステータス影響の意図
 *
**//*---------------------------------------------------------------------*/
class IntentOp
{
	//======================================================================
	// 定数
protected:
	enum
	{
		ITO_SAT	= 0,		// 癒し、浄化（サットヴァ）
		ITO_RAJ,			// 攻撃、変換（ラジャス）
		ITO_TAM,			// 守り、安定（タマス）
		ITO_SATRAJ,			// 攻撃強化（サットヴァ＋ラジャス）
		ITO_RAJTAM,			// 弱体（ラジャス＋タマス）
		ITO_TAMSAT,			// 防御結界（タマス＋サットヴァ）
	};
};

GAME_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _GAME_STATUS_INTENT_OP_H_
