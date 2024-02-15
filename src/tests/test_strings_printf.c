#include "assert.h"
#include "printf.h"
#include "printf_internal.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

static void test_memset(void)
{
    int numA = 0xefefefef;
    int numB = 2;

    memset(&numB, 0xef, sizeof(int));
    assert(numA == numB);

    int numC = 0xfbfbfbfb;
    int numD = 3;
    memset(&numD, -5, sizeof(int)); // -5 should convert to 0xfb
    assert(numC == numD);

    int numE = 0xf6f6f6f6;
    memset(&numB, -10, sizeof(int));
    assert(numE == numB);

    int numF = 0x2020000a;
    int numG = 0x20200000;
    memset(&numG, 10, sizeof(char));
    assert(numF == numG);

    memset(&numF, 5, 0);
    assert(numF == 0x2020000a); // should remain unaffected
}

static void test_memcpy(void)
{
    int numA = 0x12345678;
    int numB = 2;

    memcpy(&numB, &numA, sizeof(int));
    assert(numA == numB);

    int numC = 0x00000EEE;
    char numD = 2;
    memcpy(&numC, &numD, sizeof(char));
    assert(numC == 0x00000E02); // replaces least significant byte

    int numE = 0x0d0c0b0a;
    int numF = 5;
    memcpy(&numE, &numF, sizeof(char));
    assert(numE == 0x0d0c0b05);

    int numG = 0x2020000a;
    int numH = 5;
    memcpy(&numG, &numH, 0);
    assert(numG == 0x2020000a); // should remain unaffected
}

static void test_strlen(void)
{
    assert(strlen("green") == 5);
    
    assert(strlen("") == 0);
    assert(strlen(NULL) == 0);
    assert(strlen("a") == 1);
}

static void test_strcmp(void)
{
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);

    assert(strcmp("", "") == 0);
    assert(strcmp("", "a") < 0);
    assert(strcmp("a", "") > 0);
    assert(strcmp("applesauce", "apple") > 0);
    assert(strcmp("apples", "pears") < 0);
    assert(strcmp("applesauce", "apple") == 115);
    assert(strcmp("apple", "applesauce") == -115);
    assert(strcmp("Hi", NULL) > 0);
    assert(strcmp(NULL, "Hi") < 0);
    assert(strcmp("", NULL) == 0);
    assert(strcmp(NULL, NULL) == 0);
}

static void test_strlcat(void)
{
    char buf[20];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    buf[0] = '\0'; // start with empty string
    assert(strlen(buf) == 0);
    strlcat(buf, "CS", sizeof(buf));
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0);
    strlcat(buf, "107e", sizeof(buf));
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0);

    char bud[10];
    memset(bud, 0x77, sizeof(bud));

    bud[0] = '\0';
    assert(strlen(bud) == 0);
    strlcat(bud, "Hello", 4); //test that it appends right # of bytes
    assert(strlen(bud) == 3);
    assert(strcmp(bud, "Hel") == 0);
    strlcat(bud, "sinki", sizeof(bud));
    assert(strlen(bud) == 8);
    assert(strcmp(bud, "Helsinki") == 0);
    strlcat(bud, "Finland", sizeof(bud));
    assert(strlen(bud) == 9);
    assert(strcmp(bud, "HelsinkiF") == 0);

}

static void test_strtonum(void)
{
    int val = strtonum("013", NULL);
    assert(val == 13);

    const char *input = "107rocks", *rest = NULL;
    val = strtonum(input, &rest);
    assert(val == 107);
    assert(rest == &input[3]);
    
    // test leading zeros in decimal
    int val1 = strtonum("00500", NULL);
    assert(val1 = 500);
    
    // test invalid first char in decimal
    const char *input2 = "a500", *rest2 = NULL;
    int val2 = strtonum(input2, &rest2);
    assert(val2 == 0);
    assert(rest2 == &input2[0]);
    
    // test hex numbers
    int val3 = strtonum("0xff", NULL);
    assert (val3 == 255);

    // test hex numbers with invalid char later
    const char *input4 = "0xddg", *rest4 = NULL;
    int val4 = strtonum(input4, &rest4);
    assert(val4 == 221);
    assert(rest4 == &input4[4]);

    // test hex numbers with invalid char immediately 
    const char *input5 = "0xgdd", *rest5 = NULL;
    int val5 = strtonum(input5, &rest5);
    assert(val5 == 0);
    assert(rest5 == &input5[2]);

    // test bigger numbers
    const char *input6 = "24749", *rest6 = NULL;
    int val6 = strtonum(input6, &rest6);
    assert(val6 == 24749);
    assert(rest6 == &input6[5]);

    // test hex with leading zeros
    int val7 = strtonum("0x0000ab", NULL);
    assert(val7 == 171);

    // test decimal with x as second char
    int val8 = strtonum("2x20", NULL);
    assert(val8 == 2);
}

