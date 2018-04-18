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
#include "DigitalOut.h"
class PeltierControl: public Module {
  public:
    PeltierControl();
    virtual ~PeltierControl();

    void on_module_loaded();
    void load_config();
    void on_gcode_received(void *);

  private:

      //DigitalOut relay;
      DigitalOut plus_sig;
      DigitalOut minus_sig;
};
#endif
