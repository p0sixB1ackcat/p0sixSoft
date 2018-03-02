#include <stdio.h>
#include <stdlib.h>
#include "showbits.h"

#define REVERSEBYTE(v) ((v & 0xff000000) >> 24 \
                        | (v & 0x00ff0000) >> 8\
                        | (v & 0x0000ff00) << 8\
                        | (v & 0x000000ff) << 24)

char BigEndToSmaillEnd(void)
{
    int n = 1;
    char *p = (char *)(&n);
    return *p;
}

showb * showbites(int number)
{
  showb * showb_t_list = (showb *)malloc(sizeof(showb));
  
  if (showb_t_list == NULL) {
        return NULL;
  }
  if(BigEndToSmaillEnd())//小端，需要字节逆序
  {
      number = REVERSEBYTE(number);
  }
  unsigned char * p = (unsigned char *)&number;
    
  for(int i = 0;i < BYTESIZE;i++)
  {
    unsigned char number_c = p[i];
      
    for(int j = 0;j<BITSIZE;j++)
    {
        showb_t_list->showb_t_list[i][j] = number_c & 0x80 ? 1 : 0;
        
        number_c <<= 1;
        
    }
      
  printf(" ");
  }
  printf ("\n");
  
  showb_t_list->showb_t_bitsize = BITSIZE;
  showb_t_list->showb_t_bytesize = BYTESIZE;
    
  return showb_t_list;
  
}


