#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"

#define MAX_NAME 50
#define MAX_COMMENT 256
#define QUESTION_COUNT 15
#define MAX_FEEDBACK 1000

// ================= СТРУКТУРЫ =================
typedef struct {
    int id;
    char name[MAX_NAME];
} Student;

typedef struct {
    int id;
    char text[200];
} Question;

typedef struct {
    int interviewer_id;
    int interviewee_id;
    int question_id;
    int score;
    char comment[MAX_COMMENT];
} Feedback;

// ================= ВОПРОСЫ =================
Question list_A[QUESTION_COUNT] = {
    {1, "A1. What is a pointer in C? How to obtain address?"},
    {2, "A2. Diff between passing by value and passing by pointer?"},
    {3, "A3. What does malloc do? When should free be called?"},
    {4, "A4. What is a struct in C?"},
    {5, "A5. What are argc and argv used for?"},
    {6, "A6. Diff between singly and doubly linked list?"},
    {7, "A7. What is recursion? Example?"},
    {8, "A8. What does printf do and what means %d?"},
    {9, "A9. What is a file in C and functions to work with it?"},
    {10, "A10. What is a hash table and usage?"},
    {11, "A11. What is a binary tree?"},
    {12, "A12. Idea of sorting an array? Name one algorithm."},
    {13, "A13. What is NULL and where is it used?"},
    {14, "A14. What does 'ls' command do?"},
    {15, "A15. What does './program arg1 arg2' mean?"}
};

Question list_B[QUESTION_COUNT] = {
    {1, "B1. Diff between normal variable and pointer?"},
    {2, "B2. What happens if memory alloc with malloc is not freed?"},
    {3, "B3. How do you access structure field through a pointer?"},
    {4, "B4. What is fopen used for?"},
    {5, "B5. Diff between recursion and loop?"},
    {6, "B6. What is stored in argv[0]?"},
    {7, "B7. What is a singly linked list? Node fields?"},
    {8, "B8. Why is typedef used with structures?"},
    {9, "B9. What is a doubly linked list (two pointers)?"},
    {10, "B10. What is a binary search tree?"},
    {11, "B11. Why might a program need a hash table?"},
    {12, "B12. What does '->' operator mean?"},
    {13, "B13. Diff between static and dynamic array?"},
    {14, "B14. What does 'chmod' command do?"},
    {15, "B15. What does output redirection '>' mean?"}
};

// ================= УТИЛИТЫ =================
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ---------- Чтение студентов из JSON ----------
int load_students(const char *filename, Student **students) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Cannot open %s\n", filename);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = malloc(size + 1);
    fread(data, 1, size, f);
    data[size] = '\0';
    fclose(f);

    cJSON *json = cJSON_Parse(data);
    free(data);
    if (!json) {
        printf("JSON parse error!\n");
        return 0;
    }

    int count = cJSON_GetArraySize(json);
    *students = malloc(sizeof(Student) * count);

    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        (*students)[i].id = cJSON_GetObjectItem(item, "id")->valueint;
        strcpy((*students)[i].name, cJSON_GetObjectItem(item, "name")->valuestring);
    }

    cJSON_Delete(json);
    return count;
}

// ---------- Перемешивание студентов ----------
void shuffle_students(Student *students, int n) {
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Student tmp = students[i];
        students[i] = students[j];
        students[j] = tmp;
    }
}

// ---------- Интервью ----------
void run_interview(Student interviewer, Student interviewee, Question *questions, Feedback *feedbacks, int *fb_count) {
    printf("\n=== Interview: %s -> %s ===\n", interviewer.name, interviewee.name);

    for (int i = 0; i < QUESTION_COUNT; i++) {
        Feedback fb;
        fb.interviewer_id = interviewer.id;
        fb.interviewee_id = interviewee.id;
        fb.question_id = questions[i].id;

        printf("\nQ%d: %s\n", questions[i].id, questions[i].text);
        printf("Score (0-10): ");
        scanf("%d", &fb.score);
        clear_input_buffer();

        printf("Comment: ");
        fgets(fb.comment, MAX_COMMENT, stdin);
        fb.comment[strcspn(fb.comment, "\n")] = 0;

        feedbacks[*fb_count] = fb;
        (*fb_count)++;
    }
}

// ================= MAIN =================
int main() {
    Student *students = NULL;
    int student_count = load_students("students.json", &students);
    if (!student_count) return 1;

    printf("Loaded %d students.\n", student_count);

    shuffle_students(students, student_count);

    Feedback feedbacks[MAX_FEEDBACK];
    int fb_count = 0;

    // ---------- Формируем пары ----------
    for (int i = 0; i < student_count; i += 2) {
        if (i + 1 < student_count) {
            Student s1 = students[i];
            Student s2 = students[i + 1];

            printf("\n--- Pair: %s & %s ---\n", s1.name, s2.name);

            // Студент 1 задаёт вопросы list_A
            run_interview(s1, s2, list_A, feedbacks, &fb_count);

            // Студент 2 задаёт вопросы list_B
            run_interview(s2, s1, list_B, feedbacks, &fb_count);

        } else {
            printf("\n--- Student without pair: %s ---\n", students[i].name);
        }
    }

    // ---------- Вывод результатов ----------
    printf("\n=========== FEEDBACK RESULTS ===========\n");
    for (int i = 0; i < fb_count; i++) {
        printf("Q%d | %d -> %d | Score: %d | %s\n",
               feedbacks[i].question_id,
               feedbacks[i].interviewer_id,
               feedbacks[i].interviewee_id,
               feedbacks[i].score,
               feedbacks[i].comment);
    }

    free(students);
    return 0;
}
