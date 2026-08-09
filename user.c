#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

User *users = NULL;
int user_count = 0;
int user_capacity = INITIAL_CAPACITY;
int current_user_id = -1;

static void get_string_input(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

// Dynamic Memory Initialization
void init_user_system() {
    users = (User *)malloc(user_capacity * sizeof(User));
    if (!users) {
        perror("Memory allocation failed for users");
        exit(EXIT_FAILURE);
    }
}

// Memory Cleanup
void cleanup_user_system() {
    if (users) {
        free(users);
        users = NULL;
    }
}

// Automatic Reallocation using realloc
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
    User temp;
    int role_int;
    while (fscanf(fp, "%d %s %s %d %d", &temp.id, temp.username, temp.password, &temp.reputation, &role_int) != EOF) {
        ensure_user_capacity();
        temp.role = (UserRole)role_int;
        users[user_count++] = temp;
    }
    fclose(fp);
}

void save_users() {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) return;

    for (int i = 0; i < user_count; i++) {
        fprintf(fp, "%d %s %s %d %d\n",
                users[i].id,
                users[i].username,
                users[i].password,
                users[i].reputation,
                (int)users[i].role);
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

    char username[NAME_LEN], password[NAME_LEN];
    int role_choice;

    printf("\n--- User Registration ---\nEnter Username: ");
    get_string_input(username, NAME_LEN);

    if (find_user_by_name(username) != -1) {
        printf("\n[ERROR] Username already exists!\n");
        return;
    }

    printf("Enter Password: ");
    get_string_input(password, NAME_LEN);

    printf("Select Role (1. Student, 2. Teacher, 3. Admin): ");
    if (scanf("%d", &role_choice) != 1 || role_choice < 1 || role_choice > 3) {
        role_choice = ROLE_STUDENT;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF);

    users[user_count].id = user_count + 1;
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    users[user_count].reputation = 0;
    users[user_count].role = (UserRole)role_choice;
    user_count++;

    save_users();
    printf("\n[SUCCESS] Registered & saved dynamically to memory!\n");
}

void login_user() {
    char username[NAME_LEN], password[NAME_LEN];
    printf("\n--- User Login ---\nUsername: ");
    get_string_input(username, NAME_LEN);
    printf("Password: ");
    get_string_input(password, NAME_LEN);

    int idx = find_user_by_name(username);
    if (idx != -1 && strcmp(users[idx].password, password) == 0) {
        current_user_id = users[idx].id;
        const char *role_str = (users[idx].role == ROLE_TEACHER) ? "Teacher" :
                               (users[idx].role == ROLE_ADMIN) ? "Admin" : "Student";
        printf("\n[SUCCESS] Welcome, %s (%s)!\n", users[idx].username, role_str);
    } else {
        printf("\n[ERROR] Invalid Credentials!\n");
    }
}

void logout_user() {
    if (current_user_id != -1) {
        printf("\n[INFO] Logged out successfully.\n");
        current_user_id = -1;
    }
}

// Comparator Function for qsort
static int compare_reputation(const void *a, const void *b) {
    User *userA = (User *)a;
    User *userB = (User *)b;
    return userB->reputation - userA->reputation; // Descending Order
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

    // Sort using Function Pointer & qsort
    sort_users(compare_reputation);

    printf("\n--- Community Leaderboard (Ranked) ---\n");
    printf("%-5s | %-10s | %-18s | %-10s | %-10s\n", "Rank", "User ID", "Username", "Role", "Reputation");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < user_count; i++) {
        const char *role_str = (users[i].role == ROLE_TEACHER) ? "Teacher" :
                               (users[i].role == ROLE_ADMIN) ? "Admin" : "Student";
        printf("%-5d | %-10d | %-18s | %-10s | %-10d\n",
               i + 1, users[i].id, users[i].username, role_str, users[i].reputation);
    }
}
