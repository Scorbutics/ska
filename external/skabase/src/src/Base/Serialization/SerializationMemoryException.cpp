#include <cstdlib>
#include <cstring>
#include "SerializationMemoryException.h"

char const* ska::SerializationMemoryException::what() const noexcept {
    static char buffer[MAX_BUF];
    memset(buffer, 0, MAX_BUF);
    std::size_t counter = 0;

    for (const auto& errorData : data) {
        queue(buffer, counter, "in zone \"", errorData.zone, "\", ");
        if (errorData.bytesRequested != 0) {
            queue(buffer, counter, errorData.bytesAllocated, " bytes allocated / ");
            queue(buffer, counter, errorData.bytesAvailable, " total available bytes, and ");
            queue(buffer, counter, errorData.bytesRequested, " are now requested.\n");
        } else {
            queue(buffer, counter, errorData.bytesAllocated, " bytes written/read / ");
            queue(buffer, counter, errorData.bytesAvailable, " total expected bytes.\n");
        }
        queue(buffer, counter, errorData.error, "\n");
    }
    return buffer;
}

void ska::SerializationMemoryException::queue(char buffer[], std::size_t counter, const char* str, std::size_t size) const {
	auto oldCounter = counter;
	counter += size;
	queue_(buffer, counter, oldCounter, str);
}

void ska::SerializationMemoryException::queue_(char buffer[], std::size_t counter, std::size_t oldCounter, const char* str) const {
	if (counter >= MAX_BUF) {
		if (oldCounter < MAX_BUF) {
			sprintf(buffer + (MAX_BUF - 4), "...");
		}
	} else {
		strcat(buffer, str);
	}
}
