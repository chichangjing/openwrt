#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "strfunc.h"
#include "gpio_i2c.h"

int main(int argc , char* argv[])
{
	int fd = -1;
	int ret =0;
	unsigned int device_addr, reg_addr, reg_subaddr, reg_value, data;
		
	if((argc != 3)&&(argc != 4)&&(argc != 5))
    {
    	printf("usage: %s <device_addr> <data>. sample: %s 0xe2 0x01\n", argv[0], argv[0]);
    	printf("usage: %s <device_addr> <reg_addr> <data>. sample: %s 0x56 0x0 0x28\n", argv[0], argv[0]);
    	printf("usage: %s <device_addr> <reg_addr> <reg_subaddr> <data>. sample: %s 0xa0 0x0 0x0 0x01\n", argv[0], argv[0]);
        return -1;
    }
	
	fd = open("/dev/gpioi2c", 0);
    if(fd<0)
    {
    	printf("Open gpioi2c error!\n");
    	return -1;
    }
    if(argc == 3)
    { 
        if (StrToNumber(argv[1], &device_addr))
        {    	
            return 0;
        }
        if (StrToNumber(argv[2], &reg_value))
        {    
            return 0;
        }
        printf("write exp device_addr:0x%2x; reg_value:0x%2x;\n", device_addr, reg_value);
        data = (((device_addr&0xff)<<16) | (reg_value&0xff));
        ret = ioctl(fd, GPIO_I2C_WRITE_EXP, &data);
    }
    else if(argc == 4)
    {
        if (StrToNumber(argv[1], &device_addr))
        {    	
            return 0;
        }
        if (StrToNumber(argv[2], &reg_addr))
        {    
            return 0;
        }
        if (StrToNumber(argv[3], &reg_value))
        {    
            return 0;
        }
        printf("write device_addr:0x%2x; reg_addr:0x%2x; reg_value:0x%2x.\n", device_addr, reg_addr, reg_value);
        data = ((device_addr&0xff)<<24) | ((reg_addr&0xff)<<16) | (reg_value&0xffff);
        ret = ioctl(fd, GPIO_I2C_WRITE, &data);
    }
    else if(argc == 5)
    {
        if (StrToNumber(argv[1], &device_addr))
        {    	
            return 0;
        }
        if (StrToNumber(argv[2], &reg_addr))
        {    
            return 0;
        }
        if (StrToNumber(argv[3], &reg_subaddr))
        {    
            return 0;
        }
        if (StrToNumber(argv[4], &reg_value))
        {    
            return 0;
        }
        printf("write2 device_addr:0x%2x; reg_addr:0x%2x; reg_subaddr:0x%02x; reg_value:0x%2x.\n", device_addr, reg_addr, reg_subaddr, reg_value);
        data = ((device_addr&0xff)<<24) | ((reg_addr&0xff)<<16) | ((reg_subaddr&0xff)<<8) | (reg_value&0xff);
        ret = ioctl(fd, GPIO_I2C_WRITEX, &data);
    }
       
    return 0;
}

