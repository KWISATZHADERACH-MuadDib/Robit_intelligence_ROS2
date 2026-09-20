#include "position_classifier.hpp"

#include <cmath>

namespace vision
{
namespace
{
// 콘이 선 위에 있다고 판단하는 여유: 선 굵기/2 + 바운딩박스 반폭 * 이 값
constexpr float kOnLineBoxRatio = 0.5f;

bool isVertical(const LineInfo & l)
{
  return std::fabs(l.fit[1]) > std::fabs(l.fit[0]);
}

// 선에 수직인 방향(법선)으로 콘 중심까지의 부호 있는 거리를 구해 -1/0/+1 판별
//  vertical_line=true (세로선): 음수=왼쪽, 양수=오른쪽
//  vertical_line=false (가로선): 음수=위쪽, 양수=아래쪽
int sideOfLine(const ConeInfo & cone, const LineInfo & line, bool vertical_line)
{
  float vx = line.fit[0], vy = line.fit[1];
  const float x0 = line.fit[2], y0 = line.fit[3];

  // 방향 벡터를 통일: 세로선은 아래쪽, 가로선은 오른쪽을 향하게
  if ((vertical_line && vy < 0) || (!vertical_line && vx < 0)) {
    vx = -vx;
    vy = -vy;
  }

  const float cx = cone.box.x + cone.box.width * 0.5f;
  const float cy = cone.box.y + cone.box.height * 0.5f;

  // 법선 성분: 세로선(방향 아래) -> +x가 오른쪽, 가로선(방향 오른쪽) -> +y가 아래쪽
  const float nx = vertical_line ? vy : -vy;
  const float ny = vertical_line ? -vx : vx;
  const float d = nx * (cx - x0) + ny * (cy - y0);

  // 바운딩박스가 법선 방향으로 차지하는 반폭
  const float half_extent =
    0.5f * (cone.box.width * std::fabs(nx) + cone.box.height * std::fabs(ny));
  const float tolerance = line.thickness * 0.5f + half_extent * kOnLineBoxRatio;

  if (std::fabs(d) <= tolerance) {
    return 0;
  }
  return d < 0 ? -1 : 1;
}
}  // namespace

LineRoles assignLineRoles(const LineInfo & white, const LineInfo & blue, cv::Size frame_size)
{
  bool white_is_vertical = true;
  if (white.found && blue.found) {
    white_is_vertical = std::fabs(white.fit[1]) > std::fabs(blue.fit[1]);
  } else if (white.found) {
    white_is_vertical = isVertical(white);
  } else if (blue.found) {
    white_is_vertical = !isVertical(blue);
  }

  const float cx = frame_size.width * 0.5f;
  const float cy = frame_size.height * 0.5f;

  LineRoles roles;
  roles.vertical = white_is_vertical ? white : blue;
  roles.horizontal = white_is_vertical ? blue : white;
  if (!roles.vertical.found) {
    roles.vertical.fit = cv::Vec4f(0.f, 1.f, cx, cy);
  }
  if (!roles.horizontal.found) {
    roles.horizontal.fit = cv::Vec4f(1.f, 0.f, cx, cy);
  }
  return roles;
}

ConePosition classifyCone(const ConeInfo & cone, const LineRoles & roles)
{
  ConePosition pos;
  if (!cone.found) {
    return pos;
  }

  const int h = sideOfLine(cone, roles.vertical, true);
  const int v = sideOfLine(cone, roles.horizontal, false);

  pos.found = true;
  pos.horizontal = h < 0 ? Horizontal::Left : (h > 0 ? Horizontal::Right : Horizontal::Center);
  pos.vertical = v < 0 ? Vertical::Up : (v > 0 ? Vertical::Down : Vertical::Center);
  return pos;
}

}  // namespace vision
