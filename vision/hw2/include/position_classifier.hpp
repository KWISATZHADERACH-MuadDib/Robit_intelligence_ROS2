#ifndef POSITION_CLASSIFIER_HPP_
#define POSITION_CLASSIFIER_HPP_

#include <opencv2/core.hpp>

#include "detection_types.hpp"

namespace vision
{

// 세로선/가로선 역할이 정해진 두 기준선
struct LineRoles
{
  LineInfo vertical;    // 콘의 좌/우 판별 기준
  LineInfo horizontal;  // 콘의 위/아래 판별 기준
};

// 매트를 놓은 방향과 상관없이 더 세로에 가까운 선을 세로선으로 취급.
// 하나만 보이면 그 선의 방향으로 판단하고, 안 보이는 선은 화면 중앙을 지나는 선으로 대체.
LineRoles assignLineRoles(const LineInfo & white, const LineInfo & blue, cv::Size frame_size);

// 콘이 두 기준선에 대해 어느 위치(9분할)에 있는지 판별
ConePosition classifyCone(const ConeInfo & cone, const LineRoles & roles);

}  // namespace vision

#endif  // POSITION_CLASSIFIER_HPP_
