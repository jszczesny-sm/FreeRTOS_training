#include <drivers/injection/injector.h>
#include <stdio.h>

int injector_init(injector_driver* drv)
{
    bool ret = -1;
    if (!drv->is_initialized) {
        ret = drv->functions->initialize(drv->prvData);
        if (!ret) {
            drv->is_initialized = true;
            drv->is_running = false;
        } else {
            printf("injector [%s] initialization error\n",drv->device_name);
        }
    } else {
        printf("injector [%s] aleady initialized\n",drv->device_name);
    }
    return ret;
}

int injector_execute(injector_driver* drv, injector_action action)
{
    bool ret = 0;
    
    switch (action)
    {
    case INJECTOR_START:
        if (!drv->is_running) {
            drv->functions->start(drv->prvData);
            drv->is_running = true;
        }
        break;
    case INJECTOR_STOP:
        if (drv->is_running) {
            drv->functions->stop(drv->prvData);
            drv->is_running = false;
        }
        break;
    default:
        break;
    }

    return ret;
}

int injector_set_parameter(injector_driver* drv, injector_config* param)
{
    return drv->functions->set_parameter(param,drv->prvData);
}

int injector_get_parameter(injector_driver* drv, injector_config* param)
{
    return drv->functions->get_parameter(param,drv->prvData);
}