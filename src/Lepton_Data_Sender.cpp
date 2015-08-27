#include "mraa.hpp"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <cassert>
#include <opencv2/core/core.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LOG_SPI_DATA 0

using namespace mraa;
using namespace cv;

const int LINE_SIZE = 164;
const int PAYLOAD_SIZE = 160;
const int SCAN_LINES = 60;
const int SCAN_COLUMNS = 80;
const int CHUNK_SIZE = 20 * LINE_SIZE;
const int FRAME_SIZE = SCAN_LINES * LINE_SIZE;

const int MAX_SCAN_LINES = 200;

int socket_fd;
struct sockaddr_in servaddr;

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
	fprintf(stderr, "deselect chip\n");
	usleep(500000);
	chipSelect->write(0);	// low to select chip
	fprintf(stderr, "select chip\n");
}

void setupConnection(char *hostname)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(11539);
    servaddr.sin_addr.s_addr = inet_addr(hostname);
    connect(socket_fd, (const sockaddr*)&servaddr, sizeof(servaddr));
}

int processScanLine()
{
	spi->transfer(sendLine, recvLine, LINE_SIZE);
	return 256 * recvLine[0] + recvLine[1];
}

bool checkFrame()
{
	  for (int i = 0; i < SCAN_LINES; i++) {
	    int rowNumber = 256 * (0x0f & recvFrame[i * LINE_SIZE]) + (0xff & recvFrame[i * LINE_SIZE + 1]);
	    if (rowNumber != i) {
	      printf("bad rowNumber. expected %d, got %d\n", i, rowNumber);
//	  	chipSelect->write(1);	// deselect chip
//	  	fprintf(stderr, "deselect chip\n");
//	  	usleep(500000);
//	  	chipSelect->write(0);	// low to select chip
//	  	fprintf(stderr, "select chip\n");
//	      FILE *fp = fopen("badframe.dat", "wb");
//	      fwrite(recvFrame, FRAME_SIZE, 1, fp);
//	      fclose(fp);
//	      exit(1);
	      return false;
	    }
	  }
	  return true;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		return -1;
	}

	setupConnection(argv[1]);

	setupSPI();

#if LOG_SPI_DATA
	FILE *fp = fopen("spidata.dat", "wb");
#endif

	for (;;) {
		int i = 0;
		int result;
//		do {
//			result = processScanLine();
//			if ((result & 0x0fff) != 0x0fff) {
//				chipSelect->write(1);	// deselect chip
//				fprintf(stderr, "deselect chip\n");
//				usleep(500000);
//				chipSelect->write(0);	// low to select chip
//				fprintf(stderr, "select chip\n");
//			}
////			scanLines[i++] = result;
//		} while ((result & 0x0fff) != 0x0fff);
//
		do {
			result = processScanLine();
			scanLines[i++] = result;
#if LOG_SPI_DATA
			fwrite(recvLine, LINE_SIZE, 1, fp);
#endif
		} while ((result & 0x0fff) == 0x0fff);

		paranoia5[0] = 0xa5;
		paranoia5[1] = 0x5a;
		assert(paranoia5[0] == 0xa5);
		assert(paranoia5[1] == 0x5a);

		memcpy(recvFrame, recvLine, LINE_SIZE);

		mraa_result_t mraaResult = spi->transfer(sendChunk, recvChunk, CHUNK_SIZE);
#if LOG_SPI_DATA
		fwrite(recvChunk, CHUNK_SIZE, 1, fp);
#endif
		memcpy(recvFrame + LINE_SIZE, recvChunk, CHUNK_SIZE);

		mraaResult = spi->transfer(sendChunk, recvChunk, CHUNK_SIZE);
#if LOG_SPI_DATA
		fwrite(recvChunk, CHUNK_SIZE, 1, fp);
#endif
		memcpy(recvFrame + LINE_SIZE + CHUNK_SIZE, recvChunk, CHUNK_SIZE);

		mraaResult = spi->transfer(sendChunk, recvChunk, FRAME_SIZE - 2 * CHUNK_SIZE);
#if LOG_SPI_DATA
		fwrite(recvChunk, FRAME_SIZE - 2 * CHUNK_SIZE, 1, fp);
#endif
		memcpy(recvFrame + LINE_SIZE + 2 * CHUNK_SIZE, recvChunk, FRAME_SIZE - 2 * CHUNK_SIZE - LINE_SIZE);

		assert(paranoia5[0] == 0xa5);
		assert(paranoia5[1] == 0x5a);

//		if (!checkFrame()) {
//			continue;
//		}

		write(socket_fd, recvFrame, FRAME_SIZE);
	}
}

