/**
 ******************************************************************************
 * @file    status_led.h
 * @brief   Pilote de la LED d'état (Grove Chainable RGB LED v2.0 / P9813)
 *          pour la station météo - STM32 Nucleo L476RG (s appuie sur grove_rgb)
 *
 *  Modes et codes erreur conformes au document "MODE DE FONCTIONNEMENT" CESI.
 ******************************************************************************
 */
#ifndef STATUS_LED_H
#define STATUS_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Modes de fonctionnement (couleur continue) */
typedef enum
{
    SYS_MODE_STANDARD = 0,   /* LED verte continue  */
    SYS_MODE_CONFIGURATION,  /* LED jaune continue  */
    SYS_MODE_ECONOMIQUE,     /* LED bleue continue  */
    SYS_MODE_MAINTENANCE     /* LED orange continue */
} SystemMode_t;

/* Erreurs (clignotement 1 Hz, rouge + une 2e couleur) */
typedef enum
{
    SYS_ERR_NONE = 0,        /* Aucune erreur : la LED affiche le mode          */
    SYS_ERR_RTC,             /* Rouge/Bleu    (50%/50%)  : erreur horloge RTC   */
    SYS_ERR_GPS,             /* Rouge/Jaune   (50%/50%)  : erreur donnees GPS   */
    SYS_ERR_SENSOR_ACCESS,   /* Rouge/Vert    (50%/50%)  : erreur acces capteur */
    SYS_ERR_SENSOR_DATA,     /* Rouge/Vert    (1/3-2/3)  : donnees incoherentes */
    SYS_ERR_SD_FULL,         /* Rouge/Blanc   (50%/50%)  : carte SD pleine      */
    SYS_ERR_SD_WRITE         /* Rouge/Blanc   (1/3-2/3)  : erreur acces/ecriture SD */
} SystemError_t;

/** Initialise les GPIO et eteint la LED. A appeler une fois au demarrage. */
void StatusLed_Init(void);

/** Definit le mode courant (affiche si aucune erreur n'est active). */
void StatusLed_SetMode(SystemMode_t mode);

/** Definit l'erreur courante (SYS_ERR_NONE pour l'effacer). Prioritaire sur le mode. */
void StatusLed_SetError(SystemError_t error);

/** Met a jour la LED (non bloquant). A appeler le plus souvent possible
 *  dans la boucle principale (au moins toutes les ~50 ms). */
void StatusLed_Update(void);

/** Test BLOQUANT : affiche chaque mode puis chaque erreur pendant 4 s.
 *  Utile pour valider le branchement, a ne pas laisser dans le code final. */
void StatusLed_SelfTest(void);

#ifdef __cplusplus
}
#endif

#endif /* STATUS_LED_H */
