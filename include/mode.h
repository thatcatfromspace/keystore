#pragma once
#include <string>

enum class ModeStatus { Starting,
	                    Running,
	                    Stopping,
	                    Stopped,
	                    Error };

class Mode {
  public:
	virtual void run() = 0;
	virtual void cleanup() = 0;
	virtual void handleSignal(int signal) = 0;
	virtual ModeStatus getStatus() const = 0;
	virtual ~Mode() = default;
};
