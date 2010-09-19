#include "decoder.h"


struct mad_buffer {
	unsigned char const *s;
	unsigned long len;
};

static int samplerate;
static int nchannels;
static int nsamples;
static sample_list* left;
static sample_list* right;


MadDecoder::MadDecoder() {  reset(); }
MadDecoder::~MadDecoder() { reset(); }

void MadDecoder::reset() {
	samplerate = 0;
	nchannels = 0;
	nsamples = 0;
	left = NULL;
	right = NULL;
	
	l_samplerate = 0;
	l_nchannels = 0;
	l_nsamples = 0;
	holds_data = false;
}
bool MadDecoder::isMono() {
	if (holds_data) {
		return (l_nchannels == 1);
	}
	return false;
}

std::auto_ptr<sample_list> MadDecoder::getleft() {
	return l_left;
}

std::auto_ptr<sample_list> MadDecoder::getright() {
	return l_right;
}

int MadDecoder::decode(char *inputFile) {
	reset();
	
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
	
	l_left = std::auto_ptr<sample_list>(new std::vector<sample>());
	l_right = std::auto_ptr<sample_list>(new std::vector<sample>());
	
	left = l_left.get();
	right = l_right.get();
	int result = run_decode((unsigned char*)fdm, stat.st_size);
	l_samplerate = samplerate;
	l_nchannels = nchannels;
	l_nsamples = nsamples;
			
	if (munmap(fdm, stat.st_size) == -1) {
		printf("Could not unmap file's memory.\n");
		return 4;
	}
	if (result == 0)
		holds_data = true;
	return result;
}

/* 
callback function for populating MAD decoder with data. 
Our function just makes it buffer the whole memory mapped file in one go.
*/
static enum mad_flow input(void *data, struct mad_stream *stream) {
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
static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm) {	
	unsigned int pcm_nchannels, pcm_nsamples;
	mad_fixed_t const *left_ch, *right_ch;

	/* pcm->samplerate contains the sampling frequency */

	pcm_nchannels  = pcm->channels;
	pcm_nsamples   = pcm->length;
	left_ch    = pcm->samples[0];
	right_ch   = pcm->samples[1];
	
	nsamples += pcm_nsamples;
	nchannels = pcm_nchannels;
	samplerate = pcm->samplerate;
	
	while (pcm_nsamples--) {
		signed int sample;
		/* output sample(s) in 16-bit signed little-endian PCM */
	
		sample = (*left_ch++);
		left->push_back(sample);
		if (pcm_nchannels == 2) {
			sample = (*right_ch++);
			right->push_back(sample);
		}
	}
	return MAD_FLOW_CONTINUE;
}
/*
	callback function for when an error occusr in the decoding process.
*/
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame) {
	mad_buffer *buffer = (mad_buffer*) data;

	fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %ld\n",
	  stream->error, mad_stream_errorstr(stream),
	  stream->this_frame - buffer->s);

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}

int MadDecoder::run_decode(unsigned char const *start, unsigned long length) {
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

	// for (unsigned int i = 0; i  < length-4; i += 4) {
	// 	printf("%8x\n", *((unsigned int *) (start + i)));
	// }

	result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
	
	mad_decoder_finish(&decoder);

	printf("  Size of left buffer: %ld samples\n", left->size());
	printf("  Size of right buffer: %ld samples\n", right->size());
	printf("  Samplerate: %dhz\n", samplerate);
	printf("  Length: %fs\n", left->size() / (double)samplerate);

	return result;
}