#ifndef __MODEM_H__
#define __MODEM_H__

#include "d7ap.h"
#include "lorawan_stack.h"
#include "alp.h"
#include "modem_interface.h"

// TODO for now we are assuming running on OSS-7, we can refactor later
// so it is more portable

typedef void (*modem_command_completed_callback_t)(bool with_error,uint8_t tag_id);
typedef void (*modem_return_file_data_callback_t)(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t* output_buffer);
typedef void (*modem_write_file_data_callback_t)(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t* output_buffer);
typedef void (*modem_interface_status_callback_t)(uint8_t interface_type, uint8_t* data);


typedef struct {
    modem_command_completed_callback_t command_completed_callback;
    modem_interface_status_callback_t modem_interface_status_callback;
    modem_return_file_data_callback_t return_file_data_callback;
    modem_write_file_data_callback_t write_file_data_callback;
    target_rebooted_callback_t modem_rebooted_callback;
} modem_callbacks_t;


#endif
