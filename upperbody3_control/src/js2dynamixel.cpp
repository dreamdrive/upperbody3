#include "ros/ros.h"
#include "ros/time.h"

#include "std_msgs/Float64.h"
#include "std_msgs/String.h"

#include "trajectory_msgs/JointTrajectory.h"
#include "sensor_msgs/JointState.h"

#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>

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
        //joint_vel[i].data = jointstate->velocity[j];    // 速度読み出し
        //joint_eff[i].data = jointstate->effort[j];    // 負荷読み出し
      }
    }
  }
}

int main(int argc, char **argv)
{
  int i,j;

  const char* port_name = "/dev/ttyUSB0";
  int baud_rate = 1000000;

  uint16_t model_number = 0;
  uint8_t dxl_id[5] = {8, 3, 4, 5, 6};

  DynamixelWorkbench dxl_wb;

  const char *log;
  bool result = false;

  int32_t goal_position[5] = {2048, 2048, 2048, 2048, 2048};
  const uint8_t handler_index = 0;

  joint_name[0].data = "waist";
  joint_name[1].data = "right_j1";
  joint_name[2].data = "right_j2";
  joint_name[3].data = "left_j1";
  joint_name[4].data = "left_j2";

  // メイン関数の引数処理
  if (argc < 2)
  {
    printf("Please set '-port_name' arguments for connected Dynamixels\n");
    return 0;
  }
  else
  {
    port_name = argv[1];
  }

  // Dynamixel Worlbench 初期化
  result = dxl_wb.init(port_name, baud_rate, &log);
  if (result == false)
  {
    printf("%s\n", log);
    printf("Failed to init\n");

    return 0;
  }
  else
    printf("Succeed to init(%d)\n", baud_rate);  

  // Dynamixel 初期化
  for (int cnt = 0; cnt < 5; cnt++)
  {
    result = dxl_wb.ping(dxl_id[cnt], &model_number, &log);

    if (result == false)
    {
      printf("%s\n", log);
      printf("Failed to ping\n");
    }
    else
    {
      printf("Succeeded to ping\n");
      printf("id : %d, model_number : %d\n", dxl_id[cnt], model_number);
    }

    // 2XLサーボが交じるとここでエラー吐くのでコメントアウト
    //
    // result = dxl_wb.jointMode(dxl_id[cnt], 0, 0, &log);
    // if (result == false)
    // {
    //   printf("%s\n", log);
    //   printf("Failed to change joint mode\n");
    // }
    // else
    // {
    //   printf("Succeed to change joint mode\n");
    // }
    
    // とりあえず、トルクオンだけしてお茶を濁す
    result = dxl_wb.torqueOn(dxl_id[cnt]);
    if (result == false) printf("torqueOn ERROR \n");
    
  }

  // syncライトパケットのヘッダ生成
  result = dxl_wb.addSyncWriteHandler(dxl_id[0], "Goal_Position", &log);
  if (result == false)
  {
    printf("%s\n", log);
    printf("Failed to add sync write handler\n");
  }

  //--------------------------------------------------------------------------------------------------------

  result = dxl_wb.syncWrite(handler_index, &goal_position[0], &log);
  if (result == false)
  {
    printf("%s\n", log);
    printf("Failed to sync write position\n");
  }






  ros::init(argc, argv, "js2dynamixel");   // ノードの初期化
  ros::NodeHandle nh;                             // ノードハンドラ

  ros::Subscriber sub_joints;                     // サブスクライバの作成
  sub_joints = nh.subscribe("/skeleton/joint_states", 1, monitorJointState_callback);

  //ros::Publisher arm_pub;                         //パブリッシャの作成
  //arm_pub = nh.advertise<trajectory_msgs::JointTrajectory>("/dynamixel_workbench/joint_trajectory",1);

  ros::Rate loop_rate(100);  // 制御周期45Hz(サーボの更新が20msecなのですが、50Hzだと余裕ない感じ)

  //trajectory_msgs::JointTrajectory jtp0;
  
  // jtp0.header.frame_id = "base_link";   // ポーズ名（モーション名)

  // jtp0.joint_names.resize(5); // 名前
  // jtp0.joint_names[0] ="waist";
  // jtp0.joint_names[1] ="right_j1";
  // jtp0.joint_names[2] ="right_j2";
  // jtp0.joint_names[3] ="left_j1";
  // jtp0.joint_names[4] ="left_j2";
  
  joint_name[0].data = "waist";
  joint_name[1].data = "right_j1";
  joint_name[2].data = "right_j2";
  joint_name[3].data = "left_j1";
  joint_name[4].data = "left_j2";

  // jtp0.points.resize(1);                // ポーズは一つだけ

  // jtp0.points[0].positions.resize(5); // ポジションを1個設定  
  // jtp0.points[0].velocities.resize(5); // ポジションを5個設定  
  // jtp0.points[0].accelerations.resize(5); // ポジションを5個設定  
  // jtp0.points[0].effort.resize(5); // ポジションを5個設定  

  // for(i=0;i<5;i++){
  //   jtp0.points[0].positions[i] = 0.0;
  // }

  // jtp0.points[0].time_from_start = ros::Duration(0.02);  //実行時間20msec (10msecだと動作しない;;)

  while (ros::ok()) { // 無限ループ

  //   jtp0.header.stamp = ros::Time::now();
    
  //   jtp0.points[0].positions[0] = 0.0;  // 腰から腰
  //   jtp0.points[0].positions[1] = joint_pos[1].data;  // 右から右
  //   jtp0.points[0].positions[2] = joint_pos[2].data;  // 右から右
  //   jtp0.points[0].positions[3] = -joint_pos[1].data;  // 右から左(マスター＆スレーブ)
  //   jtp0.points[0].positions[4] = joint_pos[2].data;  // 右から左(マスター＆スレーブ)

  //  // jtp0.points[0].velocities[3] = 0.05;  // 右から左(マスター＆スレーブ)
  //  // jtp0.points[0].velocities[4] = 0.02;  // 右から左(マスター＆スレーブ)

  //   arm_pub.publish(jtp0);

    // ROS_INFO("torikometeru? = %f",joint_pos[1].data);





  for(i=0;i<5;i++){
    goal_position[i] = 2047 + 2048 * ( joint_pos[i].data / (2 * M_PI) ) ;
  }



  result = dxl_wb.syncWrite(handler_index, &goal_position[0], &log);
  if (result == false)
  {
    printf("%s\n", log);
    printf("Failed to sync write position\n");
  }





    ros::spinOnce();   // コールバック関数を呼ぶ
    loop_rate.sleep();  // 待ち
  }
  
  return 0;
}