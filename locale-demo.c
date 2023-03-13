#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>


// Demonstrates the behavior of locales on string comparisons
void compare_demo() {
    const char* a = "a";
    const char* A = "A";
    const wchar_t* wa = L"a";
    const wchar_t* wA = L"A";

    // Show comparison results for standard char
    const size_t a_size = strxfrm(NULL, a, 0);
    const size_t A_size = strxfrm(NULL, A, 0);
    char xa[1 + a_size];
    char xA[1 + A_size];
    strxfrm(xa, a, sizeof xa/sizeof *xa);
    strxfrm(xA, A, sizeof xA/sizeof *xA);
    printf("strcmp(\"a\", \"A\"): %d\n", strcmp(a, A));
    printf("strcoll(\"a\", \"A\"): %d\n", strcoll(a, A));
    printf("strcmp(strxfrm(\"a\"), strxfrm(\"A\")): %d\n", strcmp(xa, xA));

    // Show comparison results for wide char
    const size_t wa_size = wcsxfrm(NULL, wa, 0);
    const size_t wA_size = wcsxfrm(NULL, wA, 0);
    wchar_t xwa[1 + wa_size];
    wchar_t xwA[1 + wA_size];
    wcsxfrm(xwa, wa, sizeof xwa/sizeof *xwa);
    wcsxfrm(xwA, wA, sizeof xwA/sizeof *xwA);
    printf("wcscmp(\"a\", \"A\"): %d\n", wcscmp(wa, wA));
    printf("wcscoll(\"a\", \"A\"): %d\n", wcscoll(wa, wA));
    printf("wcscmp(wcsxfrm(\"a\"), wcsxfrm(\"A\")): %d\n", wcscmp(xwa, xwA));
}


int main() {
    setlocale(LC_ALL, "C");
    printf("locale: C\n");
    compare_demo();
    printf("\n");

    setlocale(LC_ALL, "en_US.UTF8");
    printf("locale: en_US.UTF8\n");
    compare_demo();
    printf("\n");

    // Demonstrate that transform fails on angstrom symbol
    const wchar_t* angstrom = L"\xc3\x85";
    errno = 0;
    wchar_t xangstrom[1 + wcsxfrm(NULL, angstrom, 0)];
    if (errno != 0) {
        perror("wcsxfrm allocation failed on angstrom symbol");
    }
    errno = 0;
    wcsxfrm(xangstrom, angstrom, sizeof xangstrom/sizeof *xangstrom);
    if (errno != 0) {
        perror("wcsxfrm transformation failed on angstrom symbol");
    }
    printf("angstrom: %ls -> %ls\n", angstrom, xangstrom);
    printf("\n");

    return 0;
}
