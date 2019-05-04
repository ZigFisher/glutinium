include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=himpp
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=HISILICON COPYING

include $(INCLUDE_DIR)/package.mk

SOURCE_DIR:=$(TOPDIR)/$(SOURCE)

define Package/$(PKG_NAME)/Default
	SECTION:=openipc
	CATEGORY:=OpenIPC
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	URL:=http://zftlab.org
	TITLE:=Hisilicon SDK
	DEPENDS:=@(TARGET_hisilicon_armv5tej_hi3516cv100||TARGET_hisilicon_armv5tej_hi3518av100||TARGET_hisilicon_armv5tej_hi3518cv100||TARGET_hisilicon_armv5tej_hi3518ev100) +libstdcpp
endef

define Package/$(PKG_NAME)-v1
	$(call Package/$(PKG_NAME)/Default)
	TITLE += (v1)
#	VARIANT:=v1
endef

define Package/$(PKG_NAME)-v1-base
	$(call Package/$(PKG_NAME)-v1)
	TITLE+= - base package
endef

define Package/$(PKG_NAME)-v1-base/config
	source "$(SOURCE)/Config.in"
endef

define Package/$(PKG_NAME)-v1-extdrv
	$(call Package/$(PKG_NAME)-v1)
	TITLE:= v1 extdrv
	SUBMENU:=ExtDrv
	DEPENDS:= @(PACKAGE_himpp-v1-base)
endef

define Package/$(PKG_NAME)-v1-sensor
	$(call Package/$(PKG_NAME)-v1)
	SUBMENU:=Sensors
	DEPENDS:= @(PACKAGE_himpp-v1-base) 
endef

define Package/$(PKG_NAME)-v1-samples
	$(call Package/$(PKG_NAME)-v1)
	SUBMENU:=Samples
	DEPENDS:= @(PACKAGE_himpp-v1-base) +libpthread
endef

include Sensors.mk
include Extdrv.mk
include Samples.mk

HIMPP_HI3518V100_VERSION = $(call qstrip,$(CONFIG_PACKAGE_HIMPP_HI3518V100_SDK_VERSION))
HIMPP_HI3518V100_SOURCE = hi3518v100-mpp2-$(HIMPP_HI3518V100_VERSION).tgz

SENSOR_TYPE = $(call qstrip,$(CONFIG_PACKAGE_SAMPLES_SNS_TYPE))
HIMPP_PREFIX = $(call qstrip,$(CONFIG_PACKAGE_HIMPP_V1_PREFIX))

HIMPP_HI3518V100_MAKE_OPTS = ARCH=arm
HIMPP_HI3518V100_MAKE_OPTS += HIARCH=hi3518
HIMPP_HI3518V100_MAKE_OPTS += LIBC=uclibc
HIMPP_HI3518V100_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3518V100_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3518V100_MAKE_OPTS += LINUX_ROOT=$(LINUX_DIR)
HIMPP_HI3518V100_MAKE_OPTS += MPP_PATH=$(PKG_BUILD_DIR)
HIMPP_HI3518V100_MAKE_OPTS += SENSOR_TYPE=$(SENSOR_TYPE)


define Package/$(PKG_NAME)/description
	Hisilicon SDK kernel modules, drivers, libs and tools.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(TAR) -zxf $(TOPDIR)/$(SOURCE)/himpp-hi3518v100/$(HIMPP_HI3518V100_SOURCE) \
		--strip-components=2 -C $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

define himpp_build
	( cd $(PKG_BUILD_DIR)/$(1) && $(MAKE) $(TARGET_CONFIGURE_OPTS) $(HIMPP_HI3518V100_MAKE_OPTS) ) || exit 1;
endef

