//
//-----------------------------------------------------------------------------
//Filename:    KompileUnit.cpp
//-----------------------------------------------------------------------------
//
// Created by bonett_w on 1/9/18.
//-----------------------------------------------------------------------------
//

#include <gtest/gtest.h>
#include <list>
#include "Kompiler.hpp"
#include "Lexer.hpp"
#include "Token.hpp"


TEST(Lexer, TokenIteration) {
    const std::string str("toto = asInt 1");

    std::list<Token> tokens{};

    ConstructiveIt constructiveIt{tokens, str.begin(), str.end(), false};
    ++constructiveIt;
    // DestructiveIt destructiveIt{tokens};
    EndIt endIt{};

    for (auto it=constructiveIt; it != endIt;) {
        ++it;
    }

    EXPECT_EQ(tokens.size(), 4);
}

TEST(Lexer, TokenIterationSpecialChar) {
    const std::string str("azertyuiop^$ù*ù*ù*ù*ù*ù*ù*ù*ù*ù");

    std::list<Token> tokens{};

    ConstructiveIt constructiveIt{tokens, str.begin(), str.end(), false};
    ++constructiveIt;
    // DestructiveIt destructiveIt{tokens};
    EndIt endIt{};

    for (auto it=constructiveIt; it != endIt;) {
        ++it;
    }

    EXPECT_EQ(tokens.size(), 2);
}

TEST(Lexer, TokenContent) {
    const std::string str("toto = asInt 1");


    std::list<Token> tokens{};

    ConstructiveIt constructiveIt{tokens, str.begin(), str.end(), false};
    ++constructiveIt;
    // DestructiveIt destructiveIt{tokens};
    EndIt endIt{};

    for (auto it=constructiveIt; it != endIt;) {

        // "toto" is TokenType::Name
        if (tokens.size() == 1) {
            EXPECT_STREQ(it->content.c_str(), "toto");
            EXPECT_EQ(it->type, TokenType::NAME);
        }

        // "=" is TokenType::Operator
        if (tokens.size() == 2) {
            EXPECT_STREQ(it->content.c_str(), "=");
            EXPECT_EQ(it->type, TokenType::OPERATOR);
        }

        // "asInt" is TokenTYpe::Name
        if (tokens.size() == 3) {
            EXPECT_STREQ(it->content.c_str(), "asInt");
            EXPECT_EQ(it->type, TokenType::NAME);
        }

        // "1" is TokenTYpe::CONSTANT
        if (tokens.size() == 4) {
            EXPECT_STREQ(it->content.c_str(), "1");
            EXPECT_EQ(it->type, TokenType::CONSTANT);
        }

        ++it;
    }

    EXPECT_EQ(tokens.size(), 4);
}

