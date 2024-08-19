#include "ast.h"

#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "utils/err.h"
#include "utils/memstk.h"
#include "utils/str.h"

MEMSTK_CLEANUP(ast_node, ast_destroy_node);
ASTDATA_CLEANUP(ast_declare);
ASTDATA_CLEANUP(ast_func_call);
ASTDATA_CLEANUP(ast_arithmetic_add);
ASTDATA_CLEANUP(ast_arithmetic_subtract);
ASTDATA_CLEANUP(ast_arithmetic_multiply);
ASTDATA_CLEANUP(ast_arithmetic_divide);

static void _ast_list_clean(void *lst);
static void _ast_stack_clean(void *itm);
static void _token_stack_clean(void *itm);
static bool _condition_block(token_list list);
static bool _variable_declaration(token_list list);
static bool _operator_token(token tok);
static int _operator_precedence(token_type type);
static void _extract_from_token(ast_node *node, token tok);
static void _make_expression_node(ast_node *node, GQueue *operator_stack,
                                  GQueue *operand_stack);
static void _operand_stack_cleanup(void *itm);
static void _operator_stack_cleanup(void *itm);
static void _arithmetic_pp(char *label, ast_node node);
static bool _function_call(token_list list);

void ast_create_node(ast_node *node, ast_node_type type, ast_data data) {
    (*node) = malloc(sizeof(struct sAstNode));
    if (*node == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for node");
    }

    (*node)->type = type;
    (*node)->data = data;
    (*node)->memstk_node =
        memstk_push((void **)&(*node), _memstk_ast_node_cleanup);
    (*node)->data_memstk_node = NULL;
}

memstk_node *ast_create_node_data(ast_data *data, ast_node_type type) {
    switch (type) {
        case ast_val_int:
            *data = (ast_data)malloc(sizeof(int));
            break;
        case ast_val_float:
            *data = (ast_data)malloc(sizeof(float));
            break;
        case ast_bool:
            *data = (ast_data)malloc(sizeof(bool));
            break;
        case ast_arithmetic_add:
        case ast_arithmetic_subtract:
        case ast_arithmetic_multiply:
        case ast_arithmetic_divide:
        case ast_cond_is:
        case ast_cond_is_not:
        case ast_cond_greater:
        case ast_cond_greater_eq:
        case ast_cond_lesser:
        case ast_cond_lesser_eq:
            *data = (ast_data)malloc(sizeof(union uAstData));
            (*data)->expression =
                (ast_expression_data)malloc(sizeof(struct sAstExpressionData));
            if ((*data)->expression == NULL) {
                free(*data);
                err_throw(err_fatal,
                          "Failed to allocate memory for ast_expression_data");
            }
            break;
        default:
            *data = (ast_data)malloc(sizeof(union uAstData));
            break;
    }

    if (*data == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for ast_data");
    }

    switch (type) {
        case ast_declare:
            (*data)->var_declare =
                (ast_var_declare)malloc(sizeof(struct sAstVarDeclare));
            if ((*data)->var_declare == NULL) {
                free((*data));
                err_throw(err_fatal,
                          "Failed to allocate memory for ast_var_declare_data");
            }
            return memstk_push((void **)&(*data), _ast_declare_cleanup);
        case ast_func_call:
            (*data)->fcall = (ast_fcall)malloc(sizeof(struct sAstFCall));
            if ((*data)->fcall == NULL) {
                free(*data);
                err_throw(err_fatal, "Failed to allocate memory for ast_fcall");
            }
            (*data)->fcall->args = NULL;
            return memstk_push((void **)&(*data), _ast_func_call_cleanup);
        case ast_arithmetic_add:
            return memstk_push((void **)&(*data), _ast_arithmetic_add_cleanup);
        case ast_arithmetic_subtract:
            return memstk_push((void **)&(*data),
                               _ast_arithmetic_subtract_cleanup);
        case ast_arithmetic_multiply:
            return memstk_push((void **)&(*data),
                               _ast_arithmetic_multiply_cleanup);
        case ast_arithmetic_divide:
            return memstk_push((void **)&(*data),
                               _ast_arithmetic_divide_cleanup);
        default:
            break;
    }
    return NULL;
}

