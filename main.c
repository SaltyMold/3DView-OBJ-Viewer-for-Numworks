#include "eadk.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define WIDTH 320
#define HEIGHT 240
#define FOV 5.0f

#define FMT_FLOAT(x) (int)(x), (int)(abs((int)(fabsf((x) * 100)) % 100))

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "3DView";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

typedef struct {
  float x, y, z;
} Vec3;

Vec3 *points = NULL;
int (*edges)[2] = NULL;
int NB_POINTS = 0;
int NB_EDGES = 0;

void load_external_data() {
  const unsigned char *data = (const unsigned char*)eadk_external_data;
  int offset = 0;
  int32_t nb_points = 0, nb_edges = 0;
  memcpy(&nb_points, data + offset, 4); offset += 4;
  memcpy(&nb_edges, data + offset, 4); offset += 4;
  NB_POINTS = nb_points;
  NB_EDGES = nb_edges;

  points = malloc(NB_POINTS * sizeof(Vec3));
  edges = malloc(NB_EDGES * sizeof(int[2]));

  memcpy(points, data + offset, NB_POINTS * sizeof(Vec3));
  offset += NB_POINTS * sizeof(Vec3);
  memcpy(edges, data + offset, NB_EDGES * sizeof(int[2]));
}

void free_external_data() {
  free(points);
  free(edges);
}

void project(Vec3 point, int *x, int *y, float scale) {
  float factor = FOV / (FOV + point.z);
  *x = (int)(point.x * factor * scale + WIDTH / 2);
  *y = (int)(-point.y * factor * scale + HEIGHT / 2);
}

Vec3 camera_transform(Vec3 p, float cam_theta, float cam_phi, float cam_dist, float cx, float cy, float cz) {
  float ex = cx + cam_dist * cosf(cam_phi) * sinf(cam_theta);
  float ey = cy + cam_dist * sinf(cam_phi);
  float ez = cz + cam_dist * cosf(cam_phi) * cosf(cam_theta);

  Vec3 eye = {ex, ey, ez};
  Vec3 pt = {p.x - eye.x, p.y - eye.y, p.z - eye.z};

  float up_x = 0, up_y = 1, up_z = 0;
  float zx = cx - ex, zy = cy - ey, zz = cz - ez;
  float zn = sqrtf(zx*zx + zy*zy + zz*zz);
  zx /= zn; zy /= zn; zz /= zn;
  float xx = up_y*zz - up_z*zy;
  float xy = up_z*zx - up_x*zz;
  float xz = up_x*zy - up_y*zx;
  float xn = sqrtf(xx*xx + xy*xy + xz*xz);
  xx /= xn; xy /= xn; xz /= xn;
  float yx = zy*xz - zz*xy;
  float yy = zz*xx - zx*xz;
  float yz = zx*xy - zy*xx;

  return (Vec3){
    pt.x*xx + pt.y*xy + pt.z*xz,
    pt.x*yx + pt.y*yy + pt.z*yz,
    pt.x*zx + pt.y*zy + pt.z*zz
  };
}

void camera_axes(float cam_theta, float cam_phi, float *forward, float *right, float *up) {
  forward[0] = cosf(cam_phi) * sinf(cam_theta);
  forward[1] = sinf(cam_phi);
  forward[2] = cosf(cam_phi) * cosf(cam_theta);

  right[0] = cosf(cam_theta);
  right[1] = 0.0f;
  right[2] = -sinf(cam_theta);

  up[0] = -sinf(cam_phi) * sinf(cam_theta);
  up[1] = cosf(cam_phi);
  up[2] = -sinf(cam_phi) * cosf(cam_theta);
}

void draw_line(int x0, int y0, int x1, int y1, eadk_color_t color) {
  int dx = abs(x1 - x0), dy = abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;
  while (true) {
    if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT) {
      eadk_display_push_rect((eadk_rect_t){x0, y0, 1, 1}, &color);
    }
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 > -dy) { err -= dy; x0 += sx; }
    if (e2 < dx) { err += dx; y0 += sy; }
  }
}

