#include "LIB_memory.hh"
#include "LIB_utildefines.h"

#include "gtest/gtest.h"

namespace {

int __cdecl Trivial_CdeclFunction1(int a, int b) {
    return a + b;
}

TEST(Detours, CDeclAttach1) {
	void* vTrivial_CdeclFunction = NewDetourEx((void*)&Trivial_CdeclFunction1, (std::function<int (int ,int)>)[&](int a, int b) -> int {
		auto fn = reinterpret_cast<int(*__cdecl)(int, int)>(vTrivial_CdeclFunction);
		EXPECT_EQ(a, 10);
		EXPECT_EQ(b, 20);
		EXPECT_EQ(fn(a, b), a + b);
		return 0xdeadbeef;
	});
	
	auto mTrivial_CDeclFunction = reinterpret_cast<int(__cdecl*)(int, int)>(vTrivial_CdeclFunction);
	EXPECT_EQ(mTrivial_CDeclFunction(10, 20), 10 + 20); // Calling the original should be working!
	EXPECT_EQ(Trivial_CdeclFunction1(10, 20), 0xdeadbeef); // Calling the function should be detoured to ours!
}

int __cdecl Trivial_CdeclFunction2(int a, int b) {
	return a * b;
}

TEST(Detours, CDeclAttach2) {
	void* vTrivial_CdeclFunction = NewDetourEx((void*)&Trivial_CdeclFunction2, (std::function<int(int, int)>)[&](int a, int b) -> int {
		auto fn = reinterpret_cast<int(*__cdecl)(int, int)>(vTrivial_CdeclFunction);
		EXPECT_EQ(a, 10);
		EXPECT_EQ(b, 20);
		EXPECT_EQ(fn(a, b), a * b);
		return 0xdeadbeef;
	});

	auto mTrivial_CDeclFunction = reinterpret_cast<int(__cdecl*)(int, int)>(vTrivial_CdeclFunction);
	EXPECT_EQ(mTrivial_CDeclFunction(10, 20), 10 * 20); // Calling the original should be working!
	EXPECT_EQ(Trivial_CdeclFunction2(10, 20), 0xdeadbeef); // Calling the function should be detoured to ours!
}

}  // namespace
