





/*
float results[800];

core::ThreadPool pool;


void FuncEnd(void* data) {
	int index = (int) data;

	float sum = 0;
	for (int i = 0; i < 1024; ++i) {
		sum += cos(i + index);
		sum -= sin(i + index);
	}

	results[index] -= sum;
}

void Func(void* data) {
	int index = (int) data;

	float sum = 0;
	for (int i = 0; i < 1024; ++i) {
		sum += cos(i + index);
		sum -= sin(i + index);
	}

	results[index] = sum;


	auto fn = core::ThreadPool::TaskFunction::Make<FuncEnd>();
	pool.TryAddTask(fn, (void*) index);
}
*/




#include <windows.h>

double PCFreq = 0.0;
__int64 CounterStart = 0;

static void StartCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	

	PCFreq = double(li.QuadPart) / 1000000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
static double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}


#include <core/parallel/job_pool.h>

void PrintPrint(const char* format, ...) {
	/*char buff[1024];

	va_list args;
	va_start(args, format);
	vsprintf_s(buff, 1024, format, args);
	va_end(args);
	OutputDebugStringA(buff);*/
}




const int AGR_COUNT = 4;
float results[AGR_COUNT];
/*
void FnFn(void* arg) {
	int k = (int) arg;
	for (int i = 0; i < 40; ++i) {
		results[k] += sin((float) i) * 2.0f + (float) k;
	}
}
*/
int nmain(int argc, char* args[]) {

//	core::HeapAllocator allocator;

//	eng::Engine eng;
	//eng.Init(&allocator);
	//eng.Loop();

	//eng.Loop2();

	
	/*core::SafeTaggedBlockAllocator taggedAllocator(&allocator, 2 * 1024);

	void* data = taggedAllocator.AllocateTaggedBlock(4);
	void* data2 = taggedAllocator.AllocateTaggedBlock(5);
	void* data3 = taggedAllocator.AllocateTaggedBlock(6);
	void* data4 = taggedAllocator.AllocateTaggedBlock(7);


	void* data5 = taggedAllocator.AllocateTaggedBlock(4);
	
	void* data6 = taggedAllocator.AllocateTaggedBlock(5);
	void* data7 = taggedAllocator.AllocateTaggedBlock(4);

	taggedAllocator.ClearByTag(5);

	void* data8 = taggedAllocator.AllocateTaggedBlock(5);
	void* data9 = taggedAllocator.AllocateTaggedBlock(6);
	
	void* data10 = taggedAllocator.AllocateTaggedBlock(7);
	void* data11 = taggedAllocator.AllocateTaggedBlock(7);*/

	/*core::SafeTaggedBlockAllocator taggedAllocator(&allocator, 2 * 1024);

	
	//void* data1 = taggedAllocator.AllocateTaggedBlock(4);
	void* data2 = taggedAllocator.AllocateTaggedBlock(5);
	void* data3 = taggedAllocator.AllocateTaggedBlock(4);
	void* data = taggedAllocator.AllocateTaggedMultiBlock(4, 3);
	void* data99 = taggedAllocator.AllocateTaggedMultiBlock(4, 3);

	taggedAllocator.ClearByTag(4);

	//void* data4 = taggedAllocator.AllocateTaggedBlock(4);

	void* data5 = taggedAllocator.AllocateTaggedMultiBlock(4, 3);*/


	/*core::SafeTaggedBlockAllocator taggedAllocator(&allocator, 2 * 1024, sizeof(u64));
	core::SingleTagProxyAllocator tagAllocator(&taggedAllocator, 4);
	void* a = tagAllocator.Allocate(2048, 8);
	taggedAllocator.ClearByTag(4);*/




	/*	
	void* queue = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
	
	HANDLE handle = CreateFileA("Shaders/textVertex3.glsl", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);

	HANDLE hp = CreateIoCompletionPort(handle, queue, 12, NULL);

	DWORD bytesRead;
	ULONG_PTR id;

	OVERLAPPED* overlapped = new OVERLAPPED();

	Memset(overlapped, 0, sizeof(OVERLAPPED));

	char buffer[1024 * 256];

	ReadFile(handle, buffer, 1024*256, NULL, overlapped);

	::CloseHandle(handle);
	//::CloseHandle( handle);

	SetLastError(0);

	int k = 0;


	while (k++ < 100) {
		
		SetLastError(0);
	if (GetQueuedCompletionStatus(queue, &bytesRead, &id, &overlapped, 0)) {
		DWORD a = GetLastError();

		printf("%d \n", a);
	} else {

		DWORD a = GetLastError();
		printf("else %d \n", a);
	}
	}

	*/
	
	//WAIT_TIMEOUT
	// k = 4;



	//pool.Init();
	//
	//auto fn = core::ThreadPool::TaskFunction::Make<Func>();
	//for (int i = 0; i < 800; i++) {
	//	pool.TryAddTask(fn, (void*) i);
	//}
 	//
	//
	//pool.WaitAllAndWork();
	//
	//
	//pool.Destroy();
	
/*	void* data;
	for (int i = 0; i < 160; i++) {
		data = (void*) i;
		Func(data);
	}

	printf("%f \n", results[0]);*/




	// vyskusat s async file loaderom dat loadovat dat zrusit a pak kuknut completition port!!!, pretoze zrusenie loadenie shadera by sa hodilo
	// aj ked premysliet ci to dava zmysel, co to vlastne znamena ze loadime a zrazu treba lodnut znova

	return 0;
}

