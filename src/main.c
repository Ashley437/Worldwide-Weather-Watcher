#include <stdio.h>
#include "setup.h"
#include "cesi_types.h"
#include "status_led.h"
#include "grove_rgb.h"
#include "grove_button.h"
#include "grove_light.h"
#include "grove_rtc_ds1307.h"
#include "grove_gps_air530z.h"
#include "grove_bme680.h"
#include "sd_logger.h"

int main(void)
{
    Global_Init();

    StatusLed_Init();
    StatusLed_SetMode(SYS_MODE_STANDARD);

    /* StatusLed_SelfTest();  <- test du branchement, a retirer ensuite */

    while (1)
    {
        StatusLed_Update();   /* a appeler a chaque tour de boucle */

        /* ... lecture capteurs, RTC, GPS, SD, boutons ... */
    }
}
