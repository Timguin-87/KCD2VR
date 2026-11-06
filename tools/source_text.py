"""Read canonical static source through explicitly marked M2 forwarding leaves.

This is a source reader, not a C/C++ preprocessor. Ordinary includes, macros
and conditional directives remain untouched. No payload is imported/executed.
"""
from dataclasses import dataclass
import hashlib
from pathlib import Path, PurePosixPath
import re


class SourceError(ValueError):
    pass


_MARKED = re.compile(r'^[ \t]*#[ \t]*include[ \t]+"([^"\r\n]+)"[ \t]*//[ \t]*M2_OWNED_SOURCE[ \t]*(?:\n|$)', re.MULTILINE)
_SELECTOR = re.compile(
    r'^[ \t]*#define[ \t]+KCD2VR_M2_HOST_SLOT[ \t]+(\d+)[ \t]*//[ \t]*M2_SOURCE_SELECTOR[ \t]*\n'
    r'[ \t]*#include[ \t]+"([^"\r\n]+)"[ \t]*//[ \t]*M2_OWNED_SOURCE[ \t]+slot=(\d+)[ \t]*\n'
    r'[ \t]*#undef[ \t]+KCD2VR_M2_HOST_SLOT[ \t]*//[ \t]*M2_SOURCE_SELECTOR[ \t]*(?:\n|$)', re.MULTILINE)
_DEFAULT_ROOT = Path(__file__).resolve().parents[1]


def _slot_body(text, slot, name):
    begin = list(re.finditer(r'^[ \t]*//[ \t]*M2_SLOT_BEGIN[ \t]+' + str(slot) + r'[ \t]*\n', text, re.MULTILINE))
    end = list(re.finditer(r'^[ \t]*//[ \t]*M2_SLOT_END[ \t]+' + str(slot) + r'[ \t]*(?:\n|$)', text, re.MULTILINE))
    if len(begin) != 1 or len(end) != 1 or end[0].start() < begin[0].end():
        raise SourceError("missing/duplicate/reversed marked source slot: " + name + " slot=" + str(slot))
    # This explicit dispatch protocol is deliberately narrower than a C
    # preprocessor. A comment cannot authorize a body the compiler would skip.
    stack, selected, offset = [], None, 0
    for line in text.splitlines(keepends=True):
        if offset == begin[0].start():
            if len(stack) != 1 or stack[0][0] != slot or not stack[0][2]:
                raise SourceError("marked source slot guard mismatch: " + name + " slot=" + str(slot))
            selected = (stack[0][0], tuple(stack[0][1]))
        if offset == end[0].start():
            if len(stack) != 1 or selected != (stack[0][0], tuple(stack[0][1])):
                raise SourceError("marked source slot guard changed within body: " + name)
            return text[begin[0].end():end[0].start()]
        directive = re.match(r'^[ \t]*#[ \t]*(if|ifdef|ifndef|elif|else|endif)\b(.*)', line)
        if directive:
            op, expression = directive.groups()
            guard = re.fullmatch(r'[ \t]+KCD2VR_M2_HOST_SLOT[ \t]*==[ \t]*(0|[1-9][0-9]*)[ \t]*(?://[^\n]*)?\n?', expression)
            value = int(guard.group(1)) if guard else None
            if op in ("if", "ifdef", "ifndef"):
                stack.append([value, [value], op == "if" and guard is not None])
            elif not stack:
                raise SourceError("unbalanced marked source slot guard: " + name)
            elif op == "endif":
                stack.pop()
            else:
                current = stack[-1]
                current[2] = current[2] and op == "elif" and guard is not None and value not in current[1]
                current[0] = value
                current[1].append(value)
        if selected is not None and re.match(r'^[ \t]*#[ \t]*(?:define|undef)[ \t]+KCD2VR_M2_HOST_SLOT\b', line):
            raise SourceError("marked source slot redefines selector: " + name)
        offset += len(line)
    raise SourceError("unbalanced marked source slot guard: " + name)


@dataclass(frozen=True)
class Origin:
    path: str
    raw_sha256: str
    size: int


