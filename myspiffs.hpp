#pragma once

#include <Arduino.h>
#include <FS.h>
#include <vfs_api.h>

class SPIFFS : public FS
{
private:
    bool m_mounted = false;
    const char *m_partition = NULL;
public:
    SPIFFS(const char *_partition = NULL);
    bool begin(bool formatOnFail = false, const char *basePath = "/spiffs", uint8_t maxOpenFiles = 10);
    bool end();
    bool format();
    bool getSize(size_t *total, size_t *used);
    size_t totalBytes();
    size_t usedBytes();
    bool isMounted();
    const char *mountedPartition();
    bool exists(const char* path);
    bool exists(const String& path);
};

