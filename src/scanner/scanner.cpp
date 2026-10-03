#include "scanner.hpp"

Scanner::Scanner()
: m_idx(0), m_line(0), m_accumulator(""), m_tokens({}), m_source("")
{
    m_keywords = {
        {"if", IF},
        {"else", ELSE},
        {"nil", NIL},
        {"while", WHILE},
        {"for", FOR},
        {"try", TRY},
        {"catch", CATCH},
        {"continue", CONTINUE},
        {"break", BREAK},
        {"return", RETURN},
        {"print", PRINT},
        {"true", BLIT_TRUE},
        {"false", BLIT_FALSE},
        {"let", LET},
        {"self", SELF},
        {"func", FUNC},
        {"struct", STRUCT},
        {"enum", ENUM},
        {"int", DTYPE_INT},
        {"double", DTYPE_DOUBLE},
        {"char", DTYPE_CHAR},
        {"bool", DTYPE_BOOL},
        {"string", DTYPE_STRING}
    };
}

void Scanner::loadSource(std::string source_code)
{
    m_source = source_code;
}
void Scanner::addAccumulator()
{
    if (m_accumulator == "") return;

    if (!isalpha(m_accumulator[0]))
    {
        std::string message {"Invalid identifier '" + m_accumulator + "'. Identifiers must begin with an alphabetical character."};
        syntaxError(m_line, message);  
    }
    else
    {
        scanIdentifierOrKeyword();
    }

    m_accumulator = "";
}

void Scanner::scanIdentifierOrKeyword()
{
    if (!m_keywords.contains(m_accumulator))
    {
        m_tokens.emplace_back(Token{
            .type = IDENTIFIER,
            .lexeme = m_accumulator,
            .line = m_line
        });
        
        return;
    }

    m_tokens.emplace_back(Token{
        .type = m_keywords.at(m_accumulator),
        .lexeme = m_accumulator,
        .line = m_line
    });
}

void Scanner::scanCharacter()
{
    switch(m_source[m_idx])
    {
    case ' ':
    case '\r':
    case '\t':
    // ignores whitespace
        addAccumulator();
        break;
    case '\n': 
        addAccumulator();
        ++m_line; 
        break;
    case ',': addToken(COMMA, ","); break;
    case '.': addToken(DOT, "."); break;
    case '(': addToken(LEFT_PAREN, "("); break;
    case ')': addToken(RIGHT_PAREN, ")"); break;
    case '{': addToken(LEFT_CURLY_BRACE, "{"); break;
    case '}': addToken(RIGHT_CURLY_BRACE, "}"); break;
    case '[': addToken(LEFT_SQUARE_BRACE, "["); break;
    case ']': addToken(RIGHT_SQUARE_BRACE, "]"); break;
    case ';': addToken(SEMICOLON, ";"); break;  
    case ':': addToken(COLON, ":"); break;
    case '=':
        if (lookAheadIs('=')){
            addToken(EQUALS_EQUALS, "==");
            ++m_idx;
        }   
        else addToken(EQUALS, "=");
        break; 
    case '/':
        if (lookAheadIs('/')){
            while(!lookAheadIs('\n')) ++m_idx;
        }   
        else if (lookAheadIs('='))
        {
            addToken(SLASH_EQUALS, "/=");
            ++m_idx;
        }
        else addToken(SLASH, "/");
        break;
    case '-':
        if (lookAheadIs('=')){
            addToken(MINUS_EQUALS, "-=");
            ++m_idx;
        }  
        else if (lookAheadIs('-')){
            addToken(DECREMENT, "--");
            ++m_idx;
        }   
        else if (lookAheadIs('>')){
            addToken(ARROW, "->");
            ++m_idx;
        }
        else addToken(MINUS, "-");
        break; 
    case '+':
        if (lookAheadIs('=')){
            addToken(PLUS_EQUALS, "+=");
            ++m_idx;
        }  
        else if (lookAheadIs('+')){
            addToken(INCREMENT, "++");
            ++m_idx;
        }   
        else addToken(PLUS, "+");
        break; 
    case '*':
        if (lookAheadIs('=')){
            addToken(ASTERISK_EQUALS, "*=");
            ++m_idx;
        }  
        else addToken(ASTERISK, "*");
        break; 
    case '%':
        if (lookAheadIs('=')){
            addToken(MODULUS_EQUALS, "%=");
            ++m_idx;
        }  
        else addToken(MODULUS, "%");
        break; 
    case '!':
        if (lookAheadIs('=')){
            addToken(BANG_EQUALS, "!=");
            ++m_idx;
        }  
        else addToken(BANG, "!");
        break; 
    case '>':
        if (lookAheadIs('=')){
            addToken(GREATER_OR_EQUAL, ">=");
            ++m_idx;
        }  
        else addToken(GREATER_THAN, ">");
        break;
    case '<':
        if (lookAheadIs('=')){
            addToken(LESS_OR_EQUAL, "<=");
            ++m_idx;
        }  
        else addToken(LESS_THAN, "<");
        break; 
    case '&':
        if (lookAheadIs('&')){
            addToken(AND_AND, "&&");
            ++m_idx;
        }  
        else syntaxError(m_line, "'&' operator expected second &.");
        break; 
    case '|':
        if (lookAheadIs('|')){
            addToken(PIPE_PIPE, "||");
            ++m_idx;
        }  
        else syntaxError(m_line, "'|' operator expected |.");
        break; 
    case '"':
        addAccumulator();
        while (!lookAheadIs('"') && m_idx < m_source.size())
        {
            if (lookAheadIs('\n')) ++m_line;

            ++m_idx;
            m_accumulator += m_source[m_idx];
        }
        if (m_idx < m_source.size()) 
        {
            m_tokens.emplace_back(Token{
                .type = STRING_LITERAL,
                .lexeme = m_accumulator,
                .line = m_line

            });
        }
        else syntaxError(m_line, "unterminated string.");
        ++m_idx;
        m_accumulator = "";
        break;
    default:
        if (std::isdigit(m_source[m_idx]) && m_accumulator == "")
        {
            while (std::isdigit(m_source[m_idx])) 
            {
                m_accumulator += m_source[m_idx];
                ++m_idx;
            }
            if (m_source[m_idx] == '.' && (m_idx + 1) < m_source.size() && std::isdigit(m_source[m_idx + 1]))
            {
                m_accumulator += m_source[m_idx];
                ++m_idx;

                while (std::isdigit(m_source[m_idx]))
                {
                    m_accumulator += m_source[m_idx];
                    ++m_idx;
                }
            }

            m_tokens.emplace_back(Token{
                .type = NUMBER,
                .lexeme = m_accumulator,
                .line = m_line
            });
            m_accumulator = "";;
            --m_idx;
        }
        else
        {
            m_accumulator += m_source[m_idx];
        }
        break;
    }

}

std::vector<Token> Scanner::scan()
{
    while(m_idx < m_source.size())
    {   
        scanCharacter();        
        ++m_idx;
    }
    
    addToken(END_OF_FILE, "end of file");

    return m_tokens;
}