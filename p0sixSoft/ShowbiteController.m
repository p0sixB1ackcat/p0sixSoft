//
//  ShowbiteController.m
//  WXStringLen
//
//  Created by WuXian on 16/1/21.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#import "ShowbiteController.h"
#import "p0sixB1ackcat.h"

@interface ShowbiteController (){
    
    NSMutableString * _numberStr;
    
}

@property (weak) IBOutlet NSTextField *inputSourceTextfield;

@property (weak) IBOutlet NSTextField *outputSourceLabel;

@end

@implementation ShowbiteController

- (instancetype)initWithCoder:(NSCoder *)coder
{
    if(self == [super initWithCoder:coder])
    {
        
    }
    return self;
}

- (void)awakeFromNib
{
    
}

- (void)viewDidLoad {
    
    [super viewDidLoad];
    char buff[1];
    
    size_t size = sizeof(buff);
    integertostr(123456789, buff, &size);
    
    NSString * intstr = [NSString stringWithCString:buff encoding:NSUTF8StringEncoding];
    NSLog(@"intstr is %@ size is %lu",intstr,size);
    
    self.inputSourceTextfield.backgroundColor = [NSColor whiteColor];
    
    _numberStr = [[NSMutableString alloc] init];
    
    self.title = @"转二进制";
    
}

int checkstr(const char *source)
{
    int ret = 0;
    if(!source)
        return ret;
    char *p = (char *)source;
    if(*p != '-')
        goto isnum;
    p++;
    ret = 1;
    while(*p != '\0')
    {
    isnum:
        if(*p < '0' || *p > '9')
        {
            ret = 0;
            break;
        }
        p++;
    }
    return ret;
}

- (IBAction)showBites:(id)sender {
    
    NSLog(@"showBites");
    
    if (self.inputSourceTextfield.stringValue.length > 0)
    {
        NSString * inputStr = self.inputSourceTextfield.stringValue;
        if(!checkstr(inputStr.UTF8String))
        {
            self.outputSourceLabel.stringValue = @" 请输入合法的数字";
            return;
        }
        int number = [inputStr intValue];
        showb * showb = showbites(number);
        
        if (showb == NULL)
        {
            self.outputSourceLabel.stringValue = @"内存分配出错!!!";
            return;
        }
        if (_numberStr.length > 0)
        {
            [self emptyButton:nil];
        }
        
        for (int i = 0; i < showb->showb_t_bytesize; i++)
        {
            for (int j = 0; j < showb->showb_t_bitsize; j++)
            {
                
                [_numberStr appendFormat:@"%d",showb->showb_t_list[i][j]];
                
            }
            
            if (i < BYTESIZE -1)
            {
                [_numberStr appendFormat:@" "];
            }
            
        }
        printf("showb的指针是%p\n",showb);
        free(showb);
        showb = NULL;
        
        self.outputSourceLabel.stringValue = [NSString stringWithFormat:@"   %@=%@",inputStr,_numberStr];
        
    }
    else
    {
        self.outputSourceLabel.stringValue = @" 请输入,不然WX没有办法来计算!!!";
    }
    
}

- (IBAction)emptyButton:(id)sender {
    
    if (self.inputSourceTextfield.stringValue.length > 0)
    {
        self.inputSourceTextfield.stringValue = @"";
    }
    if (self.outputSourceLabel.stringValue.length > 0)
    {
        self.outputSourceLabel.stringValue = @"";
    }
    
    _numberStr = [NSMutableString stringWithFormat:@""];
    NSLog(@"emptyButton");
}

@end
