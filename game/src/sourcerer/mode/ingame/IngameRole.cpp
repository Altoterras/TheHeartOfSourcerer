/***********************************************************************//**
 *	IngameRole.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "IngameRole.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IngameRole メソッド

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int IngameRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	switch(param->_cid)
	{
	case START:	return execCommand_start(res, ec, param);
	case END:	return execCommand_end(res, ec, param);
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	開始演出
**//*---------------------------------------------------------------------*/
int IngameRole::execCommand_start(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_FIRST			= 0;
	const int	STEP_INTVL_MSG		= 1;
	const int	STEP_MSG			= 2;
	const int	INTVL_END			= 998;
	const int	END					= 999;
	
	int step = param->_step;
	
	switch(step)
	{
	case STEP_FIRST:
		{
			/*
			ScreenPresentation scrprs = Global.GetGame().GetScreenPresentation();
			scrprs.SetColor1(new ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			scrprs.Start(ScreenPresentation.MODE_FADE_IN);
			*/
		}
		step = STEP_INTVL_MSG;
		break;
		
	case STEP_INTVL_MSG:
		if(param->_frmcntStep >= 15.0f)	{	step = STEP_MSG;	}
		break;

	case STEP_MSG:
		step = INTVL_END;
		break;
		
	case INTVL_END:
		if(param->_frmcntStep >= 6.0f)	{	step = END;			}
		break;
		
	case END:
		if(res != 0L)	{	res->setDone();	}
		break;
	}
	
	return step;
}

/*---------------------------------------------------------------------*//**
	終了演出
**//*---------------------------------------------------------------------*/
int IngameRole::execCommand_end(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_FIRST			= 0;
	const int	STEP_INTVL_MSG		= 1;
	const int	STEP_MSG			= 2;
	const int	INTVL_END			= 998;
	const int	END					= 999;
	
	int step = param->_step;
	
	switch(step)
	{
	case STEP_FIRST:
		/*
		{
			ScreenPresentation scrprs = Global.GetGame().GetScreenPresentation();
			scrprs.SetColor1(new ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			scrprs.Start(ScreenPresentation.MODE_FADE_OUT);
		}
		*/
		step = STEP_INTVL_MSG;
		break;
		
	case STEP_INTVL_MSG:
		if(param->_frmcntStep >= 15.0f)	{	step = STEP_MSG;	}
		break;

	case STEP_MSG:
		step = INTVL_END;
		break;
		
	case INTVL_END:
		if(param->_frmcntStep >= 6.0f)	{	step = END;			}
		break;
		
	case END:
		if(res != 0L)	{	res->setDone();	}
		break;
	}
	
	return step;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
