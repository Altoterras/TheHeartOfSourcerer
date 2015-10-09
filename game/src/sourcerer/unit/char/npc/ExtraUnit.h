/***********************************************************************//**
 *	ExtraUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_NPC_EXTRA_UNIT_H_
#define _SRCR_UNIT_CHAR_NPC_EXTRA_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "NpcCharUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;

TFW_END_NS

SRCR_BEGIN_NS

class ExtraSpirit;

/*---------------------------------------------------------------------*//**
 *	脇役ユニット
 *
**//*---------------------------------------------------------------------*/
class ExtraUnit : public NpcCharUnit
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 自らの（型の）スピリットを得る
	inline ExtraSpirit* getThisSpirit() const { return (ExtraSpirit*)getSpirit(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			setDetectTarget(Unit* unitTrg);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ExtraUnit();
	virtual ~ExtraUnit();
	virtual bool	create(Spirit* sprtRef, u16 chrdid);
//	virtual void	destroy();
	virtual void	exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:

	//======================================================================
	// 変数
protected:

private:
	s32				_cntDetect;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_NPC_EXTRA_UNIT_H_
