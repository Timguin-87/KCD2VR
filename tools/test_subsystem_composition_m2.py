#!/usr/bin/env python3
"""Prove source-only M2 patch composition in isolated clones at a frozen commit.

Never modifies the input checkout, installs files, or runs the linked harness.
All scratch clones, patches, command logs and the receipt are retained.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import zipfile


HOSTS = ("src/mod.cpp", "src/mod.h", "src/vr_menu.cpp", "src/vr_menu.h", "src/action_time_owner59c11.cpp")


def sha(data):
    return hashlib.sha256(data).hexdigest()


def load_module(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


def marker_for(lane):
    if not re.fullmatch(r"[a-z][a-z0-9_]*", lane):
        raise ValueError("noncanonical module identifier")
    return "M2_COMPOSITION_" + lane


def descriptor_edit(raw, lane):
    """An actual C++ translation-unit contribution, with no runtime side effect."""
    marker = marker_for(lane)
    newline = b"\r\n" if b"\r\n" in raw else b"\n"
    return raw + newline + ('static_assert(sizeof(char) == 1, "' + marker + '");').encode() + newline


def host_edit(raw, lane):
    """Insert INSIDE a selected lexical body, never after the dispatch #endif."""
    match = re.search(rb"^[ \t]*//[ \t]*M2_SLOT_END[ \t]+([0-9]+)[ \t]*\r?$", raw, re.MULTILINE)
    if match is None:
        raise ValueError("host has no lexical slot")
    newline = b"\r\n" if b"\r\n" in raw else b"\n"
    marker = "M2_COMPOSITION_HOST_" + lane
    return raw[:match.start()] + b"// " + marker.encode() + newline + raw[match.start():], int(match.group(1)), marker


def helper_edit(raw):
    """Preserve descriptor version 1 through an external helper link reference."""
    needle = b"ModuleDescriptor d{};"
    if raw.count(needle) != 1:
        raise ValueError("VRMap descriptor no longer has the qualified initializer")
    newline = b"\r\n" if b"\r\n" in raw else b"\n"
    return b'#include "composition_helper_m2.h"' + newline + raw.replace(needle, needle + b" d.module_version=CompositionHelperM2();", 1)


def protected_snapshot(root, names):
    return {name: sha((root / name).read_bytes()) for name in names}


