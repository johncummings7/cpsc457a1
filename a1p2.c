#include <stdio.h>
#include <stdlib.h>

/*
Helper to parse inputs to longs.
Takes the string to parse, s, and the label for what that string is meant to be (UPPER, LOWER, or N).
*/
static long string_to_long(const char *s, const char *label) {
    char *end = NULL; // Pointer used to find endpoint of the string (\0)
    long num = strtol(s, &end, 10); // Converting string to base 10 long

    // Checking that digits were prsed and that the string ends with \0 identifier
    if (end == s || *end != '\0') {
        fprintf(stderr, "Invalid %s: %s\n", label, s);
        exit(1);
    }
    return num;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Bad input, usage is: %s <your numbers...>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        long num = string_to_long(argv[i], "arg");
        printf("Good input: '%s' -> %ld\n", argv[i], num);
    }
    return 0;
}