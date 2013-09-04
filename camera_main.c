#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>

#include <cv.h>
#include <highgui.h>

#include "asef.h"
#include "utils.h"

#define WINDOW_TITLE "ASEF"

char *haar_cascade_path = NULL;
char *asef_locator_path = NULL;
AsefEyeLocator asef;
CvCapture* capture;
CvHaarClassifierCascade* cascade;
char window_title[] = WINDOW_TITLE;
IplImage *color_img, *gray_img;


void termination_handler(int signum){
	printf("\n");
	free(haar_cascade_path);
	free(asef_locator_path);
	cvDestroyWindow(window_title);
	asef_destroy(&asef);
	cvReleaseCapture( &capture);
	cvReleaseImage( &gray_img );
	cvReleaseImage( &color_img );
	exit(EXIT_SUCCESS);
}

int main( int argc, char** argv )
{

	if (parse_arguments(argc, argv, &haar_cascade_path, &asef_locator_path))
		return -1;

	struct sigaction act;
	act.sa_handler = termination_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	int frame_width, frame_height;

	if ( asef_initialze(&asef, asef_locator_path, haar_cascade_path) )
		return -1;

	if( !(capture = cvCaptureFromCAM(-1)) ) {
		fprintf( stderr, "ERROR: capture is NULL \n" );
		return -1;
	}

	get_camera_properties(capture, &frame_width, &frame_height, NULL);

	CvSize frame_size = cvSize(frame_width, frame_height);

	IplImage* frame = NULL;
	color_img = cvCreateImage( frame_size, IPL_DEPTH_8U, 3 );
	gray_img = cvCreateImage( frame_size, IPL_DEPTH_8U, 1);

	cvNamedWindow(window_title, 0);
	while (1) {
		char key = cvWaitKey(1);

		if (key == 27) {
			break;
		}

		printf("\r"); fflush(stdout);
		printf("FPS: %f  ", calculate_fps());

		frame = cvQueryFrame(capture);
		cvCopyImage(frame, color_img);
		cvCvtColor(color_img, gray_img, CV_RGB2GRAY);
		asef.input_image = gray_img;

		if( asef_detect_face(&asef) ){
			asef_locate_eyes(&asef);
			draw_markers(color_img, asef.face_rect, asef.left_eye, asef.right_eye);
		}

    // Mirroring results. assuming you're using a frontal facing camera
		cvFlip(color_img, color_img, 1);
		cvShowImage(window_title, color_img);
	}
	return 0;
}
