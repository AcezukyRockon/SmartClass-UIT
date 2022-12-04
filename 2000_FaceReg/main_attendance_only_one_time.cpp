// facereg
#include <iostream>
#include <opencv2/opencv.hpp>
#include "TMtCNN.h"
#include "TArcface.h"
#include "TRetina.h"
#include "TWarp.h"
#include "TLive.h"
#include "TBlur.h"
//mongo
#include <chrono>
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <iomanip>
//mosquitto
#include <assert.h>
#include <stdlib.h>
#include <mosquitto.h>
//time measure
#include <algorithm>
#include <chrono>
#include <vector>
#include <ctime>
#include <cstring>
//json parse
//#include <json-c/json.h>

using namespace std;
using namespace std::chrono;
//----------------------------------------------------------------------------------------
//
// Created by markson zhang
//
// Edited by Xinghao Chen 2020/7/27
//
// Modified by Q-engineering 2020/12/28
//
//----------------------------------------------------------------------------------------
// Build defines
// comment them to turn a function off
//----------------------------------------------------------------------------------------
#define RETINA                  //comment if you want to use MtCNN landmark detection instead
#define RECOGNIZE_FACE
#define TEST_LIVING
#define AUTO_FILL_DATABASE
#define BLUR_FILTER_STRANGER
// some diagnostics
#define SHOW_LEGEND
#define SHOW_LANDMARKS
//----------------------------------------------------------------------------------------
// Adjustable Parameters
//----------------------------------------------------------------------------------------
const int   MaxItemsDatabase = 2000;
const int   MinHeightFace    = 90;
const float MinFaceThreshold = 0.50;
const float FaceLiving       = 0.93;
const double MaxBlur         = -25.0;   //more positive = sharper image
const double MaxAngle        = 10.0;
//----------------------------------------------------------------------------------------
// Some globals
//----------------------------------------------------------------------------------------
const int   RetinaWidth      = 320;
const int   RetinaHeight     = 240;
float ScaleX, ScaleY;
vector<cv::String> NameFaces;
int count_face_yes = 0;
int count_face_tiny = 0;
int count_face_strange = 0;
int count_face_fake = 0;
int count_face_total = 0;
auto duration_total = 0;

