#ifndef AKINATOR_H

#define AKINATOR_H
Tree *akinator_create(const char *filename);

int akinator(const char *filename);

int akinator_play(Tree *tree);

int ask_question(Node *node);

int akinator_add_question(Node *node, char *question, char *yes_answer, char *no_answer);

int akinator_add_root(Tree *tree, char *question, char *yes_answer, char *no_answer);

void clean_stdin();

#endif
