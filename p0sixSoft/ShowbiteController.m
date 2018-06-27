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

@property (weak) IBOutlet NSTextField *outputWarringLabel;
@property (weak) IBOutlet NSTextField *outputBinField;
@property (weak) IBOutlet NSTextField *outputHexField;

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
    ret = 1;
    if(*p != '-')
        goto isnum;
    p++;
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
            self.outputWarringLabel.stringValue = @" 请输入合法的数字";
            return;
        }
        int number = [inputStr intValue];
        showb * showb = showbites(number);
        
        if (showb == NULL)
        {
            self.outputWarringLabel.stringValue = @"内存分配出错!!!";
            return;
        }
        
        [self emptyButton:nil];
        
        
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
        
        self.outputBinField.stringValue = [NSString stringWithFormat:@"%@",_numberStr];
        self.outputHexField.stringValue = [NSString stringWithFormat:@"0x%08X",number];
        
    }
    else
    {
        self.outputWarringLabel.stringValue = @" 请输入,不然WX没有办法来计算!!!";
    }
    
}

- (IBAction)emptyButton:(id)sender {
    
    if (self.outputWarringLabel.stringValue.length > 0)
    {
        self.outputWarringLabel.stringValue = @"";
    }
    if(self.inputSourceTextfield.stringValue && sender)//手动点击清空时
    {
        self.inputSourceTextfield.stringValue = @"";
    }
    if(self.outputBinField.stringValue.length)
    {
        self.outputBinField.stringValue = @"";
    }
    if(self.outputHexField.stringValue.length)
    {
        self.outputHexField.stringValue = @"";
    }
    _numberStr = [NSMutableString stringWithFormat:@""];
    NSLog(@"emptyButton");
}

@end
