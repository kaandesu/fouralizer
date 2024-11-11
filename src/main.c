#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 675
#define OFFSET_UI 85

float FREQUENCY = 5.0f;
float AMPLITUDE = 1.0f;

#define FD 3
#define TOTAL_POINTS 1000

void AddNewVector(Vector3 *vector3, Vector3 new);
void ShiftVector3By(Vector3 *vector3);
float CustomFunction(float t);

int main(void) {
  InitWindow(WIDTH, HEIGHT, "fouralizer - Fourier Series Visualizer");
  Camera camera = {0};
  camera.position = (Vector3){-2, .5, 3};
  camera.target = (Vector3){0, .0, 0};
  camera.projection = CAMERA_PERSPECTIVE;
  camera.fovy = 60;
  camera.up = (Vector3){0, 1, 0};

  Vector3 pointSinPos = (Vector3){0, 0, 0};
  Vector3 pointCosPos = (Vector3){AMPLITUDE, 0, 0};
  Vector3 pointCirclePos = (Vector3){AMPLITUDE, 0, 0};

  Vector3 sinPoints[TOTAL_POINTS];
  Vector3 cosPoints[TOTAL_POINTS];
  Vector3 funcPoints[TOTAL_POINTS];

  Color lightGreen = (Color){191, 255, 191, 255};
  Color lightRed = (Color){255, 191, 191, 255};

  bool showSine = true;
  bool showCosine = true;
  bool showFunc = true;
  bool showGrid = true;
  bool showControls = true;

  int frameNum = 0;
  Color bg = (Color){36, 36, 36, 255};

  for (int i = 0; i < TOTAL_POINTS; i++) {
    sinPoints[i] = pointSinPos;
    cosPoints[i] = pointCosPos;
    funcPoints[i] = pointCirclePos;
  }

  Vector3 axisRot = (Vector3){0, 0, 0};
  float rotation = 0;
  float freq_d = -0.0f;
  double currentTime = 0;

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    // DisableCursor();
    BeginDrawing();
    ClearBackground(bg);
    DrawFPS(10, HEIGHT - 35);

    BeginMode3D(camera);
    if (showGrid)
      DrawGrid(20, 1);
    currentTime = GetTime();

    rotation -= freq_d * GetFrameTime();
    /* Update point positions based on sine and cosine functions */
    pointSinPos.y = AMPLITUDE * sin(currentTime * FREQUENCY);
    pointCosPos.x = AMPLITUDE * cos(currentTime * FREQUENCY);

    pointSinPos.x = AMPLITUDE * -sin(rotation) * sin(currentTime * FREQUENCY);
    pointSinPos.y = AMPLITUDE * cos(rotation) * sin(currentTime * FREQUENCY);

    pointCosPos.x = AMPLITUDE * cos(currentTime * FREQUENCY) * cos(rotation);
    pointCosPos.y = AMPLITUDE * cos(currentTime * FREQUENCY) * sin(rotation);

    pointCirclePos.y = pointSinPos.y;
    pointCirclePos.x = pointCosPos.x;

    /* Draw Spherical Indicators */
    if (showSine)
      DrawSphere(pointSinPos, .1, RED);
    if (showCosine)
      DrawSphere(pointCosPos, .1, YELLOW);
    if (showFunc)
      DrawSphere(pointCirclePos, .1, GREEN);

    /* Axis & Circle lines ROTATING */
    float x1, x2, y1, y2;
    x1 = cos(currentTime * freq_d);
    y1 = -AMPLITUDE * sin(currentTime * freq_d);
    x2 = sin((currentTime * freq_d) - PI / 2);
    y2 = cos((currentTime * freq_d) - PI / 2);

    float x1_2, x2_2, y1_2, y2_2;
    x1_2 = -AMPLITUDE * sin(currentTime * freq_d);
    y1_2 = -1 * cos(currentTime * freq_d);
    x2_2 = cos((currentTime * freq_d) - PI / 2);
    y2_2 = -sin((currentTime * freq_d) - PI / 2);
    DrawLine3D((Vector3){x1, y1, 0}, (Vector3){x2, y2, 0}, WHITE);
    DrawLine3D((Vector3){x1_2, y1_2, 0}, (Vector3){x2_2, y2_2, 0}, WHITE);

    /* Axis lines STATIC */
    DrawLine3D((Vector3){0, -AMPLITUDE, 0}, (Vector3){0, AMPLITUDE, 0}, WHITE);
    DrawLine3D((Vector3){-AMPLITUDE, 0, 0}, (Vector3){AMPLITUDE, 0, 0}, WHITE);

    /* Circle with raidus AMPLITUDE */
    DrawCircle3D((Vector3){0, 0, 0}, AMPLITUDE, (Vector3){0, 0, 0}, 0, WHITE);

    /* Distance lines */

    if (showCosine && showSine) {
      DrawLine3D(pointCirclePos, pointSinPos, WHITE);
      DrawLine3D(pointCirclePos, pointCosPos, WHITE);
    }

    /* Shift points and add new positions */
    frameNum++;
    if (frameNum >= GetFPS() * 2 / 60) {
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
      if (showSine) {
        rlColor3f(1.0f, 0.0f, 0.0f);
        rlVertex3f(sinPoints[i].x, sinPoints[i].y, sinPoints[i].z);
        rlVertex3f(sinPoints[i + 1].x, sinPoints[i + 1].y, sinPoints[i + 1].z);
      }

      if (showCosine) {
        rlColor3f(1.0f, 1.0f, 0.0f);
        rlVertex3f(cosPoints[i].x, cosPoints[i].y, cosPoints[i].z);
        rlVertex3f(cosPoints[i + 1].x, cosPoints[i + 1].y, cosPoints[i + 1].z);
      }

      if (showFunc) {
        rlColor3f(0.0f, 1.0f, 0.0f);
        rlVertex3f(funcPoints[i].x, funcPoints[i].y, funcPoints[i].z);
        rlVertex3f(funcPoints[i + 1].x, funcPoints[i + 1].y,
                   funcPoints[i + 1].z);
      }
    }
    rlEnd();

    EndMode3D();
    EndDrawing();

    if (IsKeyPressed(KEY_ONE)) {
      showCosine = !showCosine;
    }

    if (IsKeyPressed(KEY_TWO)) {
      showSine = !showSine;
    }

    if (IsKeyPressed(KEY_THREE)) {
      showFunc = !showFunc;
    }

    if (IsKeyPressed(KEY_LEFT)) {
      freq_d -= 0.25f;
      rotation = 0;
    }

    if (IsKeyPressed(KEY_G)) {
      showGrid = !showGrid;
    }

    if (IsKeyPressed(KEY_UP)) {
      camera.position.y += 0.5;
    }

    if (IsKeyPressed(KEY_DOWN)) {
      camera.position.y -= 0.5;
    }

    if (IsKeyPressed(KEY_H)) {
      showControls = !showControls;
    }

    if (IsKeyPressed(KEY_C)) {
      for (int i = 0; i < TOTAL_POINTS; i++) {
        sinPoints[i] = (Vector3){0, 0, 0};
        cosPoints[i] = (Vector3){0, 0, 0};
        funcPoints[i] = (Vector3){0, 0, 0};
      }
    }

    if (IsKeyPressed(KEY_RIGHT)) {
      freq_d += 0.25f;
      rotation = 0;
    }

    if (IsKeyPressed(KEY_R)) {
      rotation = 0;
      freq_d = 0;

      for (int i = 0; i < TOTAL_POINTS; i++) {
        sinPoints[i] = (Vector3){0, 0, 0};
        cosPoints[i] = (Vector3){0, 0, 0};
        funcPoints[i] = (Vector3){0, 0, 0};
      }
    }
    DrawText(TextFormat("Spin Frequency: %.2f", freq_d), 10, 20, 15, WHITE);

    DrawText(TextFormat("Signal Frequency: %.2f", FREQUENCY), 10, 60, 15,
             WHITE);

    if (showControls) {
      DrawText("Help:", 10, 35 + OFFSET_UI, 16, WHITE);
      DrawText("Left Arrow: Decrease Spin Frequency ", 10, 60 + OFFSET_UI, 15,
               lightGreen);
      DrawText("Right Arrow: Increase Spin Frequency ", 10, 80 + OFFSET_UI, 15,
               lightRed);
      DrawText("R: Reset Spin Frequency ", 10, 105 + OFFSET_UI, 15, LIGHTGRAY);
      DrawText("Up Arrow: Increase Camera Y ", 10, 125 + OFFSET_UI, 15,
               lightGreen);
      DrawText("Down Arrow: Decrease Camera Y ", 10, 145 + OFFSET_UI, 15,
               lightRed);
      DrawText("G: Toggle Grid ", 10, 165 + OFFSET_UI, 15, LIGHTGRAY);
      DrawText("H: Toggle Help", 10, 185 + OFFSET_UI, 15, LIGHTGRAY);
      DrawText("C: Clean lines", 10, 205 + OFFSET_UI, 15, LIGHTGRAY);
      DrawText("1: Toggle Real Part ", 10, 230 + OFFSET_UI, 15, YELLOW);
      DrawText("2: Toggle Imag Part", 10, 250 + OFFSET_UI, 15, RED);
      DrawText("3: Toggle Combined", 10, 270 + OFFSET_UI, 15, GREEN);
    }

    DrawText("https://github.com/kaandesu/fouralizer", WIDTH - 325, HEIGHT - 30,
             15, RAYWHITE);
  }
  CloseWindow();
  return EXIT_SUCCESS;
}

float CustomFunction(float t) {
  // Example: Combine multiple sine functions
  return sin(2 * PI * 3 * t) + sin(2 * PI * 6 * t);
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
