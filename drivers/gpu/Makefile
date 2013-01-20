KBUILD_CFLAGS	:=$(KBUILD_CFLAGS:-msoft-float=-mfloat-abi=softfp -mfpu=neon -funroll-loops -mvectorize-with-neon-quad -ftree-vectorize -ftree-vectorizer-verbose=3)
KBUILD_CFLAGS	:=$(KBUILD_CFLAGS:-O2=-O3)

obj-y			+= drm/ vga/
obj-$(CONFIG_MSM_KGSL)	+= msm/
