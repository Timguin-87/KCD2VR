# V46.8 Test Guide

1. Load into gameplay while unarmed and able to box.
2. Press **F8** once.
3. Observe the first 25 seconds (mono reference + sequential/native observer phases). Note any visual change.
4. A parchment page appears for 2 seconds: **Record Boxing — Starting in 2...**
5. When the page closes itself, immediately box normally with mouse + keyboard for about 10 seconds. Throw several punches; moving/guarding is fine.
6. Capture ends automatically and MIS restores the pre-test mono settings.
7. Send `KCD2VR-debug*.log` and `KCD2VR.log`.

Useful report: whether any stereo phase changed visually, whether the countdown appeared/closed correctly, and whether boxing felt normal during capture.
