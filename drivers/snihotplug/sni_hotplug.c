/*
 * Copyright:	(C) 2020 Socionext
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * @file   sni_hotplug.c
 * @author
 * @date
 * @brief  Support for switch CPU to Linux from RTOS on Milbeaut Evb boards
 */
#include <linux/mutex.h>
#include <linux/of_address.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/irqreturn.h>
#include <linux/of_irq.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/sni_hotplug.h>
#include <linux/psci.h>

#define DEVICE_NAME "smp-cpuswitch"
#define D_SWITCH_CPU_OFF_LINE	(4)
//#define D_SWITCH_CPU_ON_LINE	(3)
#define D_RTOS_CPU_ID	(1)
#define D_LINUX_MAIN_CORE	(0)
#define D_RESET_CPU0	(0x00000001)
#define D_RESET_CPU1	(0x00000002)
#define D_RESET_CPU2	(0x00000004)
#define D_RESET_CPU012 (D_RESET_CPU0 | D_RESET_CPU1 | D_RESET_CPU2)
#define D_RESET_CPU012_MASK (D_RESET_CPU0 + D_RESET_CPU1 + D_RESET_CPU2)
#define D_RESET_ALL	(D_RESET_CPU012 | D_RESET_CPU012_MASK)
#define D_RESET_REG_OFFSET	(0x344)
#define D_HAVE_NEW_DATA_TO_READ	(0)
#define D_NO_NEW_DATA_TO_READ (1)

typedef enum {
	SNI_IO_RESET_TO_OFF_LINE = 0x10,
	SNI_IO_RESET_TO_ON_LINE,
	SNI_IO_NOTIFY_RTOS,
	SNI_IO_GET_CPU_STATE,
	SNI_IO_NOTIFY_LINUXMAIN_DEBUG,
} SNI_IO;

typedef struct {
	struct miscdevice miscdev;
	wait_queue_head_t	waitq;
	struct mutex mlock;
	int access_num;
	u32 read_flag;
	u32 sgi_no;
	void __iomem *cpu_rst_reg;
} T_SNI_HOTPLUG_INFO;
typedef union {
	unsigned int word;
	struct {
		unsigned int cpu_no		:4;
		unsigned int possible_status	:1;
		unsigned int present_status	:1;
		unsigned int online_status	:1;
		unsigned int active_status	:1;
		unsigned int			:24;
	} bit;
} U_SNI_CPU_STATE;
static T_SNI_HOTPLUG_INFO *dev_info;

/**
* Local static function
*/
static int set_reset_data(u32 switch_type,
		const T_SNI_HOTPLUG_INFO *dev_info)
{
	u32 reset_reg;

	/* Reset Assert CPU */
	reset_reg = readl(dev_info->cpu_rst_reg);
	reset_reg |= D_RESET_CPU2;

	pr_debug("reset_reg:%X\n", reset_reg);
	writel(reset_reg, dev_info->cpu_rst_reg);

	return 0;
}

/* .ioctl */
static long sni_hotplug_drv_ioctl(struct file *filp,
			       unsigned int cmd,
			       unsigned long arg)
{
	struct miscdevice *mdev = filp->private_data;
	T_SNI_HOTPLUG_INFO *dev_info =
			container_of(mdev, T_SNI_HOTPLUG_INFO, miscdev);
	U_SNI_CPU_STATE cpu_status;

	pr_debug("%s %s %d,cmd:%X\n",
			__FILE__, __func__, __LINE__, cmd);
	switch (cmd) {
	case SNI_IO_RESET_TO_OFF_LINE:
		set_reset_data(D_SWITCH_CPU_OFF_LINE, dev_info);
		break;
	case SNI_IO_NOTIFY_RTOS:
		arch_send_user_receive_rtos_ipi_mask(cpumask_of(D_RTOS_CPU_ID));
		break;
	case SNI_IO_GET_CPU_STATE:
		cpu_status.word = 0;
		if (copy_from_user(&cpu_status, (void __user *)arg,
			sizeof(U_SNI_CPU_STATE))) {
			pr_err("Parameter error cmd:%d\n", cmd);
			 return -EFAULT;
		}
		cpu_status.bit.possible_status =
			cpu_possible(cpu_status.bit.cpu_no);
		cpu_status.bit.present_status =
			cpu_present(cpu_status.bit.cpu_no);
		cpu_status.bit.online_status =
			cpu_online(cpu_status.bit.cpu_no);
		cpu_status.bit.active_status =
			cpu_active(cpu_status.bit.cpu_no);
		if (copy_to_user((void __user *)arg, &cpu_status,
			sizeof(U_SNI_CPU_STATE))) {
			pr_err("Parameter error cmd:%d\n", cmd);
			return -EFAULT;
		}
		break;
	case SNI_IO_NOTIFY_LINUXMAIN_DEBUG:
		arch_send_user_receive_rtos_ipi_mask(cpumask_of(D_LINUX_MAIN_CORE));
		break;
	default:
		/* Do nothing */
		pr_err("Unkonwn cmd:%d\n", cmd);
		break;
	}

	return 0;
}

/* .poll */
static u32 sni_hotplug_drv_poll(struct file *file, poll_table *wait)
{
	u32 mask = 0;
	struct miscdevice *mdev = file->private_data;
	T_SNI_HOTPLUG_INFO *dev_info =
			container_of(mdev, T_SNI_HOTPLUG_INFO, miscdev);

	pr_debug("%s %s %d\n", __FILE__, __func__, __LINE__);
	poll_wait(file, &dev_info->waitq, wait);
	if (!dev_info->read_flag)
		mask |= POLLIN | POLLRDNORM;

	pr_debug("%s %s %d,mask=%08x\n",
	__FILE__, __func__, __LINE__, mask);
	return mask;

}

