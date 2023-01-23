define Package/$(PKG_NAME)-v1-sensor-aptina_9m034
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - aptina_9m034 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_9m034/config
	config PACKAGE_HIMPP_SNSDRV_APTINA_9M034
		string
		default y if PACKAGE_himpp-v1-sensor-aptina_9m034
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0130
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - aptina_ar0130 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0130/config
	config PACKAGE_HIMPP_SNSDRV_APTINA_AR0130
		string
		default y if PACKAGE_himpp-v1-sensor-aptina_ar0130
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0330
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - aptina_ar0330 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0330/config
	config PACKAGE_HIMPP_SNSDRV_APTINA_AR0330
		string
		default y if PACKAGE_himpp-v1-sensor-aptina_ar0330
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0331
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - aptina_ar0331 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0331/config
	config PACKAGE_HIMPP_SNSDRV_APTINA_AR0331
		string
		default y if PACKAGE_himpp-v1-sensor-aptina_ar0331
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_mt9p006
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - aptina_mt9p006 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_mt9p006/config
	config PACKAGE_HIMPP_SNSDRV_APTINA_MT9P006
		string
		default y if PACKAGE_himpp-v1-sensor-aptina_mt9p006
endef

define Package/$(PKG_NAME)-v1-sensor-galaxycore_gc1004
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - galaxycore_gc1004 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-galaxycore_gc1004/config
	config PACKAGE_HIMPP_SNSDRV_GALAXYCORE_GC1004
		string
		default y if PACKAGE_himpp-v1-sensor-galaxycore_gc1004
endef

define Package/$(PKG_NAME)-v1-sensor-galaxycore_gc1014
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - galaxycore_gc1014 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-galaxycore_gc1014/config
	config PACKAGE_HIMPP_SNSDRV_GALAXYCORE_GC1014
		string
		default y if PACKAGE_himpp-v1-sensor-galaxycore_gc1014
endef

define Package/$(PKG_NAME)-v1-sensor-himax_1375
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - himax_1375 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-himax_1375/config
	config PACKAGE_HIMPP_SNSDRV_HIMAX_1375
		string
		default y if PACKAGE_himpp-v1-sensor-himax_1375
endef

define Package/$(PKG_NAME)-v1-sensor-ov_9712
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - ov_9712 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-ov_9712/config
	config PACKAGE_HIMPP_SNSDRV_ONMI_OV9712
		string
		default y if PACKAGE_himpp-v1-sensor-ov_9712
endef

define Package/$(PKG_NAME)-v1-sensor-ov_9712-plus
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - ov_9712-plus sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-ov_9712-plus/config
	config PACKAGE_HIMPP_SNSDRV_ONMI_OV9712_PLUS
		string
		default y if PACKAGE_himpp-v1-sensor-ov_9712-plus
endef

define Package/$(PKG_NAME)-v1-sensor-pana34031
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - pana34031 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-pana34031/config
	config PACKAGE_HIMPP_SNSDRV_PANASONIC_MN34031
		string
		default y if PACKAGE_himpp-v1-sensor-pana34031
endef

define Package/$(PKG_NAME)-v1-sensor-pana34041
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - pana34041 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-pana34041/config
	config PACKAGE_HIMPP_SNSDRV_PANASONIC_MN34041
		string
		default y if PACKAGE_himpp-v1-sensor-pana34041
endef

define Package/$(PKG_NAME)-v1-sensor-pixelplus_3100k
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - pixelplus_3100k sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-pixelplus_3100k/config
	config PACKAGE_HIMPP_SNSDRV_PIXELPLUS_3100K
		string
		default y if PACKAGE_himpp-v1-sensor-pixelplus_3100k
endef

define Package/$(PKG_NAME)-v1-sensor-soi_h22
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - soi_h22 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-soi_h22/config
	config PACKAGE_HIMPP_SNSDRV_SOI_H22
		string
		default y if PACKAGE_himpp-v1-sensor-soi_h22
endef

define Package/$(PKG_NAME)-v1-sensor-sony_icx692
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - sony_icx692 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-sony_icx692/config
	config PACKAGE_HIMPP_SNSDRV_SONY_ICX692
		string
		default y if PACKAGE_himpp-v1-sensor-sony_icx692
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx104
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - sony_imx104 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx104/config
	config PACKAGE_HIMPP_SNSDRV_SONY_IMX104
		string
		default y if PACKAGE_himpp-v1-sensor-sony_imx104
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx122
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - sony_imx122 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx122/config
	config PACKAGE_HIMPP_SNSDRV_SONY_IMX122
		string
		default y if PACKAGE_himpp-v1-sensor-sony_imx122
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx138
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - sony_imx138 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx138/config
	config PACKAGE_HIMPP_SNSDRV_SONY_IMX138
		string
		default y if PACKAGE_himpp-v1-sensor-sony_imx138
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx236
	$(call Package/$(PKG_NAME)-v1-sensor)
	TITLE+= - sony_imx236 sensor package
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx236/config
	config PACKAGE_HIMPP_SNSDRV_SONY_IMX236
		string
		default y if PACKAGE_himpp-v1-sensor-sony_imx236
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_9m034/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_9m034.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_9m034.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0130/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ar0130_720p.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ar0130_720p.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0330/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ar0330_1080p.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ar0330_1080p.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_ar0331/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ar0331_1080p.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ar0331_1080p.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-aptina_mt9p006/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_mt9p006.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_mt9p006.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-galaxycore_gc1004/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_gc1004.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_gc1004.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-galaxycore_gc1014/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_gc1014.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_gc1014.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-himax_1375/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_himax1375.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_himax1375.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-ov_9712/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ov9712.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ov9712.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-ov_9712-plus/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ov9712_plus.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_ov9712_plus.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-pana34031/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_mn34031_720p.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_mn34031_720p.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-pana34041/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_mn34041.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_mn34041.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-pixelplus_3100k/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_po3100k.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_po3100k.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-soi_h22/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_soih22.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_soih22.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-sony_icx692/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_icx692.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_icx692.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx104/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx104.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx104.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx122/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx122.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx122.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx138/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx138.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx138.so $(1)$(HIMPP_PREFIX)/lib/
endef

define Package/$(PKG_NAME)-v1-sensor-sony_imx236/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx236.a $(1)$(HIMPP_PREFIX)/lib/
	$(CP) $(PKG_BUILD_DIR)/component/isp2/lib/libsns_imx236.so $(1)$(HIMPP_PREFIX)/lib/
endef