class Proof:
    def __init__(self, root, commit, scratch, build_helper):
        self.root, self.commit, self.scratch = root, commit, scratch
        self.build_helper = build_helper
        self.count = 0
        self.commands = []
        self.receipt = {"schema": 1, "scope": "source-only overlays; optional pure C++ compile/link; no game/native execution", "source": str(root), "commit": commit, "scratch": str(scratch), "assertions": 0, "status": "RUNNING"}

    def check(self, condition, message):
        self.count += 1
        if not condition:
            raise AssertionError(message)

    def run(self, args, cwd=None, expect_success=True):
        result = subprocess.run([str(a) for a in args], cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=900)
        filename = "command-" + str(len(self.commands) + 1).zfill(3) + ".log"
        (self.scratch / filename).write_bytes(result.stdout)
        self.commands.append({"argv": [str(a) for a in args], "cwd": str(cwd) if cwd else None, "returncode": result.returncode, "expected_success": expect_success, "log": filename})
        self.check((result.returncode == 0) == expect_success, "unexpected command result: " + " ".join(map(str, args)) + "; see " + filename)
        return result.stdout

    def git(self, root, *args):
        return self.run(["git", "-C", root, *args])

    def checkout(self, seed, name):
        target = self.scratch / name
        self.git(seed, "worktree", "add", "--detach", target, self.commit)
        return target

    def validate_archive(self, path, lane):
        with zipfile.ZipFile(path) as archive:
            self.check(archive.testzip() is None, "source patch CRC failure")
            meta = json.loads(archive.read("patch.json"))
            self.check(meta["module"] == lane and meta["from_commit"] == self.commit, "patch not independently based on frozen commit")
            self.check(all(name == "patch.json" or name.startswith("source/src/") for name in archive.namelist()), "non-source patch member")
            self.check(not any(Path(name).suffix.lower() in (".dll", ".asi", ".exe", ".ini", ".pak") for name in archive.namelist()), "runtime payload in source patch")
            return meta

    def immutable(self, root, baseline):
        self.check(protected_snapshot(root, baseline) == baseline, "protected base bytes changed")

    def execute(self):
        # All mutable Git metadata below belongs to the new scratch clone.
        initial_head = self.git(self.root, "rev-parse", "HEAD").decode().strip()
        initial_status = self.git(self.root, "status", "--porcelain", "--untracked-files=all")
        self.check(initial_head == self.commit, "input HEAD must be the explicit frozen commit")
        self.check(not initial_status, "input checkout must be clean; wait for final freeze")
        manifest = json.loads((self.root / "subsystems.json").read_bytes())
        original_base = protected_snapshot(self.root, manifest["base_files"])
        self.run(["git", "clone", "--shared", "--no-checkout", self.root, self.scratch / "seed"])
        seed = self.scratch / "seed"
        self.git(seed, "config", "core.autocrlf", "false")
        self.git(seed, "checkout", "--detach", self.commit)
        patcher = load_module(seed / "tools/subsystem_patch.py", "m2_composition_patcher")
        reader = load_module(seed / "tools/source_text.py", "m2_composition_reader")
        self.check(patcher.read_manifest(seed)[0] == manifest, "cloned ownership contract differs")
        lanes = sorted(set(manifest["modules"]) - {"base"})
        self.check(len(lanes) == 16, "expected all 16 non-base lanes")
        author = self.checkout(seed, "author")
        patch_dir = self.scratch / "patches"
        patch_dir.mkdir()
        expected, host_markers, patch_paths = {}, {}, {}
        for lane in lanes:
            module = f"src/subsystems/{lane}/module.cpp"
            originals = {module: (author / module).read_bytes()}
            changes = {module: descriptor_edit(originals[module], lane)}
            host = f"src/subsystems/{lane}/host.inl"
            if (author / host).is_file() and b"M2_SLOT_END" in (author / host).read_bytes():
                originals[host] = (author / host).read_bytes()
                changes[host], slot, marker = host_edit(originals[host], lane)
                host_markers[lane] = {"path": host, "slot": slot, "marker": marker}
            try:
                for name, data in changes.items():
                    (author / name).write_bytes(data)
                archive = patch_dir / (lane + ".zip")
                patcher.create(author, lane, self.commit, archive)
                meta = self.validate_archive(archive, lane)
                self.check({entry["path"] for entry in meta["files"]} == set(changes), "patch lost actual owned source edits")
                if lane in host_markers:
                    marker = host_markers[lane]["marker"]
                    self.check(any(marker in reader.read_source(author / host_path, root=author) for host_path in HOSTS), "marked old host did not observe owned lexical edit: " + lane)
                patch_paths[lane] = archive
                expected.update(changes)
            finally:
                for name, data in originals.items():
                    (author / name).write_bytes(data)
        self.check(not self.git(author, "status", "--porcelain"), "author did not restore its own edits")
        # Independent, unrelated Alchemy work is intentionally on a different
        # existing owned file than that lane's module.cpp overlay.
        sentinel_path = "src/subsystems/alchemy/module.h"
        sentinel = (seed / sentinel_path).read_bytes() + b"\n// M2_INDEPENDENT_ALCHEMY_WORK_PRESERVED\n"
        outcomes = []
        for name, order in (("forward", lanes), ("reverse", list(reversed(lanes)))):
            target = self.checkout(seed, name)
            baseline = protected_snapshot(target, manifest["base_files"])
            (target / sentinel_path).write_bytes(sentinel)
            counts = []
            for lane in order:
                _, count = patcher.apply(target, patch_paths[lane])
                self.check(count > 0, "first application must write owned source")
                self.check((target / sentinel_path).read_bytes() == sentinel, "unrelated Alchemy work overwritten")
                self.immutable(target, baseline)
                counts.append({"lane": lane, "writes": count})
            for path, data in expected.items():
                self.check((target / path).read_bytes() == data, "composed source payload differs: " + path)
            for info in host_markers.values():
                self.check(any(info["marker"] in reader.read_source(target / h, root=target) for h in HOSTS), "composed old-host view missed slot edit")
            before_repeat = {p: (target / p).read_bytes() for p in expected}
            for lane in order:
                self.check(patcher.apply(target, patch_paths[lane])[1] == 0, "patch repeat must be idempotent")
            self.check(before_repeat == {p: (target / p).read_bytes() for p in expected}, "idempotent apply changed bytes")
            self.immutable(target, baseline)
            # Refuse a real concurrent edit, without partially applying peers.
            conflict_path = f"src/subsystems/{lanes[0]}/module.cpp"
            (target / conflict_path).write_bytes(expected[conflict_path] + b"\n// concurrent same-file edit\n")
            before_refusal = {p: (target / p).read_bytes() for p in expected}
            try:
                patcher.apply(target, patch_paths[lanes[0]])
            except patcher.PatchError as exc:
                self.check("same-file conflict" in str(exc), "conflict refused for wrong reason")
            else:
                self.check(False, "same-file conflict silently accepted")
            self.check(before_refusal == {p: (target / p).read_bytes() for p in expected}, "refused patch partially modified source")
            (target / conflict_path).write_bytes(expected[conflict_path])
            outcomes.append({p: sha((target / p).read_bytes()) for p in (*expected, sentinel_path)})
            self.receipt[name] = counts
        self.check(outcomes[0] == outcomes[1], "opposite application orders have different results")
        self.receipt["lanes"] = lanes
        self.receipt["host_markers"] = host_markers
        self.helper_proof(seed, patcher, manifest)
        self.immutable(self.root, original_base)
        self.check(self.git(self.root, "rev-parse", "HEAD").decode().strip() == initial_head, "input HEAD changed during proof")
        self.check(self.git(self.root, "status", "--porcelain", "--untracked-files=all") == initial_status, "input checkout changed during proof")
        self.receipt["protected_base_count"] = len(original_base)

    def helper_proof(self, seed, patcher, manifest):
        author = self.checkout(seed, "helper-author")
        target = self.checkout(seed, "helper-recipient")
        baseline = protected_snapshot(target, manifest["base_files"])
        prefix = "src/subsystems/vrmap/"
        header = prefix + "composition_helper_m2.h"
        source = prefix + "composition_helper_m2.cpp"
        descriptor = prefix + "module.cpp"
        listing = prefix + "sources.json"
        (author / header).write_text('#pragma once\n#include <cstdint>\nnamespace kcd2vr::subsystems::vrmap { std::uint32_t CompositionHelperM2() noexcept; }\n', encoding="utf-8", newline="\n")
        (author / source).write_text('#include "composition_helper_m2.h"\nnamespace kcd2vr::subsystems::vrmap { std::uint32_t CompositionHelperM2() noexcept { return 1; } }\n', encoding="utf-8", newline="\n")
        (author / descriptor).write_bytes(helper_edit((author / descriptor).read_bytes()))
        sources = json.loads((author / listing).read_bytes())
        sources.append(source)
        (author / listing).write_text(json.dumps(sources, indent=2) + "\n", encoding="utf-8", newline="\n")
        archive = self.scratch / "patches/vrmap-added-tu.zip"
        patcher.create(author, "vrmap", self.commit, archive)
        meta = self.validate_archive(archive, "vrmap")
        self.check({x["path"] for x in meta["files"]} == {header, source, descriptor, listing}, "new TU needed changes outside its owned API/source list")
        self.check(patcher.apply(target, archive)[1] == 4, "new TU overlay lost owned helper inputs")
        self.run([sys.executable, "-B", target / "tools/check_subsystems.py"], cwd=target)
        self.immutable(target, baseline)
        self.check(source in json.loads((target / listing).read_bytes()), "helper was not discovered in owned source list")
        harness = self.scratch / "helper-link-harness"
        harness.mkdir()
        (harness / "main.cpp").write_text('#include "subsystems/vrmap/module.h"\nint main() { return kcd2vr::subsystems::vrmap::Descriptor().module_version == 1 ? 0 : 1; }\n', encoding="utf-8", newline="\n")
        # This harness reads the real source-list data through CMake JSON. It
        # deliberately compiles only the two pure translation units; the real
        # native runtime remains covered by the parent's final-project link.
        cmake = '''cmake_minimum_required(VERSION 3.19)
project(M2OwnedTuLink LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
file(READ "${CHECKOUT}/src/subsystems/vrmap/sources.json" owned_json)
string(JSON count LENGTH "${owned_json}")
math(EXPR last "${count} - 1")
set(pure_sources)
foreach(i RANGE 0 ${last})
 string(JSON source GET "${owned_json}" ${i})
 if(source MATCHES "src/subsystems/vrmap/(module|composition_helper_m2)\\\\.cpp$")
  list(APPEND pure_sources "${CHECKOUT}/${source}")
 endif()
endforeach()
list(LENGTH pure_sources pure_count)
if(NOT pure_count EQUAL 2)
 message(FATAL_ERROR "owned sources.json failed helper discovery")
endif()
add_executable(owned_tu_link main.cpp ${pure_sources})
target_include_directories(owned_tu_link PRIVATE "${CHECKOUT}/src")
add_executable(missing_helper EXCLUDE_FROM_ALL main.cpp "${CHECKOUT}/src/subsystems/vrmap/module.cpp")
target_include_directories(missing_helper PRIVATE "${CHECKOUT}/src")
'''
        (harness / "CMakeLists.txt").write_text(cmake, encoding="utf-8", newline="\n")
        build = self.scratch / "helper-link-build"
        plan = {"configure": ["cmake", "-S", str(harness), "-B", str(build), "-DCHECKOUT=" + target.as_posix()], "positive": ["cmake", "--build", str(build), "--config", "Release", "--target", "owned_tu_link"], "negative": ["cmake", "--build", str(build), "--config", "Release", "--target", "missing_helper"]}
        if os.name == "nt":
            plan["configure"] += ["-G", "Visual Studio 17 2022", "-A", "x64"]
        if self.build_helper:
            self.run(plan["configure"])
            output = self.run(plan["negative"], expect_success=False).decode(errors="replace")
            self.check("CompositionHelperM2" in output and ("unresolved" in output.lower() or "undefined reference" in output.lower()), "negative control failed for a reason other than missing helper link")
            self.run(plan["positive"])
        self.receipt["added_tu"] = {"owned_changes": [header, source, descriptor, listing], "partition_checker": "PASS", "pure_compile_link": "PASS; executable NOT RUN" if self.build_helper else "NOT RUN; exact commands emitted", "negative_control": "expected unresolved external PASS" if self.build_helper else "NOT RUN", "plan": plan, "full_project_target": "KCD2VR_vrmap (parent full-project build is separate evidence)"}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--commit", required=True, help="exact frozen 40-hex commit; input checkout must be clean")
    parser.add_argument("--scratch", required=True, type=Path, help="new or empty owner directory OUTSIDE input checkout; retained")
    parser.add_argument("--build-helper", action="store_true", help="compile/link pure C++ positive and negative controls; never run result")
    args = parser.parse_args()
    root, scratch = args.root.resolve(strict=True), args.scratch.resolve()
    if not re.fullmatch(r"[0-9a-f]{40}", args.commit):
        parser.error("--commit must be an exact lower-case full SHA")
    if scratch.is_relative_to(root) or root.is_relative_to(scratch):
        parser.error("scratch and input checkout must be disjoint")
    if scratch.exists() and (not scratch.is_dir() or any(scratch.iterdir())):
        parser.error("scratch must be new or empty; this tool never deletes previous evidence")
    scratch.mkdir(parents=True, exist_ok=True)
    proof = Proof(root, args.commit, scratch, args.build_helper)
    try:
        proof.execute()
        proof.receipt["status"] = "PASS"
    except Exception as exc:
        proof.receipt.update(status="FAIL", error=str(exc))
    proof.receipt.update(assertions=proof.count, commands=proof.commands)
    (scratch / "COMPOSITION_RECEIPT.json").write_text(json.dumps(proof.receipt, indent=2) + "\n", encoding="utf-8", newline="\n")
    print(proof.receipt["status"], proof.count, "assertions;", scratch / "COMPOSITION_RECEIPT.json")
    if proof.receipt["status"] != "PASS":
        print(proof.receipt.get("error"), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
