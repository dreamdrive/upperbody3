# upperbody3
 
とりあえず、Dynamixel workbenchを立ち上げて、いろいろうごく。 
上半身ロボット。

move itからも動かす。

■ MoveIT!で実機連動のプランニング

~~~
$ roslaunch roslaunch upperbody3_moveit_config demo.launch
$ roslaunch upperbody3_description dynamixel_controllers.launch
$ rosrun upperbody3_control moveit2dynamixel
~~~


■ RVIZと実機でデジタルツイン

~~~
$ roslaunch upperbody3_description display_unjoint.launch
$ roslaunch upperbody3_description dynamixel_controllers_twin.launch
~~~

すべてのサーボ脱力

~~~
$ rosservice call /dynamixel_workbench/dynamixel_command '' 3 'Torque_Enable' 0
$ rosservice call /dynamixel_workbench/dynamixel_command '' 4 'Torque_Enable' 0
$ rosservice call /dynamixel_workbench/dynamixel_command '' 5 'Torque_Enable' 0
$ rosservice call /dynamixel_workbench/dynamixel_command '' 6 'Torque_Enable' 0
$ rosservice call /dynamixel_workbench/dynamixel_command '' 8 'Torque_Enable' 0
~~~

■ RVIZでバーチャルなロボットを動かす

~~~
$ roslaunch upperbody3_description display_unjoint.launch
~~~

■ 左右のマスタースレーブ

~~~
$ roslaunch upperbody3_description dynamixel_controllers.launch
$ rosrun upperbody3_control dynamixel2dynamixel
~~~

3と4のサーボを脱力
~~~
$ rosservice call /dynamixel_workbench/dynamixel_command '' 3 'Torque_Enable' 0
$ rosservice call /dynamixel_workbench/dynamixel_command '' 4 'Torque_Enable' 0
~~~

■ カメラによる顔追従(D435)

~~~
$ roslaunch realsense2_camera rs_camera.launch
$ roslaunch opencv_apps face_detection.launch image:=/camera/color/image_raw
$ roslaunch upperbody3_description dynamixel_controllers.launch
$ rosrun upperbody3_control dynamixel_face
~~~

