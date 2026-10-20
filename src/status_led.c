/**
 ******************************************************************************
 * @file    status_led.c
 * @brief   LED d'état de la station météo (modes + codes d'erreur).
 *
 *  Couche logique au-dessus du pilote Grove fourni dans lib/cesi_grove :
 *  l'envoi des trames P9813 est fait par GroveRGB_SetColor() et les broches
 *  (PB10 = CIN, PA8 = DIN) sont définies dans include/board_pins.h.
 *  Ce fichier ne gère donc que le choix de la couleur et le clignotement.
 ******************************************************************************
 */
#include "status_led.h"
#include "grove_rgb.h"
#include "stm32l4xx_hal.h"

/* ============================ COULEURS (R,G,B) ============================= */
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color_t;

static const Color_t COL_OFF    = {   0,   0,   0 };
static const Color_t COL_RED    = { 255,   0,   0 };
static const Color_t COL_GREEN  = {   0, 255,   0 };
static const Color_t COL_BLUE   = {   0,   0, 255 };
static const Color_t COL_YELLOW = { 255, 160,   0 };  /* a ajuster a l'oeil */
static const Color_t COL_ORANGE = { 255,  40,   0 };  /* a ajuster a l'oeil */
static const Color_t COL_WHITE  = { 255, 255, 255 };

/* Couleur continue associee a chaque mode (ordre = enum SystemMode_t) */
static const Color_t *const s_modeColor[] =
{
    &COL_GREEN,   /* SYS_MODE_STANDARD      */
    &COL_YELLOW,  /* SYS_MODE_CONFIGURATION */
    &COL_BLUE,    /* SYS_MODE_ECONOMIQUE    */
    &COL_ORANGE   /* SYS_MODE_MAINTENANCE   */
};

/* ===================== MOTIFS DE CLIGNOTEMENT (1 Hz) ======================= */
/* Periode totale = 1000 ms (1 Hz). Le rouge est toujours affiche en premier. */
typedef struct
{
    const Color_t *other;   /* 2e couleur                  */
    uint16_t       redMs;   /* duree du rouge              */
    uint16_t       otherMs; /* duree de la 2e couleur      */
} BlinkPattern_t;

/* Ordre = enum SystemError_t, sans SYS_ERR_NONE (index = erreur - 1) */
static const BlinkPattern_t s_pattern[] =
{
    { &COL_BLUE,   500, 500 },  /* SYS_ERR_RTC           */
    { &COL_YELLOW, 500, 500 },  /* SYS_ERR_GPS           */
    { &COL_GREEN,  500, 500 },  /* SYS_ERR_SENSOR_ACCESS */
    { &COL_GREEN,  333, 667 },  /* SYS_ERR_SENSOR_DATA   (vert 2x plus long) */
    { &COL_WHITE,  500, 500 },  /* SYS_ERR_SD_FULL       */
    { &COL_WHITE,  333, 667 }   /* SYS_ERR_SD_WRITE      (blanc 2x plus long) */
};

/* ============================== ETAT INTERNE =============================== */
static volatile SystemMode_t  s_mode  = SYS_MODE_STANDARD;
static volatile SystemError_t s_error = SYS_ERR_NONE;
static Color_t s_lastColor;
static uint8_t s_hasLast = 0U;

/* N'envoie la couleur a la LED que si elle a change : une trame P9813 prend
 * quelques ms avec l'horloge du projet, inutile de la renvoyer a chaque tour. */
static void applyColor(const Color_t *c)
{
    if (s_hasLast &&
        s_lastColor.r == c->r && s_lastColor.g == c->g && s_lastColor.b == c->b)
    {
        return;
    }
    GroveRGB_SetColor(c->r, c->g, c->b);
    s_lastColor = *c;
    s_hasLast   = 1U;
}

/* ============================== API PUBLIQUE =============================== */
void StatusLed_Init(void)
{
    GroveRGB_Init();            /* configure PB10 / PA8 et eteint la LED */

    s_mode    = SYS_MODE_STANDARD;
    s_error   = SYS_ERR_NONE;
    s_hasLast = 0U;

    applyColor(&COL_OFF);
}

void StatusLed_SetMode(SystemMode_t mode)
{
    s_mode = mode;
}

void StatusLed_SetError(SystemError_t error)
{
    s_error = error;
}

void StatusLed_Update(void)
{
    SystemError_t err  = s_error;
    SystemMode_t  mode = s_mode;

    if (err == SYS_ERR_NONE)
    {
        applyColor(s_modeColor[mode]);
    }
    else
    {
        const BlinkPattern_t *p = &s_pattern[(uint32_t)err - 1U];
        uint32_t phase = HAL_GetTick() % (uint32_t)(p->redMs + p->otherMs);
        applyColor((phase < p->redMs) ? &COL_RED : p->other);
    }
}

/* ================================ AUTO-TEST ================================ */
static void hold(uint32_t ms)
{
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < ms)
    {
        StatusLed_Update();
    }
}

void StatusLed_SelfTest(void)
{
    SystemMode_t  savedMode  = s_mode;
    SystemError_t savedError = s_error;

    StatusLed_SetError(SYS_ERR_NONE);
    for (int m = SYS_MODE_STANDARD; m <= SYS_MODE_MAINTENANCE; m++)
    {
        StatusLed_SetMode((SystemMode_t)m);
        hold(4000U);
    }
    for (int e = SYS_ERR_RTC; e <= SYS_ERR_SD_WRITE; e++)
    {
        StatusLed_SetError((SystemError_t)e);
        hold(4000U);
    }

    s_mode  = savedMode;
    s_error = savedError;
}