void screen(float cam_theta, float cam_phi, float cam_dist, float scale, float cx, float cy, float cz) {
    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);

    Vec3 transformed[NB_POINTS];
    int projected[NB_POINTS][2];
    for (int i = 0; i < NB_POINTS; i++) {
      Vec3 v = camera_transform(points[i], cam_theta, cam_phi, cam_dist, cx, cy, cz);
      transformed[i] = v;
      project(transformed[i], &projected[i][0], &projected[i][1], scale);
    }

    for (int i = 0; i < NB_EDGES; i++) {
      int a = edges[i][0], b = edges[i][1];
      draw_line(projected[a][0], projected[a][1],
                projected[b][0], projected[b][1],
                eadk_color_black);
    }
}

int main() {
  eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
  eadk_backlight_set_brightness(255);
  eadk_display_draw_string("Loading...", (eadk_point_t){10, 10}, false, eadk_color_black, eadk_color_white);

  load_external_data();

  char buf[64];
  snprintf(buf, sizeof(buf), "Points: %d, Edges: %d", NB_POINTS, NB_EDGES);
  eadk_display_draw_string(buf, (eadk_point_t){10, 30}, false, eadk_color_black, eadk_color_white);
  eadk_timing_msleep(1000);

  float cam_theta = 0.0f;
  float cam_phi = 0.0f;
  float cam_dist = 10.0f;
  float scale = 50.0f;
  float center_x = 0.0f, center_y = 0.0f, center_z = 0.0f;

  screen(cam_theta, cam_phi, cam_dist, scale, center_x, center_y, center_z);

  bool is_debug = false;

  while (true) {
    eadk_keyboard_state_t keys = eadk_keyboard_scan();
    bool redraw = false;

    if (eadk_keyboard_key_down(keys, eadk_key_imaginary)) {
      cam_theta += 0.05f;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_power)) {
      cam_theta -= 0.05f;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_toolbox)) {
      cam_phi += 0.05f;
      if (cam_phi > 1.5f) cam_phi = 1.5f;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_sqrt)) {
      cam_phi -= 0.05f;
      if (cam_phi < -1.5f) cam_phi = -1.5f;
      redraw = true;
    }

    float move_speed = 0.2f;
    float forward[3], right[3], up[3];
    camera_axes(cam_theta, cam_phi, forward, right, up);

    if (eadk_keyboard_key_down(keys, eadk_key_up)) {
      center_x += up[0] * move_speed;
      center_y += up[1] * move_speed;
      center_z += up[2] * move_speed;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_down)) {
      center_x -= up[0] * move_speed;
      center_y -= up[1] * move_speed;
      center_z -= up[2] * move_speed;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_left)) {
      center_x += right[0] * move_speed;
      center_y += right[1] * move_speed;
      center_z += right[2] * move_speed;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_right)) {
      center_x -= right[0] * move_speed;
      center_y -= right[1] * move_speed;
      center_z -= right[2] * move_speed;
      redraw = true;
    }

    if (eadk_keyboard_key_down(keys, eadk_key_ok)) {
      scale *= 1.05f;
      redraw = true;
    }
    if (eadk_keyboard_key_down(keys, eadk_key_back)) {
      scale /= 1.05f;
      redraw = true;
    }

    if (eadk_keyboard_key_down(keys, eadk_key_home)) {
      break;
    }

    if (eadk_keyboard_key_down(keys, eadk_key_shift)) {
      is_debug = !is_debug;
      redraw = true;
      while (eadk_keyboard_scan() != 0) eadk_timing_msleep(100);
    }

    if (redraw) {
      screen(cam_theta, cam_phi, cam_dist, scale, center_x, center_y, center_z);
    }

    if (is_debug) {
      char buf[128];
      snprintf(buf, sizeof(buf),
        "Cam: theta=%d.%02d, phi=%d.%02d, scale=%d.%02d\nCenter: (%d.%02d, %d.%02d, %d.%02d)",
        FMT_FLOAT(cam_theta), FMT_FLOAT(cam_phi), FMT_FLOAT(scale),
        FMT_FLOAT(center_x), FMT_FLOAT(center_y), FMT_FLOAT(center_z));
      eadk_display_draw_string(buf, (eadk_point_t){0, 10}, false, eadk_color_black, eadk_color_white);
    }

    eadk_timing_msleep(30);
  }

  free_external_data();
  return 0;
}