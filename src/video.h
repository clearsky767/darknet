#ifndef VIDEO_H
#define VIDEO_H

#ifdef OPENCV

void init_statistics_detections(detection_good* goods, char **names, int classes);
void add_statistics_detections(detection_good* goods,detection *dets, int num, float thresh, char **names, int classes);
void video_detector(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int delay, char *prefix, int avg_frames, float hier, int w, int h, int frames, int fullscreen,int isShowWnd, detection_video* video);

#endif

#endif
