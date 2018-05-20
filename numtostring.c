#include "numtostring.h"

static void reverse(s) char *s; {
	char *j;
	int c;
 
	j = s + strlen(s) - 1;
	while (s < j) {
		c = *s;
		*s++ = *j;
		*j-- = c;
	}
}
//void itoa(int n, char s[]) {
//	int i, sign;
// 
//	if ((sign = n) < 0)  /* record sign */
//		n = -n;          /* make n positive */
//	i = 0;
//	do {       /* generate digits in reverse order */
//		s[i++] = n % 10 + '0';   /* get next digit */
//	} while ((n /= 10) > 0);     /* delete it */
//	if (sign < 0)
//		s[i++] = '-';
//	s[i] = '\0';
//	reverse(s);
//}
void ftos(char* str, float f, uint8_t precision) {	
	int a, b, c, k, l = 0, m, i = 0, j;
	
	// check for negetive float
	if (f < 0.0) {
		
		str[i++] = '-';
		f *= -1;
	}
	
	a = f;	// extracting whole number
	f -= a;	// extracting decimal part
	k = precision;
	
	// number of digits in whole number
	while (k > -1) {
		l = pow(10, k);
		m = a / l;
		if (m > 0) {
			break;
		}
		k--;
	}

	// number of digits in whole number are k+1
	
	/*
	extracting most significant digit i.e. right most digit , and concatenating to string
	obtained as quotient by dividing number by 10^k where k = (number of digit -1)
	*/
	
	itoa(a, str, 10);
	if (a < 10)
	{
		str[i++] = '0';
	}
	for (l = k + 1; l > 0; l--) {
		b = pow(10, l - 1);
		c = a / b;
		str[i++] = c + 48;
		a %= b;
	}
	if (i == 1)
	{		
		str[i++] = '0';
	}	
	str[i++] = '.';
	
	/* extracting decimal digits till precision */

	for (l = 0; l < precision; l++) {
		f *= 10.0;
		b = f;
		str[i++] = b + 48;
		f -= b;
	}

	str[i] = '\0';

}