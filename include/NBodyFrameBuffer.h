/*
 * NBodyFrameBuffer.h
 *
 *  Created on: 7 Aug 2016
 *      Author: vincent
 */

#ifndef NBODYFRAMEBUFFER_H_
#define NBODYFRAMEBUFFER_H_

#include <stdint.h>

class NBodyFrameBuffer
{

public:

	uint64_t frameNumber;
	uint64_t particleNumber;
	float* positions;

	NBodyFrameBuffer(uint64_t frameNumber, uint64_t particleNum);
	~NBodyFrameBuffer();

};


#endif /* NBODYFRAMEBUFFER_H_ */
