#include "TF_TestMain.h"
#include "Common_3/Application/Config.h"
#include "Forge/Core/TF_String.h"
#include "utest.h"

UTEST(Core, tfCaselessCompare)
{
    EXPECT_EQ(tfStrCaselessCompare(tfToRef("test"), tfToRef("TEST")), 0);
    EXPECT_EQ(tfStrCaselessCompare(tfToRef("testAA"), tfToRef("TEST")), 1);
}

UTEST(Core, tfCompare)
{
    EXPECT_EQ(tfStrCompare(tfToRef("test"), tfToRef("TEST")), 32);
    EXPECT_EQ(tfStrCompare(tfToRef("Test"), tfToRef("test")), -32);
}

UTEST(Core, tfIndexOf)
{
    EXPECT_EQ(tfStrIndexOf(tfToRef("foo foo"), tfToRef("foo")), 0);
    EXPECT_EQ(tfStrIndexOf(tfToRef("banana"), tfToRef("ana")), 1);
    EXPECT_EQ(tfStrIndexOf(tfToRef("textbook"), tfToRef("book")), 4);
    EXPECT_EQ(tfStrIndexOf(tfToRef("abababcabababcbab"), tfToRef("ababc")), 2);
    EXPECT_EQ(tfStrIndexOf(tfToRef("This is a test string to see how Boyer-Moore handles longer searches"), tfToRef("searches")), 60);
    EXPECT_EQ(tfStrIndexOf(tfToRef("hello world"), tfToRef("hello")), 0);
    EXPECT_EQ(tfStrIndexOf(tfToRef("apple banana"), tfToRef("nana")), 8);
    EXPECT_EQ(tfStrIndexOf(tfToRef("goodbye"), tfToRef("bye")), 4);
    EXPECT_EQ(tfStrIndexOf(tfToRef("cat"), tfToRef("dog")), -1);
    EXPECT_EQ(tfStrIndexOf(tfToRef(""), tfToRef("abc")), -1);
    EXPECT_EQ(tfStrIndexOf(tfToRef("hello"), tfToRef("")), -1);
    EXPECT_EQ(tfStrIndexOf(tfToRef("abc"), tfToRef("abcd")), -1);
}

UTEST(Core, tfLastIndexOf)
{
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("foo foo"), tfToRef(" ")), 3);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("foo foo"), tfToRef("foo")), 4);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("banana"), tfToRef("ana")), 3);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("textbook"), tfToRef("book")), 4);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("abababcabababcbab"), tfToRef("ababc")), 9);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("This is a test string to see how Boyer-Moore handles longer searches"), tfToRef("searches")), 60);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("hello world"), tfToRef("hello")), 0);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("apple banana"), tfToRef("nana")), 8);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("goodbye"), tfToRef("bye")), 4);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("cat"), tfToRef("dog")), -1);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef(""), tfToRef("abc")), -1);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("hello"), tfToRef("")), -1);
    EXPECT_EQ(tfStrLastIndexOf(tfToRef("abc"), tfToRef("abcd")), -1);
}

UTEST(Core, tfEq)
{
    EXPECT_EQ(tfStrEqual(tfToRef("Hello world"), tfToRef("Hello world")), 1);
    EXPECT_EQ(tfStrEqual(tfToRef("Helloworld"), tfToRef("Hello world")), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("Hello World"), tfToRef("Hello world")), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("Hello"), tfToRef("Hello ")), 0);
}

UTEST(Core, tfDuplicate)
{
    struct TStr s1 = { 0 };
    EXPECT_EQ(tfStrAssign(&s1, tfToRef("Hello World")), true);
    struct TStr s2 = tfStrDup(&s1);
    EXPECT_NE(s1.buf, s2.buf);
    tfStrFree(&s1);
    tfStrFree(&s2);
}

UTEST(Core, tfIterateRev)
{
    struct TStrSpan           buf = tfToRef("one two three four five");
    struct TFStrSplitIterable iterable = { .buffer = buf, .delim = tfToRef(" "), .cursor = buf.len };
    struct TStrSpan           s = { 0 };
    s = tfStrSplitRevIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("five"), s), 1);
    s = tfStrSplitRevIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("four"), s), 1);
    s = tfStrSplitRevIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("three"), s), 1);
    s = tfStrSplitRevIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("two"), s), 1);
    s = tfStrSplitRevIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("one"), s), 1);
    s = tfStrSplitRevIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 1);
}

