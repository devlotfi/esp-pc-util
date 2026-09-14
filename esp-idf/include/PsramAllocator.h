#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <cstring>
#include "esp_heap_caps.h"

class PsramAllocator : public ArduinoJson::Allocator
{
public:
  explicit PsramAllocator(size_t size)
      : capacity_(size), offset_(0)
  {
    buffer_ = static_cast<uint8_t *>(heap_caps_malloc(size, MALLOC_CAP_SPIRAM));
  }

  ~PsramAllocator()
  {
    heap_caps_free(buffer_);
  }

  void *allocate(size_t size) override
  {
    size = align(size);
    const size_t totalSize = size + sizeof(BlockHeader);

    if (!buffer_)
      return nullptr;

    if (offset_ + totalSize > capacity_)
    {
      reset();

      if (offset_ + totalSize > capacity_)
        return nullptr;
    }

    auto *header = reinterpret_cast<BlockHeader *>(buffer_ + offset_);
    header->size = size;

    void *userPtr = header + 1;
    offset_ += totalSize;

    return userPtr;
  }

  void deallocate(void *) override
  {
  }

  void *reallocate(void *ptr, size_t new_size) override
  {
    if (!ptr)
      return allocate(new_size);

    auto *header = reinterpret_cast<BlockHeader *>(ptr) - 1;
    const size_t old_size = header->size;

    new_size = align(new_size);

    void *new_ptr = allocate(new_size);
    if (!new_ptr)
      return nullptr;

    std::memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);
    return new_ptr;
  }

  void reset()
  {
    offset_ = 0;
  }

  bool valid() const
  {
    return buffer_ != nullptr;
  }

private:
  struct BlockHeader
  {
    size_t size;
  };

  static size_t align(size_t n)
  {
    constexpr size_t alignment = 4;
    return (n + alignment - 1) & ~(alignment - 1);
  }

  uint8_t *buffer_;
  size_t capacity_;
  size_t offset_;
};
