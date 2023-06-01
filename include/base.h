#ifndef BASE_H
#define BASE_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#if defined(_WIN32)
  #define OS_WIN 1
#elif defined(__gnu_linux__)
  #define OS_LINUX 1
#elif defined(__APPLE__)
  #define OS_MAC 1
#endif

#if !defined(OS_WIN)
  #define OS_WIN 0
#endif
#if !defined(OS_LINUX)
  #define OS_LINUX 0
#endif
#if !defined(OS_MAC)
  #define OS_MAC 0
#endif

#if !defined(TRUE)
  #define TRUE 1
#endif
#if !defined(FALSE)
  #define FALSE 0
#endif

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)
#define PI (3.1415926535897f)
#define MIN(A,B) ( ((A)<(B))?(A):(B) )
#define MAX(A,B) ( ((A)>(B))?(A):(B) )
#define CLAMP_TOP(A,X) MIN(A,X)
#define CLAMP_BOT(X,B) MAX(X,B)
#define CLAMP(A,X,B) ( ((X)<(A))?(A):((X)>(B))?(B):(X) )

#define MEMBER(T,m) (((T*)0)->m)
#define OFFSET_OF(T,m) INT_FROM_PTR(&MEMBER(T,m))

#define MEMCPY(d,s,z) memmove(d,s,z)
#define MEMCPY_STRUCT(d,s) MEMCPY(d,s,sizeof(*(d)))

#define MEMSET(s,v,z) memset(s,v,z)
#define MEMZERO(s,z)  memset(s,0,z)
#define MEMZERO_STRUCT(s) MEMZERO(s,sizeof(*(s)))
#define MEMZERO_ARRAY(a)  memset(a,0,sizeof(a))

#define MEMMATCH(a,b,z) (memcmp((a),(b),(z)) == 0)
#define MEMMATCH_STRUCT(a,b) MEMMATCH(a,b,sizeof(*(a)))

#define MALLOC(n) malloc(n)
#define FREE(n) free(n)

//TYPEDEFS
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8       b8;
typedef i16      b16;
typedef i32      b32;
typedef i64      b64;
typedef float    f32;
typedef double   f64;

#define INT_FROM_PTR(ptr) ((u64)(ptr))
#define PTR_FROM_INT(i) (void*)((u8*)0 + (i))

static inline b32 is_pow2(uintptr_t x){
  return ((INT_FROM_PTR(x) & (INT_FROM_PTR(x)-1)) == 0);
}


//ASSERTS
#define STMNT(S) do{ S }while(0)
#define ASSERT_BREAK() (*(int*)0 = 0xABCD)
#define ASSERT(c) STMNT( if (!(c)){ ASSERT_BREAK(); } )

#define DA_REALLOC(c,p,s) realloc(p,s)
#define DA_FREE(c,p)      free(p)
typedef struct {
    size_t len;
    size_t cap;
}da_header;

#define da_hdr(t)        ((da_header *) (t) - 1)
#define da_cap(a)        ((a) ? da_hdr(a)->cap : 0)
#define da_len(a)        ((a) ? da_hdr(a)->len : 0)
#define da_push(a,v)     (da_maybegrow(a,1), (a)[da_hdr(a)->len++] = (v))
#define da_pop(a)        (da_hdr(a)->len--, (a)[da_hdr(a)->len])
#define da_maybegrow(a,n)((!(a) || da_hdr(a)->len + (n) > da_hdr(a)->cap) ? (da_grow(a,n,0),0) : 0)
#define da_grow(a,b,c)   ((a) = da_growf((a), sizeof *(a), (b), (c)))
#define da_last(a)       ((a)[da_hdr(a)->len-1])
#define da_del(a,i)      da_deln(a,i,1)
#define da_deln(a,i,n)   (memmove(&(a)[i], &(a)[(i)+(n)], sizeof *(a) * (da_hdr(a)->len-(n)-(i))), da_hdr(a)->len -= (n))
#define da_delswap(a,i)  ((a)[i] = da_last(a), da_hdr(a)->len -= 1)
#define da_free(a)       ((void) ((a) ? DA_FREE(NULL,da_hdr(a)) : (void)0), (a)=NULL)
static inline void *da_growf(void *a, size_t elemsize, size_t addlen, size_t min_cap){
  da_header temp={0}; // force debugging
  void *b;
  size_t min_len = da_len(a) + addlen;
  (void) sizeof(temp);
  if (min_len > min_cap)
    min_cap = min_len;
  if (min_cap <= da_cap(a))
    return a;
  if (min_cap < 2 * da_cap(a))
    min_cap = 2 * da_cap(a);
  else if (min_cap < 4)
    min_cap = 4;
  b = DA_REALLOC(NULL, (a) ? da_hdr(a) : 0, elemsize * min_cap + sizeof(da_header));
  b = (char *) b + sizeof(da_header);
  if (a == NULL) {
    da_hdr(b)->len = 0;
  }
  da_hdr(b)->cap = min_cap;
  return b;
}
inline void da_freef(void *a){
  DA_FREE(NULL, da_hdr(a));
}


//RNG
static inline u64 xorshift64(u64 state[])
{
  u64 x = state[0];
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  state[0] = x;
  return x;
}
static u64 xorshift64_example_state[1] = {42};
#define RND_SEED(x) (xorshift64_example_state[0] = x)
#define RND() (xorshift64(xorshift64_example_state))

//djb2 hash for more info visit http://www.cse.yorku.ca/~oz/hash.html
static inline u64 hash_str(char *s)
{
    unsigned long hash = 5381;
    int c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;

}

typedef union {
    struct { f32 x; f32 y; };
    f32 v[2];
}v2;
typedef union {
    struct { i32 x; i32 y; };
    i32 v[2];
}iv2;

typedef struct {
  int x,y,w,h;
}mRect;
typedef enum {
    M_ERR = 0,
    M_OK,
}M_RESULT;



#endif