UTEST(Core, tfReadull)
{
    unsigned long long res = 0;
    EXPECT_EQ(tfStrReadull(tfToRef("123456"), &res), 1);
    EXPECT_EQ(res, 123456);
}

UTEST(Core, tfReadu32)
{
    long long res = 0;
    EXPECT_EQ(tfStrReadll(tfToRef("123456"), &res), 1);
    EXPECT_EQ(res, 123456);
}

UTEST(Core, tfIterateWhiteSpace)
{
    struct TFStrSplitIterable iterable = { .buffer = tfToRef("one  two"), .delim = tfToRef(" "), .cursor = 0 };
    struct TStrSpan           s = { 0 };

    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("one"), s), 1);

    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 1);

    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEqual(tfToRef("two"), s), 1);

    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 1);
}

UTEST(Core, tfIterateCount)
{
    {
        struct TFStrSplitIterable iterable = { .buffer = tfToRef("one two three four five"), .delim = tfToRef(" "), .cursor = 0 };
        struct TStrSpan           slices[] = {
            tfToRef("one"), tfToRef("two"), tfToRef("three"), tfToRef("four"), tfToRef("five"),
        };
        size_t i = 0;
        while (iterable.cursor < iterable.buffer.len)
        {
            struct TStrSpan it = tfStrSplitIter(&iterable);
            EXPECT_EQ(tfStrEqual(slices[i++], it), 1);
        }
    }
    {
        struct TFStrSplitIterable iterable = { .buffer = tfToRef("one   two"), .delim = tfToRef(" "), .cursor = 0 };
        struct TStrSpan           slices[] = {
            tfToRef("one"),
            tfToRef(""),
            tfToRef(""),
            tfToRef("two"),
        };
        size_t i = 0;
        while (iterable.cursor < iterable.buffer.len)
        {
            struct TStrSpan it = tfStrSplitIter(&iterable);
            EXPECT_EQ(tfStrEqual(slices[i++], it), 1);
        }
    }
}

UTEST(Core, tfIterate)
{
    struct TFStrSplitIterable iterable = { .buffer = tfToRef("one two three four five"), .delim = tfToRef(" "), .cursor = 0 };

    struct TStrSpan s = { 0 };
    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("one"), s), 1);
    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("two"), s), 1);
    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("three"), s), 1);
    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("four"), s), 1);
    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 0);
    EXPECT_EQ(tfStrEqual(tfToRef("five"), s), 1);
    s = tfStrSplitIter(&iterable);
    EXPECT_EQ(tfStrEmpty(s), 1);
}

UTEST(Core, tffmtWriteLongLong)
{
    char   test_buffer[TFSTR_LLSTR_SIZE];
    size_t len = tfstrfmtll((struct TStrSpan){ .buf = test_buffer, .len = TFSTR_LLSTR_SIZE }, 12481);
    EXPECT_EQ(len, 5);
    EXPECT_EQ(tfStrEqual(tfToRef("12481"), (struct TStrSpan){ .buf = test_buffer, .len = len }), true);
}

UTEST(Core, tfCaselessEq)
{
    EXPECT_EQ(tfStrCaselessEqual(tfToRef("Hello world"), tfToRef("Hello world")), 1);
    EXPECT_EQ(tfStrCaselessEqual(tfToRef("Helloworld"), tfToRef("Hello world")), 0);
    EXPECT_EQ(tfStrCaselessEqual(tfToRef("Hello World"), tfToRef("Hello world")), 1);
    EXPECT_EQ(tfStrCaselessEqual(tfToRef("Hello"), tfToRef("Hello ")), 0);
}

UTEST(Core, tfCatJoin)
{
    struct TStr     buf = { 0 };
    struct TStrSpan slices[] = {
        tfToRef("one"),
        tfToRef("two"),
        tfToRef("three"),
        tfToRef("four"),
    };
    EXPECT_EQ(tfstrcatjoin(&buf, slices, 4, tfToRef(" ")), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("one two three four")), true);
    tfStrFree(&buf);
}

UTEST(Core, tfCatJoinCstr)
{
    struct TStr buf = { 0 };
    const char* slices[] = {
        "one",
        "two",
        "three",
        "four",
    };
    EXPECT_EQ(tfstrcatjoinCStr(&buf, slices, 4, tfToRef(" ")), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("one two three four")), true);
    tfStrFree(&buf);
}

UTEST(Core, appendSlice)
{
    struct TStr buf = { 0 };
    tfStrAssign(&buf, tfToRef("Hello"));
    EXPECT_EQ(tfStrAppendSlice(&buf, tfToRef(" world")), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("Hello world")), true);
    tfStrFree(&buf);
}

