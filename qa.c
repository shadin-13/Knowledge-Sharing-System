#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qa.h"
#include "user.h"

Question *questions = NULL;
int question_count = 0;
int question_capacity = INITIAL_CAPACITY;

Answer *answers = NULL;
int answer_count = 0;
int answer_capacity = INITIAL_CAPACITY;

static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void get_string_input(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

void init_qa_system() {
    questions = (Question *)malloc(question_capacity * sizeof(Question));
    answers = (Answer *)malloc(answer_capacity * sizeof(Answer));
    if (!questions || !answers) {
        perror("Memory allocation failed for QA system");
        exit(EXIT_FAILURE);
    }
}

void cleanup_qa_system() {
    if (questions) free(questions);
    if (answers) free(answers);
    questions = NULL;
    answers = NULL;
}

static void ensure_question_capacity() {
    if (question_count >= question_capacity) {
        question_capacity *= 2;
        questions = (Question *)realloc(questions, question_capacity * sizeof(Question));
        if (!questions) exit(EXIT_FAILURE);
    }
}

static void ensure_answer_capacity() {
    if (answer_count >= answer_capacity) {
        answer_capacity *= 2;
        answers = (Answer *)realloc(answers, answer_capacity * sizeof(Answer));
        if (!answers) exit(EXIT_FAILURE);
    }
}

void load_questions() {
    FILE *fp = fopen("questions.txt", "r");
    if (!fp) return;

    question_count = 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        ensure_question_capacity();
        sscanf(line, "%d;%d;%[^;];%[^;];%[^;];%d\n",
               &questions[question_count].id,
               &questions[question_count].author_id,
               questions[question_count].author_name,
               questions[question_count].category,
               questions[question_count].title,
               &questions[question_count].answer_count);
        question_count++;
    }
    fclose(fp);
}

void save_questions() {
    FILE *fp = fopen("questions.txt", "w");
    if (!fp) return;

    for (int i = 0; i < question_count; i++) {
        fprintf(fp, "%d;%d;%s;%s;%s;%d\n",
                questions[i].id,
                questions[i].author_id,
                questions[i].author_name,
                questions[i].category,
                questions[i].title,
                questions[i].answer_count);
    }
    fclose(fp);
}

void load_answers() {
    FILE *fp = fopen("answers.txt", "r");
    if (!fp) return;

    answer_count = 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        ensure_answer_capacity();
        sscanf(line, "%d;%d;%d;%[^;];%[^;];%d\n",
               &answers[answer_count].id,
               &answers[answer_count].question_id,
               &answers[answer_count].author_id,
               answers[answer_count].author_name,
               answers[answer_count].body,
               &answers[answer_count].upvotes);
        answer_count++;
    }
    fclose(fp);
}

void save_answers() {
    FILE *fp = fopen("answers.txt", "w");
    if (!fp) return;

    for (int i = 0; i < answer_count; i++) {
        fprintf(fp, "%d;%d;%d;%s;%s;%d\n",
                answers[i].id,
                answers[i].question_id,
                answers[i].author_id,
                answers[i].author_name,
                answers[i].body,
                answers[i].upvotes);
    }
    fclose(fp);
}

void post_question() {
    if (current_user_id == -1) {
        printf("\n[ERROR] Login required to ask a question!\n");
        return;
    }
    ensure_question_capacity();

    Question q;
    q.id = 100 + question_count + 1;
    q.author_id = current_user_id;
    q.answer_count = 0;

    for (int i = 0; i < user_count; i++) {
        if (users[i].id == current_user_id) {
            strcpy(q.author_name, users[i].username);
            break;
        }
    }

    printf("\n--- Ask a Question ---\nCategory: ");
    get_string_input(q.category, NAME_LEN);
    printf("Title: ");
    get_string_input(q.title, TEXT_LEN);

    questions[question_count++] = q;
    save_questions();
    printf("\n[SUCCESS] Question posted (ID: %d)\n", q.id);
}

