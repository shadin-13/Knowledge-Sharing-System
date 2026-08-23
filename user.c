#include "user.h"

unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (unsigned char)c;
    }
    return hash;
}

const char* get_role_string(Role role) {
    switch (role) {
        case ROLE_STUDENT: return "Student";
        case ROLE_TEACHER: return "Teacher";
        case ROLE_ADMIN:   return "Admin";
        default:           return "Unknown";
    }
}

int is_valid_password(const char *pass) {
    if (strlen(pass) < 6) {
        return 0;
    }
    int has_special = 0;
    for (int i = 0; pass[i] != '\0'; i++) {
        if (!isalnum((unsigned char)pass[i])) {
            has_special = 1;
            break;
        }
    }
    return has_special;
}

void register_user(User **users, int *user_count, int *capacity) {
    if (*user_count >= *capacity) {
        int new_capacity = (*capacity > 0) ? (*capacity * 2) : 5;
        User *tmp = (User*)realloc(*users, (size_t)new_capacity * sizeof(User));
        if (!tmp) {
            printf("Error: Memory allocation failed.\n");
            return;
        }
        *users = tmp;
        *capacity = new_capacity;
    }

    User new_user;
    new_user.user_id = *user_count + 1;
    new_user.reputation = 0;

    printf("\n--- Register User ---\n");
    printf("Enter username: ");
    scanf("%49s", new_user.username);

    for (int i = 0; i < *user_count; i++) {
        if (strcmp((*users)[i].username, new_user.username) == 0) {
            printf("Error: Username already exists!\n");
            return;
        }
    }

    char plain_pass[50];
    while (1) {
        printf("Enter password (min 6 chars & at least 1 special char): ");
        scanf("%49s", plain_pass);

        if (is_valid_password(plain_pass)) {
            break;
        }
        printf("Error: Password must be at least 6 characters long AND contain at least one special character (!@#$%%^&* etc.)!\n");
    }

    new_user.password_hash = hash_djb2(plain_pass);

    int role_choice;
    printf("Select Role (0: Student, 1: Teacher): ");
    scanf("%d", &role_choice);
    new_user.role = (role_choice == 1) ? ROLE_TEACHER : ROLE_STUDENT;
    printf("Note: Admin accounts cannot be created through public registration.\n");

    printf("Enter Profile Picture URL/Path (or 'default.png'): ");
    scanf("%255s", new_user.profile_pic);

    (*users)[*user_count] = new_user;
    (*user_count)++;

    save_users(*users, *user_count);
    printf("User registered successfully!\n");
}

User* login_user(User *users, int user_count) {
    char username[50], password[50];
    printf("\n--- Login ---\n");
    printf("Username: ");
    scanf("%49s", username);
    printf("Password: ");
    scanf("%49s", password);

    unsigned long hashed = hash_djb2(password);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].password_hash == hashed) {
            printf("Login successful! Welcome, %s.\n", users[i].username);
            return &users[i];
        }
    }

    printf("Error: Invalid username or password.\n");
    return NULL;
}

void save_users(User *users, int user_count) {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) return;

    for (int i = 0; i < user_count; i++) {
        fprintf(fp, "%d|%s|%lu|%d|%d|%s\n",
                users[i].user_id, users[i].username, users[i].password_hash,
                users[i].role, users[i].reputation, users[i].profile_pic);
    }
    fclose(fp);
}

void load_users(User **users, int *user_count, int *capacity) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return;

    User temp;
    while (fscanf(fp, "%d|%49[^|]|%lu|%d|%d|%255[^\n]\n",
                  &temp.user_id, temp.username, &temp.password_hash,
                  (int*)&temp.role, &temp.reputation, temp.profile_pic) == 6) {
        if (*user_count >= *capacity) {
            int new_capacity = (*capacity > 0) ? (*capacity * 2) : 5;
            User *tmp = (User*)realloc(*users, (size_t)new_capacity * sizeof(User));
            if (!tmp) {
                printf("Error: Memory allocation failed while loading users.\n");
                break;
            }
            *users = tmp;
            *capacity = new_capacity;
        }
        (*users)[*user_count] = temp;
        (*user_count)++;
    }
    fclose(fp);
}

void admin_manage_users(User *users, int user_count) {
    printf("\n--- Admin Management: All Users ---\n");
    for (int i = 0; i < user_count; i++) {
        printf("ID: %d | Username: %s | Role: %s | Rep: %d | Pic: %s\n",
               users[i].user_id, users[i].username, get_role_string(users[i].role),
               users[i].reputation, users[i].profile_pic);
    }
}
