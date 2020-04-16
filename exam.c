#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char hello[128] = {0x0,};
	char *input_one;
	char *input_two;
	char *input_three;
	char *pText;

	scanf("%s",&hello);

	pText = hello;
	input_one = strsep(&pText,",");
	input_two = strsep(&pText,",");
	input_three = strsep(&pText,",");

	printf("one : %s two: %s three: %s",input_one,input_two,input_three);

	return 0;
}

