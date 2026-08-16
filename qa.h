#ifndef QA_H
#define QA_H

#define TEXT_LEN 256
#define NAME_LEN 50
#define PATH_LEN 100
#define INITIAL_CAPACITY 10

typedef struct {
    int id;
    int question_id;
    int author_id;
    char author_name[NAME_LEN];
    char body[TEXT_LEN];
    int upvotes;
} Answer;

typedef struct {
    int id;
    int author_id;
    char author_name[NAME_LEN];
    char title[TEXT_LEN];
    char category[NAME_LEN];
    int answer_count;
    char image_path[PATH_LEN]; // Question Image / Diagram Attachment
} Question;

extern Question *questions;
extern int question_count;
extern int question_capacity;

extern Answer *answers;
extern int answer_count;
extern int answer_capacity;

void init_qa_system();
void cleanup_qa_system();
void load_questions();
void save_questions();
void load_answers();
void save_answers();

void post_question();
void list_all_questions();
void view_question_details();
void answer_question();
void upvote_answer();
void search_questions();
void admin_delete_question();

#endif
