/* Bstr 2.0 -- A C dynamic strings library
 *
 * Copyright (c) 2006-2015, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2015, Oran Agra
 * Copyright (c) 2015, Redis Labs, Inc
 * Paul Hsieh in 2002-2015
 * Michael Pollind 2024-* <mpollind at gmail dot com>
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// this is a modified string library that implements features from both bstring and sds


#ifndef TF_STRING_H_INCLUDED
#define TF_STRING_H_INCLUDED

#include "TF_Types.h"
#include "Common_3/Utilities/Interfaces/ILog.h"

#define TFSTR_LLSTR_SIZE 21
#define TFSTR_LSTR_SIZE 16 

struct TStr {
  size_t alloc;
  size_t len;
  char* buf;
};

struct TStrSpan {
  char * buf;
  size_t len;
}; 

//static inline struct TStrSpan tfToRef(char* c) { return (struct TStrSpan){ c, strlen(c) }; }
static inline struct TStrSpan tfToRef(const char* c) { return (struct TStrSpan){ (char*)c, strlen(c) }; }
static inline struct TStrSpan tfToRef(struct TStr str) { return (struct TStrSpan){ str.buf, str.len }; }

static inline struct TStrSpan tfSub(struct TStrSpan slice, size_t a, size_t b) {
  ASSERT((b - a) <= slice.len);
  return (struct TStrSpan){slice.buf + a, b - a};
}

static inline size_t tfStrAvailLen(TStr str) { return str.alloc - str.len;}
static inline TStrSpan tfStrAvailSpan(TStr str) { return (struct TStrSpan){str.buf + str.len, tfStrAvailLen(str)};}

/**
 * Creates a string from a slice 
 **/
void tfStrFree(struct TStr* str);
void tfStrUpper(struct TStrSpan slice);
void tfStrLower(struct TStrSpan slice);
#define tfStrEmpty(b) ((b).buf == NULL || (b).len == 0)

struct TStrSpan tfStrTrim(struct TStrSpan slice);
struct TStrSpan tfStrRTrim(struct TStrSpan  slice);
struct TStrSpan tfStrLTrim(struct TStrSpan  slice);

/* Enlarge the free space at the end of the bstr string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the bstr string as len 
 * but only the free buffer space we have. */
bool tfStrMakeRoomFor(struct TStr* str, size_t addlen);
/* 
 * set the length of the buffer to the length specified. this
 * will also trigger a realloction if the length is greater then the size
 * reserved. 
 *
 * Note: this does not set the null terminator for the string.
 * this will corrupt slices that are referencing a slice out of this buffer.
 **/
bool tfStrSetLen(struct TStr* str, size_t len);
/**
 * set the amount of memory reserved by the bstr. will only ever increase
 * the size of the string 
 * 
 * A reserved string can be assigned with bstrAssign
 **/
bool tfStrSetResv(struct TStr* str, size_t reserveLen); 

/** 
 * Modify an bstr string in-place to make it empty (zero length) set null terminator.
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. 
 **/
bool tfStrClear(struct TStr* str);

/**
 * takes a bstr and duplicates the underlying buffer.
 *
 * the buffer is trimmed down to the length of the string.
 *
 * if the buffer fails to allocate then BSTR_IS_EMPTY(b) will be true
 **/
struct TStr tfStrDup(const struct TStr* str);
bool tfStrAppendSlice(struct TStr* str, const struct TStrSpan slice);
bool tfStrAppendChar(struct TStr* str, char b);
bool tfStrInsertChar(struct TStr* str, size_t i, char b);
bool tfStrInserSlice(struct TStr* str, size_t i, const struct TStrSpan slice);
bool tfStrAssign(struct TStr* str, struct TStrSpan slice);
/**
 * resizes the allocation of the bstr will truncate if the allocation is less then the size 
 *
 * the buffer is trimmed down to the length of the string.
 *
 * If the buffer fails to reallocate then false is returned
 **/
bool tfStrResize(struct TStr* str, size_t len);


