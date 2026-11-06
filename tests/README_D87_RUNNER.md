# Running the regression suite on Windows

Use Visual Studio 2022 C++ tools, CMake, and Python 3. Install the Python test dependency with `python -m pip install -r tests/requirements-host.txt` in your test environment. The game installation is not a test dependency to modify.

Run `powershell -NoProfile -ExecutionPolicy Bypass -File tools/run_regressions_d87.ps1 -OutputDirectory <absolute-output-path> -Python <python-executable>`. Add `-Asan` for AddressSanitizer. The runner finds the installed sanitizer runtime and enables assertions. The shell runner delegates to this runner on Windows.

The full suite currently has historical fixture failures. A successful focused run is not a substitute for the full result. Configuration, compilation, CTest and Python results have separate exit codes. Compilation errors do not prevent CTest from reporting the remaining runnable tests. Missing executables remain NOT RUN. Python errors and missing historical assets remain errors; they are not silently skipped.

Some tests compile extracted production code against fake engine fixtures. When production signatures change, update the extractor or the fixture's external dependencies, then regenerate the evidence. Do not alter production behavior, discard an assertion, or rewrite a generated file to obtain a pass. A changed expectation needs a documented behavior decision.

The D87 audit repaired moved extraction anchors, duplicate CMake targets, the language standard, Windows macro collisions, roomscale fixture inputs, publication-history fixture setup, and build-marker checks. It did not finish migrating every historical fixture. The complete result matrix and remaining failures are delivered with the audit patch.

Host tests check code and state transitions. They do not validate actual headset images, GPU resource contents, engine injection or native combat outcomes. In particular, successful DLSS SDK calls and distinct eye handles do not prove that either submitted eye image is correct.
