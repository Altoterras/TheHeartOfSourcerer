/***********************************************************************//**
 *	MapEmp_10242_1701_RestAtHome.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/31.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10242_1701_REST_AT_HOME_H_
#define _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10242_1701_REST_AT_HOME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MapEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アサ宅での休息イベント
 *
**//*---------------------------------------------------------------------*/
class MapEmp_10242_1701_RestAtHome : public MapEmp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapEmp_10242_1701_RestAtHome();
	void begin(u32 param);
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

private:
	void restAtHome();

	//======================================================================
	// 変数
private:
	bool _isShowMotherMsg;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10242_1701_REST_AT_HOME_H_