UTEST(Core, tfcatprintf)
{
    struct TStr buf = { 0 };
    EXPECT_EQ(tfstrcatprintf(&buf, "Hello %s", "world"), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("Hello world")), true);

    tfStrSetLen(&buf, 0);
    EXPECT_EQ(tfstrcatprintf(&buf, "%d", 123), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("123")), true);

    EXPECT_EQ(tfstrcatprintf(&buf, " %lu", 156), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("123 156")), true);

    tfStrSetLen(&buf, 0);
    EXPECT_EQ(tfstrcatprintf(&buf, "a%cb", 0), true);
    EXPECT_EQ(tfStrEqual(tfToRef(buf), (struct TStrSpan){ .buf = "a\0"
                                                                "b",
                                                         .len = 3 }),
              1);
    tfStrFree(&buf);
}

UTEST(Core, tfcatfmt)
{
    struct TStr s = { 0 };
    {
        tfstrcatfmt(&s, "%S\n", tfToRef("Hello World"));
        EXPECT_EQ(tfStrEqual(tfToRef(s), tfToRef("Hello World\n")), true);
        tfStrClear(&s);
    }
    {
        tfstrcatfmt(&s, "%i\n", 125);
        EXPECT_EQ(tfStrEqual(tfToRef(s), tfToRef("125\n")), true);
        tfStrClear(&s);
    }
    {
        tfstrcatfmt(&s, "%i\n", -125);
        EXPECT_EQ(tfStrEqual(tfToRef(s), tfToRef("-125\n")), true);
        tfStrClear(&s);
    }
    tfStrFree(&s);
}

UTEST(Core, updateLen)
{
    struct TStr buf = { 0 };
    tfStrAssign(&buf, tfToRef("Hello World"));
    buf.buf[5] = '\0';
    tfStrUpdateLen(&buf);

    EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("Hello")), true);
    EXPECT_EQ(buf.len, 5);

    tfStrFree(&buf);
}

UTEST(Core, tfStrAssign)
{
    struct TStr buf = { 0 };
    {
        tfStrAssign(&buf, tfToRef("Hello World"));
        EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("Hello World")), true);
    }
    {
        tfStrAssign(&buf, tfStrTrim(tfToRef("   Hello World   ")));
        EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("Hello World")), true);
    }
    tfStrFree(&buf);
}

