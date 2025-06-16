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

int NB_POINTS = 0;
int NB_EDGES = 0;

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

void screen_batch(
  Vec3 *points, int nb_points,
  int (*edges)[2], int nb_edges,
  float cam_theta, float cam_phi, float cam_dist, float scale, float cx, float cy, float cz,
  int point_offset
) {
  Vec3 *transformed = malloc(nb_points * sizeof(Vec3));
  int (*projected)[2] = malloc(nb_points * sizeof(int[2]));
  if (!transformed || !projected) {
    eadk_display_draw_string("Erreur alloc batch", (eadk_point_t){10, 50}, false, eadk_color_red, eadk_color_white);
    while (1);
  }

  for (int i = 0; i < nb_points; i++) {
    Vec3 v = camera_transform(points[i], cam_theta, cam_phi, cam_dist, cx, cy, cz);
    transformed[i] = v;
    project(transformed[i], &projected[i][0], &projected[i][1], scale);
  }

  for (int i = 0; i < nb_edges; i++) {
    int a = edges[i][0] - point_offset;
    int b = edges[i][1] - point_offset;
    if (a >= 0 && a < nb_points && b >= 0 && b < nb_points) {
      draw_line(projected[a][0], projected[a][1],
                projected[b][0], projected[b][1],
                eadk_color_black);
    }
  }

  free(transformed);
  free(projected);
}

void screen_batches_dynamic(
  const unsigned char *data, int32_t nb_points, int32_t nb_edges,
  float cam_theta, float cam_phi, float cam_dist, float scale, float cx, float cy, float cz
) {
  int offset_points = 8;
  int offset_edges = 8 + nb_points * sizeof(Vec3);

  int points_done = 0;
  const int BATCH_POINTS = 1500; 

  Vec3 *points = malloc(BATCH_POINTS * sizeof(Vec3));
  int (*edges_batch)[2] = malloc(BATCH_POINTS * 8 * sizeof(int[2])); 
  if (!points || !edges_batch) {
    eadk_display_draw_string("Erreur alloc batch", (eadk_point_t){10, 50}, false, eadk_color_red, eadk_color_white);
    while (1);
  }

  while (points_done < nb_points) {
    int batch_points = (points_done + BATCH_POINTS < nb_points) ? BATCH_POINTS : (nb_points - points_done);
    memcpy(points, data + offset_points + points_done * sizeof(Vec3), batch_points * sizeof(Vec3));

    int nb_edges_batch = 0;
    for (int i = 0; i < nb_edges; i++) {
      int a, b;
      memcpy(&a, data + offset_edges + i * sizeof(int[2]), sizeof(int));
      memcpy(&b, data + offset_edges + i * sizeof(int[2]) + sizeof(int), sizeof(int));
      if (a >= points_done && a < points_done + batch_points &&
          b >= points_done && b < points_done + batch_points) {
        if (nb_edges_batch < BATCH_POINTS * 8) {
          edges_batch[nb_edges_batch][0] = a;
          edges_batch[nb_edges_batch][1] = b;
          nb_edges_batch++;
        }
      }
    }

    screen_batch(points, batch_points, edges_batch, nb_edges_batch,
                 cam_theta, cam_phi, cam_dist, scale, cx, cy, cz, points_done);

    points_done += batch_points;
  }

  free(points);
  free(edges_batch);
}

