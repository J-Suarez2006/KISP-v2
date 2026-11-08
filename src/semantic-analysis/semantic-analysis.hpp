#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "expression.hpp"
#include "scope.hpp"
#include "statement.hpp"
#include "token.hpp"

#include <memory>
#include <stdexcept>
#include <format>
#include <variant>
#include <functional>

using Program = std::vector<std::unique_ptr<Stmt>>;

/**
 * @class this is our semantic analyzer class. for now, we will just do simple
 * type checking, name checking, scope resolution. we will not allow for function overloading
*/
class SemanticAnalyzer
{   
private:
    Scope* m_global;
    Scope* m_current;
    std::vector<std::unique_ptr<Scope>> m_scopes;
    
    Program m_program;

    /**
     * establishes byte offsets for all declared structs
     */
    void resolveStructs(Program& program);
    
    /**
     * connects all function calls to their respective function declarations
     */
    void resolveFunctions(Program& program);

    /**
     * establishes stack offsets for local variables
     */
    void resolveVariables(Program& program);

    /**
     * infers types of expressions
     */
    void inferTypes(Program& program);

    /**
     * checks if variable assignment, return values, etc are of the correct type
     */
    void typeCheck(Program& program);

    inline Type getType(Token& token)
    {
        if (token.type == IDENTIFIER)
        {
            Scope* temp = m_current;

            while (temp != nullptr)
            {
                if (temp->structs.contains(token.lexeme))
                {
                    return &temp->structs[token.lexeme];
                }

                temp = temp->enclosing;
            }

            throw std::runtime_error(
                std::format("Error at line {}: Datatype \'{}\' could not be found.", 
                    token.line, token.lexeme
                )
            );
        }

        switch(token.type)
        {
            case DTYPE_INT:
            return DATATYPE::DT_INT;
            case DTYPE_BOOL:
            return DATATYPE::DT_BOOL;
            case DTYPE_CHAR:
            return DATATYPE::DT_CHAR;
            case DTYPE_UINT:
            return DATATYPE::DT_UINT;
            case DTYPE_FLOAT:
            return DATATYPE::DT_FLOAT;
            default:
            return DATATYPE::DT_VOID;
        }

        throw std::invalid_argument(std::format("Invalid datatype at line {}.", token.line));
        return DATATYPE::DT_VOID;
    }

    inline unsigned int getSize(Type type)
    {
        if (std::holds_alternative<DATATYPE>(type))
        {
            switch (std::get<DATATYPE>(type)) {
                case DATATYPE::DT_BOOL:
                case DATATYPE::DT_CHAR:
                case DATATYPE::DT_STRING:
                case DATATYPE::DT_VOID:
                return 1;

                case DATATYPE::DT_INT:
                case DATATYPE::DT_UINT:
                case DATATYPE::DT_FLOAT:
                return 4;

                case DATATYPE::DT_DOUBLE:
                return 8;
            }
        }

        int size = 0;
        auto& s = *std::get<Struct*>(type);

        for (auto& [id, member] : s.member_variables)
        {
            size += getSize(member.type);
        }

        return size;
    }

public:
    SemanticAnalyzer();

    void loadProgram(std::vector<std::unique_ptr<Stmt>> program);
    void analyze();

};

#endif