@dataclass(frozen=True)
class Expansion:
    text: str
    requested: str
    origins: tuple
    edges: tuple

    def provenance(self):
        return {
            "reader": "M2_OWNED_SOURCE-v1",
            "requested": self.requested,
            "expanded_sha256": hashlib.sha256(self.text.encode("utf-8")).hexdigest(),
            "origins": [vars(origin) for origin in self.origins],
            "edges": [{"from": a, "to": b, **({"slot": slot} if slot is not None else {})} for a, b, slot in self.edges],
        }


def expand_source(path, encoding="utf-8", errors="strict", *, root=None):
    repo = Path(root).resolve(strict=True) if root is not None else _DEFAULT_ROOT
    requested = Path(path).resolve(strict=True)
    origins = {}
    edges = []
    total = 0

    def inside(candidate):
        resolved = candidate.resolve(strict=True)
        if not resolved.is_relative_to(repo) or not resolved.is_file():
            raise SourceError("source escapes repository: " + str(candidate))
        return resolved

    def relative(candidate):
        return candidate.relative_to(repo).as_posix()

    def visit(candidate, stack, slot=None):
        nonlocal total
        current = inside(candidate)
        if current in stack:
            raise SourceError("marked source cycle: " + " -> ".join(relative(p) for p in (*stack, current)))
        if len(stack) >= 128:
            raise SourceError("marked source nesting exceeds 128")
        raw = current.read_bytes()
        total += len(raw)
        if total > 64 * 1024 * 1024:
            raise SourceError("expanded source exceeds 64 MiB")
        name = relative(current)
        origins.setdefault(name, Origin(name, hashlib.sha256(raw).hexdigest(), len(raw)))
        # Same universal-newline semantics as Path.read_text, before splicing.
        text = raw.decode(encoding, errors).replace("\r\n", "\n").replace("\r", "\n")
        if slot is not None:
            text = _slot_body(text, slot, name)

        def include_source(include, selected=None):
            pure = PurePosixPath(include)
            if pure.is_absolute() or "\\" in include or ":" in include:
                raise SourceError("absolute/nonportable marked include: " + include)
            candidates = (current.parent / include, repo / "src" / include)
            target = None
            for option in candidates:
                # Refuse an existing outside/symlink target rather than silently
                # selecting a different file from the root fallback.
                if option.exists():
                    target = inside(option)
                    break
            if target is None:
                raise SourceError("marked source not found: " + name + " -> " + include)
            edges.append((name, relative(target), selected))
            return visit(target, (*stack, current), selected)

        def select(match):
            if match.group(1) != match.group(3):
                raise SourceError("marked source selector mismatch: " + name)
            selected = int(match.group(1))
            if str(selected) != match.group(1) or selected > 2147483647:
                raise SourceError("marked source slot must be a bounded decimal integer: " + name)
            return include_source(match.group(2), selected)

        text = _SELECTOR.sub(select, text)
        if re.search(r'^[ \t]*#(?:define|undef)[^\n]*//[ \t]*M2_SOURCE_SELECTOR[ \t]*$', text, re.MULTILINE) or re.search(r'^[ \t]*#include[^\n]*//[ \t]*M2_OWNED_SOURCE[ \t]+slot=', text, re.MULTILINE):
            raise SourceError("malformed marked source selector: " + name)
        return _MARKED.sub(lambda match: include_source(match.group(1)), text)

    text = visit(requested, ())
    return Expansion(text, relative(requested), tuple(origins.values()), tuple(edges))


def read_source(path, encoding="utf-8", errors="strict", *, root=None):
    return expand_source(path, encoding, errors, root=root).text


def read_source_bytes(path, encoding="utf-8", errors="strict", *, root=None):
    """UTF-8 bytes of the normalized expanded source, suitable for excerpt SHA."""
    return read_source(path, encoding, errors, root=root).encode("utf-8")


def source_provenance(path, encoding="utf-8", errors="strict", *, root=None):
    return expand_source(path, encoding, errors, root=root).provenance()
