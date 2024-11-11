#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 675

#define AMPLITUDE 1
#define FREQUENCY 2

#define FD 3
#define TOTAL_POINTS 200

void AddNewVector(Vector3 *vector3, Vector3 new);
void ShiftVector3By(Vector3 *vector3);

int main(void) {
  InitWindow(WIDTH, HEIGHT, "raylib example - continuous sine and cosine wave");
  SetTargetFPS(60);
  Camera camera = {0};
  camera.position = (Vector3){-2, 2, 3};
  camera.target = (Vector3){0, 0, 0};
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

  while (!WindowShouldClose()) {
    // UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    BeginDrawing();
    ClearBackground(bg);
    DrawFPS(10, 10);

    BeginMode3D(camera);

    /* Update point positions based on sine and cosine functions */
    pointSinPos.y = AMPLITUDE * sin(GetTime() * FREQUENCY);
    pointCosPos.x = AMPLITUDE * cos(GetTime() * FREQUENCY);
    pointCirclePos.y = pointSinPos.y;
    pointCirclePos.x = pointCosPos.x;

    /* Draw Spherical Indicators */
    DrawSphere(pointSinPos, .1, RED);
    DrawSphere(pointCosPos, .1, YELLOW);
    DrawSphere(pointCirclePos, .1, GREEN);

    /* Axis & Circle lines */
    DrawLine3D((Vector3){0, -AMPLITUDE, 0}, (Vector3){0, AMPLITUDE, 0}, WHITE);
    DrawLine3D((Vector3){-AMPLITUDE, 0, 0}, (Vector3){AMPLITUDE, 0, 0}, WHITE);
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
