#include "types.h"


#include "decoder.h"

int main(int argc, char **argv) {
	
	if (argc < 2) {
		printf("Need to specify a command switch!\n");
		return 1;
	}
	if (strcmp(argv[1],"-d") == 0) {
		if (argc < 3) {
			printf("You need to specify a .mp3 input file.\n");
			return 1;
		} else {
			MadDecoder decoder;
			decoder.decode(argv[2]);
			decoder.decode(argv[3]);
			return 0;
		}
	}

	printf("Did not match any of the options.\n");
	return 1;
}


