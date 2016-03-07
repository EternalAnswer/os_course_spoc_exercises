// use buddy system to allocate memory
// 加入链表使得能够优先选择可分配的较小的空闲内存块，复杂度依然为LogN
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

const int MAX_MEM = 1 << 20;
const int MAX_DEP = 20;

struct Tnode
{
	int size;
	int maxSize, l, r;
	bool allocated;
};

Tnode memoryTree[MAX_MEM * 4];
int head[MAX_DEP + 1];
int _next[MAX_MEM], _prev[MAX_MEM];

void build(int index, int l, int r)
{
	memoryTree[index].size = memoryTree[index].maxSize = r - l;
	memoryTree[index].allocated = false;
	memoryTree[index].l = l; memoryTree[index].r = r;

	if (l + 1 == r)
		return;
	build(index * 2 + 1, l, (l + r) >> 1);
	build(index * 2 + 2, (l + r) >> 1, r);
}

int getLog(int x)
{
	int res = -1;
	for (;x;x >>= 1) ++ res;
	return res;
}

void del(int pos, int size)
{
	size = getLog(size);
	if (_prev[pos] == -1)
	{
		head[size] = _next[pos];
		if (_next[pos] != -1) _prev[_next[pos]] = -1;
		_next[pos] = -1;
	}
	else
	{
		_next[_prev[pos]] = _next[pos];
		if (_next[pos] != -1) _prev[_next[pos]] = _prev[pos];
		_next[pos] = -1; _prev[pos] = -1;
	}
}

void add(int pos, int size)
{
	size = getLog(size);
	_next[pos] = head[size];
	_prev[pos] = -1;
	if (_next[pos] != -1) _prev[_next[pos]] = pos;
	head[size] = pos;
}

int _allocate(int startPosition, int size)
{
	int index = 0, offset = 0;
	for (;offset != startPosition;)
	{
		if (offset + (memoryTree[index].size >> 1) <= startPosition)
		{
			index = (index << 1) + 2;
			offset += memoryTree[index].size;
		}
		else index = (index << 1) + 1;
	}
	for (;;)
	{
		if (memoryTree[index].size > 1 && memoryTree[(index << 1) + 1].maxSize >= size)
		{
			index = (index << 1) + 1;
			add(startPosition + memoryTree[index].size, memoryTree[index].size);
		}
		else break;
	}

	memoryTree[index].maxSize = 0;
	memoryTree[index].allocated = true;
	
	for (;;)
	{
		if (index == 0) break;
		index = (index - 1) >> 1;
		memoryTree[index].maxSize = max(memoryTree[(index << 1) + 1].maxSize, memoryTree[(index << 1) + 2].maxSize);
	}

	return startPosition;
}

int allocate(int size)
{
	if (size == 0) return -1;
	int tmp;
	for (int i = 0;i <= MAX_DEP;++i)
		if ((1 << i) >= size && head[i] != -1)
		{
			tmp = head[i];
			del(head[i], 1 << i);
			return _allocate(tmp, size);

		}
	return -1;
}

int freeMemory(int startPosition)
{
	int offset = 0;
	int nowIndex = 0;
	int returnValue, tmp, bro, last;
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
				last = nowIndex;
				for (;;)
				{
					if (nowIndex == 0) break;
					tmp = nowIndex;
					nowIndex = (nowIndex - 1) >> 1;
					if (memoryTree[(nowIndex << 1) + 1].maxSize + memoryTree[(nowIndex << 1) + 2].maxSize == memoryTree[nowIndex].size)
					{
						memoryTree[nowIndex].maxSize = memoryTree[nowIndex].size;
						bro = nowIndex * 2 + 3 - tmp;
						del(memoryTree[bro].l, memoryTree[bro].size);
						last = nowIndex;
					}
					else 
						memoryTree[nowIndex].maxSize = max(memoryTree[(nowIndex << 1) + 1].maxSize, memoryTree[(nowIndex << 1) + 2].maxSize);
				}
				add(memoryTree[last].l, memoryTree[last].size);
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
	for (int i = 0;i <= MAX_DEP;++i)
		head[i] = -1;
	for (int i = 0;i < MAX_MEM;++i)
		_next[i] = _prev[i] = -1;
	head[MAX_DEP] = 0;

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