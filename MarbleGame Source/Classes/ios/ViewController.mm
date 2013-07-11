//
//  ViewController.m
//  marblegame_ios
//
//  Created by nitrex88 on 11/27/12.
//  Copyright (c) 2012 nitrex88. All rights reserved.
//

#import "ViewController.h"
#include "ios_scene.h"
#include <unistd.h>

iOSScene *ios_scene = nil;


@interface ViewController ()
{
}
@property (strong, nonatomic) EAGLContext *context;
- (void)setupGL;
- (void)tearDownGL;
@end


@implementation ViewController

@synthesize upButton, downButton, rightButton, leftButton, touchRadiusImageView;

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [_context release];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setPreferredFramesPerSecond:30];
    
    [self setupGL];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
    
    // Dispose of any resources that can be recreated.
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation {
    if (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
        toInterfaceOrientation == UIInterfaceOrientationLandscapeRight)
        return YES;
    return NO;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    s_Cam()->onResize(self.view.frame.size.width, self.view.frame.size.height);
    
    // hack to set working directory to where we need it
    chdir([[[NSBundle mainBundle] resourcePath] UTF8String]);
    char * dir = getcwd(NULL, 0);
    NSLog(@"Current dir: %s\n", dir);
    
    ios_scene = new iOSScene();
    if (!ios_scene->initialize()) {
        NSLog(@"ios_scene failed to load");
        throw;
    }
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    delete ios_scene;
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    ios_scene->stepPhysics(self.timeSinceLastUpdate);
    
    ios_scene->update(self.timeSinceLastUpdate);
    
    s_Keyboard()->update();
    
    s_Mouse()->update(self.timeSinceLastUpdate);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    ios_scene->render();
}

#pragma mark - Input handling

static Vector3 s_movementVector = Vector3(0.0f, 0.0f, 0.0f);
Vector3 getiOSMovementVector() { return s_movementVector; }

static map<UITouch*, double> s_touchTime;
UITouch *leftTouch = nil;
UITouch *rightTouch = nil;
CGPoint leftTouchPoint;
CGPoint rightTouchPoint;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint touchPoint = [touch locationInView:self.view];
    if (touchPoint.x < s_Cam()->getScreenWidth() * 0.5f && leftTouch == nil) {
        leftTouch = touch;
        leftTouchPoint = touchPoint;
        s_movementVector = Vector3(0.0f, 0.0f, 0.0f);
        touchRadiusImageView.hidden = NO;
        touchRadiusImageView.center = touchPoint;
    }
    else if (rightTouch == nil) {
        rightTouch = touch;
        rightTouchPoint = touchPoint;
    }
    
    s_touchTime[touch] = touch.timestamp;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint touchPoint = [touch locationInView:self.view];
    if (touchPoint.x < s_Cam()->getScreenWidth() * 0.5f && touch == leftTouch) {
        float diffx = touchPoint.x - leftTouchPoint.x;
        float diffy = touchPoint.y - leftTouchPoint.y;
        s_movementVector = Vector3(diffx, diffy, 0.0f);
    }
    else if (touch == rightTouch) {
        int diffx = (rightTouchPoint.x - touchPoint.x) * 3.0f;
        int diffy = (rightTouchPoint.y - touchPoint.y) * 3.0f;
        int x, y;
        s_Mouse()->getMousePos(x, y);
        s_Mouse()->setMousePos(x-diffx, y-diffy);
        rightTouchPoint = touchPoint;
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    if (touch == leftTouch) {
        s_movementVector = Vector3(0.0f, 0.0f, 0.0f);
        touchRadiusImageView.hidden = YES;
        leftTouch = nil;
    }
    else if (touch == rightTouch) {
        rightTouch = nil;
    }
    
    // detect quick tap for jump
    if (touch.timestamp - s_touchTime[touch] < 0.1) {
        s_Keyboard()->handleKeyDown(' ');
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.2f * NSEC_PER_SEC), dispatch_get_main_queue(), ^(void) {
            s_Keyboard()->handleKeyUp(' ');
        });
    }
}


@end
