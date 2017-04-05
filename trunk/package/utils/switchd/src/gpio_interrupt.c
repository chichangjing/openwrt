#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "switchd.h"
#include "joyware.h"
 
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

extern sw_error_t trunk_link_status_change(fal_port_t lport, fal_port_link_status_t link_status);
extern int ubus_poll_fdb_mac_create(void);


static struct ubus_context *_ctx;
static struct blob_buf b;


int lport2hport(unsigned int lport, unsigned int *hport)
{
	struct jws_port_info *Port_Info,portinfo[CONFIG_JWS_PORT_TOTAL_NR + 1];
	
	Port_Info = portinfo;
	jws_port_info_get(Port_Info);
	*hport = Port_Info[lport].physical_seq;
	
	return 0;	
}

static int hport2lport(unsigned int hport, unsigned int *lport)
{	
	struct jws_port_info *Port_Info,portinfo[CONFIG_JWS_PORT_TOTAL_NR + 1];
	char i;
	
	Port_Info = portinfo;
	jws_port_info_get(Port_Info);
	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
		if(hport == Port_Info[i].physical_seq) {
			*lport = Port_Info[i].logic_seq;
			break;
		}
	}
	
	return 0;
}

static int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		LOG_ERROR("gpio/export");
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
		LOG_ERROR("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
static int gpio_set_dir(unsigned int gpio, const char *dir)
{
	int fd;
	char buf[MAX_BUF];
 
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		LOG_ERROR("gpio/direction");
		return fd;
	}

	write(fd, dir, strlen(dir) + 1);
 
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_set_value
 ****************************************************************/
static int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];
 
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		LOG_ERROR("gpio/set-value");
		return fd;
	}
 
	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_get_value
 ****************************************************************/
static int gpio_get_value(unsigned int gpio, unsigned int *value)
{
	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR("gpio/get-value");
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

/****************************************************************
 * gpio_set_edge
 ****************************************************************/
static int gpio_set_edge(unsigned int gpio, const char *edge)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		LOG_ERROR("gpio/set-edge");
		return fd;
	}
 
	write(fd, edge, strlen(edge) + 1); 
	close(fd);
	return 0;
}

/****************************************************************
 * gpio_fd_open
 ****************************************************************/
static int gpio_fd_open(unsigned int gpio)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR("gpio/fd_open");
        return -1;
	}
	return fd;
}

static int gpio_fd_close(int fd)
{
	return close(fd);
}

static int gpio_int_sys(unsigned int gpio,const char *edge)
{
    int fd;
	int len;
	char buf[MAX_BUF];

    gpio_unexport(gpio);
	gpio_export(gpio);
	gpio_set_dir(gpio, "in");
	gpio_set_edge(gpio, edge);
	fd = gpio_fd_open(gpio);
    if(fd > 0)
    {
        len = read(fd, buf, MAX_BUF);
        if(len < 0)
        {
            LOG_ERROR("first read error\n");
            close(fd);
            return -1;
        }
        lseek(fd, 0, SEEK_SET);
    }

    return fd;
}

static void gpio_subscribe_cb(struct ubus_context *ctx, struct ubus_object *obj)
{
    LOG_NORMAL("someone subscribe this service,total subscribers = %d\n",obj->has_subscribers);
}

static const struct blobmsg_policy link_get_policy[] = {
    {.name = "port", .type = BLOBMSG_TYPE_INT32},
};

static int linkGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    return 0;
}

static const struct ubus_method link_methods[] = {
	UBUS_METHOD("link_get", linkGetHandler, link_get_policy),
};

static struct ubus_object_type link_object_type =
	UBUS_OBJECT_TYPE("link_event", link_methods);

static struct ubus_object link_object = {
    .name = "link_event",
    .type = &link_object_type,
	.methods = link_methods,
	.n_methods = ARRAY_SIZE(link_methods),
    .subscribe_cb = gpio_subscribe_cb,
};

