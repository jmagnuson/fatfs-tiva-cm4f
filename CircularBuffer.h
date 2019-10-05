/*
 * CircularBuffer.h
 *
 *  Created on: Nov 2, 2012
 *      Author: jon
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

typedef struct CircularBuffer {
	unsigned char* buffer;
	int size;
	int head;
	int tail;
	int length;
} CircularBuffer;

int CircularBufferWrite(CircularBuffer *c, unsigned char *dataIn, unsigned int length);
int CircularBufferRead(CircularBuffer *c, unsigned char *dataOut, unsigned int length);

#endif /* CIRCULARBUFFER_H_ */
