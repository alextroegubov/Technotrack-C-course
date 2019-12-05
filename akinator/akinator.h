#ifndef AKINATOR_H

#define AKINATOR_H

int akinator_start(const char *filename);

int akinator_play(Tree *tree);

int ask_question(Node *node);

int akinator_add_question(Node *node, char *question, char *yes_answer);

void clean_stdin();

#endif
