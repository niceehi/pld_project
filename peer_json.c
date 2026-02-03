#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"

#define MAX_NAME 50
#define MAX_COMMENT 256
#define QUESTION_COUNT 5
#define MAX_FEEDBACK 1000

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

// ---------- Простейший список вопросов ----------
Question questions[QUESTION_COUNT] = {
    {1, "What is a pointer in C?"},
    {2, "Difference between value and pointer?"},
    {3, "What does malloc do?"},
    {4, "What is a struct?"},
    {5, "Explain recursion briefly"}
};

// ---------- Утилиты ----------
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
void run_interview(Student interviewer, Student interviewee, Feedback *feedbacks, int *fb_count) {
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

// ---------- MAIN ----------
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
            run_interview(s1, s2, feedbacks, &fb_count);
            run_interview(s2, s1, feedbacks, &fb_count);
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

