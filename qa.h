#ifndef QA_H
#define QA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

typedef struct {
    int question_id;
    char author_username[50];
    char title[150];
    char category[50];
} Question;

typedef struct {
    int answer_id;
    int question_id;
    char author_username[50];
    char body[500];
    char image_path[256];
    int upvotes;
} Answer;

void post_question(Question **questions, int *q_count, int *q_cap, const char *username);
void post_answer(Answer **answers, int *a_count, int *a_cap, Question *questions, int q_count, int q_id, const char *username);
void browse_questions(Question *questions, int q_count);
void view_question_details(Question *questions, int q_count, Answer *answers, int a_count, int q_id);
void upvote_answer(Answer *answers, int a_count, User *users, int user_count, int a_id, const char *voter_username);
void search_questions(Question *questions, int q_count, const char *keyword);
void display_leaderboard(User *users, int user_count);

void save_questions(Question *questions, int q_count);
void load_questions(Question **questions, int *q_count, int *q_cap);
void save_answers(Answer *answers, int a_count);
void load_answers(Answer **answers, int *a_count, int *a_cap);

#endif
