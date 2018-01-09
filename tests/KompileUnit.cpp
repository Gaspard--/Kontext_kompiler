//
//-----------------------------------------------------------------------------
//Filename:    KompileUnit.cpp
//-----------------------------------------------------------------------------
//
// Created by bonett_w on 1/9/18.
//-----------------------------------------------------------------------------
//

#include <gtest/gtest.h>
#include "Kompiler.hpp"

TEST(BasicTests, InstanceKompiler) {
    Kompiler kompiler;
    // std::istringstream stream("toto = asInt 1\n");

    // TODO : Implement unit tests.
    // kompiler.process(stream);
    EXPECT_EQ(1, 1);
}