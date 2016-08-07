/*
 * NBodyFileHeader.h
 *
 *  Created on: 7 Aug 2016
 *      Author: vincent
 */

#ifndef NBODYFILEHEADER_H_
#define NBODYFILEHEADER_H_

#include <stdint.h>

class NBodyFileHeader
{

public:

	uint64_t totalByteFileSize; //Estimated total size of the file in bytes (including the whole header)
	uint64_t numberOfFrames;
	uint64_t numberOfParticles;
	float timeDelta;
	float mass;

	NBodyFileHeader();
	~NBodyFileHeader();
};


#endif /* NBODYFILEHEADER_H_ */
