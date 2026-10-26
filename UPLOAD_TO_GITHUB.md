# Upload the complete Minimal Plus v1.0.27 source

This is the complete repository source. It includes BOTH corrected GitHub test files:

- `tests/mocks/pebble.h` (recognizes CenterAccent and CenterDateRow)
- `tests/watch_test.c` (recognizes compact battery percentage rendering)

To upload through the GitHub web interface:
1. Extract the ZIP on your computer.
2. Open your existing GitHub watchface repository on its default branch.
3. Use **Add file > Upload files** and drag the *contents* of this extracted folder into the upload area. Include the `.github` folder so the workflow is included; depending on your operating system, hidden folders may need to be shown.
4. Commit the upload; existing files with the same paths should be replaced. If GitHub's web uploader refuses certain paths or directories, use GitHub Desktop to commit and push the entire extracted directory.

Do not upload the ZIP itself as a single repository file: GitHub Actions needs the extracted source files at the repository root.

For CloudPebble, see `CLOUDPEBBLE_UPDATE.md`. The two new message keys (CenterAccent and CenterDateRow) must be appended to the existing keys, not inserted in the middle.

Validation before packing:
- `clang` watch tests: passed
- Node.js Clay transport tests: passed
- Python defaults tests: passed
- `clang` and `gcc` Pebble SDK-compatible compile checks: passed
- No diagnostic logging identifiers in `src/c/main.c`

The true Pebble SDK `.pbw` build still needs GitHub Actions or a Pebble SDK installation.
