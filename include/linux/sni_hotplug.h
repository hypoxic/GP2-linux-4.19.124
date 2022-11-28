#ifndef __LINUX_SNI_HOTPLUG_H
#define __LINUX_SNI_HOTPLUG_H

#ifdef CONFIG_KARINE_CPU_SWITCH
extern void sni_hotplug_irq_handler(u32 cpu, u32 sgi_no);
#else
static inline void sni_hotplug_irq_handler(u32 cpu, u32 sgi_no)
{
}
#endif
#endif
