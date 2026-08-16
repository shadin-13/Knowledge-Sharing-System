#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

User *users = NULL;
int user_count = 0;
int user_capacity = INITIAL_CAPACITY;
int current_user_id = -1;

static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void get_string_input(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

// djb2 Hashing Algorithm
unsigned long hash_password(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void init_user_system() {
    users = (User *)malloc(user_capacity * sizeof(User));
    if (!users) {
        perror("Memory allocation failed for users");
        exit(EXIT_FAILURE);
    }
}

void cleanup_user_system() {
    if (users) {
        free(users);
        users = NULL;
    }
}

static void ensure_user_capacity() {
    if (user_count >= user_capacity) {
        user_capacity *= 2;
        users = (User *)realloc(users, user_capacity * sizeof(User));
        if (!users) {
            perror("Reallocation failed for users");
            exit(EXIT_FAILURE);
        }
    }
}

void load_users() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return;

    user_count = 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        ensure_user_capacity();
        int role_int;
        sscanf(line, "%d;%[^;];%lu;%d;%d;%[^\n]\n",
               &users[user_count].id,
               users[user_count].username,
               &users[user_count].password_hash,
               &users[user_count].reputation,
               &role_int,
               users[user_count].profile_pic);
        users[user_count].role = (UserRole)role_int;
        user_count++;
    }
    fclose(fp);
}

void save_users() {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) return;

    for (int i = 0; i < user_count; i++) {
        fprintf(fp, "%d;%s;%lu;%d;%d;%s\n",
                users[i].id,
                users[i].username,
                users[i].password_hash,
                users[i].reputation,
                (int)users[i].role,
                users[i].profile_pic);
    }
    fclose(fp);
}

int find_user_by_name(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) return i;
    }
    return -1;
}

void register_user() {
    ensure_user_capacity();

    char username[NAME_LEN], plain_pass[PASS_LEN], pic_path[PATH_LEN];
    int role_choice;

    printf("\n--- User Registration ---\nEnter Username: ");
    get_string_input(username, NAME_LEN);

    if (find_user_by_name(username) != -1) {
        printf("\n[ERROR] Username already exists!\n");
        return;
    }

    printf("Enter Password (Min 5 chars): ");
    get_string_input(plain_pass, PASS_LEN);

    if (strlen(plain_pass) < 5) {
        printf("\n[ERROR] Password too short! Must be at least 5 characters.\n");
        return;
    }

    printf("Enter Profile Picture Path/FileName (or press Enter for 'default.png'): ");
    get_string_input(pic_path, PATH_LEN);
    if (strlen(pic_path) == 0) {
        strcpy(pic_path, "default.png");
    }

    printf("Select Role (1. Student, 2. Teacher, 3. Admin): ");
    if (scanf("%d", &role_choice) != 1 || role_choice < 1 || role_choice > 3) {
        role_choice = ROLE_STUDENT;
    }
    clear_input_buffer();

    users[user_count].id = user_count + 1;
    strcpy(users[user_count].username, username);
    users[user_count].password_hash = hash_password(plain_pass);
    users[user_count].reputation = 0;
    users[user_count].role = (UserRole)role_choice;
    strcpy(users[user_count].profile_pic, pic_path);
    user_count++;

    save_users();
    printf("\n[SUCCESS] Registered securely! Profile Image linked.\n");
}

void login_user() {
    char username[NAME_LEN], plain_pass[PASS_LEN];
    printf("\n--- User Login ---\nUsername: ");
    get_string_input(username, NAME_LEN);
    printf("Password: ");
    get_string_input(plain_pass, PASS_LEN);

    int idx = find_user_by_name(username);
    unsigned long input_hash = hash_password(plain_pass);

    if (idx != -1 && users[idx].password_hash == input_hash) {
        current_user_id = users[idx].id;
        const char *role_str = (users[idx].role == ROLE_TEACHER) ? "Teacher" :
                               (users[idx].role == ROLE_ADMIN) ? "Admin" : "Student";
        printf("\n[SUCCESS] Welcome, %s (%s)!\n[Avatar: %s]\n", 
               users[idx].username, role_str, users[idx].profile_pic);
    } else {
        printf("\n[ERROR] Invalid Username or Password!\n");
    }
}

