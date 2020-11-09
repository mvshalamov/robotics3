#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>


// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv)) {
      ROS_ERROR("failed to call service");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool left = false;
    bool right = false;
    bool forward = false;
    for(int i = 0; i < img.height; i++) {
      for(int j = 0; j < img.step; j = j + 4) {
        if (img.data[i*img.step+j] == white_pixel && img.data[i*img.step+j+1] == white_pixel && img.data[i*img.step+j+2] == white_pixel) {
          if (j <= 400) {
            left = true;
ROS_INFO("img go left:%1.2f", (float)j);
            break;
          } else if (j >= 2000) {
            right = true;
ROS_INFO("img go right:%1.2f", (float)j);
            break;
          } else {
            forward = true;
          }
        }
      
      }
      if (left == true || right == true) {
        break;
      }
    }
    if (left == false && right == false && forward == false) {
      ROS_INFO("img go stop:%1.2f,", (float)1);
      drive_robot(0.0, 0.0);
    } else {
      ROS_INFO("img go %1.2f, %1.2f, %1.2f", (float)left, (float)forward, (float)right );
      if (left) {
        drive_robot(-0.2, 0.0);
      } else if (right) {
        drive_robot(0.2, 0.0);
      } else {
        drive_robot(0.0, -0.4);
      }
    }

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
