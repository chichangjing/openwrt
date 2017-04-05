/*
 * Copyright 2011-2015 Freescale Semiconductor, Inc.
 * Copyright 2011 Linaro Ltd.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/regulator/consumer.h>
#include <linux/irqchip/arm-gic.h>
#include "common.h"
#include "hardware.h"

#define GPC_CNTR		0x000
#define GPC_CNTR_PCIE_PHY_PDU_SHIFT	0x7
#define GPC_CNTR_PCIE_PHY_PDN_SHIFT	0x6
#define PGC_PCIE_PHY_CTRL		0x200
#define PGC_PCIE_PHY_PDN_EN		0x1
#define GPC_IMR1		0x008
#define GPC_PGC_MF_PDN		0x220
#define GPC_PGC_GPU_PDN		0x260
#define GPC_PGC_GPU_PUPSCR	0x264
#define GPC_PGC_GPU_PDNSCR	0x268
#define GPC_PGC_CPU_PDN		0x2a0
#define GPC_PGC_CPU_PUPSCR	0x2a4
#define GPC_PGC_CPU_PDNSCR	0x2a8
#define GPC_PGC_CPU_SW_SHIFT		0
#define GPC_PGC_CPU_SW_MASK		0x3f
#define GPC_PGC_CPU_SW2ISO_SHIFT	8
#define GPC_PGC_CPU_SW2ISO_MASK		0x3f
#define GPC_PGC_DISP_PGCR_OFFSET	0x240
#define GPC_PGC_DISP_PUPSCR_OFFSET	0x244
#define GPC_PGC_DISP_PDNSCR_OFFSET	0x248
#define GPC_PGC_DISP_SR_OFFSET		0x24c
#define GPC_M4_LPSR		0x2c
#define GPC_M4_LPSR_M4_SLEEPING_SHIFT	4
#define GPC_M4_LPSR_M4_SLEEPING_MASK	0x1
#define GPC_M4_LPSR_M4_SLEEP_HOLD_REQ_MASK	0x1
#define GPC_M4_LPSR_M4_SLEEP_HOLD_REQ_SHIFT	0
#define GPC_M4_LPSR_M4_SLEEP_HOLD_ACK_MASK	0x1
#define GPC_M4_LPSR_M4_SLEEP_HOLD_ACK_SHIFT	1

#define IMR_NUM			4

#define GPU_VPU_PUP_REQ		BIT(1)
#define GPU_VPU_PDN_REQ		BIT(0)

#define GPC_CLK_MAX		10
#define DEFAULT_IPG_RATE		66000000
#define GPC_PU_UP_DELAY_MARGIN		2

struct pu_domain {
	struct generic_pm_domain base;
	struct regulator *reg;
	struct clk *clk[GPC_CLK_MAX];
	int num_clks;
};

struct disp_domain {
	struct generic_pm_domain base;
	struct clk *clk[GPC_CLK_MAX];
	int num_clks;
};

static void __iomem *gpc_base;
static u32 gpc_mf_irqs[IMR_NUM];
static u32 gpc_wake_irqs[IMR_NUM];
static u32 gpc_saved_imrs[IMR_NUM];
static u32 gpc_mf_request_on[IMR_NUM];
static u32 bypass;
static DEFINE_SPINLOCK(gpc_lock);
static struct notifier_block nb_pcie;
static struct pu_domain imx6q_pu_domain;
static bool pu_on;	/* keep always on i.mx6qp */
static void imx6q_pu_pgc_power_off(struct pu_domain *pu, bool off);
static struct clk *ipg;

void imx_gpc_add_m4_wake_up_irq(u32 irq, bool enable)
{
	unsigned int idx = irq / 32 - 1;
	unsigned long flags;
	u32 mask;

	/* Sanity check for SPI irq */
	if (irq < 32)
		return;

	mask = 1 << irq % 32;
	spin_lock_irqsave(&gpc_lock, flags);
	gpc_wake_irqs[idx] = enable ? gpc_wake_irqs[idx] | mask :
		gpc_wake_irqs[idx] & ~mask;
	spin_unlock_irqrestore(&gpc_lock, flags);
}