void list_all_questions() {
    if (question_count == 0) {
        printf("\n[INFO] No questions found.\n");
        return;
    }
    printf("\n========================================================================\n");
    printf("%-5s | %-12s | %-15s | %-30s | %-7s\n", "ID", "Category", "Author", "Title", "Answers");
    printf("========================================================================\n");
    for (int i = 0; i < question_count; i++) {
        printf("%-5d | %-12s | %-15s | %-30s | %-7d\n",
               questions[i].id, questions[i].category, questions[i].author_name,
               questions[i].title, questions[i].answer_count);
    }
    printf("========================================================================\n");
}

void view_question_details() {
    int q_id, q_idx = -1;
    printf("\nEnter Question ID: ");
    if (scanf("%d", &q_id) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    for (int i = 0; i < question_count; i++) {
        if (questions[i].id == q_id) { q_idx = i; break; }
    }

    if (q_idx == -1) {
        printf("\n[ERROR] Question not found!\n");
        return;
    }

    printf("\nQUESTION #%d [%s]\nTitle : %s\nAuthor: %s\n",
           questions[q_idx].id, questions[q_idx].category, questions[q_idx].title, questions[q_idx].author_name);
    printf("----------------------------------------------------\nANSWERS:\n");

    int count = 0;
    for (int i = 0; i < answer_count; i++) {
        if (answers[i].question_id == q_id) {
            printf("  [%d] By %s (Upvotes: %d)\n      \"%s\"\n\n",
                   answers[i].id, answers[i].author_name, answers[i].upvotes, answers[i].body);
            count++;
        }
    }
    if (count == 0) printf("  No answers yet.\n");
}

void answer_question() {
    if (current_user_id == -1) {
        printf("\n[ERROR] Login required!\n");
        return;
    }
    int q_id, q_idx = -1;
    printf("\nEnter Question ID to answer: ");
    if (scanf("%d", &q_id) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    for (int i = 0; i < question_count; i++) {
        if (questions[i].id == q_id) { q_idx = i; break; }
    }
    if (q_idx == -1) {
        printf("\n[ERROR] Question not found!\n");
        return;
    }

    ensure_answer_capacity();

    Answer ans;
    ans.id = 500 + answer_count + 1;
    ans.question_id = q_id;
    ans.author_id = current_user_id;
    ans.upvotes = 0;

    for (int i = 0; i < user_count; i++) {
        if (users[i].id == current_user_id) {
            strcpy(ans.author_name, users[i].username);
            break;
        }
    }

    printf("Your Answer: ");
    get_string_input(ans.body, TEXT_LEN);

    answers[answer_count++] = ans;
    questions[q_idx].answer_count++;

    save_answers();
    save_questions();
    printf("\n[SUCCESS] Answer posted successfully!\n");
}

void upvote_answer() {
    if (current_user_id == -1) {
        printf("\n[ERROR] Login required!\n");
        return;
    }
    int a_id, a_idx = -1;
    printf("\nEnter Answer ID to upvote: ");
    if (scanf("%d", &a_id) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    for (int i = 0; i < answer_count; i++) {
        if (answers[i].id == a_id) { a_idx = i; break; }
    }
    if (a_idx == -1) {
        printf("\n[ERROR] Answer not found!\n");
        return;
    }

    answers[a_idx].upvotes++;
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == answers[a_idx].author_id) {
            users[i].reputation += 5;
            break;
        }
    }

    save_answers();
    save_users();
    printf("\n[SUCCESS] Upvoted Answer #%d (+5 Rep earned)!\n", a_id);
}

void search_questions() {
    char keyword[NAME_LEN];
    printf("\nEnter search keyword: ");
    get_string_input(keyword, NAME_LEN);

    printf("\n--- Search Results for '%s' ---\n", keyword);
    int matches = 0;
    for (int i = 0; i < question_count; i++) {
        if (strstr(questions[i].title, keyword) != NULL || strstr(questions[i].category, keyword) != NULL) {
            printf("ID: %d | [%s] %s (By %s)\n", questions[i].id, questions[i].category, questions[i].title, questions[i].author_name);
            matches++;
        }
    }
    if (matches == 0) printf("No matching questions found.\n");
}
