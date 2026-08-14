#include <string.h>

void* memset(void* dest, int c, size_t n) {
    unsigned char* p = dest;
    while (n--) *p++ = (unsigned char)c;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        p1++; p2++;
    }
    return 0;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (n-- && (*d++ = *src++));
    while (n--) *d++ = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest + strlen(dest);
    while ((*d++ = *src++));
    return dest;
}

int atoi(const char* s) {
    int result = 0;
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result * sign;
}

void itoa(int value, char* str, int base) {
    char* p = str;
    char* p1;
    char temp;
    unsigned int uvalue;

    if (value < 0 && base == 10) {
        *p++ = '-';
        uvalue = -value;
    } else {
        uvalue = value;
    }

    do {
        int digit = uvalue % base;
        *p++ = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        uvalue /= base;
    } while (uvalue);

    *p = '\0';
    p1 = (*str == '-') ? str + 1 : str;
    p--;
    while (p1 < p) {
        temp = *p;
        *p-- = *p1;
        *p1++ = temp;
    }
}