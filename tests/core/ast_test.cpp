#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>

#include "glib.h"

extern "C" {
#include "core/ast.h"
#include "core/lexer.h"
#include "core/token.h"
#include "utils/memstk.h"
}

#include "../helpers/ast_helpers.h"
#include "../helpers/token_list_helpers.h"

TEST(AstTest, TestNode) {
    ast_node n;

    ast_data d1 = (ast_data)malloc(sizeof(int));
    d1->val_int = 69;
    ast_create_node(&n, ast_val_int, d1);
    EXPECT_EQ(n->data->val_int, 69);
    ast_destroy_node(&n);
    EXPECT_EQ(n, nullptr);

    ast_data d2 = (ast_data)malloc(sizeof(float));
    d2->val_float = 69.42;
    ast_create_node(&n, ast_val_float, d2);
    EXPECT_FLOAT_EQ(n->data->val_float, 69.42);
    ast_destroy_node(&n);
    EXPECT_EQ(n, nullptr);
}

TEST(AstTest, TestArithmetic) {
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "\t1.414 + 1 * 3");
        ast_node node = ast_parse_expression(list);

        EXPECT_EQ(node->type, ast_arithmetic_add);
        EXPECT_EQ(node->data->expression->left->type, ast_val_float);
        EXPECT_FLOAT_EQ(node->data->expression->left->data->val_float, 1.414);
        EXPECT_EQ(node->data->expression->right->type, ast_arithmetic_multiply);
        EXPECT_EQ(node->data->expression->right->data->expression->left->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->expression->left->data
                      ->val_int,
                  1);
        EXPECT_EQ(node->data->expression->right->data->expression->right->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->expression->right->data
                      ->val_int,
                  3);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }

    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "1 + 2 * 2.2 / 3");
        ast_node node = ast_parse_expression(list);

        EXPECT_EQ(node->type, ast_arithmetic_add);
        EXPECT_EQ(node->data->expression->left->type, ast_val_int);
        EXPECT_EQ(node->data->expression->left->data->val_int, 1);
        EXPECT_EQ(node->data->expression->right->type, ast_arithmetic_divide);
        EXPECT_EQ(node->data->expression->right->data->expression->right->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->expression->right->data
                      ->val_int,
                  3);
        EXPECT_EQ(node->data->expression->right->data->expression->left->type,
                  ast_arithmetic_multiply);
        EXPECT_EQ(node->data->expression->right->data->expression->left->data
                      ->expression->left->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->expression->left->data
                      ->expression->left->data->val_int,
                  2);
        EXPECT_EQ(node->data->expression->right->data->expression->left->data
                      ->expression->right->type,
                  ast_val_float);
        EXPECT_FLOAT_EQ(node->data->expression->right->data->expression->left
                            ->data->expression->right->data->val_float,
                        2.2);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }

    {
        token_list list;
        token_list_create(&list);

        parse_line(&list, "(1 + 2) * 2.2 / 3");
        ast_node node = ast_parse_expression(list);

        EXPECT_EQ(node->type, ast_arithmetic_divide);
        EXPECT_EQ(node->data->expression->right->type, ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->val_int, 3);
        EXPECT_EQ(node->data->expression->left->type, ast_arithmetic_multiply);
        EXPECT_EQ(node->data->expression->left->data->expression->right->type,
                  ast_val_float);
        EXPECT_FLOAT_EQ(node->data->expression->left->data->expression->right
                            ->data->val_float,
                        2.20);
        EXPECT_EQ(node->data->expression->left->data->expression->left->type,
                  ast_arithmetic_add);
        EXPECT_EQ(node->data->expression->left->data->expression->left->data
                      ->expression->left->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->left->data->expression->left->data
                      ->expression->left->data->val_int,
                  1);
        EXPECT_EQ(node->data->expression->left->data->expression->left->data
                      ->expression->right->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->left->data->expression->left->data
                      ->expression->right->data->val_int,
                  2);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
}

