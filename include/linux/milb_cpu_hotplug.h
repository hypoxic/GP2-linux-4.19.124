#ifndef __LINUX_MILB_CPU_HOTPLUG_H
#define __LINUX_MILB_CPU_HOTPLUG_H

#ifdef CONFIG_M10V_CPU_SWITCH
extern void milb_cpuhot_irq_handler(u32 cpu, u32 sgi_no);
#else
static inline void milb_cpuhot_irq_handler(u32 cpu, u32 sgi_no)
{
}
static inline void milb_notify_end_cpu_line(u32 end_line)
{
}
#endif
#endif
