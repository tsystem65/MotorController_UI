idf_component_register(SRCS "ota_manager.c"
                      INCLUDE_DIRS "."
                      REQUIRES event_manager esp_https_ota esp_http_client wifi_manager
                               nvs_flash mbedtls esp_pm)