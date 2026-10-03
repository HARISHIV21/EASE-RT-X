// Linux/ROS2 integration point for EASE-RT X.
// Built only in the ROS2 environment; the Windows FreeRTOS host build does not require ROS2.
#ifdef EASE_RT_X_WITH_ROS2
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
class EaseRtAutonomyNode : public rclcpp::Node {
public:
    EaseRtAutonomyNode():Node("ease_rt_autonomy") { pub_=create_publisher<std_msgs::msg::String>("/ease_rt_x/health",10); timer_=create_wall_timer(std::chrono::milliseconds(100),[this]{std_msgs::msg::String m;m.data="EASE-RT X autonomy heartbeat";pub_->publish(m);}); }
private:rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;rclcpp::TimerBase::SharedPtr timer_;
};
int main(int argc,char**argv){rclcpp::init(argc,argv);rclcpp::spin(std::make_shared<EaseRtAutonomyNode>());rclcpp::shutdown();return 0;}
#endif
