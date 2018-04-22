/*
1. We simply pull the thermistor data for each extruder
2. Compare to the target temperature
3. Manipulate the peltiers accordingly
*/

#include "PeltierControl.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"
#include "PeltierControlPool.h"
#include "Gcode.h"
#include "Config.h"
#include "ConfigValue.h"
#include "checksumm.h"
#include "mbed.h"
#include "DigitalOut.h"
#include "Thermistor.h"
#include "utils.h"
#include "PinNames.h"
#include "PublicDataRequest.h"
#include "PeltierControlPublicAccess.h"

#include <string>

using namespace std;

#define peltier_module_enable_control_checksum               CHECKSUM("peltier_moudle_enable_control")
//plus digital pin
#define plus_failsafe_checksum                               CHECKSUM("plus_failsafe_set_to")
#define plus_startup_state_checksum                          CHECKSUM("plus_startup_state")
#define plus_startup_value_checksum                          CHECKSUM("plus_startup_value")
#define plus_output_pin_checksum                             CHECKSUM("plus_output_pin")

//minus digital pin
#define minus_failsafe_checksum                               CHECKSUM("minus_failsafe_set_to")
#define minus_startup_state_checksum                          CHECKSUM("minus_startup_state")
#define minus_startup_value_checksum                          CHECKSUM("minus_startup_value")
#define minus_output_pin_checksum                             CHECKSUM("minus_output_pin")


PeltierControl::PeltierControl() {}

PeltierControl::PeltierControl(uint16_t name)
{
  this->name_checksum = name;
}

void PeltierControl::on_halt(void *arg)
{
  if(arg == nullptr){
    this->plus_digital_pin->set(this->plus_failsafe);

    this->plus_pin_state= this->plus_failsafe;
  }
}
void PeltierControl::on_module_loaded()
{
  this->plus_pin_changed = false;
  this->minus_pin_changed = false;

  this->register_for_event(ON_GCODE_RECEIVED);
  this->register_for_event(ON_MAIN_LOOP);
  this->register_for_event(ON_GET_PUBLIC_DATA);
  this->register_for_event(ON_SET_PUBLIC_DATA);
  this->register_for_event(ON_HALT);

  this->on_config_reload(this);
}

void PeltierControl::on_config_reload(void *argument)
{
  //General Config
  this->plus_pin_state = THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, plus_startup_state_checksum )->by_default(false)->as_bool();
  this->plus_failsafe= THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, plus_failsafe_checksum )->by_default(0)->as_number();
  this->minus_pin_state = THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, minus_startup_state_checksum )->by_default(false)->as_bool();
  this->minus_failsafe= THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, minus_failsafe_checksum )->by_default(0)->as_number();

  //Pin default config

  //Plus digital pin
  this->plus_digital_pin= new Pin();
  this->plus_digital_pin->from_string(THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, plus_output_pin_checksum )->by_default("nc")->as_string())->as_output();
  if(this->plus_digital_pin->connected()) {
      if(failsafe == 1) {
          set_high_on_debug(plus_digital_pin->port_number, plus_digital_pin->pin);
      }else{
          set_low_on_debug(plus_digital_pin->port_number, plus_digital_pin->pin);
      }
  }else{
      delete this->plus_digital_pin;
      this->plus_digital_pin= nullptr;
  }
  this->plus_digital_pin->set(this->plus_pin_state);
  //


  //Minus digital pin
  this->minus_digital_pin = new Pin();
  this->minus_digital_pin->from_string(THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, minus_output_pin_checksum )->by_default("nc")->as_string())->as_output();
  if(this->minus_digital_pin->connected()) {
      if(failsafe == 1) {
          set_high_on_debug(minus_digital_pin->port_number, minus_digital_pin->pin);
      }else{
          set_low_on_debug(minus_digital_pin->port_number, minus_digital_pin->pin);
      }
  }else{
      delete this->minus_digital_pin;
      this->minus_digital_pin = nullptr;
  }
  this->minus_digital_pin->set(this->minus_pin_state);
  //
}
void PeltierControl::on_gcode_received(void *argument)
{
  //peltier control cases

  if(tc.get_temperature() < tc.get_target_temperature())
  {
    //now peltier can be turned on or off.

  }
  else if(tc.get_temperature() > tc.get_target_temperature())
  {
    //now peltier can be turned on or off.

  }
}

void PeltierControl::on_get_public_data(void *argument)
{
  //plus pin
  PublicDataRequest *pdr_p = static_cast<PublicDataRequest *>(argument);

  if(!pdr_p->starts_with(peltier_control_checksum)) return;

  if(!pdr_p->second_element_is(this->name_checksum)) return;

  struct pad_peltier_control *pad_p = static_cast<struct pad_peltier_control *>(pdr_p->get_data_ptr());
  pad_p->name = this->name_checksum;
  pad_p->state = this->plus_pin_state;
  pad_p->value = this->plus_pin_value;
  pdr_p->set_taken();


  //minus pin
  PublicDataRequest *pdr_m = static_cast<PublicDataRequest *>(argument);

  if(!pdr_m->starts_with(peltier_control_checksum)) return;

  if(!pdr_m->second_element_is(this->name_checksum)) return;

  struct pad_peltier_control *pad_m = static_cast<struct pad_peltier_control *>(pdr_m->get_data_ptr());
  pad_m->name = this->name_checksum;
  pad_m->state = this->minus_pin_state;
  pad_m->value = this->minus_pin_value;
  pdr_m->set_taken();
}

void PeltierControl::on_set_public_data(void *argument)
{
  //plus pin
  PublicDataRequest *pdr_p = static_cast<PublicDataRequest *>(argument);

  if(!pdr_p->starts_with(peltier_control_checksum)) return;

  if(!pdr_p->second_element_is(this->name_checksum)) return;

  if(pdr_p->third_element_is(plus_state_checksum)) {
      bool t_p = *static_cast<bool *>(pdr_p->get_data_ptr());
      this->plus_pin_state = t_p;
      this->plus_pin_changed= true;
      pdr_p->set_taken();

  } else if(pdr_p->third_element_is(plus_value_checksum)) {
      float t_p = *static_cast<float *>(pdr_p->get_data_ptr());
      this->plus_pin_value = t_p;
      this->plus_pin_changed= true;
      pdr_p->set_taken();
  }

  //minus pin
  PublicDataRequest *pdr_m = static_cast<PublicDataRequest *>(argument);

  if(!pdr_m->starts_with(peltier_control_checksum)) return;

  if(!pdr_m->second_element_is(this->name_checksum)) return;

  if(pdr_m->third_element_is(minus_state_checksum)) {
      bool t_m = *static_cast<bool *>(pdr_m->get_data_ptr());
      this->minus_pin_state = t_m;
      this->minus_pin_changed= true;
      pdr_m->set_taken();

  } else if(pdr_m->third_element_is(minus_value_checksum)) {
      float t_m = *static_cast<float *>(pdr_m->get_data_ptr());
      this->minus_pin_value = t_m;
      this->minus_pin_changed= true;
      pdr_m->set_taken();
  }

}

void PeltierControl::on_main_loop(void *argument)
{
  //Plus pin
  if(this->plus_pin_changed) {
      if(this->plus_pin_state) {
              this->plus_digital_pin->set(true);
      } else {
              this->plus_digital_pin->set(false);
      }
      this->plus_pin_changed = false;
  }
  //Minus pin
  if(this->minus_pin_changed) {
      if(this->minus_pin_state) {
              this->minus_digital_pin->set(true);
      } else {
              this->minus_digital_pin->set(false);
      }
      this->minus_pin_changed = false;
  }
}