void ast_destroy_node_data(ast_data *data, ast_node_type type) {
    switch (type) {
        case ast_arithmetic_add:
        case ast_arithmetic_subtract:
        case ast_arithmetic_multiply:
        case ast_arithmetic_divide:
        case ast_cond_is:
        case ast_cond_is_not:
        case ast_cond_greater:
        case ast_cond_greater_eq:
        case ast_cond_lesser:
        case ast_cond_lesser_eq:
            ast_destroy_node(&((*data)->expression->left));
            ast_destroy_node(&((*data)->expression->right));
            free((*data)->expression);
            break;
        case ast_declare:
            str_destroy(&((*data)->var_declare->name));
            ast_destroy_node(&((*data)->var_declare->value));
            free((*data)->var_declare);
            break;
        case ast_str:
        case ast_identf:
            str_destroy(&((*data)->val_str));
            break;
        case ast_func_call:
            str_destroy(&((*data)->fcall->name));
            GList *iter;
            for (iter = (*data)->fcall->args; iter != NULL; iter = iter->next) {
                ast_node node = (ast_node)iter->data;
                ast_destroy_node(&node);
            }
            g_list_free((*data)->fcall->args);
            free((*data)->fcall);
            break;
        default:
            break;
    }
    free((*data));
}

void ast_destroy_node(ast_node *node) {
    if (!(*node)) return;
    if ((*node)->data_memstk_node != NULL) {
        (*node)->data_memstk_node->freed = true;
    }
    (*node)->memstk_node->freed = true;
    ast_destroy_node_data(&((*node)->data), (*node)->type);
    free(*node);
    *node = NULL;
}

void ast_parse_tokens(token_list tokens) {
    if (tokens == NULL) {
        err_throw(err_error, "No tokens passed.");
        return;
    }

    GList *nodes = NULL;
    token_list line_tokens;
    token_list_create(&line_tokens);

    memstk_node *nodes_memstk = memstk_push((void **)nodes, _ast_list_clean);

    GList *token_iter;
    for (token_iter = tokens->tokens; token_iter != NULL;
         token_iter = token_iter->next) {
        token tok = (token)token_iter->data;
        if (tok->type == token_eos || tok->type == token_eof) {
            // determine the synatx of line
            // variable declaration
            if (_variable_declaration(line_tokens)) {
                ast_node node = ast_parse_variable_declaration(line_tokens);
                if (node != NULL) {
                    nodes = g_list_append(nodes, node);
                }
            }
            // function call
            else if (_function_call(line_tokens)) {
                token my_tok = token_cpy(tok);
                token_list_append(&line_tokens, &my_tok);
                ast_node node = ast_parse_function_call(line_tokens);
                if (node != NULL) {
                    nodes = g_list_append(nodes, node);
                }
            }
            // condition block
            else if (_condition_block(line_tokens)) {
                ast_node node =
                    ast_parse_condition_block(line_tokens, token_iter);
                if (node != NULL) {
                    nodes = g_list_append(nodes, node);
                }
            }
            // expression
            else {
                ast_node node = ast_parse_expression(line_tokens);
                if (node != NULL) {
                    nodes = g_list_append(nodes, node);
                }
            }

            token_list_clear(&line_tokens);
        } else {
            token my_tok = token_cpy(tok);
            token_list_append(&line_tokens, &my_tok);
        }
    }

    if (nodes != NULL) {
        GList *iter;
        for (iter = nodes; iter != NULL; iter = iter->next) {
            ast_node node = (ast_node)iter->data;
            if (node != NULL) {
                printf("AST: ");
                ast_pp(node);
                printf("\n");
                ast_destroy_node(&node);
            }
        }
        nodes_memstk->freed = true;
        g_list_free(nodes);
    }

    token_list_destroy(&line_tokens);
}

ast_node ast_parse_variable_declaration(token_list tokens) {
    // build variable value tokens list
    token_list expression_toks;
    token_list_clear(&expression_toks);

    GList *iter = tokens->tokens;
    iter = iter->next;
    iter = iter->next;
    for (; iter != NULL; iter = iter->next) {
        token tok = (token)iter->data;
        token tok_cp = token_cpy(tok);
        token_list_append(&expression_toks, &tok_cp);
    }

    ast_data node_d;
    memstk_node *node_d_memstk = ast_create_node_data(&node_d, ast_declare);

    GList *firstel = g_list_first(tokens->tokens);
    if (firstel != NULL) {
        token identf_tok = (token)firstel->data;
        str_create(&(node_d->var_declare->name), str_val(&(identf_tok->value)));
        node_d->var_declare->value = ast_parse_expression(expression_toks);
    } else {
        err_throw(err_error, "Invalid variable declaration");
    }

    ast_node node;
    ast_create_node(&node, ast_declare, node_d);
    node->data_memstk_node = node_d_memstk;

    token_list_destroy(&expression_toks);
    return node;
}

ast_node ast_parse_condition_block(token_list tokens, GList *token_iter) {
    ast_node node = NULL;

    // TODO: Implement this logic

    return node;
}

