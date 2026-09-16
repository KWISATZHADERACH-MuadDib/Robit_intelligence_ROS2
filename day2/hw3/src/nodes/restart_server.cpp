#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>

#include <csignal>
#include <climits>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

// restart_server 자신의 실행 파일 경로를 이용해 같은 디렉터리에 설치된
// topic_test_publisher의 절대 경로를 계산한다.
std::string siblingExecutablePath(const std::string& name)
{
  char buf[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (len <= 0)
  {
    return name;
  }
  buf[len] = '\0';
  std::string self_path(buf);
  auto slash = self_path.find_last_of('/');
  std::string dir = (slash == std::string::npos) ? "." : self_path.substr(0, slash);
  return dir + "/" + name;
}

// topic_test_publisher를 자식 프로세스로 실행/관리하면서,
// "/restart_request" 서비스 요청이 오면 실제로 프로세스를 종료 후 재실행한다.
class RestartServer : public rclcpp::Node
{
public:
  RestartServer() : Node("restart_server"), child_pid_(-1)
  {
    publisher_path_ = siblingExecutablePath("topic_test_publisher");

    spawnChild();

    service_ = create_service<std_srvs::srv::Trigger>(
        "/restart_request",
        std::bind(&RestartServer::handleRestart, this, std::placeholders::_1, std::placeholders::_2));

    RCLCPP_INFO(get_logger(), "/restart_request 서비스 대기 중... (topic_test_publisher pid=%d)", child_pid_);
  }

  ~RestartServer() override
  {
    killChild();
  }

private:
  pid_t child_pid_;
  std::string publisher_path_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr service_;

  // topic_test_publisher 자식 프로세스를 fork + exec로 실행
  void spawnChild()
  {
    pid_t pid = fork();
    if (pid == 0)
    {
      execl(publisher_path_.c_str(), publisher_path_.c_str(), (char*)nullptr);
      _exit(127);  // exec 실패 시에만 도달
    }
    child_pid_ = pid;
  }

  // 자식 프로세스에 SIGTERM을 보내고 종료될 때까지 대기
  void killChild()
  {
    if (child_pid_ > 0)
    {
      kill(child_pid_, SIGTERM);
      waitpid(child_pid_, nullptr, 0);
      child_pid_ = -1;
    }
  }

  // 재시작 요청 처리: 기존 topic_test_publisher를 실제로 종료하고 새로 실행
  void handleRestart(const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
                      std::shared_ptr<std_srvs::srv::Trigger::Response> response)
  {
    (void)request;
    RCLCPP_INFO(get_logger(), "재시작 요청 수신 - topic_test_publisher(pid=%d) 종료", child_pid_);

    killChild();
    spawnChild();

    RCLCPP_INFO(get_logger(), "topic_test_publisher 재실행 완료 (pid=%d)", child_pid_);

    response->success = true;
    response->message = "topic_test_publisher가 재시작되었습니다";
  }
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RestartServer>());
  rclcpp::shutdown();
  return 0;
}
