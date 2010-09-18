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

int decode(unsigned char const *start, unsigned long length) {
	mad_buffer buf;
	
	buf.s = start;
	buf.len = length;
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
	
	decode((unsigned char*)fdm, stat.st_size);
	
	fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fileno(fi), 0);
	if (fdm == MAP_FAILED) {
		printf("Could not map file to memory.\n");
		return 3;
	}
	
	
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