ast_node ast_parse_function_call(token_list tokens) {
    ast_node node;

    token_list args_toks;
    token_list_create(&args_toks);

    GList *toks_iter = tokens->tokens;
    token func_name = (token)toks_iter->data;

    ast_data node_d;
    memstk_node *node_d_memstk = ast_create_node_data(&node_d, ast_func_call);
    str_cpy(&(node_d->fcall->name), &(func_name->value));

    toks_iter = toks_iter->next;

    for (; toks_iter != NULL; toks_iter = toks_iter->next) {
        token tok = (token)toks_iter->data;
        if (tok->type == token_oparentheses) continue;

        if (tok->type == token_comma || tok->type == token_cparentheses ||
            tok->type == token_eos) {
            if (args_toks->size > 0) {
                ast_node arg_exp = ast_parse_expression(args_toks);
                node_d->fcall->args =
                    g_list_append(node_d->fcall->args, arg_exp);
                token_list_clear(&args_toks);
            }
        } else {
            token tok_cpy = token_cpy(tok);
            token_list_append(&args_toks, &tok_cpy);
        }
    }

    ast_create_node(&node, ast_func_call, node_d);
    node->data_memstk_node = node_d_memstk;
    token_list_destroy(&args_toks);

    return node;
}

ast_node ast_parse_expression(token_list tokens) {
    // Handle non-arithmetic expression
    if (tokens->size == 1) {
        token tok = (token)tokens->tokens->data;
        ast_node node = NULL;
        _extract_from_token(&node, tok);
        return node;
    }

    ast_node head;
    GQueue *operator_stack = g_queue_new(), *operand_stack = g_queue_new();

    memstk_node *operator_stack_memstk =
        memstk_push((void **)&operator_stack, _token_stack_clean);
    memstk_node *operand_stack_memstk =
        memstk_push((void **)&operand_stack, _ast_stack_clean);

    GList *iter;
    for (iter = tokens->tokens; iter != NULL; iter = iter->next) {
        token tok = (token)iter->data;

        if (tok->type == token_str) {
            ast_data node_d;
            memstk_node *node_d_memstk = ast_create_node_data(&node_d, ast_str);
            str_create(&node_d->val_str, str_val(&tok->value));

            ast_node node;
            ast_create_node(&node, ast_str, node_d);
            node->data_memstk_node = node_d_memstk;
            g_queue_push_tail(operand_stack, node);
            continue;
        }

        if (tok->type == token_oparentheses) {
            token tok_cp = token_cpy(tok);
            g_queue_push_tail(operator_stack, tok_cp);
            continue;
        } else if (tok->type == token_cparentheses) {
            token top;
            while ((top = (token)g_queue_peek_tail(operator_stack)) != NULL &&
                   top->type != token_oparentheses) {
                ast_node node;
                _make_expression_node(&node, operator_stack, operand_stack);
                g_queue_push_tail(operand_stack, node);
            }
            token opara_tok = g_queue_pop_tail(operator_stack);
            token_destroy(&opara_tok);
            continue;
        }

        // check if operand
        else if (!_operator_token(tok)) {
            ast_node node;
            _extract_from_token(&node, tok);
            g_queue_push_tail(operand_stack, node);
        }

        // its operator
        else if (_operator_token(tok)) {
            token tok_cp = token_cpy(tok);
            token top;
            while ((top = (token)g_queue_peek_tail(operator_stack)) != NULL &&
                   _operator_precedence(top->type) <=
                       _operator_precedence(tok_cp->type)) {
                ast_node node;
                _make_expression_node(&node, operator_stack, operand_stack);
                g_queue_push_tail(operand_stack, node);
            }
            g_queue_push_tail(operator_stack, tok_cp);
        }
    }

    // process rest of stack
    while (!g_queue_is_empty(operator_stack)) {
        ast_node node;
        _make_expression_node(&node, operator_stack, operand_stack);
        g_queue_push_tail(operand_stack, node);
    }

    head = g_queue_pop_tail(operand_stack);

    operator_stack_memstk->freed = true;
    operand_stack_memstk->freed = true;

    g_queue_free_full(operand_stack, _operand_stack_cleanup);
    g_queue_free_full(operator_stack, _operator_stack_cleanup);

    return head;
}

