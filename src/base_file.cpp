#include <stdio.h> //fopen, rename
#include <string>
#include <string.h> // setvbuf
#include <iostream>
#include <chrono>
#include "base_file.h"

namespace logging {

char error_str[512];
const char *error_to_str(int errnum) {
return strerror_r(errnum, error_str, sizeof(error_str));
}

void BaseFile::rename(const char *old_filename, const char *new_filename) {
int ret = ::rename(old_filename, new_filename);
if (0 != ret) {
std::cerr << "failed in logging::BaseFile::rename\n";
}
}

void BaseFile::close(void) {
fflush(_file);
fclose(_file);
_file = NULL;
}

BaseFile::BaseFile(const std::string file_name) {
_written_bytes = 0;

_file = fopen(file_name.c_str(), "ae");
if (NULL == _file) {
std::cerr << "failed in logging::BaseFile::BaseFile, fopen return NULL\n";
} else {
int ret = setvbuf(_file, _local_buffer, _IOFBF, sizeof(_local_buffer));
if (0 != ret) {
int err = ferror(_file);
std::cerr << "failed in logging::BaseFile::append, setvbuf error info:" << error_to_str(err) << std::endl;
}
}
}

BaseFile::~BaseFile(void) {
if (NULL != _file){
fflush(_file);
fclose(_file);
_file = NULL;
}
}

void BaseFile::append_data(const char *data, size_t size, bool flush_now) {
size_t remainder = size;
do {
size_t n = fwrite(data, 1, remainder, _file);

// records the number of bytes written to the file
_written_bytes += n;

if (n != remainder) {
int err = ferror(_file);
if (err){
std::cerr << "failed in logging::BaseFile::append,error info:" << error_to_str(err) << std::endl;
break;
}
}
remainder -= n;

} while (0 != remainder);

if (flush_now) {
fflush(_file);
}
}

size_t BaseFile::get_written_bytes(void) {
return _written_bytes;
}

void BaseFile::flush(void) {
fflush(_file);
}

} // namespace logging