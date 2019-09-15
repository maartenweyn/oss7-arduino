#include "dash7_modem.h"

D7_modem* modem;

void command_completed_cb(bool with_error,  uint8_t tag_id) {
  SerialUSB.println("command completed!");
}

void return_file_data_cb(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t* buffer) {
  if(file_id == D7A_FILE_UID_FILE_ID && size == D7A_FILE_UID_SIZE) {
    SerialUSB.print("received modem uid: ");
    for (int i=0;i<D7A_FILE_UID_SIZE;i++)
      SerialUSB.print(buffer[0], HEX);

    SerialUSB.println();
  }
}

void modem_rebooted_cb(system_reboot_reason_t reboot_reason) {
  SerialUSB.print("modem rebooted, reason: ");
  SerialUSB.println(reboot_reason);
}

modem_callbacks_t callbacks = (modem_callbacks_t){
  .command_completed_callback = &command_completed_cb,
  .modem_interface_status_callback = NULL,
  .return_file_data_callback = &return_file_data_cb,
  .write_file_data_callback = NULL,
  .modem_rebooted_callback = &modem_rebooted_cb
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialUSB.begin(115200);
  modem = new D7_modem();
}

void loop() {

  modem->process_rx();

}
