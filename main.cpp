#include <mad.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

struct mad_buffer {
  unsigned char const *s;
  unsigned long len;
};

/* 
	callback function for populating MAD decoder with data. 
	Our function just makes it buffer the whole memory mapped file in one go.
*/
enum mad_flow input(void *data, struct mad_stream *stream) {
	mad_buffer *buffer = (mad_buffer*) data;
	
	if (!buffer->len)
		return MAD_FLOW_STOP;
		
	mad_stream_buffer(stream, buffer->s, buffer->len);
	buffer->len = 0;
	return MAD_FLOW_CONTINUE;
}

/*
	callback function for processing individual PCM frames
*/
enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm) {
	
  // unsigned int nchannels, nsamples;
  // mad_fixed_t const *left_ch, *right_ch;
  // 
  // /* pcm->samplerate contains the sampling frequency */
  // 
  // nchannels = pcm->channels;
  // nsamples  = pcm->length;
  // left_ch   = pcm->samples[0];
  // right_ch  = pcm->samples[1];
  // 
  // while (nsamples--) {
  //   signed int sample;
  // 
  //   /* output sample(s) in 16-bit signed little-endian PCM */
  // 
  //   //sample = scale(*left_ch++);
  //   putchar((sample >> 0) & 0xff);
  //   putchar((sample >> 8) & 0xff);
  // 
  //   if (nchannels == 2) {
  //     //sample = scale(*right_ch++);
  //     putchar((sample >> 0) & 0xff);
  //     putchar((sample >> 8) & 0xff);
  //   }
  // }

  return MAD_FLOW_CONTINUE;
}
/*
	callback function for when an error occusr in the decoding process.
*/
enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame) {
	mad_buffer *buffer = (mad_buffer*) data;
	
	fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
	  stream->error, mad_stream_errorstr(stream),
	  stream->this_frame - buffer->s);
	
	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}

int decode(unsigned char const *start, unsigned long length) {
	mad_buffer buf;
	mad_decoder decoder;
	int result;
	
	buf.s = start;
	buf.len = length;
	
	mad_decoder_init(&decoder, 
		&buf,
		input, 
		0 /* header */, 
		0 /* filter */, 
		output,
		error, 
		0 /* message */
	);
	
	for (unsigned int i = 0; i  < length-4; i += 4) {
		printf("%x\n", *((unsigned int *) (start + i)));
	}
	
	//result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
	
	mad_decoder_finish(&decoder);
	
	return result;
}

int optDecode(int argc, char **argv) {
	if (argc != 3) {
		printf("You need to specify a .mp3 input file.\n");
		return 1;
	}
	char* inputFile = argv[2];
	void *fdm;
	
	FILE* fi = fopen(inputFile, "r");
	struct stat stat;
	if (fi == NULL || fstat(fileno(fi), &stat) == -1 || stat.st_size == 0) {
		printf("%s does not exist or is empty.\n", inputFile);
		return 2;
	}
	
	printf("Processing %s... [%ld bytes]\n", inputFile, (long) stat.st_size);
		
	fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fileno(fi), 0);
	if (fdm == MAP_FAILED) {
		printf("Could not map file to memory.\n");
		return 3;
	}
		
	decode((unsigned char*)fdm, stat.st_size);

	if (munmap(fdm, stat.st_size) == -1) {
		printf("Could not unmap file's memory.\n");
		return 4;
	}
	return 0;
}


int main(int argc, char **argv) {
	
	if (argc < 2) {
		printf("Need to specify a command switch!\n");
		return 1;
	}
	if (strcmp(argv[1],"-d") == 0) {
		return optDecode(argc, argv);
	}
	printf("Did not match any of the options.\n");
	return 1;
}


