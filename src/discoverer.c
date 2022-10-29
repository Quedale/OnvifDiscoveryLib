#include "discoverer.h"
#include "generated/soapH.h"
// #include "soap_parser.h"
#include <pthread.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "onvif_discovery.h"

// #include "wsddapi.h"


#define PORT     3702 
#define MAXLINE 4096
char * ENVELOP = "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" xmlns:wsd=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">\n" \
          "              <soap:Header>\n" \
          "                  <wsa:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>\n" \
          "                  <wsa:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Resolve</wsa:Action>\n" \
          "                  <wsa:MessageID>urn:uuid:29206488-217f-4a1d-83c7-da156409aec8</wsa:MessageID>\n" \
          "              </soap:Header>\n" \
          "              <soap:Body>\n" \
          "                  <wsd:Resolve>\n" \
          "                      <wsa:EndpointReference>\n" \
          "                          <wsa:Address>urn:uuid:1c852a4d-b800-1f08-abcd-6cc21717ea2a</wsa:Address>\n" \
          "                      </wsa:EndpointReference>\n" \
          "                  </wsd:Resolve>\n" \
          "              </soap:Body>\n" \
          "          </soap:Envelope>";





void UdpDiscoverer__init(struct UdpDiscoverer* self, void * func, void * done_func) {
    self->done_callback = done_func;
    self->found_callback = func;
}

struct UdpDiscoverer UdpDiscoverer__create(void * func, void * done_func) {
    struct UdpDiscoverer result;
    memset (&result, 0, sizeof (result));
    UdpDiscoverer__init(&result,func, done_func);
    return result;
}


void UdpDiscoverer__reset(struct UdpDiscoverer* self) {
}

void UdpDiscoverer__destroy(struct UdpDiscoverer* self) {
  if (self) {
     UdpDiscoverer__reset(self);
     free(self);
  }
}

static gboolean * 
event_callback (void * e)
{
    printf("event callback\n");
    struct EventDispatch * in = (struct EventDispatch *) e;
    printf("creating disco evnt\n");
    DiscoveryEvent *event = (DiscoveryEvent *) in->data;
    printf("inside invoke : %s\n",event->server->matches[0].addr);
    
    g_mutex_lock (&in->lock);
    printf("invoking event\n");
    ((GSourceFunc) in->callback) (in->data);
    in->fired = TRUE;
    g_cond_signal (&in->cond);
    g_mutex_unlock (&in->lock);

    printf("cleanup \n");
    // void * data = in->data;
    // free(in->data);
    // free(in);
    return FALSE;
}

int discovery_event(void * e){
    printf("discovery_event \n");
    DiscoveryEvent *event = (DiscoveryEvent *) e;
    printf("created DiscoveryEvent\n");
    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) event->data;
    printf("created DiscoverThreadInput\n");
    //Define new event point to dispatch on main thread
    struct EventDispatch * evt_dispatch =  (struct EventDispatch *) malloc(sizeof(struct EventDispatch)); 
    printf("setting data\n");
    printf("discovery_event socpe count : %i\n",event->server->matches[0].scope_count);
    printf("discovery_event socpe entry 0 : %s\n",event->server->matches[0].scopes[0]);
    // evt_dispatch->data = in;
    printf("setting event callback\n");
    evt_dispatch->callback = in->callback;
    evt_dispatch->data = event;
    printf("invoke idle callback\n");
    printf("before invoke : %s\n",event->server->matches[0].addr);
    //Dispatch notification of completion

    g_mutex_init (&evt_dispatch->lock);
    g_cond_init (&evt_dispatch->cond);
    evt_dispatch->fired = FALSE;
    gdk_threads_add_idle((void *)event_callback,evt_dispatch);

    g_mutex_lock (&evt_dispatch->lock);
    while (!evt_dispatch->fired)
        g_cond_wait (&evt_dispatch->cond, &evt_dispatch->lock);
    g_mutex_unlock (&evt_dispatch->lock);

    g_mutex_clear (&evt_dispatch->lock);
    g_cond_clear (&evt_dispatch->cond);

    printf("after invoke : %s\n",event->server->matches[0].addr);
    return 0;
}

