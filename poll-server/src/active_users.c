#include "active_users.h"

#include <string.h>

int add_active_user(struct active_users * active_users, char * display_name, char * ip) {
    bool added;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (!active_users->users[i].occupied) {
            strcpy(active_users->users[i].display_name, display_name);
            strcpy(active_users->users[i].ip, ip);
            active_users->users[i].occupied = true;
            added = true;
        }
    }

    if (!added) {
        (void) fprintf(stderr, "adding active user: max connections exceeded\n");
        return -1;
    }

    return 0;
}

struct active_user * read_active_user_by_name(struct active_users * active_users, char * display_name) {
    struct active_user * user = NULL;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users->users[i].occupied && strcmp(active_users->users[i].display_name, display_name) == 0) {
            user = &active_users->users[i];
        }
    }

    return user;
}

struct active_user * read_active_user_by_ip(struct active_users * active_users, char * ip) {
    struct active_user * user = NULL;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users->users[i].occupied && strcmp(active_users->users[i].ip, ip) == 0) {
            user = &active_users->users[i];
        }
    }

    return user;
}

int update_active_user_name_by_name(struct active_users * active_users, char * new_display_name, char * old_display_name) {
    bool updated;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users->users[i].occupied && strcmp(active_users->users[i].display_name, old_display_name) == 0) {
            active_users->users[i].display_name[0] = '\0';
            strcpy(active_users->users[i].display_name, new_display_name);
            updated = true;
        }
    }

    if (!updated) {
        (void) fprintf(stderr, "removing active user: user with display name \"%s\" not found\n", old_display_name);
        return -1;
    }

    return 0;
}

int update_active_user_name_by_ip(struct active_users * active_users, char * new_display_name, char * ip) {
    bool updated;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users->users[i].occupied && strcmp(active_users->users[i].ip, ip) == 0) {
            active_users->users[i].display_name[0] = '\0';
            strcpy(active_users->users[i].display_name, new_display_name);
            updated = true;
        }
    }

    if (!updated) {
        (void) fprintf(stderr, "updating active user: user with IP address \"%s\" not found\n", ip);
        return -1;
    }

    return 0;
}

int delete_active_user_by_name(struct active_users * active_users, char * display_name) {
    bool removed;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users->users[i].occupied && strcmp(active_users->users[i].display_name, display_name) == 0) {
            active_users->users[i].display_name[0] = '\0';
            active_users->users[i].ip[0] = '\0';
            active_users->users[i].occupied = false;
            removed = true;
        }
    }

    if (!removed) {
        (void) fprintf(stderr, "deleting active user: user with display name \"%s\" not found\n", display_name);
        return -1;
    }

    return 0;
}

int delete_active_user_by_ip(struct active_users * active_users, char * ip) {
    bool removed;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users->users[i].occupied && strcmp(active_users->users[i].ip, ip) == 0) {
            active_users->users[i].display_name[0] = '\0';
            active_users->users[i].ip[0] = '\0';
            active_users->users[i].occupied = false;
            removed = true;
        }
    }

    if (!removed) {
        (void) fprintf(stderr, "deleting active user: user with IP address \"%s\" not found\n", ip);
        return -1;
    }

    return 0;
}
