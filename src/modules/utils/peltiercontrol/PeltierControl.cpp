/*
1. We simply pull the thermistor data for each extruder
2. Compare to the target temperature
3. Manipulate the peltiers accordingly
*/

#include "PeltierControl.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"

#include "Gcode.h"
#include "Config.h"
#include "ConfigValue.h"
#include "checksumm.h"
#include "mbed.h"
#include "DigitalOut.h"
#include "Thermistor.h"
#include "utils.h"

#include <string>

using namespace std;

#define peltier_moudle_enable_control_checksum      CHECKSUM("peltier_moudle_enable_control")
#define plus_sig_checksum                           CHECKSUM("plus_sig")
#define minus_sig_checksum                          CHECKSUM("minus_sig")

PeltierControl::PeltierControl()
{
  plus_sig(pin);
  minus_sig(pin);
  sensor = nullptr;
}

void PeltierControl::on_module_loaded()
{
  this->load_config();
  this->register_for_event(ON_GCODE_RECEIVED);
}

void PeltierControl::load_config()
{
  //General Config

  //Pin default config
  this->plus_sig = THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, plus_sig)->by_default("nc")->as_string();
  this->plus_sig = THEKERNEL->config->value(peltier_control_checksum, this->name_checksum, minus_sig)->by_default("nc")->as_string();
}
void PeltierControl::on_gcode_received(void *argument)
{
  //peltier control cases

  if(TemperatureControl::get_temperature() < TemperatureControl::get_target_temperature())
  {
    //now peltier can be turned on or off.
    this->plus_sig.write(1);
    this->minus_sig.write(0);

  }
  else if(TemperatureControl::get_temperature() > TemperatureControl::get_target_temperature())
  {
    //now peltier can be turned on or off.
    this->plus_sig.write(0);
    this->minus_sig.write(1);
  }
}
