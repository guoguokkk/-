#include<functional>
#include<list>


int funcA(int a, int b)
{
	printf("funcA\n");
	return 0;
}

int main()
{
	std::function<int(int)> call;

	int n = 5;
	//[]捕获列表
	//()参数列表
	//{}函数体
	call = [n](int a) ->int
	{
		printf("funcB: %d, %d\n", n, a);
		return 1;
	};
	int s = call(1);
	printf("%d", s);
	return 0;
}