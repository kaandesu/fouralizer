#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 675
#define OFFSET_UI 85

float FREQUENCY = 2.0f;
float AMPLITUDE = 1.0f;

#define FD 3
#define TOTAL_POINTS 200

void AddNewVector(Vector3 *vector3, Vector3 new);
void ShiftVector3By(Vector3 *vector3);

int main(void) {
  InitWindow(WIDTH, HEIGHT, "fouralizer - Fourier Series Visualizer");
  SetTargetFPS(60);
  Camera camera = {0};
  camera.position = (Vector3){-2, .5, 3};
  camera.target = (Vector3){0, .5, 0};
  camera.projection = CAMERA_PERSPECTIVE;
  camera.fovy = 60;
  camera.up = (Vector3){0, 1, 0};

  Vector3 pointSinPos = (Vector3){0, 0, 0};
  Vector3 pointCosPos = (Vector3){AMPLITUDE, 0, 0};
  Vector3 pointCirclePos = (Vector3){AMPLITUDE, 0, 0};

  Vector3 sinPoints[TOTAL_POINTS];
  Vector3 cosPoints[TOTAL_POINTS];
  Vector3 funcPoints[TOTAL_POINTS];

  int frameNum = 0;
  Color bg = (Color){46, 46, 46, 255};

  for (int i = 0; i < TOTAL_POINTS; i++) {
    sinPoints[i] = pointSinPos;
    cosPoints[i] = pointCosPos;
    funcPoints[i] = pointCirclePos;
  }

  Vector3 axisRot = (Vector3){0, 0, 0};
  float rotation = 0;
  float freq_d = -0.0f;

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    // DisableCursor();
    BeginDrawing();
    ClearBackground(bg);
    DrawFPS(10, HEIGHT - 35);

    BeginMode3D(camera);

    rotation -= freq_d * GetFrameTime();
    /* Update point positions based on sine and cosine functions */
    pointSinPos.y = AMPLITUDE * sin(GetTime() * FREQUENCY);
    pointCosPos.x = AMPLITUDE * cos(GetTime() * FREQUENCY);

    pointSinPos.x = AMPLITUDE * -sin(rotation) * sin(GetTime() * FREQUENCY);
    pointSinPos.y = AMPLITUDE * cos(rotation) * sin(GetTime() * FREQUENCY);

    pointCosPos.x = AMPLITUDE * cos(GetTime() * FREQUENCY) * cos(rotation);
    pointCosPos.y = AMPLITUDE * cos(GetTime() * FREQUENCY) * sin(rotation);

    pointCirclePos.y = pointSinPos.y;
    pointCirclePos.x = pointCosPos.x;

    /* Draw Spherical Indicators */
    DrawSphere(pointSinPos, .1, RED);
    DrawSphere(pointCosPos, .1, YELLOW);
    DrawSphere(pointCirclePos, .1, GREEN);

    /* Axis & Circle lines ROTATING */
    float x1, x2, y1, y2;
    x1 = cos(GetTime() * freq_d);
    y1 = -AMPLITUDE * sin(GetTime() * freq_d);
    x2 = sin((GetTime() * freq_d) - PI / 2);
    y2 = cos((GetTime() * freq_d) - PI / 2);

    float x1_2, x2_2, y1_2, y2_2;
    x1_2 = -AMPLITUDE * sin(GetTime() * freq_d);
    y1_2 = -1 * cos(GetTime() * freq_d);
    x2_2 = cos((GetTime() * freq_d) - PI / 2);
    y2_2 = -sin((GetTime() * freq_d) - PI / 2);
    DrawLine3D((Vector3){x1, y1, 0}, (Vector3){x2, y2, 0}, WHITE);
    DrawLine3D((Vector3){x1_2, y1_2, 0}, (Vector3){x2_2, y2_2, 0}, WHITE);

    /* Axis lines STATIC */
    DrawLine3D((Vector3){0, -AMPLITUDE, 0}, (Vector3){0, AMPLITUDE, 0}, WHITE);
    DrawLine3D((Vector3){-AMPLITUDE, 0, 0}, (Vector3){AMPLITUDE, 0, 0}, WHITE);

    /* Circle with raidus AMPLITUDE */
    DrawCircle3D((Vector3){0, 0, 0}, AMPLITUDE, (Vector3){0, 0, 0}, 0, WHITE);

    /* Distance lines */
    DrawLine3D(pointCirclePos, pointSinPos, WHITE);
    DrawLine3D(pointCirclePos, pointCosPos, WHITE);

    /* Shift points and add new positions */
    frameNum++;
    if (frameNum >= 2) {
      AddNewVector(sinPoints, pointSinPos);
      ShiftVector3By(sinPoints);
      AddNewVector(cosPoints, pointCosPos);
      ShiftVector3By(cosPoints);
      AddNewVector(funcPoints, pointCirclePos);
      ShiftVector3By(funcPoints);
      frameNum = 0;
    }

    /* Draw continuous sine & cosine wave */
    rlBegin(RL_LINES);
    for (int i = 0; i < TOTAL_POINTS - 1; i++) {
      rlColor3f(1.0f, 0.0f, 0.0f);
      rlVertex3f(sinPoints[i].x, sinPoints[i].y, sinPoints[i].z);
      rlVertex3f(sinPoints[i + 1].x, sinPoints[i + 1].y, sinPoints[i + 1].z);

      rlColor3f(1.0f, 1.0f, 0.0f);
      rlVertex3f(cosPoints[i].x, cosPoints[i].y, cosPoints[i].z);
      rlVertex3f(cosPoints[i + 1].x, cosPoints[i + 1].y, cosPoints[i + 1].z);

      rlColor3f(0.0f, 1.0f, 0.0f);
      rlVertex3f(funcPoints[i].x, funcPoints[i].y, funcPoints[i].z);
      rlVertex3f(funcPoints[i + 1].x, funcPoints[i + 1].y, funcPoints[i + 1].z);
    }
    rlEnd();

    EndMode3D();
    EndDrawing();

    if (IsKeyPressed(KEY_LEFT)) {
      rotation = 0;
      freq_d -= 0.2f;
      for (int i = 0; i < TOTAL_POINTS; i++) {
        sinPoints[i] = pointSinPos;
        cosPoints[i] = pointCosPos;
        funcPoints[i] = pointCirclePos;
      }
    }

    if (IsKeyPressed(KEY_RIGHT)) {
      rotation = 0;
      freq_d += 0.2f;

      for (int i = 0; i < TOTAL_POINTS; i++) {
        sinPoints[i] = pointSinPos;
        cosPoints[i] = pointCosPos;
        funcPoints[i] = pointCirclePos;
      }
    }

    if (IsKeyPressed(KEY_R)) {
      rotation = 0;
      freq_d = 0;

      for (int i = 0; i < TOTAL_POINTS; i++) {
        sinPoints[i] = pointSinPos;
        cosPoints[i] = pointCosPos;
        funcPoints[i] = pointCirclePos;
      }
    }
    DrawText(TextFormat("Spin Frequency: %.2f", freq_d), 10, 20, 15, WHITE);

    DrawText(TextFormat("Signal Frequency: %.2f", FREQUENCY), 10, 60, 15,
             WHITE);
    DrawText("CONTROLS:", 10, 20 + OFFSET_UI, 15, LIGHTGRAY);
    DrawText("Left Arrow: Decrease Spin Frequency ", 10, 50 + OFFSET_UI, 15,
             LIGHTGRAY);
    DrawText("Right Arrow: Increase Spin Frequency ", 10, 75 + OFFSET_UI, 15,
             LIGHTGRAY);
    DrawText("R: Reset Spin Frequency ", 10, 100 + OFFSET_UI, 15, LIGHTGRAY);

    DrawText("https://github.com/kaandesu/fouralizer", WIDTH - 325, HEIGHT - 30,
             15, RAYWHITE);
  }
  CloseWindow();
  return EXIT_SUCCESS;
}

void AddNewVector(Vector3 *vector3, Vector3 new) {
  vector3[TOTAL_POINTS - 1] = new;
}

void ShiftVector3By(Vector3 *vector3) {
  for (int i = 1; i < TOTAL_POINTS; i++) {
    vector3[i - 1] = vector3[i];
    vector3[i - 1].z -= 0.02;
  }
}
