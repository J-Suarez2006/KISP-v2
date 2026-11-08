#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "token.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

enum class DATATYPE : uint8_t
{
    DT_INT, DT_UINT, DT_FLOAT, DT_DOUBLE, DT_BOOL, DT_CHAR, DT_STRING, DT_VOID
};

struct Struct;

/*
 * @note DATATYPE is for primitive datatypes, std::string is for structs.
*/
using Type = std::variant<DATATYPE, Struct*>;

struct MemberVariable
{
    Type type;
    uint64_t memory_offset;
};

struct Struct
{
    std::string name;
    std::unordered_map<std::string, MemberVariable> member_variables;
};

inline bool operator==(Type lhs, const Token& rhs)
{
    if (std::holds_alternative<DATATYPE>(lhs))
    {
        auto dt = std::get<DATATYPE>(lhs);
        if (dt == DATATYPE::DT_INT) return rhs.type == DTYPE_INT;
        if (dt == DATATYPE::DT_UINT) return rhs.type == DTYPE_UINT;
        if (dt == DATATYPE::DT_FLOAT) return rhs.type == DTYPE_FLOAT;
        if (dt == DATATYPE::DT_DOUBLE) return rhs.type == DTYPE_DOUBLE;
        if (dt == DATATYPE::DT_BOOL) return rhs.type == DTYPE_BOOL;
        if (dt == DATATYPE::DT_CHAR) return rhs.type == DTYPE_CHAR;
        if (dt == DATATYPE::DT_STRING) return rhs.type == DTYPE_STRING;
        if (dt == DATATYPE::DT_VOID) return rhs.type == DTYPE_VOID;

        return false;
    }

    if (rhs.type != IDENTIFIER) return false;
    
    return std::get<Struct*>(lhs)->name == rhs.lexeme;
}

inline bool operator==(const Token& lhs, Type rhs)
{
    return rhs == lhs;
}

/**
 * @struct contains metadata about a function. does not contain the actual
 * statements. 
*/
struct Function
{
    Type return_type;
    std::vector<std::pair<Type, std::string>> parameters;
};

struct Local 
{
    Type type;
    uint64_t stack_offset;
};

struct Scope
{
    Scope* enclosing;
    std::unordered_map<std::string, Struct> structs;
    std::unordered_map<std::string, Function> functions;
    std::unordered_map<std::string, Local> locals;
};

#endif