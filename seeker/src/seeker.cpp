#include "ros/ros.h"
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <std_srvs/SetBool.h>
#include "nav_msgs/Odometry.h"
#include "gazebo_msgs/ModelStates.h"
#include "gazebo_msgs/ModelState.h"
#include <vector>
#include <string>
#include <geometry_msgs/Vector3.h>

// Global Variable
bool enableService = false;

// ROS Service Call "enable"
bool enable(std_srvs::SetBool::Request &req, std_srvs::SetBool::Response &res){
  if (req.data == true){
    enableService = true;
  } else if (req.data == false){
    enableService = false;
  }
  return enableService;
}

// Class to provide information on object
class ObjectMonitor {
  public:
    ObjectMonitor(const ros::Publisher& displacement_publisher, const ros::Publisher& position_publisher) : distance_pub_object(displacement_publisher), position_pub_object(position_publisher){
    }

    // Function that subscribes to /gazebo_msgs/model_states and publishes calculated object information
    void modelStatesCallback(const gazebo_msgs::ModelStates::ConstPtr& msg){
	
      double object_x = msg->pose[1].position.x;
      double object_y = msg->pose[1].position.y;
      double object_z = msg->pose[1].position.z;

      double turtlebot_x = msg->pose[2].position.x;
      double turtlebot_y = msg->pose[2].position.y;
      double turtlebot_z = msg->pose[2].position.z;

      geometry_msgs::Vector3 displacement_topic_info = displacementFunction(object_x, object_y, object_z, turtlebot_x, turtlebot_y, turtlebot_z);
      
      // Publish displacement topic information
      distance_pub_object.publish(displacement_topic_info);

      if (enableService == true){
        // Publish new position
        gazebo_msgs::ModelState position_info = setPosition(object_x, object_y);
        position_pub_object.publish(position_info);
      }
    }

  private:
    ros::Publisher distance_pub_object;
    ros::Publisher position_pub_object;

    // Function to calculate displacement information
    geometry_msgs::Vector3 displacementFunction (double object_x, double object_y, double object_z, double turtlebot_x, double turtlebot_y, double turtlebot_z){
      geometry_msgs::Vector3 result;
      result.x = object_x - turtlebot_x;
      result.y = object_y - turtlebot_y;
      result.z = object_z - turtlebot_z;
      return result;
    }

    // Function to set turtlebot's new position
    gazebo_msgs::ModelState setPosition (double x, double y){
      gazebo_msgs::ModelState position;
      position.model_name = "mobile_base";
      position.pose.position.x = x;
      position.pose.position.y = y;
      return position;
    }

};

int main(int argc, char **argv){
  ros::init(argc, argv, "seeker");
  ros::NodeHandle nh;
	
  // Create displacement publisher
  ros::Publisher displacement_publisher = nh.advertise<geometry_msgs::Vector3>("displacement", 1);

  // Create position publisher
  ros::Publisher position_publisher = nh.advertise<gazebo_msgs::ModelState>("gazebo/set_model_state", 1);

  // Create Service Server
  ros::ServiceServer service = nh.advertiseService("enable", enable);

  ObjectMonitor monitor(displacement_publisher, position_publisher);

  // Create Subscriber
  ros::Subscriber sub = nh.subscribe("/gazebo/model_states", 1, &ObjectMonitor::modelStatesCallback, &monitor);

  ROS_INFO("Displacement: ");

  ros::Rate r(1.0);
  
  ros::spin();
  return 0;
}
