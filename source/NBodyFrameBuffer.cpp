/*
 * NBodyFrameBuffer.cpp
 *
 *  Created on: 7 Aug 2016
 *      Author: vincent
 */

#include "NBodyFrameBuffer.h"

#include <new>
#include <stdlib.h>
#include <stdio.h>


NBodyFrameBuffer::NBodyFrameBuffer(uint64_t frameNumber, uint64_t particleNum)
{
	this->frameNumber = frameNumber;
	this->particleNumber = particleNum;

	this->positions = new (std::nothrow) float[particleNum * 3];

	if(this->positions == NULL)
	{
		fprintf(stderr, "NBodyFrameBuffer::NBodyFrameBuffer: Unable to allocate memory for the frame buffer\n");
		exit(1);
	}
}



NBodyFrameBuffer::~NBodyFrameBuffer()
{
	if(positions != NULL)
		delete[] positions;
}

