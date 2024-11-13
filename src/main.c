#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 675
#define OFFSET_UI 60
#define PANEL_RATIO 2.0f
#define FOVY 60
#define UPVEC3                                                                 \
  (Vector3) { 0, 1, 0 }
#define ZEROVEC3                                                               \
  (Vector3) { 0, 0, 0 }

float FREQUENCY = 1.0f;
float AMPLITUDE = 1.0f;

#define TOTAL_POINTS 300

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
bool showControls = false;
bool lockCursorAndCam = false;

Vector3 sinPoints[TOTAL_POINTS];
Vector3 cosPoints[TOTAL_POINTS];
Vector3 funcPoints[TOTAL_POINTS];

Camera camera = {0};
Camera cameraStatic = {0};

Color lightGreen = (Color){191, 255, 191, 255};
Color lightRed = (Color){255, 191, 191, 255};

int frameNum = 0;
double currentTime = 0;

Vector3 pointSinPos;
Vector3 pointCosPos;
Vector3 pointFuncPos;

bool fullwidth = false;

RenderTexture renderTexture = {0};
float screen1Width = PANEL_RATIO;

RenderTexture graph1RenderTexture = {0};
float graph1Width = PANEL_RATIO;
float graph1Height = PANEL_RATIO;

RenderTexture graph2RenderTexture = {0};
float graph2Width = PANEL_RATIO;
float graph2Height = PANEL_RATIO;

Color bg = (Color){45, 45, 45, 255};

