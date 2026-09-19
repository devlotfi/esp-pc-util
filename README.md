<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/github-banner.png">

# 📜 esp-pc-util

A smart desk pc utility

## Features

- HID interactions (Volume UP/DOWN/MUTE, Track PREVIOUS/PAUSE/NEXT)
- Screen customisation via desktop app (Wallpaper, Colors, Brightness...)

# 📌 Contents

- [Tech stack](#tech-stack)
- [How does the system work ?](#how-does-the-system-work-)
- [Build](#build)
- [Images](#images)
- [Desktop App](#desktop-app)

# Tech stack

## Desktop App

<p float="left">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/html.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/css.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/ts.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/tailwind.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/react.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/fontawesome.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/formik.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/i18n.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/tanstack-router.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/tanstack-query.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/heroui.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/nodeserial.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/electron.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/vite.svg">
</p>

## IOT

<p float="left">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/cpp.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/espressif.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/arduino-json.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/lvgl.svg">
</p>

# How does the system work ?

- The esp32 responds to user interactions by sending the appropriate HID keystroke
- The desktop app uses serial to communicate with the esp32
- The app is used for customisation and also for periodicly sending pc stats

# Build

## Components

- ESP32S3 (N16R8)
- 3x Push Buttons
- Rotary Encoder Module (EC11)
- 2.8 in tft lcd (ILI9341)
- Cables
- Case (Optional)

<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/fritzing.png">

## Images

Some images of the final build

<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/build-1.jpg">
<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/build-2.jpg">

# Desktop App

<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/preview-1.png">
<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/preview-2.png">
<img src="https://raw.githubusercontent.com/devlotfi/esp-pc-util/main/github-assets/preview-3.png">
