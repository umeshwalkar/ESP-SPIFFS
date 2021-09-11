//refer :https://esp32.com/viewtopic.php?t=12331
// https://github.com/lukaswagner/SPIFFS
// https://github.com/particle-iot/esp32-at

#include <Arduino.h>
#include <myspiffs.hpp>

// this is optional name useful to track error on which partition.
// consider this as 'drive' label and can be any text name.
#define base_path_1 "/A:" // "/appMem"
#define base_path_2 "/B:" // "/dataMem"

// name should be same as defined in partition table
SPIFFS Spiffs_1("storage1"); // the first entry is considered if using in-build lib SPIFFS.h with current partition table
SPIFFS Spiffs_2("storage2"); // the last entry will be consider to upoad fs i.e. 'data' folder

const char driveAfile[] = "/written.txt";
const char driveBfile[] = "/uploaded.txt";

const char writebuff[] = "ESP32 is a series of low-cost,low-power system on a chip microcontrollers with integrated Wi-Fi and dual-mode Bluetooth.ESP32 series employs either a Tensilica Xtensa LX6 microprocessor in both dual-core and single-core variations,Xtensa LX7 dual-core microprocessor or a single-core RISC-V microprocessor and includes built-in antenna switches,RF balun,power amplifier,low-noise receive amplifier,filters and power-management modules.ESP32 is created and developed by Espressif Systems.Its successor of ESP8266.";

bool mount(const char *base_path, SPIFFS *spiffs)
{
    Serial.printf("Mounting %s... ", spiffs->mountedPartition());
    if (spiffs->begin(true, base_path))
    {
        Serial.println("success.");
        return true;
    }
    else
    {
        // try one more time. sometime re-mounting works.
        if (spiffs->begin(true, base_path))
        {
            Serial.println("success.");
            return true;
        }
        Serial.println("failed.");
        return false;
    }
}

void printSize(SPIFFS *spiffs)
{
    size_t total, used;
    spiffs->getSize(&total, &used);
    Serial.printf(
        "Partition '%s': %u of %u bytes used.\n",
        spiffs->mountedPartition(),
        used,
        total);
}

void readFile(const char *path, SPIFFS *spiffs)
{
    auto file = spiffs->open(path);
    if (file)
    {
        Serial.printf("Contents of %s: %s\n", path, file.readString().c_str());
        file.close();
    }
    else
    {
        Serial.printf("Could not open %s for reading.\n", path);
    }
}

void writeFile(const char *path, SPIFFS *spiffs)
{
    auto file = spiffs->open(path, FILE_APPEND);
    if (file)
    {
        Serial.printf("written %d: %d\n", file.print(writebuff), strlen(writebuff));

        file.close();
    }
    else
    {
        Serial.printf("Could not open %s for reading.\n", path);
    }
}

void countUp(const char *path, SPIFFS *spiffs)
{
    auto file = spiffs->open(path);
    if (!file)
    {
        Serial.printf("Could not open %s for reading.\n", path);
        return;
    }
    auto text = file.readString();
    file.close();

    auto oldValue = text.toInt();
    auto newValue = oldValue + 1;

    file = spiffs->open(path, FILE_WRITE);
    if (!file)
    {
        Serial.printf("Could not open %s for writing.\n", path);
        return;
    }
    file.print(newValue);
    file.close();
    Serial.printf("Updated content of %s from %li to %li.\n", path, oldValue, newValue);
}

void unmount(SPIFFS *spiffs)
{
    Serial.printf("Unmounting %s... ", spiffs->mountedPartition());
    if (spiffs->end())
    {
        Serial.println("success.");
    }
    else
    {
        Serial.println("failed.");
    }
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    if (mount(base_path_1, &Spiffs_1))
    {
        printSize(&Spiffs_1);
        Serial.printf("file %s ", driveAfile);
        if (Spiffs_1.exists(driveAfile))
        {
            Serial.println("exists");
        }
        else
        {
            Serial.println("not exists");
        }
    }

    if (mount(base_path_2, &Spiffs_2))
    {
        printSize(&Spiffs_2);
        Serial.printf("file %s ", driveBfile);
        if (Spiffs_2.exists(driveBfile))
        {
            Serial.println("exists");
        }
        else
        {
            Serial.println("not exists");
        }
    }
}

void loop()
{
    char ch = 0;
    if (Serial.available())
    {
        ch = Serial.read();
        switch (ch)
        {
        case 'M':
            mount(base_path_1, &Spiffs_1);
            break;
        case 'm':
            mount(base_path_2, &Spiffs_2);
            break;

        case 'U':
            unmount(&Spiffs_1);
            break;
        case 'u':
            unmount(&Spiffs_2);
            break;

        case 'F':
            // unmount(&Spiffs_1);
            Spiffs_1.format();
            break;

        case 'f':
            // unmount(&Spiffs_2);
            Spiffs_2.format();
            break;

        case 'R':
            readFile(driveAfile, &Spiffs_1);
            break;
        case 'r':
            readFile(driveBfile, &Spiffs_2);
            break;

        case 'W':
            writeFile(driveAfile, &Spiffs_1);
            break;
        case 'w':
            writeFile(driveBfile, &Spiffs_2);
            break;

        case 'C':
            countUp(driveAfile, &Spiffs_1);
            break;
        case 'c':
            countUp(driveBfile, &Spiffs_2);
            break;

        case 'P':
            printSize(&Spiffs_1);
            break;
        case 'p':
            printSize(&Spiffs_2);
            break;

        case 'L':
        {
            Serial.print("\n=====================================\n");
            File root = Spiffs_1.open("/");
            File file = root.openNextFile();
            while (file)
            {
                Serial.printf(PSTR("[MAIN] FILE   : %s, Size: %u bytes\n"), file.name(), file.size());
                file = root.openNextFile();
            }
            Serial.print("=====================================\n");
        }
        break;

        case 'l':
        {
            Serial.print("\n=====================================\n");
            File root = Spiffs_2.open("/");
            File file = root.openNextFile();
            while (file)
            {
                Serial.printf(PSTR("[MAIN] FILE   : %s, Size: %u bytes\n"), file.name(), file.size());
                file = root.openNextFile();
            }
            Serial.print("=====================================\n");
        }
        break;
        default:
            break;
        }
    }
    delay(10);
}
