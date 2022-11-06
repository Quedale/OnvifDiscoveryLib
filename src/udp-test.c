#include "onvif_discovery.h"
#include <stddef.h>


int probe_callback(void * args){
    printf("prob callback...\n");
    DiscoveryEvent * e = (DiscoveryEvent *) args;
    DiscoveredServer * server = e->server;

    printf("MSG UUID : %s\n", server->msg_uuid);
    printf("PROBE UUID : %s\n", server->relate_uuid);

    int i;
    for(i=0;i<server->match_count;i++){
        struct ProbMatch match = server->matches[i];
        printf("\tAddress : %s\n",match.addr);
        printf("\tAddress UUID : %s\n",match.addr_uuid);
        printf("\tAddress PROBE UUID : %s\n",match.prob_uuid);
        printf("\tservice : %s\n",match.service);
        printf("\tTypes : %s\n",match.types);
        printf("\tVersion : %i\n",match.version);
        int a;
        for(a=0;a<match.scope_count;a++){
            char * scope = match.scopes[a];
            printf("\t\tScope : %s\n",scope);
        }
    }

}

int main(int argc, char** argv)
{


        printf("sending probe...\n");
        sendProbe(NULL,probe_callback);

        return 0;

}