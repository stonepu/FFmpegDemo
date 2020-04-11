#pragma once
#include <string>
#include <exception>

using std::exception;


struct Error : public exception
{
	Error() { msg = "error"; }
	Error(const std::string& msg) { this->msg = msg; }
	const char* what() {
		return msg.c_str();
	}
private:
	std::string msg;
};