static int link_notify(unsigned int lport,int link_status)
{
    blob_buf_init(&b, 0);
    blobmsg_add_u32(&b, "port", lport);
    blobmsg_add_u32(&b, "link", link_status);
    return ubus_notify(_ctx, &link_object, "link_ev", b.head, -1);
}

#define MV88E6097Sw_Global2_IntSource_reg	0x1C00
#define MV88E1112PHY_IntStatus_reg			0x13
#define MV88E1112PHY_PageAddress_reg		0x16
#define MV88E6097Sw_SerdesInt_bit			0x700		//bit8 for port8,bit9 for port9,bit10 for port10
#define MV88E1112PHY_LinkInt_Change_bit		0x400
/* 6097 interrupt callback function */
static void callback_switch(struct uloop_fd *ptr,uint32_t events)
{
	int ret;
	unsigned int hport,lport;
	unsigned int SerdesLink, PHYLink;
	fal_port_link_status_t link_status;

	char buf[MAX_BUF];
    int len = read(ptr->fd, buf, MAX_BUF);
    if(len < 0)
        LOG_ERROR("read error\n");

    //LOG_NORMAL(" GPIO interrupt occurred,len = %d\n",len);
    lseek(ptr->fd, 0, SEEK_SET);

	/* port security function, use switch interrupt */
	if(ubus_poll_fdb_mac_create())
    {
		LOG_ERROR("Connection to ubus failed\n");
    }
	
	ret = fal_reg_get(0,MV88E6097Sw_Global2_IntSource_reg,&SerdesLink,4);
	if(ret)
	{
        LOG_ERROR("ubus poll fal_reg_get error!\n");
    }
	/* As long as one serdes port's link change, get the change serdes port */
	if((SerdesLink & MV88E6097Sw_SerdesInt_bit) != 0)
	{
		for(hport=8; hport<11; hport++){
			if(((SerdesLink & MV88E6097Sw_SerdesInt_bit)>>hport) & 0x1)
			{
				hport2lport(hport,&lport);
				//LOG_NORMAL("SerdesLink = 0x%x, lport_%d\n",SerdesLink,lport);
				ret = fal_port_link_status_get(0, lport, &link_status);
				if(ret)
				{
		            LOG_ERROR("ubus poll fal_port_link_status_get error!\n");
		        }
				/* dynamic change trunk status */
				if(link_status == FAL_PORT_LINK_STATUS_UP){
					LOG_NORMAL("Interrupt lport_%d LINK UP\n",lport);

					trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_UP);
				}else if(link_status == FAL_PORT_LINK_STATUS_DOWN){
					LOG_NORMAL("Interrupt lport_%d LINK DOWN\n",lport);

					/* Flush link status up to down port's ATU */
					ret = fal_mac_flush_port(0, lport);
					if(ret)
					{
			            LOG_ERROR("ubus poll fal_mac_flush_port error!\n");
			        }
					trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_DOWN);
				}

				/* serdes port link status has been changed, to do... */
                if(link_notify(lport,link_status))
				{
		            LOG_ERROR("ubus notify port link status event fail!\n");
		        }
                //LOG_NORMAL("ubus notify port %d link status event OK\n",lport);
			}
		}	
	}

	for(hport=0; hport<8; hport++){
		ret = fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink);
		if(ret)
		{
		    LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
		}	

		if((PHYLink & MV88E1112PHY_LinkInt_Change_bit) != 0)
		{
			hport2lport(hport,&lport);
			//LOG_NORMAL("phy lport_%d change\n",lport);
			ret = fal_port_link_status_get(0, lport, &link_status);
			if(ret)
			{
	            LOG_ERROR("ubus poll fal_port_link_status_get error!\n");
	        }
			/* dynamic change trunk status */
			if(link_status == FAL_PORT_LINK_STATUS_UP){
				LOG_NORMAL("Interrupt lport_%d LINK UP\n",lport);
				
				trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_UP);
			}else if(link_status == FAL_PORT_LINK_STATUS_DOWN){
				LOG_NORMAL("Interrupt lport_%d LINK DOWN\n",lport);

				/* Flush link status up to down port's ATU */
				ret = fal_mac_flush_port(0, lport);
				if(ret)
				{
		            LOG_ERROR("ubus poll fal_mac_flush_port error!\n");
		        }
				trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_DOWN);
			}

			/* internal phy port link status has been changed, to do... */
            if(link_notify(lport,link_status))
            {
                LOG_ERROR("ubus notify port link status event fail!\n");
            }
		}
	}

}