void imx_gpc_hold_m4_in_sleep(void)
{
	int val;
	unsigned long timeout = jiffies + msecs_to_jiffies(500);

	/* wait M4 in wfi before asserting hold request */
	while (!imx_gpc_is_m4_sleeping())
		if (time_after(jiffies, timeout))
			pr_err("M4 is NOT in expected sleep!\n");

	val = readl_relaxed(gpc_base + GPC_M4_LPSR);
	val &= ~(GPC_M4_LPSR_M4_SLEEP_HOLD_REQ_MASK <<
		GPC_M4_LPSR_M4_SLEEP_HOLD_REQ_SHIFT);
	writel_relaxed(val, gpc_base + GPC_M4_LPSR);

	timeout = jiffies + msecs_to_jiffies(500);
	while (readl_relaxed(gpc_base + GPC_M4_LPSR)
		& (GPC_M4_LPSR_M4_SLEEP_HOLD_ACK_MASK <<
		GPC_M4_LPSR_M4_SLEEP_HOLD_ACK_SHIFT))
		if (time_after(jiffies, timeout))
			pr_err("Wait M4 hold ack timeout!\n");
}

void imx_gpc_release_m4_in_sleep(void)
{
	int val;

	val = readl_relaxed(gpc_base + GPC_M4_LPSR);
	val |= GPC_M4_LPSR_M4_SLEEP_HOLD_REQ_MASK <<
		GPC_M4_LPSR_M4_SLEEP_HOLD_REQ_SHIFT;
	writel_relaxed(val, gpc_base + GPC_M4_LPSR);
}

unsigned int imx_gpc_is_m4_sleeping(void)
{
	if (readl_relaxed(gpc_base + GPC_M4_LPSR) &
		(GPC_M4_LPSR_M4_SLEEPING_MASK <<
		GPC_M4_LPSR_M4_SLEEPING_SHIFT))
		return 1;

	return 0;
}

unsigned int imx_gpc_is_mf_mix_off(void)
{
	return readl_relaxed(gpc_base + GPC_PGC_MF_PDN);
}

static void imx_gpc_mf_mix_off(void)
{
	int i;

	for (i = 0; i < IMR_NUM; i++)
		if (((gpc_wake_irqs[i] | gpc_mf_request_on[i]) &
						gpc_mf_irqs[i]) != 0)
			return;

	pr_info("Turn off M/F mix!\n");
	/* turn off mega/fast mix */
	writel_relaxed(0x1, gpc_base + GPC_PGC_MF_PDN);
}

void imx_gpc_pre_suspend(bool arm_power_off)
{
	void __iomem *reg_imr1 = gpc_base + GPC_IMR1;
	int i;

	if (cpu_is_imx6q() && imx_get_soc_revision() == IMX_CHIP_REVISION_2_0)
		imx6q_pu_pgc_power_off(&imx6q_pu_domain, true);

	/* Tell GPC to power off ARM core when suspend */
	if ((cpu_is_imx6sx() || cpu_is_imx6ul()) && arm_power_off)
		imx_gpc_mf_mix_off();

	if (arm_power_off)
		writel_relaxed(0x1, gpc_base + GPC_PGC_CPU_PDN);

	for (i = 0; i < IMR_NUM; i++) {
		gpc_saved_imrs[i] = readl_relaxed(reg_imr1 + i * 4);
		writel_relaxed(~gpc_wake_irqs[i], reg_imr1 + i * 4);
	}
}

void imx_gpc_post_resume(void)
{
	void __iomem *reg_imr1 = gpc_base + GPC_IMR1;
	int i;

	if (cpu_is_imx6q() && imx_get_soc_revision() == IMX_CHIP_REVISION_2_0)
		imx6q_pu_pgc_power_off(&imx6q_pu_domain, false);

	/* Keep ARM core powered on for other low-power modes */
	writel_relaxed(0x0, gpc_base + GPC_PGC_CPU_PDN);
	/* Keep M/F mix powered on for other low-power modes */
	if (cpu_is_imx6sx() || cpu_is_imx6ul())
		writel_relaxed(0x0, gpc_base + GPC_PGC_MF_PDN);

	for (i = 0; i < IMR_NUM; i++)
		writel_relaxed(gpc_saved_imrs[i], reg_imr1 + i * 4);
}

