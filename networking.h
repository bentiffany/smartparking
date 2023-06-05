/*
 * networking.h
 *
 *  Created on: May 28, 2023
 *      Author: Ben
 */

#ifndef NETWORKING_H_
#define NETWORKING_H_

// Standard includes
#include <stdbool.h>
#include <stdio.h>

// Simplelink includes
#include "simplelink.h"

// header includes
#include "pin_mux_config.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"


// Constant definitions
#define MAX_URI_SIZE 128
#define URI_SIZE MAX_URI_SIZE + 1

#define SERVER_NAME                 "a3ht8copzioq2z-ats.iot.us-east-1.amazonaws.com"
#define AWS_DST_PORT                8443

#define SL_SSL_CA_CERT              "/cert/rootca.der"
#define SL_SSL_PRIVATE              "/cert/private.der"
#define SL_SSL_CLIENT               "/cert/client.der"

#define HTTP_MODE_TX                0
#define HTTP_MODE_RX                1

#define SHADOW_POSTHEADER           "POST /things/CC3200_SmartGarage/shadow HTTP/1.1\n\r"
#define SHADOW_GETHEADER            "GET /things/CC3200_SmartGarage/shadow HTTP/1.1\n\r"

#define HOSTHEADER                  "Host: a3ht8copzioq2z-ats.iot.us-east-1.amazonaws.com\r\n"
#define CHEADER                     "Connection: Keep-Alive\r\n"
#define CTHEADER                    "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1                   "Content-Length: "
#define CLHEADER2                   "\r\n\r\n"

// This has to be somewhat recent for TLS to work!
#define DATE                04    /* Current Date */
#define MONTH               6     /* Month 1-12 */
#define YEAR                2023  /* Current year */
#define HOUR                12    /* Time - hours */
#define MINUTE              0    /* Time - minutes */
#define SECOND              0     /* Time - seconds */


//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *);

//*****************************************************************************

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *);

//*****************************************************************************

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *, SlHttpServerResponse_t *);

//*****************************************************************************

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *);

//*****************************************************************************

void SimpleLinkSockEventHandler(SlSockEvent_t *);

//*****************************************************************************

long ConfigureSimpleLinkToDefaultState(void);

//*****************************************************************************

long WlanConnect(void);

//*****************************************************************************
int set_time(void);

//*****************************************************************************
int InitializeAppVariables(void);

//*****************************************************************************
int tls_connect(void);

//*****************************************************************************
long printErrConvenience(char *, long);

//*****************************************************************************
int connectToAccessPoint(void);

//*****************************************************************************
int http_post(char *, int);

//*****************************************************************************
int updateShadowStatus(const char *);

//*****************************************************************************
int getShadowCommand(void);

/**************************************************************************/


#endif /* NETWORKING_H_ */
