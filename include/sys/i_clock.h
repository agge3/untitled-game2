#pragma once

class IClock {
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void restart() = 0;
};
