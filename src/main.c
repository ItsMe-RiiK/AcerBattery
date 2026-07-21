#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SYSFS_BASE "/sys/bus/wmi/drivers/acer-wmi-battery"

void print_usage(const char* progname)
{
    printf("Usage: %s [OPTIONS]\n", progname);
    printf("Options:\n");
    printf("  --status         Show current battery status (health, calibration, temp)\n");
    printf("  --health <0|1>   Set health mode (1 to enable, 0 to disable)\n");
    printf("  --calibrate <0|1> Set calibration mode (1 to enable, 0 to disable)\n");
    printf("\nNote: Setting modes requires root privileges (sudo).\n");
}

int read_sysfs(const char* filename, char* buffer, size_t size)
{
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", SYSFS_BASE, filename);

    FILE* f = fopen(filepath, "r");
    if (!f)
    {
        perror("Failed to open sysfs file for reading");
        return -1;
    }

    if (fgets(buffer, size, f) == NULL)
    {
        perror("Failed to read sysfs file");
        fclose(f);
        return -1;
    }

    fclose(f);
    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';
    return 0;
}

int write_sysfs(const char* filename, const char* value)
{
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", SYSFS_BASE, filename);

    FILE* f = fopen(filepath, "w");
    if (!f)
    {
        perror("Failed to open sysfs file for writing (did you use sudo?)");
        return -1;
    }

    if (fprintf(f, "%s\n", value) < 0)
    {
        perror("Failed to write to sysfs file");
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

void show_status()
{
    char buf[64];

    printf("=== Acer Battery WMI Status ===\n");

    if (read_sysfs("health_mode", buf, sizeof(buf)) == 0)
    {
        printf("Health Mode      : %s\n", strcmp(buf, "1") == 0 ? "Enabled" : "Disabled");
    }

    if (read_sysfs("calibration_mode", buf, sizeof(buf)) == 0)
    {
        printf("Calibration Mode : %s\n", strcmp(buf, "1") == 0 ? "Enabled" : "Disabled");
    }

    if (read_sysfs("temperature", buf, sizeof(buf)) == 0)
    {
        int mC = atoi(buf);
        printf("Temperature      : %.1f C\n", mC / 1000.0);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Ensure driver is loaded
    if (access(SYSFS_BASE, F_OK) != 0)
    {
        fprintf(stderr, "Error: Acer Battery WMI driver not found.\n");
        fprintf(stderr, "Make sure the RKKDR kernel driver is loaded (make load).\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--status") == 0)
        {
            show_status();
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[i], "--health") == 0)
        {
            if (i + 1 < argc)
            {
                const char* val = argv[++i];
                if (write_sysfs("health_mode", val) == 0)
                {
                    printf("Health mode set to %s.\n", val);
                }
                return EXIT_SUCCESS;
            }
            else
            {
                fprintf(stderr, "Error: --health requires an argument (0 or 1)\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(argv[i], "--calibrate") == 0)
        {
            if (i + 1 < argc)
            {
                const char* val = argv[++i];
                if (write_sysfs("calibration_mode", val) == 0)
                {
                    printf("Calibration mode set to %s.\n", val);
                }
                return EXIT_SUCCESS;
            }
            else
            {
                fprintf(stderr, "Error: --calibrate requires an argument (0 or 1)\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
