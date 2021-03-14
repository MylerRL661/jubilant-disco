#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>

class Orig_Image
{
public:
	double *image;
	// Reads .txt file representing an image of R rows and C Columns stored in filename 
	// and converts it to a 1D array of doubles of size R*C
	// Memory allocation is performed inside readTXT

	void Read_Text(std::string fileName, int sizeC, int sizeR)
	{
		image = new double[sizeR*sizeC];
		int i = 0;
		std::ifstream myfile(fileName);
		if (myfile.is_open())
		{

			while (myfile.good())
			{
				if (i > sizeR*sizeC - 1) break;
				myfile >> *(image + i);
				//std::cout << *(image+i) << ' '; // This line display the converted data on the screen, you may comment it out. 
				i++;
			}
			myfile.close();
		}

		else std::cout << "Unable to open file";
		//cout << i;
	}
};

//class to reference the image
class Reference_Image : public Orig_Image
{

};
//class for the expanded image
class Expanded_Image : public Orig_Image
{
public:
	int checkImage(Reference_Image Wally)
	{
		//best location of starting pixel
		int BestLocation = 0; 
		//Value of the SSD that has made it the best match
		double BestSquareSumOfDifferences = 100000000000000;

		for (int row = 0; row < 768; row++)
		{
			for (int column = 0; column < 1024; column++)
			{

				if (768 - row < 49 || 1024 - column < 36)
					continue;

				//This will go through every pixel in the cluttered scene.
				//You can access the pixel by doing the column + (row * number of columns)

				int PixelLocation = column + row * 1024;
				int SumOfDifferences = 0;

				//Double for loop going through reference image...
				for (int referenceRow = 0; referenceRow < 49; referenceRow++)
				{
					for (int referenceColumn = 0; referenceColumn < 36; referenceColumn++)
					{

						//PixelLocation (when going through expanded image)
						//Being equal to (0, 0) in the reference image.

						int referencePixel = referenceColumn + referenceRow * 36;

						//ignores white background of the reference image
						if (Wally.image[referencePixel] == 255)
							continue;

						int big_pixel = (referenceRow * 1024 + referenceColumn) + PixelLocation;
						
						//Difference between the 2 images
						double Difference = image[big_pixel] - Wally.image[referencePixel];

						SumOfDifferences += Difference * Difference;
					}
				}

				if (SumOfDifferences < BestSquareSumOfDifferences)
				{
					BestSquareSumOfDifferences = SumOfDifferences;
					BestLocation = PixelLocation;
				}
			}
		}
		return BestLocation;
	}

	//ColourShader to mark Wallys location
	void ColourShader(int Location) 
	{
		//loops to display box around Wally
		for (int referenceRow = 0; referenceRow < 49; referenceRow++)
		{
			for (int referenceColumn = 0; referenceColumn < 36; referenceColumn++)
			{
				int Pixel = Location + referenceRow * 1024 + referenceColumn;
				//colour of the shader = dark grey
				image[Pixel] = 45;
			}
		}
	}




	// Converts a 1D array of doubles of size R*C to .pgm image of R rows and C Columns and stores .pgm in filename
	//.pgm filename found in the source files
	// Use P = 255 for greyscale images and 1 for binary images.
	void Write_Program(const char *filename, int sizeR, int sizeC, int P)
	{

		int i, j;
		unsigned char *Picture;
		std::ofstream myfile;

		Picture = (unsigned char *) new unsigned char[sizeR*sizeC];

		// convert the integer values to unsigned char

		for (i = 0; i < sizeR*sizeC; i++)
			Picture[i] = (unsigned char)image[i];

		myfile.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

		if (!myfile) {
			std::cout << "Can't open file: " << filename << std::endl;
			exit(1);
		}

		myfile << "P5" << std::endl;
		myfile << sizeC << " " << sizeR << std::endl;
		myfile << P << std::endl;

		myfile.write(reinterpret_cast<char *>(Picture), (sizeR*sizeC) * sizeof(unsigned char));

		if (myfile.fail()) {
			std::cout << "Can't write image " << filename << std::endl;
			exit(0);
		}

		myfile.close();

		delete[] Picture;

	}

};

//main function for the Where's Wally Assignment
//functions plugged from other classes
int main()
{
	Reference_Image Wally_image;
	Wally_image.Read_Text("Wally_grey.txt", 36, 49);

	Expanded_Image Cluttered_Image;
	Cluttered_Image.Read_Text("Cluttered_scene.txt", 1024, 768);

	int Location = Cluttered_Image.checkImage(Wally_image);

	Cluttered_Image.ColourShader(Location);
	Cluttered_Image.Write_Program("ProgramOutput.pgm", 768, 1024, 255);

	system("pause");
};