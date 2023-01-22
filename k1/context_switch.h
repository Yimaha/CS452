
#pragma once

extern "C" void foo(char* sp); // print the sp value
extern "C" void bar(); // print the sp value
extern "C" void setSP(char* sp); // used to set the current sp, which is kinda useful at the beginning
// extern "C" void switch_to_user(char * userSP);
 
