//
//  ViewController.m
//  p0sixSoft
//
//  Created by p0sixB1ackcat on 2018/2/10.
//  Copyright © 2018年 p0sixB1ackcat. All rights reserved.
//

#import "ViewController.h"
#import "ShowbiteController.h"
#import "p0sixB1ackcat.h"

@interface ViewController ()
{
    NSTextView * _textView;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.view.window.title = @"p0sixB1ackcat";
    _textView = self.plainTextView.documentView;
    _textView.backgroundColor = [NSColor colorWithRed:38.0f/255.0f green:38.0f/255.0f blue:38.0f/255.0f alpha:0.6f];
    _textView.font = [NSFont systemFontOfSize:18.0f];
    _textView.textColor = [NSColor greenColor];
    
}

- (IBAction)getStrLenBtn:(id)sender {
    
    if (_textView.string.length > 0) {
        
        self.resultLabel.stringValue = [NSString stringWithFormat:@"%ld",_textView.string.length];
    }else{
        self.resultLabel.stringValue = @"0";
    }
    
    
    NSMutableArray * arr = [NSMutableArray arrayWithObjects:@"keys",@"bsd",@"area",@"hack",@"orderid",@"0x80",@"appid",@"appids", nil];
    
    [WXUtils strsort:arr flag:WXSortFlagAscending];
    
    //[self strsort:arr];
    
    NSLog(@"arr is %@",arr);
    
}

- (IBAction)qingkongBtn:(id)sender {
    
    if (_textView.string.length > 0) {
        
        _textView.string = @"";
        
    }
    
    self.resultLabel.stringValue = @"得到的结果会显示在这里";
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end