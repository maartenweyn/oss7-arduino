#ifndef FS_H_
#define FS_H_

#include <arduino.h>

#ifndef FRAMEWORK_FS_FILE_COUNT
#define FRAMEWORK_FS_FILE_COUNT 80
#endif

#ifndef FRAMEWORK_FS_USER_FILE_COUNT
#define FRAMEWORK_FS_USER_FILE_COUNT 33
#endif

#ifndef FRAMEWORK_FS_PERMANENT_STORAGE_SIZE
#define FRAMEWORK_FS_PERMANENT_STORAGE_SIZE 1900
#endif

#ifndef FRAMEWORK_FS_VOLATILE_STORAGE_SIZE
#define FRAMEWORK_FS_VOLATILE_STORAGE_SIZE 1024
#endif

#define FS_MAGIC_NUMBER { 0x34, 0xC2, 0x00, 0x00 } // first 2 bytes fixed, last 2 byte for version
#define FS_MAGIC_NUMBER_SIZE 4

typedef enum
{
    FS_STORAGE_TRANSIENT = 0, // The content is not kept in memory. It cannot be read back.
    FS_STORAGE_VOLATILE = 1,  // The content is kept in a volatile memory of the device. It is accessible for read, and is lost on power off.
    FS_STORAGE_RESTORABLE = 2, // The content is kept in a volatile memory of the device. It is accessible for read, and can be backed-up upon request in a permanent storage location. It is restored from the permanent location on device power on.
    FS_STORAGE_PERMANENT = 3  // The content is kept in a permanent memory of the device. It is accessible for read and write
} fs_storage_class_t;

typedef struct  __attribute__((__packed__))
{
    uint32_t length;
    fs_storage_class_t storage : 2;
    uint8_t rfu : 6; //FIXME: 'valid' field or invalid storage qualifier?
} fs_file_stat_t;

#endif
