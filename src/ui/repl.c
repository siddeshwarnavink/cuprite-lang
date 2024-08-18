#include "repl.h"

#include <stdbool.h>
#include <stdio.h>

#include "core/ast.h"
#include "core/lexer.h"
#include "core/token.h"
#include "glib.h"
#include "glibconfig.h"
#include "ui/repl.h"
#include "utils/err.h"

void repl_run() {
    printf("\033[1;36m");
    printf("                        _ _       \n");
    printf("   ___ _   _ _ __  _ __(_) |_ ___ \n");
    printf("  / __| | | | '_ \\| '__| | __/ _ \\\n");
    printf(" | (__| |_| | |_) | |  | | ||  __/\n");
    printf("  \\___|\\__,_| .__/|_|  |_|\\__\\___|\n");
    printf("            |_|                   \n");
    printf("by Siddeshwar\n\033[0m");
    printf("\033[1;37mtype \"!exit\" to quit.\n\n\033[0m");

    bool running = true, debug_mode = false;
    char input[150];
    token_list list;
    err myerr;
    token_list_create(&list);
    GQueue *block_stack = g_queue_new();

    err_create();

    while (running && !err_occurred()) {
        if (g_queue_is_empty(block_stack))
            printf("cup > ");
        else
            printf("> ");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') input[len - 1] = '\0';

            if (strcmp(input, "!exit") == 0) {
                printf("bye \n");
                running = false;
            } else if (strcmp(input, "!debug") == 0) {
                printf("Debug mode turned ");
                if (debug_mode)
                    printf("\033[0;31mOFF\033[0m\n");
                else
                    printf("\033[0;32mON\033[0m\n");
                debug_mode = !debug_mode;
            } else {
                parse_line(&list, input);

                if (list != NULL) {
                    GList *last_node = g_list_last(list->tokens);
                    if (last_node != NULL) {
                        token last_tok = (token)last_node->data;
                        switch (last_tok->type) {
                            case token_do:
                                g_queue_push_tail(block_stack,
                                                  GINT_TO_POINTER('b'));
                                break;
                            case token_end:
                                g_queue_pop_tail(block_stack);
                                break;
                            default:
                                break;
                        }
                    }

                    token eos_tok;
                    token_create(&eos_tok, token_eos, NULL);
                    token_list_append(&list, &eos_tok);

                    if (g_queue_is_empty(block_stack)) {
                        // display tokens
                        if (debug_mode) {
                            GList *iter;
                            for (iter = list->tokens; iter != NULL;
                                 iter = iter->next) {
                                token tok = (token)iter->data;
                                token_pp(tok);
                            }
                        }

                        ast_parse_tokens(list);
                        token_list_clear(&list);
                    }
                }
            }
        } else {
            err_throw(err_error, "Statement too long");
        }
    }

    g_queue_free(block_stack);
    token_list_destroy(&list);
}
