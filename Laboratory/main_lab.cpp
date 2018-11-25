#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>

//#include<../deps/getopt.h>

////////////////////////////////////////////////////////////////////////////////
int main_FBX(int argc, char** argv);
int main_templab();
int main_PhysX();
int main_Creator();
int main_voxel();
int main_gl();

////////////////////////////////////////////////////////////////////////////////
static void usage(void)
{
	printf("Usage: events [-f] [-h] [-n WINDOWS]\n");
	printf("Options:\n");
	printf("  -f use full screen\n");
	printf("  -h show this help\n");
	printf("  -n the number of windows to create\n");
}

int main(int argc, char** argv) {
//	int ch, count;
// 	while ((ch = getopt(argc, argv, "hfn:")) != -1)
// 	{
// 		switch (ch)
// 		{
// 		case 'h':
// 			usage();
// 			exit(0);
// 
// 		case 'f':
// 			usage();
// 			break;
// 
// 		case 'n':
// 			count = (int)strtol(optarg, nullptr, 10);
// 			break;
// 
// 		default:
// 			usage();
// 			exit(1);
// 		}
// 	}
// 	return main_FBX(argc, argv);
// 	return main_templab();
// 	return main_PhysX();
// 	return main_Creator();
// 	return main_opengl_lab();
//	return main_gl();
	return main_voxel();
}
