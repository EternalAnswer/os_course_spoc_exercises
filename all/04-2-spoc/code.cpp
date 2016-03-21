#include <cstdio>
#include <algorithm>
#include <cstring>

using namespace std;

const int Maxn = 10000;

int lastVisitTime[Maxn];
int seq[Maxn];
int limit;

int main()
{
	scanf("%d", &limit);

	memset(lastVisitTime, -1, sizeof(lastVisitTime));

	int lastMiss = 0;
	int curTime = 0;
	int head = 0;

	for (;;)
	{
		int page;
		scanf("%d", &page);
		seq[curTime] = page;
		if (lastVisitTime[page] == -1)
		{
			printf("Miss\n");
			if (curTime - lastMiss <= limit)
				lastVisitTime[page] = curTime;
			else
			{
				lastVisitTime[page] = curTime;
				for (int i = head;i < lastMiss;++ i)
					if (lastVisitTime[i] < lastMiss)
						lastVisitTime[page] = -1;
				head = lastMiss;
			}
			lastMiss = curTime;
		}
		else
		{
			printf("Hit\n");
			lastVisitTime[page] = curTime;
		}
		++ curTime;
	}

	return 0;
}