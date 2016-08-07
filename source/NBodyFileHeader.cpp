/*
 * NBodyFileHeader.cpp
 *
 *  Created on: 7 Aug 2016
 *      Author: vincent
 */

#include "NBodyFileHeader.h"


NBodyFileHeader::NBodyFileHeader()
{
	totalByteFileSize = 0;
	numberOfFrames = 0;
	numberOfParticles = 0;
	timeDelta = 0;
	mass = 0;
}


NBodyFileHeader::~NBodyFileHeader()
{

}


