include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=ZStackHomeAutomation
PKG_VERSION:=1.0
PKG_RELEASE:=1

# Source settings (i.e. where to find the source codes)
# This is a custom variable, used below
SOURCE_DIR:=../Source

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/ZStackHomeAutomation
	DEPENDS:= +libprotobuf-c +libpthread
	SECTION:=ZStack
	CATEGORY:=Network
	TITLE:=Z-Stack ZNP Home Automation Gateway
endef

# Package description; a more verbose description on what our package does
define Package/ZStackHomeAutomation/description
  TI's Z-Stack Home Automation Gateway for OpenWRT environment.
endef

# Package preparation instructions; create the build directory and copy the source code. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -r $(SOURCE_DIR)/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

ZNP_SERVER = $(PKG_BUILD_DIR)/Projects/zstack/linux/zstackserverznp
NWK_MGR = $(PKG_BUILD_DIR)/Projects/zstack/linux/nwkmgr
GW_SERVER = $(PKG_BUILD_DIR)/Projects/zstack/linux/hagateway
HA_APP = $(PKG_BUILD_DIR)/Projects/zstack/linux/demo/project
NPI_LNX = $(PKG_BUILD_DIR)/Projects/tools/LinuxHost
SCRIPTS_DIR = $(PKG_BUILD_DIR)/Projects/zstack/linux/sdb/scripts


INSTALL_PATH = /root/zstack

# Package build instructions; invoke the target-specific compiler to first compile the source file, and then to link the file into the final executable
define Build/Compile
	echo $(BUILD_DIR)
	$(MAKE) -C $(ZNP_SERVER) \
               CC="$(TARGET_CC)" \
           CFLAGS="$(TARGET_CFLAGS)" \
          LDFLAGS="$(TARGET_LDFLAGS)"
	$(MAKE) -C $(NWK_MGR) \
               CC="$(TARGET_CC)" \
           CFLAGS="$(TARGET_CFLAGS)" \
          LDFLAGS="$(TARGET_LDFLAGS)"
	$(MAKE) -C $(GW_SERVER) \
               CC="$(TARGET_CC)" \
           CFLAGS="$(TARGET_CFLAGS)" \
          LDFLAGS="$(TARGET_LDFLAGS)"
	$(MAKE) -C $(HA_APP) \
               CC="$(TARGET_CC)" \
           CFLAGS="$(TARGET_CFLAGS)" \
          LDFLAGS="$(TARGET_LDFLAGS)"          
	$(MAKE) -C $(NPI_LNX) \
               CC="$(TARGET_CC)" \
           CFLAGS="$(TARGET_CFLAGS)" \
          LDFLAGS="$(TARGET_LDFLAGS)" 
endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/ZStackHomeAutomation/install
	$(INSTALL_DIR) $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(ZNP_SERVER)/server_znp $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(ZNP_SERVER)/config.ini $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(NWK_MGR)/nwkmgr_svr $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(NWK_MGR)/nwkmgr_config.ini $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(GW_SERVER)/gateway_svr $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(GW_SERVER)/gateway_config.ini $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(GW_SERVER)/gateway_config.tlg $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(HA_APP)/ha_main.bin $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(NPI_LNX)/npi_lnx $(1)$(INSTALL_PATH)
	$(INSTALL_BIN) $(NPI_LNX)/NPI_Gateway.cfg $(1)$(INSTALL_PATH)
	$(CP) $(SCRIPTS_DIR)/hagateway/* $(1)$(INSTALL_PATH)
	$(CP) $(SCRIPTS_DIR)/sample_app/* $(1)$(INSTALL_PATH)
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,ZStackHomeAutomation))
