#include "opencv.hpp"
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
using namespace cv;
using namespace std;


#define POSE_TO_USE "Psi"
xn::UserGenerator g_UserGenerator;
xn::SceneMetaData scenemetadata;
void XN_CALLBACK_TYPE
	User_NewUser(xn::UserGenerator& generator,XnUserID nId,void* pCookie)
{
	printf("New User %d\n",nId);
	g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(POSE_TO_USE,nId);
}
void XN_CALLBACK_TYPE
	User_LostUser(xn::UserGenerator& generator,XnUserID nId,void* pCookie)
{ 
}
void XN_CALLBACK_TYPE
	Pose_Detected(xn::PoseDetectionCapability& pose,const XnChar* strPose,XnUserID nId,void* pCookie)
{
	printf("Pose %s for user %d\n",strPose,nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId,TRUE);
}
void XN_CALLBACK_TYPE
	Calibration_Start(xn::SkeletonCapability& capability,XnUserID nId,void* pCookie) 
{
	printf("Starting calibration for user %d\n",nId);
}
void XN_CALLBACK_TYPE
	Calibration_End(xn::SkeletonCapability& capability,XnUserID nId,XnBool bSuccess,void* pCookie) 
{
	if(bSuccess)
	{printf("User calibrated\n");
	g_UserGenerator.GetSkeletonCap().StartTracking(nId);
		}
	else
	{printf("Failed to calibrate");
	g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(POSE_TO_USE,nId);
	}


}
void main()
{printf(" ----------MEASUREMENT GENERATOR----------\n");
	XnStatus nRetVal=XN_STATUS_OK;
XnSkeletonJointPosition Head,Neck,sl,sr,el,wl,hl,hr,fl,ftl;
double width;
xn::Context context;
double max;
double min;
double c,w;
double slength,alength,tlength,chest,waist;
nRetVal=context.Init();
Mat img;
int thresh=100;
nRetVal=g_UserGenerator.Create(context);
XnCallbackHandle h1,h2,h3;
g_UserGenerator.RegisterUserCallbacks(User_NewUser,User_LostUser,NULL,h1);
g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(Pose_Detected,NULL,NULL,h2);
g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(Calibration_Start,Calibration_End,NULL,h3);
g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
nRetVal=context.StartGeneratingAll();
printf("Kindly get ready to give your measurements \n");
int k=0;
double armwidth,thigh,sleevelength;
while(TRUE)
{k++;
	nRetVal=context.WaitAndUpdateAll();
XnUserID aUsers[15];
XnUInt16 nUsers=15;
g_UserGenerator.GetUsers(aUsers,nUsers);
for(int i=0;i<nUsers;++i)
{
	if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
	{//XnSkeletonJointPosition Head;
	
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_HEAD,Head);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_NECK,Neck);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_LEFT_SHOULDER,sl);
		g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_LEFT_FINGERTIP,ftl);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_RIGHT_SHOULDER,sr);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_LEFT_ELBOW,el);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_LEFT_WRIST,wl);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_LEFT_HIP,hl);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_RIGHT_HIP,hr);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i],XN_SKEL_LEFT_FOOT,fl);

	slength=sqrt(pow(sl.position.X-Neck.position.X,2)+pow(sl.position.Y-Neck.position.Y,2));
	sleevelength=sqrt(pow(sl.position.X-ftl.position.X,2)+pow(sl.position.Y-ftl.position.Y,2));
	alength=sqrt(pow(sl.position.X-wl.position.X,2)+pow(sl.position.Y-wl.position.Y,2));
	tlength=sqrt(pow(hl.position.X-fl.position.X,2)+pow(hl.position.Y-fl.position.Y,2));
	//printf("%d %d %f %f %f\n",k,aUsers[i],Head.position.X,Head.position.Y,Head.position.Z,Head.fConfidence);
	}

c=(sr.position.X-sl.position.X)/10;
w=(hr.position.X-hl.position.X)/10;
}

if(k==300)
	break;
}// printf("Shoulder length is %.2f cm\n",slength/10);
//printf("Arm length is %f cm\n",alength/10);
//printf("Trouser length is %f cm\n",tlength/10);*/
//printf("WAIST %f",w);
//context.Shutdown();
//}
printf("Now kindly turn to your side ");
int l=0;
while(TRUE)
{
l++;

	nRetVal=context.WaitAndUpdateAll();
XnUserID aUsers[15];
XnUInt16 nUsers=15;
g_UserGenerator.GetUsers(aUsers,nUsers);

for(int i=0; i<nUsers; ++i )
{ 
	if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])) 
			{   g_UserGenerator.GetUserPixels(aUsers[i],scenemetadata);
				const XnLabel *depthPixels = scenemetadata.Data();		 
				
				img=Mat::zeros(480,640,CV_8UC3);
			


				 
					int x=0;
					int y=0;
					for(x=0;x<640;++x)
					{
						for(y=0;y<480;++y)
						{
							if (depthPixels[640*y+x]!=0)
							{
								
								img.at<unsigned char>(y,x)=255;
								
							}
					
						}
					}
					blur(img,img,Size(3,3));
					Mat output;
				vector<vector<Point>> contours;
				vector<Vec4i> hierarchy;
				Canny(img,output,thresh,thresh*2,3);
				findContours( output, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
				 max=contours[0][0].x;
				 min=contours[0][0].x;
				int i,j;
				for(i=0;i<contours.size();i++)
				{for(j=0;j<contours[i].size();j++)
				{if(max<contours[i][j].x)
				max=contours[i][j].x;
				}
				}
				for(i=0;i<contours.size();i++)
				{for(j=0;j<contours[i].size();j++)
				{if(min>contours[i][j].x)
				min=contours[i][j].x;
				}
				}
				//printf("%d  Width in mm is: %f\n",l,max-min); 
				imshow("Image",output);
				waitKey(1);
			} } 
