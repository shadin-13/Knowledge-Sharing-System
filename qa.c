#include "qa.h"

void post_question(Question **questions, int *q_count, int *q_cap, const char *username) {
    if (*q_count >= *q_cap) {
        int new_capacity = (*q_cap > 0) ? (*q_cap * 2) : 5;
        Question *tmp = (Question*)realloc(*questions, (size_t)new_capacity * sizeof(Question));
        if (!tmp) {
            printf("Error: Memory allocation failed.\n");
            return;
        }
        *questions = tmp;
        *q_cap = new_capacity;
    }

    Question new_q;
    new_q.question_id = *q_count + 1;
    strcpy(new_q.author_username, username);

    while (getchar() != '\n');
    printf("\nEnter Question Title: ");
    fgets(new_q.title, sizeof(new_q.title), stdin);
    new_q.title[strcspn(new_q.title, "\n")] = 0;

    printf("Enter Category/Tag: ");
    fgets(new_q.category, sizeof(new_q.category), stdin);
    new_q.category[strcspn(new_q.category, "\n")] = 0;

    (*questions)[*q_count] = new_q;
    (*q_count)++;

    save_questions(*questions, *q_count);
    printf("Question posted successfully!\n");
}

void post_answer(Answer **answers, int *a_count, int *a_cap, Question *questions, int q_count, int q_id, const char *username) {
    int question_exists = 0;
    for (int i = 0; i < q_count; i++) {
        if (questions[i].question_id == q_id) {
            question_exists = 1;
            break;
        }
    }

    if (!question_exists) {
        printf("Error: Question ID %d does not exist. Answer was not posted.\n", q_id);
        return;
    }

    if (*a_count >= *a_cap) {
        int new_capacity = (*a_cap > 0) ? (*a_cap * 2) : 5;
        Answer *tmp = (Answer*)realloc(*answers, (size_t)new_capacity * sizeof(Answer));
        if (!tmp) {
            printf("Error: Memory allocation failed.\n");
            return;
        }
        *answers = tmp;
        *a_cap = new_capacity;
    }

    Answer new_a;
    new_a.answer_id = *a_count + 1;
    new_a.question_id = q_id;
    strcpy(new_a.author_username, username);
    new_a.upvotes = 0;

    while (getchar() != '\n');
    printf("\nEnter Answer Content: ");
    fgets(new_a.body, sizeof(new_a.body), stdin);
    new_a.body[strcspn(new_a.body, "\n")] = 0;

    printf("Attach Image/Diagram Path for Answer (or 'None'): ");
    fgets(new_a.image_path, sizeof(new_a.image_path), stdin);
    new_a.image_path[strcspn(new_a.image_path, "\n")] = 0;

    if (strlen(new_a.image_path) == 0) {
        strcpy(new_a.image_path, "None");
    }

    (*answers)[*a_count] = new_a;
    (*a_count)++;

    save_answers(*answers, *a_count);
    printf("Answer posted successfully!\n");
}

void browse_questions(Question *questions, int q_count) {
    printf("\n================ ALL QUESTIONS ================\n");
    if (q_count == 0) {
        printf("No questions posted yet.\n");
        return;
    }
    for (int i = 0; i < q_count; i++) {
        printf("[%d] %s (Tag: %s) - Posted by: %s\n",
               questions[i].question_id, questions[i].title,
               questions[i].category, questions[i].author_username);
    }
}

void view_question_details(Question *questions, int q_count, Answer *answers, int a_count, int q_id) {
    Question *q = NULL;
    for (int i = 0; i < q_count; i++) {
        if (questions[i].question_id == q_id) {
            q = &questions[i];
            break;
        }
    }

    if (!q) {
        printf("Question not found!\n");
        return;
    }

    printf("\n-----------------------------------------------\n");
    printf("QUESTION ID: %d\nTitle: %s\nCategory: %s\nAuthor: %s\n",
           q->question_id, q->title, q->category, q->author_username);
    printf("------------------- ANSWERS -------------------\n");

    int found_answers = 0;
    for (int i = 0; i < a_count; i++) {
        if (answers[i].question_id == q_id) {
            found_answers++;
            printf("Ans ID: %d | Author: %s | Upvotes: %d\n", answers[i].answer_id, answers[i].author_username, answers[i].upvotes);
            printf("Content: %s\n", answers[i].body);
            if (strcmp(answers[i].image_path, "None") != 0 && strlen(answers[i].image_path) > 0) {
                printf("Attached Diagram/Image: %s\n", answers[i].image_path);
            }
            printf("-----------------------------------------------\n");
        }
    }

    if (!found_answers) {
        printf("No answers submitted yet for this question.\n");
    }
}