static int imx_gpc_irq_set_wake(struct irq_data *d, unsigned int on)
{
	unsigned int idx = d->irq / 32 - 1;
	unsigned long flags;
	u32 mask;

	/* Sanity check for SPI irq */
	if (d->irq < 32)
		return -EINVAL;

	mask = 1 << d->irq % 32;
	spin_lock_irqsave(&gpc_lock, flags);
	gpc_wake_irqs[idx] = on ? gpc_wake_irqs[idx] | mask :
				  gpc_wake_irqs[idx] & ~mask;
	spin_unlock_irqrestore(&gpc_lock, flags);

	return 0;
}

void imx_gpc_mask_all(void)
{
	void __iomem *reg_imr1 = gpc_base + GPC_IMR1;
	int i;

	for (i = 0; i < IMR_NUM; i++) {
		gpc_saved_imrs[i] = readl_relaxed(reg_imr1 + i * 4);
		writel_relaxed(~0, reg_imr1 + i * 4);
	}

}

void imx_gpc_restore_all(void)
{
	void __iomem *reg_imr1 = gpc_base + GPC_IMR1;
	int i;

	for (i = 0; i < IMR_NUM; i++)
		writel_relaxed(gpc_saved_imrs[i], reg_imr1 + i * 4);
}

void imx_gpc_irq_unmask(struct irq_data *d)
{
	void __iomem *reg;
	u32 val;

	/* Sanity check for SPI irq */
	if (d->irq < 32)
		return;

	reg = gpc_base + GPC_IMR1 + (d->irq / 32 - 1) * 4;
	val = readl_relaxed(reg);
	val &= ~(1 << d->irq % 32);
	writel_relaxed(val, reg);
}

void imx_gpc_irq_mask(struct irq_data *d)
{
	void __iomem *reg;
	u32 val;

	/* Sanity check for SPI irq */
	if (d->irq < 32)
		return;

	reg = gpc_base + GPC_IMR1 + (d->irq / 32 - 1) * 4;
	val = readl_relaxed(reg);
	val |= 1 << (d->irq % 32);
	writel_relaxed(val, reg);
}

static int imx_pcie_regulator_notify(struct notifier_block *nb,
					unsigned long event,
					void *ignored)
{
	u32 value = readl_relaxed(gpc_base + GPC_CNTR);

	switch (event) {
	case REGULATOR_EVENT_PRE_ENABLE:
		value |= 1 << GPC_CNTR_PCIE_PHY_PDU_SHIFT;
		writel_relaxed(value, gpc_base + GPC_CNTR);
		break;
	case REGULATOR_EVENT_PRE_DISABLE:
		value |= 1 << GPC_CNTR_PCIE_PHY_PDN_SHIFT;
		writel_relaxed(value, gpc_base + GPC_CNTR);
		writel_relaxed(PGC_PCIE_PHY_PDN_EN,
				gpc_base + PGC_PCIE_PHY_CTRL);
		break;
	default:
		break;
	}

	return NOTIFY_OK;
}

int imx_gpc_mf_power_on(unsigned int irq, unsigned int on)
{
	unsigned int idx = irq / 32 - 1;
	unsigned long flags;
	u32 mask;

	mask = 1 << (irq % 32);
	spin_lock_irqsave(&gpc_lock, flags);
	gpc_mf_request_on[idx] = on ? gpc_mf_request_on[idx] | mask :
				  gpc_mf_request_on[idx] & ~mask;
	spin_unlock_irqrestore(&gpc_lock, flags);

	return 0;
}

int imx_gpc_mf_request_on(unsigned int irq, unsigned int on)
{
	if (cpu_is_imx6sx() || cpu_is_imx6ul())
		return imx_gpc_mf_power_on(irq, on);
	else if (cpu_is_imx7d())
		return imx_gpcv2_mf_power_on(irq, on);
	else
		return 0;
}
EXPORT_SYMBOL_GPL(imx_gpc_mf_request_on);

