#pragma once

namespace core {

	class Thread {
	public:
		using Function = unsigned long(*)(void*);

	public:
		Thread();
		~Thread();

		bool Create(Function fn, void* parameter);
		bool Join();

	private:
		void* _handle;
		bool _running;
		unsigned long _id;
	};

}
