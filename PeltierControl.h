#ifndef PELTIERCONTROL_H
#define PELTIERCONTROL_H

/*
switch.FANPWMminus.output_pin                1.30!            #pin out
switch.FANPWMplus.output_pin                 0.15!            #pin out

switch.Heater2minus.output_pin               0.16!            #pin out
switch.Heater2plus.output_pin                0.17!            #pin out

switch.bedminus.output_pin                   4.28!            #pin out
switch.bedplus.output_pin                    2.12!            #pin out
*/

#include "module.h"
#include "mbed.h"
#include "DigitalOut.h"
#include "TemperatureControl.h"
#include "PeltierControlPool.h"
#include "Pin.h"

class TemperatureControl;

class PeltierControl: public Module {
  public:
    PeltierControl();
    PeltierControl(uint16_t name);
    ~PeltierControl();

    void on_module_loaded();
    void on_main_loop(void *argument);
    void on_config_reload(void *argument);
    void on_gcode_received(void *argument);
    void on_get_public_data(void *argument);
    void on_set_public_data(void *argument);
    void on_halt(void *arg);

  private:

      Pin *plus_digital_pin;
      Pin *minus_digital_pin;

      float plus_pin_value;
      float minus_pin_value;

      TemperatureControl tc;

      struct{
        uint16_t  name_checksum;
        bool      plus_pin_changed:1;
        bool      minus_pin_changed:1;
        bool      plus_pin_state:1;
        bool      minus_pin_state:1;
        uint8_t   plus_failsafe:1;
        uint8_t   minus_failsafe:1;
      };
};

#endif
