/*
 * CircularBuffer.c
 *
 *  Created on: Dec 15, 2012
 *      Author: jon
 */

#include "CircularBuffer.h"

int CircularBufferWrite(CircularBuffer *c, unsigned char *dataIn, unsigned int length){
	if (c->size >= (c->length + length)){
		// buffer won't overflow

		if ((c->head + length >= c->size)){
			// rollover
			int bytesToLast = c->size - c->head;
			int bytesRemainder = length - bytesToLast;

			memcpy(c->buffer+c->head, dataIn, bytesToLast);
			memcpy(c->buffer, dataIn+bytesToLast, bytesRemainder);

			c->head = bytesRemainder;
			c->length += length;
		}
		else {
			// no rollover
			memcpy(c->buffer+c->head, dataIn, length);
			c->head += length;
			c->length += length;
		}

		return length;
	}
	else {
		// buffer would overflow

		return 0; // TODO: could just write whatever is left of the buffer and return that
	}
}

int CircularBufferRead(CircularBuffer *c, unsigned char *dataOut, unsigned int length){

	if (c->length >= length){
		if ((c->tail + length >= c->size)){
			// rollover
			int bytesToLast = c->size - c->tail;
			int bytesRemainder = length - bytesToLast;

			memcpy(dataOut, c->buffer+c->tail, bytesToLast);
			memcpy(dataOut+bytesToLast, c->buffer, bytesRemainder);

			c->tail = bytesRemainder;
			c->length -= length;
		}
		else {
			// no rollover
			memcpy(dataOut, c->buffer+c->tail, length);
			c->tail += length;
			c->length -= length;
		}

		return length;
	}
	else {
		// want to read more bytes than there are
		return 0;
	}
}
