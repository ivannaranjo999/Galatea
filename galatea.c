#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <pwd.h>

#define MAX_TODOS 100
#define MAX_LINE 1024
#define PATH_MAX 4096

typedef struct{
  int done;
  int prio;
  char deadline[10+1]; //YYYY-MM-DD
  char text[MAX_LINE];
} Todo;

char *get_tasks_file_path() {
  static char path[PATH_MAX];
  const char *home = getenv("HOME");

  if (!home) {
    home = getpwuid(getuid())->pw_dir;
  }

  snprintf(path, sizeof(path), "%s/.config/galatea", home);
  mkdir(path, 0755);

  strncat(path, "/galatea-tasks.txt", sizeof(path) - strlen(path) - 1);
  return path;
}


int load_todos(Todo todos[]) {
  FILE *file = fopen(get_tasks_file_path(), "r");
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
  FILE *file = fopen(get_tasks_file_path(), "w");
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
  todos[index].done = !todos[index].done;
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

  todos[index].prio = prio;
  save_todos(todos, todo_count);
  printf("Set priority for task #%d to: %d\n", index, prio);
}

void tui(Todo todos[], int *todo_count) {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);

  int selected = 0;
  while (1) {
    clear();
    for (int i = 0; i < *todo_count; i++) {
      if (i == selected) attron(A_REVERSE);
      char display_line[MAX_LINE + 64];
      snprintf(display_line, sizeof(display_line), "[%c] %s", 
              todos[i].done ? 'x' : ' ', todos[i].text);

      if (todos[i].prio > 0 || strlen(todos[i].deadline) > 1) {
        strcat(display_line, "  (");
        if (todos[i].prio > 0) {
          char prio_str[32];
          snprintf(prio_str, sizeof(prio_str), "Priority: %d", todos[i].prio);
          strcat(display_line, prio_str);
        }
        if (strcmp(todos[i].deadline, "          ") != 0 && 
          strlen(todos[i].deadline) > 1) {
          if (todos[i].prio > 0) strcat(display_line, ", ");
          strcat(display_line, "Due: ");
          strcat(display_line, todos[i].deadline);
        }
      strcat(display_line, ")");
}
      mvprintw(i, 0, "%s", display_line);
      if (i == selected) attroff(A_REVERSE);
    }
    mvprintw(*todo_count + 2, 0, "j/k = navigate | a = add | x = toggle | p = priority | f = deadline | d = delete | q = quit");
    refresh();

    int ch = getch();
    switch (ch) {
      case 'q': endwin(); return;
      case KEY_UP: 
      case 'k': 
        if (selected > 0) selected--; 
        else selected = *todo_count - 1;
        break;
      case KEY_DOWN: 
      case 'j': 
        if (selected < *todo_count - 1) selected++;
        else selected = 0;
        break;
      case 'x':
        mark_done(todos, *todo_count, selected);
        break;
      case 'd':
        delete_todo(todos, todo_count, selected);
        if (selected > *todo_count) selected = *todo_count;
        break;
      case 'a': {
        echo();
        char add_buffer[MAX_LINE];
        mvprintw(*todo_count + 4, 0, "New task: ");
        getnstr(add_buffer, MAX_LINE - 1);
        noecho();
        if (strlen(add_buffer) > 0) {
          add_todo(todos, todo_count, add_buffer);
        }
        break;
      }
      case 'p':
        echo();
        char prio_buffer[16];
        mvprintw(*todo_count + 4, 0, "Set priority (1-3): ");
        getnstr(prio_buffer, sizeof(prio_buffer) - 1);
        noecho();
        int new_prio = atoi(prio_buffer);
        set_priority(todos, *todo_count, selected, new_prio);
        break;
      case 'f':
        echo();
        char dl_buffer[10+1];
        mvprintw(*todo_count + 4, 0, "Set deadline (YYYY-MM-DD): ");
        getnstr(dl_buffer, sizeof(dl_buffer) - 1);
        noecho();
        set_deadline(todos, *todo_count, selected, dl_buffer);
        break;
    }
  }

    endwin();
}

int main(int argc, char *argv[]) {
  Todo todos[MAX_TODOS];
  int todo_count = load_todos(todos);

  if (argc == 1) {
    tui(todos, &todo_count);
    return 0;
  } else {
    if (strcmp(argv[1], "list") == 0){
      list_todos(todos, todo_count);
    } else if (strcmp(argv[1], "add") == 0 && argc >= 3) {
      add_todo(todos, &todo_count, argv[2]);
    } else if (strcmp(argv[1], "mark") == 0 && argc >= 3) {
      mark_done(todos, todo_count, atoi(argv[2]));
    } else if (strcmp(argv[1], "delete") == 0 && argc >= 3) {
      delete_todo(todos, &todo_count, atoi(argv[2]));
    } else if (strcmp(argv[1], "edit") == 0 && argc >= 4) {
      edit_todo(todos, todo_count, atoi(argv[2]), argv[3]);
    } else if (strcmp(argv[1], "deadline") == 0 && argc >= 4) {
      set_deadline(todos, todo_count, atoi(argv[2]), argv[3]);
    } else if (strcmp(argv[1], "priority") == 0 && argc >= 4) {
      set_priority(todos, todo_count, atoi(argv[2]), atoi(argv[3]));
    } else if (strcmp(argv[1], "help") == 0){
      printf("Usage:\n");
      printf("  %s list\n", argv[0]);
      printf("  %s add \"task\"\n", argv[0]);
      printf("  %s mark <number>\n", argv[0]);
      printf("  %s delete <number>\n", argv[0]);
      printf("  %s edit <number> \"new text\"\n", argv[0]);
      printf("  %s deadline <number> YYYY-MM-DD\n", argv[0]);
      printf("  %s priority <number> <1|2|3>\n", argv[0]);
    } else {
      printf("Unknown or incomplete command.\n");
    }
  }

  return 0;
}