/* .read */
static ssize_t sni_hotplug_drv_read(struct file *file,
				char *buff, size_t count, loff_t *pos)
{
	int ret = 4;
	u32 read_data;

	/* return value receive from rtos via SMC */
	read_data = psci_ops.sni_r(0, 0);

	if (copy_to_user(buff, &read_data, sizeof(read_data)))
		ret = -EFAULT;
	else
		/* Set the data had been read flag */
		dev_info->read_flag = D_NO_NEW_DATA_TO_READ;

	return ret;
}

/* .write */
static ssize_t sni_hotplug_drv_write(struct file *file,
		const char __user *buffer,
		size_t count, loff_t *ppos)
{
	int ret = 4;
	u32 write_data;

	if (copy_from_user(&write_data, buffer,  sizeof(write_data)))
		ret = -EFAULT;
	else
		/* arg1 send to rtos via SMC */
		psci_ops.sni_w(write_data, 0);
	return ret;
}

/* .open */
static int sni_hotplug_drv_open(struct inode *inode,
	struct file *file)
{
	struct miscdevice *mdev = file->private_data;
	T_SNI_HOTPLUG_INFO *dev_info =
			container_of(mdev, T_SNI_HOTPLUG_INFO, miscdev);
	int ret = 0;

	mutex_lock(&dev_info->mlock);
	if (dev_info->access_num > 0) {
		ret = -EBUSY;
	} else {
		ret = nonseekable_open(inode, file);
		if (ret == 0)
			dev_info->access_num++;
	}
	mutex_unlock(&dev_info->mlock);
	/* Set the data had been read flag as initialization */
	dev_info->read_flag = D_NO_NEW_DATA_TO_READ;
	return ret;
}

/* .release */
static int sni_hotplug_drv_release(struct inode *inode,
		struct file *file)
{
	struct miscdevice *mdev = file->private_data;
	T_SNI_HOTPLUG_INFO *dev_info =
			container_of(mdev, T_SNI_HOTPLUG_INFO, miscdev);
	int ret = 0;

	mutex_lock(&dev_info->mlock);
	if (dev_info->access_num > 0)
		dev_info->access_num--;
	else
		ret = -EPERM;

	mutex_unlock(&dev_info->mlock);
	return ret;
}
/* IRQ Handler for SGI from RTOS */
void sni_hotplug_irq_handler(u32 cpu, u32 sgi_no)
{
	if (dev_info && dev_info->sgi_no == sgi_no) {
		/* Set the new data to be read flag */
		dev_info->read_flag = D_HAVE_NEW_DATA_TO_READ;
		wake_up_interruptible(&dev_info->waitq);
	} else {
		pr_warn("CPU%u: Unknown IPI message 0x%x\n",
			cpu, sgi_no);
	}
}
/* The file operations for the cpu plug information */
const struct file_operations sni_hotplug_drv_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = sni_hotplug_drv_ioctl,
	.open		= sni_hotplug_drv_open,
	.release	= sni_hotplug_drv_release,
	.llseek		= no_llseek,
	.read		= sni_hotplug_drv_read,
	.write		= sni_hotplug_drv_write,
	.poll		= sni_hotplug_drv_poll,
};

static int sni_hotplug_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;

	dev_info = devm_kzalloc(dev, sizeof(*dev_info), GFP_KERNEL);

	if (!dev_info) {
		ret = -ENOMEM;
		goto error;
	}
	memset(dev_info, 0, sizeof(*dev_info));

	dev_info->cpu_rst_reg = of_iomap(node, 0);
	if (!dev_info->cpu_rst_reg) {
		pr_err("cpu_rst_reg error\n");
		ret = -ENODEV;
		goto error;
	}

	if (of_property_read_u32(node, "sgi_no", &dev_info->sgi_no)) {
		pr_err("sgi_no error\n");
		ret = -ENODEV;
		goto error;
	}
	init_waitqueue_head(&dev_info->waitq);
	mutex_init(&dev_info->mlock);

	dev_info->miscdev.name = DEVICE_NAME;
	dev_info->miscdev.minor = MISC_DYNAMIC_MINOR;
	dev_info->miscdev.fops = &sni_hotplug_drv_fops;
	platform_set_drvdata(pdev, dev_info);


	return misc_register(&dev_info->miscdev);

 error:
	return ret;
}
static int sni_hotplug_remove(struct platform_device *pdev)
{
	pr_info("Goodby...\n");
	return 0;
}
static const struct of_device_id sni_hotplug_of_match[] = {
		{.compatible = "socionext,"DEVICE_NAME },
		{}
};
MODULE_DEVICE_TABLE(of, sni_hotplug_of_match);
static struct platform_driver sni_hotplug_driver = {
	.probe = sni_hotplug_probe,
	.remove = sni_hotplug_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = DEVICE_NAME,
		.of_match_table = of_match_ptr(sni_hotplug_of_match),
	},
};
module_platform_driver(sni_hotplug_driver);
MODULE_DESCRIPTION("SNI hotplug driver");
MODULE_AUTHOR("sasaki.masayuki <sasaki.masayuki_s@aa.socionext.com>");
MODULE_LICENSE("GPL");


