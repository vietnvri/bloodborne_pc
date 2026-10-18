// SPDX-License-Identifier: GPL-2.0-or-later
// The mouse pointer's placement on the game's menu stage (bbport_menu_pointer.h StagePosition):
// the frame fitted into the window, the UI canvas over the frame, the 1920x1080 stage centred in
// the canvas. Everything else in that module runs against the live game.
#include <cassert>
#include <cmath>
#include <cstdio>
#include "bbport_menu_pointer.h"

static bool near(float a, float b) {
    return std::fabs(a - b) < 0.01f;
}

int main() {
    using BbMenuPointer::StagePosition;
    float x = 0, y = 0;
    // The window is the frame is the canvas: window pixels are stage pixels.
    assert(StagePosition(1920, 1080, 1920, 1080, 1920, 1080, 960, 540, x, y) && near(x, 960) && near(y, 540));
    assert(StagePosition(1920, 1080, 1920, 1080, 1920, 1080, 0, 0, x, y) && near(x, 0) && near(y, 0));
    // A 1440p output shown in a 1440p window: the canvas is still 1920x1080, so stage pixels
    // are two thirds of window pixels.
    assert(StagePosition(2560, 1440, 2560, 1440, 1920, 1080, 2560, 1440, x, y) && near(x, 1920) && near(y, 1080));
    assert(StagePosition(2560, 1440, 2560, 1440, 1920, 1080, 1280, 720, x, y) && near(x, 960) && near(y, 540));
    // A 1080p frame letterboxed in a 16:10 window (1920x1200): 60 px bars above and below.
    assert(StagePosition(1920, 1200, 1920, 1080, 1920, 1080, 960, 60, x, y) && near(x, 960) && near(y, 0));
    assert(StagePosition(1920, 1200, 1920, 1080, 1920, 1080, 960, 1140, x, y) && near(x, 960) && near(y, 1080));
    // Pillarboxed: a 1080p frame in a 2560x1080 window has 320 px bars left and right.
    assert(StagePosition(2560, 1080, 1920, 1080, 1920, 1080, 320, 540, x, y) && near(x, 0) && near(y, 540));
    assert(StagePosition(2560, 1080, 1920, 1080, 1920, 1080, 2240, 540, x, y) && near(x, 1920) && near(y, 540));
    // Above the bar is above the stage: callers hit-test, so negative values are fine.
    assert(StagePosition(1920, 1200, 1920, 1080, 1920, 1080, 960, 0, x, y) && near(y, -60));
    // A scaled window: the frame is 1080p, the window a quarter of it.
    assert(StagePosition(960, 540, 1920, 1080, 1920, 1080, 480, 270, x, y) && near(x, 960) && near(y, 540));
    // Ultrawide: a 3440x1440 output with its 2580x1080 HUD canvas; the stage is centred, so the
    // canvas' left edge is 330 stage pixels left of the stage and its centre is the stage's.
    assert(StagePosition(3440, 1440, 3440, 1440, 2580, 1080, 1720, 720, x, y) && near(x, 960) && near(y, 540));
    assert(StagePosition(3440, 1440, 3440, 1440, 2580, 1080, 0, 0, x, y) && near(x, -330) && near(y, 0));
    // Nothing to place.
    assert(!StagePosition(0, 0, 1920, 1080, 1920, 1080, 1, 1, x, y));
    assert(!StagePosition(1920, 1080, 0, 0, 1920, 1080, 1, 1, x, y));
    std::puts("menu pointer: PASS (stage position: fit, letterbox, pillarbox, scaled window, ultrawide canvas)");
    return 0;
}
