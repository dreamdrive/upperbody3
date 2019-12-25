#include "ros/ros.h"
#include "ros/time.h"

#include "std_msgs/Float64.h"
#include "std_msgs/String.h"

#include "trajectory_msgs/JointTrajectory.h"
#include "opencv_apps/FaceArrayStamped.h"

# define SRVCNT 5

std_msgs::String joint_name[SRVCNT];
std_msgs::Float64 joint_pos[SRVCNT];
std_msgs::Float64 joint_vel[SRVCNT];
std_msgs::Float64 joint_eff[SRVCNT];

double face_x = 0;
double face_y = 0;

ros::Publisher arm_pub;

void face_callback(const opencv_apps::FaceArrayStamped::ConstPtr& facearraystamped)
{
  //facearraystamped->faces[0].face.x;
  //facearraystamped->faces[0].face.y;
  //facearraystamped->faces[0].face.height;
  //facearraystamped->faces[0].face.width;
  
  face_x = 0; // 未検出のときは0
  face_y = 0; // 未検出のときは0

  if ( facearraystamped->faces.size() > 0 ){
    ROS_INFO("The face is in X = %d , Y = %d", (int)facearraystamped->faces[0].face.x,(int)facearraystamped->faces[0].face.y);
    face_x = facearraystamped->faces[0].face.x;
    face_y = facearraystamped->faces[0].face.y;
  }

}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "dynamixel_face");  // ノードの初期化
  ros::NodeHandle nh; // ノードハンドラ

  ros::Subscriber sub_face;  // サブスクライバの作成
  sub_face = nh.subscribe("/face_detection/faces", 10, face_callback);

  //パブリッシャの作成
  arm_pub = nh.advertise<trajectory_msgs::JointTrajectory>("/dynamixel_workbench/joint_trajectory",1);

  ros::Rate loop_rate(10);  // 制御周期5Hz
  
  trajectory_msgs::JointTrajectory jtp0;

  jtp0.header.frame_id = "base_link";   // ポーズ名（モーション名)

  jtp0.joint_names.resize(SRVCNT); // 名前
  jtp0.joint_names[0] ="waist";
  jtp0.joint_names[1] ="right_j1";
  jtp0.joint_names[2] ="right_j2";
  jtp0.joint_names[3] ="left_j1";
  jtp0.joint_names[4] ="left_j2";

  jtp0.points.resize(1);                // ポーズは一つだけ

  jtp0.points[0].positions.resize(SRVCNT); // ポジションを5個設定  
  jtp0.points[0].velocities.resize(SRVCNT); // ポジションを5個設定  
  jtp0.points[0].accelerations.resize(SRVCNT); // ポジションを5個設定  
  jtp0.points[0].effort.resize(SRVCNT); // ポジションを5個設定  

  jtp0.points[0].positions[0] = 0.0;
  jtp0.points[0].positions[1] = 0.0;   // 原点のコマンドを送り続けるので起動時にposition0に移動するだけ
  jtp0.points[0].positions[2] = 0.0;   // 原点のコマンドを送り続けるので起動時にposition0に移動するだけ
  jtp0.points[0].positions[3] = 0.0;   // 原点のコマンドを送り続けるので起動時にposition0に移動するだけ
  jtp0.points[0].positions[4] = 0.0;   // 原点のコマンドを送り続けるので起動時にposition0に移動するだけ

  while(ros::ok()) {
    jtp0.header.stamp = ros::Time::now();  

    if(fabs(jtp0.points[0].positions[0] - ((double)face_x - 320.0)/5000.0) < 1.7){
      jtp0.points[0].positions[0] = jtp0.points[0].positions[0] - (face_x - 320.0)/5000.0;
    }

    if(fabs(((double)face_x - 240.0)/240.0) < 1.2){
      jtp0.points[0].positions[2] = (face_y - 240.0)/240.0;
      jtp0.points[0].positions[4] = (face_y - 240.0)/240.0;
    }
    jtp0.points[0].time_from_start = ros::Duration(0.08);  //実行時間(0.5秒かけて移動)

    arm_pub.publish(jtp0);
    ros::spinOnce();   // コールバック関数を呼ぶ
    loop_rate.sleep();
    //ROS_INFO("Joint1(%s)= %f | Joint2(%s)= %f ", jtp0.joint_names[0].c_str(),jtp0.points[0].positions[0], jtp0.joint_names[1].c_str(),jtp0.points[0].positions[1]);
  }

  return 0;
}

