#ifndef _ASEF_H_
#define _ASEF_H_

#include <stdio.h>
#include "cv.h"

typedef struct{
	IplImage * input_image;
	CvRect face_rect;	
	CvPoint left_eye, right_eye;
	
	// Internal ASEF variables
	CvMat *lfilter, *rfilter;
	int n_rows, n_cols;
	CvRect lrect, rrect;
	CvMat *lfilter_dft, *rfilter_dft;

	CvMat face_image;
	CvMat *scaled_face_image_32fc1, *scaled_face_image_8uc1;
	
	CvMat *lcorr, *rcorr;
	CvMat *lroi, *rroi;
	CvMat *lut;

} AsefEyeLocator;

class EyeAsef {
public:
    EyeAsef(const char *felpath) {
        if (asef_initialze(&asef_, felpath))
            printf("Load the ASEF eye detect error! \n");
    }

    ~EyeAsef() {
        asef_destroy(&asef_);
    }

    int asef_initialze(AsefEyeLocator *asef, const char *asef_file_name);
    void asef_destroy(AsefEyeLocator *asef);
    
    void run(IplImage *gray_img, const CvRect faceRect) {
        asef_.input_image = gray_img;
        asef_.face_rect = faceRect;
        asef_locate_eyes(&asef_);
        draw_markers(gray_img, asef_.face_rect, asef_.left_eye, asef_.right_eye);
    }

private:
    AsefEyeLocator asef_;
    void asef_locate_eyes(AsefEyeLocator *asef);
    static int load_asef_filters(const char* file_name, int *p_n_rows,
        int *p_n_cols, CvRect *left_eye_region, CvRect *right_eye_region, 
        CvMat **p_left_filter, CvMat **p_right_filter);

    static int read_line(FILE *fp, char *buf, int size);
    void draw_markers(CvArr* img, CvRect rect, CvPoint pt1, CvPoint pt2) {
        CvScalar color = CV_RGB(255,0,0);

        cvCircle(img, pt1, 3, color, 3, CV_AA, 0);
        cvCircle(img, pt2, 3, color, 3, CV_AA, 0);	
        cvRectangle( img, cvPoint(rect.x,rect.y), 
            cvPoint(rect.x+rect.width, rect.y+ rect.height), color, 3, CV_AA, 0);
    }
};


#endif