void __init imx_gpc_init(void)
{
	struct device_node *np;
	int i;
	u32 val;
	u32 cpu_pupscr_sw2iso, cpu_pupscr_sw;
	u32 cpu_pdnscr_iso2sw, cpu_pdnscr_iso;

	np = of_find_compatible_node(NULL, NULL, "fsl,imx6q-gpc");
	gpc_base = of_iomap(np, 0);
	WARN_ON(!gpc_base);

	/* Initially mask all interrupts */
	for (i = 0; i < IMR_NUM; i++)
		writel_relaxed(~0, gpc_base + GPC_IMR1 + i * 4);

	/* Read supported wakeup source in M/F domain */
	if (cpu_is_imx6sx() || cpu_is_imx6ul()) {
		of_property_read_u32_index(np, "fsl,mf-mix-wakeup-irq", 0,
			&gpc_mf_irqs[0]);
		of_property_read_u32_index(np, "fsl,mf-mix-wakeup-irq", 1,
			&gpc_mf_irqs[1]);
		of_property_read_u32_index(np, "fsl,mf-mix-wakeup-irq", 2,
			&gpc_mf_irqs[2]);
		of_property_read_u32_index(np, "fsl,mf-mix-wakeup-irq", 3,
			&gpc_mf_irqs[3]);
		if (!(gpc_mf_irqs[0] | gpc_mf_irqs[1] |
			gpc_mf_irqs[2] | gpc_mf_irqs[3]))
			pr_info("No wakeup source in Mega/Fast domain found!\n");
	}

	/* Register GPC as the secondary interrupt controller behind GIC */
	gic_arch_extn.irq_mask = imx_gpc_irq_mask;
	gic_arch_extn.irq_unmask = imx_gpc_irq_unmask;
	gic_arch_extn.irq_set_wake = imx_gpc_irq_set_wake;

	/*
	 * If there are CPU isolation timing settings in dts,
	 * update them according to dts, otherwise, keep them
	 * with default value in registers.
	 */
	cpu_pupscr_sw2iso = cpu_pupscr_sw =
		cpu_pdnscr_iso2sw = cpu_pdnscr_iso = 0;

	/* Read CPU isolation setting for GPC */
	of_property_read_u32(np, "fsl,cpu_pupscr_sw2iso", &cpu_pupscr_sw2iso);
	of_property_read_u32(np, "fsl,cpu_pupscr_sw", &cpu_pupscr_sw);
	of_property_read_u32(np, "fsl,cpu_pdnscr_iso2sw", &cpu_pdnscr_iso2sw);
	of_property_read_u32(np, "fsl,cpu_pdnscr_iso", &cpu_pdnscr_iso);

	/* Return if no property found in dtb */
	if ((cpu_pupscr_sw2iso | cpu_pupscr_sw
		| cpu_pdnscr_iso2sw | cpu_pdnscr_iso) == 0)
		return;

	/* Update CPU PUPSCR timing if it is defined in dts */
	val = readl_relaxed(gpc_base + GPC_PGC_CPU_PUPSCR);
	val &= ~(GPC_PGC_CPU_SW2ISO_MASK << GPC_PGC_CPU_SW2ISO_SHIFT);
	val &= ~(GPC_PGC_CPU_SW_MASK << GPC_PGC_CPU_SW_SHIFT);
	val |= cpu_pupscr_sw2iso << GPC_PGC_CPU_SW2ISO_SHIFT;
	val |= cpu_pupscr_sw << GPC_PGC_CPU_SW_SHIFT;
	writel_relaxed(val, gpc_base + GPC_PGC_CPU_PUPSCR);

	/* Update CPU PDNSCR timing if it is defined in dts */
	val = readl_relaxed(gpc_base + GPC_PGC_CPU_PDNSCR);
	val &= ~(GPC_PGC_CPU_SW2ISO_MASK << GPC_PGC_CPU_SW2ISO_SHIFT);
	val &= ~(GPC_PGC_CPU_SW_MASK << GPC_PGC_CPU_SW_SHIFT);
	val |= cpu_pdnscr_iso2sw << GPC_PGC_CPU_SW2ISO_SHIFT;
	val |= cpu_pdnscr_iso << GPC_PGC_CPU_SW_SHIFT;
	writel_relaxed(val, gpc_base + GPC_PGC_CPU_PDNSCR);
}

