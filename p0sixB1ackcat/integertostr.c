#include <stdio.h>
#include <string.h>
#include <integertostr.h>


int integertostr(int num,char * outputbuff,size_t * bufsize)
{
    int n,i = 0;
    char tmp[20];
    
    n = num % 10;//求个位
    while(n > 0){
        
        tmp[i++] = n+'0';//将每一位的数字加上字符0变成字符
        num = (num-n)/10;//右移
        n = num % 10;//继续求出个位
    }
    tmp[i] = n + '0';
    tmp[i+1] = '\0';
    
    size_t len = strlen(tmp);
    
    for(i = 0;i < len - 1; i++){
        
        outputbuff[i] = tmp[len-1-i];
        
    }
    
    outputbuff[i] = tmp[0];
    outputbuff[i+1] = '\0';
    *bufsize = len;
    
    return 0;
}



