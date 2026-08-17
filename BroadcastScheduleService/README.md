# Broadcast Schedule Service

Cross-platform C++17 service that executes due rows in `pa_broadcast_schedule` for one configured `LOCATION_ID` only. It supports concurrent execution of multiple due schedules, records each run in `pa_broadcast_schedule_log`, and advances `NEXT_RUN_AT` transactionally before dispatching work.

The default executor implements PAManager's M44 prerecorded-message protocol directly: it connects to the configured PA device, sends M44, and records success only after receiving A44. The C++ `IBroadcastExecutor` interface remains available for a future protocol variant.

## Prerequisites

- CMake 3.10 or later
- C++17 compiler: Visual Studio 2017 (15.7+) on Windows, or GCC 7+ / Clang 5+ on Ubuntu
- MySQL Connector/C or MariaDB Connector/C development files
- The tables in `../schedule_sql.txt` applied to the target MySQL database

Ubuntu installation example:

```bash
sudo apt-get install build-essential cmake libmariadb-dev
mkdir build && cd build
cmake ..
cmake --build . -j
```

Visual Studio 2017 example (Developer Command Prompt):

```bat
mkdir build-vs2017 && cd build-vs2017
cmake .. -G "Visual Studio 15 2017" -A x64 ^
  -DMYSQL_INCLUDE_DIR=C:\mysql\include ^
  -DMYSQL_LIBRARY=C:\mysql\lib\libmysql.lib
cmake --build . --config Release
```

The generated Visual Studio 2017 project is `build-vs2017/BroadcastScheduleService.sln`. Its `Release` directory is a runnable Win32 package: `BroadcastScheduleService.exe`, `libmariadb.dll`, `MSVCP140.dll`, `VCRUNTIME140.dll`, `vc_redist.x86.exe`, and `start_service.bat` are kept together. Edit the variables at the top of `start_service.bat` before starting it. `vc_redist.x86.exe` is provided for machines without the Visual C++ runtime.

## Run

Set `PA_SCHEDULER_DB_PASSWORD` instead of placing a password on the command line when possible.

```bash
export PA_SCHEDULER_DB_PASSWORD='secret'
./build/BroadcastScheduleService \
  --location-id 3 --db-host 127.0.0.1 --db-user pa_service --db-name pa \
  --poll-seconds 2 --pa-host 10.10.20.30 --pa-port 5000
```

Optional protocol header parameters mirror `PaTcpClient`: `--pa-server-id` (default `1`), `--pa-console-id` (default `54`), `--pa-version` (default `00001`), `--pa-line-id` (default `000`), and `--pa-station-id` (default six spaces). `--pa-connect-timeout-ms` and `--pa-response-timeout-ms` default to 5000 ms and 4000 ms.

`Start()` and `Stop()` are thread-safe, idempotent lifecycle APIs. `Stop()` stops new dispatches, requests cancellation from every executor, and waits for current executions to return. The command adapter cannot forcibly terminate a child process; a custom executor should honor its `stopRequested` argument for immediate protocol-level cancellation.

## Database TLS certificate verification

The service verifies the database server certificate by default. Use `--db-tls-verify 0` only for an internal database using a self-signed certificate; this avoids the certificate-chain error but does not authenticate the server certificate. The bundled `start_service.bat` sets this to `0` as an editable deployment default. Set `DB_TLS_VERIFY=1` after the database CA certificate is trusted on the service host.

## Scheduling and safety

- Only `LOCATION_ID = --location-id` rows are selected.
- A transaction locks due rows, advances `NEXT_RUN_AT`, and creates an `IN_PROGRESS` log row before execution. This prevents duplicate dispatch by multiple service processes using the same database.
- `ONCE` schedules are cleared after dispatch. `DAILY` and `WEEKLY` schedules use `REPEAT_INTERVAL`; weekly schedules use Monday-to-Sunday bits 0–6 of `WEEKDAY_MASK`.
- Jobs from a polling batch run asynchronously, so multiple due schedules can execute at the same time. Each job uses its own PA TCP connection, making M44 request/response handling independent.