TEST(AstTest, TestBoolExp) {
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "sidd is 33");
        ast_node node = ast_parse_expression(list);

        EXPECT_EQ(node->type, ast_cond_is);
        EXPECT_EQ(node->data->expression->left->type, ast_identf);
        EXPECT_STREQ(str_val(&(node->data->expression->left->data->val_str)),
                     "sidd");
        EXPECT_EQ(node->data->expression->right->type, ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->val_int, 33);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "notsidd is not 11*3");
        ast_node node = ast_parse_expression(list);
        token_list_pp(list);
        EXPECT_EQ(node->type, ast_cond_is_not);
        EXPECT_EQ(node->data->expression->left->type, ast_identf);
        EXPECT_STREQ(str_val(&(node->data->expression->left->data->val_str)),
                     "notsidd");
        EXPECT_EQ(node->data->expression->right->type, ast_arithmetic_multiply);
        EXPECT_EQ(node->data->expression->right->data->expression->left->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->expression->left->data
                      ->val_int,
                  11);
        EXPECT_EQ(node->data->expression->right->data->expression->right->type,
                  ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->expression->right->data
                      ->val_int,
                  3);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "h <= 28");
        ast_node node = ast_parse_expression(list);
        token_list_pp(list);
        EXPECT_EQ(node->type, ast_cond_lesser_eq);
        EXPECT_EQ(node->data->expression->left->type, ast_identf);
        EXPECT_STREQ(str_val(&(node->data->expression->left->data->val_str)),
                     "h");
        EXPECT_EQ(node->data->expression->right->type, ast_val_int);
        EXPECT_EQ(node->data->expression->right->data->val_int, 28);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "a > 10 and b <= 12");
        ast_node node = ast_parse_expression(list);

        EXPECT_EQ(node->type, ast_logical_and);
        EXPECT_EQ(node->data->expression->left->type, ast_cond_greater);
        check_identf(node->data->expression->left->data->expression->left, "a");
        check_int(node->data->expression->left->data->expression->right, 10);

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
}

TEST(AstTest, TestVariableDeclaration) {
    token_list list;
    token_list_create(&list);
    ast_node node = NULL;

    char stm1[] = "a = 69.420";
    parse_line(&list, stm1);

    node = ast_parse_variable_declaration(list);
    EXPECT_EQ(node->type, ast_declare);
    EXPECT_STREQ(str_val(&node->data->var_declare->name), "a");
    EXPECT_EQ(node->data->var_declare->value->type, ast_val_float);
    EXPECT_FLOAT_EQ(node->data->var_declare->value->data->val_float, 69.420);
    ast_destroy_node(&node);
    token_list_clear(&list);
    node = NULL;

    char stm2[] = "name = \"Siddeshwar\"";
    parse_line(&list, stm2);
    node = ast_parse_variable_declaration(list);
    EXPECT_EQ(node->type, ast_declare);
    EXPECT_STREQ(str_val(&node->data->var_declare->name), "name");
    EXPECT_EQ(node->data->var_declare->value->type, ast_str);
    EXPECT_STREQ(str_val(&node->data->var_declare->value->data->val_str),
                 "Siddeshwar");
    ast_destroy_node(&node);
    token_list_clear(&list);
    node = NULL;

    token_list_destroy(&list);
}

TEST(AstTest, TestFunctionCall) {
    token_list list;
    token_list_create(&list);

    {
        parse_line(&list, "say \"hello\"\n");
        ast_node node = ast_parse_function_call(list);

        EXPECT_EQ(node->type, ast_func_call);
        EXPECT_STREQ(str_val(&node->data->fcall->name), "say");
        ast_node first_arg =
            (ast_node)g_list_nth_data(node->data->fcall->args, 0);
        EXPECT_EQ(first_arg->type, ast_str);
        EXPECT_STREQ(str_val(&first_arg->data->val_str), "hello");

        ast_destroy_node(&node);
        token_list_clear(&list);
    }

    {
        parse_line(&list, "say \"hello\", \"world\"\n");
        ast_node node = ast_parse_function_call(list);

        EXPECT_EQ(node->type, ast_func_call);
        EXPECT_STREQ(str_val(&node->data->fcall->name), "say");
        ast_node first_arg =
            (ast_node)g_list_nth_data(node->data->fcall->args, 0);
        EXPECT_EQ(first_arg->type, ast_str);
        EXPECT_STREQ(str_val(&first_arg->data->val_str), "hello");
        ast_node sec_arg =
            (ast_node)g_list_nth_data(node->data->fcall->args, 1);
        EXPECT_EQ(sec_arg->type, ast_str);
        EXPECT_STREQ(str_val(&sec_arg->data->val_str), "world");

        ast_destroy_node(&node);
        token_list_clear(&list);
    }

    token_list_destroy(&list);
}
