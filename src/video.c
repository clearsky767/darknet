#include "network.h"
#include "image.h"
#include "video.h"
#include <sys/time.h>

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "opencv2/videoio/videoio_c.h"

#define DEMO 1

#define OPENCV 1

#ifdef OPENCV

void init_statistics_detections(detection_good* goods, char **names, int classes)
{
    int j;
    for(j = 0; j < classes; ++j){
        goods[j].name = names[j];
    }
    return;
}

void add_statistics_detections(detection_good* goods,detection *dets, int num, float thresh, char **names, int classes)
{
    int i,j;
    int *max = calloc(classes,sizeof(int));
    memset(max, 0, classes*sizeof(int));
    
    for(i = 0; i < num; ++i){
        float prob = 0;
        int class = -1;
       
        for(j = 0; j < classes; ++j){
            if (dets[i].prob[j] > thresh){
                float tempprob = dets[i].prob[j]*100;
                printf("%s: %.0f%%\n", names[j], tempprob);
                if(tempprob > prob)
                {
                    prob = tempprob;
                    class = j;
                }
            }
        }
        if(prob > 0)
        {
            max[class]++;
            goods[class].frame_count++;
            if(max[class]>goods[class].max)
            {
                goods[class].max = max[class];
            }
        }
    }
    free(max);
    return;
}

void video_detector(char *cfgfile, char *weightfile, float thresh, const char *filename, char **names, int classes,int isShowWnd, detection_video* video)
{
    printf("in video_detector func!\n");
    image **alphabet = load_alphabet();
    //load and set net
    network *net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    
    //cv load video file
    printf("load video file: %s\n", filename);
    CvCapture *cap = cvCaptureFromFile(filename);

    if(1 == isShowWnd)
    {
        cvNamedWindow("Demo", CV_WINDOW_AUTOSIZE);
    }

    //frame detect
    image img = get_image_from_stream(cap);
    image resizedimg = letterbox_image(img, net->w, net->h);

    layer l = net->layers[net->n-1];
    double start_time = what_time_is_it_now();
    double frame_time = start_time;
    int frame_num = 1;

    while(1)
    {
        int status = fill_image_from_stream(cap, img);
        if(status == 0) break;
        frame_num++;
        resizedimg = letterbox_image(img, net->w, net->h);
        float *X = resizedimg.data;
        network_predict(net, X);
        int nboxes = 0;
        detection *dets = get_network_boxes(net, img.w, img.h, thresh, thresh, 0, 1, &nboxes);
        float nms = .4;
        if (nms > 0) do_nms_obj(dets, nboxes, l.classes, nms);
        if(1 == isShowWnd){
            draw_detections(img, dets, nboxes, thresh, names, alphabet, classes);
        }
        add_statistics_detections(video->goods, dets, nboxes, thresh, names, classes);
        video->frame_count++;
        free_detections(dets, nboxes);
        //show fps
        float fps = 1./(what_time_is_it_now() - frame_time);
        printf("\nframe %d FPS:%.1f\n",frame_num,fps);
        frame_time=what_time_is_it_now();
        //show img to window
        if(1 == isShowWnd){
            show_image_cv2(img,"Demo");
        }
        // wait for frame
        char c = cvWaitKey(1);
        if(1 == isShowWnd)
        {
            if (-1 != c)
                c = c%256;
            if (27 == c) {
                break;
            }
        }
    }
    printf("%s: Predicted video in %f seconds.\n", filename, what_time_is_it_now()-start_time);
    free_image(img);
    free_image(resizedimg);
    cvReleaseCapture(&cap);
    if(1 == isShowWnd){
        cvDestroyWindow("Demo");
    }
    printf("out video_detector func!\n");
}

#endif

