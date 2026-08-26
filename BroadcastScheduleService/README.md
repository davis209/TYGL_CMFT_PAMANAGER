# Broadcast Schedule Service

Cross-platform C++17 service that executes due rows in `pa_broadcast_schedule` for one configured `LOCATION_ID` only. It supports concurrent execution of multiple due schedules, records each run in `pa_broadcast_schedule_log`, and advances `NEXT_RUN_AT` transactionally before dispatching work. Database access is provided by `TA_IRS_Core::PaBroadcastScheduleAccessFactory` under `BroadcastScheduleService/src`; its MySQL statements are registered under `[PaBroadcastSchedule]` in `core/data_access_interface/sqlcode.txt`.

The default executor implements PAManager's M44 prerecorded-message protocol directly. It maintains one shared TCP connection to the configured PA device, sends an initial and 60-second heartbeat, serializes M44 requests, and records success only after receiving A44. The C++ `IBroadcastExecutor` interface remains available for a future protocol variant.

## Prerequisites

- CMake 3.10 or later
- C++17 compiler: Visual Studio 2017 (15.7+) on Windows, or GCC 7+ / Clang 5+ on Ubuntu
- The existing Transactive `TA_DAI` and `TA_PADAI` data access libraries
- The tables in `../schedule_sql.txt` applied to the PA database configured for `DatabaseFactory`

Ubuntu installation example:

```bash
sudo apt-get install build-essential cmake
mkdir build && cd build
cmake ..
cmake --build . -j
```

Visual Studio 2017

Open `build-vs2017/BroadcastScheduleService.sln` directly in Visual Studio 2017, select `Release | Win32`, then build. This is a native MSBuild project: source, include, library and output paths are all relative to `build-vs2017`, so the entire `BroadcastScheduleService` directory can be copied to another location and built without regenerating the solution.

The Visual Studio project must be included in the original Transactive build so it can link with `TA_DAI` and `TA_PADAI`; it no longer links with MariaDB Connector/C directly.

The existing `build-vs2017/Release` directory is a runnable Win32 package: `BroadcastScheduleService.exe`, `MSVCP140.dll`, `VCRUNTIME140.dll`, `vc_redist.x86.exe`, and `start_service.bat` are kept together. Edit the location and PA device variables at the top of `start_service.bat` before starting it. `vc_redist.x86.exe` is provided for machines without the Visual C++ runtime.

## Run

```bash
./build/BroadcastScheduleService \
  --location-id 3 \
  --poll-seconds 2 --pa-host 10.10.20.30 --pa-port 5000
```

Optional protocol header parameters mirror `PaTcpClient`: `--pa-server-id` (default `1`), `--pa-console-id` (default `54`), `--pa-version` (default `00001`), `--pa-line-id` (default `000`), and `--pa-station-id` (default six spaces). `--pa-connect-timeout-ms` and `--pa-response-timeout-ms` default to 5000 ms and 4000 ms.

`Start()` and `Stop()` are thread-safe, idempotent lifecycle APIs. `Stop()` stops new dispatches, requests cancellation from every executor, and waits for current executions to return. The command adapter cannot forcibly terminate a child process; a custom executor should honor its `stopRequested` argument for immediate protocol-level cancellation.

Database host, port, user, password, TLS and schema settings are not service command-line options. They must be configured through the original project's `DatabaseFactory`/`Pa_Cd` database configuration.

## Scheduling and safety

- Only `LOCATION_ID = --location-id` rows are selected.
- A transaction locks due rows, advances `NEXT_RUN_AT`, and creates an `IN_PROGRESS` log row before execution. This prevents duplicate dispatch by multiple service processes using the same database.
- `ONCE` schedules are cleared after dispatch. `DAILY` and `WEEKLY` schedules use `REPEAT_INTERVAL`; weekly schedules use Monday-to-Sunday bits 0–6 of `WEEKDAY_MASK`.
- Jobs from a polling batch run asynchronously, so multiple due schedules can execute at the same time. M44 device requests are serialized over the shared PA TCP connection, matching PAManager's single-session behavior.
