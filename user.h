
#ifndef USER_H
#define USER_H

#define NAME_LEN 50
#define INITIAL_CAPACITY 10

// Enum for User Roles
typedef enum {
    ROLE_STUDENT = 1,
    ROLE_TEACHER,
    ROLE_ADMIN
} UserRole;

// User Structure
typedef struct {
    int id;
    char username[NAME_LEN];
    char password[NAME_LEN];
    int reputation;
    UserRole role;
} User;

// Dynamic Array Storage for Users
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

// Function Pointer Type for Sorting
typedef int (*CompareFunc)(const void *, const void *);
void sort_users(CompareFunc cmp);

#endif
