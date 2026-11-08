#include "semantic-analysis.hpp"
#include "expression.hpp"
#include "scope.hpp"
#include "statement.hpp"
#include "token.hpp"
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <variant>

void SemanticAnalyzer::loadProgram(std::vector<std::unique_ptr<Stmt>> program)
{
    this->m_program = std::move(program);
    m_global = new Scope{
        .enclosing = nullptr
    };
    m_current = m_global;
    m_scopes.push_back(std::make_unique<Scope>(m_global));
}

void SemanticAnalyzer::analyze()
{
    resolveStructs(m_program);
    resolveFunctions(m_program);
    resolveVariables(m_program);
    inferTypes(m_program);
    typeCheck(m_program);
}

void SemanticAnalyzer::resolveStructs(Program& program)
{
    m_current = m_global;

    std::function<void(std::unique_ptr<Stmt>&)> resolveStatementStructs = [&](std::unique_ptr<Stmt>& stmt) -> void
    {
        if (stmt == nullptr) return;

        if (std::holds_alternative<StructDefStmt>(*stmt))
        {
            auto& struct_stmt = std::get<StructDefStmt>(*stmt);
            struct_stmt.scope = m_current;

            // throw exception if already declared
            if (m_current->structs.contains(struct_stmt.identifier))
            {
                throw std::runtime_error(
                    std::format("Error at line {}: double declaration of struct \'{}\'.",
                    struct_stmt.left_curly_brace.line, struct_stmt.identifier)
                );
            }

            uint64_t byte_offset = 0;
            Struct s;

            s.name = struct_stmt.identifier;

            // for definition in definitions - create a member
            for (auto& definition : struct_stmt.definitions)
            {
                if (!std::holds_alternative<VarDeclStmt>(*definition))
                {
                    throw std::runtime_error(
                        "Error: struct declarations can only have variable declarations.\n"
                    );
                }

                auto& var_decl = std::get<VarDeclStmt>(*definition);
                MemberVariable member = {
                    .type = getType(var_decl.data_type),
                    .memory_offset = byte_offset                    
                };

                byte_offset += getSize(member.type);
            }

            m_current->structs[struct_stmt.identifier] = s;
        }
        else if (std::holds_alternative<BlockStmt>(*stmt))
        {
            m_scopes.emplace_back(new Scope{.enclosing = m_current});
            m_current = &*m_scopes.back();
            std::get<BlockStmt>(*stmt).scope = m_current;

            resolveStructs(std::get<BlockStmt>(*stmt).block);

            m_current = m_current->enclosing;
        }
        else if (std::holds_alternative<VarDeclStmt>(*stmt))
        {
            auto& var_decl = std::get<VarDeclStmt>(*stmt);
            var_decl.scope = m_current;
        }
        else if (std::holds_alternative<FuncDefStmt>(*stmt))
        {
            auto& func_def = std::get<FuncDefStmt>(*stmt);
            func_def.scope = m_current;
        }
        else if (std::holds_alternative<IfStmt>(*stmt))
        {
            auto& if_stmt = std::get<IfStmt>(*stmt);
            if_stmt.scope = m_current;

            resolveStatementStructs(if_stmt.if_block);
            resolveStatementStructs(if_stmt.else_block);
        }
        else if (std::holds_alternative<WhileStmt>(*stmt))
        {
            auto& while_stmt = std::get<WhileStmt>(*stmt);
            while_stmt.scope = m_current;

            resolveStatementStructs(while_stmt.stmt);
        }
        else if (std::holds_alternative<ForStmt>(*stmt))
        {
            auto& for_stmt = std::get<ForStmt>(*stmt);
            for_stmt.scope = m_current;

            resolveStatementStructs(for_stmt.stmt);
        }
        else if (std::holds_alternative<ReturnStmt>(*stmt))
        {
            std::get<ReturnStmt>(*stmt).scope = m_current;
        }
        else if (std::holds_alternative<PrintStmt>(*stmt))
        {
            std::get<PrintStmt>(*stmt).scope = m_current;
        }
        else if (std::holds_alternative<ExprStmt>(*stmt))
        {
            std::get<ExprStmt>(*stmt).scope = m_current;
        }
    };

    for (auto& stmt : program)
    {
        resolveStatementStructs(stmt);
    }
}

