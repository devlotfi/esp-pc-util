#pragma once

#include <ArduinoJson.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_err.h"
#include "esp_heap_caps.h"
#include "tinyusb_cdc_acm.h"

#include "Vars.h"

class JsonSerial
{
public:
  static constexpr size_t RX_BUFFER_SIZE = 256 * 1024;
  static constexpr size_t USB_READ_BUFFER_SIZE = 512;
  static constexpr size_t MAX_JSON_SIZE = RX_BUFFER_SIZE;

  using JsonCallback =
      void (*)(JsonDocument &document);

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

  void onJson(JsonCallback callback)
  {
    callback_ = callback;
  }

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

  tinyusb_cdcacm_itf_t cdcPort_;

  JsonCallback callback_ = nullptr;

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
    JsonDocument document(&psramAllocator);

    DeserializationError error =
        deserializeJson(
            document,
            rxBuffer_,
            receivedLength_);

    if (error)
      return;

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
};