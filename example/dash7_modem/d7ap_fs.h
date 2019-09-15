#ifndef D7AP_FS_H_
#define D7AP_FS_H_

#include "arduino.h"
#include "fs.h"
#include "dae.h"


#define D7A_FILE_UID_FILE_ID 0x00
#define D7A_FILE_UID_SIZE 8

#define D7A_FILE_FIRMWARE_VERSION_FILE_ID 0x02
#define D7A_FILE_FIRMWARE_VERSION_APP_NAME_SIZE 6
#define D7A_FILE_FIRMWARE_VERSION_GIT_SHA1_SIZE 7
#define D7A_FILE_FIRMWARE_VERSION_SIZE (2 + D7A_FILE_FIRMWARE_VERSION_APP_NAME_SIZE + D7A_FILE_FIRMWARE_VERSION_GIT_SHA1_SIZE)

#define D7A_FILE_FACTORY_SETTINGS_FILE_ID 0x01
#define D7A_FILE_FACTORY_SETTINGS_SIZE 13

#define D7A_FILE_ENGINEERING_MODE_FILE_ID 0x05
#define D7A_FILE_ENGINEERING_MODE_SIZE  9

#define D7A_FILE_VID_FILE_ID 0x06
#define D7A_FILE_VID_SIZE 3

#define D7A_FILE_DLL_CONF_FILE_ID  0x0A
#define D7A_FILE_DLL_CONF_SIZE    7

#define D7A_FILE_ACCESS_PROFILE_ID 0x20 // the first access class file
#define D7A_FILE_ACCESS_PROFILE_SIZE 65
#define D7A_FILE_ACCESS_PROFILE_COUNT 15

#define D7A_FILE_NWL_SECURITY   0x0D
#define D7A_FILE_NWL_SECURITY_SIZE  5

#define D7A_FILE_NWL_SECURITY_KEY   0x0E
#define D7A_FILE_NWL_SECURITY_KEY_SIZE  16

#define D7A_FILE_NWL_SECURITY_STATE_REG     0x0F
#define D7A_FILE_NWL_SECURITY_STATE_REG_SIZE  2 + (FRAMEWORK_FS_TRUSTED_NODE_TABLE_SIZE)*(D7A_FILE_NWL_SECURITY_SIZE + D7A_FILE_UID_SIZE)

#define D7AP_FS_SYSTEMFILES_COUNT 0x2F // reserved up until 0x3F but used only until 0x2F so use this for limiting memory usage
#define D7AP_FS_USERFILES_COUNT (FRAMEWORK_FS_FILE_COUNT - D7AP_FS_SYSTEMFILES_COUNT)


typedef struct __attribute__((__packed__))
{
    fs_storage_class_t storage_class : 2;
    uint8_t _rfu : 2;
    dae_act_condition_t action_condition : 3;
    bool action_protocol_enabled : 1;
} d7ap_fs_file_properties_t;

typedef struct __attribute__((__packed__))
{
    uint8_t file_permissions; // TODO not used for now
    d7ap_fs_file_properties_t file_properties;
    uint8_t action_file_id;
    uint8_t interface_file_id;
    uint32_t length;
    uint32_t allocated_length;
} d7ap_fs_file_header_t;

typedef enum {
  EM_OFF = 0,
  EM_CONTINUOUS_TX = 1,
  EM_TRANSIENT_TX = 2,
  EM_PER_RX = 3,
  EM_PER_TX = 4
} engineering_mode_t;

#endif
