// Even though this is a C++ testing framework, I am taking
// care to only use the C interface to the locale functionality
// to demonstrate the lowest-level of the error.

#include <array>
#include <cerrno>
#include <clocale>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <type_traits>

#include <gtest/gtest.h>


constexpr ::ssize_t MAXLEN = 1000;

// Unified char/wchar interface for xfrm functions
template <typename T>
std::array<T, MAXLEN> transform(const T* in)
{
    std::array<T, MAXLEN> out = {};
    if constexpr(std::is_same<T, char>::value) {
        out.fill('\0');
        ::strxfrm(out.data(), in, MAXLEN);
    } else {
        out.fill(L'\0');
        ::wcsxfrm(out.data(), in, MAXLEN);
    }
    return out;
}

// Unified char/wchar interface for coll functions
template <typename T>
int collate(const T* one, const T* two)
{
    if constexpr(std::is_same<T, char>::value) {
        return ::strcoll(one, two);
    } else {
        return ::wcscoll(one, two);
    }
}

// Unified char/wchar interface for cmp functions
template <typename T>
int compare(const T* one, const T* two)
{
    if constexpr(std::is_same<T, char>::value) {
        return ::strcmp(one, two);
    } else {
        return ::wcscmp(one, two);
    }
}
template <typename T>
int compare(const std::array<T, MAXLEN>& one, const std::array<T, MAXLEN>& two)
{
    if constexpr(std::is_same<T, char>::value) {
        return ::strcmp(one.data(), two.data());
    } else {
        return ::wcscmp(one.data(), two.data());
    }
}


class BrokenLocaleDemo : public ::testing::Test {
protected:
    BrokenLocaleDemo() {
        // Reset errno
        errno = 0;
        // Set locale to "C"
        ::setlocale(LC_ALL, "C");
    }
    virtual ~BrokenLocaleDemo() override {
        // Reset locale to "C"
        ::setlocale(LC_ALL, "C");
    }
};


TEST_F(BrokenLocaleDemo, XfrmCollate_C) {
    // With the "C" locale, "a" always compares greater than "A"
    EXPECT_GT(compare("a", "A"), 0);
    EXPECT_GT(compare(L"a", L"A"), 0);
    EXPECT_GT(collate("a", "A"), 0);
    EXPECT_GT(collate(L"a", L"A"), 0);
    EXPECT_GT(compare(transform("a"), transform("A")), 0);
    EXPECT_GT(compare(transform(L"a"), transform(L"A")), 0);
}


TEST_F(BrokenLocaleDemo, XfrmCollate_locale) {
    ::setlocale(LC_ALL, "en_US.UTF8");

    // "a" compares greater than "A" without locale
    EXPECT_GT(compare("a", "A"), 0);
    EXPECT_GT(compare(L"a", L"A"), 0);

    // With locale, "a" should be less than "A"
    EXPECT_LT(collate("a", "A"), 0);
    EXPECT_LT(collate(L"a", L"A"), 0);
    EXPECT_LT(compare(transform("a"), transform("A")), 0);
    EXPECT_LT(compare(transform(L"a"), transform(L"A")), 0);
}


TEST_F(BrokenLocaleDemo, AngstromSymbol_EN_US_UTF8) {
    ::setlocale(LC_ALL, "en_US.UTF8");
    const wchar_t* angstrom = L"\xc3\x85";

    // Get the transformed value of angstrom
    wchar_t xfrm_angstrom[MAXLEN] = {L'\0'};
    wcsxfrm(xfrm_angstrom, angstrom, sizeof xfrm_angstrom/sizeof *xfrm_angstrom);

    // There should have been no error with the transformation
    EXPECT_EQ(errno, 0);

    // The transformation should have done something
    EXPECT_STRNE(angstrom, xfrm_angstrom);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}