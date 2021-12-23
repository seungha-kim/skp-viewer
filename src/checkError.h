#pragma once

void priv_checkError(const char* file, int line);

#define checkError() priv_checkError(__FILE__, __LINE__)