UTEST(Core, tf_rtrim)
{
    EXPECT_EQ(tfStrEqual(tfStrRTrim(tfToRef("Hello world  ")), tfToRef("Hello world")), true);
    EXPECT_EQ(tfStrEqual(tfStrRTrim(tfToRef("  Hello world  ")), tfToRef("  Hello world")), true);
    EXPECT_EQ(tfStrEqual(tfStrRTrim(tfToRef("  ")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrRTrim(tfToRef(" ")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrRTrim(tfToRef("\n")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrRTrim(tfToRef("\t")), tfToRef("")), true);
}

UTEST(Core, tf_ltrim)
{
    EXPECT_EQ(tfStrEqual(tfStrLTrim(tfToRef("Hello world  ")), tfToRef("Hello world  ")), true);
    EXPECT_EQ(tfStrEqual(tfStrLTrim(tfToRef("  ")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrLTrim(tfToRef(" ")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrLTrim(tfToRef("\n")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrLTrim(tfToRef("\t")), tfToRef("")), true);
}

UTEST(Core, tfStrTrim)
{
    {
        struct TStr buf = { 0 };
        tfStrAssign(&buf, tfToRef("  Hello World "));
        tfStrAssign(&buf, tfStrTrim(tfToRef(buf)));
        EXPECT_EQ(tfStrEqual(tfToRef(buf), tfToRef("Hello World")), true);
        tfStrFree(&buf);
    }

    EXPECT_EQ(tfStrEqual(tfStrTrim(tfToRef("Hello world  ")), tfToRef("Hello world")), true);
    EXPECT_EQ(tfStrEqual(tfStrTrim(tfToRef("  \t Hello world  ")), tfToRef("Hello world")), true);
    EXPECT_EQ(tfStrEqual(tfStrTrim(tfToRef("  ")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrTrim(tfToRef(" ")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrTrim(tfToRef("\n")), tfToRef("")), true);
    EXPECT_EQ(tfStrEqual(tfStrTrim(tfToRef("\t")), tfToRef("")), true);
}

UTEST(Core, tfsscanf)
{
    {
        uint32_t a = 0;
        uint32_t b = 0;
        int      read = tfstrsscanf(tfToRef("10.132"), "%u.%u", &a, &b);
        EXPECT_EQ(read, 2);
        EXPECT_EQ(a, 10);
        EXPECT_EQ(b, 132);
    }
}

// UTEST(Core, tfSliceToUtf16CodePoint) {
//   unsigned char leftPointingMagnify[] = {0x3D,0xD8,0x0D,0xDD};
//
//   struct tf_utf_result_s res;
//   {
//     struct TFStrSplitIterable iter = {
//       .buffer = (struct TStrSpan) {
//         .buf = (const char*)leftPointingMagnify,
//         .len = 2,
//       },
//       .cursor = 0
//     };
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ((bool)res.invalid, true);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//   {
//     struct TFStrSplitIterable iter = {
//       .buffer = (struct TStrSpan) {
//         .buf = (const char*)leftPointingMagnify,
//         .len = sizeof(leftPointingMagnify),
//       },
//       .cursor = 0
//     };
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ(res.codePoint, 0x1F50D);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//   {
//     struct TFStrSplitIterable iter = (struct TFStrSplitIterable){
//       .buffer = (struct TStrSpan) {
//         .buf = NULL,
//         .len = 0,
//       },
//       .cursor = 0
//     };
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ((bool)res.invalid, true);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//   {
//     char badInput[] = {0};
//     struct TFStrSplitIterable iter = (struct TFStrSplitIterable){
//         .buffer = (struct TStrSpan){
//           .buf = badInput,
//           .len = 0,
//         },
//         .cursor = 0};
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ((bool)res.invalid, true);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//
// }

// UTEST(Core, tfSliceToUtf8CodePoint) {
//   char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//   struct TFStrSplitIterable iter = {
//     .buffer = (struct TStrSpan) {
//       .len = sizeof(smilyCat),
//       .buf = smilyCat
//     },
//     .cursor = 0
//   };
//
//   struct tf_utf_result_s res = tfUtf8NextCodePoint(&iter);
//   EXPECT_EQ(res.codePoint, 0x0001f63c);
//
//   char charU[] = {'U'};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct TStrSpan) {
//     .len = sizeof(charU),
//     .buf = charU
//   }, 0), 'U');
//
//   char ringOperator[] = {0xe2, 0x88, 0x98};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct TStrSpan) {
//     .len = sizeof(ringOperator),
//     .buf = ringOperator
//   }, 0), 0x2218);
//
//   // this has an extra byte
//   char badRingOperator[] = {0xe2, 0x88, 0x98, 0x1};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct TStrSpan) {
//     .len = sizeof(badRingOperator),
//     .buf = badRingOperator
//   }, 1), 1);
// }

// UTEST(Core, tfUtf8CodePointIter) {
//   {
//     unsigned char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//     struct TFStrSplitIterable iter = {
//       .buffer = (struct TStrSpan) {
//         .buf = (char*)smilyCat,
//         .len = sizeof(smilyCat),
//       },
//       .cursor = 0
//     };
//     struct tf_utf_result_s res = tfUtf8NextCodePoint(&iter);
//     EXPECT_EQ(res.codePoint, 0x0001f63c);
//     EXPECT_EQ((bool)res.finished, true);
//     EXPECT_EQ((bool)res.invalid, false);
//   }
// {
//   // Ḽơᶉëᶆ
//   unsigned char buffer[] = {0xE1, 0xB8, 0xBC, 0xC6, 0xA1, 0xE1, 0xB6, 0x89,0xC3, 0xAB,0xE1, 0xB6,0x86 };
//   struct TFStrSplitIterable iterable = {
//     .buffer = {
//       .buf = (const char*)buffer,
//       .len = sizeof(buffer)
//     },
//     .cursor = 0,
//   };
//   struct tf_utf_result_s s = {0};
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint, 0x00001E3C); // 0xE1, 0xB8, 0xBC
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x1a1); // 0xBC, 0xC6
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x1D89); // 0xE1, 0xB6, 0x89
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x00EB); // 0xC3 0xAB
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x1D86); //0xE1 0xB6 0x86
//   EXPECT_EQ((bool)s.finished, true);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ((bool)s.finished, true);
//   EXPECT_EQ((bool)s.invalid, true);
//   }
// }

#include "Common_3/Utilities/Interfaces/IFileSystem.h"
#include "Common_3/Utilities/Interfaces/IMemory.h"
#include "Common_3/Utilities/Interfaces/ILog.h"
UTEST_STATE();
TF_UTEST_MAIN("TF_StringTest")
