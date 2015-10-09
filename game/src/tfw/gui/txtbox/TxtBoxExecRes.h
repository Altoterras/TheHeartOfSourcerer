/***********************************************************************//**
 *  TxtBoxExecRes.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/12/22.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GUI_TXTBOX_TXT_BOX_EXEC_RES_H_
#define _TFW_GUI_TXTBOX_TXT_BOX_EXEC_RES_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../framemod/FrameExecDraw.h"

#include "TxtBox.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	テキスト ボックス フレーム制御結果
 *	
**//*---------------------------------------------------------------------*/
class TxtBoxExecRes : public ExecRes
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline TxtBox::Command getCommand()	{	return (TxtBox::Command)_value1;	} 
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GUI_TXTBOX_TXT_BOX_EXEC_RES_H_
