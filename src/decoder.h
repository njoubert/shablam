#include "types.h"

#include <mad.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <memory>

// class Decoder {
// public:
// 	Decoder();
// 	virtual ~Decoder();
// 	virtual bool isMono();	
// 	virtual sample_list* getleft();
// 	virtual sample_list* getright(); //returns null if mono
// };

class MadDecoder  {
public:
	MadDecoder();
	virtual ~MadDecoder();

	//return 0 if successful;
	int decode(char* filename);
	bool isMono();	
	std::auto_ptr<sample_list> getleft();
	std::auto_ptr<sample_list> getright(); //returns null if mono

private:
	bool holds_data;
	int l_samplerate;
	int l_nchannels;
	int l_nsamples;
	std::auto_ptr<sample_list> l_left;
	std::auto_ptr<sample_list> l_right;

	void reset();
	int run_decode(unsigned char const *start, unsigned long length);
};