struct TFStrSplitIterable {
  const struct TStrSpan buffer; // the buffer to iterrate over
  const struct TStrSpan delim; // delim to split across 
  size_t cursor; // the current position in the buffer
};
//#define bstr_iter_has_more(it) (it.cursor < it.buffer.len)

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {
 *     .delim = bstr_ref(" "),
 *     .buffer = bstr_ref("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  bstr_iter_has_more(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct TStrSpan tfStrSplitIter(struct TFStrSplitIterable*);

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * For the reverse case move the cursor to the end of the string
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {
 *     .delim = bstr_ref(" "),
 *     .buffer = bstr_ref("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  !bstr_is_empty(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct TStrSpan tfStrSplitRevIter(struct TFStrSplitIterable*);

/* Set the bstr string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the bstr string has been changed where
 * the length is not correctly updated. using vsprintf for instance.
 *
 * After the call, slices are not valid if they reference this bstr 
 * 
 * s = bstrEmpty();
 * s[2] = '\0';
 * bstrUpdateLen(s);
 * printf("%d\n", s.len);
 *
 * The output will be "2", but if we comment out the call to bstrUpdateLen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. 
 ** */
bool tfStrUpdateLen(struct TStr* str);

/* Append to the bstr string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified bstr string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = bstrCreate("Sum is: ");
 * bstrcatprintf(s,"%d+%d = %d",a,b,a+b)
 *
 * if valid BSTR_OK else BSTR_ERR
 */
bool tfstrcatprintf(struct TStr* s, const char *fmt, ...); 
bool tfstrcatvprintf(struct TStr* str, const char* fmt, va_list ap);

int tfstrsscanf(struct TStrSpan slice, const char* fmt, ...);
int tfstrvsscanf(struct TStrSpan slice, const char* fmt, va_list ap);

/* This function is similar to bstrcatprintf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the bstr as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %c - char
 * %s - C String
 * %S - struct bstr_const_slice_s slice 
 * %i - signed int
 * %l - signed long
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %L - unsigned long
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
bool tfstrcatfmt(struct TStr*, char const *fmt, ...);


/*
 * join an array of slices and cat them to bstr. faster since the lengths are known ahead of time.
 * the buffer can be pre-reserved upfront.
 *
 * this modifies bstr so slices that reference this bstr can become invalid.
 **/
bool tfstrcatjoin(struct TStr*, struct TStrSpan* slices, size_t numSlices, struct TStrSpan sep);
/*
 * join an array of strings and cat them to bstr 
 **/
bool tfstrcatjoinCStr(struct TStr*, const char** argv, size_t argc, struct TStrSpan sep);

/**
 * this should fit safetly within BSTR_LLSTR_SIZE. 
 *
 * the number of bytes written to the slice is returned else -1 if the 
 * value is unable to be written or the length of the slice is greater
 *
 **/
int tfstrfmtll(struct TStrSpan slice, long long value); 
int tfstrfmtull(struct TStrSpan slice, unsigned long long value); 

/*  
 * Parse a string into a 64-bit integer.
 *
 * when base is 0, the function will try to determine the base from the string
 *   * if the string starts with 0x, the base will be 16
 *   * if the string starts with 0b, the base will be 2
 *   * if the string starts with 0o, the base will be 8
 *   * otherwise the base will be 10
 */
bool tfStrReadll(struct TStrSpan, long long* result);
bool tfStrReadull(struct TStrSpan, unsigned long long* result);

/* Scan/search functions */
/*  
 *  Compare two strings without differentiating between case. The return
 *  value is the difference of the values of the characters where the two
 *  strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, if the first slice is longer 1 else -1. 
 */
int tfStrCaselessCompare (const struct TStrSpan b0, const struct TStrSpan b1);
/*
 *  The return value is the difference of the values of the characters where the
 *  two strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, if the first slice is longer 1 else -1.
 */
int tfStrCompare  (const struct TStrSpan b0, const struct TStrSpan b1);
/**
*  Test if two strings are equal ignores case true else false.  
**/
bool tfStrCaselessEqual (const struct TStrSpan b0, const struct TStrSpan b1);
/**
*  Test if two strings are equal return true else false.  
**/
bool tfStrEqual (const struct TStrSpan b0, const struct TStrSpan b1);

int tfStrIndexOfOffset(const struct TStrSpan haystack, size_t offset, const struct TStrSpan needle);
int tfStrIndexOf(const struct TStrSpan haystack, const struct TStrSpan needle);
int tfStrLastIndexOfOffset(const struct TStrSpan str, size_t offset, const struct TStrSpan needle);
int tfStrLastIndexOf(const struct TStrSpan str, const struct TStrSpan needle);

int tfStrIndexOfCaselessOffset(const struct TStrSpan haystack, size_t offset, const struct TStrSpan needle);
int tfStrIndexOfCaseless(const struct TStrSpan haystack, const struct TStrSpan needle);
int tfStrLastIndexOfCaseless(const struct TStrSpan haystack, const struct TStrSpan needle);
int tfStrLastIndexOfCaselessOffset(const struct TStrSpan haystack, size_t offset, const struct TStrSpan needle);

int tfStrIndexOfAny(const struct TStrSpan haystack, const struct TStrSpan characters);
int tfStrLastIndexOfAny(const struct TStrSpan haystack, const struct TStrSpan characters);

#endif
