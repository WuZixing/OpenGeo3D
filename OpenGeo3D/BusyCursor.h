#pragma once

class BusyCursor {
public:
	BusyCursor();
	~BusyCursor();

	static void beginWaiting();
	static void endWaiting();

};
