# MERIVUS Development Workflow

## Change flow

1. Classify the task: QML, C++, fleet logic, MAVLink, LTE/RTK, AI, hardware, or
   cross-layer.
2. Record the operator outcome, worst credible failure, allowed files, and
   acceptance evidence in `docs/templates/feature-brief.md`.
3. Trace the existing QGC extension point and QML-to-C++ data flow.
4. Design immutable targets, command states, timeout/cancel/retry behavior, and
   per-vehicle reconciliation before command implementation.
5. Plan a vertical slice:

   ```text
   domain contract/tests
     -> C++ model/controller
     -> QML presentation
     -> Mock/SITL integration
     -> UI and command-safety review
   ```

6. Verify with the smallest relevant checks, then expand to a full build and
   simulated integration.

## Verification ladder

```text
static/unit checks
  -> local Qt build
  -> Mock Link
  -> single-vehicle SITL
  -> multi-vehicle SITL with faults
  -> hardware-in-the-loop
  -> controlled field test
```

Real-aircraft testing always requires explicit human authorization and must not
be automated by Codex.

## Windows commands

```powershell
.\tools\dev\check-windows-environment.ps1
.\tools\dev\build-windows.ps1 -Configuration Release
```

Use `-Reconfigure` after `.pro`, `.pri`, resource, or toolchain changes. Use
`-Clean` only when an incremental build cannot be trusted.
