set(holang_src
    lexer.cpp
    object.cpp
    parser.cpp
)

add_library(holang STATIC ${holang_src})
