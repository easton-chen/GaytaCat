long long num = 5;

long long fun(long long n)
{
	if( n == (long long)0 ) return (long long)1;
	else return n * fun(n-1);
}

int main()
{
	long long b;	
	b = fun(num);
	return 0;
}
