/***********************************************************************//**
*	main.m
*	Enlight iPhone Application
*	
*	Created by Ryoutarou Kishi on 2009/04/10.
*	Copyright 2009 Altoterras Corporation. All rights reserved.
*
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import <UIKit/UIKit.h>

////////////////////////////////////////////////////////////////////////////
// グローバル関数

/*---------------------------------------------------------------------*//**
	main 関数 - アプリケーションのエントリポイント
**//*---------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	#if __has_feature(objc_arc)
		@autoreleasepool
		{
			@try
			{
				return UIApplicationMain(argc, argv, nil, @"EnlightAppDelegate");
			}
			@catch (NSException *exception)
			{
				NSLog(@"%@", exception);
			}
		}
	#else
		@try
		{
			NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
			int retval = UIApplicationMain(argc, argv, nil, @"EnlightAppDelegate");
			[pool release];
			return retval;
		}
		@catch (NSException *exception)
		{
			NSLog(@"%@", exception);
		}
	#endif
	return 0;
}