int screenWidth, screenHeight;

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);
  InitWindow(WIDTH, HEIGHT, "fouralizer - Fourier Series Visualizer");
  // DisableCursor();
  renderTexture = LoadRenderTexture(WIDTH / screen1Width, HEIGHT);
  graph1RenderTexture =
      LoadRenderTexture(WIDTH / graph1Width, HEIGHT / graph1Height);
  graph2RenderTexture =
      LoadRenderTexture(WIDTH / graph2Width, HEIGHT / graph2Height);

  camera.position = (Vector3){-2, .5, 3};
  camera.target = ZEROVEC3;
  camera.projection = CAMERA_PERSPECTIVE;
  camera.fovy = FOVY;
  camera.up = UPVEC3;

  cameraStatic.position = (Vector3){-.01, 4, 0};
  cameraStatic.target = (Vector3){0, -0, 0};
  cameraStatic.projection = CAMERA_PERSPECTIVE;
  cameraStatic.fovy = FOVY;
  cameraStatic.up = UPVEC3;

  Matrix rotationMatrix = MatrixRotate(camera.up, 90 * DEG2RAD);
  Vector3 direction = Vector3Transform(camera.target, rotationMatrix);
  camera.target = direction;

  pointSinPos = ZEROVEC3;
  pointCosPos = ZEROVEC3;
  pointFuncPos = ZEROVEC3;

  for (int i = 0; i < TOTAL_POINTS; i++) {
    sinPoints[i] = pointSinPos;
    cosPoints[i] = pointCosPos;
    funcPoints[i] = pointFuncPos;
  }

  float s_t = 1;
  while (!WindowShouldClose()) {
    if (lockCursorAndCam)
      UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    currentTime = GetTime();
    rotation -= freq_d * GetFrameTime() * 2 * PI;

    /* Update point positions based on sine and cosine functions
     * with the rotation matrix
     */
    pointSinPos.x =
        -sin(rotation) * sin(currentTime * FREQUENCY * 2 * PI) * -s_t;
    pointSinPos.y =
        cos(rotation) * sin(currentTime * FREQUENCY * 2 * PI) * -s_t;

    pointCosPos.x = cos(currentTime * FREQUENCY * 2 * PI) * cos(rotation) * s_t;
    pointCosPos.y = cos(currentTime * FREQUENCY * 2 * PI) * sin(rotation) * s_t;

    pointFuncPos.y = pointSinPos.y;
    pointFuncPos.x = pointCosPos.x;

    /* Shift points and add new positions in 60fps ideally */
    frameNum++;
    if (frameNum >= GetFPS() * 1 / 60) {
      AddShiftWavePoints();
      frameNum = 0;
    }

    BeginDrawing();
    ClearBackground(bg);

    BeginTextureMode(renderTexture);
    ClearBackground(BLACK);
    DrawFPS(10, HEIGHT - 35);
    BeginMode3D(camera);
    if (showGrid)
      DrawGrid(12, 1);
    DrawWaveIndicators();
    DrawAxisLines();
    /* Draw continuous sine & cosine wave */
    DrawWaves();
    EndMode3D();
    DrawHelp();
    EndTextureMode();

    BeginTextureMode(graph1RenderTexture);
    ClearBackground(BLACK);
    BeginMode3D(cameraStatic);
    DrawGrid(30, 1);
    rlBegin(RL_LINES);
    for (int i = 0; i < TOTAL_POINTS - 1; i++) {
      rlColor3f(0.0f, 1.0f, 0.0f);
      rlVertex3f(funcPoints[i].x, 0, funcPoints[i].z);
      rlVertex3f(funcPoints[i + 1].x, 0, funcPoints[i + 1].z);
    }
    rlEnd();
    EndMode3D();
    EndTextureMode();

    BeginTextureMode(graph2RenderTexture);
    ClearBackground(BLACK);
    BeginMode3D(cameraStatic);
    DrawGrid(30, 1);
    rlBegin(RL_LINES);
    for (int i = 0; i < TOTAL_POINTS - 1; i++) {
      rlColor3f(0.0f, 1.0f, 0.0f);
      rlVertex3f(funcPoints[i].y, 0, funcPoints[i].z);
      rlVertex3f(funcPoints[i + 1].y, 0, funcPoints[i + 1].z);
    }
    EndMode3D();
    EndTextureMode();

    float padding = 20.0f;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (!fullwidth) {
      DrawTexturePro(
          graph1RenderTexture.texture,
          (Rectangle){0, 0, graph1RenderTexture.texture.width - 2 * padding,
                      -graph1RenderTexture.texture.height},
          (Rectangle){padding + (screenWidth / 2.0f), padding,
                      (screenWidth / graph1Width) - 2 * padding,
                      (-1 * screenHeight / graph1Height) + (padding)},
          (Vector2){0, 0}, 0, WHITE);

      DrawTexturePro(
          graph2RenderTexture.texture,
          (Rectangle){1, 0, graph2RenderTexture.texture.width - 2 * padding,
                      -graph2RenderTexture.texture.height},
          (Rectangle){padding + (screenWidth / 2.0f),
                      (screenHeight / 2.0f) + padding,
                      (screenWidth / graph2Width) - 2 * padding,
                      (-1 * screenHeight / graph2Height) + (2 * padding)},
          (Vector2){0, 0}, 0, WHITE);
    }

    /* Keep this one at the very bottom, so in front */
    DrawTexturePro(renderTexture.texture,
                   (Rectangle){0, 0, renderTexture.texture.width - padding,
                               -renderTexture.texture.height},
                   (Rectangle){padding, padding,
                               (screenWidth / screen1Width) - padding,
                               (-1 * screenHeight) + (2 * padding)},
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
  if (showCosine && showSine && showFunc) {
    DrawLine3D(pointFuncPos, pointSinPos, WHITE);
    DrawLine3D(pointFuncPos, pointCosPos, WHITE);
  }
}

void AddNewVector(Vector3 *vector3, Vector3 new) {
  vector3[TOTAL_POINTS - 1] = new;
}

void ShiftVector3By(Vector3 *vector3) {
  for (int i = 1; i < TOTAL_POINTS; i++) {
    vector3[i - 1] = vector3[i];
    vector3[i - 1].z -= 1.0f / GetFPS();
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
  if (!showControls) {
    DrawText("Press 'H' to open help", 10, 35 + OFFSET_UI, 16, WHITE);
    return;
  }
  DrawRectangle(5, 30 + OFFSET_UI, 210, 350, (Color){100, 100, 100, 100});
  DrawText("Help: Press 'H' to close", 10, 35 + OFFSET_UI, 16, WHITE);
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
  DrawText("F: Toggle 3D Panel fullwidth", 10, 290 + OFFSET_UI, 15, LIGHTGRAY);
  DrawText("Esc: Close Fouralizer", 10, 310 + OFFSET_UI, 15, LIGHTGRAY);
  DrawText("L: (Un)Lock Cursor & Cam", 10, 330 + OFFSET_UI, 15, LIGHTGRAY);
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
  case KEY_L:
    lockCursorAndCam = !lockCursorAndCam;
    if (lockCursorAndCam) {
      DisableCursor();
    } else {
      EnableCursor();
    }
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