static void test_to_base(void)
{
    char buf[5];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    int n = signed_to_base(buf, bufsize, -9999, 10, 6);
    assert(strcmp(buf, "-099") == 0);
    assert(n == 6);

    int i = unsigned_to_base(buf, bufsize, 1, 10, 0);
    assert(strcmp(buf, "1") == 0);
    assert(i == 1);

    int j = unsigned_to_base(buf, bufsize, 9999, 10, 0);
    assert(strcmp(buf, "9999") == 0);
    assert(j == 4);

    int k = signed_to_base(buf, bufsize, -5, 10, 3);
    assert(strcmp(buf, "-05") == 0);
    assert(k == 3);

    int l = unsigned_to_base(buf, bufsize, 12, 16, 5);
    assert(strcmp(buf, "0000") == 0);
    assert(l == 5);

    int m = unsigned_to_base(buf, bufsize, 62886, 16, 2);
    assert(strcmp(buf, "f5a6") == 0);
    assert(m == 4);

    int o = unsigned_to_base(buf, 1, 9933, 10, 0);
    assert(strcmp(buf, "") == 0); 
    assert(o == 4);
    
    int p = signed_to_base(buf, bufsize, -16, 16, 8);
    assert(strcmp(buf, "-000") == 0);  
    assert(p == 8);

    int q = unsigned_to_base(buf, 0, 23, 16, 5);
    assert(strcmp(buf, "-000") == 0); // bufsize 0 means write nothing so no change from previous
    assert(q == 5);

    int r = signed_to_base(buf, bufsize, -31, 16, 1);
    assert(strcmp(buf, "-1f") == 0);
    assert(r == 3);

    int s = signed_to_base(buf, bufsize, -3348337, 16, 5);
    assert(strcmp(buf, "-331") == 0);
    assert(s == 7);
}

static void test_snprintf(void)
{
    char buf[100];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    // Start off simple...
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // % sign
    snprintf(buf, bufsize, "%%");
    assert(strcmp(buf, "%") == 0);
    
    // Decimal
    snprintf(buf, bufsize, "%d", 45);
    assert(strcmp(buf, "45") == 0);

    snprintf(buf, bufsize, "%010d", 2);
    assert(strcmp(buf, "0000000002") == 0);

    snprintf(buf, bufsize, "%05d", -3);
    assert(strcmp(buf, "-0003") == 0);
    
    // Hexadecimal
    snprintf(buf, bufsize, "%x", 0xef);
    assert(strcmp(buf, "ef") == 0);
     
    snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);
    
    // Pointer
    snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);

    // Character
    snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);
    
    // String
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    snprintf(buf, bufsize, "Hi %s", "Peter");
    assert(strcmp(buf, "Hi Peter") == 0);

    snprintf(buf, bufsize, "Hi %s%s", "Peter", "McEvoy");
    assert(strcmp(buf, "Hi PeterMcEvoy") == 0);
    
    // Format string with intermixed codes
    snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);

    snprintf(buf, bufsize, "Every code, right here: 100%% effort. %d cats. %x dogs. %p. %c. %s.", 2, 12, (void *) 0x2020001c, 'E', "End of Message");
    assert(strcmp(buf, "Every code, right here: 100% effort. 2 cats. c dogs. 0x2020001c. E. End of Message.") == 0);
    
    // Test return value
    assert(snprintf(buf, bufsize, "100%%%d%x%p%c%s.", -2, 12, (void *) 0x20202020, 'F', "I'm tired") == 28);
    assert(strcmp(buf, "100%-2c0x20202020FI'm tired.") == 0);

    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);
    assert(strcmp(buf, "H") == 0);

    snprintf(buf, 0, "This shouldn't appear %d", 5);
    assert(strcmp(buf, "H") == 0);

    snprintf(buf, 1, "Neither should this. %x", 0x0e);
    assert(strcmp(buf, "") == 0);

    
}

static void test_printf(void) { 
    printf("Every code, right here: 100%% effort. %d cats. %x dogs. %p. %c. %s.\n", 2, 12, (void *) 0x2020001c, 'E', "End of Message");
}


void main(void)
{
    uart_init();
    uart_putstring("Start execute main() in tests/test_strings_printf.c\n");
    
    test_memset();
    test_memcpy();
    test_strlen();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_to_base();
    test_snprintf();
    test_printf();

    uart_putstring("Successfully finished executing main() in tests/test_strings_printf.c\n");
    uart_putchar(EOT);
}
