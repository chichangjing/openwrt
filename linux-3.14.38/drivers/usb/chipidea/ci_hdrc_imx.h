/*
 * Copyright 2012-2015 Freescale Semiconductor, Inc.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef __DRIVER_USB_CHIPIDEA_CI_HDRC_IMX_H
#define __DRIVER_USB_CHIPIDEA_CI_HDRC_IMX_H

#include <linux/usb/otg.h>

struct imx_usbmisc_data {
	struct device *dev;
	int index;

	unsigned int disable_oc:1; /* over current detect disabled */
	unsigned int evdo:1; /* set external vbus divider option */
	unsigned int adp_prb_n2; /* OTG ADP probe charge time of n-2 */
	unsigned int adp_prb_n1; /* OTG ADP probe charge time of n-1 */
	unsigned int adp_prb_n; /* OTG ADP probe charge time of n */
	/*
	 * Specifies the delay between powering up the xtal 24MHz clock
	 * and release the clock to the digital logic inside the analog block
	 */
	unsigned int osc_clkgate_delay;
	struct regmap *anatop;
	enum usb_dr_mode available_role;
};

int imx_usbmisc_init(struct imx_usbmisc_data *);
int imx_usbmisc_init_post(struct imx_usbmisc_data *);
int imx_usbmisc_set_wakeup(struct imx_usbmisc_data *, bool);
int imx_usbmisc_power_lost_check(struct imx_usbmisc_data *);
int imx_usbmisc_hsic_set_connect(struct imx_usbmisc_data *);
int imx_usbmisc_hsic_set_clk(struct imx_usbmisc_data *, bool);
void imx_usbmisc_adp_probe_enable(struct imx_usbmisc_data *data);
void imx_usbmisc_adp_probe_start(struct imx_usbmisc_data *data);
void imx_usbmisc_adp_sense_enable(struct imx_usbmisc_data *data);
bool imx_usbmisc_adp_is_probe_int(struct imx_usbmisc_data *data);
bool imx_usbmisc_adp_is_sense_int(struct imx_usbmisc_data *data);
bool imx_usbmisc_adp_sense_connection(struct imx_usbmisc_data *data);
bool imx_usbmisc_adp_attach_event(struct imx_usbmisc_data *data);

#endif /* __DRIVER_USB_CHIPIDEA_CI_HDRC_IMX_H */
