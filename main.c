#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "qa.h"

static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    init_user_system();
    init_qa_system();

    load_users();
    load_questions();
    load_answers();

    int choice;
    while (1) {
        printf("\n======================================================\n");
        printf("   KNOWLEDGE SHARING PLATFORM (Admin, Hash, Images)   \n");
        printf("======================================================\n");

        UserRole current_role = 0;
        if (current_user_id != -1) {
            for (int i = 0; i < user_count; i++) {
                if (users[i].id == current_user_id) {
                    current_role = users[i].role;
                    const char *r_str = (current_role == ROLE_TEACHER) ? "Teacher" :
                                        (current_role == ROLE_ADMIN) ? "Admin" : "Student";
                    printf(" [Logged in: %s | Role: %s | Rep: %d]\n", users[i].username, r_str, users[i].reputation);
                    break;
                }
            }
        } else {
            printf(" [Status: Guest Mode]\n");
        }

        printf("------------------------------------------------------\n");
        printf("1. Register User\n2. Login\n3. Logout\n4. Browse All Questions\n");
        printf("5. View Question Details\n6. Post Question\n7. Answer Question\n");
        printf("8. Upvote Answer\n9. Search Questions\n10. Leaderboard (qsort)\n");
        
        if (current_role == ROLE_ADMIN) {
            printf("--- ADMIN CONTROLS ---\n");
            printf("11. [Admin] Delete User\n");
            printf("12. [Admin] Change User Role\n");
            printf("13. [Admin] Delete Question\n");
            printf("14. Exit\n");
        } else {
            printf("11. Exit\n");
        }
        
        printf("------------------------------------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        if (current_role == ROLE_ADMIN) {
            switch (choice) {
                case 1: register_user(); break;
                case 2: login_user(); break;
                case 3: logout_user(); break;
                case 4: list_all_questions(); break;
                case 5: view_question_details(); break;
                case 6: post_question(); break;
                case 7: answer_question(); break;
                case 8: upvote_answer(); break;
                case 9: search_questions(); break;
                case 10: view_leaderboard(); break;
                case 11: admin_delete_user(); break;
                case 12: admin_change_role(); break;
                case 13: admin_delete_question(); break;
                case 14:
                    cleanup_user_system();
                    cleanup_qa_system();
                    exit(0);
                default: printf("\nInvalid Choice!\n");
            }
        } else {
            switch (choice) {
                case 1: register_user(); break;
                case 2: login_user(); break;
                case 3: logout_user(); break;
                case 4: list_all_questions(); break;
                case 5: view_question_details(); break;
                case 6: post_question(); break;
                case 7: answer_question(); break;
                case 8: upvote_answer(); break;
                case 9: search_questions(); break;
                case 10: view_leaderboard(); break;
                case 11:
                    cleanup_user_system();
                    cleanup_qa_system();
                    exit(0);
                default: printf("\nInvalid Choice!\n");
            }
        }
    }
    return 0;
}
