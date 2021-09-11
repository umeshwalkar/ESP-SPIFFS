# Multi-Partition SPIFFS for ESP32

Adapted from the ESP32's build-in SPIFFS lib, this version allows using multiple SPIFFS partitions.
This is modified version of [SPIFFS](https://github.com/lukaswagner/SPIFFS) by Lukas Wagner.
Unlike original lib, in this library user can keep mounted multiple partitions.


## Why?

Let's pretend you need to store some calibration data on the internal flash. That works. But what if you want to update some config files by uploading them from your PC? Your calibration data is gone, since the whole partition has been flashed.

Would be cool if you could use multiple partitions. But the build-in library to work with SPIFFS partitions always uses the first partition with the subtype `spiffs`. That's where this lib comes in.

## What?

The main change in comparison to the build-in lib is the option to specify a partition label with `Spiffs.begin(...)`. Additional important changes:

- Renamed header to `myspiffs.hpp` to avoid confusion with `SPIFFS.h`.
- Renamed class `SPIFFSFS` to `SPIFFS` since the `FS` in `SPIFFS` already means filesystem.
- Renamed global object `SPIFFS` to `Spiffs` to avoid conflict with renamed class.

## Notes

- This was only tested on an ESP32. It probably won't work with an ESP8266 due to the threading functions called in `format()`.
- This was only tested with platformio.
- Uploading a FS image with platformio always writes to the last partition marked as `spiffs`.
- User need to create its own global SPIFFS class object. refer example.
