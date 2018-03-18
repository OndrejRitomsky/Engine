Engine:

This project is engine (2D) for games in c++, utilising only minimum amount of libraries.

I am using VS 2017 and x64 only. 

(The commited glew.lib is 2015 and might make problems in release conf., I did not need release so far)


Currently containing:

	(More will be commited when its ready)

	Core library: 
	Mostly collections and commonly used algs.
	Its one of the few places where templates are used. 
	To minimize code in templates, collections often use base class, containing internal (uglier) implementation which doesnt depend upon template.

	Main TODOs for core library:
		Rework handlemap
		Change IAllocator from interface to function pointers (will get rid of all virtual functions)
		Implementation of move and forward
		Wrap often used functions like memcpy, memmove
		Better jobpool

	Renderer:
	This is first try of implementing abstract renderer, its mostly unfinished
	to make it truly abstract I ll need first create second implementation (DX11), but before that I need to connect
	the renderer to the rest of the engine (which will be commited, when its ready) and see what does and does not work.

	Currently there is no way to initiate the opengl renderer, I am still using sdl to do that, but I ll remove that later and do proper
	initialisation.

	Main Renderer for core library:
		Rework allocations to use tagged allocators
		Initialisation
		Changing content of buffers (currently the function is missing)
		Change virtual functions to function pointers (as in Core)
		
		
	Test suit and test libraries:
	Few tests to test core library collections, array, hashmap, handlemap
	It needs more tests, but already this few helped a few times
	The test suit is not the nicest and require the tests to be registered manualy
	There are few performance tests for hashmap
	
	
	
	



