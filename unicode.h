#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// UTF-8의 유효비트 검출을 위한 마스크
#define UTF_F_MASK 0xF
#define UTF_S_MASK 0x3F
#define UTF_T_MASK 0x3F

// Unicode 분리를 위한 마스크
#define UNI_F_MASK 0xF000
#define UNI_S_MASK 0xFC0
#define UNI_T_MASK 0x3F

// UTF-8의 초중종성의 인코딩 체크
#define CHK_F(x) (((x) & 0xF0) == 0xE0)
#define CHK_S(x) (((x) & 0xC0) == 0x80)
#define CHK_T(x) (((x) & 0xC0) == 0x80)

// UTF-8의 글자간 구분 숫자
#define SEP 236

// 유니코드와 utf8셋의 자료형을 정의함
#define unicode_t   unsigned short
#define utf8_t      unsigned char 

// utf8셋으로 초중종성을 닮을 자료구조
typedef struct han_struct{
char f_idx;
    char m_idx;
    char l_idx;
    utf8_t f_letter[4];
    utf8_t m_letter[4];
    utf8_t l_letter[4];
}Hangul;

/*
 * 
struct hangul_string{
    Hangul hangul;
    Hangul *next;
}
*/

// 유니코드로 초중종성을 닮을 자료구조

// 유니코드의 한글 초중종성 테이블
static const utf8_t first_letter[][3] = {"ㄱ", "ㄲ", "ㄴ", "ㄷ", 
    "ㄸ", "ㄹ", "ㅁ", "ㅂ", 
    "ㅃ", "ㅅ", "ㅆ", "ㅇ", 
    "ㅈ", "ㅉ", "ㅊ", "ㅋ", 
    "ㅌ", "ㅍ", "ㅎ"};
static const utf8_t middle_letter[][3] = {"ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", 
    "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", 
    "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ",
    "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "l"};
static const utf8_t last_letter[][3] = {"", "ㄱ", "ㄲ", "ㄳ", 
           "ㄴ", "ㄵ", "ㄶ", "ㄷ", 
           "ㄹ", "ㄺ", "ㄻ", "ㄼ", 
           "ㄽ", "ㄾ", "ㄿ", "ㅀ", 
           "ㅁ", "ㅂ", "ㅄ", "ㅅ", 
           "ㅆ", "ㅇ", "ㅈ", "ㅊ", 
           "ㅋ", "ㅌ", "ㅍ", "ㅎ"};

unicode_t Utf8_To_Uni(utf8_t *); //utf8 인코딩을 유니코드 숫자로 바꿈
utf8_t *Uni_To_Utf8(unicode_t, unsigned int); // 유니코드 숫자를 utf8 인코딩함
Hangul Break_Han(unicode_t); // 한글의 초중종성을 분리함
utf8_t *Make_String(Hangul *, unsigned int); // 자소분리된것들을 문자열로 만듦
Hangul *Break_String(unicode_t *, unsigned int); // 문자열의 자소분리
/* 할당된 공간을 해제함*/
void Free_Hangul(Hangul *);
void Free_Utf8(utf8_t *);
void Free_Unicode(unicode_t *);

// utf8 인코딩을 유니코드 숫자로 바꾸어줌.
// utf8 인코딩에 문제가 있을경우 -1을 리턴
unicode_t Utf8_To_Uni(utf8_t *ch)
{
    unicode_t result = 0;

    //utf8 인코딩의 유효성 체크
    if (!CHK_F(ch[0]))
        return -1;
    if (!CHK_S(ch[1]))
        return -1;
    if (!CHK_T(ch[2]))
        return -1;
    
    // 인코딩에서 유효 비트 추출
    result |= ((ch[0] & UTF_F_MASK) << 12);
    result |= ((ch[1] & UTF_S_MASK) << 6);
    result |= (ch[2] & UTF_T_MASK);

    return result;
}

// 유니코드를 utf8 인코딩을 해줌
utf8_t *Uni_To_Utf8(unicode_t unicode, unsigned int flags)
{
   utf8_t *utf_set;

   // utf8 셋을 위한 공간을 할당받음
   if ((flags & 0x1) == 1) //마지막자리의 유니코드라면
   {
       utf_set = malloc(sizeof(utf8_t) * 4);
       utf_set[3] |= '\0';
   } else {
       utf_set = malloc(sizeof(utf8_t) * 3);
   }


   // utf8 셋을 유니코드로부터 생성함
   utf_set[0] |= 0xE0; 
   utf_set[0] |= ((unicode & UNI_F_MASK) >> 12);
   utf_set[1] |= 0x80;
   utf_set[1] |= ((unicode & UNI_S_MASK) >> 6);
   utf_set[2] |= 0x80;
   utf_set[2] |= (unicode & UNI_T_MASK);

   return utf_set;
}

// 한글의 자소 분리(utf8셋으로 저장)
Hangul Break_Han(unicode_t unicode)
{
    unicode_t buf;
    Hangul hangul;
    memset(&hangul, 0, sizeof(Hangul));

    buf = unicode - 0xAC00;

    // 한글의 자소분리
    hangul.f_idx = buf / (28 * 21);
    hangul.m_idx = (buf % (28 * 21)) / 28;
    hangul.l_idx = buf % 28;
    memcpy(hangul.f_letter, first_letter[hangul.f_idx], 3);
    hangul.f_letter[3] = '\0';
    memcpy(hangul.m_letter, middle_letter[hangul.m_idx], 3);
    hangul.f_letter[3] = '\0';
    memcpy(hangul.l_letter, last_letter[hangul.l_idx], 3);
    hangul.f_letter[3] = '\0';

    return hangul;
}

// 한글 유니코드 문자열의 자소를 분리해줌(utf8셋으로 저장)
Hangul *Break_String(unicode_t *unicode, unsigned int length)
{
    int i;
    Hangul *hangul_list, temp;
    hangul_list = (Hangul *)malloc(sizeof(Hangul) * length);

    // 자소분리후 저장
    for (i = 0;i < length; i++) {
        temp = Break_Han(unicode[i]);
        memcpy(&hangul_list[i], &temp, sizeof(Hangul));    
    }

    return hangul_list;
}

// 자소분리된 구조체를 문자열로 만들어서 리턴
utf8_t *Make_String(Hangul *hangul_list, unsigned int length)
{
    int i, struct_size,string_idx;
    utf8_t *utf_string;

    struct_size = sizeof(utf8_t) * 3 * length + 1;
    utf_string = malloc(sizeof(utf8_t) * 3 * length + 1);

    for (i = 0, string_idx = 0; i < length;) {
        
        /*
        utf_string[string_idx] = hangul_list[i].f_letter;
        utf_string[string_idx] = hangul_list[i].m_letter;
        if (strcmp(hangul_list[i].l_letter, "") == 0) // 종성이 비었다면
        {
            string_idx += 2;     
        } else {// 비어있지 않다면
            string_idx += 3;
        }
        */
    }
    
    return utf_string;
}

// 할당된 메모리 해제
void Free_Hangul(Hangul *hangul)
{
    free(hangul);
}

void Free_Utf8(utf8_t *utf_set)
{
    free(utf_set);
}

void Free_Unicode(unicode_t *unicode_list)
{
    free(unicode_list);
}
