#pragma once

class BusyCursor {
public:
	BusyCursor();
	~BusyCursor();

	static void BeginWaiting();
	static void EndWaiting();

};
