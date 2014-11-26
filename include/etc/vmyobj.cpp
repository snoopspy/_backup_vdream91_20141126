#include <stdio.h> // gilgil temp

#include "vmyobj.h"

VMyObj::VMyObj(QObject *parent) :
QObject(parent)
{
	printf("VMyObj::VMyObj(QObject *parent)\n"); // gilgil temp
}

#ifdef GTEST
#include <gtest/gtest.h>
#include <stdio.h>

TEST(VMyObj, test)
{
	printf("TEST(VMyObj, test)\n"); // gilgil temp
}

#endif // GTEST
