#include <iostream>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"

void Utils::getFiles(std::string path, std::vector<std::string>& files)
{
	// check the parameter !
	if( path.empty() ) {
		return;
	}
	// check if dir_name is a valid dir
	struct stat s;
	lstat( path.c_str(), &s );
	if( ! S_ISDIR( s.st_mode ) ) {
		return;
	}

	struct dirent * filename;    // return value for readdir()
	DIR * dir;                   // return value for opendir()
	dir = opendir( path.c_str() );
	if( NULL == dir ) {
		return;
	}

	/* read all the files in the dir ~ */
	while( ( filename = readdir(dir) ) != NULL ) {
		// get rid of "." and ".."
		if( strcmp( filename->d_name , "." ) == 0 ||
			strcmp( filename->d_name , "..") == 0 )
			continue;
        std::string full_path = path + filename->d_name;
        struct stat s;
        lstat( full_path.c_str(), &s );
        if( S_ISDIR( s.st_mode ) ) {
            continue;
        }
        files.push_back(full_path);
	}
}

std::string Utils::getCurrentTime0() {
    std::time_t result = std::time(nullptr);

    auto sec = std::chrono::seconds(result);
    std::chrono::time_point<std::chrono::system_clock> now(sec);
 
    auto timet = std::chrono::system_clock::to_time_t(now);
    auto localTime = *std::gmtime(&timet);
 
    std::stringstream ss;
    std::string str;
    ss << std::put_time(&localTime, "%Y/%m/%d-%H:%M:%S");
    ss >> str;
 
    return str;
}

std::string Utils::ReadFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}
