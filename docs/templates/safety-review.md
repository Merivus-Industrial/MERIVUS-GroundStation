# Multi-vehicle command safety review

## Command

- Intent/action:
- Risk class:
- Target source:
- Frozen target system IDs:

## Preconditions

- [ ] Link and telemetry freshness
- [ ] Unique system IDs
- [ ] Position and RTK quality
- [ ] Flight mode/capability
- [ ] Armed state and altitude
- [ ] Battery/geofence/mission constraints

## Lifecycle

- [ ] Requested/validated/confirmed
- [ ] Queued/sent
- [ ] ACK/rejection
- [ ] Executing/completed
- [ ] Timeout/cancel/retry
- [ ] Partial-result reconciliation

## AI boundary

- [ ] Versioned structured schema
- [ ] No free-form MAVLink fields
- [ ] Ambiguity rejected
- [ ] Coordinates/units validated
- [ ] Deterministic preview and operator confirmation
- [ ] Injection, replay, and bypass tests

## Verification

- Mock/SITL scenario:
- Fault injection:
- Audit record:
- Rollback/abort:
- Reviewer decision:
