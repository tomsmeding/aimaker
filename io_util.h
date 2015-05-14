#pragma once

#include <iostream>

using namespace std;

void clearScreen(void){
	cout<<"\x1B[2J"<<flush;
}