//----------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------------------
//  Computing the cosine distance between input feature and ground truth feature
//----------------------------------------------------------------------------------------
inline float CosineDistance(const cv::Mat &v1, const cv::Mat &v2)
{
    double dot = v1.dot(v2);
    double denom_v1 = norm(v1);
    double denom_v2 = norm(v2);
    return dot / (denom_v1 * denom_v2);
}
//----------------------------------------------------------------------------------------
// painting
//----------------------------------------------------------------------------------------
void DrawObjects(cv::Mat &frame, vector<FaceObject> &Faces)
{
    for(size_t i=0; i < Faces.size(); i++){
        FaceObject& obj = Faces[i];

//----- rectangle around the face -------
        obj.rect.x *= ScaleX;
        obj.rect.y *= ScaleY;
        obj.rect.width *= ScaleX;
        obj.rect.height*= ScaleY;
        cv::rectangle(frame, obj.rect, cv::Scalar(0, 255, 0));
//---------------------------------------

//----- diagnostic ----------------------
#ifdef SHOW_LANDMARKS
        for(int u=0;u<5;u++){
            obj.landmark[u].x*=ScaleX;
            obj.landmark[u].y*=ScaleY;
        }

        cv::circle(frame, obj.landmark[0], 2, cv::Scalar(0, 255, 255), -1);
        cv::circle(frame, obj.landmark[1], 2, cv::Scalar(0, 255, 255), -1);
        cv::circle(frame, obj.landmark[2], 2, cv::Scalar(0, 255, 255), -1);
        cv::circle(frame, obj.landmark[3], 2, cv::Scalar(0, 255, 255), -1);
        cv::circle(frame, obj.landmark[4], 2, cv::Scalar(0, 255, 255), -1);
#endif // SHOW_LANDMARKS
//---------------------------------------
#ifdef SHOW_LEGEND
        cv::putText(frame, cv::format("Angle : %0.1f", obj.Angle),cv::Point(10,40),cv::FONT_HERSHEY_SIMPLEX,0.6, cv::Scalar(180, 180, 0));
        cv::putText(frame, cv::format("Face prob : %0.4f", obj.FaceProb),cv::Point(10,60),cv::FONT_HERSHEY_SIMPLEX,0.6, cv::Scalar(180, 180, 0));
        cv::putText(frame, cv::format("Name prob : %0.4f", obj.NameProb),cv::Point(10,80),cv::FONT_HERSHEY_SIMPLEX,0.6, cv::Scalar(180, 180, 0));
#ifdef TEST_LIVING
        if(obj.Color==2){
            //face is too tiny
            cv::putText(frame, cv::format("Live prob : ??"),cv::Point(10,100),cv::FONT_HERSHEY_SIMPLEX,0.6, cv::Scalar(180, 180, 0));
        }
        else{
            //face is ok
            cv::putText(frame, cv::format("Live prob : %0.4f", obj.LiveProb),cv::Point(10,100),cv::FONT_HERSHEY_SIMPLEX,0.6, cv::Scalar(180, 180, 0));
        }
#endif // TEST_LIVING
#endif // SHOW_LEGEND
//----- labels ----------------------------
#ifdef RECOGNIZE_FACE
        cv::String Str;
        cv::Scalar color;
        int  baseLine = 0;

        switch(obj.Color){
            case 0 : color = cv::Scalar(255, 255, 255); break;  //default white -> face ok
            case 1 : color = cv::Scalar( 80, 255, 255); break;  //yellow ->stranger
            case 2 : color = cv::Scalar(255, 237, 178); break;  //blue -> too tiny
            case 3 : color = cv::Scalar(127, 127, 255); break;  //red -> fake
            default: color = cv::Scalar(255, 255, 255);
        }

        switch(obj.NameIndex){
            case -1: Str="Stranger"; break;
            case -2: Str="too tiny"; break;
            case -3: Str="Fake !"; break;
            default: Str=NameFaces[obj.NameIndex];
        }

        cv::Size label_size = cv::getTextSize(Str, cv::FONT_HERSHEY_SIMPLEX, 0.6, 1, &baseLine);
        int x = obj.rect.x;
        int y = obj.rect.y - label_size.height - baseLine;
        if(y<0) y = 0;
        if(x+label_size.width > frame.cols) x=frame.cols-label_size.width;

        cv::rectangle(frame, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),color, -1);
        cv::putText(frame, Str, cv::Point(x, y+label_size.height+2),cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0));
        //cout << "Nhan ra: " << Str << endl; //print person in console log
#endif // RECOGNIZE_FACE
    }
}

// connect to mosquitto local function
void mosquitto_facereg(const char topic[11], const char str[12]){
    int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);

	rc = mosquitto_connect(mosq, "localhost", 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
	}
	mosquitto_publish(mosq, NULL, topic, 11, str, 0, false);
	//printf("%d",result);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

void get_iso_date(char isodate_c [11]) {
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    char *ptr; // declare a ptr pointer
    ptr = strtok(buf, " T "); // use strtok() function to separate string using comma (,) delimiter.
    // use while loop to check ptr is not null
    while (ptr != NULL)
    {
        //cout << ptr  << endl; // print the string token
        ptr = strtok (NULL, " T ");
    }
    strcpy(isodate_c, buf);
}

// mongo insert
bool mongo_checkifalrattened(mongoc_collection_t *collection, char class_id[20]){
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_t *query;
    char *str;
    char isodate_c [11];
    bool check = 0;
    get_iso_date(isodate_c);

    query = bson_new ();
    BSON_APPEND_UTF8 (query, "iso_date", isodate_c);
    BSON_APPEND_UTF8 (query, "class_id", class_id);

    cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

    while (mongoc_cursor_next (cursor, &doc)) {
      str = bson_as_canonical_extended_json (doc, NULL);
      //printf ("%s\n", str);
      //cout << str << endl;
      bson_free (str);
      check = 1;
   }

    bson_destroy (query);
    mongoc_cursor_destroy (cursor);
    return check;
}

