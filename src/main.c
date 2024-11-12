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

void DrawHelp();
void DrawAxisLines();
void DrawWaves();
void input();
void DrawWaveIndicators();
void AddShiftWavePoints();

float rotation = 0;
float freq_d = -0.0f;

bool showSine = true;
bool showCosine = true;
bool showFunc = true;
bool showGrid = true;
bool showControls = true;

Vector3 sinPoints[TOTAL_POINTS];
Vector3 cosPoints[TOTAL_POINTS];
Vector3 funcPoints[TOTAL_POINTS];

Camera camera = {0};

Color lightGreen = (Color){191, 255, 191, 255};
Color lightRed = (Color){255, 191, 191, 255};

int frameNum = 0;
double currentTime = 0;

Vector3 pointSinPos;
Vector3 pointCosPos;
Vector3 pointFuncPos;

RenderTexture renderTexture = {0};
float screen1Width = 2.0f;
bool fullwidth = false;

Color bg = (Color){45, 45, 45, 255};

int main(void) {
  InitWindow(WIDTH, HEIGHT, "fouralizer - Fourier Series Visualizer");
  renderTexture = LoadRenderTexture(WIDTH / screen1Width, HEIGHT);

  camera.position = (Vector3){-2, .5, 3};
  camera.target = (Vector3){0, .0, 0};
  camera.projection = CAMERA_PERSPECTIVE;
  camera.fovy = 60;
  camera.up = (Vector3){0, 1, 0};

  pointSinPos = (Vector3){0, 0, 0};
  pointCosPos = (Vector3){AMPLITUDE, 0, 0};
  pointFuncPos = (Vector3){AMPLITUDE, 0, 0};

  for (int i = 0; i < TOTAL_POINTS; i++) {
    sinPoints[i] = pointSinPos;
    cosPoints[i] = pointCosPos;
    funcPoints[i] = pointFuncPos;
  }

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    currentTime = GetTime();
    rotation -= freq_d * GetFrameTime();

    /* Update point positions based on sine and cosine functions
     * with the rotation matrix
     */
    pointSinPos.x = AMPLITUDE * -sin(rotation) * sin(currentTime * FREQUENCY);
    pointSinPos.y = AMPLITUDE * cos(rotation) * sin(currentTime * FREQUENCY);

    pointCosPos.x = AMPLITUDE * cos(currentTime * FREQUENCY) * cos(rotation);
    pointCosPos.y = AMPLITUDE * cos(currentTime * FREQUENCY) * sin(rotation);

    pointFuncPos.y = pointSinPos.y;
    pointFuncPos.x = pointCosPos.x;

    /* Shift points and add new positions in 60fps ideally */
    frameNum++;
    if (frameNum >= GetFPS() * 2 / 60) {
      AddShiftWavePoints();
      frameNum = 0;
    }

    BeginDrawing();
    BeginTextureMode(renderTexture);
    ClearBackground(bg);
    DrawFPS(10, HEIGHT - 35);
    BeginMode3D(camera);
    if (showGrid)
      DrawGrid(20, 1);
    DrawWaveIndicators();
    DrawAxisLines();
    /* Draw continuous sine & cosine wave */
    DrawWaves();
    EndMode3D();
    DrawHelp();
    EndTextureMode();

    DrawTexturePro(renderTexture.texture,
                   (Rectangle){0, 0, renderTexture.texture.width,
                               -renderTexture.texture.height},
                   (Rectangle){0, 0, WIDTH / screen1Width, -1 * HEIGHT},
                   (Vector2){0, 0}, 0, WHITE);

    EndDrawing();
    input();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}

