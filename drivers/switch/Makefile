# Switch Class Driver
obj-$(CONFIG_SWITCH)		+= switch_class.o
obj-$(CONFIG_SWITCH_GPIO)	+= switch_gpio.o
obj-$(CONFIG_SWITCH_DTH)   += switch_dth.o
ifdef CONFIG_PANTECH_DEBUG
obj-y += pansw_class.o # 20110301 khlee_wifi for wifi rpc (pantech_debug.c) to read nv mac address 
endif
obj-$(CONFIG_SWITCH_SKY)	+= switch_sky.o