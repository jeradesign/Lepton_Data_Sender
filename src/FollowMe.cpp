#include "mraa.hpp"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <cassert>
#include <opencv2/core/core.hpp>

using namespace mraa;
using namespace cv;

const int LINE_SIZE = 164;
const int PAYLOAD_SIZE = 160;
const int SCAN_LINES = 60;
const int SCAN_COLUMNS = 80;
const int CHUNK_SIZE = 20 * LINE_SIZE;
const int FRAME_SIZE = SCAN_LINES * LINE_SIZE;

const int MAX_SCAN_LINES = 200;

Spi *spi;
Gpio *chipSelect;

uint8_t paranoia1[LINE_SIZE];
uint8_t sendLine[LINE_SIZE];
uint8_t paranoia2[LINE_SIZE];
uint8_t recvLine[LINE_SIZE];
uint8_t paranoia3[LINE_SIZE];
uint8_t sendFrame[FRAME_SIZE];
uint8_t paranoia4[CHUNK_SIZE];
uint8_t recvFrame[FRAME_SIZE];
uint8_t paranoia5[CHUNK_SIZE];
uint8_t sendChunk[CHUNK_SIZE];
uint8_t paranoia6[CHUNK_SIZE];
uint8_t recvChunk[CHUNK_SIZE];
uint8_t paranoia7[CHUNK_SIZE];

int scanLines[MAX_SCAN_LINES];

void setupSPI()
{
	chipSelect = new Gpio(10);
	chipSelect->dir(DIR_OUT);

	spi = new mraa::Spi(5);	// bus 5 for edison
	spi->mode(SPI_MODE3);
	spi->frequency(12000000);
	spi->lsbmode(0);
	spi->bitPerWord(8);

	chipSelect->write(1);	// deselect chip
	usleep(200000);
	chipSelect->write(0);	// low to select chip
}

int processScanLine()
{
	spi->transfer(sendLine, recvLine, LINE_SIZE);
	return 256 * recvLine[0] + recvLine[1];
}

int main()
{
	setupSPI();

	for (;;) {
		printf("\033[1;1H");
		int i = 0;
		int result;
		do {
			result = processScanLine();
			scanLines[i++] = result;
		} while ((result & 0x0fff) == 0x0fff);

		paranoia5[0] = 0xa5;
		paranoia5[1] = 0x5a;
		assert(paranoia5[0] == 0xa5);
		assert(paranoia5[1] == 0x5a);

		memcpy(recvFrame, recvLine, LINE_SIZE);

		mraa_result_t mraaResult = spi->transfer(sendChunk, recvChunk, CHUNK_SIZE);
		memcpy(recvFrame + LINE_SIZE, recvChunk, CHUNK_SIZE);

		mraaResult = spi->transfer(sendChunk, recvChunk, CHUNK_SIZE);
		memcpy(recvFrame + LINE_SIZE + CHUNK_SIZE, recvChunk, CHUNK_SIZE);

		mraaResult = spi->transfer(sendChunk, recvChunk, FRAME_SIZE - 2 * CHUNK_SIZE);
		memcpy(recvFrame + LINE_SIZE + 2 * CHUNK_SIZE, recvChunk, FRAME_SIZE - 2 * CHUNK_SIZE - LINE_SIZE);

		assert(paranoia5[0] == 0xa5);
		assert(paranoia5[1] == 0x5a);

		cv::Mat img(SCAN_LINES, SCAN_COLUMNS, CV_16UC1, recvFrame + 4, LINE_SIZE);

//		printf("mraa_result = %d\n", mraaResult);

		static char *charArray = "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";

		for (int k = 0; k < SCAN_LINES; k++) {
			for (int m = 0; m < SCAN_COLUMNS; m++) {
				//			printf("k = %d, m = %d\n", k, m);
//				fflush(stdout);
				uint8_t c = (img.at<uint16_t>(k, m) >> 8) & 0x3f;
				printf("%c", charArray[c]);
//				fflush(stdout);
			}
//			printf("\n");
//			fflush(stdout);
		}
	}
}

