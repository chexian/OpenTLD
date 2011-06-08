#include <LKTracker.h>
using namespace cv;

LKTracker::LKTracker(){
  term_criteria = TermCriteria( TermCriteria::COUNT+TermCriteria::EPS, 20, 0.03);
  window_size = Size(5,5);
  level = 5;
  lambda = 0.5;
}

void LKTracker::trackf2f(const Mat& img1, const Mat& img2, vector<Point2f> &points1, vector<cv::Point2f> &points2){
  //Forward-Backward tracking
  calcOpticalFlowPyrLK( img1,img2, points1, points2, status,sim_error, window_size, level, term_criteria, lambda, 0);
  calcOpticalFlowPyrLK( img2,img1, points2, pointsFB, FB_status,FB_error, window_size, level, term_criteria, lambda, 0);
  //Compute the real FB-error
  for( int i= 0; i<points1.size(); ++i ){
        FB_error[i] = norm(pointsFB[i]-points1[i]);
  }
  //Filter out points with FB_error[i] > median(FB_error) && points with sim_error[i] > median(sim_error)
  filterPts(points2);
}

void LKTracker::filterPts(vector<Point2f>& points2){
  //Get Error Medians
  float simmed = median(sim_error);
  float fbmed = median(FB_error);
  printf("Forward-Backward error median = %f\nSimilarity error median = %f\n",fbmed,simmed);
  size_t i, k;
  for( i=k = 0; i<points2.size(); ++i ){
      //TODO: Evaluate if it would be better to only get the median of the tracked points
      if( !status[i] )
        continue;
      if(FB_error[i] <= fbmed && sim_error[i]<= simmed)
        points2[k++] = points2[i];
  }
  points2.resize(k);
}

/*
 * old OpenCV style
void LKTracker::init(Mat img0, vector<Point2f> &points){
  //Preallocate
  //pyr1 = cvCreateImage(Size(img1.width+8,img1.height/3),IPL_DEPTH_32F,1);
  //pyr2 = cvCreateImage(Size(img1.width+8,img1.height/3),IPL_DEPTH_32F,1);
  //const int NUM_PTS = points.size();
  //status = new char[NUM_PTS];
  //track_error = new float[NUM_PTS];
  //FB_error = new float[NUM_PTS];
}


void LKTracker::trackf2f(..){
  cvCalcOpticalFlowPyrLK( &img1, &img2, pyr1, pyr1, points1, points2, points1.size(), window_size, level, status, track_error, term_criteria, CV_LKFLOW_INITIAL_GUESSES);
  cvCalcOpticalFlowPyrLK( &img2, &img1, pyr2, pyr1, points2, pointsFB, points2.size(),window_size, level, 0, 0, term_criteria, CV_LKFLOW_INITIAL_GUESSES | CV_LKFLOW_PYR_A_READY | CV_LKFLOW_PYR_B_READY );
}
*/

