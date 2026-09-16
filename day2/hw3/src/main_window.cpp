#include "../include/ros2_day2_hw3/main_window.hpp"
#include <QPushButton>
#include <QString>

// UI 세팅, 라벨 배열 연결, QNode 생성 및 신호 연결
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
  ui->setupUi(this);

  value_labels_[0] = ui->topic1_value;
  value_labels_[1] = ui->topic2_value;
  value_labels_[2] = ui->topic3_value;

  state_labels_[0] = ui->topic1_state;
  state_labels_[1] = ui->topic2_state;
  state_labels_[2] = ui->topic3_state;

  connect(ui->restart_button, &QPushButton::clicked, this, &MainWindow::onRestartClicked);

  qnode = new QNode();

  QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));
  QObject::connect(qnode, SIGNAL(topicValueUpdated(int, QString)), this,
                    SLOT(onTopicValueUpdated(int, QString)));
  QObject::connect(qnode, SIGNAL(topicStateChanged(int, bool)), this,
                    SLOT(onTopicStateChanged(int, bool)));
  QObject::connect(qnode, SIGNAL(restartResult(bool, QString)), this,
                    SLOT(onRestartResult(bool, QString)));

  for (int i = 0; i < TOPIC_COUNT; ++i)
  {
    onTopicStateChanged(i, false);
  }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{
  delete ui;
}

// 토픽의 최신 값을 라벨에 표시
void MainWindow::onTopicValueUpdated(int idx, QString value)
{
  value_labels_[idx]->setText(value);
}

// 토픽 생존 상태에 따라 상태 라벨 텍스트/색상 갱신
void MainWindow::onTopicStateChanged(int idx, bool alive)
{
  if (alive)
  {
    state_labels_[idx]->setText("정상");
    state_labels_[idx]->setStyleSheet("color: green;");
  }
  else
  {
    state_labels_[idx]->setText("응답 없음");
    state_labels_[idx]->setStyleSheet("color: red;");
    value_labels_[idx]->setText("응답 없음");
  }
}

// 재시작 버튼 클릭: 서비스 호출 요청
void MainWindow::onRestartClicked()
{
  ui->restart_result->setText("요청 중...");
  qnode->requestRestart();
}

// 재시작 서비스 응답 결과 표시 (3초 후 자동으로 지움)
void MainWindow::onRestartResult(bool success, QString message)
{
  ui->restart_result->setText((success ? "성공: " : "실패: ") + message);

  QTimer::singleShot(3000, this, [this]() { ui->restart_result->setText("대기 중"); });
}