#define MV88E1112PHY_Fiber_Page				0x1
#define MV88E1112PHY_Copper_Page			0x0

/* hport 10 interrupt callback function */
static void callback_phya(struct uloop_fd *ptr,uint32_t events)
{
	int ret;
	unsigned int hport,lport;
	unsigned int PHYLink;
	fal_port_link_status_t link_status;
	
	PortConfig portconfig;	
	
	char buf[MAX_BUF];
    int len = read(ptr->fd, buf, MAX_BUF);
    if(len < 0)
        LOG_ERROR("read error\n");

    //LOG_NORMAL(" GPIO interrupt occurred,len = %d\n",len);
    lseek(ptr->fd, 0, SEEK_SET);

	/* port security function, use PHY interrupt */
	if(ubus_poll_fdb_mac_create())
    {
		LOG_ERROR("Connection to ubus failed\n");
    }
	
	/* Hport 10 */
	hport = 10;	
	hport2lport(hport,&lport);

	/* Copper Register at 88E1112 Page 0 */
	if(fal_phy_reg_set(0,hport,MV88E1112PHY_PageAddress_reg,MV88E1112PHY_Copper_Page))
	{
		LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
	}
	/* clear interrupt status */
	if(fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink))
	{
		LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
	}
	//LOG_NORMAL("Copper    PHYLink = 0x%x\n",PHYLink);
	if((PHYLink & MV88E1112PHY_LinkInt_Change_bit) != 0)
	{
		ret = fal_port_link_status_get(0, lport, &link_status);
		if(ret)
		{
            LOG_ERROR("ubus poll fal_port_link_status_get error!\n");
        }
		/* dynamic change trunk status */
		if(link_status == FAL_PORT_LINK_STATUS_UP){
			LOG_NORMAL("Interrupt lport_%d LINK UP\n",lport);

			trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_UP);
		}else if(link_status == FAL_PORT_LINK_STATUS_DOWN){
			LOG_NORMAL("Interrupt lport_%d LINK DOWN\n",lport);

			/* Flush link status up to down port's ATU */
			ret = fal_mac_flush_port(0, lport);
			if(ret)
			{
	            LOG_ERROR("ubus poll fal_mac_flush_port error!\n");
	        }
			trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_DOWN);
		}
	}

	/* Fiber Register at 88E1112 Page 1 */
	if(fal_phy_reg_set(0,hport,MV88E1112PHY_PageAddress_reg,MV88E1112PHY_Fiber_Page))
	{
		LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
	}
	/* clear interrupt status */
	if(fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink))
	{
		LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
	}
	//LOG_NORMAL("Fiber    PHYLink = 0x%x\n",PHYLink);
	if((PHYLink & MV88E1112PHY_LinkInt_Change_bit) != 0)
	{
		ret = fal_port_link_status_get(0, lport, &link_status);
		if(ret)
		{
            LOG_ERROR("ubus poll fal_port_link_status_get error!\n");
        }
		/* dynamic change trunk status */
		if(link_status == FAL_PORT_LINK_STATUS_UP){
			LOG_NORMAL("Interrupt lport_%d LINK UP\n",lport);

			trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_UP);
		}else if(link_status == FAL_PORT_LINK_STATUS_DOWN){
			LOG_NORMAL("Interrupt lport_%d LINK DOWN\n",lport);

			/* Flush link status up to down port's ATU */
			ret = fal_mac_flush_port(0, lport);
			if(ret)
			{
	            LOG_ERROR("ubus poll fal_mac_flush_port error!\n");
	        }
			trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_DOWN);
		}
	}
}


