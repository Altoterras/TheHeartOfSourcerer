/***********************************************************************//**
 *	FileSerializer.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_FILE_SERIALIZER_H_
#define _TFW_FILE_FILE_SERIALIZER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../serialize/Serializer.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

/*---------------------------------------------------------------------*//**
 *	ファイルによる直列化モジュール
 *	
**//*---------------------------------------------------------------------*/
class FileSerializer : public Serializer
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool store(const void* val, s32 size);
	virtual bool restore(void* val, s32 size);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FileSerializer();
	FileSerializer(FileBase* fileRef);
	~FileSerializer();
	void setFile(FileBase* file, bool isDeleteFileInstanceDelegating);

private:

	//======================================================================
	// 変数
private:
	FileBase* _file;
	bool _isDeleteFileInstanceDelegating;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_FILE_SERIALIZER_H_