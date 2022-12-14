#include "generated/soapH.h"
#include "wsddapi.h"
#include "generated/wsdd.nsmap"

#include "onvif_discovery.h"

struct MessageEntry {
  char * id;
  int (*cc)(void * );
  void * data;
};

struct MessageMapping {
    int count;
    struct MessageEntry **list;
};


int init;
struct MessageMapping * MAPPINGS;

//Place holder for successful client compile
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{ printf("wsdd_event_Hello\n"); }

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{ printf("wsdd_event_Bye\n"); }

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches)
{
  printf("wsdd_event_Probe\n");
  return SOAP_WSDD_ADHOC;
}


// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

int MessageMapping__get_index_of_msg(struct MessageMapping* self, const char * msgid)
{
  int i;
  for(i=0;i<self->count;i++){
    if(strcmp(self->list[i]->id,msgid) == 0){
      return i;
    }
  }
  return -1;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches)
{

  DiscoveredServer * server =  (DiscoveredServer *) malloc(sizeof(DiscoveredServer));
  server->match_count = ProbeMatches->__sizeProbeMatch;
  server->matches = (struct ProbMatch *) malloc (sizeof (struct ProbMatch) * server->match_count);
  server->msg_uuid = (char *) MessageID;
  server->relate_uuid = (char *) RelatesTo;

  // printf("wsdd_event_ProbeMatches\n"); 
  // printf("MessageID : %s\n",MessageID);
  // printf("RelatesTo : '%s'\n",RelatesTo);
  // printf("SequenceId : %s\n",SequenceId);

  int index = MessageMapping__get_index_of_msg(MAPPINGS,RelatesTo);
  if(index < 0){
    printf("error index \n");
    return;
  }

  struct MessageEntry * entry = MAPPINGS->list[index];
  // printf("LAST_MESSAGE : '%s'\n",entry->id);
  int i;
  for (i=0;i<ProbeMatches->__sizeProbeMatch;i++){
    struct ProbMatch ret_match;

    struct wsdd__ProbeMatchType match = ProbeMatches->ProbeMatch[i];

    ret_match.prob_uuid = malloc(strlen(RelatesTo) + 1);
    strcpy(ret_match.prob_uuid,(char *) RelatesTo);

    ret_match.addr_uuid = malloc(strlen(match.wsa__EndpointReference.Address) + 1);
    strcpy(ret_match.addr_uuid,match.wsa__EndpointReference.Address);

    ret_match.addr = malloc(strlen(match.XAddrs) + 1);
    strcpy(ret_match.addr,match.XAddrs);

    ret_match.types = malloc(strlen(match.Types) + 1);
    strcpy(ret_match.types,match.Types);

    ret_match.scopes = malloc (0);
    ret_match.scope_count = 0;
    // printf("ret_match.prob_uuid : %s\n",ret_match.prob_uuid);
    // printf("ret_match.addr_uuid : %s\n",ret_match.addr_uuid);
    // printf("ret_match.addr : %s\n",ret_match.addr);
    // printf("ret_match.types : %s\n",ret_match.types);
    // printf("ret_match.service : %s\n",ret_match.service);
    // printf("ret_match.version : %d\n",ret_match.version);
    // printf("match.Scopes->MatchBy : %s\n",match.Scopes->MatchBy);
    // printf("match.Scopes->__item : %s\n",match.Scopes->__item);
    
    char *tmp;
    ret_match.scope_count = 0;

    char *p = strtok ((char *)match.Scopes->__item, "\n");
    while (p != NULL){
      tmp=trimwhitespace(p);
      if(tmp[0] == '\0'){
        p = strtok(NULL,"\n");
        continue;
      }
      ret_match.scope_count++;
      ret_match.scopes = realloc (ret_match.scopes,sizeof (char *) * ret_match.scope_count);
      ret_match.scopes[ret_match.scope_count-1]= malloc(strlen(tmp)+1);
      strcpy(ret_match.scopes[ret_match.scope_count-1],tmp);
      p = strtok (NULL, "\n");
    }

    server->matches[i] = ret_match;

  }
  
  DiscoveryEvent ret_event;
  ret_event.data = entry->data;
  ret_event.server = server;
  entry->cc (&ret_event);
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    printf("wsdd_event_Resolve\n");
  return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{ printf("wsdd_event_ResolveMatches\n"); }


void MessageMapping__insert_element(struct MessageMapping* self,struct MessageEntry * msg, int index)
{ 
  int i;
  int count = self->count;
  self->list = (struct MessageEntry **) realloc (self->list,sizeof (struct MessageEntry*) * (count+1));
  for(i=self->count; i> index; i--){
      self->list[i] = self->list[i-1];
  }
  self->list[index]=msg;
  self->count++;
  return;
};

struct MessageEntry ** MessageMapping__remove_element_and_shift(struct MessageMapping* self, struct MessageEntry **array, int index, int array_length)
{
  int i;
  for(i = index; i < array_length; i++) {
      array[i] = array[i + 1];
  }
  return array;
};


void MessageMapping__remove_element(struct MessageMapping* self, int index){
  //Remove element and shift content
  self->list = MessageMapping__remove_element_and_shift(self,self->list, index, self->count);  /* First shift the elements, then reallocate */
  //Resize count
  self->count--;
  //Assign arythmatic
  int count = self->count;
  //Resize array memory
  self->list = realloc (self->list,sizeof(struct MessageEntry) * count);
  return;
};



const int TIMEOUT_VAL = 2;
void sendProbe(void * data, int (*cc)(void * )){
  
  struct soap * serv = soap_new1(SOAP_IO_UDP);
  if (!soap_valid_socket(soap_bind(serv, NULL, 0, 1000)))
  {
          soap_print_fault(serv, stderr);
          exit(1);
  }

  struct MessageEntry * msg = (struct MessageEntry *)malloc(sizeof(struct MessageEntry));
  msg->cc = cc;
  char * nid = (char *) soap_wsa_rand_uuid(serv);
  msg->id = malloc(strlen(nid) +1);
  strcpy(msg->id,nid);

  msg->data = data;
  if(!init){
    MAPPINGS = (struct MessageMapping *)malloc(sizeof(struct MessageMapping));
    MAPPINGS->count = 0;
    MAPPINGS->list=malloc(0);
  }

  MessageMapping__insert_element(MAPPINGS,msg,0);

  //Broadcast prob request
  int ret = soap_wsdd_Probe(serv,SOAP_WSDD_ADHOC,SOAP_WSDD_TO_TS,"soap.udp://239.255.255.250:3702", msg->id,NULL,NULL,NULL,NULL);
  if (ret != SOAP_OK){
    soap_print_fault(serv, stderr);
    printf("error sending prob...%i\n",ret);
  }

  //Listen for responses
  if (soap_wsdd_listen(serv, TIMEOUT_VAL) != SOAP_OK){
    soap_print_fault(serv, stderr);
    printf("error listening prob...\n");
  }

  soap_destroy(serv);
  soap_end(serv);
  soap_done(serv);

  //Pop message from mapping
  int index = MessageMapping__get_index_of_msg(MAPPINGS,msg->id);
  MessageMapping__remove_element(MAPPINGS,index);
  free(msg);

  return;
}

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

// C substring function definition
void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

void urldecode2(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

char * onvif_extract_scope(char * key, struct ProbMatch * match){
  int a;
  const char delimeter[2] = "/";
  const char * onvif_key_del = "onvif://www.onvif.org/";
  char* key_w_del;
  char* ret_val = "";
  int alloc = 0;
  //Concat key with delimeter
  key_w_del = malloc(strlen(key)+1+strlen(delimeter));
  strcpy(key_w_del, key);
  strcat(key_w_del, delimeter);
  printf("---------------SCOPES--------------\n");
  for (a = 0 ; a < match->scope_count ; ++a) {
    //Check for onvif key prefix
    printf("scope : %s\n",match->scopes[a]);
    if(startsWith(onvif_key_del, match->scopes[a])){
        
      //Drop onvif scope prefix
      char * subs = malloc(strlen(match->scopes[a])-strlen(onvif_key_del) + 1);
      substring(match->scopes[a],subs,strlen(onvif_key_del)+1,strlen(match->scopes[a])-strlen(onvif_key_del)+1);
      
      if(startsWith(key_w_del,subs)){ // Found Scope
          //Extract value
          char * sval = malloc(strlen(match->scopes[a])-strlen(onvif_key_del) + 1);
          substring(subs,sval,strlen(key_w_del)+1,strlen(subs)-strlen(key_w_del)+1);

          //Decode http string (e.g. %20 = whitespace)
          char *output = malloc(strlen(sval)+1);
          urldecode2(output, sval);

          if(!alloc){
            ret_val = malloc(strlen(output)+1);
            memcpy(ret_val,output,strlen(output)+1);
            alloc = 1;
          } else {
            ret_val = realloc(ret_val, strlen(ret_val) + strlen(output) +1);
            strcat(ret_val, " ");
            strcat(ret_val, output);
          }

          free(output);
          free(sval);
        }

        free(subs);
    } else {//TODO possibly handle 3rd-party scopes
        printf("\t\tScope : %s\n",match->scopes[a]);
    }
  }

  free(key_w_del);
  return ret_val;
}