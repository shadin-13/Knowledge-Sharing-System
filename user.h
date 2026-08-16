#ifndef USER_H
#define USER_H

#define NAME_LEN 50
#define PASS_LEN 64
#define PATH_LEN 100
#define INITIAL_CAPACITY 10

typedef enum {
    ROLE_STUDENT = 1,
    ROLE_TEACHER,
    ROLE_ADMIN
} UserRole;

typedef struct {
    int id;
    char username[NAME_LEN];
    unsigned long password_hash;
    int reputation;
    UserRole role;
    char profile_pic[PATH_LEN]; // Profile Picture Path
} User;

extern User *users;
extern int user_count;
extern int user_capacity;
extern int current_user_id;

void init_user_system();
void cleanup_user_system();
void load_users();
void save_users();

int find_user_by_name(const char *username);
void register_user();
void login_user();
void logout_user();
void view_leaderboard();

// Admin Functions
void admin_delete_user();
void admin_change_role();

typedef int (*CompareFunc)(const void *, const void *);
void sort_users(CompareFunc cmp);

// Password Hashing Helper
unsigned long hash_password(const char *str);

#endif
