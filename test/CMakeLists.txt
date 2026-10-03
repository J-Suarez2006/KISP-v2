
add_executable(scanner-test scanner/scanner_test.cpp)
target_include_directories(scanner-test
PRIVATE
    ../src
    ../src/scanner
    ../vendor
)
target_sources(scanner-test 
PRIVATE 
    ../src/scanner/scanner.cpp
)
add_test(NAME SCANNER_TEST COMMAND scanner-test)

add_executable(parser-test parser/parser_test.cpp)
target_include_directories(parser-test
PRIVATE
    ../src 
    ../src/scanner
    ../src/parser 
    ../vendor
)
target_sources(parser-test 
PRIVATE 
    ../src/scanner/scanner.cpp
    ../src/parser/parser.cpp
)
add_test(NAME PARSER_TEST COMMAND parser-test)