void DrawAxisLines() {
  /* Axis & Circle lines ROTATING */
  float x1, x2, y1, y2;
  x1 = cos(currentTime * freq_d);
  y1 = -AMPLITUDE * sin(currentTime * freq_d);
  x2 = sin((currentTime * freq_d) - PI / 2);
  y2 = cos((currentTime * freq_d) - PI / 2);

  DrawLine3D((Vector3){x1, y1, 0}, (Vector3){x2, y2, 0}, WHITE);
  DrawLine3D((Vector3){y1, -x1, 0}, (Vector3){y2, -x2, 0}, WHITE);

  /* Axis lines STATIC */
  DrawLine3D((Vector3){0, -AMPLITUDE, 0}, (Vector3){0, AMPLITUDE, 0}, WHITE);
  DrawLine3D((Vector3){-AMPLITUDE, 0, 0}, (Vector3){AMPLITUDE, 0, 0}, WHITE);

  /* Circle with raidus AMPLITUDE */
  DrawCircle3D((Vector3){0, 0, 0}, AMPLITUDE, (Vector3){0, 0, 0}, 0, WHITE);

  /* Distance lines */

  if (showCosine && showSine) {
    DrawLine3D(pointFuncPos, pointSinPos, WHITE);
    DrawLine3D(pointFuncPos, pointCosPos, WHITE);
  }
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

void AddShiftWavePoints() {
  sinPoints[TOTAL_POINTS - 1] = pointSinPos;
  cosPoints[TOTAL_POINTS - 1] = pointCosPos;
  funcPoints[TOTAL_POINTS - 1] = pointFuncPos;
  ShiftVector3By(sinPoints);
  ShiftVector3By(cosPoints);
  ShiftVector3By(funcPoints);
}

void DrawWaveIndicators() {
  if (showSine)
    DrawSphere(pointSinPos, .1, RED);
  if (showCosine)
    DrawSphere(pointCosPos, .1, YELLOW);
  if (showFunc)
    DrawSphere(pointFuncPos, .1, GREEN);
}

void DrawWaves() {
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
      rlVertex3f(funcPoints[i + 1].x, funcPoints[i + 1].y, funcPoints[i + 1].z);
    }
  }
  rlEnd();
}

void DrawHelp() {
  DrawText(TextFormat("Spin Frequency: %.2f", freq_d), 10, 20, 15, WHITE);
  DrawText(TextFormat("Signal Frequency: %.2f", FREQUENCY), 10, 60, 15, WHITE);
  DrawText("https://github.com/kaandesu/fouralizer", WIDTH - 325, HEIGHT - 30,
           15, RAYWHITE);
  if (!showControls)
    return;
  DrawText("Help:", 10, 35 + OFFSET_UI, 16, WHITE);
  DrawText("Left: Decrease Spin Freq", 10, 60 + OFFSET_UI, 15, lightGreen);
  DrawText("Right: Increase Spin Freq", 10, 80 + OFFSET_UI, 15, lightRed);
  DrawText("Up: Increase Camera Y ", 10, 105 + OFFSET_UI, 15, lightGreen);
  DrawText("Down: Decrease Camera Y ", 10, 125 + OFFSET_UI, 15, lightRed);
  DrawText("R: Reset Spin Freq", 10, 145 + OFFSET_UI, 15, LIGHTGRAY);
  DrawText("G: Toggle Grid ", 10, 165 + OFFSET_UI, 15, LIGHTGRAY);
  DrawText("H: Toggle Help", 10, 185 + OFFSET_UI, 15, LIGHTGRAY);
  DrawText("C: Clean lines", 10, 205 + OFFSET_UI, 15, LIGHTGRAY);
  DrawText("1: Toggle Real Part ", 10, 230 + OFFSET_UI, 15, YELLOW);
  DrawText("2: Toggle Imag Part", 10, 250 + OFFSET_UI, 15, RED);
  DrawText("3: Toggle Combined", 10, 270 + OFFSET_UI, 15, GREEN);
}

void input() {
  switch (GetKeyPressed()) {
  case KEY_F:
    fullwidth = !fullwidth;
    if (fullwidth) {
      screen1Width = 1;
    } else {
      screen1Width = 2;
    }

    renderTexture = LoadRenderTexture(WIDTH / screen1Width, HEIGHT);
    break;
  case KEY_ONE:
    showCosine = !showCosine;
    break;
  case KEY_TWO:
    showSine = !showSine;
    break;
  case KEY_THREE:
    showFunc = !showFunc;
    break;
  case KEY_LEFT:
    freq_d -= 0.25f;
    rotation = 0;
    break;
  case KEY_RIGHT:
    freq_d += 0.25f;
    rotation = 0;
    break;
  case KEY_G:
    showGrid = !showGrid;
    break;
  case KEY_UP:
    camera.position.y += 0.5;
    break;
  case KEY_DOWN:
    camera.position.y -= 0.5;
    break;
  case KEY_H:
    showControls = !showControls;
    break;
  case KEY_C:
    for (int i = 0; i < TOTAL_POINTS; i++) {
      sinPoints[i] = (Vector3){0, 0, 0};
      cosPoints[i] = (Vector3){0, 0, 0};
      funcPoints[i] = (Vector3){0, 0, 0};
    }
    break;
  case KEY_R:
    rotation = 0;
    freq_d = 0;
    for (int i = 0; i < TOTAL_POINTS; i++) {
      sinPoints[i] = (Vector3){0, 0, 0};
      cosPoints[i] = (Vector3){0, 0, 0};
      funcPoints[i] = (Vector3){0, 0, 0};
    }
    break;
  default:
    break;
  }
}
