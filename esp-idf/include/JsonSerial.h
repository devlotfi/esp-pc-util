#pragma once

#include <ArduinoJson.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "tinyusb_cdc_acm.h"
#include "SerialMessages.h"
#include "Vars.h"

class JsonSerial
{
public:
  static constexpr size_t RX_BUFFER_SIZE = 256 * 1024;
  static constexpr size_t USB_READ_BUFFER_SIZE = 512;
  static constexpr size_t MAX_JSON_SIZE = RX_BUFFER_SIZE;

  using JsonCallback = void (*)(JsonDocument &document);
  using ImageCallback = void (*)(const char *type, size_t typeLen, const char *base64, size_t base64Len);

  explicit JsonSerial(
      tinyusb_cdcacm_itf_t cdcPort = TINYUSB_CDC_ACM_0)
      : cdcPort_(cdcPort)
  {
  }

  esp_err_t begin()
  {
    rxBuffer_ = static_cast<uint8_t *>(
        heap_caps_malloc(
            RX_BUFFER_SIZE,
            MALLOC_CAP_SPIRAM));

    txBuffer_ = static_cast<uint8_t *>(
        heap_caps_malloc(
            MAX_JSON_SIZE + HEADER_SIZE,
            MALLOC_CAP_SPIRAM));

    if (rxBuffer_ == nullptr || txBuffer_ == nullptr)
    {
      if (rxBuffer_)
      {
        heap_caps_free(rxBuffer_);
        rxBuffer_ = nullptr;
      }

      if (txBuffer_)
      {
        heap_caps_free(txBuffer_);
        txBuffer_ = nullptr;
      }

      return ESP_ERR_NO_MEM;
    }

    const tinyusb_config_cdcacm_t cdcConfig = {
        .cdc_port = cdcPort_,
        .callback_rx = nullptr,
        .callback_rx_wanted_char = nullptr,
        .callback_line_state_changed = nullptr,
        .callback_line_coding_changed = nullptr,
    };

    return tinyusb_cdcacm_init(&cdcConfig);
  }

  void onJson(JsonCallback callback) { callback_ = callback; }
  void onImage(ImageCallback callback) { imageCallback_ = callback; }

  void process()
  {
    uint8_t buffer[USB_READ_BUFFER_SIZE];
    size_t received = 0;

    esp_err_t err = tinyusb_cdcacm_read(
        cdcPort_,
        buffer,
        sizeof(buffer),
        &received);

    if (err != ESP_OK || received == 0)
      return;

    for (size_t i = 0; i < received; ++i)
    {
      processByte(buffer[i]);
    }
  }

  bool send(JsonDocument &document)
  {
    if (txBuffer_ == nullptr)
      return false;

    size_t jsonSize = serializeJson(
        document,
        txBuffer_ + HEADER_SIZE,
        MAX_JSON_SIZE);

    if (jsonSize >= MAX_JSON_SIZE)
      return false;

    return sendBuffer(
        txBuffer_ + HEADER_SIZE,
        jsonSize);
  }

  bool sendJson(
      const char *json,
      size_t length)
  {
    if (json == nullptr)
      return false;

    if (length > MAX_JSON_SIZE)
      return false;

    return sendBuffer(
        reinterpret_cast<const uint8_t *>(json),
        length);
  }

private:
  static constexpr size_t HEADER_SIZE = 4;
  static constexpr const char *TYPE_KEY = "type";
  static constexpr const char *IMAGE_KEY = "image";

  tinyusb_cdcacm_itf_t cdcPort_;
  JsonCallback callback_ = nullptr;
  ImageCallback imageCallback_ = nullptr;

  uint8_t *rxBuffer_ = nullptr;
  uint8_t *txBuffer_ = nullptr;

  enum class RxState : uint8_t
  {
    HEADER,
    PAYLOAD
  };

  RxState rxState_ = RxState::HEADER;

  uint8_t header_[HEADER_SIZE];
  size_t headerBytes_ = 0;

  uint32_t expectedLength_ = 0;
  size_t receivedLength_ = 0;

  void processByte(uint8_t byte)
  {
    if (rxState_ == RxState::HEADER)
    {
      processHeaderByte(byte);
    }
    else
    {
      processPayloadByte(byte);
    }
  }

  void processHeaderByte(uint8_t byte)
  {
    header_[headerBytes_] = byte;
    ++headerBytes_;

    if (headerBytes_ < HEADER_SIZE)
      return;

    expectedLength_ =
        (static_cast<uint32_t>(header_[0]) << 24) |
        (static_cast<uint32_t>(header_[1]) << 16) |
        (static_cast<uint32_t>(header_[2]) << 8) |
        static_cast<uint32_t>(header_[3]);

    headerBytes_ = 0;
    receivedLength_ = 0;

    if (expectedLength_ > MAX_JSON_SIZE)
    {
      resetReceiver();
      return;
    }

    rxState_ = RxState::PAYLOAD;

    if (expectedLength_ == 0)
    {
      resetReceiver();
    }
  }

