#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TODO_FILE "todo.txt"
#define MAX_TODOS 100
#define MAX_LINE 1024

typedef struct{
  int done;
  int prio;
  char deadline[10+1]; //YYYY-MM-DD
  char text[MAX_LINE];
} Todo;

int load_todos(Todo todos[]) {
  FILE *file = fopen(TODO_FILE, "r");
  if (!file) return 0;

  int todo_count = 0;
  char line[MAX_LINE];

  while (fgets(line, sizeof(line), file) && todo_count < MAX_TODOS) {
    if(line[0] != '[') continue;
    
    Todo t;
    t.done = (line[1] == 'x') ? 1 : 0;

    char *prio_str = strtok(line + 4, "|");
    char *deadline = strtok(NULL, "|");
    char *text = strtok(NULL,"\n");

    t.prio = atoi(prio_str);
    strncpy(t.deadline, deadline ? deadline : "", sizeof(t.deadline));
    strncpy(t.text, text ? text : "", MAX_LINE);

    todos[todo_count++] = t;
  }
  fclose(file);
  return todo_count;
}

void save_todos(Todo todos[], int todo_count) {
  FILE *file = fopen(TODO_FILE, "w");
  if (!file){
    perror("Error writing in file");
    return;
  }
  for (int todo = 0; todo < todo_count; todo++){
    fprintf(file, "[%c]|%d|%s|%s\n",
            todos[todo].done ? 'x' : ' ',
            todos[todo].prio,
            strlen(todos[todo].deadline) >  0 ? todos[todo].deadline : "          ",
            todos[todo].text
            );
  }
  fclose(file);
}

void list_todos(Todo todos[], int todo_count) {
  if (todo_count == 0){
    printf("No todos yet!\n");
    return;
  }
  for (int todo = 0; todo < todo_count; todo++) {
    printf("%2d. [%c] %s", todo, todos[todo].done ? 'x' : ' ', todos[todo].text);
    if (todos[todo].prio > 0)
      printf("  (Priority: %d", todos[todo].prio);
    if (strcmp(todos[todo].deadline, "          ") != 0 && 
        strlen(todos[todo].deadline) > 1)
      printf(", Due: %s", todos[todo].deadline);
    if (todos[todo].prio > 0 || strlen(todos[todo].deadline) > 1)
      printf(")");
    printf("\n");
  }
}

void add_todo(Todo todos[], int *todo_count, const char *text){
  if(*todo_count >= MAX_TODOS){
    printf("Too many todos!\n");
    return;
  }
  Todo t = {0};
  t.done = 0;
  t.prio = 2;
  strcpy(t.deadline, "");
  strncpy(t.text, text, MAX_LINE);
  todos[(*todo_count)++] = t;
  save_todos(todos, *todo_count);
  printf("Added: %s\n", text);
}

void mark_done(Todo todos[], int todo_count, int index){
  if (index < 0 || index >= todo_count){
    printf("Invalid todo number.\n");
    return;
  }
  todos[index].done = 1;
  save_todos(todos, todo_count);  
  printf("Marked done: %s\n", todos[index].text);
}

void delete_todo(Todo todos[], int *todo_count, int index){
  if (index < 0 || index >= *todo_count){
    printf("Invalid todo number.\n");
    return;
  }
  for (int i = index; i < *todo_count; i++){
    todos[i] = todos[i+1];
  }
  (*todo_count)--;
  save_todos(todos,*todo_count);
  printf("Deleted todo #%d\n", index);
}

void edit_todo(Todo todos[], int todo_count, int index, const char *new_text) {
  if (index < 0 || index >= todo_count){
    printf("Invalid todo number.\n");
    return;
  }

  strncpy(todos[index].text, new_text, MAX_LINE);
  save_todos(todos, todo_count);
  printf("Updated task #%d to: %s\n", index, new_text);
}

void set_deadline(Todo todos[], int todo_count, int index, const char *date) {
  if (index < 0 || index >= todo_count){
    printf("Invalid todo number.\n");
    return;
  }
  strncpy(todos[index].deadline, date, 10+1);
  save_todos(todos,todo_count);
  printf("Set deadline for task #%d to: %s\n", index, date);
}

void set_priority(Todo todos[], int todo_count, int index, int prio) {
  if (index < 0 || index >= todo_count || prio < 1 || prio > 3) {
    printf("Invalid todo number or priority.\n");
    return;
  }

  todos[todo_count].prio = prio;
  save_todos(todos, todo_count);
  printf("Set priority for task #%d to: %d\n", index, prio);
}

int main(int argc, char *argv[]) {
  Todo todos[MAX_TODOS];
  int todo_count = load_todos(todos);

  if(argc < 2) {
    printf("Usage:\n");
    printf("  %s list\n", argv[0]);
    printf("  %s add \"task\"\n", argv[0]);
    printf("  %s done <number>\n", argv[0]);
    printf("  %s delete <number>\n", argv[0]);
    printf("  %s edit <number> \"new text\"\n", argv[0]);
    printf("  %s deadline <number> YYYY-MM-DD\n", argv[0]);
    printf("  %s priority <number> <1|2|3>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "list") == 0){
    list_todos(todos, todo_count);
  } else if (strcmp(argv[1], "add") == 0 && argc >= 3) {
    add_todo(todos, &todo_count, argv[2]);
  } else if (strcmp(argv[1], "done") == 0 && argc >= 3) {
    mark_done(todos, todo_count, atoi(argv[2]));
  } else if (strcmp(argv[1], "delete") == 0 && argc >= 3) {
    delete_todo(todos, &todo_count, atoi(argv[2]));
  } else if (strcmp(argv[1], "edit") == 0 && argc >= 4) {
    edit_todo(todos, todo_count, atoi(argv[2]), argv[3]);
  } else if (strcmp(argv[1], "deadline") == 0 && argc >= 4) {
    set_deadline(todos, todo_count, atoi(argv[2]), argv[3]);
  } else if (strcmp(argv[1], "priority") == 0 && argc >= 4) {
    set_priority(todos, todo_count, atoi(argv[2]), atoi(argv[3]));
  } else {
    printf("Unknown or incomplete command.\n");
  }

  return 0;
}
