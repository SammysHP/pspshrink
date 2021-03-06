#include <iostream>
#include <iomanip>
#include <string>
#include <zlib.h>

#include "ciso.h"

using namespace std;

bool stop = false;

void* displayInfo(void* compressor);

int main(int argc, char *argv[])
{
	int level;
	int result;
	string filenameIn;
	string filenameOut;

	if (argc != 4)
	{
		cout << "Usage: ciso level infile outfile" << endl;
		cout << "  level: 1-9 compress ISO to CSO (1=large/fast - 9=small/slow" << endl;
		cout << "         0   decompress CSO to ISO" << endl;
		return 0;
	}
	
	level = argv[1][0] - '0';
	if (level < 0 || level > 9)
	{
        cerr << "Unknown mode: " << argv[1][0] << endl;
		return 1;
	}

	filenameIn = argv[2];
	filenameOut = argv[3];
	
	pthread_t thread;
	CIso isoCompressor;

    try
    {
    	pthread_create(&thread, NULL, displayInfo, &isoCompressor);
    	
    	if (level==0)
    	{
    		isoCompressor.decompress(filenameIn, filenameOut);
    		pthread_join(thread, NULL);
    		cout << "ciso decompression completed" << endl;
        }
    	else
    	{
    		isoCompressor.compress(filenameIn, filenameOut, level);
    		pthread_join(thread, NULL);
    		cout << "ciso compression completed" << endl;
    	}
    }
    catch (CIsoException& e)
    {
        cout << "Error: " << e.what() << endl;
    }

	return 0;
}

void* displayInfo(void* compressor)
{
	CIso* isoCompressor = (CIso*) compressor;
	
	cout << "Progress:    %";
	
	while(!stop)
	{
		int progress = isoCompressor->getProgress();
		int compressionRate = isoCompressor->getCompressionRate();
		
		cout << "\b\b\b\b" << setw(3) << progress << "%" << flush;
	
		if(progress == 100)
		{
			cout << endl;
			if(compressionRate != 0)
			{
			cout << "CompressionRate: " << compressionRate << "%" << endl;
			}

			return NULL;
		}
		else
		{
			usleep(500);
		}
	}
	
	return NULL;
}
