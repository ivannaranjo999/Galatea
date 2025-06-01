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
    if (line[0] == '[' && (line[1] == ' ' || line[1] == 'x')) {
      todos[todo_count].done = (line[1] == 'x') ? 1 : 0;
      strncpy(todos[todo_count].text, line + 4, MAX_LINE);
      todos[todo_count].text[strcspn(todos[todo_count].text, "\n")] = 0;
      todo_count++;
    }
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
    fprintf(file, "[%c] %s\n", todos[todo].done ? 'x' : ' ',
          todos[todo].text);
  }
  fclose(file);
}

void list_todos(Todo todos[], int todo_count) {
  if (todo_count == 0){
    printf("No todos yet!\n");
    return;
  }

  for (int todo = 0; todo < todo_count; todo++){
    printf("%2d. [%c] %s\n", todo, todos[todo].done ? 'x' : ' ',
           todos[todo].text);
  }
}

void add_todo(Todo todos[], int *todo_count, const char *text){
  if(*todo_count >= MAX_TODOS){
    printf("Too many todos!\n");
    return;
  }
  todos[*todo_count].done = 0;
  strncpy(todos[*todo_count].text, text, MAX_LINE);
  (*todo_count)++;
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

int main(int argc, char *argv[]) {
  Todo todos[MAX_TODOS];
  int todo_count = load_todos(todos);

  if(argc < 2) {
    printf("Usage:\n");
    printf("  %s list\n", argv[0]);
    printf("  %s add \"task\"\n", argv[0]);
    printf("  %s done <number>\n", argv[0]);
    printf("  %s delete <number>\n", argv[0]);
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
  } else {
    printf("Unknown or incomplete command.\n");
  }

  return 0;
}