int main() {
  eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
  eadk_backlight_set_brightness(255);
  eadk_display_draw_string("Loading...", (eadk_point_t){10, 10}, false, eadk_color_black, eadk_color_white);

  const unsigned char *data = (const unsigned char*)eadk_external_data;
  int32_t nb_points = 0, nb_edges = 0;
  memcpy(&nb_points, data, 4);
  memcpy(&nb_edges, data + 4, 4);
  NB_POINTS = nb_points;
  NB_EDGES = nb_edges;

  float cam_theta = 0.0f;
  float cam_phi = 0.0f;
  float cam_dist = 10.0f;
  float scale = 50.0f;
  float center_x = 0.0f, center_y = 0.0f, center_z = 0.0f;

  char buf[64];
  snprintf(buf, sizeof(buf), "Points: %d, Edges: %d", NB_POINTS, NB_EDGES);
  eadk_display_draw_string(buf, (eadk_point_t){10, 30}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press shift to change camera distance", (eadk_point_t){10, 50}, false, eadk_color_black, eadk_color_white);
  for (int i = 0; i < 10; i++) {
    char buf[32];
    snprintf(buf, "%d ms", 1000 - 100 * i);
    eadk_display_draw_string(buf, (eadk_point_t){10, 200}, false, eadk_color_black, eadk_color_white);
    eadk_timing_msleep(100);
    eadk_keyboard_state_t keys = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keys, eadk_key_shift)) {
      eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
      eadk_display_draw_string("Chooce camera distance ", (eadk_point_t){10, 10}, false, eadk_color_black, eadk_color_white);
      eadk_display_draw_string("If the camera is in the model", (eadk_point_t){10, 30}, false, eadk_color_black, eadk_color_white);
      eadk_display_draw_string("Right/left arrow to increase/decrease", (eadk_point_t){10, 50}, false, eadk_color_black, eadk_color_white);
      eadk_display_draw_string("or up/down to increase/decrease * 10", (eadk_point_t){10, 70}, false, eadk_color_black, eadk_color_white);
      eadk_display_draw_string("Press ok to continue", (eadk_point_t){10, 90}, false, eadk_color_black, eadk_color_white);
      snprintf(buf, sizeof(buf), "Camera distance: %d.%02d", (int)cam_dist, (int)(fabsf(cam_dist * 100)) % 100);
      eadk_display_draw_string(buf, (eadk_point_t){10, 130}, false, eadk_color_black, eadk_color_white);
      while (1){
        eadk_keyboard_state_t keys = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(keys, eadk_key_home)) {
          return 0;
        }
        if (eadk_keyboard_key_down(keys, eadk_key_right)) {
          cam_dist += 0.5f;
          snprintf(buf, sizeof(buf), "Camera distance: %d.%02d", (int)cam_dist, (int)(fabsf(cam_dist * 100)) % 100);
          eadk_display_draw_string(buf, (eadk_point_t){10, 130}, false, eadk_color_black, eadk_color_white);
          eadk_timing_msleep(50);
        }
        if (eadk_keyboard_key_down(keys, eadk_key_left)) {
          cam_dist -= 0.5f;
          if (cam_dist < 0.5f) cam_dist = 0.5f;
          snprintf(buf, sizeof(buf), "Camera distance: %d.%02d", (int)cam_dist, (int)(fabsf(cam_dist * 100)) % 100);
          eadk_display_draw_string(buf, (eadk_point_t){10, 130}, false, eadk_color_black, eadk_color_white);
          eadk_timing_msleep(50);
        }
        if (eadk_keyboard_key_down(keys, eadk_key_up)) {
          cam_dist += 5.0f;
          snprintf(buf, sizeof(buf), "Camera distance: %d.%02d", (int)cam_dist, (int)(fabsf(cam_dist * 100)) % 100);
          eadk_display_draw_string(buf, (eadk_point_t){10, 130}, false, eadk_color_black, eadk_color_white);
          eadk_timing_msleep(50);
        }
        if (eadk_keyboard_key_down(keys, eadk_key_down)) {
          cam_dist -= 5.0f;
          if (cam_dist < 0.5f) cam_dist = 0.5f;
          snprintf(buf, sizeof(buf), "Camera distance: %d.%02d", (int)cam_dist, (int)(fabsf(cam_dist * 100)) % 100);
          eadk_display_draw_string(buf, (eadk_point_t){10, 130}, false, eadk_color_black, eadk_color_white);
          eadk_timing_msleep(50);
        }
        if (eadk_keyboard_key_down(keys, eadk_key_ok)) {
          break;
        }
      }
    }
  }

  eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
  screen_batches_dynamic(data, NB_POINTS, NB_EDGES, cam_theta, cam_phi, cam_dist, scale, center_x, center_y, center_z);

  bool is_debug = false;
  bool is_cam_mode = false;

  uint32_t elapsed = 0;

  while (true) {
    bool redraw = false;

    eadk_keyboard_state_t keys = eadk_keyboard_scan();

    float cam_speed = 0.02f * ((float)elapsed / 60.0f);
    float move_speed = 0.05f * ((float)elapsed / 60.0f);

    if (!is_cam_mode) {
      if (eadk_keyboard_key_down(keys, eadk_key_imaginary)) {
        cam_theta += cam_speed;
        redraw = true;
      }
      if (eadk_keyboard_key_down(keys, eadk_key_power)) {
        cam_theta -= cam_speed;
        redraw = true;
      }
      if (eadk_keyboard_key_down(keys, eadk_key_toolbox)) {
        cam_phi += cam_speed;
        if (cam_phi > 1.5f) cam_phi = 1.5f;
        redraw = true;
      }
      if (eadk_keyboard_key_down(keys, eadk_key_sqrt)) {
        cam_phi -= cam_speed;
        if (cam_phi < -1.5f) cam_phi = -1.5f;
        redraw = true;
      }

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
    }

    if (eadk_keyboard_key_down(keys, eadk_key_home)) {
      return 0;
    }

    if (eadk_keyboard_key_down(keys, eadk_key_shift)) {
      is_debug = !is_debug;
      redraw = true;
      while (eadk_keyboard_scan() != 0) eadk_timing_msleep(100);
    }

    if (eadk_keyboard_key_down(keys, eadk_key_zero)) {
      is_cam_mode = !is_cam_mode;
      redraw = true;
      while (eadk_keyboard_scan() != 0) eadk_timing_msleep(100);
    }

    if (!is_cam_mode){
      if (redraw) {
        uint32_t start = (uint32_t)eadk_timing_millis();
        eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
        screen_batches_dynamic(data, NB_POINTS, NB_EDGES, cam_theta, cam_phi, cam_dist, scale, center_x, center_y, center_z);
        uint32_t end = (uint32_t)eadk_timing_millis();
        elapsed = end - start;
      }
    }
    else {
      uint32_t start = (uint32_t)eadk_timing_millis();

      cam_theta -= cam_speed;

      eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
      screen_batches_dynamic(data, NB_POINTS, NB_EDGES, cam_theta, cam_phi, cam_dist, scale, center_x, center_y, center_z);
      eadk_display_draw_string("Camera Mode... Press 0 to quit", (eadk_point_t){0, 225}, false, eadk_color_black, eadk_color_white);

      uint32_t end = (uint32_t)eadk_timing_millis();
      elapsed = end - start;
    }

    if (is_debug) {
      char buf[192];
      snprintf(buf, sizeof(buf),
        "Cam: theta=%d.%02d, phi=%d.%02d, scale=%d.%02d\n"
        "Center: (%d.%02d, %d.%02d, %d.%02d)\n"
        "Cam_speed: %d.%02d, Move_speed: %d.%02d\n"
        "Framerate: %u ms\n"
        "Sleep: %u ms",
        FMT_FLOAT(cam_theta), FMT_FLOAT(cam_phi), FMT_FLOAT(scale),
        FMT_FLOAT(center_x), FMT_FLOAT(center_y), FMT_FLOAT(center_z), 
        FMT_FLOAT(cam_speed), FMT_FLOAT(move_speed),
        elapsed,
        elapsed < 60 ? 60 - elapsed : 0
      );
      eadk_display_draw_string(buf, (eadk_point_t){0, 0}, false, eadk_color_black, eadk_color_white);
    }


    if (elapsed < 60) {
      eadk_timing_msleep(60 - elapsed);
    }
  }

  return 0;
}
