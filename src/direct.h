#ifdef WINELIB

#ifndef _DIRECT_H_
#define _DIRECT_H_

#include <filesystem>
#include <string>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Path Manipulation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Sizes for buffers used by the _makepath() and _splitpath() functions.
// note that the sizes include space for 0-terminator
#define _MAX_PATH   260 // max. length of full pathname
#define _MAX_DRIVE  3   // max. length of drive component
#define _MAX_DIR    256 // max. length of path component
#define _MAX_FNAME  256 // max. length of file name component
#define _MAX_EXT    256 // max. length of extension component



// Structure to hold file information (simplified)
struct _finddata_t {
    char* name;
    unsigned long attrib; // For this example, we ignore file attributes
};

int _findfirst(const std::string& pattern, _finddata_t* fBuffer);

int _findnext(int hFile, struct _finddata_t* fBuffer);

// Global vector to store the list of files found
extern std::vector<_finddata_t> findHandles;

// Function prototype for _findnext
int _findnext(int hFile, struct _finddata_t *fBuffer);

int _mkdir(const char* path);

#endif
#endif