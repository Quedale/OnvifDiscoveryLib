#include "discoverer.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif
#include <pthread.h>
#include <stdio.h>
#include "onvif_discovery.h"
#include "clogger.h"

struct DiscoverThreadInput {
    struct UdpDiscoverer * server;
    int retry_count;
    int timeout;
    void * user_data;
    void (*callback)(DiscoveryEvent *);
    void (*done_callback)(DiscoveryEvent *);
};

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

void UdpDiscoverer__destroy(struct UdpDiscoverer* self) {
  if (self) {
     free(self);
  }
}

int discovery_event(DiscoveryEvent * event){
    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) event->data;

    //Remap user_data from thread struct to original user_data
    event->data = in->user_data;

    (in->callback) (event);

    return 0;
}

void * start_discovery(void * vargp) {

    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) vargp;
    for(int i=0;i<in->retry_count;i++){
        sendProbe(in, in->timeout, discovery_event);
    }

    //Dispatch notification of completion
    DiscoveryEvent * ret_event = DiscoveryEvent__create(NULL,in->user_data);
    (in->done_callback) (ret_event);

    free(in);
    return NULL;
}

void UdpDiscoverer__start(struct UdpDiscoverer* self, void *user_data, int retry_count, int timeout) {

    pthread_t thread_id;

    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) malloc(sizeof(struct DiscoverThreadInput));
    in->server = self;
    in->user_data = user_data;
    in->callback = self->found_callback;
    in->done_callback = self->done_callback;
    in->retry_count = retry_count;
    in->timeout = timeout;

    pthread_create(&thread_id, NULL, start_discovery, (void *)in);
    pthread_detach(thread_id);
}
