/***********************************************************************//**
 *	EvMatterFactory.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_EV_MATTER_FACTORY_H_
#define _TFW_EVSYS_EV_MATTER_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class EvMatter;
class CStringBase;

/*---------------------------------------------------------------------*//**
 *	イベント マター ファクトリー
 *
**//*---------------------------------------------------------------------*/
class EvMatterFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual EvMatter* newEventMatter(const CStringBase* nameMatter) = 0;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_EVSYS_EV_MATTER_FACTORY_H_