void SemanticAnalyzer::resolveFunctions(Program &program)
{
    m_current = m_global;

    std::function<void(std::unique_ptr<Expr>&)> resolveExprFunctions = [&](std::unique_ptr<Expr>& expr) -> void 
    {
        if (expr == nullptr) return;

        if (std::holds_alternative<CallExpr>(*expr))
        {
            auto& call = std::get<CallExpr>(*expr);
            Scope* temp = m_current;
            
            // For now we will just support normal functions as a callable
            if (!std::holds_alternative<LiteralExpr>(*call.callee))
            {
                throw std::runtime_error(
                    std::format("Error at line {}: Expression is not a callable.\n",
                    call.l_paren.line)
                );
            }

            auto& callee = std::get<LiteralExpr>(*call.callee);
            if (callee.literal.type != IDENTIFIER)
            {
                throw std::runtime_error(
                    std::format("Error at line {}: Expression is not a callable.\n",
                    call.l_paren.line)
                );
            }

            while (temp != nullptr)
            {
                if (temp->functions.contains(callee.literal.lexeme))
                {
                    if (temp->functions.at(callee.literal.lexeme).parameters.size() != call.args.size())
                    {
                        throw std::runtime_error(
                            std::format("Error at line {}:" 
                                "Call expression has incorrect number of arguments.\n",
                            call.l_paren.line)
                        );
                    }

                    return;
                }

                temp = temp->enclosing;
            }

            throw std::runtime_error(
                std::format("Error at line {}: Function \'{}\' could not be found.\n", 
                    call.l_paren.line, callee.literal.lexeme
                )
            );

            for (auto& arg : call.args)
            {
                resolveExprFunctions(arg);
            }
        }
        else if (std::holds_alternative<LiteralExpr>(*expr))
        {
            return;
        }
        else if (std::holds_alternative<UnaryExpr>(*expr))
        {
            resolveExprFunctions(std::get<UnaryExpr>(*expr).expr);
        }
        else if (std::holds_alternative<BinaryExpr>(*expr))
        {
            resolveExprFunctions(std::get<BinaryExpr>(*expr).lhs);
            resolveExprFunctions(std::get<BinaryExpr>(*expr).rhs);
        }
        else if (std::holds_alternative<GroupedExpr>(*expr))
        {
            resolveExprFunctions(std::get<GroupedExpr>(*expr).expr);
        }
    };

    // each statement should be - switch scopes, go through statements, go through expressions
    std::function<void(std::unique_ptr<Stmt>&)> resolveStatementFunctions = [&](std::unique_ptr<Stmt>& stmt) -> void 
    {
        if (stmt == nullptr) return;

        if (std::holds_alternative<FuncDefStmt>(*stmt))
        {
            auto& func_def = std::get<FuncDefStmt>(*stmt);

            // throw exception if already declared
            if (m_current->functions.contains(func_def.identifier))
            {
                throw std::runtime_error(
                    std::format(
                        "Error at line {}: Double declaration of function \'{}\'.\n",
                        func_def.left_curly_brace.line, func_def.identifier
                    )
                );
            }

            std::vector<std::pair<Type, std::string>> parameters;
            for (auto& parameter : func_def.param)
            {
                parameters.push_back({getType(parameter.second), parameter.first});
            }

            m_current->functions[func_def.identifier] = Function{
                .return_type = getType(func_def.return_type),
                .parameters = parameters
            };

        }
        else if (std::holds_alternative<BlockStmt>(*stmt))
        {
            auto& block = std::get<BlockStmt>(*stmt);
            m_current = block.scope;
            
            for (auto& stmt : block.block)
            {
                resolveStatementFunctions(stmt);    
            }
        
            m_current = m_current->enclosing;
        }
        else if (std::holds_alternative<VarDeclStmt>(*stmt))
        {
            auto& var_decl = std::get<VarDeclStmt>(*stmt);
            
            resolveExprFunctions(var_decl.expr);
        }
        else if (std::holds_alternative<StructDefStmt>(*stmt))
        {
            return;
        }
        else if (std::holds_alternative<IfStmt>(*stmt))
        {
            auto& if_stmt = std::get<IfStmt>(*stmt);
            
            resolveExprFunctions(if_stmt.conditional);
            resolveStatementFunctions(if_stmt.if_block);
            resolveStatementFunctions(if_stmt.else_block);
        }
        else if (std::holds_alternative<WhileStmt>(*stmt))
        {
            auto& while_stmt = std::get<WhileStmt>(*stmt);

            resolveExprFunctions(while_stmt.conditional);
            resolveStatementFunctions(while_stmt.stmt);
        }
        else if (std::holds_alternative<ForStmt>(*stmt))
        {
            auto& for_stmt = std::get<ForStmt>(*stmt);
            
            resolveStatementFunctions(for_stmt.init);
            resolveStatementFunctions(for_stmt.conditional);
            resolveExprFunctions(for_stmt.update);
            resolveStatementFunctions(for_stmt.stmt);
        }
        else if (std::holds_alternative<ReturnStmt>(*stmt))
        {
            resolveExprFunctions(std::get<ReturnStmt>(*stmt).expr);
        }
        else if (std::holds_alternative<PrintStmt>(*stmt))
        {
            resolveExprFunctions(std::get<PrintStmt>(*stmt).expr);
        }
        else if (std::holds_alternative<ExprStmt>(*stmt))
        {
            resolveExprFunctions(std::get<ExprStmt>(*stmt).expr);
        }
    };

    for (auto& stmt : program)
    {
        resolveStatementFunctions(stmt);
    }
}

