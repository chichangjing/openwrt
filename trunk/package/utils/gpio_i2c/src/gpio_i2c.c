#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>

#include <linux/gpio.h>
//#include <mach/at91sam9g45.h>
//#include <mach/at91_pmc.h>
//#include <mach/at91_pio.h>
//#include <mach/gpio.h>
//#include <mach/io.h>
//#include <mach/at91_spi.h>

#include "gpio_i2c.h" 

#define DRV_NAME "gpio_i2c"
#define DRV_DESC "Custom GPIO-based I2C Driver"
#define DRV_VERSION "0.0.1"

spinlock_t  gpioi2c_lock;


/*
   change log :
   1. change the base address 
   2. change time_delay_us dly amplify 155/25

   hi3531 skt :  
   I2C_SCL  -- GPIO12_5 
   I2C_SDA  -- GPIO12_4
   GPIO12 base addr : 0x20210000
   */


#define SCL 114//AT91_PIN_PB11   //管脚
#define SDA 113//AT91_PIN_PB10

//#define HW_REG(reg)         *((volatile unsigned int *)(reg))
#define DELAY(us)           time_delay_us(us)
struct eeprom_data{
    unsigned char addr;
    unsigned char data;
};


/* 
 * I2C by GPIO simulated  clear 0 routine.
 *
 * @param whichline: GPIO control line
 *
 */
static void i2c_clr(unsigned char whichline)
{
    //unsigned char regvalue;

    if(whichline == SCL)
    {
        gpio_direction_output(SCL,0);		//gpio		
        return;
    }
    else if(whichline == SDA)
    {
        gpio_direction_output(SDA,0);		//gpio		
        return;
    }
    else if(whichline == (SDA|SCL))
    {
        gpio_direction_output(SCL,0);
        gpio_direction_output(SDA,0);		//gpio	
        return;
    }
    else
    {
        printk("Error input.\n");
        return;
    }
}

/* 
 * I2C by GPIO simulated  set 1 routine.
 *
 * @param whichline: GPIO control line
 *
 */
static void  i2c_set(unsigned char whichline)
{
    //unsigned char regvalue;

    if(whichline == SCL)
    {
        gpio_direction_output(SCL,1);		//gpio		
        return;
    }
    else if(whichline == SDA)
    {
        gpio_direction_output(SDA,1);		//gpio		
        return;
    }
    else if(whichline == (SDA|SCL))
    {
        gpio_direction_output(SCL,1);
        gpio_direction_output(SDA,1);		//gpio	
        return;
    }
    else
    {
        printk("Error input.\n");
        return;
    }
}

/*
 *  delays for a specified number of micro seconds rountine.
 *
 *  @param usec: number of micro seconds to pause for
 *
 */
void time_delay_us(unsigned int usec)
{
    volatile int i,j;

    for(i=0;i<usec * 5;i++)
    {
        for(j=0;j<100;j++)
        {;}
    }
}

/* 
 * I2C by GPIO simulated  read data routine.
 *
 * @return value: a bit for read 
 *
 */
static unsigned char i2c_data_read(void)
{
    unsigned char regvalue;

    gpio_direction_input(SDA);//将SDA配置为输入,不用内部上拉电阻
    DELAY(1);
    if(0 == gpio_get_value(SDA)){
       regvalue = 0;
       return regvalue;
    }else{
        regvalue = 1;
        return regvalue;
    }
}

/*
 * sends a start bit via I2C rountine.
 *
 */
static void i2c_start_bit(void)
{
    DELAY(1);
    i2c_set(SDA | SCL);
    DELAY(1);
    i2c_clr(SDA);
    DELAY(1);
}

/*
 * sends a stop bit via I2C rountine.
 *
 */
static void i2c_stop_bit(void)
{
    /* clock the ack */
    DELAY(1);
    i2c_set(SCL);
    DELAY(1); 
    DELAY(1); 
    i2c_clr(SCL);  

    /* actual stop bit */
    DELAY(1);
    i2c_clr(SDA);
    DELAY(1);
    i2c_set(SCL);
    DELAY(1);
    i2c_set(SDA);
    DELAY(1);
}

/*
 * sends a character over I2C rountine.
 *
 * @param  c: character to send
 *
 */
