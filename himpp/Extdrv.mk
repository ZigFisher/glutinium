define Package/$(PKG_NAME)-v1-extdrv-gpio-i2c
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= gpio-i2c bit-bang i2c driver
endef

define Package/$(PKG_NAME)-v1-extdrv-gpio-i2c/config
	config PACKAGE_HIMPP_EXTDRV_GPIO_I2C
		string
		default y if PACKAGE_himpp-v1-extdrv-gpio-i2c
endef

define Package/$(PKG_NAME)-v1-extdrv-gpio-i2c-ex
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= gpio-i2c-ex bit-bang i2c driver
endef

define Package/$(PKG_NAME)-v1-extdrv-gpio-i2c-ex/config
	config PACKAGE_HIMPP_EXTDRV_GPIO_I2C_EX
		string
		default y if PACKAGE_himpp-v1-extdrv-gpio-i2c-ex
endef

define Package/$(PKG_NAME)-v1-extdrv-hi_i2c
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= hi-i2c hardware i2c driver

endef

define Package/$(PKG_NAME)-v1-extdrv-hi_i2c/config
	config PACKAGE_HIMPP_EXTDRV_HI_I2C
		string
		default y if PACKAGE_himpp-v1-extdrv-hi_i2c
endef

define Package/$(PKG_NAME)-v1-extdrv-pwm
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= pwm driver
endef

define Package/$(PKG_NAME)-v1-extdrv-pwm/config
	config PACKAGE_HIMPP_EXTDRV_PWM
		string
		default y if PACKAGE_himpp-v1-extdrv-pwm
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-ad9020
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= ssp-ad9020 driver
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-ad9020/config
	config PACKAGE_HIMPP_EXTDRV_SSP_AD9020
		string
		default y if PACKAGE_himpp-v1-extdrv-ssp-ad9020
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-pana
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= ssp-pana driver
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-pana/config
	config PACKAGE_HIMPP_EXTDRV_SSP_PANA
		string
		default y if PACKAGE_himpp-v1-extdrv-ssp-pana
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-sony
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= ssp-sony driver
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-sony/config
	config PACKAGE_HIMPP_EXTDRV_SSP_SONY
		string
		default y if PACKAGE_himpp-v1-extdrv-ssp-sony
endef

define Package/$(PKG_NAME)-v1-extdrv-tw2865
	$(call Package/$(PKG_NAME)-v1-extdrv)
	TITLE+= tw2865 driver
	depends on PACKAGE_himpp-v1-extdrv-hi_i2c
endef

define Package/$(PKG_NAME)-v1-extdrv-tw2865/config
	config PACKAGE_HIMPP_EXTDRV_TW2865
		string
		default y if PACKAGE_himpp-v1-extdrv-tw2865
endef

define Package/$(PKG_NAME)-v1-extdrv-gpio-i2c/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/gpio-i2c -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/gpio-i2c/i2c_read $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/gpio-i2c/i2c_write $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-gpio-i2c-ex/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/gpio-i2c-ex -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/gpio-i2c-ex/i2c_read_ex $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/gpio-i2c-ex/i2c_write_ex $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-hi_i2c/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/hi_i2c -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/hi_i2c/i2c_read $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/hi_i2c/i2c_write $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-pwm/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/pwm -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/pwm/pwm_test $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/pwm/pwm_write $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-ad9020/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/ssp-ad9020 -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/ssp-ad9020/ssp_read $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/ssp-ad9020/ssp_write $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-pana/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/ssp-pana -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/ssp-pana/ssp_read $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/ssp-pana/ssp_write $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-ssp-sony/install
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
	find $(PKG_BUILD_DIR)/extdrv/ssp-sony -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/ssp-sony/ssp_read $(1)$(HIMPP_PREFIX)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/ssp-sony/ssp_write $(1)$(HIMPP_PREFIX)/bin
endef

define Package/$(PKG_NAME)-v1-extdrv-tw2865/install
#	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/bin
#	$(INSTALL_DIR) $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv
#	find $(PKG_BUILD_DIR)/extdrv/tw2865 -name "*.ko" -type f -exec cp {} $(1)$(HIMPP_PREFIX)/lib/himpp-ko/extdrv \;
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/tw2865/i2c_read $(1)$(HIMPP_PREFIX)/bin
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extdrv/tw2865/i2c_write $(1)$(HIMPP_PREFIX)/bin
endef
