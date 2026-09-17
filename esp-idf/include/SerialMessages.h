#pragma once

#include <ArduinoJson.h>
#include "Vars.h"

class MessageTypes
{
public:
  static constexpr const char *SET_LED = "SET_LED";
  static constexpr const char *SET_DISPLAY = "SET_DISPLAY";
  static constexpr const char *PC_STATS = "PC_STATS";
  static constexpr const char *GET_DATA = "GET_DATA";
  static constexpr const char *SET_WALLPAPER = "SET_WALLPAPER";
  static constexpr const char *SET_WALLPAPER_COMPLETED = "SET_WALLPAPER_COMPLETED";
};