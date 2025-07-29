# AGENTS.md

## For Agentic Coding Agents
This repository is C++17, built for Visual Studio/MSBuild. There are no automated tests or linting tools; all testing is manual.

## Build & Run
- **Build:**
  - Open `.sln` (e.g., `Desktop.sln`) in Visual Studio 2022 and build (Debug/Release, x64/Win32/ARM64)
  - Or run: `msbuild <path-to-sln> /p:Configuration=Debug /p:Platform=x64`
- **Run:**
  - Executable: `<Project>/<Project>/x64/Debug/<Project>.exe` (adjust for output dir)

## Testing & Linting
- No automated/unit tests or test runners; all testing is manual by running the app and verifying behavior.
- No single-test or test suite commands.
- No linting or formatting tools are configured.

## Code Style Guidelines
- **C++ Standard:** C++17 (`<LanguageStandard>stdcpp17</LanguageStandard>`)
- **Naming:**
  - Classes: PascalCase (e.g., `Desktop`, `App`)
  - Variables/functions: camelCase (e.g., `frameTime`, `hasValidSelection`)
  - Constants: ALL_CAPS with underscores (e.g., `GRID_MIN_X`)
- **Imports:** Use `#include`; angle brackets for system headers, quotes for local headers.
- **Formatting:** 4 spaces per indent, braces on new lines, align parameters vertically.
- **Types:** Prefer explicit types, use `nullptr` for pointers, initialize all members.
- **Error Handling:** Use return values or exceptions; always check for null pointers and valid indices.

_No Cursor or Copilot rules are present in this repository._