static void i2c_send_byte(unsigned char c)
{
    int i;
    local_irq_disable();
    // i2c_set(SDA);
    //    printk("%s  device_addr = %d\n",__FUNCTION__, c);			
    for (i=0; i<8; i++)
    {
        DELAY(1);
        i2c_clr(SCL);
        DELAY(1);

        if (c & (1<<(7-i)))
            i2c_set(SDA);
        else
            i2c_clr(SDA);

        DELAY(1);
        i2c_set(SCL);
        DELAY(1);
        i2c_clr(SCL);
    }
    DELAY(1);
    // i2c_set(SDA);
    local_irq_enable();
}

/*  receives a character from I2C rountine.
 *
 *  @return value: character received
 *
 */
static unsigned char i2c_receive_byte(void)
{
    int j=0;
    int i;

    local_irq_disable();//关中断
    for (i=0; i<8; i++)
    {
        DELAY(1);
        i2c_clr(SCL);
        DELAY(1);
        i2c_set(SCL);
        DELAY(1);

        if (i2c_data_read())
            j+=(1<<(7-i));

        DELAY(1);
        i2c_clr(SCL);
    }
    local_irq_enable();//使能中断

    return j;
}

/*  receives an acknowledge from I2C rountine.
 *
 *  @return value: 0--Ack received; 1--Nack received
 *          
 */
static int i2c_receive_ack(void)
{
    int timeout = 1000;

    gpio_direction_input(SDA);//将SDA配置为输入,不用内部上拉电阻

    i2c_clr(SCL);//SCL输出低电平
    DELAY(1);
    i2c_set(SCL);//SCL输出高电平
    DELAY(1);

    //获取ack值
    while(NACK == i2c_data_read())
    {
        timeout--;
        if(timeout <= 0)
        {
            return 1;
        }
    }

    i2c_clr(SCL);

    return 0;
}

/*
 * sends a ack bit via I2C rountine.
 *
 */
static void i2c_send_ack(void)
{
    DELAY(1);
    i2c_clr(SDA | SCL);
    DELAY(1);
    i2c_set(SCL);
    DELAY(1);
    i2c_clr(SCL);
    DELAY(1);
    i2c_set(SDA);
    DELAY(1);
}

EXPORT_SYMBOL(gpio_i2c_read);//导出的符号可以被其他模块使用
/*I2C读函数*/
unsigned char gpio_i2c_read(unsigned char devaddress, unsigned char address)
{
    int rxdata;
    //    printk("%s\n",__FUNCTION__);
    //      printk("%s  device_addr = %d\n",__FUNCTION__, devaddress);			
    //      printk("%s  reg_addr = %d\n",__FUNCTION__,address);			

    spin_lock(&gpioi2c_lock);//循环等待到自旋锁解锁(置为1),然后将自旋锁锁上(置为0)

    i2c_start_bit();//开始信号
    devaddress = (unsigned char)(devaddress << 1);
    devaddress = devaddress & ~0x01; //发设备地址 ＋ 写信号
    i2c_send_byte((unsigned char)(devaddress));
    i2c_receive_ack();//接收ack信号
    i2c_send_byte(address);//发送设备内部寄存器地址
    i2c_receive_ack();// 接收ack信号  
    i2c_start_bit();//重启开始信号
    devaddress = devaddress | 0x01; //发送设备地址 ＋ 读信号
    i2c_send_byte((unsigned char)(devaddress));
    i2c_receive_ack();//接收ack信号
    rxdata = i2c_receive_byte();//接收读的数据内容
    //i2c_send_ack();
    i2c_stop_bit();//结束信号

    spin_unlock(&gpioi2c_lock);//将自旋锁解锁(置为1)
    return rxdata;
}


EXPORT_SYMBOL(gpio_i2c_write);//导出声明
/*I2C写函数*/
void gpio_i2c_write(unsigned char devaddress, unsigned char address, unsigned char data)
{
    //   printk("%s\n",__FUNCTION__);
    spin_lock(&gpioi2c_lock);//加锁

    i2c_start_bit();//开始信号
    devaddress = (unsigned char)(devaddress << 1);
    devaddress = devaddress & ~0x01; //发设备地址 ＋ 写信号
    i2c_send_byte((unsigned char)(devaddress));
    i2c_receive_ack();//接收ack信号
    i2c_send_byte(address);//发送设备内部寄存器地址
    i2c_receive_ack();//接收ack信号
    i2c_send_byte(data);// 发送要写入的数据内容
    // i2c_receive_ack();//add by hyping for tw2815
    i2c_stop_bit();//结束信号

    spin_unlock(&gpioi2c_lock);//解锁
}