#ifdef CONFIG_PM
static void imx6q_pu_pgc_power_off(struct pu_domain *pu, bool off)
{
	if (off) {
		int iso, iso2sw;
		u32 val;

		/* Read ISO and ISO2SW power down delays */
		val = readl_relaxed(gpc_base + GPC_PGC_GPU_PDNSCR);
		iso = val & 0x3f;
		iso2sw = (val >> 8) & 0x3f;

		/* Wait ISO + ISO2SW IPG clock cycles */
		ndelay((iso + iso2sw) * 1000 / 66);

		/* Gate off PU domain when GPU/VPU when powered down */
		writel_relaxed(0x1, gpc_base + GPC_PGC_GPU_PDN);

		/* Request GPC to power down GPU/VPU */
		val = readl_relaxed(gpc_base + GPC_CNTR);
		val |= GPU_VPU_PDN_REQ;
		writel_relaxed(val, gpc_base + GPC_CNTR);

		while (readl_relaxed(gpc_base + GPC_CNTR) & GPU_VPU_PDN_REQ)
			;
	} else {
		int i, sw, sw2iso;
		u32 val, ipg_rate = clk_get_rate(ipg);

		/* Enable reset clocks for all devices in the PU domain */
		for (i = 0; i < pu->num_clks; i++)
			clk_prepare_enable(pu->clk[i]);

		/* Gate off PU domain when GPU/VPU when powered down */
		writel_relaxed(0x1, gpc_base + GPC_PGC_GPU_PDN);

		/* Read ISO and ISO2SW power down delays */
		val = readl_relaxed(gpc_base + GPC_PGC_GPU_PUPSCR);
		sw = val & 0x3f;
		sw2iso = (val >> 8) & 0x3f;

		/* Wait ISO + ISO2SW IPG clock cycles */
		ndelay((sw + sw2iso) * 1000 / 66);

		/* Request GPC to power up GPU/VPU */
		val = readl_relaxed(gpc_base + GPC_CNTR);
		val |= GPU_VPU_PUP_REQ;
		writel_relaxed(val, gpc_base + GPC_CNTR);

		while (readl_relaxed(gpc_base + GPC_CNTR) & GPU_VPU_PUP_REQ)
			;

		/* Wait power switch done */
		udelay(2 * DEFAULT_IPG_RATE / ipg_rate +
			GPC_PU_UP_DELAY_MARGIN);

		/* Disable reset clocks for all devices in the PU domain */
		for (i = 0; i < pu->num_clks; i++)
			clk_disable_unprepare(pu->clk[i]);
	}
}

static int imx6q_pm_pu_power_off(struct generic_pm_domain *genpd)
{
	struct pu_domain *pu = container_of(genpd, struct pu_domain, base);

	if (&imx6q_pu_domain == pu && pu_on && cpu_is_imx6q()
	    && imx_get_soc_revision() == IMX_CHIP_REVISION_2_0)
		return 0;

	imx6q_pu_pgc_power_off(pu, true);

	if (pu->reg)
		regulator_disable(pu->reg);

	return 0;
}

static int imx6q_pm_pu_power_on(struct generic_pm_domain *genpd)
{
	struct pu_domain *pu = container_of(genpd, struct pu_domain, base);
	int ret;

	if (cpu_is_imx6q() && imx_get_soc_revision() == IMX_CHIP_REVISION_2_0
		&& &imx6q_pu_domain == pu) {
		if (!pu_on)
			pu_on = true;
		else
			return 0;
	}

	if (pu->reg) {
		ret = regulator_enable(pu->reg);
		if (ret) {
			pr_err("%s: failed to enable regulator: %d\n", __func__, ret);
			return ret;
		}
	}

	imx6q_pu_pgc_power_off(pu, false);

	return 0;
}

