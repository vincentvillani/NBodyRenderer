/*
 * NBodyFileInput.cpp
 *
 *  Created on: 7 Aug 2016
 *      Author: vincent
 */

#include "NBodyFileInput.h"



NBodyFileHeader* FileInputReadHeader(FILE* inputFile)
{
	NBodyFileHeader* fileHeader = new NBodyFileHeader();
	//uint64_t fileHeaderByteSize = sizeof(uint64_t) * 3 + sizeof(float) * 2;

	//Go to the start of the file and read the header in
	fseek(inputFile, 0, SEEK_SET);
	fread(&(fileHeader->totalByteFileSize), sizeof(uint64_t), 1, inputFile);
	fread(&(fileHeader->numberOfFrames), sizeof(uint64_t), 1, inputFile);
	fread(&(fileHeader->numberOfParticles), sizeof(uint64_t), 1, inputFile);
	fread(&(fileHeader->timeDelta), sizeof(float), 1, inputFile);
	fread(&(fileHeader->mass), sizeof(float), 1, inputFile);

	return fileHeader;
}


void FileInputReadFrame(NBodyFileHeader* fileHeader, uint64_t frameNumber, NBodyFrameBuffer* frameBuffer, FILE* inputFile)
{
	//Seek to this frames position
	uint64_t fileHeaderByteSize = sizeof(uint64_t) * 3 + sizeof(float) * 2;
	uint64_t frameOffsetByteSize = frameNumber * sizeof(uint64_t) + frameNumber * fileHeader->numberOfParticles * 3 * sizeof(float);
	fseek(inputFile, fileHeaderByteSize + frameOffsetByteSize, SEEK_SET);

	//Read in the frames data
	fread(&(frameBuffer->frameNumber), sizeof(uint64_t), 1, inputFile);
	fread(frameBuffer->positions, fileHeader->numberOfParticles * 3 * sizeof(float), 1, inputFile);
}
