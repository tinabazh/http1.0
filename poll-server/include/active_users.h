#ifndef POLL_SERVER_ACTIVE_USERS_H
#define POLL_SERVER_ACTIVE_USERS_H

#include <objects.h>

/**
 * add_active_user
 * <p>
 * Adds a connection to the active users struct.
 * </p>
 * @param active_users the active users struct.
 * @param display_name the users display name.
 * @param ip the users IP address.
 * @return 0 on success, -1 on failure.
 */
int add_active_user(struct active_users * active_users, char * display_name, char * ip);

/**
 * read_active_user_by_name
 * <p>
 * Reads an active user using display name as an index. If no user is found NULL is returned.
 * </p>
 * @param active_users the active users struct.
 * @param display_name the users display name.
 * @return pointer to an active user struct if user is found, NULL if user is not found.
 */
struct active_user * read_active_user_by_name(struct active_users * active_users, char * display_name);

/**
 * read_active_user_by_ip
 * <p>
 * Reads an active user using IP address as an index. If no user is found NULL is returned.
 * </p>
 * @param active_users the active users struct.
 * @param ip the users IP address.
 * @return pointer to an active user struct if user is found, NULL if user is not found.
 */
struct active_user * read_active_user_by_ip(struct active_users * active_users, char * ip);

/**
 * update_active_user_name_by_name
 * <p>
 * Updates a users display name in the active users struct using display name as an index.
 * </p>
 * @param active_users the active users struct.
 * @param new_display_name the new display name.
 * @param old_display_name the old display name.
 * @return 0 on success, -1 on failure.
 */
int update_active_user_name_by_name(struct active_users * active_users, char * new_display_name, char * old_display_name);

/**
 * update_active_user_name_by_ip
 * <p>
 * Updates a users display name in the active users struct using IP address as an index.
 * </p>
 * @param active_users the active users struct.
 * @param new_display_name the users new display name.
 * @param ip the users IP address.
 * @return 0 on success, -1 on failure.
 */
int update_active_user_name_by_ip(struct active_users * active_users, char * new_display_name, char * ip);

/**
 * delete_active_user_by_name
 * <p>
 * Deletes a connection from the active users struct using display name as an index.
 * </p>
 * @param active_users the active users struct.
 * @param display_name the users display name.
 * @return 0 on success, -1 on failure.
 */
int delete_active_user_by_name(struct active_users * active_users, char * display_name);


/**
 * delete_active_user_by_ip
 * <p>
 * Deletes a connection from the active users struct using IP address as an index.
 * </p>
 * @param active_users the active users struct.
 * @param ip the users IP address.
 * @return 0 on success, -1 on failure.
 */
int delete_active_user_by_ip(struct active_users * active_users, char * ip);

#endif //POLL_SERVER_ACTIVE_USERS_H
