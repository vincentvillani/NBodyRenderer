/*
 * NBodyFileInput.h
 *
 *  Created on: 7 Aug 2016
 *      Author: vincent
 */

#ifndef NBODYFILEINPUT_H_
#define NBODYFILEINPUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "NBodyFileHeader.h"
#include "NBodyFrameBuffer.h"

NBodyFileHeader* FileInputReadHeader(FILE* inputFile);
void FileInputReadFrame(NBodyFileHeader* fileHeader, uint64_t frameNumber, NBodyFrameBuffer* frameBuffer, FILE* inputFile);

#endif /* NBODYFILEINPUT_H_ */
