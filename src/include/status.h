#ifndef _STATUS_H_
#define _STATUS_H_

#define S_OK        0   // Success
#define S_GENERR   -1   // Generic error
#define S_UIINERR  -2   // User input error
#define S_NEARG    -3   // Not enough arguments
#define S_NULLPTR  -4   // Null pointer
#define S_MALLOC   -5   // Memory allocation failed
#define S_REALLOC  -6   // Memory reallocation failed
#define S_FERR     -7   // Generic file error
#define S_FNF      -8   // File not found
#define S_FEX      -9   // File exists
#define S_FPERM    -10  // File permissions

#endif // !_STATUS_H_
