#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "event_manager.h"


#define TAG "EVENT_MANAGER"
#define EVENT_POOL_SIZE 10
#define NOTIFY_QUEUE_SIZE 2048

ESP_EVENT_DEFINE_BASE(WIFI_MGR_EVENTS);
ESP_EVENT_DEFINE_BASE(ACCESS_MGR_EVENTS);
ESP_EVENT_DEFINE_BASE(OTA_MGR_EVENTS);

// KEYLEN: точна довжина ключа в байтах (з урахуванням можливого padding)
static const size_t KEYLEN = offsetof(event_listeners_record_t, event_id)
                           + sizeof(((event_listeners_record_t*)0)->event_id)
                           - offsetof(event_listeners_record_t, event_base);


static SemaphoreHandle_t subscribers_mutex;
static portMUX_TYPE pool_mux = portMUX_INITIALIZER_UNLOCKED;

static event_listeners_record_t *subscribers_map = NULL;

static QueueHandle_t event_notify_queue;

static system_event_t event_pool[EVENT_POOL_SIZE];
static bool used[EVENT_POOL_SIZE];


static void event_manager_task(void *arg);
static esp_err_t event_manager_task_start();
static int get_subscriber_index(event_listeners_record_t *record, QueueHandle_t queue);
static esp_err_t add_subscriber(event_listeners_record_t *record, QueueHandle_t queue);
static esp_err_t remove_subscriber(event_listeners_record_t *record, QueueHandle_t queue);
static esp_err_t event_manager_publish(esp_event_base_t event_base, int32_t event_id, void* data, size_t data_size);
static system_event_t *allocate_event_slot(void);
static void free_event_slot(system_event_t *evt);

esp_err_t event_manager_init() {
    subscribers_mutex = xSemaphoreCreateMutex();
    configASSERT(subscribers_mutex);

    return event_manager_task_start();
}

