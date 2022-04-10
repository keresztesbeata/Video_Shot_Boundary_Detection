#include "stdafx.h"
#include "CutDetectionAlgorithms.h"

const char videoFilePath[] = "Videos/Megamind.avi";
const char outputDirPath[] = "Videos/keyFrames/Megamind/key_frame_%d.jpg";

int main() {
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		cout<<"Menu:"<<endl;
		cout<<" 1 - Cut detection using PBA and custom threshold"<<endl;
		cout << " 0 - Exit"<<endl;
		cout<<"Option: "<<endl;
		cin >> op;
		switch (op)
		{
		case 1: {
			float threshold;
			cout<<"threshold = ";
			cin >> threshold;

			vector<Mat> frames = pixelBasedApproach(videoFilePath, threshold);
			int index = 0;
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFileName[MAX_PATH];
				sprintf(outputFileName, outputDirPath, index);
				index++;

				// save the current frame
				imwrite(outputFileName, f);

				/*
				// print the current frame
				imshow("keyFrame = ", f);
				char c = waitKey(0);  // waits a key press to advance to the next frame
				if (c == 27) {
					// press ESC to exit
					printf("ESC pressed - capture finished\n");
					break;  //ESC pressed
				};
				*/
			}
			waitKey(0);
			break;
		}
		default:break;
		}
	} while (op != 0);
	return 0;
}
