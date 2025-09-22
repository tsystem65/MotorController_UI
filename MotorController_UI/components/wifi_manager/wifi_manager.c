#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "wifi_manager.h"
#include "event_manager.h"
#include "nvs_manager.h"

#define MAX_SCAN_AP_COUNT      10
#define MAX_RECONNECT_ATTEMPTS 5
#define RECONNECT_DELAY_MS     5000

static const char *TAG = "WIFI_MANAGER";

static int reconnect_attempts = 0;
static bool wifi_creds_changed = false;
static bool wifi_connected = false;
static esp_timer_handle_t reconnect_timer;
static wifi_creds_t current_wifi_creds;

static esp_timer_handle_t nvs_operation_timer;

static void delayed_nvs_write_wifi_creds_cb(void* arg);

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = false;
        wifi_event_sta_disconnected_t *disconnect_data = (wifi_event_sta_disconnected_t*)event_data;
        wifi_disconnected_event_data_t ev_data = {0};

        size_t len = disconnect_data->ssid_len;
        if (len >= sizeof(ev_data.ssid)) len = sizeof(ev_data.ssid) - 1;
        memcpy(ev_data.ssid, (char*)disconnect_data->ssid, len);
        ev_data.ssid[len] = '\0';
        strlcpy(ev_data.reason, wifi_reason_to_string(disconnect_data->reason), sizeof(ev_data.reason));

        event_manager_notify(WIFI_MGR_EVENTS, WIFI_MGR_EVENT_DISCONNECTED, &ev_data, sizeof(ev_data));
        
        if (!esp_timer_is_active(reconnect_timer)) {
            esp_timer_start_once(reconnect_timer, RECONNECT_DELAY_MS * 1000);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        wifi_connected = true;
        wifi_connected_event_data_t event_data = {0};

        reconnect_attempts = 0;

        size_t ssid_len = strnlen((char*)current_wifi_creds.ssid, sizeof(current_wifi_creds.ssid));
        memcpy(event_data.ssid, current_wifi_creds.ssid, ssid_len);
        event_data.ssid[ssid_len] = '\0';

        size_t pass_len = strnlen((char*)current_wifi_creds.password, sizeof(current_wifi_creds.password));
        memcpy(event_data.password, current_wifi_creds.password, pass_len);
        event_data.password[pass_len] = '\0';
        if(wifi_creds_changed) {
            esp_timer_start_once(nvs_operation_timer, 1000 * 1000);
            wifi_creds_changed = false;
        }
        event_manager_notify(WIFI_MGR_EVENTS, WIFI_MGR_EVENT_CONNECTED, &event_data, sizeof(event_data));
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE) {
        uint16_t ap_count = MAX_SCAN_AP_COUNT;
        wifi_ap_record_t aps[MAX_SCAN_AP_COUNT];

        esp_wifi_scan_get_ap_records(&ap_count, aps);

        wifi_scan_event_data_t scan_data = {0};
        scan_data.ap_count = ap_count;

        size_t max_aps = sizeof(scan_data.wifi_aps) / sizeof(scan_data.wifi_aps[0]);
        for (size_t i = 0; i < max_aps && i < ap_count; i++)
        {
            size_t ssid_len = strnlen((char*)aps[i].ssid, sizeof(aps[i].ssid));
            memcpy(scan_data.wifi_aps[i].ssid, aps[i].ssid, ssid_len);
            scan_data.wifi_aps[i].ssid[ssid_len] = '\0';
            scan_data.wifi_aps[i].rssi = aps[i].rssi;
        }
        
        event_manager_notify(WIFI_MGR_EVENTS, WIFI_MGR_EVENT_SCAN_DONE, &scan_data, sizeof(scan_data));
    }
}

static void reconnect_timer_cb(void *arg)
{
    if (reconnect_attempts < MAX_RECONNECT_ATTEMPTS) {
        reconnect_attempts++;
        ESP_LOGI("WIFI", "Reconnect attempt %d...", reconnect_attempts);
        esp_wifi_connect();
    } else {
        ESP_LOGW("WIFI", "Max reconnect attempts reached");
    }
}

void wifi_manager_init() {
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_timer_create_args_t reconnect_timer_args = {
        .callback = &reconnect_timer_cb,
        .arg = NULL,
        .name = "wifi_reconnect_timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&reconnect_timer_args, &reconnect_timer));

    esp_timer_create_args_t timer_args = {
        .callback = delayed_nvs_write_wifi_creds_cb,
        .arg = NULL,
        .name = "delayed_nvs_save"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &nvs_operation_timer));

    // wifi_creds_t stored_creds = {0};
    // esp_err_t err = nvs_manager_get_wifi_creds(&stored_creds);
    // if (err == ESP_OK) {
    //     ESP_LOGI(TAG, "Stored Wi-Fi credentials found. Connecting to SSID: %s", stored_creds.ssid);

    //     wifi_config_t wifi_config = {0};
    //     strlcpy((char*)wifi_config.sta.ssid, stored_creds.ssid, sizeof(wifi_config.sta.ssid));
    //     strlcpy((char*)wifi_config.sta.password, stored_creds.password, sizeof(wifi_config.sta.password));
    //     wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    //     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    //     ESP_ERROR_CHECK(esp_wifi_connect());

    //     strlcpy(current_wifi_creds.ssid, stored_creds.ssid, sizeof(current_wifi_creds.ssid));
    //     strlcpy(current_wifi_creds.password, stored_creds.password, sizeof(current_wifi_creds.password));
    // } else {
    //     ESP_LOGW(TAG, "No stored Wi-Fi credentials found in NVS.");
    // }
}

