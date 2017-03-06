deps_config := \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/bt/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/esp32/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/ethernet/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/freertos/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/log/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/lwip/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/mbedtls/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/openssl/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/spi_flash/Kconfig \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/pxl/Desktop/iotbits_esp32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
