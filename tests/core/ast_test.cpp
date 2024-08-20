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

    check_int(n, 69);

    ast_destroy_node(&n);
    EXPECT_EQ(n, nullptr);

    ast_data d2 = (ast_data)malloc(sizeof(float));
    d2->val_float = 69.42;
    ast_create_node(&n, ast_val_float, d2);

    check_float(n, 69.42);

    ast_destroy_node(&n);
    EXPECT_EQ(n, nullptr);
}

TEST(AstTest, TestArithmetic) {
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "\t1.414 + 1 * 3");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_arithmetic_add,
                  [](ast_expression_data adddata) -> void {
                      check_float(adddata->left, 1.414);

                      check_exp(adddata->right, ast_arithmetic_multiply,
                                [](ast_expression_data muldata) -> void {
                                    check_int(muldata->left, 1);
                                    check_int(muldata->right, 3);
                                });
                  });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }

    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "1 + 2 * 2.2 / 3");
        ast_node node = ast_parse_expression(list);

        check_exp(
            node, ast_arithmetic_add, [](ast_expression_data adddata) -> void {
                check_int(adddata->left, 1);

                check_exp(adddata->right, ast_arithmetic_divide,
                          [](ast_expression_data divdata) -> void {
                              check_exp(
                                  divdata->left, ast_arithmetic_multiply,
                                  [](ast_expression_data muldata) -> void {
                                      check_int(muldata->left, 2);
                                      check_float(muldata->right, 2.2);
                                  });

                              check_int(divdata->right, 3);
                          });
            });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }

    {
        token_list list;
        token_list_create(&list);

        parse_line(&list, "(1 + 2) * 2.2 / 3");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_arithmetic_divide,
                  [](ast_expression_data divdata) -> void {
                      check_exp(
                          divdata->left, ast_arithmetic_multiply,
                          [](ast_expression_data muldata) -> void {
                              check_exp(
                                  muldata->left, ast_arithmetic_add,
                                  [](ast_expression_data adddata) -> void {
                                      check_int(adddata->left, 1);
                                      check_int(adddata->right, 2);
                                  });

                              check_float(muldata->right, 2.2);
                          });

                      check_int(divdata->right, 3);
                  });

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

        check_exp(node, ast_cond_is, [](ast_expression_data data) -> void {
            check_identf(data->left, "sidd");
            check_int(data->right, 33);
        });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "notsidd is not 11*3");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_cond_is_not, [](ast_expression_data data) -> void {
            check_identf(data->left, "notsidd");
            check_exp(data->right, ast_arithmetic_multiply,
                      [](ast_expression_data adddata) -> void {
                          check_int(adddata->left, 11);
                          check_int(adddata->right, 3);
                      });
        });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "h <= 28");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_cond_lesser_eq,
                  [](ast_expression_data data) -> void {
                      check_identf(data->left, "h");
                      check_int(data->right, 28);
                  });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "a > 10 and b <= 12");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_logical_and,
                  [](ast_expression_data and_data) -> void {
                      check_exp(and_data->left, ast_cond_greater,
                                [](ast_expression_data gr_data) -> void {
                                    check_identf(gr_data->left, "a");
                                    check_int(gr_data->right, 10);
                                });
                      check_exp(and_data->right, ast_cond_lesser_eq,
                                [](ast_expression_data lse_data) -> void {
                                    check_identf(lse_data->left, "b");
                                    check_int(lse_data->right, 12);
                                });
                  });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "a > 10 or b <= 12");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_logical_or,
                  [](ast_expression_data and_data) -> void {
                      check_exp(and_data->left, ast_cond_greater,
                                [](ast_expression_data gr_data) -> void {
                                    check_identf(gr_data->left, "a");
                                    check_int(gr_data->right, 10);
                                });
                      check_exp(and_data->right, ast_cond_lesser_eq,
                                [](ast_expression_data lse_data) -> void {
                                    check_identf(lse_data->left, "b");
                                    check_int(lse_data->right, 12);
                                });
                  });

        ast_destroy_node(&node);
        token_list_destroy(&list);
        memstk_clean();
    }
    {
        token_list list;
        token_list_create(&list);
        parse_line(&list, "not a > 10");
        ast_node node = ast_parse_expression(list);

        check_exp(node, ast_logical_not,
                  [](ast_expression_data and_data) -> void {
                      check_exp(and_data->right, ast_cond_greater,
                                [](ast_expression_data gr_data) -> void {
                                    check_identf(gr_data->left, "a");
                                    check_int(gr_data->right, 10);
                                });
                  });

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