void mongo_facereg(mongoc_collection_t *collection, char class_id[20]){
    bson_oid_t oid;
    bson_t *doc;
    bson_error_t error;
    char isodate_c [11];
    get_iso_date(isodate_c);

    /*time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *s = asctime (timeinfo);
    s[strlen(s)-1]  = '\0';
    printf ( "Current local time and date: %s", asctime (timeinfo) );*/

    doc = bson_new ();
    bson_oid_init (&oid, NULL);
    BSON_APPEND_OID (doc, "_id", &oid);
    BSON_APPEND_UTF8 (doc, "class_id", class_id);
    //BSON_APPEND_UTF8 (doc, "timestamp", s);
    bson_append_now_utc (doc, "timestamp", -1);
    BSON_APPEND_UTF8 (doc, "iso_date", isodate_c);

    if (!mongoc_collection_insert_one (
           collection, doc, NULL, NULL, &error)) {
        fprintf (stderr, "%s\n", error.message);
    }

    bson_destroy (doc);
}

//----------------------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    float f;
    float FPS[16];
    int   n,Fcnt=0;
    size_t i;
    cv::Mat frame;
    cv::Mat result_cnn;
    cv::Mat faces;
    std::vector<FaceObject> Faces;
    vector<cv::Mat> fc1;
    string pattern_jpg = "./img/*.jpg";
    cv::String NewItemName;
    size_t FaceCnt;
    //the networks
    TLive Live;
    TWarp Warp;
    TMtCNN MtCNN;
    TArcFace ArcFace;
    TRetina Rtn(RetinaWidth, RetinaHeight, true);     //have Vulkan support on a Jetson Nano
    TBlur Blur;
    //some timing
    chrono::steady_clock::time_point Tbegin, Tend;

    // mongodb ----------------------------------------------------------
    mongoc_client_t *client;
    mongoc_collection_t *collection;

    // lux test case------------------------------------------------------
    //int count_face_yes = 0;
    //int count_face_tiny = 0;
    //int count_face_strange = 0;
    //int count_face_fake = 0;
    //int count_face_total = 0;
    //auto duration_total = 0;

    mongoc_init ();
    //client = mongoc_client_new ("mongodb+srv://nhom1:nhom1@smartpodium.ra3hh.mongodb.net/SmartDB?retryWrites=true&w=majority");
    client = mongoc_client_new ("mongodb://localhost:27017");
    char class_id[20] = "CE412.M21.MTCL";
    // mongodb end-------------------------------------------------------

    // mosquitto message declaration
    //char mos_str_on[4] = "ONN";
    //char mos_str_off[4] = "OFF";
    int startfacecheck_counter = 20;
	int check_timer_startface_recognizeface = 0;
	auto start_timer = high_resolution_clock::now();

    Live.LoadModel();

    for(i=0;i<16;i++) FPS[i]=0.0;

    //OpenCV Version
    cout << "OpenCV Version: " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "."
    << CV_SUBMINOR_VERSION << endl;
    cout << " " << endl;

#ifdef RECOGNIZE_FACE
    cout << "Trying to recognize faces" << endl;
    cout << " " << endl;
#ifdef RETINA
    cout << "Using Retina" << endl;
    cout << " " << endl;
#else
    cout << "Using MtCNN" << endl;
    cout << " " << endl;
#endif // RETINA

#ifdef TEST_LIVING
    cout << "Test living or fake fave" << endl;
    cout << " " << endl;
#endif // TEST_LIVING

#ifdef AUTO_FILL_DATABASE
    cout << "Automatic adding strangers to database" << endl;
    cout << " " << endl;
#ifdef BLUR_FILTER_STRANGER
    cout << "Blur filter - only sharp images to database" << endl;
    cout << " " << endl;