/* hport 8 interrupt callback function */
static void callback_phyb(struct uloop_fd *ptr,uint32_t events)
{
	int ret;
	unsigned int hport,lport;
	unsigned int PHYLink;
	fal_port_link_status_t link_status;
	char id[]=CONFIG_JWS_SERIAL_NUMBER;
	
	PortConfig portconfig;

	char buf[MAX_BUF];
    int len = read(ptr->fd, buf, MAX_BUF);
    if(len < 0)
        LOG_ERROR("read error\n");

    //LOG_NORMAL(" GPIO interrupt occurred,len = %d\n",len);
    lseek(ptr->fd, 0, SEEK_SET);

	/* port security function, use PHY interrupt */
	if(ubus_poll_fdb_mac_create())
    {
		LOG_ERROR("Connection to ubus failed\n");
    }

	/* GE-Z20204A all board use hport 9 to trigger this callback function */
	if(!strcasecmp(id, "53-20-01-05-04-01-00-00"))
		hport = 9;
	else if(!strcasecmp(id, "53-20-01-06-04-02-00-00"))
		hport = 9;
	else if(!strcasecmp(id, "53-20-01-06-04-02-00-00"))
		hport = 9;
	else if(!strcasecmp(id, "53-20-01-0A-04-04-00-00"))
		hport = 9;
	/* Other bocard use hport 8 to trigger this callback function */
	else 
		hport = 8;
	hport2lport(hport,&lport);

	/* Copper Register at 88E1112 Page 0 */
	if(fal_phy_reg_set(0,hport,MV88E1112PHY_PageAddress_reg,MV88E1112PHY_Copper_Page))
	{
		LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
	}
	/* clear interrupt status */
	if(fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink))
	{
		LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
	}
	//LOG_NORMAL("Fiber    PHYLink = 0x%x\n",PHYLink);
	if((PHYLink & MV88E1112PHY_LinkInt_Change_bit) != 0)
	{
		ret = fal_port_link_status_get(0, lport, &link_status);
		if(ret)
		{
            LOG_ERROR("ubus poll fal_port_link_status_get error!\n");
        }
		/* dynamic change trunk status */
		if(link_status == FAL_PORT_LINK_STATUS_UP){
			LOG_NORMAL("Interrupt lport_%d LINK UP\n",lport);

			trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_UP);
		}else if(link_status == FAL_PORT_LINK_STATUS_DOWN){
			LOG_NORMAL("Interrupt lport_%d LINK DOWN\n",lport);

			/* Flush link status up to down port's ATU */
			ret = fal_mac_flush_port(0, lport);
			if(ret)
			{
	            LOG_ERROR("ubus poll fal_mac_flush_port error!\n");
	        }
			trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_DOWN);
		}

		/* internal phy port link status has been changed, to do... */
        if(link_notify(lport,link_status))
        {
            LOG_ERROR("ubus notify port link status event fail!\n");
        }
	}
}