void wifi_manager_connect(const char* ssid, const char* password) {
    ESP_ERROR_CHECK(esp_wifi_disconnect());

    wifi_config_t wifi_config = { 0 };
    strlcpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strlcpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    if(strncmp(current_wifi_creds.ssid, ssid, sizeof(current_wifi_creds.ssid)) != 0 ||
       strncmp(current_wifi_creds.password, password, sizeof(current_wifi_creds.password)) != 0)
    {
        strlcpy(current_wifi_creds.ssid, ssid, sizeof(current_wifi_creds.ssid));
        strlcpy(current_wifi_creds.password, password, sizeof(current_wifi_creds.password));
        wifi_creds_changed = true;
    } else {
        wifi_creds_changed = false;
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "Wi-Fi STA init finished.");
}

void wifi_manager_scan() {
    wifi_scan_config_t scan_cfg = {
        .ssid        = NULL,         // всі SSID
        .bssid       = NULL,         // всі BSSID
        .channel     = 0,            // всі канали
        .show_hidden = false,         // виводити приховані мережі
        .scan_type   = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time   = {
            .passive      = 100,     // 100 ms на канал у пасивному режимі
            .active.min   = 30,      // мінімум  30 ms, якщо щось знайдено – до
            .active.max   = 100      // максимум 100 ms на канал в active-scan
        }
    };

    esp_wifi_scan_start(&scan_cfg, false);
}

static void delayed_nvs_write_wifi_creds_cb(void* arg) {
    nvs_manager_set_wifi_creds(&current_wifi_creds);
    ESP_LOGI(TAG, "Wi-Fi creds saved to NVS");
}

const char* wifi_mgr_get_connected_ssid() {
    if(wifi_connected)
        return current_wifi_creds.ssid;

    else return NULL;
}

const char* wifi_reason_to_string(uint8_t reason) {
    switch (reason) {
        // Authentication/Authorization issues
        case WIFI_REASON_AUTH_EXPIRE:
            return "Authentication expired";
        case WIFI_REASON_AUTH_LEAVE:
            return "Deauthentication due to leaving";
        case WIFI_REASON_AUTH_FAIL:
            return "Authentication failed";
        case WIFI_REASON_NOT_AUTHED:
            return "Not authenticated";
        case WIFI_REASON_ASSOC_NOT_AUTHED:
            return "Association but not authenticated";
        
        // Association issues
        case WIFI_REASON_ASSOC_EXPIRE:
            return "Disassociated due to inactivity";
        case WIFI_REASON_ASSOC_TOOMANY:
            return "Too many associated stations";
        case WIFI_REASON_ASSOC_LEAVE:
            return "Deassociated due to leaving";
        case WIFI_REASON_ASSOC_FAIL:
            return "Association failed";
        case WIFI_REASON_NOT_ASSOCED:
            return "Not associated";
        
        // Security/Encryption issues
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            return "4-way handshake timeout";
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            return "Handshake timeout";
        case WIFI_REASON_MIC_FAILURE:
            return "MIC failure";
        case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
            return "Group key update timeout";
        case WIFI_REASON_802_1X_AUTH_FAILED:
            return "802.1X authentication failed";
        case WIFI_REASON_CIPHER_SUITE_REJECTED:
            return "Cipher suite rejected";
        
        // Connection/Signal issues
        case WIFI_REASON_BEACON_TIMEOUT:
            return "Beacon timeout";
        case WIFI_REASON_NO_AP_FOUND:
            return "No AP found";
        case WIFI_REASON_CONNECTION_FAIL:
            return "Connection failed";
        case WIFI_REASON_TIMEOUT:
            return "Timeout";
        
        // Power/Channel issues
        case WIFI_REASON_DISASSOC_PWRCAP_BAD:
            return "Disassociated due to poor power capability";
        case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
            return "Disassociated due to unsupported channel";
        
        // Common generic reasons
        case WIFI_REASON_UNSPECIFIED:
            return "Unspecified reason";
        case WIFI_REASON_STA_LEAVING:
            return "Station leaving";
        case WIFI_REASON_ROAMING:
            return "Roaming";
        
        // Default case
        default:
            return "Unknown reason";
    }
}