#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "core/ast.h"
#include "core/lexer.h"
#include "core/token.h"
#include "utils/err.h"
#include "utils/memstk.h"

int main() {
    memstk_init();

    bool running = true;
    char input[50];
    token_list list;
    err myerr;
    token_list_create(&list);

    err_create();

    while (running && !err_occurred()) {
        printf("cuprite > ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
            }

            if (strcmp(input, "exit") == 0) {
                printf("bye \n");
                running = false;
            } else {
                parse_line(&list, input);

                GList *iter;
                for (iter = list->tokens; iter != NULL; iter = iter->next) {
                    token tok = (token)iter->data;
                    token_pp(tok);
                }

                token eos_tok;
                token_create(&eos_tok, token_eos, NULL);
                token_list_append(&list, &eos_tok);

                ast_parse_tokens(list);
                token_list_clear(&list);
            }
        } else {
            printf("Statement too long\n");
        }
    }

    token_list_destroy(&list);

    memstk_clean();

    return 0;
}
