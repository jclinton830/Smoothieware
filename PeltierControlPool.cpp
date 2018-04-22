#include "libs/Module.h"
#include "libs/Kernel.h"
#include <math.h>
using namespace std;
#include <vector>
#include "PeltierControlPool.h"
#include "PeltierControl.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"


void PeltierControlPool::load_tools()
{
    vector<uint16_t> modules;
    THEKERNEL->config->get_module_list( &modules, peltier_control_checksum );


    for( unsigned int i = 0; i < modules.size(); i++ ) {
        // If module is enabled
        if( THEKERNEL->config->value(peltier_control_checksum, modules[i], enable_checksum )->as_bool() == true ) {
            PeltierControl *controller = new PeltierControl(modules[i]);
            THEKERNEL->add_module(controller);
        }
    }
}