void upvote_answer(Answer *answers, int a_count, User *users, int user_count, int a_id, const char *voter_username) {
    for (int i = 0; i < a_count; i++) {
        if (answers[i].answer_id == a_id) {
            if (strcmp(answers[i].author_username, voter_username) == 0) {
                printf("You cannot upvote your own answer.\n");
                return;
            }

            answers[i].upvotes++;
            printf("Answer upvoted! Total upvotes: %d\n", answers[i].upvotes);

            for (int j = 0; j < user_count; j++) {
                if (strcmp(users[j].username, answers[i].author_username) == 0) {
                    users[j].reputation += 5;
                    printf("Author %s gained +5 reputation! (Total: %d)\n", users[j].username, users[j].reputation);
                    save_users(users, user_count);
                    break;
                }
            }
            save_answers(answers, a_count);
            return;
        }
    }
    printf("Answer ID not found.\n");
}

void search_questions(Question *questions, int q_count, const char *keyword) {
    printf("\n--- Search Results for '%s' ---\n", keyword);
    int count = 0;
    for (int i = 0; i < q_count; i++) {
        if (strstr(questions[i].title, keyword) || strstr(questions[i].category, keyword)) {
            printf("[%d] %s (Tag: %s)\n", questions[i].question_id, questions[i].title, questions[i].category);
            count++;
        }
    }
    if (!count) printf("No matching questions found.\n");
}

int compare_users_by_rep(const void *a, const void *b) {
    User *u1 = (User *)a;
    User *u2 = (User *)b;
    return u2->reputation - u1->reputation;
}

void display_leaderboard(User *users, int user_count) {
    if (user_count == 0) {
        printf("No users to display.\n");
        return;
    }

    User *sorted = (User*)malloc((size_t)user_count * sizeof(User));
    memcpy(sorted, users, (size_t)user_count * sizeof(User));

    qsort(sorted, (size_t)user_count, sizeof(User), compare_users_by_rep);

    printf("\n================ LEADERBOARD (qsort) ================\n");
    for (int i = 0; i < user_count; i++) {
        printf("Rank %d: %s | Reputation: %d pts | Role: %s\n",
               i + 1, sorted[i].username, sorted[i].reputation, get_role_string(sorted[i].role));
    }
    free(sorted);
}

void save_questions(Question *questions, int q_count) {
    FILE *fp = fopen("questions.txt", "w");
    if (!fp) return;
    for (int i = 0; i < q_count; i++) {
        fprintf(fp, "%d|%s|%s|%s\n",
                questions[i].question_id, questions[i].author_username,
                questions[i].title, questions[i].category);
    }
    fclose(fp);
}

void load_questions(Question **questions, int *q_count, int *q_cap) {
    FILE *fp = fopen("questions.txt", "r");
    if (!fp) return;
    Question temp;
    while (fscanf(fp, "%d|%49[^|]|%149[^|]|%49[^\n]\n",
                  &temp.question_id, temp.author_username, temp.title,
                  temp.category) == 4) {
        if (*q_count >= *q_cap) {
            int new_capacity = (*q_cap > 0) ? (*q_cap * 2) : 5;
            Question *tmp = (Question*)realloc(*questions, (size_t)new_capacity * sizeof(Question));
            if (!tmp) {
                printf("Error: Memory allocation failed while loading questions.\n");
                break;
            }
            *questions = tmp;
            *q_cap = new_capacity;
        }
        (*questions)[*q_count] = temp;
        (*q_count)++;
    }
    fclose(fp);
}

void save_answers(Answer *answers, int a_count) {
    FILE *fp = fopen("answers.txt", "w");
    if (!fp) return;
    for (int i = 0; i < a_count; i++) {
        fprintf(fp, "%d|%d|%s|%s|%s|%d\n",
                answers[i].answer_id, answers[i].question_id, answers[i].author_username,
                answers[i].body, answers[i].image_path, answers[i].upvotes);
    }
    fclose(fp);
}

void load_answers(Answer **answers, int *a_count, int *a_cap) {
    FILE *fp = fopen("answers.txt", "r");
    if (!fp) return;
    Answer temp;
    while (fscanf(fp, "%d|%d|%49[^|]|%499[^|]|%255[^|]|%d\n",
                  &temp.answer_id, &temp.question_id, temp.author_username,
                  temp.body, temp.image_path, &temp.upvotes) == 6) {
        if (*a_count >= *a_cap) {
            int new_capacity = (*a_cap > 0) ? (*a_cap * 2) : 5;
            Answer *tmp = (Answer*)realloc(*answers, (size_t)new_capacity * sizeof(Answer));
            if (!tmp) {
                printf("Error: Memory allocation failed while loading answers.\n");
                break;
            }
            *answers = tmp;
            *a_cap = new_capacity;
        }
        (*answers)[*a_count] = temp;
        (*a_count)++;
    }
    fclose(fp);
}
