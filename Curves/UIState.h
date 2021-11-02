#pragma once

#include <glm/glm.hpp>
#include <iostream>


enum class State {
    Nothing,
    Highlighting,
    Dragging,
    Creating,
    Selected,
    Tangent,
    Transition,
    TangentHighlighting
};

/// Finite state machine because apparently UI control is very annoying.
/// Here's how it works:
///                       ┌────────┐
///                       │CREATING├──────────────┐
///                       └────────┘              │
///                            ▲                  │
///                            │MOUSEUP           │
///                            │                  ▼
///                        ┌───┴───┐  HOVER   ┌────────────┐
///                        │NOTHING├─────────►│HIGHLIGHTING│
///                        └───────┘          └───┬────────┘
///                            ▲                  │
///                            │MOUSEUP           │
///                            │                  │
///                      ┌─────┴────┐             │MOUSEDOWN
///                      │TRANSITION│             │
///                      └──────────┘             │
///                            ▲                  │
///                MOUSEDOWN ON│                  │
///                     NOTHING│                  ▼
///                        ┌───┴────┐ MOUSEUP ┌────────┐
///                 ┌──────┤SELECTED│◄────────┤DRAGGING│
///                 │      └───┬────┘         └────────┘
/// HOVER ON TANGENT│        ▲ │                  ▲
///                 │        │ │                  │
///                 ▼        │ │                  │
///   ┌─────────────────┐    │ └──────────────────┘
///   │TANGENT_HIGHLIGHT│    │  MOUSEDOWN WHILE IN
///   └─────────────┬───┘    │  RANGE OF HIGHLIGHT
///                 │        │
///        MOUSEDOWN│        │
///       ON TANGENT│        │
///                 ▼        │
///           ┌───────┐   MUP│
///           │TANGENT├──────┘
///           └───────┘
///
class UIState {
public:
    UIState() {
        state = State::Nothing;

        highlighting_item = { -1, 0.0f };
        tangent_highlighting_item = { -1, 0.0f };
    }

    State state;

    /// Records the current highlighting item and its distance.
    std::pair<int, float> highlighting_item;
    std::pair<int, float> tangent_highlighting_item;
    int dragging_item;
    int selected_item;
    glm::vec2 mouse_pos;
    glm::vec2 offset;
};
