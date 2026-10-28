/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-web-server/
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "sdkconfig.h"

#define AP_SSID "MotorMonitor"
#define AP_PASSWORD "monitor1234"

static const char *TAG = "web_server";

// HTML web page to serve

// Referências ao HTML incorporado no firmware
extern const uint8_t index_html_start[] asm("_binary_index_html_start");

extern const uint8_t index_html_end[] asm("_binary_index_html_end");

// HTTP GET handler for root "/"
static esp_err_t root_get_handler(httpd_req_t *req)
{
    size_t html_size = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html; charset=utf-8");

    return httpd_resp_send(req, (const char *)index_html_start, html_size);
}

// Start the HTTP server
static httpd_handle_t start_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP server started on port %d", config.server_port);

        // Register URI handler
        httpd_uri_t uri_get = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_get);

        return server;
    }

    ESP_LOGE(TAG, "Failed to start HTTP server");
    return NULL;
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize TCP/IP stack and event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Cria interface Wi-Fi Access Point
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Configura a rede criada pelo ESP32
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = sizeof(AP_SSID) - 1,
            .password = AP_PASSWORD,
            .channel = 1,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    // Ativa o modo Access Point
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());

    // Consulta o endereço IP do AP
    esp_netif_ip_info_t ip_info;
    ESP_ERROR_CHECK(
        esp_netif_get_ip_info(ap_netif, &ip_info));

    ESP_LOGI(TAG, "Rede Wi-Fi criada: %s", "MotorMonitor");
    ESP_LOGI(TAG, "IP do ESP32: " IPSTR,
             IP2STR(&ip_info.ip));

    // Inicializa o servidor HTTP
    start_web_server();
}