#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    ROLE_STUDENT = 0,
    ROLE_TEACHER,
    ROLE_ADMIN
} Role;

typedef struct {
    int user_id;
    char username[50];
    unsigned long password_hash;
    Role role;
    int reputation;
    char profile_pic[256];
} User;

unsigned long hash_djb2(const char *str);
const char* get_role_string(Role role);
int is_valid_password(const char *pass);
void register_user(User **users, int *user_count, int *capacity);
User* login_user(User *users, int user_count);
void save_users(User *users, int user_count);
void load_users(User **users, int *user_count, int *capacity);
void admin_manage_users(User *users, int user_count);

#endif
