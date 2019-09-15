#ifndef MODEM_INTERFACE_H
#define MODEM_INTERFACE_H


typedef enum {
    REBOOT_REASON_POR = 0,
    REBOOT_REASON_WDT = 1,
    REBOOT_REASON_SOFTWARE_REBOOT = 2,
    REBOOT_REASON_RESET_PIN = 3,
    REBOOT_REASON_OTHER = 254,
    REBOOT_REASON_NOT_IMPLEMENTED = 255,
} system_reboot_reason_t;


typedef enum
{
    SERIAL_MESSAGE_TYPE_ALP_DATA=0X01,
    SERIAL_MESSAGE_TYPE_PING_REQUEST=0X02,
    SERIAL_MESSAGE_TYPE_PING_RESPONSE=0X03,
    SERIAL_MESSAGE_TYPE_LOGGING=0X04,
    SERIAL_MESSAGE_TYPE_REBOOTED=0X05,
} serial_message_type_t;

typedef void (*target_rebooted_callback_t)(system_reboot_reason_t reboot_reason);

#endif
