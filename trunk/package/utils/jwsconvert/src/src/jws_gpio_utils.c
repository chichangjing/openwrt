#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

 
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

static int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
 
	return 0;
}


static int gpio_unexport(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}


static int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
	int fd, len;
	char buf[MAX_BUF];
 
	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}

	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
 
	close(fd);
	return 0;
}


static int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd, len;
	char buf[MAX_BUF];
 
	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}
 
	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	return 0;
}


static int gpio_get_value(unsigned int gpio, unsigned int *value)
{
	int fd, len;
	char buf[MAX_BUF];
	char ch;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/get-value");
		return fd;
	}
 
	read(fd, &ch, 1);

	if (ch != '0') {
		*value = 1;
	} else {
		*value = 0;
	}
 
	close(fd);
	return 0;
}


static int gpio_set_edge(unsigned int gpio, char *edge)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		return fd;
	}
 
	write(fd, edge, strlen(edge) + 1); 
	close(fd);
	return 0;
}


static int gpio_fd_open(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio/fd_open");
	}
	return fd;
}


int gpio_fd_close(int fd)
{
	return close(fd);
}


int init_power_a(void)
{
    int power_pin_a = 117;
    int pina_fd = -1;

	gpio_export(power_pin_a);
	gpio_set_dir(power_pin_a, 0);
	//pina_fd = gpio_fd_open(power_pin_a);
    //return pina_fd;
    return 0;
}


int init_power_b(void)
{
    int power_pin_b = 120;
    int pinb_fd = -1;

	gpio_export(power_pin_b);
	gpio_set_dir(power_pin_b, 0);
	//pinb_fd = gpio_fd_open(power_pin_b);
    //return pinb_fd;
    
    return 0;
}


int init_alarm_k(void)
{
    int gpio_nr = 119;
    int gpio_fd = -1;

	gpio_export(gpio_nr);
	gpio_set_dir(gpio_nr, 1);
	//gpio_fd = gpio_fd_open(gpio_nr);
    //return gpio_fd;
    
    return 0;
}


int init_alarm_led(void)
{
    int gpio_nr = 75;
    int gpio_fd = -1;

	gpio_export(gpio_nr);
	gpio_set_dir(gpio_nr, 1);
	//gpio_fd = gpio_fd_open(gpio_nr);
    //return gpio_fd;
    
    return 0;
}


int init_ring_led(void)
{
    int gpio_nr = 121;
    int gpio_fd = -1;

	gpio_export(gpio_nr);
	gpio_set_dir(gpio_nr, 1);
	//gpio_fd = gpio_fd_open(gpio_nr);
    //return gpio_fd;
    
    return 0;
}

int init_run_led(void)
{
    int gpio_nr = 76;
    int gpio_fd = -1;

	gpio_export(gpio_nr);
	gpio_set_dir(gpio_nr, 1);
	//gpio_fd = gpio_fd_open(gpio_nr);
    //return gpio_fd;
    
    return 0;
}

int power_a_get(int *value)
{
    return gpio_get_value(117, value);
}


int power_b_get(int *value)
{
    return gpio_get_value(120, value);
}


int alarm_k_set(int value)
{
    return gpio_set_value(119, value);
}


int alarm_led_set(int value)
{
    return gpio_set_value(75, value);
}


int ring_led_set(int value)
{
    return gpio_set_value(121, value);
}

int run_led_set(int value)
{
    return gpio_set_value(76, value);
}