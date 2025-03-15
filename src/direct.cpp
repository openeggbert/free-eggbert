#include <iostream>
#include <filesystem>
#include <string>
#include "direct.h"
#include <cstring>



// Function that mimics _findfirst
int my_findfirst(const std::string& pattern, _finddata_t* fBuffer) {
    namespace fs = std::filesystem;

    // Get the directory path from the pattern
    fs::path p(pattern);

    // Check if there's a filename pattern (e.g., "*.xch")
    if (p.has_filename()) {
        fs::path dir = p.parent_path();
        std::string file_pattern = p.filename().string();

        // Iterate through files in the directory matching the pattern
        for (const auto& entry : fs::directory_iterator(dir)) {
            // Check if the filename contains the pattern
            if (entry.path().filename().string().find(file_pattern) != std::string::npos) {
                // Convert std::string to C-string and assign it to fBuffer->name
                std::string filename = entry.path().filename().string();
                strncpy(fBuffer->name, filename.c_str(), sizeof(fBuffer->name) - 1);
                fBuffer->name[sizeof(fBuffer->name) - 1] = '\0';  // Null-terminate
                return 0; // Success
            }
        }
    }

    return -1; // Error or file not found
}

std::vector<_finddata_t> findHandles;

int _findnext(int hFile, struct _finddata_t* fBuffer) {
    // Check if the handle is valid
    if (hFile == -1) {
        return -1;
    }

    // Retrieve the next file in the list for the given directory handle
    try {
        // This method will now retrieve the next file using the file iterator (if valid)
        if (hFile < findHandles.size()) {
            // Copy the data for the found file into the fBuffer struct
            *fBuffer = findHandles[hFile];
            // Increment handle counter to simulate getting the next file
            ++hFile;
            return 0;
        } else {
            return -1;  // End of directory
        }
    } catch (...) {
        return -1;  // Error
    }
}

// Function to mimic _mkdir from Windows
int _mkdir(const char* path) {
    try {
        // Use filesystem to create the directory
        std::filesystem::create_directory(path);
        return 0;  // Success
    } catch (const std::filesystem::filesystem_error& e) {
        // If there is an error (e.g., the directory already exists), return -1
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return -1;  // Failure
    }
}



