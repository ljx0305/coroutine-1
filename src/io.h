#ifndef __IO_H__
#define __IO_H__

#include "coroutine.h"

class Epoller;

Epoller* get_epoller();

void EventLoop();

int Connect(const int& fd, const std::string& ip, const uint16_t& port);
int Read(const int& fd, uint8_t* data, const int& size);
int Write(const int& fd, const uint8_t* data, const int& size);
int ReadGivenSize(const int& fd, uint8_t* data, const int& size);
int WriteGivenSize(const int& fd, const uint8_t* data, const int& size);

#endif // __IO_H__
