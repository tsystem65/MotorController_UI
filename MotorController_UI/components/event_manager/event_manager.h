#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "stdlib.h"
#include "esp_event_base.h"
#include "esp_err.h"
#include "freertos/queue.h"
#include "uthash.h"

#define MAX_SUBSCRIBERS_FOR_EVENT 30
#define MAX_EVENT_PAYLOAD 512

ESP_EVENT_DECLARE_BASE(WIFI_MGR_EVENTS);
ESP_EVENT_DECLARE_BASE(ACCESS_MGR_EVENTS);
ESP_EVENT_DECLARE_BASE(OTA_MGR_EVENTS);

enum {
    WIFI_MGR_EVENT_CONNECTED,
    WIFI_MGR_EVENT_DISCONNECTED,
    WIFI_MGR_EVENT_SCAN_DONE
};

enum {
    ACCESS_MGR_EVENT_HTTP_FAIL,
    ACCESS_MGR_EVENT_PARSE_FAILED,
    ACCESS_MGR_EVENT_DENIED,
    ACCESS_MGR_EVENT_GRANTED,
    ACCESS_MGR_EVENT_NOT_FOUND
};

enum {
    OTA_MGR_EVENT_STARTED,
    OTA_MGR_EVENT_BEGIN_FAILED,
    OTA_MGR_EVENT_PERFORM,
    OTA_MGR_EVENT_PERFORM_FAILED,
    OTA_MGR_EVENT_COMPLETE_DATA_NOT_RECEIVED,
    OTA_MGR_EVENT_FINISH_FAILED,
    OTA_MGR_EVENT_COMPLETED
};

typedef struct
{
    esp_event_base_t event_base;
    int32_t event_id;
    uint8_t data[MAX_EVENT_PAYLOAD];
    size_t data_size;
} system_event_t;

typedef struct {
    esp_event_base_t event_base;
    int32_t event_id;
    QueueHandle_t queues[MAX_SUBSCRIBERS_FOR_EVENT];
    size_t queue_count;

    UT_hash_handle   hh;
} event_listeners_record_t;

typedef struct
{
    char ssid[32];
    char password[64];
} wifi_connected_event_data_t;

typedef struct
{
    char ssid[32];
    char reason[128];
} wifi_disconnected_event_data_t;

typedef struct
{
    struct {
        char ssid[32];
        int rssi;
    } wifi_aps[10];

    uint16_t ap_count;
} wifi_scan_event_data_t;

typedef struct
{
    int bytes_downloaded;
    int total_image_size_bytes;
} ota_mgr_perform_event_data_t;

esp_err_t event_manager_init();
esp_err_t event_manager_subscribe(esp_event_base_t event_base, int32_t event_id, QueueHandle_t queue);
esp_err_t event_manager_unsubscribe(esp_event_base_t event_base, int32_t event_id, QueueHandle_t queue);
esp_err_t event_manager_notify(esp_event_base_t base, int32_t id, const void *data, size_t size);

#endif // EVENT_MANAGER_H