void ast_pp(ast_node head) {
    if (!head) return;
    switch (head->type) {
        case ast_declare:
            printf("(declare %s ", str_val(&(head->data->var_declare->name)));
            ast_pp(head->data->var_declare->value);
            printf(")");
            break;
        case ast_func_call:
            printf("(call %s ", str_val(&(head->data->fcall->name)));
            GList *iter;
            for (iter = head->data->fcall->args; iter != NULL;
                 iter = iter->next) {
                printf("(");
                ast_pp((ast_node)iter->data);
                printf(") ");
            }
            printf(")");
            break;
        case ast_identf:
            printf("%s", str_val(&head->data->val_str));
            break;
        case ast_str:
            printf("\"%s\"", str_val(&head->data->val_str));
            break;
        case ast_bool:
            printf("%s", head->data->val_bool ? "true" : "false");
            break;
        case ast_val_int:
            printf("%d", head->data->val_int);
            break;
        case ast_val_float:
            printf("%.2f", head->data->val_float);
            break;
        case ast_cond_is:
            _arithmetic_pp("eq", head);
            break;
        case ast_cond_is_not:
            _arithmetic_pp("neq", head);
            break;
        case ast_cond_greater:
            _arithmetic_pp("gr", head);
            break;
        case ast_cond_greater_eq:
            _arithmetic_pp("gre", head);
            break;
        case ast_cond_lesser:
            _arithmetic_pp("ls", head);
            break;
        case ast_cond_lesser_eq:
            _arithmetic_pp("lse", head);
            break;
        case ast_arithmetic_add:
            _arithmetic_pp("add", head);
            break;
        case ast_arithmetic_subtract:
            _arithmetic_pp("sub", head);
            break;
        case ast_arithmetic_multiply:
            _arithmetic_pp("mul", head);
            break;
        case ast_arithmetic_divide:
            _arithmetic_pp("div", head);
            break;
        default:
            printf("<Unknown>");
    }
}

static void _arithmetic_pp(char *label, ast_node node) {
    printf("(%s ", label);
    ast_pp(node->data->expression->left);
    printf(" ");
    ast_pp(node->data->expression->right);
    printf(")");
}

static bool _condition_block(token_list list) {
    if (list->tokens == NULL) return false;
    token first_tok = (token)list->tokens->data;
    return first_tok->type == token_when;
}

static bool _variable_declaration(token_list list) {
    GList *iter = list->tokens;
    if (!iter) return false;
    token first_tok = (token)iter->data;
    if (!iter->next) return false;
    iter = iter->next;
    token second_tok = (token)iter->data;
    if (first_tok != NULL && second_tok != NULL) {
        return list->size >= 3 && first_tok->type == token_identf &&
               second_tok->type == token_equal;
    }
    return false;
}

static bool _operator_token(token tok) {
    switch (tok->type) {
        case token_plus:
        case token_hyphen:
        case token_asterisk:
        case token_fslash:
        case token_percent:
        case token_not:
        case token_is:
        case token_isnot:
        case token_greater:
        case token_greater_eq:
        case token_less:
        case token_less_eq:
            return true;
        default:
            return false;
    }
}

static int _operator_precedence(token_type type) {
    switch (type) {
        case token_fslash:
            return 1;
        case token_asterisk:
            return 1;
        case token_percent:
            return 2;
        case token_plus:
            return 3;
        case token_hyphen:
            return 4;
        default:
            return 69;
    }
}

static void _extract_from_token(ast_node *node, token tok) {
    ast_data node_d;

    switch (tok->type) {
        case token_str: {
            memstk_node *node_d_memstk = ast_create_node_data(&node_d, ast_str);
            str_create(&node_d->val_str, tok->value->data);
            ast_create_node(node, ast_str, node_d);
            (*node)->data_memstk_node = node_d_memstk;
        } break;
        case token_num_int: {
            memstk_node *node_d_memstk =
                ast_create_node_data(&node_d, ast_val_int);
            node_d->val_int = atoi(tok->value->data);
            ast_create_node(node, ast_val_int, node_d);
            (*node)->data_memstk_node = node_d_memstk;
        } break;
        case token_num_float: {
            memstk_node *node_d_memstk =
                ast_create_node_data(&node_d, ast_val_float);
            node_d->val_float = atof(tok->value->data);
            ast_create_node(node, ast_val_float, node_d);
            (*node)->data_memstk_node = node_d_memstk;
        } break;
        case token_bool_t:
        case token_bool_f: {
            memstk_node *node_d_memstk =
                ast_create_node_data(&node_d, ast_bool);
            node_d->val_bool = tok->type == token_bool_t;
            ast_create_node(node, ast_bool, node_d);
            (*node)->data_memstk_node = node_d_memstk;
        } break;
        case token_identf: {
            memstk_node *node_d_memstk =
                ast_create_node_data(&node_d, ast_identf);
            str_cpy(&(node_d->val_str), &(tok->value));
            ast_create_node(node, ast_identf, node_d);
            (*node)->data_memstk_node = node_d_memstk;
        } break;
        default:
            err_throw(err_error, "Invalid expression");
            break;
    }
}