static int imx_pm_dispmix_on(struct generic_pm_domain *genpd)
{
	struct disp_domain *disp = container_of(genpd, struct disp_domain, base);
	u32 val = readl_relaxed(gpc_base + GPC_CNTR);
	int i;
	u32 ipg_rate = clk_get_rate(ipg);

	if ((cpu_is_imx6sl() &&
		imx_get_soc_revision() >= IMX_CHIP_REVISION_1_2) || cpu_is_imx6sx()) {

		/* Enable reset clocks for all devices in the disp domain */
		for (i = 0; i < disp->num_clks; i++)
			clk_prepare_enable(disp->clk[i]);

		writel_relaxed(0x0, gpc_base + GPC_PGC_DISP_PGCR_OFFSET);
		writel_relaxed(0x20 | val, gpc_base + GPC_CNTR);
		while (readl_relaxed(gpc_base + GPC_CNTR) & 0x20)
			;

		writel_relaxed(0x1, gpc_base + GPC_PGC_DISP_SR_OFFSET);

		/* Wait power switch done */
		udelay(2 * DEFAULT_IPG_RATE / ipg_rate +
			GPC_PU_UP_DELAY_MARGIN);

		/* Disable reset clocks for all devices in the disp domain */
		for (i = 0; i < disp->num_clks; i++)
			clk_disable_unprepare(disp->clk[i]);
	}
	return 0;
}

static int imx_pm_dispmix_off(struct generic_pm_domain *genpd)
{
	struct disp_domain *disp = container_of(genpd, struct disp_domain, base);
	u32 val = readl_relaxed(gpc_base + GPC_CNTR);
	int i;

	if ((cpu_is_imx6sl() &&
		imx_get_soc_revision() >= IMX_CHIP_REVISION_1_2) || cpu_is_imx6sx()) {

		/* Enable reset clocks for all devices in the disp domain */
		for (i = 0; i < disp->num_clks; i++)
			clk_prepare_enable(disp->clk[i]);

		writel_relaxed(0xFFFFFFFF,
				gpc_base + GPC_PGC_DISP_PUPSCR_OFFSET);
		writel_relaxed(0xFFFFFFFF,
				gpc_base + GPC_PGC_DISP_PDNSCR_OFFSET);
		writel_relaxed(0x1, gpc_base + GPC_PGC_DISP_PGCR_OFFSET);
		writel_relaxed(0x10 | val, gpc_base + GPC_CNTR);
		while (readl_relaxed(gpc_base + GPC_CNTR) & 0x10)
			;

		/* Disable reset clocks for all devices in the disp domain */
		for (i = 0; i < disp->num_clks; i++)
			clk_disable_unprepare(disp->clk[i]);
	}
	return 0;
}

static struct generic_pm_domain imx6q_arm_domain = {
	.name = "ARM",
};

static struct pu_domain imx6q_pu_domain = {
	.base = {
		.name = "PU",
		.power_off = imx6q_pm_pu_power_off,
		.power_on = imx6q_pm_pu_power_on,
		.power_off_latency_ns = 25000,
		.power_on_latency_ns = 2000000,
	},
};

static struct disp_domain imx6s_display_domain = {
	.base = {
		.name = "DISPLAY",
		.power_off = imx_pm_dispmix_off,
		.power_on = imx_pm_dispmix_on,
	},
};

static struct generic_pm_domain *imx_gpc_domains[] = {
	&imx6q_arm_domain,
	&imx6q_pu_domain.base,
	&imx6s_display_domain.base,
};

static struct genpd_onecell_data imx_gpc_onecell_data = {
	.domains = imx_gpc_domains,
	.num_domains = ARRAY_SIZE(imx_gpc_domains),
};

