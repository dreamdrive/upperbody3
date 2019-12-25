#include "ros/ros.h"
#include "ros/time.h"

#include "std_msgs/Float64.h"
#include "std_msgs/String.h"

#include "trajectory_msgs/JointTrajectory.h"
#include "sensor_msgs/JointState.h"

std_msgs::String joint1_name, joint2_name;
std_msgs::Float64 joint1_pos, joint2_pos;
std_msgs::Float64 joint1_vel, joint2_vel;
std_msgs::Float64 joint1_eff, joint2_eff;

void monitorJointState_callback(const sensor_msgs::JointState::ConstPtr& jointstate)
{
  joint1_name.data = jointstate->name[0];    // 名前読み出し
  joint2_name.data = jointstate->name[1];    // 名前読み出し

  joint1_pos.data = jointstate->position[0];    // ポジション読み出し
  joint2_pos.data = jointstate->position[1];    // ポジション読み出し

  joint1_vel.data = jointstate->velocity[0];    // 速度読み出し
  joint2_vel.data = jointstate->velocity[1];    // 速度読み出し

  joint1_eff.data = jointstate->effort[0];    // 負荷読み出し
  joint2_eff.data = jointstate->effort[1];    // 負荷読み出し
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "dynamixel2dynamixel");  // ノードの初期化
  ros::NodeHandle nh; // ノードハンドラ  

  ros::Subscriber sub_joints;  // サブスクライバの作成
  sub_joints = nh.subscribe("/dynamixel_workbench/joint_states", 10, monitorJointState_callback);

  ros::Publisher arm_pub; //パブリッシャの作成
  arm_pub = nh.advertise<trajectory_msgs::JointTrajectory>("/dynamixel_workbench/joint_trajectory",100);

  ros::Rate loop_rate(45);  // 制御周期45Hz(サーボの更新が20msecなのですが、50Hzだと余裕ない感じ)

  trajectory_msgs::JointTrajectory jtp0;
  
  jtp0.header.frame_id = "base_link";
  jtp0.points.resize(1);

  jtp0.joint_names.resize(2);
  jtp0.joint_names[0] ="pan";
  jtp0.joint_names[1] ="tilt";
  
  jtp0.points[0].positions.resize(2);

  jtp0.points[0].effort.resize(2);  // 
  jtp0.points[0].effort[0] = 0.0;   // pan側のeffortを0に設定
//jtp0.points[0].effort[1] = 0.0;   // tilt側は設定しない

  joint1_pos.data = 0.0;  // 初期化
 
  while (ros::ok()) { // 無限ループ

    jtp0.header.stamp = ros::Time::now();
    jtp0.points[0].positions[0] = joint1_pos.data;  // panにpanの値をセット(上書き)
    jtp0.points[0].positions[1] = joint1_pos.data;  // tiltにpanの値をセット(マスター＆スレーブ)
    jtp0.points[0].time_from_start = ros::Duration(0.02);  //実行時間20msec (10msecだと動作しない;;)
    arm_pub.publish(jtp0);

    ros::spinOnce();   // コールバック関数を呼ぶ
    loop_rate.sleep();  // 待ち
    ROS_INFO("Joint1(%s)= %f | %f | %f ||| Joint2(%s)= %f | %f | %f ", joint1_name.data.c_str(),joint1_pos.data,joint1_vel.data,joint1_eff.data,joint2_name.data.c_str(),joint2_pos.data,joint2_vel.data,joint2_eff.data);
  }
  
  return 0;
}