#pragma once

int genid(void) {
	static id = 0;
	return id++;
}
