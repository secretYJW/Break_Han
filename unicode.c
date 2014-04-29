#include "unicode.h"

#define PRINT_HAN(H) printf("초성: %s, 중성: %s, 종성: %s\n", \
			(H).f_letter, (H).m_letter, (H).l_letter);

int main()
{
	int i;
	utf8_t *buf;
	utf8_t test[] = {234, 176, 132};
	utf8_t data[3][3] =  {{234, 176, 132},{236, 158, 189},{236, 157, 180}};
	utf8_t data2[3] = {227, 132, 177};
	utf8_t Test_buf[30][3];
	unicode_t test_char;
	unicode_t my_string[3];
	unicode_t Test_String[30];
	Hangul hangul;
	Hangul *hangul_string;

	// 유니코드로의 변환테스트
	test_char = Utf8_To_Uni(test);
	printf("%d\n", test_char);
	test_char = Utf8_To_Uni(data2);
	printf("%d\n", test_char);

	// utf8인코딩 테스트
	buf = Uni_To_Utf8(44036, 1);	
	printf("%s\n", buf);
	Free_Utf8(buf);

	// 자소 분리 테스트
	hangul = Break_Han(44036);
	PRINT_HAN(hangul);
	/*printf("초성: %s, 중성: %s, 종성: %s\n", 
			hangul.f_letter, hangul.m_letter, hangul.l_letter);*/
	//hangul = Break_Han(test_char);
	//PRINT_HAN(hangul);

	// 문자열 자소 분리 테스트
	for (i = 0; i < 3; i++)
		my_string[i] = Utf8_To_Uni(&data[i]);
	hangul_string = Break_String(my_string, 3);
	for (i = 0; i < 3; i++) {
		printf("초성: %s, 중성: %s, 종성: %s\n", hangul_string[i].f_letter, 
				hangul_string[i].m_letter, hangul_string[i].l_letter);
	}
	Free_Unicode(hangul_string);

	gets(Test_buf);

	for (i = 0; i < 30; i++)
		Test_String[i] = Utf8_To_Uni(&Test_buf[i]);
	hangul_string = Break_String(Test_String, 30);
	for (i = 0; i < 30; i++) 
		PRINT_HAN(hangul_string[i]);

	return 0;
}
