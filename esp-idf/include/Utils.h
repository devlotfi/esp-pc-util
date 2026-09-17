#pragma once

#include <cstdint>
#include <cstdlib>

uint32_t hexColor(const char *str)
{
  if (str == nullptr)
    return 0;

  // Skip '#'
  if (str[0] == '#')
    str++;

  return static_cast<uint32_t>(strtoul(str, nullptr, 16));
}