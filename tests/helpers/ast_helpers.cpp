#include "ast_helpers.h"

void check_identf(ast_node node, const char *value) {
    EXPECT_EQ(node->type, ast_identf);
    EXPECT_STREQ(str_val(&(node->data->val_str)), value);
}

void check_int(ast_node node, const int value) {
    EXPECT_EQ(node->type, ast_val_int);
    EXPECT_EQ(node->data->val_int, value);
}

void check_float(ast_node node, const float value) {
    EXPECT_EQ(node->type, ast_val_float);
    EXPECT_EQ(node->data->val_float, value);
}

void check_exp(ast_node node, ast_node_type type, exp_cb cb) {
    EXPECT_EQ(node->type, type);
    cb(node->data->expression);
}