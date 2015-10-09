/***********************************************************************//**
 *	EsTokenPos.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/28.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsTokenPos.h"

// Friends
#include "EsContext.h"
#include "EsTokenGetter.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsTokenPos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コードインデックスから位置を逆算する
	※重い処理なので使用は最小限にすること
**//*---------------------------------------------------------------------*/
bool EsTokenPos::calcBackwardByIndex(const EsContext* ctx, s32 index)
{
	EsTokenGetter tg(ctx->getKeywordSet(), 0L, ctx->getScriptCode(), TFW_IS_FLAG(ctx->getBehaviorModeFlags(), ES_MF_PARSE_STRING_SIGN), false);
	EsToken tknWk;
	EsTokenGetter::TokenHint th = EsTokenGetter::TH_NULL;
	while(true)
	{
		th = tg.nextToken(&tknWk);

		if(tknWk.getPos()->getBeginIndex() >= index)
		{
			this->copy(tknWk.getPos());
			return true;	// 成功
		}

		if((th == EsTokenGetter::TH_EOF) || (th == EsTokenGetter::TH_ERROR))
		{
			return false;	// 失敗
		}
	}
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
