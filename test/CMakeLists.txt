
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