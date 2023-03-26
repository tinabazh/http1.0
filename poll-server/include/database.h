#ifndef POLL_SERVER_DATABASE_H
#define POLL_SERVER_DATABASE_H

#include <objects.h>

int init_db(struct core_object * co);

void close_db(struct core_object * co);

void create_user(struct core_object * co);

int create_message(struct core_object * co, const char* uuid);

void create_channel(struct core_object * co, const char* uuid);

// TODO this one is a bit more complicated than the rest, reach out to Markus :)
int create_auth(struct core_object * co);

#endif //POLL_SERVER_DATABASE_H
