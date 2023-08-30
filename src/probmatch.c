#include "probmatch.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void ProbMatches__init(ProbMatches* self) {
    self->match_count = 0;
    self->matches = malloc(0);
}

ProbMatches* ProbMatches__create() {
    printf("ProbMatches__create...\n");
    ProbMatches* result = (ProbMatches*) malloc(sizeof(ProbMatches));
    ProbMatches__init(result);
    return result;
}

void ProbMatches__destroy(ProbMatches* self) {
    printf("ProbMatches__destroy...\n");
    if (self) {
        int i;
        for(i=0;i<self->match_count;i++){
            ProbMatch *match = self->matches[i];
            ProbMatch__destroy(match);
        }
        free(self->matches);
        free(self);
    }
}

void ProbMatches__insert_match(ProbMatches* self, ProbMatch* match){
    self->match_count++;
    self->matches = realloc (self->matches, sizeof (ProbMatch*) * self->match_count);
    self->matches[self->match_count-1]=match;
}

void ProbMatch__insert_scope(ProbMatch* self, char * scope){
    self->scope_count++;
    self->scopes = realloc (self->scopes,sizeof (char *) * self->scope_count);
    self->scopes[self->scope_count-1]= malloc(strlen(scope)+1);
    strcpy(self->scopes[self->scope_count-1],scope);
}

void ProbMatch__destroy(ProbMatch* self) {
    printf("ProbMatch__destroy...\n");
    if (self) {
        int i;
        for(i=0;i<self->scope_count;i++){
            free(self->scopes[i]);
        }
        free(self->prob_uuid);
        free(self->addr_uuid);
        for(i=0;i<self->addrs_count;i++){
            free(self->addrs[i]);
        }
        free(self->addrs);
        free(self->types);
        free(self->scopes);
        free(self->service);
        free(self);
    }
}

void ProbMatch__init(ProbMatch* self) {
    self->prob_uuid = malloc(0);
    self->addr_uuid = malloc(0);
    self->addrs = malloc(0);
    self->types = malloc(0);
    self->scope_count = 0;
    self->addrs_count = 0;
    self->scopes = malloc(0);
    self->service = malloc(0);
    self->version = -1;
}

ProbMatch* ProbMatch__create() {
    printf("ProbMatch__create...\n");
    ProbMatch* result = (ProbMatch*) malloc(sizeof(ProbMatch));
    ProbMatch__init(result);
    return result;
}

void ProbMatch__set_prob_uuid(ProbMatch* self, char * prob_uuid){
    self->prob_uuid = realloc(self->prob_uuid,strlen(prob_uuid) + 1);
    strcpy(self->prob_uuid,prob_uuid);
}

void ProbMatch__set_addr_uuid(ProbMatch* self, char * addr_uuid){
    self->addr_uuid = realloc(self->addr_uuid,strlen(addr_uuid) + 1);
    strcpy(self->addr_uuid,addr_uuid);
}

void ProbMatch__add_addr(ProbMatch* self, char * addr){
    self->addrs_count++;
    self->addrs = realloc (self->addrs, sizeof (char *) * self->addrs_count);
    self->addrs[self->addrs_count-1] = (char*) malloc(strlen(addr)+1);
    strcpy(self->addrs[self->addrs_count-1],addr);
}

void ProbMatch__set_types(ProbMatch* self, char * types){
    self->types = realloc(self->types,strlen(types) + 1);
    strcpy(self->types,types);
}

void ProbMatch__set_version(ProbMatch* self, int version){
    self->version = version;
}