#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs_manager.h"

static const char *TAG = "NVS_MANAGER";
static const char *LAST_STATE_NS = "last_state";
static const char *WIFI_NS = "wifi_config";

// Ініціалізація NVS
esp_err_t nvs_manager_init(void) {
    ESP_LOGI(TAG, "Initializing NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash due to error: %s", esp_err_to_name(ret));
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to erase NVS: %s", esp_err_to_name(ret));
            return ret;
        }
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NVS: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "NVS initialized successfully");
    }
    return ret;
}

esp_err_t nvs_manager_set_machine_last_state(double cable_length_counter, double used_cable_length) {
    nvs_handle_t handle;
    //ESP_LOGI(TAG, "Opening NVS namespace %s for writing", LAST_STATE_NS);
    esp_err_t ret = nvs_open(LAST_STATE_NS, NVS_READWRITE, &handle);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        //ESP_LOGW(TAG, "Namespace %s not found, creating it...", LAST_STATE_NS);
        ret = nvs_open(LAST_STATE_NS, NVS_READWRITE, &handle); // Повторна спроба
    }
    if (ret != ESP_OK) {
        //ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(ret));
        return ret;
    }

    // Збереження cable_length_counter
    ret = nvs_set_blob(handle, "CLC", &cable_length_counter, sizeof(double));
    if (ret != ESP_OK) {
        //ESP_LOGE(TAG, "Failed to set cable_length_counter: %s", esp_err_to_name(ret));
        nvs_close(handle);
        return ret;
    }

    // Збереження used_cable_length
    ret = nvs_set_blob(handle, "UCL", &used_cable_length, sizeof(double));
    if (ret != ESP_OK) {
        //ESP_LOGE(TAG, "Failed to set used_cable_length: %s", esp_err_to_name(ret));
        nvs_close(handle);
        return ret;
    }

    // Коміт змін
    ret = nvs_commit(handle);
    if (ret != ESP_OK) {
        //ESP_LOGE(TAG, "Failed to commit NVS: %s", esp_err_to_name(ret));
        nvs_close(handle);
        return ret; // Повертаємо помилку, якщо коміт не вдався
    }

    nvs_close(handle);
    //ESP_LOGI(TAG, "Successfully saved cable_length_counter: %.2f", cable_length_counter);
    //ESP_LOGI(TAG, "Successfully saved used_cable_length: %.2f", used_cable_length);
    return ESP_OK; // Повертаємо успіх лише після всіх операцій
}

// Читання used_cable_length
esp_err_t nvs_manager_get_machine_last_state(double *cable_length_counter, double *used_cable_length) {
    nvs_handle_t handle;
    ESP_LOGI(TAG, "Opening NVS namespace %s for reading", LAST_STATE_NS);
    esp_err_t ret = nvs_open(LAST_STATE_NS, NVS_READONLY, &handle);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Namespace %s not found, initializing with default values", LAST_STATE_NS);
        *cable_length_counter = 0.0f;
        *used_cable_length = 0.0f;
        return ESP_OK;
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(ret));
        return ret;
    }

    // Читання cable_length_counter
    size_t size = sizeof(double);
    ret = nvs_get_blob(handle, "CLC", cable_length_counter, &size);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        *cable_length_counter = 0.0f;
        ESP_LOGW(TAG, "cable_length_counter not found, using default: 0.0");
        ret = ESP_OK;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get cable_length_counter: %s", esp_err_to_name(ret));
        nvs_close(handle);
        return ret;
    }

    // Читання used_cable_length
    size = sizeof(double);
    ret = nvs_get_blob(handle, "UCL", used_cable_length, &size);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        *used_cable_length = 0.0f;
        ESP_LOGW(TAG, "used_cable_length not found, using default: 0.0");
        ret = ESP_OK;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get used_cable_length: %s", esp_err_to_name(ret));
        nvs_close(handle);
        return ret;
    }

    nvs_close(handle);
    ESP_LOGI(TAG, "Read cable_length_counter: %.2f", *cable_length_counter);
    ESP_LOGI(TAG, "Read used_cable_length: %.2f", *used_cable_length);
    return ESP_OK;
}

esp_err_t nvs_manager_set_wifi_creds(const wifi_creds_t* creds) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_NS, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(handle, "wifi_cfg", creds, sizeof(wifi_creds_t));
    if (err != ESP_OK) return err;

    err = nvs_commit(handle);
    nvs_close(handle);

    return err;
}

esp_err_t nvs_manager_get_wifi_creds(wifi_creds_t* creds) {
    if (!creds) return ESP_ERR_INVALID_ARG;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_NS, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    size_t required_size = sizeof(wifi_creds_t);
    err = nvs_get_blob(handle, "wifi_cfg", creds, &required_size);
    nvs_close(handle);

    return err;
}

// Скидання used_cable_length до 0
esp_err_t nvs_manager_reset_machine_last_state(void) {
    return nvs_manager_set_machine_last_state(0.0f, 0.0f);
}