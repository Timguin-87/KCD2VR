# V33 - Renderer stereo direct backing probe

V32 installed the correct generic CVar helpers but observed no renderer-stereo registrations because WHGame+0x10178xx runs before the late ASI hook. V33 reads the fixed backing globals recovered statically from that block. It is read-only.

Key markers: `PIPELINE-V33`, `STEREO-RENDERER-V33 direct`.

Recovered defaults visible in the DLL include r_StereoEyeDist=0.02 and r_StereoScreenDist=0.25.