static esp_err_t event_manager_task_start()
{
    event_notify_queue = xQueueCreate(EVENT_POOL_SIZE, sizeof(system_event_t *));
    if (!event_notify_queue) return ESP_ERR_NO_MEM;

    BaseType_t ok = xTaskCreate(
        event_manager_task,
        "event_mgr_task",
        NOTIFY_QUEUE_SIZE,
        NULL,
        10,
        NULL
    );
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

static void event_manager_task(void *arg)
{
    system_event_t *evt;
    while (1) {
        if (xQueueReceive(event_notify_queue, &evt, portMAX_DELAY)) {
            event_manager_publish(evt->event_base, evt->event_id, evt->data, evt->data_size);
            free_event_slot(evt);
        }
    }
}

static int get_subscriber_index(event_listeners_record_t* record, QueueHandle_t queue) {
    for(size_t i = 0; i < record->queue_count; i++) {
        if(record->queues[i] == queue) {
            return (int)i;
        }
    }

    return -1;
}

static esp_err_t remove_subscriber(event_listeners_record_t *record, QueueHandle_t queue) {
    int idx = get_subscriber_index(record, queue);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    
    record->queues[idx] = record->queues[record->queue_count - 1];
    record->queue_count--;

    return ESP_OK;
}

static esp_err_t add_subscriber(event_listeners_record_t *record, QueueHandle_t queue) {
    if(record->queue_count < MAX_SUBSCRIBERS_FOR_EVENT) {
        if(get_subscriber_index(record, queue) < 0) {
            record->queues[record->queue_count++] = queue;
            return ESP_OK;
        } else {
            ESP_LOGI(TAG, "Queue is already a subscriber");
            return ESP_OK;
        }
    } else {
        ESP_LOGE(TAG, "Too many subscribers for event");
        return ESP_ERR_NO_MEM;
    }
}

esp_err_t event_manager_subscribe(esp_event_base_t event_base, int32_t event_id, QueueHandle_t queue) {
    esp_err_t err = ESP_OK;

    if(queue == NULL) {
        ESP_LOGE(TAG, "Failed to add subscriber. Invalid queue handle");
        return ESP_ERR_INVALID_ARG;
    }

    event_listeners_record_t *record = NULL;
    struct { esp_event_base_t ev_base; int32_t ev_id; } key = {event_base, event_id};

    if (xSemaphoreTake(subscribers_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take subscribers_mutex");
        return ESP_ERR_TIMEOUT;
    }

    HASH_FIND(hh, subscribers_map, &key, sizeof(key), record);

    if(record == NULL) {
        record = malloc(sizeof(*record));
        if(!record) {
            ESP_LOGE(TAG, "Cannot allocate listener record");
            err = ESP_ERR_NO_MEM;
            goto cleanup;
        }
        memset(record, 0, sizeof(*record));
        record->event_base = event_base;
        record->event_id = event_id;
        record->queue_count = 0;

        HASH_ADD_KEYPTR(hh, subscribers_map, &record->event_base, KEYLEN, record);
    }

    err = add_subscriber(record, queue);

cleanup:
    if (xSemaphoreGive(subscribers_mutex) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to give mutex");
    }
    return err;
}

esp_err_t event_manager_unsubscribe(esp_event_base_t event_base, int32_t event_id, QueueHandle_t queue) {
    esp_err_t err = ESP_OK;

    if(queue == NULL) {
        ESP_LOGE(TAG, "Failed to delete subscriber. Invalid queue handle");
        return ESP_ERR_INVALID_ARG;
    }

    event_listeners_record_t *record = NULL;
    struct { esp_event_base_t ev_base; int32_t ev_id; } key = {event_base, event_id};

    if (xSemaphoreTake(subscribers_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take subscribers_mutex");
        return ESP_ERR_TIMEOUT;
    }

    HASH_FIND(hh, subscribers_map, &key, sizeof(key), record);

    if(record != NULL) {
        err = remove_subscriber(record, queue);
        
        if(record->queue_count == 0) {
            HASH_DEL(subscribers_map, record);
            free(record);
        }
    } else {
        err = ESP_ERR_NOT_FOUND;
    }

    if (xSemaphoreGive(subscribers_mutex) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to give mutex");
    }
    return err;
}

static esp_err_t event_manager_publish(esp_event_base_t event_base, int32_t event_id, void* data, size_t data_size) { 
    if ((data == NULL && data_size > 0) || data_size > MAX_EVENT_PAYLOAD) {
        ESP_LOGE(TAG, "Invalid publish arguments: data=%p, size=%u", data, (unsigned)data_size);
        return ESP_ERR_INVALID_ARG;
    }

    event_listeners_record_t *record = NULL;
    struct { esp_event_base_t ev_base; int32_t ev_id; } key = {event_base, event_id};

    if (xSemaphoreTake(subscribers_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take subscribers_mutex");
        return ESP_ERR_TIMEOUT;
    }

    HASH_FIND(hh, subscribers_map, &key, sizeof(key), record);
    if (!record || record->queue_count == 0) {
        xSemaphoreGive(subscribers_mutex);
        return ESP_ERR_NOT_FOUND;
    }

    QueueHandle_t queues_copy[MAX_SUBSCRIBERS_FOR_EVENT];
    memcpy(queues_copy, record->queues, record->queue_count * sizeof(queues_copy[0]));
    xSemaphoreGive(subscribers_mutex);

    system_event_t evt = {
        .event_base = event_base,
        .event_id = event_id,
        .data_size = data_size
    };
    if (data && data_size) {
        memcpy(evt.data, data, data_size);
    }

    esp_err_t err = ESP_OK;
    for (size_t i = 0; i < record->queue_count; ++i) {
        if (xQueueSend(queues_copy[i], &evt, 0) != pdTRUE) {
            ESP_LOGW(TAG, "Publish: queue[%u] full or unavailable", (unsigned)i);
            err = ESP_FAIL;
        }
    }

    return err;
}

esp_err_t event_manager_notify(esp_event_base_t base, int32_t id, const void *data, size_t size)
{
    if ((data == NULL && size > 0) || (data != NULL && size == 0) || size > MAX_EVENT_PAYLOAD)
        return ESP_ERR_INVALID_ARG;

    system_event_t *evt = allocate_event_slot();
    if (!evt) {
        return ESP_ERR_NO_MEM;
    }

    evt->event_base = base;
    evt->event_id = id;
    evt->data_size = size;

    if (data && size > 0) {
        memcpy(evt->data, data, size);
    }

    if (xQueueSend(event_notify_queue, &evt, 0) != pdTRUE) {
        free_event_slot(evt);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static system_event_t* allocate_event_slot()
{
    taskENTER_CRITICAL(&pool_mux);
    for (int i = 0; i < EVENT_POOL_SIZE; ++i) {
        if (!used[i]) {
            used[i] = true;
            taskEXIT_CRITICAL(&pool_mux);
            return &event_pool[i];
        }
    }
    taskEXIT_CRITICAL(&pool_mux);
    return NULL;
}

static void free_event_slot(system_event_t *evt)
{
    intptr_t index = evt - event_pool;
    if (index >= 0 && index < EVENT_POOL_SIZE) {
        taskENTER_CRITICAL(&pool_mux);
        used[index] = false;
        taskEXIT_CRITICAL(&pool_mux);
    }
}