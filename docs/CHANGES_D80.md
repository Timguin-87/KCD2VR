# D80 historical repair notes

These notes describe the prior source revision. The November 1 D80 headset run
exposed failures that are addressed in [D81](CHANGES_D81.md); the old statements
below are not a D81 gameplay acceptance record.

The last run gave useful counterexamples. Grip alone was rejected merely because
unarmed mode was selected. A flask attachment appeared after the immediate read
had already failed. A contact fraction of zero pinned the whole held mortar,
including its rotation. Those paths now have separate, narrow corrections.

The pickup uses the requesting hand's actual Grip+Trigger state from one completed
pose. The flask operation is called once, then its exact attachment can arrive
later; that does not extend tracking grace. Held-tool contact uses current planes
and a small translation correction, leaving tangent motion and rotation responsive.
It remains a sparse approximation, not full tool-mesh physics.

Outside stations, an incomplete collision query no longer pins the entire
wrist or slowly returns it from an old surface. It retires that geometry and
follows the current tracked request. Repeated callbacks can reuse a completed
result only for the exact same publication and world-space hand features.
Known current contacts still block inward motion; unavailable coverage can
briefly allow clipping. No extra tracking grace or smoothing filter was added.

The hammer no longer freezes its native grip during the approach to the anvil,
when the authored slot is still moving. It remains controller-placed during
that transition and settles its grip at native anvil idle. Optional left-trigger
anvil assistance is momentary: hold for native placement, release for free
control. Heating and transfers stay free.

Crossbow reloads can pass their ordinary markerless transition at native speed,
then return to the gesture-driven cock/load phase. It does not grant loading
credit or create a bolt. A confirmed saddle gait now gets a connected upper-body
solve with native pelvis and legs. Mount, dismount and protected actions remain
native. Dice is recognized by the existing activity camera/input policy; no
camera-hook or stereo changes were made.

Station prompts use 30% size and can hide after ten seconds, returning for a new
work step or a look at an open palm. The old scaler relinquishes these children
before the movie takes over. Show Hints is in Immersion. PlayStation art uses the
same input mapping as Xbox. Nothing scales the whole HUD root.

Map markers keep stable slots and skip unchanged writes. An idle notification
path avoids taking its mutex. Failed NPC life and physics experiments stay OFF,
including hidden background work. The startup menu adds graphics choices and a
developer-mode checkbox; existing INI and calibration are preserved.

Optional gore is separate. Heads only is the normal enabled mode; Heads + limbs
remains available. The separate v62.39 pack adds fitted elbow/knee caps, enabled by default
in its script. It does not establish that hair-color or floor-contact issues
are fixed. Current native game sounds are used for the connected herb confirmations;
there is no generic replacement sound or replay of recipe/item effects.

The release validation report lists the focused checks and actual Windows link.
A successful link is not a headset result. General finger-edge jitter, all reload
variants, horse patting, NPC head follow and the complete FPS deficit remain open.