static void _make_expression_node(ast_node *node, GQueue *operator_stack,
                                  GQueue *operand_stack) {
    ast_node opr1 = g_queue_pop_tail(operand_stack);
    ast_node opr2 = g_queue_pop_tail(operand_stack);
    token opt = g_queue_pop_tail(operator_stack);

    if (opr1 == NULL || opr2 == NULL || opt == NULL) {
        return;
    }

    ast_data node_d;
    memstk_node *node_d_memstk = NULL;

    switch (opt->type) {
        case token_plus:
            node_d_memstk = ast_create_node_data(&node_d, ast_arithmetic_add);
            break;
        case token_hyphen:
            node_d_memstk =
                ast_create_node_data(&node_d, ast_arithmetic_subtract);
            break;
        case token_asterisk:
            node_d_memstk =
                ast_create_node_data(&node_d, ast_arithmetic_multiply);
            break;
        case token_fslash:
            node_d_memstk =
                ast_create_node_data(&node_d, ast_arithmetic_divide);
            break;
        case token_is:
            node_d_memstk = ast_create_node_data(&node_d, ast_cond_is);
            break;
        case token_isnot:
            node_d_memstk = ast_create_node_data(&node_d, ast_cond_is_not);
            break;
        case token_greater:
            node_d_memstk = ast_create_node_data(&node_d, ast_cond_greater);
            break;
        case token_greater_eq:
            node_d_memstk = ast_create_node_data(&node_d, ast_cond_greater_eq);
            break;
        case token_less:
            node_d_memstk = ast_create_node_data(&node_d, ast_cond_lesser);
            break;
        case token_less_eq:
            node_d_memstk = ast_create_node_data(&node_d, ast_cond_lesser_eq);
            break;
        default:
            err_throw(err_error, "Invalid arithmetic operator");
            break;
    }

    node_d->expression->left = opr2;
    node_d->expression->right = opr1;

    switch (opt->type) {
        case token_plus:
            ast_create_node(node, ast_arithmetic_add, node_d);
            break;
        case token_hyphen:
            ast_create_node(node, ast_arithmetic_subtract, node_d);
            break;
        case token_asterisk:
            ast_create_node(node, ast_arithmetic_multiply, node_d);
            break;
        case token_fslash:
            ast_create_node(node, ast_arithmetic_divide, node_d);
            break;
        case token_is:
            ast_create_node(node, ast_cond_is, node_d);
            break;
        case token_isnot:
            ast_create_node(node, ast_cond_is_not, node_d);
            break;
        case token_greater:
            ast_create_node(node, ast_cond_greater, node_d);
            break;
        case token_greater_eq:
            ast_create_node(node, ast_cond_greater_eq, node_d);
            break;
        case token_less:
            ast_create_node(node, ast_cond_lesser, node_d);
            break;
        case token_less_eq:
            ast_create_node(node, ast_cond_lesser_eq, node_d);
            break;
        default:
            err_throw(err_error, "Invalid arithmetic operator");
            break;
    }

    (*node)->data_memstk_node = node_d_memstk;

    token_destroy(&opt);
}

static void _operand_stack_cleanup(void *itm) {
    ast_node node = itm;
    if (node != NULL) {
        ast_destroy_node(&node);
    }
}

static void _operator_stack_cleanup(void *itm) {
    token tok = itm;
    if (tok != NULL) {
        token_destroy(&tok);
    }
}

static bool _function_call(token_list list) {
    GList *iter = list->tokens;
    if (!iter) return false;
    token first_tok = (token)iter->data;
    if (!iter->next) return false;
    iter = iter->next;
    token second_tok = (token)iter->data;
    if (first_tok != NULL && second_tok != NULL) {
        return first_tok->type == token_identf && !_operator_token(second_tok);
    }
    return false;
}

static void _ast_list_clean(void *lst) {
    GList *list = (GList *)lst, *iter;
    for (iter = list; iter != NULL; iter = iter->next) {
        ast_node node = (ast_node)iter->data;
        ast_destroy_node(&node);
    }

    g_list_free(list);
}

static void _ast_stack_clean(void *itm) {
    GQueue *stack = (GQueue *)itm;
    g_queue_free_full(stack, _operand_stack_cleanup);
}

static void _token_stack_clean(void *itm) {
    GQueue *stack = (GQueue *)itm;
    g_queue_free_full(stack, _operator_stack_cleanup);
}
