# Luminosity backlog

## Blockers
- [ ] **critical** `src/pkjs/openmeteoweatherdata.js:338` — weather XHR aborted after 1 s (`xhr.timeout = 1000`, ms). Own fixture `test-data.json` has `generationtime_ms 3006` before network RTT. No retry, no `status === 200` check → watch stuck on empty/stale cache. Fix: timeout ≥ 15000, check status, retry once.
- [ ] **high** `src/pkjs/openmeteoweatherdata.js:275` — forecast sliced at phone-local hour (`new Date().getHours()`) but Open-Meteo queried with `timezone=auto` → `hourly[0]` is location-local midnight. Ring/high-low window/staleness shift by tz offset; center temp (`json.current`) and ring disagree. Fix: derive start index from `json.current.time` / `utc_offset_seconds` vs `hourly.time`.
- [ ] **high** `src/c/settings.c:95-98` + `src/c/forecast.c:80-83` — cache persists hour-of-day only, no date; `(hour − fetched_hour + 24) % 24` ⇒ same clock hour next day = staleness 0, yesterday's forecast shown as fresh. Fix: persist date/unix hour, bump `CURRENT_WEATHER_STRUCT_VERSION`, discard cache older than forecast horizon.
- [ ] **high** `src/c/render.c:249-267` — round branch (chalk/gabbro) draws `COLOR_24H_FETCH` over `COLOR_24H_CURRENT` when fresh. Rect branch comment says fetch must be drawn first. Fix: reorder in `#else` branch. (Caught independently by both reviews.)
- [ ] **medium** `src/pkjs/index.js:52-56` — `CFG_OVERRIDE_LOC` on with empty coords → `Number('') = 0` → lat 0/lon 0 (Gulf of Guinea). Fix: finite-range validation; else fall back to geolocation.
- [ ] **medium** `src/pkjs/space.js:123-125` — midnight sun: sunrise→0, sunset→2π, both normalize to same hour ⇒ `is_night_hour()` true all day, ring drawn all-night. Fix: send sunset=24 sentinel, treat `sunset==0` as all-day.
- [ ] **medium** `src/c/messaging.c:6-12` — `send_message` writes dict without checking `app_message_outbox_begin`; null iterator when settings save overlaps in-flight 0/30-min ping → crash. Fix: bail unless `APP_MSG_OK`; log `outbox_failed_callback`.
- [ ] **medium** `package.json:10-13` — DECISION NEEDED: `health` capability required while `aplite` in `targetPlatforms` (no health API on aplite; capabilities gate installability). Fix: drop health or drop aplite.

## Cleanups (non-blocking)
- [ ] `src/c/declarations.c:51` vs `declarations.h:108` — `date_buffer` defined 11 B ("2999-99-99"), extern declares 37 (`HOURLY_WEATHER_DATA_BUFFER_LEN`); `update.c:17` `strftime(..., sizeof(...))` bound wrong. Latent (format fits today), fix the definition/extern agreement.
- [ ] `src/c/messaging.c:33-40` — settings/weather handlers deref sibling tuples unchecked after entering on one key; null-guard each tuple.
- [ ] `package.json` messageKeys — `FORECAST_WIND_HIGH`/`FORECAST_WIND_LOW` defined, never sent/read by anyone (pre-existing dead keys).
- [ ] `src/c/declarations.{h,c}` — empty `StringTable` struct + `extern strings` + global; no users. Delete.
- [ ] `src/pkjs/openmeteoweatherdata.js` — `testWeather()` + ~400-line fixture + committed `test-data.json`; wscript bundles every pkjs file. Strip before release (incl. commented debug lines `index.js:96-98`).
- [ ] Debug noise — per-tick `APP_LOG(INFO)` (callbacks.c:7,107; update.c:19,88; messaging.c:83,116), per-string `console.log` in pkjs; wscript `if False and hint` lint hook (pre-existing). Downgrade/remove.
- [ ] Trailing newlines missing at EOF: callbacks.c, forecast.c, render.c, render.h, settings.c, weather.c.
- [ ] refine layout on gabbro (round 2)
- [ ] refine layout on emery (time 2)
- [ ] add a state setting for state machine of weather data fetch. states are: waiting / stale (hourglass), no connection (phone?), no location (compass?), invalid weather data or server error (no data)
- [ ] calculate intensity strings onboard the watch, so we can advance through the 12 extra hours in the buffer when long-out-of-comms

