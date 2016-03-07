// use buddy system to allocate memory
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

const int MAX_MEM = 1 << 20;

struct Tnode
{
	int size;
	int maxSize;
	bool allocated;
};

Tnode memoryTree[MAX_MEM * 4];

void build(int index, int l, int r)
{
	memoryTree[index].size = memoryTree[index].maxSize = r - l;
	memoryTree[index].allocated = false;

	if (l + 1 == r)
		return;
	build(index * 2 + 1, l, (l + r) >> 1);
	build(index * 2 + 2, (l + r) >> 1, r);
}

int allocate(int size)
{
	if (size == 0 || size > memoryTree[0].maxSize) return -1;

	int nowIndex = 0;
	int startPosition = 0;
	for (;size != memoryTree[nowIndex].size;)
	{
		if (memoryTree[(nowIndex << 1)+ 1].maxSize >= size) nowIndex = (nowIndex << 1) + 1;
		else
		{
			nowIndex = (nowIndex << 1) + 2;
			startPosition += memoryTree[nowIndex].size;
		}
	}

	memoryTree[nowIndex].maxSize = 0;
	memoryTree[nowIndex].allocated = true;

	for (;;)
	{
		if (nowIndex == 0) break;
		nowIndex = (nowIndex - 1) >> 1;
		memoryTree[nowIndex].maxSize = max(memoryTree[(nowIndex << 1) + 1].maxSize, memoryTree[(nowIndex << 1) + 2].maxSize);
	}

	return startPosition;
}

int freeMemory(int startPosition)
{
	int offset = 0;
	int nowIndex = 0;
	int returnValue;
	for (;;)
	{
		bool lastLayer = (memoryTree[nowIndex].size == 1);
		if (offset == startPosition)
		{
			if (memoryTree[nowIndex].allocated)
			{
				memoryTree[nowIndex].allocated = false;
				memoryTree[nowIndex].maxSize = memoryTree[nowIndex].size;
				returnValue = memoryTree[nowIndex].size;
				for (;;)
				{
					if (nowIndex == 0) break;
					nowIndex = (nowIndex - 1) >> 1;
					if (memoryTree[(nowIndex << 1) + 1].maxSize + memoryTree[(nowIndex << 1) + 2].maxSize == memoryTree[nowIndex].size)
						memoryTree[nowIndex].maxSize = memoryTree[nowIndex].size;
					else 
						memoryTree[nowIndex].maxSize = max(memoryTree[(nowIndex << 1) + 1].maxSize, memoryTree[(nowIndex << 1) + 2].maxSize);
				}
				return returnValue;
			}
			nowIndex = (nowIndex << 1) + 1;
		}
		else
		{
			if (offset + (memoryTree[nowIndex].size >> 1) > startPosition)
				nowIndex = (nowIndex << 1) + 1;
			else
			{
				nowIndex = (nowIndex << 1) + 2;
				offset += memoryTree[nowIndex].size;
			}
		}
		if (lastLayer) break;
	}
	return -1;
}

int main()
{
	build(0, 0, MAX_MEM);

	for (;;)
	{
		int type, argv;
		// 0 for allocate, 1 for free
		scanf("%d %d", &type, &argv);

		if (type == 0)
		{
			int allocatedSize = 1;
			for (;allocatedSize <= MAX_MEM && allocatedSize < argv;allocatedSize <<= 1);
			int pos = -1;
			if (allocatedSize <= MAX_MEM && argv > 0)
				pos = allocate(allocatedSize);
			if (pos == -1)
				printf("failed to allocate memory for size %d\n", argv);
			else
				printf("successfully allocate memory : start_position = %d, size = %d\n", pos, allocatedSize);
		}
		else
		{
			int freeSize = -1;
			freeSize = freeMemory(argv);
			if (freeSize != -1)
				printf("successfully free the memory block : start_position = %d, size = %d\n", argv, freeSize);
			else
				printf("failed to free the memory starting from %d\n", argv);
		}
	}
	return 0;
}