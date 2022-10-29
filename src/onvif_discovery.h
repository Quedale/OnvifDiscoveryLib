#ifndef onvif_discovery_h__
#define onvif_discovery_h__
 
 struct ProbMatch {
    const char *prob_uuid;
    char *addr_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch
    char *addr; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/wsa:EndpointReference/wsa:Address
    char *types; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:Types
    int scope_count;
    char **scopes; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:Scopes
    char *service; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:XAddrs
    unsigned int version; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:MetadataVersion
};

typedef struct {
    char *msg_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Header/wsa:MessageID
    char *relate_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Header/wsa:RelatesTo
    struct ProbMatch *matches; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/
    int match_count;
} DiscoveredServer;

typedef struct {
    DiscoveredServer * server;
    void * data;

} DiscoveryEvent;

__attribute__ ((visibility("default"))) 
extern void sendProbe(void * data, int (*cc)(void * ));

__attribute__ ((visibility("default"))) 
extern char * onvif_extract_scope(char * key, struct ProbMatch * match);

#endif 