#include "token.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "glib.h"
#include "utils/err.h"
#include "utils/memstk.h"
#include "utils/str.h"

MEMSTK_CLEANUP(token_list, token_list_destroy);
MEMSTK_CLEANUP(token, token_destroy);

static const char *_get_token_label(token_type type);
static bool _has_data(token_type type);

void token_list_create(token_list *list) {
    *list = malloc(sizeof(struct sToken_list));
    if (*list == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for token_list");
    }
    (*list)->tokens = NULL;
    (*list)->size = 0;
    (*list)->memstk_node =
        memstk_push((void **)&(*list), _memstk_token_list_cleanup);
}

void token_list_append(token_list *list, token *tok) {
    (*list)->tokens = g_list_append((*list)->tokens, *tok);
    (*list)->size++;
}

void token_list_remove_last(token_list *list) {
    GList *last_itm = g_list_last((*list)->tokens);
    if (last_itm != NULL) {
        token tok = (token)last_itm->data;
        tok->memstk_node->freed = true;
        token_destroy(&tok);
        (*list)->tokens = g_list_remove((*list)->tokens, last_itm->data);
        (*list)->size--;
    }
}

void token_list_clear(token_list *list) {
    token_list_destroy(list);
    token_list_create(list);
}

void token_list_destroy(token_list *list) {
    if (*list != NULL) {
        if ((*list)->tokens != NULL) {
            GList *iter;
            for (iter = (*list)->tokens; iter != NULL; iter = iter->next) {
                token tok = (token)iter->data;
                token_destroy(&tok);
            }
            g_list_free((*list)->tokens);
        }
        (*list)->memstk_node->freed = true;
        free(*list);
        *list = NULL;
    }
}

void token_create(token *token, token_type type, char *value) {
    (*token) = malloc(sizeof(struct sToken));
    if (*token == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for token");
    }

    (*token)->type = type;

    if (value != NULL && _has_data(type)) {
        str_create(&((*token)->value), value);
    } else {
        (*token)->value = NULL;
    }
    (*token)->memstk_node =
        memstk_push((void **)&(*token), _memstk_token_cleanup);
}

void token_pp(token tok) {
    if (tok != NULL) {
        if (_has_data(tok->type)) {
            printf("Token(type:\"%s\", value: \"%s\")\n",
                   _get_token_label(tok->type),
                   tok->value != NULL ? str_val(&(tok->value)) : "<NULL>");
        } else {
            printf("Token(type:\"%s\")\n", _get_token_label(tok->type));
        }
    }
}

void token_destroy(token *token) {
    str my_str = (*token)->value;
    str_destroy(&my_str);
    (*token)->memstk_node->freed = true;
    free(*token);
    *token = NULL;
}

token token_cpy(token tok) {
    token tok_new;
    token_create(&tok_new, tok->type,
                 _has_data(tok->type) ? str_val(&(tok->value)) : NULL);
    return tok_new;
}

static const char *_get_token_label(token_type type) {
    switch (type) {
        case token_identf:
            return "IDENTIFIER";
        case token_num_int:
            return "INT";
        case token_num_float:
            return "FLOAT";
        case token_when:
            return "WHEN";
        case token_is:
            return "IS";
        case token_not:
            return "NOT";
        case token_do:
            return "DO";
        case token_end:
            return "END";
        case token_str:
            return "STRING";
        case token_bool_t:
            return "BOOL_TRUE";
        case token_bool_f:
            return "BOOL_FALSE";
        case token_oparentheses:
            return "(";
        case token_cparentheses:
            return ")";
        case token_plus:
            return "+";
        case token_hyphen:
            return "-";
        case token_asterisk:
            return "*";
        case token_fslash:
            return "/";
        case token_percent:
            return "%";
        case token_equal:
            return "=";
        case token_greater:
            return ">";
        case token_less:
            return "<";
        case token_greater_eq:
            return ">=";
        case token_less_eq:
            return "<=";
        case token_eos:
            return "EoS";
        case token_eof:
            return "EoF";
        case token_isnot:
            return "isnot";
        case token_and:
            return "and";
        case token_or:
            return "or";
        case token_comma:
            return ",";
        default:
            return "UNKNOWN";
    }
}

static bool _has_data(token_type type) {
    switch (type) {
        case token_num_int:
        case token_num_float:
        case token_identf:
        case token_str:
            return true;
        default:
            return false;
    }
}
