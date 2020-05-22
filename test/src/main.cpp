#include <Arduino.h>

#include <Esp.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>

//

#include <BluetoothSerial.h>

uint8_t address[6] = {0x70, 0x20, 0x84, 0x6D, 0x3D, 0x4C};
uint8_t address2[6] = {0x4C, 0x3D, 0x6D, 0x84, 0x20, 0x70};

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        Serial.println("ESP_SPP_INIT_EVT");
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        Serial.println("ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        Serial.println("ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        Serial.println("ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        Serial.println("ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        Serial.println("ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        Serial.println("ESP_SPP_DATA_IND_EVT");
        break;
    case ESP_SPP_CONG_EVT:
        Serial.println("ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        Serial.println("ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        Serial.println("ESP_SPP_SRV_OPEN_EVT");
        break;
    default:
        Serial.printf("ESP_SPP_DEFAULT:%d", event);
        break;
    }
}

bool init_bluetooth()
{
    if (!btStart())
    {
        Serial.println("Failed to initialize controller");
        return false;
    }

    if (esp_bluedroid_init() != ESP_OK)
    {
        Serial.println("Failed to initialize bluedroid");
        return false;
    }

    if (esp_bluedroid_enable() != ESP_OK)
    {
        Serial.println("Failed to enable bluedroid");
        return false;
    }

    return true;
}

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("The device started in master mode, make sure remote BT device is on!");

    while (!init_bluetooth())
    {
        delay(100);
    }

    if (esp_spp_register_callback(esp_spp_cb) != ESP_OK)
    {
        Serial.println("spp register failed");
        return;
    }

    if (esp_spp_init(ESP_SPP_MODE_CB) != ESP_OK)
    {
        Serial.println("spp init failed");
        return;
    }
}

void loop()
{
    delay(10000);
    esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE;
    esp_spp_role_t role = ESP_SPP_ROLE_MASTER;
    uint8_t remote_scn = 0;
    Serial.printf("1. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHORIZE;
    role = ESP_SPP_ROLE_MASTER;
    remote_scn = 0;
    Serial.printf("2. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHENTICATE;
    role = ESP_SPP_ROLE_MASTER;
    remote_scn = 0;
    Serial.printf("3. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_NONE;
    role = ESP_SPP_ROLE_SLAVE;
    remote_scn = 0;
    Serial.printf("4. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHORIZE;
    role = ESP_SPP_ROLE_SLAVE;
    remote_scn = 0;
    Serial.printf("5. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHENTICATE;
    role = ESP_SPP_ROLE_SLAVE;
    remote_scn = 0;
    Serial.printf("6. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_NONE;
    role = ESP_SPP_ROLE_MASTER;
    remote_scn = 0;
    Serial.printf("7. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address2), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHORIZE;
    role = ESP_SPP_ROLE_MASTER;
    remote_scn = 0;
    Serial.printf("8. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address2), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHENTICATE;
    role = ESP_SPP_ROLE_MASTER;
    remote_scn = 0;
    Serial.printf("9. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address2), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_NONE;
    role = ESP_SPP_ROLE_SLAVE;
    remote_scn = 0;
    Serial.printf("10. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address2), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHORIZE;
    role = ESP_SPP_ROLE_SLAVE;
    remote_scn = 0;
    Serial.printf("11. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address2), ESP_OK);

    delay(10000);
    sec_mask = ESP_SPP_SEC_AUTHENTICATE;
    role = ESP_SPP_ROLE_SLAVE;
    remote_scn = 0;
    Serial.printf("12. %d ?= %d\r\n", esp_spp_connect(sec_mask, role, remote_scn, address2), ESP_OK);
}