if(l==160)
	break;
}
width=(max-min)/10;
double s=(hr.position.X-hl.position.Y)/10;
double seat=2*3.14*sqrt(0.5*(pow(s,2)+pow(width,2)))/3.5;
chest=2*3.14*sqrt(0.5*(pow(c,2)+pow(width,2)))/3.5;
waist=2*3.14*sqrt(0.5*(pow(w,2)+pow(width,2)))/3;
/*printf("WAIST IN CM IS %f \n",waist);
printf("CHEST IN CM IS %f \n",chest);
context.Shutdown();
printf("REQUIRED MEASUREMENTS \n");
printf("SHOULDER LENGTH --> %f CM\n",slength/10);
printf("ARM LENGTH --> %f CM\n",alength/10);
printf("TROUSER LENGTH --> %f CM\n",tlength/10);
printf("WAIST --> %f CM\n",waist);
printf("CHEST --> %f CM\n",chest);*/
printf("\n Thank you!\n Now kindly turn back and stretch your arms\n ");
int m=0;
while(TRUE)
{
m++;

	nRetVal=context.WaitAndUpdateAll();
XnUserID aUsers[15];
XnUInt16 nUsers=15;
g_UserGenerator.GetUsers(aUsers,nUsers);

for(int i=0; i<nUsers; ++i )
{ 
	if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])) 
			{   g_UserGenerator.GetUserPixels(aUsers[i],scenemetadata);
				const XnLabel *depthPixels = scenemetadata.Data();		 
				
				img=Mat::zeros(480,640,CV_8UC3);

				 
					int a=0;
					int b=0;
					for(a=0;a<320;++a)
					{
						for(b=0;b<480;++b)
						{
							if (depthPixels[640*b+a]!=0)
							{
								
								img.at<unsigned char>(b,a)=255;
								
							}
					
						}
					}
					blur(img,img,Size(3,3));
					Mat output;
				vector<vector<Point>> contours;
				vector<Vec4i> hierarchy;
				Canny(img,output,thresh,thresh*2,3);
				findContours( output, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
				 max=contours[0][0].y;
				 min=contours[0][0].y;
				int i,j;
				for(i=0;i<contours.size();i++)
				{for(j=0;j<contours[i].size();j++)
				{if(max<contours[i][j].y)
				max=contours[i][j].y;
				}
				}
				for(i=0;i<contours.size();i++)
				{for(j=0;j<contours[i].size();j++)
				{if(min>contours[i][j].y)
				min=contours[i][j].y;
				}
				}
				//printf("%d  Width in mm is: %f\n",l,max-min); 
				imshow("Image",output);
				waitKey(1);
			} } 
if(m==160)
	break;}
armwidth=max-min;
printf("Armwidth is %f cm",armwidth/10);
printf(" Thank you! Now kindly stretch your legs apart ");
int n=0;
while(TRUE)
{
n++;

	nRetVal=context.WaitAndUpdateAll();
XnUserID aUsers[15];
XnUInt16 nUsers=15;
g_UserGenerator.GetUsers(aUsers,nUsers);

for(int i=0; i<nUsers; ++i )
{ 
	if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])) 
			{   g_UserGenerator.GetUserPixels(aUsers[i],scenemetadata);
				const XnLabel *depthPixels = scenemetadata.Data();		 
				
				img=Mat::zeros(480,640,CV_8UC3);
			


				 
					int x=0;
					int y=0;
					for(x=0;x<300;++x)
					{
						for(y=270;y<480;++y)
						{
							if (depthPixels[640*y+x]!=0)
							{
								
								img.at<unsigned char>(y,x)=255;
								
							}
					
						}
					}
					blur(img,img,Size(3,3));
					Mat output;
				vector<vector<Point>> contours;
				vector<Vec4i> hierarchy;
				Canny(img,output,thresh,thresh*2,3);
				findContours( output, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
				 max=contours[0][0].x;
				 min=contours[0][0].x;
				int i,j;
				for(i=0;i<contours.size();i++)
				{for(j=0;j<contours[i].size();j++)
				{if(max<contours[i][j].x)
				max=contours[i][j].x;
				}
				for(i=0;i<contours.size();i++)
				{for(j=0;j<contours[i].size();j++)
				{if(min>contours[i][j].x)
				min=contours[i][j].x;
				}
				}
				//printf("%d  Width in mm is: %f\n",l,max-min); 
				imshow("Image",output);
				waitKey(1);
				} } }
if(n==160)
	{ //thigh=(max-min)/10;
		break;
}thigh=(max-min)/10;
}//printf("Leg width is %f ",max-min);
printf("CHEST -->  %.2f CM \n",chest);
printf("WAIST --> %.2f CM\n",waist);
printf("SEAT --> %.2f CM \n",seat);
printf("JACKET LENGTH --> %.2f CM\n",alength/10);
printf("FRONT WIDTH -->  %.2f CM \n",chest/2);
printf("ARMHOLE --> %.2f CM",armwidth*3.14/10);
printf("SHOULDER LENGTH --> %.2f CM\n",slength/10);
printf("WAISTBAND --> %.2f CM\n",waist);
printf("THIGH --> %.2f CM",thigh*3.14/10);
printf("SLEEVE LENGTH --> %.2f CM\n",sleevelength);
getchar();
}


	

