#pragma once 
#include <iostream>
#include <string>

using namespace std;

void* loadLibrary(const string& path); 
void* getFunction(void* library, const string& name); 
void closeLibrary(void* library);
