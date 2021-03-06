#include "myspiffs.hpp"

#include <esp_spiffs.h>
#include <esp_task_wdt.h>
#include <freertos/task.h>
#include <FS.h>
#include <vfs_api.h>

SPIFFS::SPIFFS(const char *_partition) : FS(FSImplPtr(new VFSImpl()))
{
    m_partition = _partition;
}

bool SPIFFS::begin(bool formatOnFail, const char *basePath, uint8_t maxOpenFiles)
{
    // don't do anything if partition is already mounted
    if (esp_spiffs_mounted(m_partition))
    {
        log_w("SPIFFS Already Mounted!");
        m_mounted = true;
        return true;
    }

    // try mounting the partition
    esp_vfs_spiffs_conf_t conf = {
        .base_path = basePath,
        .partition_label = m_partition,
        .max_files = maxOpenFiles,
        .format_if_mount_failed = false};
    auto error = esp_vfs_spiffs_register(&conf);

    // try to reformat (and register again) on error
    if (error == ESP_FAIL && formatOnFail)
    {
        if (format())
        {
            error = esp_vfs_spiffs_register(&conf);
        }
    }

    // if still error, abort
    if (error != ESP_OK)
    {
        log_e("Mounting SPIFFS failed! Error: %d", error);
        return false;
    }

    // success - update state and report success
    m_mounted = true;
    _impl->mountpoint(basePath);
    return true;
}

bool SPIFFS::end()
{
    if (esp_spiffs_mounted(m_partition))
    {
        esp_err_t error = esp_vfs_spiffs_unregister(m_partition);
        if (error != ESP_OK)
        {
            log_e("Unmounting SPIFFS failed! Error: %d", error);
            return false;
        }
        _impl->mountpoint(NULL);
        m_mounted = false;
        // m_partition = NULL;
    }

    return true;
}

void enableWDT(int32_t core)
{
    auto idle = xTaskGetIdleTaskHandleForCPU(core);
    if (idle != NULL)
    {
        esp_task_wdt_add(idle);
    }
}

void disableWDT(int32_t core)
{
    auto idle = xTaskGetIdleTaskHandleForCPU(core);
    if (idle != NULL)
    {
        esp_task_wdt_delete(idle);
    }
}

bool SPIFFS::format()
{
    auto core = xPortGetCoreID();
    enableWDT(1 - core);
    esp_err_t error = esp_spiffs_format(m_partition);
    disableWDT(1 - core);
    if(error){
        log_e("Formatting SPIFFS failed! Error: %d", error);
        return false;
    }
    return true;
}

bool SPIFFS::getSize(size_t *total, size_t *used)
{
    if (!m_mounted)
    {
        log_w("SPIFFS Not Mounted!");
        return false;
    }

    auto code = esp_spiffs_info(m_partition, total, used);
    return code == ESP_OK;
}

size_t SPIFFS::totalBytes()
{
    size_t total,used;
    if(getSize(&total, &used))	
    {
    	return total;
    }
    return 0;
}

size_t SPIFFS::usedBytes()
{
    size_t total,used;
    if(getSize(&total, &used))	
    {
    	return used;
    }
    return 0;
}

bool SPIFFS::exists(const char* path)
{
    if (!m_mounted)
    {
        log_w("SPIFFS Not Mounted!");
        return false;
    }

    File f = open(path, "r");
    if((f == true) && !f.isDirectory())		
    {    
	    f.close();	
    	return true;
    }
    return false;
}

bool SPIFFS::exists(const String& path)
{
    return exists(path.c_str());
}

bool SPIFFS::isMounted()
{
    return m_mounted;
}

const char *SPIFFS::mountedPartition()
{
    return m_partition;
}

