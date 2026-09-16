#pragma once

#include <cstring>

bool isValidHexColor(const char *str)
{
  if (!str)
    return false;
  if (strlen(str) != 7)
    return false;
  if (str[0] != '#')
    return false;
  for (int i = 1; i < 7; ++i)
  {
    char c = str[i];
    bool isDigit = (c >= '0' && c <= '9');
    bool isUpper = (c >= 'A' && c <= 'F');
    bool isLower = (c >= 'a' && c <= 'f');
    if (!(isDigit || isUpper || isLower))
      return false;
  }
  return true;
}