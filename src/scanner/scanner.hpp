#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "token.hpp"
#include "error.hpp"

#include <unordered_map>
#include <vector>
#include <string>

class Scanner
{
    private:
    std::unordered_map<std::string, TypeToken> m_keywords;
    int m_idx;
    int m_line;
    std::string m_accumulator;
    std::vector<Token> m_tokens;
    std::string m_source;

    void scanCharacter();
    void scanIdentifierOrKeyword();
    void addAccumulator();

    inline void addToken(TypeToken token_type, std::string lexeme)
    {
        addAccumulator();

        m_tokens.emplace_back(Token{
            .type = token_type,
            .lexeme = lexeme,
            .line = m_line
        });
    }

    inline bool lookAheadIs(char expected)
    {
        return (m_idx + 1 < m_source.size()) && (m_source[m_idx + 1] == expected);
    }

    public:
    Scanner();

    void loadSource(std::string source_code);
    std::vector<Token> scan();
};

#endif