define copy_stg_snsdrv
	( $(CP) $(PKG_BUILD_DIR)/component/isp2/lib/* $(PKG_BUILD_DIR)/lib/ ) || exit 1;
endef

define stg_snsdrv
	HIMPP_HI3518V100_BUILD_CMDS += \
		$$(call copy_stg_snsdrv,)
endef

define HIMPP_HI3518V100_BUILD_CMDS
	( cd $(PKG_BUILD_DIR)/lib; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive \
	  || exit 1; \
	done; \
	);
endef

###############################################################################
# build snsdrv
###############################################################################

define add_snsdrv
	HIMPP_HI3518V100_BUILD_CMDS += \
		$$(call himpp_build,component/isp2/sensor/$$(call qstrip,$(2)))
endef

###############################################################################
# build extdrv
###############################################################################

define add_extdrv
	HIMPP_HI3518V100_BUILD_CMDS += $$(call himpp_build,extdrv/$(2))
	EXTDRV_TO_INSTALL += $$(patsubst %,extdrv/$(2)/%,$(3))
	PROGRAM_TO_INSTALL += $$(patsubst %,extdrv/$(2)/%,$(4))
endef

###############################################################################
# build samples
###############################################################################

define add_sample
	HIMPP_HI3518V100_BUILD_CMDS += $$(call himpp_build,sample/$(2))
	PROGRAM_TO_INSTALL += $$(patsubst %,sample/$(2)/%,$(3))
endef

define Build/Compile
	echo -e "\e[1;34m \nBuild/compile $(PKG_NAME)\n $<\e[0m"
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_APTINA_9M034),$(eval $(call add_snsdrv,APTINA_9M034,aptina_9m034, 9m034)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_APTINA_AR0130),$(eval $(call add_snsdrv,APTINA_AR0130,aptina_ar0130,ar0130_720p)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_APTINA_AR0330),$(eval $(call add_snsdrv,APTINA_AR0330,aptina_ar0330,ar0330_1080p)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_APTINA_AR0331),$(eval $(call add_snsdrv,APTINA_AR0331,aptina_ar0331,ar0331_1080p)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_APTINA_MT9P006),$(eval $(call add_snsdrv,APTINA_MT9P006,aptina_mt9p006,mt9p006)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_GALAXYCORE_GC1004),$(eval $(call add_snsdrv,GALAXYCORE_GC1004,galaxycore_gc1004,gc1004)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_GALAXYCORE_GC1014),$(eval $(call add_snsdrv,GALAXYCORE_GC1014,galaxycore_gc1014,gc1014)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_HIMAX_1375),$(eval $(call add_snsdrv,HIMAX_1375,himax_1375,himax1375)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_ONMI_OV9712),$(eval $(call add_snsdrv,ONMI_OV9712,ov_9712,ov9712)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_ONMI_OV9712),$(eval $(call add_snsdrv,ONMI_OV9712,ov_9712+,ov9712_plus)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_PANASONIC_MN34031),$(eval $(call add_snsdrv,PANASONIC_MN34031,pana34031,mn34031_720p)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_PANASONIC_MN34041),$(eval $(call add_snsdrv,PANASONIC_MN34041,pana34041,mn34041)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_PIXELPLUS_3100K),$(eval $(call add_snsdrv,PIXELPLUS_3100K,pixelplus_3100k,po3100k)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_SOI_H22),$(eval $(call add_snsdrv,SOI_H22,soi_h22,soih22)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_SONY_ICX692),$(eval $(call add_snsdrv,SONY_ICX692,sony_icx692,icx692)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_SONY_IMX104),$(eval $(call add_snsdrv,SONY_IMX104,sony_imx104,imx104)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_SONY_IMX122),$(eval $(call add_snsdrv,SONY_IMX122,sony_imx122,imx122)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_SONY_IMX138),$(eval $(call add_snsdrv,SONY_IMX138,sony_imx138,imx138)))
	$(if $(CONFIG_PACKAGE_HIMPP_SNSDRV_SONY_IMX236),$(eval $(call add_snsdrv,SONY_IMX236,sony_imx236,imx236)))
	$(eval $(call stg_snsdrv,))

	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_GPIO_I2C),$(eval $(call add_extdrv,GPIO_I2C,gpio-i2c,gpioi2c.ko,i2c_read i2c_write)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_GPIO_I2C_EX),$(eval $(call add_extdrv,GPIO_I2C_EX,gpio-i2c-ex,gpioi2c_ex.ko,i2c_read_ex i2c_write_ex)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_HI_I2C),$(eval $(call add_extdrv,HI_I2C,hi_i2c,hi_i2c.ko,i2c_read i2c_write)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_PWM),$(eval $(call add_extdrv,PWM,pwm,pwm.ko,pwm_write)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_SSP_AD9020),$(eval $(call add_extdrv,SSP_AD9020,ssp-ad9020,ssp_ad9020.ko,ssp_read ssp_write)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_SSP_PANA),$(eval $(call add_extdrv,SSP_PANA,ssp-pana,ssp_pana.ko,ssp_read ssp_write)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_SSP_SONY),$(eval $(call add_extdrv,SSP_SONY,ssp-sony,ssp_sony.ko,ssp_read ssp_write)))
	$(if $(CONFIG_PACKAGE_HIMPP_EXTDRV_TW2865),$(eval $(call add_extdrv,TW2865,tw2865,tw2865.ko,)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_AUDIO),$(eval $(call add_sample,AUDIO,audio,sample_audio)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_HIFB),$(eval $(call add_sample,HIFB,hifb,sample_hifb)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_IQ),$(eval $(call add_sample,IQ,iq,sample_iq)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_IVE),$(eval $(call add_sample,IVE,ive,sample_ive_canny sample_ive_detect \
		sample_ive_FPN sample_ive_sobel_with_cached_mem \
		sample_ive_test_memory)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_REGION),$(eval $(call add_sample,REGION,region,sample_region)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_TDE),$(eval $(call add_sample,TDE,tde,sample_tde)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_VDA),$(eval $(call add_sample,VDA,vda,sample_vda)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_VENC),$(eval $(call add_sample,VENC,venc,sample_venc)))
	$(if $(CONFIG_PACKAGE_HIMPP_SAMPLES_VIO),$(eval $(call add_sample,VIO,vio,sample_vio)))
	$(call HIMPP_HI3518V100_BUILD_CMDS,)
endef

define Package/$(PKG_NAME)-v1-base/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/include/hi3518v100mpp
	find $(PKG_BUILD_DIR)/lib ! -name "libsns*" -name "lib*.so" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/ \;
	find $(PKG_BUILD_DIR)/ko -name "*.ko" -not -path "$(PKG_BUILD_DIR)/ko/extdrv/*" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/ \;
	find $(PKG_BUILD_DIR)/ko -name "*.sh" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/ \;
	$(CP) -a $(PKG_BUILD_DIR)/include/* $(1)$(HIMPP_PREFIX)/include/hi3518v100mpp/
endef

$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-aptina_9m034))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-aptina_ar0130))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-aptina_ar0330))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-aptina_ar0331))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-aptina_mt9p006))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-galaxycore_gc1004))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-galaxycore_gc1014))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-himax_1375))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-ov_9712))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-ov_9712-plus))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-pana34031))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-pana34041))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-pixelplus_3100k))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-soi_h22))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-sony_icx692))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-sony_imx104))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-sony_imx122))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-sony_imx138))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-sensor-sony_imx236))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-gpio-i2c))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-gpio-i2c-ex))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-hi_i2c))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-pwm))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-ssp-ad9020))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-ssp-pana))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-ssp-sony))
# $(eval $(call BuildPackage,$(PKG_NAME)-v1-extdrv-tw2865))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-audio))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-hifb))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-iq))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-ive))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-region))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-tde))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-vda))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-venc))
$(eval $(call BuildPackage,$(PKG_NAME)-v1-samples-vio))

$(eval $(call BuildPackage,$(PKG_NAME)-v1-base))
