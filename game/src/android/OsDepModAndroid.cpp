/***********************************************************************//**
 *	OsDepModAndroid.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/12/10.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OsDepModAndroid.h"

// Friends

// External
#include "../tfw/file/File.h"
#include "../tfw/string/CcString.h"
#include "../tfw/string/VcString.h"
#include "../tfw/string/VwString.h"
#include "../tfw/txt/TxtUtils.h"
#include "../tfw/txt/implement/SjisTxt.h"
#include "../tfw/txt/implement/Utf8Txt.h"

// Library
#include <android_native_app_glue.h>
#include <jni.h>

////////////////////////////////////////////////////////////////////////////

namespace app
{

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OsDepModAndroid メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アプリケーションのバージョン文字列を得る
**//*---------------------------------------------------------------------*/
void OsDepModAndroid::getAppVersionString(VcString* ver)
{
	*ver = "3.0";
}

/*---------------------------------------------------------------------*//**
	リソースパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::getResourceDirPath(tfw::VcString* path, const CcString prod)
{
	MediaState mstat = checkExternalMediaState();
	if((mstat == MSTAT_READ_WRITE) || (mstat == MSTAT_READ_ONLY))
	{
		// 外部保存領域を使う
		CcString subdir("/Android/obb/");
		if(!getExternalStorageDirPath(path, &subdir, &prod, false))	{	return false;	}
	}
	else
	{
		// AssetManager 管理のファイルを使うのだと思われる
		path->set("");
	}

	// 製品定義を保存する
	_prod.set(&prod);

	TRACE("{OsDepModAndroid::getResourceDirPath} path: %s\n", path->getRaw());
	return true;
}

/*---------------------------------------------------------------------*//**
	ドキュメントパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::getDocumentDirPath(tfw::VcString* path, bool isMakeDirIfNoExist)
{
	MediaState mstat = checkExternalMediaState();
	if(mstat == MSTAT_READ_WRITE)
	{
		// 外部保存領域を使う
		CcString subdir("/Android/data/");
		if(!getExternalStorageDirPath(path, &subdir, &_prod, isMakeDirIfNoExist))	{	return false;	}
	}
	else
	{
		JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;
		jobject jobjActivity = ((AndroidMainModule*)OsModule::getMainModule())->_clazzActivity;

		jclass jclsActivity = jni->FindClass("android/app/Activity");
		if(jclsActivity == NULL)							{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 101 ###\n");	return false;	}
		jclass jclsContext = jni->FindClass("android/content/Context");
		if(jclsContext == NULL)								{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 102 ###\n");	return false;	}
		jclass jclsFile = jni->FindClass("java/io/File");
		if(jclsFile == NULL)								{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 103 ###\n");	return false;	}

		jmethodID jmethodGetFilesDir = jni->GetMethodID(jclsContext, "getFilesDir", "()Ljava/io/File;");
		if(jmethodGetFilesDir == NULL)						{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 104 ###\n");	return false;	}
		jmethodID jmethodGetPath = jni->GetMethodID(jclsFile, "getPath", "()Ljava/lang/String;");
		if(jmethodGetPath == NULL)							{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 105 ###\n");	return false;	}

		jobject jobjFile = jni->CallObjectMethod(jobjActivity, jmethodGetFilesDir);
		if(jobjFile == NULL)								{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 201 ###\n");	return false;	}
		jstring jstrFilesDirPath = (jstring)jni->CallObjectMethod(jobjFile, jmethodGetPath);
		if(jstrFilesDirPath == NULL)						{	TRACE("{OsDepModAndroid::getDocumentDirPath} ### 202 ###\n");	return false;	}

		jsize lenPathStr = jni->GetStringLength(jstrFilesDirPath);
		const char* charsPathStr = jni->GetStringUTFChars(jstrFilesDirPath, NULL);
		path->set(charsPathStr, (int)lenPathStr);
		jni->ReleaseStringUTFChars(jstrFilesDirPath, charsPathStr);

		TRACE("{OsDepModAndroid::getDocumentDirPath} path: %s\n", path->getRaw());
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	一時ディレクトリのパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::getTemporaryDirPath(VcString* path)
{
	/*
	NSString* nspathTemp = NSTemporaryDirectory();
	*path = [nspathTemp UTF8String];
	if(path->getLength() > 1)
	{
		char c = path->getAt(path->getLength() - 1);
		if((c != '/') && (c != '\\'))	{	*path += '/';	}
	}
	return true;
	*/
	return false;
}

