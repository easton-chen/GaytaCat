#include<stdio.h>

int num = 5;

int fun(int n)
{
	if( n == 0 ) return 1;
	else return n * fun(n-1);
}

int main()
{
	int b;	
	b = fun(num);
	printf("%d\n",b);
	return 0;
}
