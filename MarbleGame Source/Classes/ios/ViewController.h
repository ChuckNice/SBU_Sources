//
//  ViewController.h
//  marblegame_ios
//
//  Created by nitrex88 on 11/27/12.
//  Copyright (c) 2012 nitrex88. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface ViewController : GLKViewController

@property (nonatomic, retain) IBOutlet UIImageView *upButton;
@property (nonatomic, retain) IBOutlet UIImageView *downButton;
@property (nonatomic, retain) IBOutlet UIImageView *leftButton;
@property (nonatomic, retain) IBOutlet UIImageView *rightButton;
@property (nonatomic, retain) IBOutlet UIImageView *touchRadiusImageView;

@end
