# Stereo help-string xref probe

Read-only probe for the surviving `dual rendering` and `stereo mode` phrases.

Earlier probes searched the exact phrase RVAs. This build first walks backward to
the start of each containing NUL-terminated help string, then scans WHGame's
executable pages for references to that actual string start.

Useful prefixes:
- STEREO-HELPSTRING target
- STEREO-HELPSTRING containing-string
- STEREO-HELPSTRING xref
- STEREO-HELPSTRING nearby-call
- STEREO-HELPSTRING nearby-rip

Use DiagnosticRendering=false.
