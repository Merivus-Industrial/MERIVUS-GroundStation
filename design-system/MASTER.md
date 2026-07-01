# MERIVUS QGC Design System

Implementation target: desktop Qt 5.15.2, QML, Qt Quick Controls 2, and
QGroundControl controls. This file overrides generic web/mobile design advice.

## Product character

- Calm, operational, precise, information-dense, and trustworthy.
- Modern without neon HUD styling, glass effects, decorative blinking, or
  continuous animation.
- Selection gestures accelerate work; they are not the visual theme.

## Layout

- Primary canvas: map and spatial mission context.
- Left/compact overlay: fleet, groups, and filters.
- Right panel: contextual details, tasks, alerts, or assistant.
- Persistent status: link, GNSS/RTK, command state, and selected targets.
- Use an 8 px spacing base with 4 px half steps.
- Verify at 1920x1080 and 1366x768 without hiding safety-critical context.

## Semantic tokens

Map tokens to the existing QGC palette rather than fixed component colors:

- `surface/base`, `surface/raised`, `surface/overlay`
- `text/primary`, `text/secondary`, `text/disabled`
- `status/nominal`, `status/advisory`, `status/warning`, `status/critical`
- `selection/single`, `selection/group`, `selection/all`
- `link/connected`, `link/degraded`, `link/offline`, `telemetry/stale`
- `rtk/none`, `rtk/float`, `rtk/fixed`
- `command/queued`, `command/sent`, `command/ack`, `command/failed`

Never use red/green or color alone to distinguish states.

## Typography and motion

- Use the QGC/system sans-serif stack and translation APIs.
- Use tabular figures for coordinates, altitude, latency, battery, timestamps,
  and countdowns.
- Use motion only to explain state or spatial continuity.
- Critical alerts may pulse briefly, then remain persistent and readable.
- Never delay cancellation or operator input for animation.

## Required interaction behavior

- Freeze and display the resolved target set when confirmation opens.
- Keep selection and command scope visible while commands are pending.
- Show per-vehicle progress and partial outcomes.
- Provide visible alternatives to gesture-only operations.
- Define normal, focused, pressed, disabled, busy, warning, error, timeout,
  offline, stale, cancelled, and partial-success states where applicable.