static int imx_gpc_genpd_init(struct device *dev, struct regulator *pu_reg)
{
	struct clk *clk;
	bool is_off;
	int pu_clks, disp_clks, ipg_clks = 1;
	int i = 0, k = 0;

	/* No pu and display misc on i.mx6ul */
	if (cpu_is_imx6ul())
		return 0;

	imx6q_pu_domain.base.of_node = dev->of_node;
	imx6q_pu_domain.reg = pu_reg;

	imx6s_display_domain.base.of_node = dev->of_node;

	if ((cpu_is_imx6sl() &&
			imx_get_soc_revision() >= IMX_CHIP_REVISION_1_2)) {
		pu_clks = 2 ;
		disp_clks = 5;
	} else if (cpu_is_imx6sx()) {
		pu_clks = 1;
		disp_clks = 7;
	} else {
		pu_clks = 6;
		disp_clks = 0;
	}

	/* Get pu domain clks */
	for (i = 0; i < pu_clks ; i++) {
		clk = of_clk_get(dev->of_node, i);
		if (IS_ERR(clk))
			break;
		imx6q_pu_domain.clk[i] = clk;
	}
	imx6q_pu_domain.num_clks = i;

	ipg = of_clk_get(dev->of_node, pu_clks);

	/* Get disp domain clks */
	for (k = 0, i = pu_clks + ipg_clks; i < pu_clks + ipg_clks + disp_clks;
		i++, k++) {
		clk = of_clk_get(dev->of_node, i);
		if (IS_ERR(clk))
			break;
		imx6s_display_domain.clk[k] = clk;
	}
	imx6s_display_domain.num_clks = k;

	is_off = IS_ENABLED(CONFIG_PM_RUNTIME);
	if (is_off && !(cpu_is_imx6q() &&
		imx_get_soc_revision() == IMX_CHIP_REVISION_2_0))
		imx6q_pm_pu_power_off(&imx6q_pu_domain.base);

	pm_genpd_init(&imx6q_pu_domain.base, NULL, is_off);
	pm_genpd_init(&imx6s_display_domain.base, NULL, is_off);

	return __of_genpd_add_provider(dev->of_node, __of_genpd_xlate_onecell,
				     &imx_gpc_onecell_data);

}

#else
static inline int imx_gpc_genpd_init(struct device *dev, struct regulator *reg)
{
	return 0;
}
#endif /* CONFIG_PM */

static int imx_gpc_probe(struct platform_device *pdev)
{
	struct regulator *pu_reg;
	int ret;

	of_property_read_u32(pdev->dev.of_node, "fsl,ldo-bypass", &bypass);
	pu_reg = devm_regulator_get(&pdev->dev, "pu");
	if (!IS_ERR(pu_reg)) {
		/* The regulator is initially enabled */
		ret = regulator_enable(pu_reg);
		if (ret < 0) {
			dev_err(&pdev->dev, "failed to enable pu regulator: %d\n", ret);
			return ret;
		}
		/* We only bypass pu since arm and soc has been set in u-boot */
		if (bypass)
			regulator_allow_bypass(pu_reg, true);
	} else {
		pu_reg = NULL;
	}

	if (cpu_is_imx6sx()) {
		struct regulator *pcie_reg;

		pcie_reg = devm_regulator_get(&pdev->dev, "pcie-phy");
		if (IS_ERR(pcie_reg)) {
			ret = PTR_ERR(pcie_reg);
			dev_info(&pdev->dev, "pcie regulator not ready.\n");
			return ret;
		}
		nb_pcie.notifier_call = &imx_pcie_regulator_notify;

		ret = regulator_register_notifier(pcie_reg, &nb_pcie);
		if (ret) {
			dev_err(&pdev->dev,
				"pcie regulator notifier request failed\n");
			return ret;
		}
	}

	return imx_gpc_genpd_init(&pdev->dev, pu_reg);
}

static struct of_device_id imx_gpc_dt_ids[] = {
	{ .compatible = "fsl,imx6q-gpc" },
	{ .compatible = "fsl,imx6sl-gpc" },
	{ }
};

static struct platform_driver imx_gpc_driver = {
	.driver = {
		.name = "imx-gpc",
		.owner = THIS_MODULE,
		.of_match_table = imx_gpc_dt_ids,
	},
	.probe = imx_gpc_probe,
};

static int __init imx_pgc_init(void)
{
	return platform_driver_register(&imx_gpc_driver);
}
subsys_initcall(imx_pgc_init);
