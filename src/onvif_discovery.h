#ifndef onvif_discovery_h__
#define onvif_discovery_h__

#include "probmatch.h" 

typedef struct {
    char *msg_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Header/wsa:MessageID
    char *relate_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Header/wsa:RelatesTo
    ProbMatches *matches; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/
} DiscoveredServer;

typedef struct {
    DiscoveredServer * server;
    void * data;

} DiscoveryEvent;

__attribute__ ((visibility("default"))) 
extern void sendProbe(void * data, int timeout, int (*cc)(void * ));

__attribute__ ((visibility("default"))) 
extern char * onvif_extract_scope(char * key, ProbMatch * match);

#endif 