#endif // BLUR_FILTER_STRANGER
#endif // AUTO_FILL_DATABASE
#endif // RECOGNIZE_FACE

    //if you like to load a picture of a face into the database
    //give the name of the .jpg image as argument on the command line
    //without arguments the app will run the .mp4 video or use the camera
    if(argc>1){
        const char* imagepath = argv[1];

        cv::Mat frame = cv::imread(imagepath, 1);
        if(frame.empty()){
            fprintf(stderr, "cv::imread %s failed\n", imagepath);
            return -1;
        }
        //extract
        ScaleX = ((float) frame.cols) / RetinaWidth;
        ScaleY = ((float) frame.rows) / RetinaHeight;
        // copy/resize image to result_cnn as input tensor
        cv::resize(frame, result_cnn, Size(RetinaWidth,RetinaHeight),INTER_LINEAR);
        //get the face
        Rtn.detect_retinaface(result_cnn,Faces);
        //only one face per picture
        if(Faces.size()==1){
            if(Faces[0].FaceProb>MinFaceThreshold){
                //get centre aligned image
                cv::Mat aligned = Warp.Process(result_cnn,Faces[0]);

                cv::String Str = imagepath;
                n   = Str.rfind('/');
                Str = Str.erase(0,n+1);
                Str = Str.erase(Str.length()-4, Str.length()-1);  //remove .jpg


                imwrite("./img/"+Str+".jpg", aligned);
                cout << "Stored to database : " << Str << endl;
            }
        }
        return 0;
    }


    //loading the faces
	cv::glob(pattern_jpg, NameFaces);
    FaceCnt=NameFaces.size();
	if(FaceCnt==0) {
		cout << "No image files[jpg] in database" << endl;
	}
	else{
        cout << "Found "<< FaceCnt << " pictures in database." << endl;
        for(i=0; i<FaceCnt; i++){
            //convert to landmark vector and store into fc
            faces = cv::imread(NameFaces[i]);
            fc1.push_back(ArcFace.GetFeature(faces));
            //get a proper name
            cv::String &Str = NameFaces[i];
            n   = Str.rfind('/');
            Str = Str.erase(0,n+1);
            n   = Str.find('#');
            if(n>0) Str = Str.erase(n,Str.length()-1);                //remove # some numbers.jpg
            else    Str = Str.erase(Str.length()-4, Str.length()-1);  //remove .jpg
            if(FaceCnt>1) printf("\rloading: %.2lf%% ",(i*100.0)/(FaceCnt-1));
        }
        cout << "" << endl;
        cout << "Loaded "<<FaceCnt<<" faces in total"<<endl;
    }

    // RaspiCam or Norton_2.mp4 ?
    //cv::VideoCapture cap(0);             //RaspiCam
    //cv::VideoCapture cap("Norton_A.mp4");   //Movie
    cv::VideoCapture cap("cut2.mp4");   //Movie
    if (!cap.isOpened()) {
        cerr << "ERROR: Unable to open the camera" << endl;
        return 0;
    }
    cout << "Start grabbing, press ESC on TLive window to terminate" << endl;
    while(1){
        cap >> frame;
        //count_face_total++;
        if (frame.empty()) {
            cerr << "End of movie" << endl;
            break;
        }
        ScaleX = ((float) frame.cols) / RetinaWidth;
        ScaleY = ((float) frame.rows) / RetinaHeight;

        // copy/resize image to result_cnn as input tensor
        cv::resize(frame, result_cnn, Size(RetinaWidth,RetinaHeight),INTER_LINEAR);

        Tbegin = chrono::steady_clock::now();

#ifdef RETINA
        Rtn.detect_retinaface(result_cnn,Faces);
#else
        MtCNN.detect(result_cnn,Faces);
#endif // RETINA

#ifdef RECOGNIZE_FACE
        //reset indicators
        for(i=0;i<Faces.size();i++){
            Faces[i].NameIndex = -2;    //-2 -> too tiny (may be negative to signal the drawing)
            Faces[i].Color     =  2;
            Faces[i].NameProb  = 0.0;
            Faces[i].LiveProb  = 0.0;
        }
        //run through the faces only when you got one face.
        //more faces (if large enough) are not a problem
        //in this app with an input image of 324x240, they become too tiny

		if(Faces.size()==1){
            if (check_timer_startface_recognizeface == 0) {
                start_timer = high_resolution_clock::now();
                check_timer_startface_recognizeface = 1;
            }
            //looks stupid, running through a loop of size 1
            //however, for your convenience using [i]
            for(i=0;i<Faces.size();i++){
                if(Faces[i].FaceProb>MinFaceThreshold){
                    //get centre aligned image
                    cv::Mat aligned = Warp.Process(result_cnn,Faces[i]);
                    Faces[i].Angle  = Warp.Angle;
                    //features of camera image
                    cv::Mat fc2 = ArcFace.GetFeature(aligned);
                    //reset indicators
                    Faces[i].NameIndex = -1;    //a stranger
                    Faces[i].Color     =  1;
                    //the similarity score
                    if(FaceCnt>0){

                        vector<double> score_;
                        for(size_t c=0;c<FaceCnt;c++) score_.push_back(CosineDistance(fc1[c], fc2));
                        int Pmax = max_element(score_.begin(),score_.end()) - score_.begin();
                        Faces[i].NameIndex = Pmax;
                        Faces[i].NameProb  = score_[Pmax];
                        score_.clear();
                        if(Faces[i].NameProb >= MinFaceThreshold){
                            //recognize a face
                            if(Faces[i].rect.height < MinHeightFace){//##########################################################################################3
                                Faces[i].Color = 2; //found face in database, but too tiny
                                count_face_yes++;
                                if ((count_face_yes % startfacecheck_counter) == 0){
                                    collection = mongoc_client_get_collection (client, "SmartDB", NameFaces[Faces[i].NameIndex].c_str());
                                    mosquitto_facereg("ESP32/LED1","1stLEDisONN");
                                    if (mongo_checkifalrattened(collection, class_id) == 0) {
                                        mongo_facereg(collection, class_id);
                                        mongoc_collection_destroy (collection);
										cout << "updated " << NameFaces[Faces[i].NameIndex].c_str() << " to database" << endl;
                                    }
                                    else cout << NameFaces[Faces[i].NameIndex].c_str() << " alr reg" << endl;
                                }

								if ((count_face_yes % startfacecheck_counter) == 0){
									auto stop_timer = high_resolution_clock::now();
									auto duration = duration_cast<microseconds>(stop_timer - start_timer);
									cout << "[TEST TIME] Time taken by function: "<< duration.count() << " microseconds" << endl;
									//duration_total = duration_total + duration.count();
									//cout << "[TEST TIME] Total duration after " << count_face_yes << " time(s): " << duration_total << " microseconds" << endl;
								}
								check_timer_startface_recognizeface = 0;
                            }
                            else{//##########################################################################################3
                                //auto start_timer = high_resolution_clock::now();
                                Faces[i].Color = 0; //found face in database and of good size
                                //cout << "main: " << NameFaces[Faces[i].NameIndex] << endl;
                                count_face_yes++;

                                // mosquitto publish
                                if ((count_face_yes % startfacecheck_counter) == 0){
                                    collection = mongoc_client_get_collection (client, "SmartDB", NameFaces[Faces[i].NameIndex].c_str());
                                    mosquitto_facereg("ESP32/LED1","1stLEDisONN");
                                    if (mongo_checkifalrattened(collection, class_id) == 0) {
                                        mongo_facereg(collection, class_id);
                                        mongoc_collection_destroy (collection);
										cout << "updated " << NameFaces[Faces[i].NameIndex].c_str() << " to database" << endl;
                                    }
                                    else cout << NameFaces[Faces[i].NameIndex].c_str() << " alr reg" << endl;
                                }

								if ((count_face_yes % startfacecheck_counter) == 0){
									auto stop_timer = high_resolution_clock::now();
									auto duration = duration_cast<microseconds>(stop_timer - start_timer);
									cout << "[TEST TIME] Time taken by function: "<< duration.count() << " microseconds" << endl;
									//duration_total = duration_total + duration.count();
									//cout << "[TEST TIME] Total duration after " << count_face_yes << " time(s): " << duration_total << " microseconds" << endl;
								}
								check_timer_startface_recognizeface = 0;

#ifdef TEST_LIVING
                                //test fake face
                                float x1 = Faces[i].rect.x;
                                float y1 = Faces[i].rect.y;
                                float x2 = Faces[i].rect.width+x1;
                                float y2 = Faces[i].rect.height+y1;
                                struct LiveFaceBox LiveBox={x1,y1,x2,y2};

                                Faces[i].LiveProb=Live.Detect(result_cnn,LiveBox);
                                if(Faces[i].LiveProb<=FaceLiving){
                                    Faces[i].Color     =  3; //fake
                                    Faces[i].NameIndex = -3;
                                    count_face_fake++;
                                }
#endif // TEST_LIVING
                            }
                        }
                        else{
                            Faces[i].NameIndex = -1;    //a stranger
                            Faces[i].Color     =  1;
                            count_face_strange++;
                        }
                    }
                    // for LUX test case
                    //cout << "count_face_yes: " << count_face_yes << endl;
                    //cout << ": " << count_face_total << endl;
                    //count_face_yes_percent = count_face_yes / count_face_total;
                    //cout << "count_face_yes_percent: " << count_face_yes_percent << endl;
                    //std::cout << std::fixed << std::setprecision(2) << count_face_yes_percent << endl;
                    //test if the face is recognized, or should it be added to database
                    if(Faces[i].NameIndex == -1){
                        if(Faces[i].rect.height < MinHeightFace){
                            //a stranger with a small face
                            Faces[i].Color = 2; //too tiny
                            count_face_tiny++;
                        }
#ifdef AUTO_FILL_DATABASE
                        else{
                            //a stranger with a large face, who can be added to the database
                            if(FaceCnt>=MaxItemsDatabase){
                                cout << "Database f2222222222 !" << endl;
                            }
                            else{
                                double blur=0.0;
#ifdef BLUR_FILTER_STRANGER
                                blur=Blur.Execute(aligned);
#endif // BLUR_FILTER_STRANGER
                                if(Warp.Angle<=MaxAngle && blur>=MaxBlur){
//                                cout <<"Probability : " << Faces[i].FaceProb << "   Angle : " << Warp.Angle << "   Blur : " << blur << endl;
//                                cv::imshow("Add",aligned);
                                    FaceCnt++;
                                    NewItemName = format("person %i",(int)FaceCnt);
                                    fc1.push_back(fc2);
                                    NameFaces.push_back(NewItemName);
                                    imwrite("./img/"+NewItemName+".jpg", aligned);
                                    cout << "Stored to database : " << NewItemName << endl;
                                }
                            }
                        }
#endif // AUTO_FILL_DATABASE
                    }
                }
            }
        }
#endif // RECOGNIZE_FACE

        Tend = chrono::steady_clock::now();

        DrawObjects(frame, Faces);

        //calculate frame rate
        f = chrono::duration_cast <chrono::milliseconds> (Tend - Tbegin).count();
        if(f>0.0) FPS[((Fcnt++)&0x0F)]=1000.0/f;
        for(f=0.0, i=0;i<16;i++){ f+=FPS[i]; }
        cv::putText(frame, cv::format("FPS %0.2f", f/16),cv::Point(10,20),cv::FONT_HERSHEY_SIMPLEX,0.6, cv::Scalar(180, 180, 0));

        //show output
        cv::imshow("Jetson Nano - 2014.5 MHz", frame);
        char esc = cv::waitKey(5);
        if(esc == 27) break;
        // print debugs for project
        //count_face_total = count_face_yes + count_face_tiny + count_face_strange + count_face_fake;
        //cout << "[TEST ACCURACY count_face] yes: " << count_face_yes <<  " | tiny: " << count_face_tiny << " | stranger: " << count_face_strange << " | fake: " << count_face_fake << " | total: "<<  count_face_total << endl;
        //if(count_face_total==1000)
        //{
        //    exit(0);
        //}
    }
    cv::destroyAllWindows();
    mongoc_client_destroy (client);
    mongoc_cleanup ();
    return 0;
}
