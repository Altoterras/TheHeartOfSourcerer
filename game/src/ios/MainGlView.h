/***********************************************************************//**
 *	MainGlView.h
 *	Enlight iPhone Application
 *	
 *	Created by Ryoutarou Kishi on 2009/04/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "Base.h"
#import "MainViewController.h"
#import "OsDepModIos.h"
#import "PsnsIos.h"
#import "StopWatchIos.h"
#import "../etk/body/EtkBody.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#define RENDER_SPF		(30.0f)
#define RENDER_FPS		(1.0f / RENDER_SPF)
#define LOGIC_SPF		(30.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	メイン OpenGL ビュー
 *	
**//*---------------------------------------------------------------------*/
@interface MainGlView : UIView
{
@private
	// The pixel dimensions of the backbuffer
	GLint _backingWidth;
	GLint _backingHeight;

	EAGLContext* _context;
	MainViewController* _vcMain;

	// OpenGL names for the renderbuffer and framebuffers used to render to this view
	GLuint _viewRenderbuffer;
	GLuint _viewFramebuffer;

	// OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist)
	GLuint _depthRenderbuffer;

	NSTimer* _animationTimer;
	NSTimeInterval _animationInterval;

	int _cntMoveIntvl;
	float _scaleCoord;
	unsigned int _gcflags;
	
	etk::EtkBody* _body;
	OsDepModIos* _osdep;
	PsnsIos* _psns;
	StopWatchIos* _swFrame;
	UITextField* _txtfldSourceEdit;
}

@property(readonly) EAGLContext* context;
@property(readonly) NSTimer* animationTimer;
@property(readonly) NSTimeInterval animationInterval;
@property(nonatomic, retain) MainViewController* mainViewController;
@property(nonatomic, readonly) UITextField* txtfldSourceEdit;

- (BOOL)initGame:(int)sgscase;
- (void)cleanupGame:(int)tgscase;
- (void)enterBackground;
- (void)leaveBackground;
- (bool)checkResidentMemoryMultiTaskingMode;
- (void)onRotateFromInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (BOOL)canBecomeFirstResponder;

- (void)startAnimation;
- (void)stopAnimation;
- (void)setAnimationInterval:(NSTimeInterval)interval;
- (void)drawView;

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;

@end

////////////////////////////////////////////////////////////////////////////
