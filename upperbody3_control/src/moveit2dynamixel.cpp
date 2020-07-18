#include "ros/ros.h"
#include "ros/time.h"

#include "std_msgs/Float64.h"
#include "std_msgs/String.h"

#include "trajectory_msgs/JointTrajectory.h"
#include "sensor_msgs/JointState.h"

std_msgs::String joint_name[5];
std_msgs::Float64 joint_pos[5];
std_msgs::Float64 joint_vel[5];
std_msgs::Float64 joint_eff[5];

//コールバックがあるとグローバルに読み込み
void monitorJointState_callback(const sensor_msgs::JointState::ConstPtr& jointstate)
{
  int i,j;
  for(i=0;i<5;i++){
    for(j=0;j<5;j++){
      if(joint_name[i].data == jointstate->name[j]){
        joint_pos[i].data = jointstate->position[j];    // ポジション読み出し
       // joint_vel[i].data = jointstate->velocity[j];    // 速度読み出し
       // joint_eff[i].data = jointstate->effort[j];    // 負荷読み出し
      }
    }
  }
}

int main(int argc, char **argv)
{
  int i;
  ros::init(argc, argv, "moveit2dynamixel");   // ノードの初期化
  ros::NodeHandle nh;                             // ノードハンドラ

  ros::Subscriber sub_joints;                     // サブスクライバの作成
  sub_joints = nh.subscribe("/joint_states", 1, monitorJointState_callback);    // moveitから取る

  ros::Publisher arm_pub;                         //パブリッシャの作成
  arm_pub = nh.advertise<trajectory_msgs::JointTrajectory>("/dynamixel_workbench/joint_trajectory",1);

  ros::Rate loop_rate(10);  // 制御周期45Hz(サーボの更新が20msecなのですが、50Hzだと余裕ない感じ)

  trajectory_msgs::JointTrajectory jtp0;
  
  jtp0.header.frame_id = "base_link";   // ポーズ名（モーション名)

  jtp0.joint_names.resize(5); // 名前
  jtp0.joint_names[0] ="waist";
  jtp0.joint_names[1] ="right_j1";
  jtp0.joint_names[2] ="right_j2";
  jtp0.joint_names[3] ="left_j1";
  jtp0.joint_names[4] ="left_j2";
  
  joint_name[0].data = "waist";
  joint_name[1].data = "right_j1";
  joint_name[2].data = "right_j2";
  joint_name[3].data = "left_j1";
  joint_name[4].data = "left_j2";

  jtp0.points.resize(1);                // ポーズは一つだけ

  jtp0.points[0].positions.resize(5); // ポジションを1個設定  
  jtp0.points[0].velocities.resize(5); // ポジションを5個設定  
  jtp0.points[0].accelerations.resize(5); // ポジションを5個設定  
  jtp0.points[0].effort.resize(5); // ポジションを5個設定  

  for(i=0;i<5;i++){
    jtp0.points[0].positions[i] = 0.0;
  }

  jtp0.points[0].time_from_start = ros::Duration(0.1);  //実行時間20msec (10msecだと動作しない;;)

  while (ros::ok()) { // 無限ループ

    jtp0.header.stamp = ros::Time::now();
    
    jtp0.points[0].positions[0] = joint_pos[0].data;  // 右から右
    jtp0.points[0].positions[1] = joint_pos[1].data;  // 右から右
    jtp0.points[0].positions[2] = joint_pos[2].data;  // 右から右
    jtp0.points[0].positions[3] = joint_pos[3].data;  // 右から左(マスター＆スレーブ)
    jtp0.points[0].positions[4] = joint_pos[4].data;  // 右から左(マスター＆スレーブ)

    arm_pub.publish(jtp0);

    //ROS_INFO("torikometeru? = %f",joint_pos[1].data);
    ros::spinOnce();   // コールバック関数を呼ぶ
    loop_rate.sleep();  // 待ち
  }
  
  return 0;
}