  void processPayloadByte(uint8_t byte)
  {
    if (receivedLength_ >= MAX_JSON_SIZE)
    {
      resetReceiver();
      return;
    }

    rxBuffer_[receivedLength_] = byte;
    ++receivedLength_;

    if (receivedLength_ == expectedLength_)
    {
      handleCompleteFrame();
      resetReceiver();
    }
  }

  void handleCompleteFrame()
  {
    const char *json = reinterpret_cast<const char *>(rxBuffer_);

    // ---- Fast path: is this a SET_WALLPAPER frame? ----
    size_t typeLen = 0;
    const char *type = findStringValue(json, receivedLength_, TYPE_KEY, typeLen);

    if (type != nullptr &&
        typeLen == strlen(MessageTypes::SET_WALLPAPER) &&
        memcmp(type, MessageTypes::SET_WALLPAPER, typeLen) == 0 &&
        imageCallback_ != nullptr)
    {
      size_t imgLen = 0;
      const char *img = findStringValue(json, receivedLength_, IMAGE_KEY, imgLen);

      if (img != nullptr && imgLen > 0)
      {
        imageCallback_(type, typeLen, img, imgLen);
      }
      else
      {
        ESP_LOGE("JsonSerial", "SET_WALLPAPER frame missing 'image' field");
      }
      return;
    }

    // ---- Normal path: small JSON document ----
    JsonDocument document(&spiRamAllocator);

    DeserializationError error =
        deserializeJson(document, rxBuffer_, receivedLength_);

    if (error)
    {
      ESP_LOGE("JsonSerial", "JSON error: %s", error.c_str());
      return;
    }

    if (callback_)
    {
      callback_(document);
    }
  }

  bool sendBuffer(
      const uint8_t *data,
      size_t length)
  {
    if (data == nullptr || txBuffer_ == nullptr)
      return false;

    if (length > MAX_JSON_SIZE)
      return false;

    txBuffer_[0] =
        static_cast<uint8_t>(
            (length >> 24) & 0xFF);

    txBuffer_[1] =
        static_cast<uint8_t>(
            (length >> 16) & 0xFF);

    txBuffer_[2] =
        static_cast<uint8_t>(
            (length >> 8) & 0xFF);

    txBuffer_[3] =
        static_cast<uint8_t>(
            length & 0xFF);

    if (data != txBuffer_ + HEADER_SIZE)
    {
      memcpy(
          txBuffer_ + HEADER_SIZE,
          data,
          length);
    }

    const uint8_t *frame = txBuffer_;
    size_t remaining = HEADER_SIZE + length;

    while (remaining > 0)
    {
      size_t written =
          tinyusb_cdcacm_write_queue(
              cdcPort_,
              frame,
              remaining);

      if (written == 0)
      {
        /*
         * CDC TX FIFO is currently full.
         * Give the TinyUSB task time to transmit.
         */
        tinyusb_cdcacm_write_flush(
            cdcPort_,
            0);

        vTaskDelay(1);
        continue;
      }

      frame += written;
      remaining -= written;
    }

    /*
     * Ask TinyUSB to flush the queued data.
     * Zero timeout = non-blocking.
     */
    tinyusb_cdcacm_write_flush(
        cdcPort_,
        0);

    return true;
  }

  void resetReceiver()
  {
    rxState_ = RxState::HEADER;
    headerBytes_ = 0;
    expectedLength_ = 0;
    receivedLength_ = 0;
  }

  static const char *findStringValue(const char *json, size_t jsonLen, const char *key, size_t &outLen)
  {
    outLen = 0;
    if (json == nullptr || key == nullptr)
      return nullptr;

    const size_t keyLen = strlen(key);
    if (keyLen == 0 || jsonLen < keyLen + 4)
      return nullptr;

    auto isWs = [](char c)
    {
      return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    };

    for (size_t i = 0; i + keyLen + 3 <= jsonLen; ++i)
    {
      if (json[i] != '"')
        continue;
      if (memcmp(json + i + 1, key, keyLen) != 0)
        continue;
      if (json[i + 1 + keyLen] != '"')
        continue;

      size_t j = i + 1 + keyLen + 1; // past the closing quote of the key
      while (j < jsonLen && isWs(json[j]))
        ++j;
      if (j >= jsonLen || json[j] != ':')
        continue;
      ++j;
      while (j < jsonLen && isWs(json[j]))
        ++j;
      if (j >= jsonLen || json[j] != '"')
        continue;
      ++j;

      const size_t start = j;
      while (j < jsonLen && json[j] != '"')
        ++j;
      if (j >= jsonLen)
        return nullptr; // unterminated string

      outLen = j - start;
      return json + start;
    }
    return nullptr;
  }
};