void SemanticAnalyzer::resolveVariables(Program &program)
{
    m_current = m_global;
    uint64_t current_stack_offset = 0;
    
    std::function<void(std::unique_ptr<Expr>&)> resolveExprVariables = [&](std::unique_ptr<Expr>& expr) -> void
    {
        if (expr == nullptr) return;

        if (std::holds_alternative<LiteralExpr>(*expr))
        {
            Token literal = std::get<LiteralExpr>(*expr).literal;

            if (literal.type != IDENTIFIER) return;

            Scope* temp = m_current;

            while (temp != nullptr)
            {
                if (temp->locals.contains(literal.lexeme))
                {
                    return;
                }

                temp = temp->enclosing;
            }

            throw std::runtime_error(
                std::format("Error at line {}: Undeclared identifier \'{}\'.\n", 
                    literal.line, literal.lexeme
                )
            );
        }
        else if (std::holds_alternative<CallExpr>(*expr))
        {
            auto& call = std::get<CallExpr>(*expr);
            
            for (auto& arg : call.args)
            {
                resolveExprVariables(arg);
            }
        }
        else if (std::holds_alternative<UnaryExpr>(*expr))
        {
            resolveExprVariables(std::get<UnaryExpr>(*expr).expr);
        }
        else if (std::holds_alternative<BinaryExpr>(*expr))
        {
            resolveExprVariables(std::get<BinaryExpr>(*expr).lhs);
            resolveExprVariables(std::get<BinaryExpr>(*expr).rhs);
        }
        else if (std::holds_alternative<GroupedExpr>(*expr))
        {
            resolveExprVariables(std::get<GroupedExpr>(*expr).expr);
        }
    };

    std::function<void(std::unique_ptr<Stmt>&)> resolveStatementVariables = [&](std::unique_ptr<Stmt>& stmt) -> void
    {
        if (stmt == nullptr) return;

        if (std::holds_alternative<VarDeclStmt>(*stmt))
        {
            auto& var_decl = std::get<VarDeclStmt>(*stmt);
            
            if (m_current->locals.contains(var_decl.identifier))
            {
                throw std::runtime_error(
                    std::format(
                        "Error at line {}: Double declaration of variable \'{}\'.\n",
                        var_decl.data_type.line, var_decl.identifier
                    )
                );
            }

            Local local{
                .type = getType(var_decl.data_type),
                .stack_offset = current_stack_offset
            };

            current_stack_offset += getSize(local.type);

            m_current->locals[var_decl.identifier] = local;
        }
        else if (std::holds_alternative<BlockStmt>(*stmt))
        {
            auto& block = std::get<BlockStmt>(*stmt);
            uint64_t temp = current_stack_offset;
            m_current = block.scope;
            current_stack_offset = 0;
            
            for (auto& stmt : block.block)
            {
                resolveStatementVariables(stmt);    
            }
        
            m_current = m_current->enclosing;
            current_stack_offset = temp;
        }
        else if (std::holds_alternative<FuncDefStmt>(*stmt))
        {
            auto& func_def = std::get<FuncDefStmt>(*stmt);

            resolveStatementVariables(func_def.function);
        }
        else if (std::holds_alternative<StructDefStmt>(*stmt))
        {
            return;
        }
        else if (std::holds_alternative<IfStmt>(*stmt))
        {
            auto& if_stmt = std::get<IfStmt>(*stmt);
            
            resolveExprVariables(if_stmt.conditional);
            resolveStatementVariables(if_stmt.if_block);
            resolveStatementVariables(if_stmt.else_block);
        }
        else if (std::holds_alternative<WhileStmt>(*stmt))
        {
            auto& while_stmt = std::get<WhileStmt>(*stmt);

            resolveExprVariables(while_stmt.conditional);
            resolveStatementVariables(while_stmt.stmt);
        }
        else if (std::holds_alternative<ForStmt>(*stmt))
        {
            auto& for_stmt = std::get<ForStmt>(*stmt);
            
            resolveStatementVariables(for_stmt.init);
            resolveStatementVariables(for_stmt.conditional);
            resolveExprVariables(for_stmt.update);
            resolveStatementVariables(for_stmt.stmt);
        }
        else if (std::holds_alternative<ReturnStmt>(*stmt))
        {
            resolveExprVariables(std::get<ReturnStmt>(*stmt).expr);
        }
        else if (std::holds_alternative<PrintStmt>(*stmt))
        {
            resolveExprVariables(std::get<PrintStmt>(*stmt).expr);
        }
        else if (std::holds_alternative<ExprStmt>(*stmt))
        {
            resolveExprVariables(std::get<ExprStmt>(*stmt).expr);
        }
    };

    for (auto& stmt : program)
    {
        resolveStatementVariables(stmt);
    }
}

void SemanticAnalyzer::inferTypes(Program &program)
{

}

void SemanticAnalyzer::typeCheck(Program &program)
{

}