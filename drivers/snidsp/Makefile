# Makefile for snidsp

obj-$(CONFIG_SNI_DSP)		+= sni_dsp_ipcu_drv.o sni_dsp_ipcu_parts.o sni_dsp_ipcu_comm.o

ifeq ($(CONFIG_ARCH_MILBEAUT_KARINE), y)
	obj-$(CONFIG_SNI_DSP)		+= sni_dsp_drv_karine.o
else
	obj-$(CONFIG_SNI_DSP)		+= sni_dsp_drv_m20v.o
endif