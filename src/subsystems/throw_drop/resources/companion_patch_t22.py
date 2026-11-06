"""Source-only in-memory T22 companion planner. No archive writer or installer.

Keep the current combined archive as input. Reuse the reviewed T20 role-table
migration, but NEVER export the old T18 conversion/escrow loader or Lua carrier.
Use only with a fresh process and the reviewed T22/T23/T24 DLL composition.
"""
import copy
import hashlib
from pathlib import Path
import companion_patch_t18 as prior

base = prior.base
RESERVATION = 'Scripts/KCD2VR/native_throw_reservation_t20.lua'
BOOTSTRAP = Path(__file__).with_name('startup_reservation_t22.lua')
SOURCE = Path(__file__).with_name('native_throw_reservation_t20.lua')
STARTUP_MEMBER = 'Scripts/Startup/zzkcd2vr_supply_t15.lua'
STARTUP_SOURCE = Path(__file__).with_name('startup_supply_t22.lua')


# Exact reviewed historical payload identities, not an old companion archive
# or the integrating chat's current source file. Unknown revisions still stop.
KNOWN_SUPPLY_SHA256 = frozenset({
    '162dcda1a32dd4a80d6ff6c75f48efbfd29c37d837c4e88314cc61a19196c9a4',
    'e7046553bba171cf4c0858dece5d447cfb0324f797c300f8a1ca25550b1e1d8e',
})
RETIRED_CARRIER_SHA256 = '38c6256c2add6830139fc6a838e0aea59d72b0feafb61fcefadeab0d5d1b30fa'


def plan_members(members, native):
    """Return copied members, changed names, removed names; never write a PAK."""
    folded = {}
    for name in members:
        base.safe_name(name, name.endswith('/'))
        base.require(name.casefold() not in folded, 'duplicate/case-alias member')
        folded[name.casefold()] = name
    for owned in (base.SUPPLY_MEMBER, prior.LUA, prior.TABLE, RESERVATION, STARTUP_MEMBER):
        base.require(owned.casefold() not in folded or folded[owned.casefold()] == owned,
                     'owned member case alias: ' + owned)
    known_supply = KNOWN_SUPPLY_SHA256 | {hashlib.sha256(BOOTSTRAP.read_bytes()).hexdigest()}
    base.require(base.SUPPLY_MEMBER in members and
                 hashlib.sha256(members[base.SUPPLY_MEMBER][1]).hexdigest() in known_supply,
                 'unknown supply entry: review current combined source before replacement')
    if prior.LUA in members:
        base.require(hashlib.sha256(members[prior.LUA][1]).hexdigest() == RETIRED_CARRIER_SHA256,
                     'unknown retired carrier source')
    if RESERVATION in members:
        base.require(members[RESERVATION][1] == SOURCE.read_bytes(), 'unknown reservation source')
    base.require(STARTUP_MEMBER in members and members[STARTUP_MEMBER][1] in
                 (Path(__file__).with_name('startup_supply_t15.lua').read_bytes(), STARTUP_SOURCE.read_bytes()),
                 'unknown or missing original supply startup')
    # Reject any second loader reference. The known T15 startup references the
    # SUPPLY_MEMBER entry and remains byte-identical. Never rewrite other scripts.
    for name, (_, data) in members.items():
        if name in (base.SUPPLY_MEMBER, prior.LUA, RESERVATION) or not name.lower().endswith('.lua'):
            continue
        base.require(b'native_ammo_supply_t18.lua' not in data and
                     b'native_throw_reservation_t20.lua' not in data,
                     'competing carrier/reservation loader: ' + name)
    # Reuse only the exact finite BrokenGUID/three-role schema validation.
    # Never read or manufacture a retired executor in an intermediate result.
    result, _ = prior.plan_members(copy.deepcopy(members), native, include_carrier=False)
    result.pop(prior.LUA, None)
    for name, data in ((base.SUPPLY_MEMBER, BOOTSTRAP.read_bytes()), (RESERVATION, SOURCE.read_bytes()),
                       (STARTUP_MEMBER, STARTUP_SOURCE.read_bytes())):
        info = copy.copy(result[name][0] if name in result else result[base.SUPPLY_MEMBER][0])
        info.filename = name
        result[name] = (info, data)
    changed = {n for n in result if n not in members or result[n][1] != members[n][1]}
    removed = set(members) - set(result)
    base.require(changed <= {base.SUPPLY_MEMBER, prior.TABLE, RESERVATION, STARTUP_MEMBER} and removed <= {prior.LUA},
                 'unrelated companion member changed')
    base.require(all(result[n][1] == data for n, (_, data) in members.items()
                     if n not in changed and n not in removed), 'unrelated payload changed')
    base.require(prior.LUA not in result and b'native_ammo_supply_t18.lua' not in result[base.SUPPLY_MEMBER][1],
                 'retired carrier survived composition')
    return result, changed, removed
