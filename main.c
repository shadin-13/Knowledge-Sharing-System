#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "qa.h"

int main() {
    int user_cap = 5, user_count = 0;
    User *users = (User*)malloc((size_t)user_cap * sizeof(User));

    int q_cap = 5, q_count = 0;
    Question *questions = (Question*)malloc((size_t)q_cap * sizeof(Question));

    int a_cap = 5, a_count = 0;
    Answer *answers = (Answer*)malloc((size_t)a_cap * sizeof(Answer));

    if (!users || !questions || !answers) {
        fprintf(stderr, "Error: Initial memory allocation failed.\n");
        free(users);
        free(questions);
        free(answers);
        return EXIT_FAILURE;
    }

    load_users(&users, &user_count, &user_cap);
    load_questions(&questions, &q_count, &q_cap);
    load_answers(&answers, &a_count, &a_cap);

    User *current_user = NULL;
    int choice;

    while (1) {
        printf("\n==========================================\n");
        printf("       KNOWLEDGE SHARING PLATFORM \n");
        printf("============================================\n");
        if (current_user) {
            printf("[Status: Logged in as %s (%s)]\n", current_user->username, get_role_string(current_user->role));
        } else {
            printf("[Status: Guest Mode / Not Logged In]\n");
        }
        printf("--------------------------------------------------\n");
        printf("1. Register User\n");
        printf("2. Login\n");
        printf("3. Logout\n");
        printf("4. Browse All Questions\n");
        printf("5. View Question Details & Answers\n");
        printf("6. Post Question\n");
        printf("7. Answer Question\n");
        printf("8. Upvote Answer\n");
        printf("9. Search Questions\n");
        printf("10. Leaderboard (qsort)\n");
        if (current_user && current_user->role == ROLE_ADMIN) {
            printf("11. Admin Panel (Manage Users)\n");
        }
        printf("0. Exit\n");
        printf("Select option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting...\n");
            break;
        }

        if (choice == 0) break;

        switch (choice) {
            case 1:
                if (current_user) {
                    printf("Please logout before registering another user.\n");
                } else {
                    register_user(&users, &user_count, &user_cap);
                }
                break;
            case 2:
                current_user = login_user(users, user_count);
                break;
            case 3:
                current_user = NULL;
                printf("Logged out successfully.\n");
                break;
            case 4:
                browse_questions(questions, q_count);
                break;
            case 5: {
                int q_id;
                printf("Enter Question ID: ");
                scanf("%d", &q_id);
                view_question_details(questions, q_count, answers, a_count, q_id);
                break;
            }
            case 6:
                if (!current_user) {
                    printf("Please login first to post a question.\n");
                } else {
                    post_question(&questions, &q_count, &q_cap, current_user->username);
                }
                break;
            case 7: {
                if (!current_user) {
                    printf("Please login first to answer a question.\n");
                } else {
                    int q_id;
                    printf("Enter Question ID to answer: ");
                    scanf("%d", &q_id);
                    post_answer(&answers, &a_count, &a_cap, questions, q_count, q_id, current_user->username);
                }
                break;
            }
            case 8: {
                int a_id;
                printf("Enter Answer ID to upvote: ");
                scanf("%d", &a_id);
                if (!current_user) {
                    printf("Please login first to upvote an answer.\n");
                } else {
                    upvote_answer(answers, a_count, users, user_count, a_id, current_user->username);
                }
                break;
            }
            case 9: {
                char kw[50];
                printf("Enter search keyword: ");
                scanf("%49s", kw);
                search_questions(questions, q_count, kw);
                break;
            }
            case 10:
                display_leaderboard(users, user_count);
                break;
            case 11:
                if (current_user && current_user->role == ROLE_ADMIN) {
                    admin_manage_users(users, user_count);
                } else {
                    printf("Unauthorized action!\n");
                }
                break;
            default:
                printf("Invalid choice!\n");
        }
    }

    free(users);
    free(questions);
    free(answers);
    printf("Goodbye!\n");
    return 0;
}
