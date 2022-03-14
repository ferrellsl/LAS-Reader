
//This program reads a compressed LAZ on umcompressed LAS file and extracts the ground points based on classification.
//Points are written to text file

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "lasreader.hpp"


#if defined(_MSC_VER) && \
    (_MSC_FULL_VER >= 150000000)
#define LASCopyString _strdup
#else
#define LASCopyString strdup
#endif


FILE *myfileout; //our output file

I64 p_count;
F64 **LASData; //array to hold our input points
F64 **OUTData; //array to hold our ground points
I64 ground_points; //total number of ground points
F64 myx_scale, myy_scale, myz_scale, myx_offset, myy_offset, myz_offset;


//function to create a 2D array of floats dynamically
int Alloc2dGrid(F64 ***f, I64 nx, I64 ny)
{
	I64 i;

	*f = (F64 **)calloc(nx, sizeof(F64));
	for (i = 0; i < nx; i++) {

		(*f)[i] = (F64 *)calloc(ny, sizeof(F64));
	}
	return(1);
}



int main(int argc, char *argv[])
{ 
	int i;

	LASreader * lasreader;
	LASreadOpener lasreadopener;

	lasreadopener.set_file_name(*++argv);
	lasreader = lasreadopener.open();
	if (!lasreader) {
		printf("Could not open lidar file.\n");
		printf("Usage: las_ground_reader.exe input.laz output.txt\n");
		return NULL;
	}
//read LAS header and get number of points, offsets, and scale factors
	myx_offset = lasreader->header.x_offset; //get x offset
	myy_offset = lasreader->header.y_offset; //get y offset
	myz_offset = lasreader->header.z_offset; //get z offset
	myx_scale = lasreader->header.x_scale_factor; //get x scale
	myy_scale = lasreader->header.y_scale_factor; //get y scale
	myz_scale = lasreader->header.z_scale_factor; //get z scale

	p_count = lasreader->npoints;  //get total points from header
	printf("Total points = %lld\n", p_count);

	printf("Offsets %lf %lf %lf\n", myx_offset, myy_offset, myz_offset);
	printf("Scale factors: %lf %lf %lf\n", myx_scale, myy_scale, myz_scale);
	myfileout = fopen(*++argv, "w"); //open our output file for writing
	
	LASpoint laspoint;

	Alloc2dGrid(&LASData, p_count+1, 3);//initialize an array to store the input points
										
	while (lasreader->read_point()) // loop over points where there is a point to read
	{
			if (lasreader->point.classification == 2) { //if class = 2 (ground points) then put them into an array for later output
													//entire file could possibly be ground points
			LASData[ground_points][1] = lasreader->point.get_x();
			LASData[ground_points][2] = lasreader->point.get_y();
			LASData[ground_points][3] = lasreader->point.get_z();

			ground_points++; //count the number of ground points
		}
	}
	

	Alloc2dGrid(&OUTData, ground_points + 1, 3);  //initialize our output array making it large enough to hold our ground points
	for (i = 0; i < ground_points; i++) { //stuff our ground points into the output array and send them to our output file
		OUTData[i][1] = LASData[i][1];
		OUTData[i][2] = LASData[i][2];
		OUTData[i][3] = LASData[i][3];
		fprintf(myfileout, "%.2llf\t%.2llf\t%.2llf\n", OUTData[i][1], OUTData[i][2], OUTData[i][3]);
	}

		printf("Done\n");
	//close everything and release our memory
		fclose(myfileout);
		free(&LASData);
		free(&OUTData);
		lasreader->close();
		delete lasreader;


  return 0;
} //end main
