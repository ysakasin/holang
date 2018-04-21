set(holang_src
    lexer.cpp
    object.cpp
    parser.cpp
    node/int_literal_node.cpp
    node/bool_literal_node.cpp
    node/string_literal_node.cpp
    node/ident_node.cpp
    node/assign_node.cpp
    node/binop_node.cpp
    node/exprs_node.cpp
    node/stmts_node.cpp
    node/if_node.cpp
    node/func_call_node.cpp
    node/func_def_node.cpp
    node/class_def_node.cpp
    node/sign_change_node.cpp
    node/prime_expr_node.cpp
    node/ref_field_node.cpp
    node/import_node.cpp
)

add_library(holang STATIC ${holang_src})