/*---------------------------------------------------------------------*//**
	外部記憶装置保存ディレクトリのパスを取得
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::getExternalStorageDirPath(tfw::VcString* path, const tfw::CStringBase* subdir, const tfw::CStringBase* prod, bool isMakeDirIfNoExist)
{
	JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

	jclass jclsEnvironment = jni->FindClass("android/os/Environment");
	if(jclsEnvironment == NULL)							{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 101 ###\n");	return false;	}
	jclass jclsFile = jni->FindClass("java/io/File");
	if(jclsFile == NULL)								{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 102 ###\n");	return false;	}

	jmethodID jmethodEnvGetExternalStorageDirectory = jni->GetStaticMethodID(jclsEnvironment, "getExternalStorageDirectory", "()Ljava/io/File;");
	if(jmethodEnvGetExternalStorageDirectory == NULL)	{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 201 ###\n");	return false;	}
	jmethodID jmethodFileGetAbsolutePath = jni->GetMethodID(jclsFile, "getAbsolutePath", "()Ljava/lang/String;");
	if(jmethodFileGetAbsolutePath == NULL)				{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 202 ###\n");	return false;	}

	// Environment.getExternalStorageDirectory()
	jobject jobjExternalStorateDirectory = jni->CallStaticObjectMethod(jclsEnvironment, jmethodEnvGetExternalStorageDirectory);
	if(jobjExternalStorateDirectory == NULL)			{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 301 ###\n");	return false;	}
	// String pathExternalStorateDir = externalStorateDirectory.getAbsolutePath()
	jstring jstrExternalStorateDirectoryPath = (jstring)jni->CallObjectMethod(jobjExternalStorateDirectory, jmethodFileGetAbsolutePath);
	if(jstrExternalStorateDirectoryPath == NULL)		{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 302 ###\n");	return false;	}

	// パス文字列取得
	int len = jni->GetStringUTFLength(jstrExternalStorateDirectoryPath);
	const char* chars = jni->GetStringUTFChars(jstrExternalStorateDirectoryPath, NULL);
	path->set(chars, len);
	jni->ReleaseStringUTFChars(jstrExternalStorateDirectoryPath, chars);
		
	path->add(subdir);
	path->add(prod);
	path->add("/");

	// ディレクトリを作成する
	if(isMakeDirIfNoExist)
	{
		jmethodID jmethodFileCtor = jni->GetMethodID(jclsFile, "<init>", "(Ljava/lang/String;)V");
		if(jmethodFileCtor == NULL)							{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 401 ###\n");	return false;	}
		jmethodID jmethodExists = jni->GetMethodID(jclsFile, "exists", "()Z");
		if(jmethodExists == NULL)							{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 402 ###\n");	return false;	}
		jmethodID jmethodMkdirs = jni->GetMethodID(jclsFile, "mkdirs", "()Z");
		if(jmethodMkdirs == NULL)							{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 403 ###\n");	return false;	}

		// String strNewPath = new String(...)
		jstring jstrNewPath = jni->NewStringUTF(path->getRaw());
		if(jstrNewPath == NULL)								{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 501 ###\n");	return false;	}
		// File fileNewPath = new File(strNewPath)
		jobject jobjNewPathFile = jni->NewObject(jclsFile, jmethodFileCtor, jstrNewPath); 
		if(jobjNewPathFile == NULL)							{	TRACE("{OsDepModAndroid::getExternalStorageDirPath} ### 502 ###\n");	return false;	}

		// fileNewPath.exists()
		jboolean jboResult = jni->CallBooleanMethod(jobjNewPathFile, jmethodExists);
		if(!jboResult)
		{
			// fileNewPath.mkdirs()
			jboolean jboResult = jni->CallBooleanMethod(jobjNewPathFile, jmethodMkdirs);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	外部記憶装置の状態取得
**//*---------------------------------------------------------------------*/
OsDepModAndroid::MediaState OsDepModAndroid::checkExternalMediaState() const
{
	JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

	jclass jclsEnvironment = jni->FindClass("android/os/Environment");
	if(jclsEnvironment == NULL)							{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 101 ###\n");	return MSTAT_NULL;	}
	jclass jclsString = jni->FindClass("java/lang/String");
	if(jclsString == NULL)								{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 102 ###\n");	return MSTAT_NULL;	}

	jmethodID jmethodEnvGetExternalStorageState = jni->GetStaticMethodID(jclsEnvironment, "getExternalStorageState", "()Ljava/lang/String;");
	if(jmethodEnvGetExternalStorageState == NULL)		{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 201 ###\n");	return MSTAT_NULL;	}
	jmethodID jmethodStringEquals = jni->GetMethodID(jclsString, "equals", "(Ljava/lang/Object;)Z");
	if(jmethodStringEquals == NULL)						{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 202 ###\n");	return MSTAT_NULL;	}

	jfieldID jfieldMediaMounted = jni->GetStaticFieldID(jclsEnvironment, "MEDIA_MOUNTED", "Ljava/lang/String;");
	if(jfieldMediaMounted == NULL)						{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 301 ###\n");	return MSTAT_NULL;	}
	jfieldID jfieldMediaMountedReadOnly = jni->GetStaticFieldID(jclsEnvironment, "MEDIA_MOUNTED_READ_ONLY", "Ljava/lang/String;");
	if(jfieldMediaMountedReadOnly == NULL)				{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 302 ###\n");	return MSTAT_NULL;	}

	jstring jstrState = (jstring)jni->CallStaticObjectMethod(jclsEnvironment, jmethodEnvGetExternalStorageState);
	if(jstrState == NULL)								{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 401 ###\n");	return MSTAT_NULL;	}

	jstring jstringMediaMounted = (jstring)jni->GetStaticObjectField(jclsEnvironment, jfieldMediaMounted);
	if(jstringMediaMounted == NULL)						{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 501 ###\n");	return MSTAT_NULL;	}
	jstring jstringMediaMountedReadOnly = (jstring)jni->GetStaticObjectField(jclsEnvironment, jfieldMediaMountedReadOnly);
	if(jstringMediaMountedReadOnly == NULL)				{	TRACE("{OsDepModAndroid::checkExternalMediaState} ### 502 ###\n");	return MSTAT_NULL;	}

	jboolean jbRes = jni->CallBooleanMethod(jstrState, jmethodStringEquals, jstringMediaMounted);
	if(jbRes)
	{
		return MSTAT_READ_WRITE;
	}

	jbRes = jni->CallBooleanMethod(jstrState, jmethodStringEquals, jstringMediaMountedReadOnly);
	if(jbRes)
	{
		return MSTAT_READ_ONLY;
	}

	return MSTAT_NULL;
}

/*---------------------------------------------------------------------*//**
	ホームページへのジャンプ動作
**//*---------------------------------------------------------------------*/
void OsDepModAndroid::gotoHp(CStringBase* str) const
{
	JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;
	jobject jobjActivity = ((AndroidMainModule*)OsModule::getMainModule())->_clazzActivity;

	// 各クラス取得
	jclass jclsActivity = jni->GetObjectClass(jobjActivity);
	if(jclsActivity == NULL)							{	TRACE("{OsDepModAndroid::gotoHp} ### 101 ###\n");	return;	}
	jclass jclsUri = jni->FindClass("android/net/Uri");
	if(jclsUri == NULL)									{	TRACE("{OsDepModAndroid::gotoHp} ### 102 ###\n");	return;	}
	jclass jclsIntent = jni->FindClass("android/content/Intent");
	if(jclsIntent == NULL)								{	TRACE("{OsDepModAndroid::gotoHp} ### 103 ###\n");	return;	}

	// Uri uri = Uri.parse("http://....");
	jmethodID jmethodUriParse = jni->GetStaticMethodID(jclsUri, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
	if(jmethodUriParse == NULL)							{	TRACE("{OsDepModAndroid::gotoHp} ### 201 ###\n");	return;	}
	jstring jstrUrl = jni->NewStringUTF(str->getRaw());
	if(jstrUrl == NULL)									{	TRACE("{OsDepModAndroid::gotoHp} ### 202 ###\n");	return;	}
	jvalue jvalUriParseArgs[1];
	jvalUriParseArgs[0].l = jstrUrl;
	jobject jobjUri = jni->CallStaticObjectMethodA(jclsUri, jmethodUriParse, jvalUriParseArgs);
	if(jobjUri == NULL)									{	TRACE("{OsDepModAndroid::gotoHp} ### 203 ###\n");	return;	}

	// Intent intent = new Intent(Intent.ACTION_VIEW, uri);
	jmethodID jmethodIntentCtor = jni->GetMethodID(jclsIntent, "<init>", "(Ljava/lang/String;)V");
	if(jmethodIntentCtor == NULL)						{	TRACE("{OsDepModAndroid::gotoHp} ### 301 ###\n");	return;	}
	jfieldID jfieldActionView = jni->GetStaticFieldID(jclsIntent, "ACTION_VIEW", "Ljava/lang/String;");
	if(jfieldActionView == NULL)						{	TRACE("{OsDepModAndroid::gotoHp} ### 302 ###\n");	return;	}
	jobject jobjActionView = jni->GetStaticObjectField(jclsIntent, jfieldActionView);
	if(jobjActionView == NULL)							{	TRACE("{OsDepModAndroid::gotoHp} ### 303 ###\n");	return;	}
	jvalue jvalIntentCtorArgs[2];
	jvalIntentCtorArgs[0].l = jobjActionView;
	jvalIntentCtorArgs[1].l = jobjUri;
	jobject jobjIntent = jni->NewObjectA(jclsIntent, jmethodIntentCtor, jvalIntentCtorArgs);
	if(jobjIntent == NULL)								{	TRACE("{OsDepModAndroid::gotoHp} ### 304 ###\n");	return;	}
    
	// startActivity(i);
	jmethodID jmethodStartActivity = jni->GetMethodID(jclsActivity, "startActivity", "(Landroid/content/Intent;)V");
	if(jmethodStartActivity == NULL)					{	TRACE("{OsDepModAndroid::gotoHp} ### 401 ###\n");	return;	}
	jvalue jvalAtartActivityArgs[1];
	jvalAtartActivityArgs[0].l = jobjIntent;
	jni->CallVoidMethodA(jobjActivity, jmethodStartActivity, jvalAtartActivityArgs);
}

/*---------------------------------------------------------------------*//**
	広告バナー表示制御
**//*---------------------------------------------------------------------*/
void OsDepModAndroid::showAdBanner(bool show) const
{
	ASSERT(false);	// 未実装
}

/*---------------------------------------------------------------------*//**
	クリップボードからペーストするテキストを得る
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::getPasteTextFromClipboard(Txt** txt)
{
	JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;
	jobject jobjActivity = ((AndroidMainModule*)OsModule::getMainModule())->_clazzActivity;

	jclass jclsClipboardManager = jni->FindClass("android/text/ClipboardManager");
	if(jclsClipboardManager == NULL)					{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 101 ###\n");	return false;	}
	jclass jclsContext = jni->FindClass("android/content/Context");
	if(jclsContext == NULL)								{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 102 ###\n");	return false;	}

	jmethodID jmethodGetSystemService = jni->GetMethodID(jclsContext, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	if(jmethodGetSystemService == NULL)					{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 201 ###\n");	return false;	}
	jmethodID jmethodCmGetText = jni->GetMethodID(jclsClipboardManager, "getText", "()Ljava/lang/CharSequence;");
	if(jmethodCmGetText == NULL)						{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 202 ###\n");	return false;	}

	jfieldID jfieldClipboardService = jni->GetStaticFieldID(jclsContext, "CLIPBOARD_SERVICE", "Ljava/lang/String;");
	if(jfieldClipboardService == NULL)					{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 301 ###\n");	return false;	}

	jobject jobjClipboardService = jni->GetStaticObjectField(jclsContext, jfieldClipboardService);
	if(jobjClipboardService == NULL)					{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 302 ###\n");	return false;	}

	// ClipboardManager cm = (ClipboardManager)getSystemService(CLIPBOARD_SERVICE);
	jvalue jvalGetSystemServiceArgs[1];
	jvalGetSystemServiceArgs[0].l = jobjClipboardService;
	jobject jobjClipboardManager = jni->CallObjectMethodA(jobjActivity, jmethodGetSystemService, jvalGetSystemServiceArgs);
	if(jobjClipboardManager == NULL)					{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 401 ###\n");	return false;	}

	// String strClipText = cm.getText();
	jstring jstrClipText = (jstring)jni->CallObjectMethod(jobjClipboardManager, jmethodCmGetText);
	if(jstrClipText == NULL)							{	TRACE("{OsDepModAndroid::getPasteTextFromClipboard} ### 402 ###\n");	return false;	}
	VcString strClipText;
	int len = jni->GetStringLength(jstrClipText);
	const char* chars = jni->GetStringUTFChars(jstrClipText, NULL);
	strClipText.set(chars, (int)len);
	jni->ReleaseStringUTFChars(jstrClipText, chars);

	SjisTxt* sjistxt = new SjisTxt();
	Utf8Txt utf8txt;
	utf8txt.setSingleString(&strClipText);
	TxtUtils::convUtf8ToSjisText(sjistxt, &utf8txt);

	*txt = sjistxt;
	return true;
}

/*---------------------------------------------------------------------*//**
	クリップボードへコピーするテキストを設定する
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::setCopyTextToClipboard(const Txt* txt)
{
	JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;
	jobject jobjActivity = ((AndroidMainModule*)OsModule::getMainModule())->_clazzActivity;

	jclass jclsClipboardManager = jni->FindClass("android/text/ClipboardManager");
	if(jclsClipboardManager == NULL)					{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 101 ###\n");	return false;	}
	jclass jclsContext = jni->FindClass("android/content/Context");
	if(jclsContext == NULL)								{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 102 ###\n");	return false;	}

	jmethodID jmethodGetSystemService = jni->GetMethodID(jclsContext, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	if(jmethodGetSystemService == NULL)					{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 201 ###\n");	return false;	}
	jmethodID jmethodCmSetText = jni->GetMethodID(jclsClipboardManager, "setText", "(Ljava/lang/CharSequence;)V");
	if(jmethodCmSetText == NULL)						{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 202 ###\n");	return false;	}

	jfieldID jfieldClipboardService = jni->GetStaticFieldID(jclsContext, "CLIPBOARD_SERVICE", "Ljava/lang/String;");
	if(jfieldClipboardService == NULL)					{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 301 ###\n");	return false;	}

	jobject jobjClipboardService = jni->GetStaticObjectField(jclsContext, jfieldClipboardService);
	if(jobjClipboardService == NULL)					{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 302 ###\n");	return false;	}

	// utf-8 の文字列を得る
	Utf8Txt utftxt;
	if(txt->getEncode() == TxtDef::ENC_SJIS)
	{
		TxtUtils::convSjisToUtf8Text(&utftxt, (SjisTxt*)txt);
		txt = &utftxt;
	}
	VcString strClipText;
	txt->takeRawString(&strClipText);

	// ClipboardManager cm = (ClipboardManager)getSystemService(CLIPBOARD_SERVICE);
	jvalue jvalGetSystemServiceArgs[1];
	jvalGetSystemServiceArgs[0].l = jobjClipboardService;
	jobject jobjClipboardManager = jni->CallObjectMethodA(jobjActivity, jmethodGetSystemService, jvalGetSystemServiceArgs);
	if(jobjClipboardManager == NULL)					{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 401 ###\n");	return false;	}

	// cm.setText(strClipText);
	jstring jstrClipText = jni->NewStringUTF(strClipText.getRaw());
	if(jstrClipText == NULL)							{	TRACE("{OsDepModAndroid::setCopyTextToClipboard} ### 402 ###\n");	return false;	}
	jni->CallVoidMethod(jobjClipboardManager, jmethodCmSetText, jstrClipText);

	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを作成する
**//*---------------------------------------------------------------------*/
u32 OsDepModAndroid::createTextField(const Txt* txt, const RectF32* rect, const View* view, u32 flags)
{
	ASSERT(false);	// 未実装
	return 0;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを破棄する
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::destroyTextField(u32 oscmpid)
{
	ASSERT(false);	// 未実装
	return false;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを表示（非表示）する
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::showTextField(u32 oscmpid, bool isShow)
{
	ASSERT(false);	// 未実装
	return false;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドの文字列を得る
**//*---------------------------------------------------------------------*/
bool OsDepModAndroid::getTextFieldString(u32 oscmpid, Txt** txt)
{
	ASSERT(false);	// 未実装
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OsDepModAndroid::OsDepModAndroid(JNIEnv* jnienv, jobject jobjActivity)
	: OsDepMod(&_mainmod)
{
	_mainmod._jnienv = jnienv;
	_mainmod._clazzActivity = jobjActivity;
}

////////////////////////////////////////////////////////////////////////////

} // namespace app
