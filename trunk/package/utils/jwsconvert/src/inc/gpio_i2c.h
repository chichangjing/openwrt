
#ifndef _GPIO_I2C_H
#define _GPIO_I2C_H

#define GPIO_I2C_BASE   'H'
#define GPIO_I2C_READ   _IO(GPIO_I2C_BASE, 0x01)
#define GPIO_I2C_WRITE  _IO(GPIO_I2C_BASE, 0x02)

#define GPIO_I2C_READX  _IO(GPIO_I2C_BASE, 0x03)
#define GPIO_I2C_WRITEX _IO(GPIO_I2C_BASE, 0x04)
#define GPIO_I2C_READ_EXP   _IO(GPIO_I2C_BASE, 0x05)
#define GPIO_I2C_WRITE_EXP  _IO(GPIO_I2C_BASE, 0x06)
#define GPIO_I2C_READ_PAGE   _IO(GPIO_I2C_BASE, 0x07)
#define GPIO_I2C_WRITE_PAGE  _IO(GPIO_I2C_BASE, 0x08)

#define ACK 0
#define NACK 1

typedef unsigned char	byte;
typedef struct _i2c_page_data
{
    unsigned char data[32];
    unsigned int  size;
    unsigned int  read_size;
} i2c_page_data;

unsigned char gpio_i2c_read(unsigned char devaddress, unsigned char address);
void gpio_i2c_write(unsigned char devaddress, unsigned char address, unsigned char value);

#endif

