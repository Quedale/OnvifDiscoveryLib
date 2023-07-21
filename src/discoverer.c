#include "discoverer.h"
#include "generated/soapH.h"
#include <pthread.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "onvif_discovery.h"

struct DiscoverThreadInput {
    struct UdpDiscoverer * server;
    int retry_count;
    int timeout;
    void * user_data;
    void * callback;
    void * done_callback;
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


void UdpDiscoverer__reset(struct UdpDiscoverer* self) {
}

void UdpDiscoverer__destroy(struct UdpDiscoverer* self) {
  if (self) {
     UdpDiscoverer__reset(self);
     free(self);
  }
}

static gboolean * 
main_thread_dispatch (void * e)
{
    printf("GTK Thread Dispatch...\n");
    struct EventDispatch * in = (struct EventDispatch *) e;
    DiscoveryEvent *event = (DiscoveryEvent *) in->data;

    g_mutex_lock (&in->lock);
    ((GSourceFunc) in->callback) (event);
    in->fired = TRUE;
    g_cond_signal (&in->cond);
    g_mutex_unlock (&in->lock);

    printf("GTK Thread Dispatch Done...\n");

    return FALSE;
}

int discovery_event(void * e){

    DiscoveryEvent *event = (DiscoveryEvent *) e;
    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) event->data;
    //Define new event point to dispatch on main thread
    struct EventDispatch * evt_dispatch =  (struct EventDispatch *) malloc(sizeof(struct EventDispatch)); //freed after event dispatched [event_callback]

    //Remap user_data from thread struct to original user_data
    event->data = in->user_data;

    evt_dispatch->callback = in->callback;
    evt_dispatch->data = event;
    
    //Dispatch discovery on GUI thread
    g_mutex_init (&evt_dispatch->lock);
    g_cond_init (&evt_dispatch->cond);
    evt_dispatch->fired = FALSE;
    gdk_threads_add_idle((void *)main_thread_dispatch,evt_dispatch);

    g_mutex_lock (&evt_dispatch->lock);
    while (!evt_dispatch->fired)
        g_cond_wait (&evt_dispatch->cond, &evt_dispatch->lock);
    g_mutex_unlock (&evt_dispatch->lock);

    g_mutex_clear (&evt_dispatch->lock);
    g_cond_clear (&evt_dispatch->cond);

    free(evt_dispatch);
    return 0;
}

void * start_discovery(void * vargp) {

    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) vargp;
    for(int i=0;i<in->retry_count;i++){
        sendProbe(in, in->timeout, discovery_event);
    }

    // //Dispatch notification of completion
    DiscoveryEvent * ret_event =  (DiscoveryEvent *) malloc(sizeof(DiscoveryEvent)); 
    ret_event->data = in->user_data;
    ret_event->server = NULL;

    struct EventDispatch * evt_dispatch =  (struct EventDispatch *) malloc(sizeof(struct EventDispatch)); 
    evt_dispatch->data = ret_event;
    evt_dispatch->callback = in->done_callback;

    g_mutex_init (&evt_dispatch->lock);
    g_cond_init (&evt_dispatch->cond);
    evt_dispatch->fired = FALSE;
    gdk_threads_add_idle((void *)main_thread_dispatch,evt_dispatch);
    g_mutex_lock (&evt_dispatch->lock);
    while (!evt_dispatch->fired)
        g_cond_wait (&evt_dispatch->cond, &evt_dispatch->lock);
    g_mutex_unlock (&evt_dispatch->lock);

    g_mutex_clear (&evt_dispatch->lock);
    g_cond_clear (&evt_dispatch->cond);

    free(ret_event);
    free(evt_dispatch);
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