void logout_user() {
    if (current_user_id != -1) {
        printf("\n[INFO] Logged out successfully.\n");
        current_user_id = -1;
    }
}

void admin_delete_user() {
    int cur_idx = -1;
    for(int i=0; i<user_count; i++) {
        if(users[i].id == current_user_id) { cur_idx = i; break; }
    }

    if (cur_idx == -1 || users[cur_idx].role != ROLE_ADMIN) {
        printf("\n[ACCESS DENIED] Admin privileges required!\n");
        return;
    }

    int target_id;
    printf("\n--- Admin: Delete User ---\nEnter User ID to Delete: ");
    if (scanf("%d", &target_id) != 1) { clear_input_buffer(); return; }
    clear_input_buffer();

    int target_idx = -1;
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == target_id) { target_idx = i; break; }
    }

    if (target_idx == -1) {
        printf("\n[ERROR] User not found!\n");
        return;
    }

    if (users[target_idx].id == current_user_id) {
        printf("\n[ERROR] Admin cannot delete themselves!\n");
        return;
    }

    for (int i = target_idx; i < user_count - 1; i++) {
        users[i] = users[i + 1];
    }
    user_count--;
    save_users();
    printf("\n[SUCCESS] User ID %d deleted by Admin.\n", target_id);
}

void admin_change_role() {
    int cur_idx = -1;
    for(int i=0; i<user_count; i++) {
        if(users[i].id == current_user_id) { cur_idx = i; break; }
    }

    if (cur_idx == -1 || users[cur_idx].role != ROLE_ADMIN) {
        printf("\n[ACCESS DENIED] Admin privileges required!\n");
        return;
    }

    int target_id, new_role;
    printf("\n--- Admin: Change User Role ---\nEnter User ID: ");
    if (scanf("%d", &target_id) != 1) { clear_input_buffer(); return; }
    printf("Enter New Role (1. Student, 2. Teacher, 3. Admin): ");
    if (scanf("%d", &new_role) != 1 || new_role < 1 || new_role > 3) { clear_input_buffer(); return; }
    clear_input_buffer();

    for (int i = 0; i < user_count; i++) {
        if (users[i].id == target_id) {
            users[i].role = (UserRole)new_role;
            save_users();
            printf("\n[SUCCESS] Role updated for User ID %d.\n", target_id);
            return;
        }
    }
    printf("\n[ERROR] User not found!\n");
}

static int compare_reputation(const void *a, const void *b) {
    User *userA = (User *)a;
    User *userB = (User *)b;
    return userB->reputation - userA->reputation;
}

void sort_users(CompareFunc cmp) {
    if (user_count > 1) {
        qsort(users, user_count, sizeof(User), cmp);
    }
}

void view_leaderboard() {
    if (user_count == 0) {
        printf("\n[INFO] No registered users.\n");
        return;
    }

    sort_users(compare_reputation);

    printf("\n--- Community Leaderboard (Ranked) ---\n");
    printf("%-5s | %-10s | %-15s | %-10s | %-10s | %-15s\n", "Rank", "User ID", "Username", "Role", "Reputation", "Profile Pic");
    printf("----------------------------------------------------------------------------------\n");
    for (int i = 0; i < user_count; i++) {
        const char *role_str = (users[i].role == ROLE_TEACHER) ? "Teacher" :
                               (users[i].role == ROLE_ADMIN) ? "Admin" : "Student";
        printf("%-5d | %-10d | %-15s | %-10s | %-10d | %-15s\n",
               i + 1, users[i].id, users[i].username, role_str, users[i].reputation, users[i].profile_pic);
    }
}
