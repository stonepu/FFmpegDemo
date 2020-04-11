#include <iostream>
#include "Processor.h"
#include <string>

#undef main

int main() {

	int ret = 0;
	std::string filename("E:/winter/pict1.mp4");
	auto grabber = new Grabber();
	try {
		grabber->open(filename);
		cout << "open success" << endl;

		auto vdecoder = new VideoDecoder();
		//ret = vdecoder->initCodec(grabber->getVideoCtx());
		//if (ret < 0) {
		//	cout << "init v failed" << endl;
		//}
		//cout << "init v success" << endl;

		//auto adecoder = new AudioDecoder();
		//ret = adecoder->initCodec(grabber->getAudioCtx());
		//if (ret < 0) {
		//	cout << "init a failed" << endl;
		//}
		//cout << "init a success" << endl;

		auto render = new AVRender();

		auto processor = new Processor();
		processor->setGrabber(grabber);
		//processor->setADecoder(adecoder);
		processor->setVDecoder(vdecoder);
		processor->setRender(render);
		processor->start();

		render->loopEvent();
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		//return 0;
	}
	catch (Error e) {
		cout << e.what() << endl;
	}

_END:
	system("pause");
	
}