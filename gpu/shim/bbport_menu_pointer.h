// SPDX-License-Identifier: GPL-3.0-or-later
// bbport: the mouse as a pointer in the game's own menus (title screen, System, inventory,
// the pause grid): hovering a row moves the game's cursor to it, a left click chooses it (or
// steps an option's value by the arrow it landed on), a right click is Back and the wheel
// scrolls. Adapted from bbhost db5457cbd447f4ef129b58df0a78e0a8819c7033 through DarkIzuku's
// port of it (gpu/shim/input/menu_pointer.cpp); game version 1.09.
//
// Threads: the window thread feeds events (OnEvent, Pump); the pad thread asks once per sample
// (bbgpu_menu_pointer_sample in bbgpu.h); the game's own menu thread runs the hooked list
// update, which hit-tests the rows under the pointer. State shared between them is atomic or
// under one lock.
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

union SDL_Event;
struct SDL_Window;

namespace BbMenuPointer {

/// Loader, after the patches and before the image is protected: checks the list component's
/// code at its 1.09 offsets and hooks its update. Refuses (with one log line) on any other
/// executable. `image` is the flat memory image; bbhost's addresses are image + bn - 0x400000.
void Install(unsigned char* image, std::size_t size);
/// Hooks are in place.
bool Installed();

/// The game is in a menu the pointer should own: the front end as a whole, or in the world a
/// menu the flow runs or a list that has had focus within the last few frames. The mouse is
/// released from the camera (relative mode) while this holds.
bool InMenu();

/// Window thread, every SDL event. `blocked`: the port's own menu or text entry has the input.
void OnEvent(const SDL_Event& event, bool blocked);
/// Window thread, after the events: the window size, whether the mouse is captured (relative
/// mode) and whether the port's text dialog is open; shows or hides the system cursor.
void Pump(SDL_Window* window, bool captured, bool text_active, bool overlay_open);

/// The pointer in the Scaleform stage's pixels (1920x1080). The frame of `ow` x `oh` output
/// pixels is fitted into the window (letterboxed, centred); the game's UI canvas of `cw` x `ch`
/// covers that frame (1920x1080, or wider on ultrawide outputs where only the HUD canvas is
/// widened); the menus' stage sits centred in the canvas. Window units are whatever the mouse
/// position is in. False for an empty window or frame.
inline bool StagePosition(int ww, int wh, int ow, int oh, int cw, int ch, float mx, float my,
                          float& sx, float& sy) {
    if (ww <= 0 || wh <= 0 || ow <= 0 || oh <= 0 || cw <= 0 || ch <= 0) {
        return false;
    }
    const float present = std::min(float(ww) / float(ow), float(wh) / float(oh));
    const float fx = (float(ww) - float(ow) * present) * 0.5f;
    const float fy = (float(wh) - float(oh) * present) * 0.5f;
    const float cx = (mx - fx) / (float(ow) * present) * float(cw);
    const float cy = (my - fy) / (float(oh) * present) * float(ch);
    sx = cx - float(cw - 1920) * 0.5f;
    sy = cy - float(ch - 1080) * 0.5f;
    return true;
}

/// The pad's Confirm and Back buttons in this region of the game: Cross decides on the US and
/// European discs, Circle on the Japanese and Asian ones (read from the game's own setting).
std::uint32_t ConfirmButton();
std::uint32_t BackButton();

} // namespace BbMenuPointer
