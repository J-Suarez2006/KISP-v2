#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "scanner.hpp"

TEST_CASE("Testing keywords") {
    std::string source = "if else nil while for try"
    "catch continue break return print true false"
    "let self struct enum int double char bool string";

    std::vector<TypeToken> token_type_test = {
        IF, ELSE, NIL, WHILE, FOR, TRY, CATCH, CONTINUE,
        BREAK, RETURN, PRINT, BLIT_TRUE, BLIT_FALSE,
        LET, SELF, STRUCT, ENUM, DTYPE_INT, DTYPE_DOUBLE,
        DTYPE_CHAR, DTYPE_BOOL, DTYPE_STRING
    };

    Scanner scanner;
    scanner.loadSource(source);

    std::vector<Token> tokens = scanner.scan();

    for (int i = 0; i < tokens.size(); ++i)
    {
        CHECK(tokens[i].type == token_type_test[i]);
    }
}

