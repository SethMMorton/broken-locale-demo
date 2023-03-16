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
#include <utility>

#include <gtest/gtest.h>


constexpr std::size_t MAXLEN = 1000;

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
    virtual ~BrokenLocaleDemo() {
        // Reset locale to "C"
        ::setlocale(LC_ALL, "C");
    }
};


class BrokenLocaleDemoParam : public BrokenLocaleDemo, public testing::WithParamInterface<std::pair<const char*, const wchar_t*>> {
protected:
    BrokenLocaleDemoParam() : BrokenLocaleDemo() { }
    virtual ~BrokenLocaleDemoParam() = default;
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
    ::setlocale(LC_ALL, "en_US.UTF-8");

    // "a" compares greater than "A" without locale
    EXPECT_GT(compare("a", "A"), 0);
    EXPECT_GT(compare(L"a", L"A"), 0);

    // With locale, "a" should be less than "A"
    EXPECT_LT(collate("a", "A"), 0);
    EXPECT_LT(collate(L"a", L"A"), 0);
    EXPECT_LT(compare(transform("a"), transform("A")), 0);
    EXPECT_LT(compare(transform(L"a"), transform(L"A")), 0);
}


TEST_P(BrokenLocaleDemoParam, Errno) {
    ::setlocale(LC_ALL, GetParam().first);  // This is an invalid locale string
    const wchar_t* x = GetParam().second;
    // const wchar_t* x = L"\xc3\x85";

    // Get the transformed value of "a"
    wchar_t xfrm_x[MAXLEN] = {L'\0'};
    ::wcsxfrm(xfrm_x, x, sizeof xfrm_x/sizeof *xfrm_x);

    // There should have been no error with the transformation
    EXPECT_EQ(errno, 0);
    if (errno != 0) {
        std::cerr << "Error number means: " << ::strerror(errno) << std::endl;;
    }

    // The transformation should have done something
    EXPECT_STRNE(x, xfrm_x);
}


INSTANTIATE_TEST_SUITE_P(
    LocaleCombos,
    BrokenLocaleDemoParam,
    testing::Values(
        std::make_pair("en_US.UTF8", L"a"),  // invalid locale string
        std::make_pair("en_US.UTF8", L"\xc3\x85"),
        std::make_pair("en_US.UTF-8", L"a"),  // valid locale string
        std::make_pair("en_US.UTF-8", L"\xc3\x85")
    )
);


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}