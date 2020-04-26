#include <iostream>
#include "Processor.h"
#include <string>

#undef main

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

using std::cout;
using std::endl;
int main() {

	int ret = 0;
	std::string filename("G:/uttorent2��/��������/��������.mp4");
	auto grabber = new Grabber();
	try {
		grabber->open(filename);
		cout << "open success" << endl;

		auto render = new AVRender();

		auto processor = new Processor();
		processor->setGrabber(grabber);
		processor->setRender(render);
		processor->start();

		//���߳̽���ͼ�����;
		processor->startDraw();
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		goto _END;
	}

_END:
	system("pause");
	
}