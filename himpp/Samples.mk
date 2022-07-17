define Package/$(PKG_NAME)-v1-samples-audio
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_audio
endef

define Package/$(PKG_NAME)-v1-samples-audio/config
	config PACKAGE_HIMPP_SAMPLES_AUDIO
		string
		default y if PACKAGE_himpp-v1-samples-audio
endef

define Package/$(PKG_NAME)-v1-samples-hifb
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_hifb
endef

define Package/$(PKG_NAME)-v1-samples-hifb/config
	config PACKAGE_HIMPP_SAMPLES_HIFB
		string
		default y if PACKAGE_himpp-v1-samples-hifb
endef

define Package/$(PKG_NAME)-v1-samples-iq
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_iq
endef

define Package/$(PKG_NAME)-v1-samples-iq/config
	config PACKAGE_HIMPP_SAMPLES_IQ
		string
		default y if PACKAGE_himpp-v1-samples-iq
endef

define Package/$(PKG_NAME)-v1-samples-ive
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= ive_canny ive_detect ive_FPN ive_sobel e.g.
endef

define Package/$(PKG_NAME)-v1-samples-ive/config
	config PACKAGE_HIMPP_SAMPLES_IVE
		string
		default y if PACKAGE_himpp-v1-samples-ive
endef

define Package/$(PKG_NAME)-v1-samples-region
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_region
endef

define Package/$(PKG_NAME)-v1-samples-region/config
	config PACKAGE_HIMPP_SAMPLES_REGION
		string
		default y if PACKAGE_himpp-v1-samples-region
endef

define Package/$(PKG_NAME)-v1-samples-tde
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_tde
endef

define Package/$(PKG_NAME)-v1-samples-tde/config
	config PACKAGE_HIMPP_SAMPLES_TDE
		string
		default y if PACKAGE_himpp-v1-samples-tde
endef

define Package/$(PKG_NAME)-v1-samples-vda
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_vda
endef

define Package/$(PKG_NAME)-v1-samples-vda/config
	config PACKAGE_HIMPP_SAMPLES_VDA
		string
		default y if PACKAGE_himpp-v1-samples-vda
endef

define Package/$(PKG_NAME)-v1-samples-venc
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_venc
endef

define Package/$(PKG_NAME)-v1-samples-venc/config
	config PACKAGE_HIMPP_SAMPLES_VENC
		string
		default y if PACKAGE_himpp-v1-samples-venc
endef

define Package/$(PKG_NAME)-v1-samples-vio
	$(call Package/$(PKG_NAME)-v1-samples)
	TITLE+= sample_vio
endef

define Package/$(PKG_NAME)-v1-samples-vio/config
	config PACKAGE_HIMPP_SAMPLES_VIO
		string
		default y if PACKAGE_himpp-v1-samples-vio
endef

define Package/$(PKG_NAME)-v1-samples-audio/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/audio/sample_audio $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
endef

define Package/$(PKG_NAME)-v1-samples-hifb/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/samples_data/hifb/res
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/hifb/sample_hifb $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
	$(CP) $(PKG_BUILD_DIR)/sample/hifb/res/* $(1)$(HIMPP_PREFIX)/samples_data/hifb/res/
endef

define Package/$(PKG_NAME)-v1-samples-iq/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/iq/sample_iq $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
endef

define Package/$(PKG_NAME)-v1-samples-ive/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/samples_data/ive/input
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/samples_data/ive/output
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/ive/sample_ive_canny $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/ive/sample_ive_detect $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/ive/sample_ive_FPN $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/ive/sample_ive_sobel_with_cached_mem $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/ive/sample_ive_test_memory $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
	$(CP) $(PKG_BUILD_DIR)/sample/ive/input/* $(1)$(HIMPP_PREFIX)/samples_data/ive/input
	find $(PKG_BUILD_DIR)/sample/ive -name "*.yuv" -not -path "$(PKG_BUILD_DIR)/sample/ive/input/*" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/samples_data/ive/ \;
endef

define Package/$(PKG_NAME)-v1-samples-region/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/samples_data/region
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/region/sample_region $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
	$(CP) $(PKG_BUILD_DIR)/sample/region/mm.bmp $(1)$(HIMPP_PREFIX)/samples_data/region/
endef

define Package/$(PKG_NAME)-v1-samples-tde/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/samples_data/tde/res
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/tde/sample_tde $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
	$(CP) $(PKG_BUILD_DIR)/sample/tde/res/* $(1)$(HIMPP_PREFIX)/samples_data/tde/res/
endef

define Package/$(PKG_NAME)-v1-samples-vda/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/vda/sample_vda $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
endef

define Package/$(PKG_NAME)-v1-samples-venc/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/venc/sample_venc $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
endef

define Package/$(PKG_NAME)-v1-samples-vio/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sample/vio/sample_vio $(1)$(HIMPP_PREFIX)/bin
	$(STRIP) $(1)$(HIMPP_PREFIX)/bin/*
endef
