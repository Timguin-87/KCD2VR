"""Build a narrow proposal for the shared final native RequestDialog boundary.

Input must be exact retail dialoginitiator_inner.xml. This standalone builder
does not install or modify the companion PAK. Native BT compilation is untested.
"""
from pathlib import Path
import argparse
import hashlib
import xml.etree.ElementTree as ET

RETAIL_SHA = '8747688421a01070a0b71a2a231010f589c03c28d2505e0f6ab9a637f2097fa0'
VARIABLES = ('kcd2vrOriginalInitiator28', 'kcd2vrUseTwins28', 'kcd2vrMovePlayer28')
LUA = "data.kcd2vrOriginalInitiator28 = (System.GetCVar('wh_dlg_NoCameras') == 1 and System.GetCVar('wh_dlg_ForcePOV') == 1)"
CONDITION = '$kcd2vrOriginalInitiator28 & $this.id == $__player'
COPY = '$kcd2vrUseTwins28 = $dialogParameters.useTwinsAsInitiator\n$kcd2vrMovePlayer28 = $dialogParameters.movePlayer'
OVERRIDE = '$kcd2vrUseTwins28 = false\n$kcd2vrMovePlayer28 = false\n$disableRotations = true'

def normalized(node):
    return (node.tag, tuple(sorted(node.attrib.items())),
            (node.text or '').strip(), tuple(normalized(child) for child in node))

def build(data: bytes) -> bytes:
    if hashlib.sha256(data).hexdigest() != RETAIL_SHA:
        raise ValueError('shared dialogue source is not the exact retail input')
    root = ET.fromstring(data)
    tree = root.find("BehaviorTree[@name='speech_dialogInitiator_inner']")
    assert tree is not None
    sequence = tree.find('Root/Behavior/FuseBox/Child/Sequence')
    variables = tree.find('Variables')
    assert sequence is not None and variables is not None
    requests = sequence.findall('RequestDialog')
    assert len(requests) == 1
    request = requests[0]
    assert request.get('UseTwins') == '$dialogParameters.useTwinsAsInitiator'
    assert request.get('AllowPositioning') == '$dialogParameters.movePlayer'
    assert request.get('DisableRotations') == '$disableRotations'
    for name in VARIABLES:
        assert tree.find(f".//Variable[@name='{name}']") is None
        ET.SubElement(variables, 'Variable', name=name, type='_bool', values='false',
                      isPersistent='0', form='single')
    at = list(sequence).index(request)
    sequence.insert(at, ET.Element('Expression', expressions=COPY))
    sequence.insert(at+1, ET.Element('ExecuteLua', code=LUA))
    guard = ET.Element('IfCondition', failOnCondition='false', condition=CONDITION)
    ET.SubElement(guard, 'Expression', expressions=OVERRIDE)
    sequence.insert(at+2, guard)
    request.set('UseTwins', '$kcd2vrUseTwins28')
    request.set('AllowPositioning', '$kcd2vrMovePlayer28')
    # Keep native editor metadata in step without changing its runtime arguments.
    editor_variables = tree.find('EditorData/Variables')
    editor_sequence = tree.find('EditorData/Root/Behavior/FuseBox/Child/Sequence')
    assert editor_variables is not None and editor_sequence is not None
    for name in VARIABLES:
        ET.SubElement(editor_variables, 'Variable', name=name, comment='Scoped local-player original actor request.')
    editor_request = editor_sequence.find('RequestDialog')
    assert editor_request is not None
    at = list(editor_sequence).index(editor_request)
    editor_sequence.insert(at, ET.Element('Expression'))
    editor_sequence.insert(at+1, ET.Element('ExecuteLua'))
    editor_guard = ET.Element('IfCondition')
    ET.SubElement(editor_guard, 'Expression')
    editor_sequence.insert(at+2, editor_guard)
    return ET.tostring(root, encoding='us-ascii', xml_declaration=True)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('retail_xml', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    result = build(args.retail_xml.read_bytes())
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(result)
    print(f'Built proposal only: {args.output}; SHA256={hashlib.sha256(result).hexdigest()}; native BT compile NOT_RUN')

if __name__ == '__main__': main()