/*I2C读写功能*/
int gpioi2c_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    unsigned int val = 0;	
    unsigned int write_val = 0;	
    unsigned char device_addr, reg_addr;//reg_addr_high;
    short reg_val;
    int ret;
    //i2c_page_data page_data;

    //    struct eeprom_data eeprom;
    ret = copy_from_user(&val, (unsigned int*)arg,sizeof(val));
    //printk("%s\n",__FUNCTION__);
    /*   switch(cmd)
         {
         case GPIO_I2C_READ: //读
    //第一个参数：设备地址
    //第二个参数：要读的地址
    //返回值：地址里的数据    
    eeprom.data = gpio_i2c_read(0x69, eeprom.addr);
    copy_to_user((struct eeprom_data*)arg, &eeprom, sizeof(eeprom));
    break;
    case GPIO_I2C_WRITE: //写
    //第一个参数：设备地址
    //第二个参数：访问的内部地址
    //第三个参数：写入的数据
    gpio_i2c_write(0x69,eeprom.addr, eeprom.data);                  
    break;      

    default:
    return -1;
    }*/

    switch(cmd)
    {
        case GPIO_I2C_READ://读
            //	val = *(unsigned int *)arg;
            device_addr = (val&0xff000000)>>24;//设备地址
            reg_addr = (val&0xff0000)>>16;//要读的寄存器地址
            //          printk("device_addr = %d\n",device_addr);			
            //          printk("reg_addr = %d\n",reg_addr);			
            reg_val = gpio_i2c_read(device_addr, reg_addr);//返回地址里的数据
                      //printk("reg_val = %d\n",reg_val);			
            write_val = (val & 0xffff0000)|reg_val;
            DELAY(1);
            ret = copy_to_user((unsigned int*)arg, &write_val, sizeof(write_val));

            //   *(unsigned int *)arg = (val&0xffff0000)|reg_val;			
            break;

        case GPIO_I2C_WRITE://写
            val = *(unsigned int *)arg;
            device_addr = (val&0xff000000)>>24;//设备地址
            reg_addr = (val&0xff0000)>>16;//要访问的内部地址

            reg_val = val&0xffff;//写入的数据
            gpio_i2c_write(device_addr, reg_addr, reg_val);
            break;	
        default:
            return -1;
    }
    return 0;
}

int gpioi2c_open(struct inode * inode, struct file * file)
{
    return 0;
}
int gpioi2c_close(struct inode * inode, struct file * file)
{
    return 0;
}


static struct file_operations gpioi2c_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl      = gpioi2c_ioctl,
    .open       = gpioi2c_open,
    .release    = gpioi2c_close
};


static struct miscdevice gpioi2c_dev = {
    .minor		= MISC_DYNAMIC_MINOR,//让操作系统分配此设备号
    .name		= "gpioi2c",//设备文件名，内核帮你创建 /dev/gpioi2c
    .fops  = &gpioi2c_fops,//指针驱动分配的硬件操作集合
};

static int __init gpio_i2c_init(void)
{
    int ret;
    //unsigned int reg;
    printk("%s\n",__FUNCTION__);
    gpio_request(SDA, "SDA");
    gpio_request(SCL, "SCL");
    ret = misc_register(&gpioi2c_dev);//注册混杂设备
    if(0 != ret)
        return -1;

    //配置SCL与SDA管脚为输出口,并且输出高电平    
    i2c_set(SCL | SDA);
    spin_lock_init(&gpioi2c_lock);//初始化自旋锁,将自旋锁设置为1,表示有一个资源可用
    return 0;    
}

static void __exit gpio_i2c_exit(void)
{
    printk("%s\n",__FUNCTION__);
    gpio_free(SDA);
    gpio_free(SCL);
    misc_deregister(&gpioi2c_dev);//注销混杂设备
}


module_init(gpio_i2c_init);
module_exit(gpio_i2c_exit);

#ifdef MODULE
//#include <linux/compile.h>
#endif
//MODULE_INFO(build, UTS_VERSION);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
