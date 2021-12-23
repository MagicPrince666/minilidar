#include <iostream>
#include <linux/input.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <iomanip>

#include "ultrasonic.h"

GpioKey::GpioKey(Xepoll *epoll)
: epoll_(epoll)
{
    char          buf[256] = { 0, };  /* RATS: Use ok */
    int           fd = -1;
    std::string dev = "";
    std::vector<std::string> events;
    getFiles("/dev/input/", events);
    for (auto iter : events) {
        std::cout << "Device name "<< iter << std::endl;
        if ((fd = open(iter.c_str(), O_RDONLY, 0)) >= 0) {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            dev = buf;
            std::cout << "Device info "<< dev << std::endl;
            if(dev == "gpio-keys") {
                key_input_fd_ = fd;
                break;
            }
            close(fd);
        }
    }
    if(key_input_fd_ <= 0) {
        exit(0);
    }
    init();
}

GpioKey::~GpioKey(void)
{
    if (key_input_fd_ > 0) {
        close(key_input_fd_);
    }
}

void GpioKey::getFiles(std::string path, std::vector<std::string>& files)
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

int GpioKey::IRKey(void)
{
    struct input_event key;
    int ret = read(key_input_fd_, &key, sizeof(key));
    if(ret > 0) {
        if(key.code == 103) {
            // std::cout << "Value = " << key.value <<std::endl;
            // std::cout << "Time = " << key.time.tv_sec << "." << (key.time.tv_usec)/1000 << " s" <<std::endl;

            if(key.value == 1) { // 记录开始时间
                last_time_ = key.time;
            } else {
                // 换算距离
                double time = 0.0;
                double starttime = last_time_.tv_sec * 1000000 + last_time_.tv_usec;
                double endtime = key.time.tv_sec * 1000000 + key.time.tv_usec;

                time = (endtime - starttime) * 0.0001;

                double distance = 170.0 * time * 0.0001;
                std::cout << "Time = " << time << " ms" <<std::endl;
                std::cout << "Distance = " << std::setprecision(3) << distance << " m" <<std::endl;
            }
        }
    }
    return ret;
}

bool GpioKey::init() {
  // 绑定回调函数
  if (key_input_fd_ > 0) {
        std::cout << "Bind epoll" << std::endl;
        epoll_->add(key_input_fd_, std::bind(&GpioKey::IRKey, this));
  }
  return true;
}

void GpioKey::Transfer(int num) {
    is_action_ = num;
}
