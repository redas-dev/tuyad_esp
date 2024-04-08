include $(TOPDIR)/rules.mk

PKG_NAME:=tuyad_esp
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/tuyad_esp
	CATEGORY:=Base system
	TITLE:=tuyad_esp
	DEPENDS:=+tuya-iot-core-sdk +libubox +libubus +argp-standalone +libblobmsg-json
endef

define Package/tuyad_esp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuyad_esp $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
	$(INSTALL_BIN) ./files/tuyad_esp.init $(1)/etc/init.d/tuyad_esp
	$(INSTALL_CONF) ./files/tuyad_esp.config $(1)/etc/config/tuyad_esp
endef

$(eval $(call BuildPackage,tuyad_esp))