void *scan(void * vargp) {
    struct sockaddr_in     servaddr; 
    int sockfd; 
    char buffer[MAXLINE]; 
    int n, len; //Used by the socket

    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) vargp;
    printf("scan1 -- %p\n", (void *)in->data);
    printf("sending prob ...\n");
    sendProbe(in, discovery_event);

    printf("scan2 -- %p\n", (void *)in->data);
    DiscoveryEvent * ret_event =  (DiscoveryEvent *) malloc(sizeof(DiscoveryEvent)); 
    ret_event->data = in;
    ret_event->server = NULL;

    printf("creating evt dispatch\n");


    //Define new event point to dispatch on main thread
    struct EventDispatch * evt_dispatch =  (struct EventDispatch *) malloc(sizeof(struct EventDispatch)); 
    evt_dispatch->data = ret_event;
    evt_dispatch->callback = in->done_callback;

    // printf("adding idle cb\n");
    // g_mutex_init (&evt_dispatch->lock);
    // g_cond_init (&evt_dispatch->cond);
    // evt_dispatch->fired = FALSE;
    // //Dispatch notification of completion
    // gdk_threads_add_idle((void *)event_callback,evt_dispatch);
    // g_mutex_lock (&evt_dispatch->lock);
    // while (!evt_dispatch->fired)
    //     g_cond_wait (&evt_dispatch->cond, &evt_dispatch->lock);
    // g_mutex_unlock (&evt_dispatch->lock);

    // g_mutex_clear (&evt_dispatch->lock);
    // g_cond_clear (&evt_dispatch->cond);


    
    // struct soap* serv = soap_new1(SOAP_IO_UDP);
    // if (!soap_valid_socket(soap_bind(serv, NULL, 0, 1000)))
    // {
    //         soap_print_fault(serv, stderr);
    //         exit(1);
    // }
    // int res = soap_wsdd_Probe(serv, 
    //                             SOAP_WSDD_ADHOC, 
    //                             SOAP_WSDD_TO_TS,
    //                             "soap.udp://239.255.255.250:3702",
    //                             soap_wsa_rand_uuid(serv), 
    //                             NULL, 
    //                             NULL, 
    //                             NULL, 
    //                             "");
    // if (res != SOAP_OK)
    // {
    //         soap_print_fault(serv, stderr);
    //         exit(1);
    // }
    // soap_wsdd_listen(serv, 1);
    // soap_destroy(serv);
    // soap_end(serv);
    // soap_done(serv);

    // //Initialize memory
    // memset(&servaddr, 0, sizeof(servaddr)); 
    
    // //Cast to expected struct
    // struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) vargp;

    // // Creating socket file descriptor 
    // if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) { 
    //     perror("socket creation failed"); 
    //     exit(EXIT_FAILURE); 
    // }

    // //Set broacast setting
    // int broadcastEnable=1;
    // if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
    //     perror("Error");
    // }

    // //Set timeout setting
    // struct timeval tv;
    // tv.tv_sec = 2;
    // tv.tv_usec = 100000;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    //     perror("Error");
    // }
        
    // // Filling server information 
    // servaddr.sin_family = AF_INET; 
    // servaddr.sin_port = htons(PORT); 
    // servaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    
    // //Send envelop udp request
    // sendto(sockfd, (const char *)ENVELOP, strlen(ENVELOP), 
    //     MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
    //         sizeof(servaddr)); 

    // //Looping until timeout
    // do {

    //     //Define dedicated pointer to dispatch to the main thread
    //     DiscoveryEvent * ret_event =  (DiscoveryEvent *) malloc(sizeof(DiscoveryEvent)); 
    //     ret_event->player = in->data;

    //     //Reset buffer
    //     memset(buffer, 0, sizeof(buffer));

    //     //Reading response
    //     n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
    //                 MSG_WAITALL, (struct sockaddr *) &servaddr, 
    //                 &len); 

    //     if (n > 0){
    //         //Wrap buffer
    //         buffer[n] = '\0'; 
    //         printf("%s",buffer);
    //         //Extract struct and set pointer on event struct
    //         ret_event->server = parse_soap_msg(buffer);
    //         ret_event->widget = in->widget;

    //         struct EventDispatch * evt_dispatch =  (struct EventDispatch *) malloc(sizeof(struct EventDispatch)); 
    //         evt_dispatch->callback = in->callback;
    //         evt_dispatch->data = ret_event;
    //         //Dispatch notification about found server
    //         gdk_threads_add_idle((void *)event_callback,evt_dispatch);
    //     }

    // //Loop until timeout
    // } while (n > 0);

    // close(sockfd); 

    // DiscoveryEvent * ret_event =  (DiscoveryEvent *) malloc(sizeof(DiscoveryEvent)); 
    // ret_event->player = in->data;
    // ret_event->widget = in->widget;

    // //Define new event point to dispatch on main thread
    // struct EventDispatch * evt_dispatch =  (struct EventDispatch *) malloc(sizeof(struct EventDispatch)); 
    // evt_dispatch->data = ret_event;
    // evt_dispatch->callback = in->done_callback;

    // //Dispatch notification of completion
    // gdk_threads_add_idle((void *)event_callback,evt_dispatch);

    // //Clean up
    // free(in);
}

void * UdpDiscoverer__start(struct UdpDiscoverer* self, void * widget, void *player) {

    pthread_t thread_id;

    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) malloc(sizeof(struct DiscoverThreadInput));
    in->server = self;
    in->data = player;
    in->callback = self->found_callback;
    in->done_callback = self->done_callback;
    in->widget=widget;

    printf("UdpDiscoverer__start -- %p\n", (void *)player);
    pthread_create(&thread_id, NULL, scan, (void *)in);
}

//Place holder until I replace the legacy discoverer
// void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
// { printf("wsdd_event_Hello\n"); }

// void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
// { printf("wsdd_event_Bye\n"); }

// soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches)
// {
//     printf("wsdd_event_Probe\n");
//   return SOAP_WSDD_ADHOC;
// }

// void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches)
// { printf("wsdd_event_ProbeMatches\n"); }

// soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
// {
//     printf("wsdd_event_Resolve\n");
//   return SOAP_WSDD_ADHOC;
// }

// void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
// { printf("wsdd_event_ResolveMatches\n"); }