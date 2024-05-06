#pragma once
#include <SD.h>
#include "spi_interrupt.h"
#include <Audio.h>

class RandomAccesSDWAVLoader
{
	public:
		RandomAccesSDWAVLoader(const char* path)
		{
			AudioNoInterrupts();
			SD.begin(10);
			if (!SD.exists(path)) Serial.println("pathDoesntExist");
			folder = SD.open(path);
			AudioInterrupts();
			while (true)
			{
				if (!folder.openNextFile()) break;
				numberOfFilesInFolder++;
			}
			folder.seek(0);
		}

		uint32_t loadFile(float relativeIndex, int16_t* samples, uint32_t maxLength)
		{
			relativeIndex = constrain(relativeIndex, 0.0f, 0.999f);

			uint16_t targetFilePosition = relativeIndex * numberOfFilesInFolder;

			while (currentFilePosition != targetFilePosition)
			{
				currentFilePosition++;
				if (currentFilePosition > numberOfFilesInFolder)
				{
					currentFilePosition = 0;
					folder.seek(0);
				}
				currentFile = folder.openNextFile();
			}

			currentFile.seek(44);

			uint32_t i = 0;
			while (currentFile.available() && i < maxLength)
			{
				samples[i] = int16_t(currentFile.read() | currentFile.read() << 8);
				i++;
			}
			return i;
		}

		uint32_t loadNextFile(int16_t* samples, uint32_t maxLength)
		{
			currentFilePosition++;
			if (currentFilePosition > numberOfFilesInFolder)
			{
				currentFilePosition = 0;
				folder.seek(0);
			}
			currentFile = folder.openNextFile();

			currentFile.seek(44);

			uint32_t i = 0;
			while (currentFile.available() && i < maxLength)
			{
				samples[i] = int16_t(currentFile.read() | currentFile.read() << 8);
				i++;
			}
			return i;
		}

		String getFileNameData(const char* markers)
		{
			String filename = currentFile.name();
			uint16_t first = filename.indexOf(markers[0]);
			uint16_t last = filename.indexOf(markers[1]);
			return filename.substring(first, last - first);
		}

	private:
		int16_t currentFilePosition = -1;
		int16_t numberOfFilesInFolder = -1;
		File folder = NULL;
		File currentFile = NULL;

};