#define PIN_SWITCH  51
#define PIN_PHY_A   70
#define PIN_PHY_B   42
struct uloop_fd *switch_ctl = NULL;
struct uloop_fd *phya_ctl = NULL;
struct uloop_fd *phyb_ctl = NULL;
int gpioInterruptInit(void)
{
    int switch_fd,phya_fd,phyb_fd;
	unsigned int SerdesLink, PHYLink;
	unsigned int hport;
	char id[]=CONFIG_JWS_SERIAL_NUMBER;

	/* switch interrput */
    switch_fd = gpio_int_sys(PIN_SWITCH,"falling");
    if(switch_fd <= 0)
    {
        LOG_ERROR("switch gpio interrupt create error\n");
        return -1;
    }

    switch_ctl = calloc(1,sizeof(struct uloop_fd));
    switch_ctl->fd  = switch_fd;
    switch_ctl->cb  = callback_switch;
    if(uloop_fd_add(switch_ctl,ULOOP_ERROR_CB | ULOOP_PRI) < 0)
        goto out_free;

	/* phy_a interrupt */
	phya_fd = gpio_int_sys(PIN_PHY_A,"falling");
    if(phya_fd <= 0)
    {
        LOG_ERROR("switch gpio interrupt create error\n");
        return -1;
    }

    phya_ctl = calloc(1,sizeof(struct uloop_fd));
    phya_ctl->fd  = phya_fd;
	phya_ctl->cb  = callback_phya;
    if(uloop_fd_add(phya_ctl,ULOOP_ERROR_CB | ULOOP_PRI) < 0)
        goto out_free;

	/* phy_b interrupt */
	phyb_fd = gpio_int_sys(PIN_PHY_B,"falling");
    if(phyb_fd <= 0)
    {
        LOG_ERROR("switch gpio interrupt create error\n");
        return -1;
    }

    phyb_ctl = calloc(1,sizeof(struct uloop_fd));
    phyb_ctl->fd  = phyb_fd;
	phyb_ctl->cb  = callback_phyb;
    if(uloop_fd_add(phyb_ctl,ULOOP_ERROR_CB | ULOOP_PRI) < 0)
        goto out_free;

	/* switch serdes interrupt status get */
	if(fal_reg_get(0,MV88E6097Sw_Global2_IntSource_reg,&SerdesLink,4))
	{
        LOG_ERROR("ubus poll fal_reg_get error!\n");
    }
	/* switch PHY interrupt status get */
	for(hport=0; hport<8; hport++){
		if(fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink))
		{
		    LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
		}	
	}

	/* External PHY interrupt function */
	/* GE-Z20204A all board use hport 9 to trigger this callback function */
	if((!strcasecmp(id, "53-20-01-05-04-01-00-00"))||(!strcasecmp(id, "53-20-01-06-04-02-00-00"))||(!strcasecmp(id, "53-20-01-09-04-03-00-00"))||(!strcasecmp(id, "53-20-01-0A-04-04-00-00")))
	{
		hport = 9;
		/* Copper Register at 88E1112 Page 0 */
		if(fal_phy_reg_set(0,hport,MV88E1112PHY_PageAddress_reg,MV88E1112PHY_Copper_Page))
		{
			LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
		}
		/* clear interrupt status */
		if(fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink))
		{
			LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
		}
	}
	/* Other bocard use hport 8 to trigger this callback function */
	else if((!strcasecmp(id, "53-20-01-0A-02-01-00-00"))||(!strcasecmp(id, "53-20-01-0C-06-01-00-00"))||(!strcasecmp(id, "53-20-01-08-06-01-00-00"))) 
	{
		hport = 8;
		/* Copper Register at 88E1112 Page 0 */
		if(fal_phy_reg_set(0,hport,MV88E1112PHY_PageAddress_reg,MV88E1112PHY_Copper_Page))
		{
			LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
		}
		/* clear interrupt status */
		if(fal_phy_reg_get(0,hport,MV88E1112PHY_IntStatus_reg,&PHYLink))
		{
			LOG_ERROR("ubus poll fal_phy_reg_get error!\n");
		}
	}
	
    return 0;

out_free:
    free(switch_ctl);
    LOG_ERROR("%m");
    close(switch_fd);
    return -1;
}

void ubus_init_gpio(struct ubus_context *ctx)
{
    int ret;

    _ctx = ctx;
	ret = ubus_add_object(ctx, &link_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [link_event]: %s\n", ubus_strerror(ret));
    }
}
