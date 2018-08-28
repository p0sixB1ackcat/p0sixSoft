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
#import "PBCSecurityAPI.h"
#import "PBCMemoryPool.h"
#import "MJExtension.h"

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
    
    NSString *enc = [PBCEncrypt RSAEncryptStr:@"0123456789" key:nil];
    
    //NSLog(@"enc is %@",enc);
    
    NSString *dec = [PBCEncrypt RSADecryptStr:enc key:nil];
    
    //NSLog(@"dec is %@",dec);
    
    void *buffer[0x3] = {0x00};
    for(int i = 0; i < 2; i++)
    {
        buffer[i] = PBCMalloc();
        printf("buffer[%d] = %p\n",i,buffer[i]);
    }
    
    flagMemoryUnoccupied(buffer[0]);

    printf("内存块:%p被标记为空闲\n",buffer[0]);
    
    void *newBuffer0 = PBCMalloc();
    printf("newBuffer0 address is %p\n",newBuffer0);
    
    NSMutableDictionary *dic = [NSMutableDictionary dictionaryWithDictionary:@{@"v1":@"v1",@"v2":@"v2",@"1":@"1",@"2":@"2",@"abc":@"abc",@"abc1":@"abc1",@"key":@"30819f300d06092a864886f70d010101050003818d00308189028181008f54545f47cccd297cbc745d0b3ffcb18351904a5d15463e1629a6bafa813453b5083be618401af6cebe636575ffacad9b92765208b6364d5e60fab1cf21f61088c80c906a37cc41ee39138bdd79c652504688a668a751c308ed4bae84145617012612ad50450000769272b005464a0e0243a4b0883a1a261dfd0c31e865d7510203010001",@"zyl1":@"zyl1",@"123":@"123"}];
    
    NSLog(@"dic is %@",dic);
    NSString *json = [dic mj_JSONString];
    NSLog(@"json is %@",json);
    
    NSString *newJson = [PBCJsonSort JsonSort:json];
    NSLog(@"newJson is %@",newJson);
    
    NSDictionary *newdic = [newJson mj_JSONObject];
    
    NSLog(@"newdic is %@",newdic);
    
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
