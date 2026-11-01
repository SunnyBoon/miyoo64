"""Synthetic native-generator tests; never build or run any game code."""
from contextlib import redirect_stderr, redirect_stdout
import hashlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import gen_build

# Frozen SHA-256 baselines from the pre-M02 generator, with the temporary root
# normalized to <ROOT>. All inputs used to obtain these hashes are the fixture below.
US_BASELINES = {'handheld': {'assets_noload.ld': 'c17a42ef4d593f93620ed9f14444e20e6f0e99fc0f86d06a0edd99b393d0952d',
              'assets_progbits.ld': '39b04ca269d27ce8ecbb059b79ea32159d08d35ab0b1146249686b77699f78e8',
              'dontsave.ld': '99defe703ca746070439f5fba3c80fdb92301cdc3aff195b650f3fade2f9abdf',
              'handheld.ninja': 'fdfde5cd03ad43c0ff6382dfc9c1760487186eb009cd106f9b463507165f4bc2',
              'handheld/build_stamp.c': '995877cb1c40de899bc2a20b8f56e3dfa972610d6ba9d4910721acf91fe4a57d',
              'handheld_game.mk': '9e78a68e964a97e782a3272767022b33208849f9edc72f03e369a7965407e4cd',
              'port_effects.c': '0ec6e74eef418f31c5f9254ea4a0c61600a3df0edadd021525410ea96582f1b7',
              'port_segtab.c': '966e4512b40b0b4d05f1b9fc0c49d6682fdb0012dab0fa4eb48f0e06f9b777a9',
              'rom_syms.ld': 'cbd4c658a5ac2b18fe92c0e75bdf2196ad8e5da8ce98fb4004fa780746bb5c43',
              'segments.json': '4030891c07f6bce2d883f0515a6b53865f0ed2f98c24b0bb2c88cf7e63e5d7ba'},
 'miyoo': {'assets_noload.ld': 'c17a42ef4d593f93620ed9f14444e20e6f0e99fc0f86d06a0edd99b393d0952d',
           'assets_progbits.ld': '39b04ca269d27ce8ecbb059b79ea32159d08d35ab0b1146249686b77699f78e8',
           'dontsave.ld': '99defe703ca746070439f5fba3c80fdb92301cdc3aff195b650f3fade2f9abdf',
           'miyoo.ninja': '42add5603473c07dd0f111ce60925ea51b3b6c565f750b88c8a26bbad72f7b8c',
           'miyoo/build_stamp.c': '995877cb1c40de899bc2a20b8f56e3dfa972610d6ba9d4910721acf91fe4a57d',
           'port_effects.c': '0ec6e74eef418f31c5f9254ea4a0c61600a3df0edadd021525410ea96582f1b7',
           'port_segtab.c': '966e4512b40b0b4d05f1b9fc0c49d6682fdb0012dab0fa4eb48f0e06f9b777a9',
           'rom_syms.ld': 'cbd4c658a5ac2b18fe92c0e75bdf2196ad8e5da8ce98fb4004fa780746bb5c43',
           'segments.json': '4030891c07f6bce2d883f0515a6b53865f0ed2f98c24b0bb2c88cf7e63e5d7ba'},
 'pc': {'assets_noload.ld': 'c17a42ef4d593f93620ed9f14444e20e6f0e99fc0f86d06a0edd99b393d0952d',
        'assets_progbits.ld': '39b04ca269d27ce8ecbb059b79ea32159d08d35ab0b1146249686b77699f78e8',
        'dontsave.ld': '99defe703ca746070439f5fba3c80fdb92301cdc3aff195b650f3fade2f9abdf',
        'pc.ninja': 'e59339c6f25e33a143f674fa51a5c0f0163fc64751d44bbf2e533e7d765c167a',
        'pc/build_stamp.c': '995877cb1c40de899bc2a20b8f56e3dfa972610d6ba9d4910721acf91fe4a57d',
        'pc_game.mk': '69576fbb3a238e9d22d0276b978cc357560a50a2a95d1c7938dc6d8b4243b181',
        'port_effects.c': '0ec6e74eef418f31c5f9254ea4a0c61600a3df0edadd021525410ea96582f1b7',
        'port_segtab.c': '966e4512b40b0b4d05f1b9fc0c49d6682fdb0012dab0fa4eb48f0e06f9b777a9',
        'rom_syms.ld': 'cbd4c658a5ac2b18fe92c0e75bdf2196ad8e5da8ce98fb4004fa780746bb5c43',
        'segments.json': '4030891c07f6bce2d883f0515a6b53865f0ed2f98c24b0bb2c88cf7e63e5d7ba'}}


class BuildGeneratorTests(unittest.TestCase):
    def setUp(self):
        self.root = Path(self.enterContext(tempfile.TemporaryDirectory(prefix='pal_build_test_')))
        self.script = self.root / 'tools/port/gen_build.py'
        self.script.parent.mkdir(parents=True)
        shutil.copyfile(Path(gen_build.__file__), self.script)
        self.decomp = self.root / 'refs/papermario'
        self.build = self.root / 'build/game'
        self.make_fixture()

    def write(self, name, text):
        target = self.root / name
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(text, encoding='utf-8')
        return target

    def make_fixture(self):
        for filename in ('game/port/port_host.c', 'game/port/port_test.c',
                         'bench/kit/gfx_pc.c', 'bench/kit/gfx_cc.c', 'bench/kit/gfx_soft.c'):
            self.write(filename, '/* synthetic source */\n')
        for filename in ('savestate', 'miyoo_panel', 'miyoo_menu', 'miyoo_remap',
                         'miyoo_worker', 'miyoo_clock', 'miyoo_sleepsave'):
            self.write(f'kit/platform/{filename}.c', '/* synthetic source */\n')
        self.write('refs/papermario/src/effect_shims.yaml', '- shim_test_fn\n- shim_NULL\n')
        ninja = []
        for region in ('us', 'pal'):
            sources = [('main', 'src/main.c', 'cc'), ('font', f'assets/{region}/font.c', 'cc'),
                       ('map', 'src/world/map.c', 'cc'), ('boot', 'src/boot/entry.s', 'as'),
                       ('messages', f'assets/{region}/msg.bin', 'bin')]
            ld = []
            for segment, source, rule in sources:
                obj = f'ver/{region}/build/{source}.o'
                address = {'main': '0x80025C00', 'font': '0x00000000', 'map': '0x80240000',
                           'boot': '0xA4000040', 'messages': '0x00000000'}[segment]
                ld.extend([f'    .{segment} {address} :', f'        {obj}(.text);'])
                if segment == 'main':
                    ld.extend(['    .main_bss (NOLOAD) :', f'        {obj}(.bss);'])
                ninja.extend([f'build {obj}: {rule} {source}', f'  cppflags = -DVERSION_{region.upper()}',
                              '  cflags = -synthetic'])
                self.write('refs/papermario/' + source, '/* synthetic */\n')
            for header in ('ld_addrs.h', 'message_ids.h', 'synthetic.inc.h'):
                self.write(f'refs/papermario/ver/{region}/build/include/{header}', '/* synthetic header */\n')
            ninja.extend([f'build generated_code_{region}: phony ver/{region}/build/include/synthetic.inc.h',
                          f'build inc_img_bins_{region}: phony'])
            self.write(f'refs/papermario/ver/{region}/papermario.ld', '\n'.join(ld) + '\n')
            self.write(f'refs/papermario/ver/{region}/undefined_syms.txt',
                       f'{region}_only_symbol = 0x1234;\ngBackgroundImage = 0x80200000;\n')
            self.write(f'refs/papermario/assets/{region}/effects/effect_macros.h',
                       '#define EFFECT_DEF_TEST(func_name) void func_name(float)\n')
            index = '01' if region == 'us' else '02'
            self.write(f'refs/papermario/assets/{region}/effects/effect_defs.h',
                       f'    EFFECT_TEST = 0x{index},\nEFFECT_DEF_TEST(fx_test);\n')
            names = ['main', 'font', 'map', 'boot', 'messages']
            if region == 'pal':
                names += ['msg_pal_en', 'msg_pal_de', 'msg_pal_fr', 'msg_pal_es']
            ranges = [{'name': name, 'type': 'code', 'rom_start': i * 16, 'rom_end': (i + 1) * 16}
                      for i, name in enumerate(names)]
            ranges.append({'name': None, 'type': None, 'rom_start': len(names) * 16, 'rom_end': None})
            prefix = 'build/game' + ('/pal' if region == 'pal' else '')
            self.write(prefix + '/rom_segments.json', json.dumps(ranges, indent=0))
        self.write('refs/papermario/build.ninja', '\n'.join(ninja) + '\n')

    def cli(self, *args, extra_env=None):
        environment = dict(os.environ, PM64_SAN='', MIYOO_TOOLCHAIN='/opt/miyoomini-toolchain')
        environment.update(extra_env or {})
        return subprocess.run([sys.executable, '-B', str(self.script), *args], cwd=self.root,
                              env=environment, text=True, capture_output=True, timeout=10)

    def snapshot(self, region='us'):
        base = self.build / 'pal' if region == 'pal' else self.build
        return {str(path.relative_to(base)): path.read_bytes() for path in base.rglob('*')
                if path.is_file() and (region == 'pal' or 'pal' not in path.relative_to(base).parts)}

    def generated(self, region='us'):
        return {name: data for name, data in self.snapshot(region).items() if name != 'rom_segments.json'}

    def hashes(self):
        return {name: hashlib.sha256(data.replace(str(self.root).encode(), b'<ROOT>')).hexdigest()
                for name, data in self.generated().items()}

    def test_us_default_matches_frozen_original(self):
        result = self.cli()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(self.hashes(), US_BASELINES['pc'])

    def test_us_all_platforms_match_frozen_original(self):
        for platform in ('pc', 'handheld', 'miyoo'):
            with self.subTest(platform=platform):
                # Remove only synthetic generated output from this test's temporary tree.
                for name in self.generated():
                    (self.build / name).unlink()
                result = self.cli(platform)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(self.hashes(), US_BASELINES[platform])

    def test_explicit_us_is_identical_to_default(self):
        self.assertEqual(self.cli().returncode, 0)
        before = self.generated()
        result = self.cli('--region', 'us')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(self.generated(), before)

    def test_pal_all_platforms_inputs_defines_and_output_isolation(self):
        self.assertEqual(self.cli('handheld').returncode, 0)
        self.assertEqual(self.cli('miyoo').returncode, 0)
        us = self.snapshot()
        for platform in ('pc', 'handheld', 'miyoo'):
            result = self.cli(platform, '--region', 'pal')
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(self.snapshot(), us)
            ninja = (self.build / 'pal' / f'{platform}.ninja').read_text()
            for text in ('-DVERSION=pal -DVERSION_PAL', '/ver/pal/build/include', '/assets/pal', '/build/game/pal/'):
                self.assertIn(text, ninja)
            for text in ('VERSION_US', '/ver/us/', '/assets/us/'):
                self.assertNotIn(text, ninja)
            self.assertIn('builddir = /work/build/game/pal', ninja)
            self.assertIn('/build/game/pal/handheld/src/main.c.o' if platform == 'miyoo'
                          else f'/build/game/pal/{platform}/src/main.c.o', ninja)
            if platform != 'miyoo':
                make = (self.build / 'pal' / f'{platform}_game.mk').read_text()
                self.assertIn('/build/game/pal/port_effects.c', make)
                self.assertIn('-DVERSION_PAL', make)
                self.assertNotIn('/ver/us/', make)
        self.assertIn('load_effect(0x02)', (self.build / 'pal/port_effects.c').read_text())
        symbols = (self.build / 'pal/rom_syms.ld').read_text()
        self.assertIn('pal_only_symbol', symbols)
        self.assertNotIn('us_only_symbol', symbols)
        self.assertIn('msg_pal_de_ROM_START', symbols)
        self.assertIn('font_ROM_START = 0', symbols)

    def test_selected_region_ignores_other_regions_missing_inputs(self):
        for path in (self.decomp / 'ver/us/papermario.ld', self.decomp / 'assets/us/effects/effect_defs.h'):
            path.unlink()
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_us_ignores_missing_pal_inputs(self):
        (self.decomp / 'ver/pal/papermario.ld').unlink()
        result = self.cli()
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_missing_prerequisites_leave_existing_outputs_unchanged(self):
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)
        before = self.snapshot('pal')
        names = ['ver/pal/papermario.ld', 'build.ninja', 'ver/pal/undefined_syms.txt',
                 'ver/pal/build/include/ld_addrs.h', 'ver/pal/build/include/message_ids.h',
                 'ver/pal/build/include/synthetic.inc.h', 'assets/pal/effects/effect_macros.h',
                 'assets/pal/effects/effect_defs.h', 'assets/pal/font.c', 'src/effect_shims.yaml']
        for name in names:
            with self.subTest(name=name):
                target = self.decomp / name
                content = target.read_bytes()
                target.unlink()
                result = self.cli('--region', 'pal')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('missing', result.stderr)
                self.assertNotIn('Traceback', result.stderr)
                self.assertEqual(self.snapshot('pal'), before)
                target.write_bytes(content)

    def test_missing_pal_segment_table_does_not_fall_back_to_us(self):
        (self.build / 'pal/rom_segments.json').unlink()
        us = self.snapshot()
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('rom_segments.py --region pal', result.stderr)
        self.assertEqual(self.generated('pal'), {})
        self.assertEqual(self.snapshot(), us)

    def test_unknown_region_or_platform_has_no_outputs(self):
        for args in (('--region', 'jp'), ('unknown',)):
            with self.subTest(args=args):
                result = self.cli(*args)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('invalid choice', result.stderr)
                self.assertEqual(self.generated(), {})
                self.assertEqual(self.generated('pal'), {})

    def test_mixed_region_flags_rejected(self):
        path = self.decomp / 'build.ninja'
        path.write_text(path.read_text().replace('  cppflags = -DVERSION_PAL', '  cppflags = -DVERSION_US'))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('wrong region', result.stderr)
        self.assertEqual(self.generated('pal'), {})

    def test_missing_object_rule_rejected(self):
        path = self.decomp / 'build.ninja'
        path.write_text(path.read_text().replace('build ver/pal/build/src/main.c.o:', 'build ver/pal/build/src/other.c.o:'))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('no build.ninja object rule', result.stderr)
        self.assertEqual(self.generated('pal'), {})

    def test_empty_pal_units_rejected(self):
        (self.decomp / 'ver/pal/papermario.ld').write_text('    .main 0x80025C00 :\n')
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('no native C units', result.stderr)

    def test_missing_pal_phony_targets_rejected(self):
        path = self.decomp / 'build.ninja'
        path.write_text(path.read_text().replace('build generated_code_pal:', 'build wrong_generated_target:'))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('missing PAL prerequisite targets', result.stderr)

    def test_wrong_pal_segment_table_rejected(self):
        (self.build / 'pal/rom_segments.json').write_bytes((self.build / 'rom_segments.json').read_bytes())
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('four msg_pal', result.stderr)

    def test_invalid_segment_bounds_rejected(self):
        target = self.build / 'pal/rom_segments.json'
        table = json.loads(target.read_text())
        table[0]['rom_end'] = -1
        target.write_text(json.dumps(table))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('bounds', result.stderr)

    def test_missing_native_segment_range_rejected(self):
        target = self.build / 'pal/rom_segments.json'
        table = json.loads(target.read_text())
        table[0]['name'] = 'not_main'
        target.write_text(json.dumps(table))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('missing ROM ranges', result.stderr)

    def test_proven_follow_preserves_rom_symbols_and_isolation(self):
        for region in ('us', 'pal'):
            with self.subTest(region=region):
                result = self.cli('--region', region)
                self.assertEqual(result.returncode, 0, result.stderr)
                before = self.generated(region)
                other = self.snapshot('pal' if region == 'us' else 'us')
                path = self.decomp / f'ver/{region}/papermario.ld'
                text = path.read_text()
                obj = f'ver/{region}/build/assets/{region}/font.c.o'
                text = text.replace(f'    .font 0x00000000 :\n        {obj}(.text);',
                    f'    .font 0x00000000 : AT(font_ROM_START) SUBALIGN(8)\n'
                    f'    {{\n        {obj}(.text);\n    }}\n'
                    '    . = ALIGN(., 16);\n    font_VRAM_END = .;')
                path.write_text(text.replace('.map 0x80240000 :', '.map font_VRAM_END :'))
                result = self.cli('--region', region)
                self.assertEqual(result.returncode, 0, result.stderr)
                after = self.generated(region)
                for name in ('rom_syms.ld', 'port_segtab.c', 'pc.ninja'):
                    if name == 'rom_syms.ld':
                        self.assertEqual(after[name], before[name].replace(
                            b'PROVIDE(map_VRAM = 0x80240000);\n', b''))
                    else:
                        self.assertEqual(after[name], before[name])
                self.assertEqual(self.snapshot('pal' if region == 'us' else 'us'), other)
                snapshot = self.snapshot(region)
                path.write_text(path.read_text().replace('font_VRAM_END = .;',
                                                       'font_VRAM_END = .;\n    . += 16;'))
                result = self.cli('--region', region)
                self.assertNotEqual(result.returncode, 0)
                self.assertEqual(self.snapshot(region), snapshot)

    def test_global_follow_validation_on_all_platforms(self):
        for region in ('us', 'pal'):
            path = self.decomp / f'ver/{region}/papermario.ld'
            original = path.read_text()
            obj = f'ver/{region}/build/assets/{region}/font.c.o'
            base = f'    .font 0x00000000 :\n    {{\n        {obj}(.text);\n    }}\n'
            bss = f'    .font_bss (NOLOAD) : SUBALIGN(8)\n    {{\n        {obj}(.bss);\n    }}\n'
            valid = original.replace(f'    .font 0x00000000 :\n        {obj}(.text);',
                                     base + bss + '    . = ALIGN(., 16);\n    font_VRAM_END = .;')
            valid = valid.replace('.map 0x80240000 :', '.map font_VRAM_END :')
            self.assertNotEqual(valid, original)
            variants = {'late duplicate owner': valid + base,
                        'late BSS': valid.replace(bss, '') + bss,
                        'duplicate BSS': valid + bss}
            for platform in ('pc', 'handheld', 'miyoo'):
                with self.subTest(region=region, platform=platform):
                    path.write_text(original)
                    result = self.cli(platform, '--region', region)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    before = self.generated(region)
                    path.write_text(valid)
                    result = self.cli(platform, '--region', region)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    after = self.generated(region)
                    self.assertEqual(after['port_segtab.c'], before['port_segtab.c'])
                    self.assertEqual(after['rom_syms.ld'], before['rom_syms.ld'].replace(
                        b'PROVIDE(map_VRAM = 0x80240000);\n', b''))
                    self.assertEqual(after[platform + '.ninja'], before[platform + '.ninja'])
                    segments = json.loads(after['segments.json'])
                    self.assertEqual([x['name'] for x in segments],
                                     ['main', 'font', 'map', 'boot', 'messages'])
                    self.assertEqual(segments[2]['objs'], [f'ver/{region}/build/src/world/map.c.o'])
                    for label, invalid in variants.items():
                        for existing in (True, False):
                            with self.subTest(case=label, existing=existing):
                                if existing:
                                    path.write_text(valid)
                                    self.assertEqual(self.cli(platform, '--region', region).returncode, 0)
                                else:
                                    # Only synthetic output files owned by this test.
                                    destination = self.build / 'pal' if region == 'pal' else self.build
                                    for filename in self.generated(region):
                                        (destination / filename).unlink()
                                snapshot = self.snapshot(region)
                                other = self.snapshot('us' if region == 'pal' else 'pal')
                                path.write_text(invalid)
                                result = self.cli(platform, '--region', region)
                                self.assertNotEqual(result.returncode, 0)
                                self.assertIn('unproven predecessor VRAM end', result.stderr)
                                self.assertEqual(self.snapshot(region), snapshot)
                                self.assertEqual(self.snapshot('us' if region == 'pal' else 'pal'), other)
                                if not existing:
                                    self.assertEqual(self.generated(region), {})
            path.write_text(original)

    def test_unsupported_symbolic_linker_address_rejected(self):
        path = self.decomp / 'ver/pal/papermario.ld'
        path.write_text(path.read_text().replace('.main 0x80025C00 :', '.main UNRESOLVED_VRAM :'))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('unsupported linker section', result.stderr)
        self.assertEqual(self.generated('pal'), {})

    def test_ninja_continuations(self):
        path = self.decomp / 'build.ninja'
        path.write_text(path.read_text().replace(': cc src/main.c', ': cc $\n  src/main.c'))
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_effect_render_failure_does_not_publish_partial_build(self):
        self.assertEqual(self.cli('--region', 'pal').returncode, 0)
        before = self.snapshot('pal')
        (self.decomp / 'assets/pal/effects/effect_defs.h').write_text(
            '    EFFECT_MISSING = 0x03,\nEFFECT_DEF_MISSING(fx_bad);\n')
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('missing effect macro', result.stderr)
        self.assertEqual(self.snapshot('pal'), before)
        self.assertEqual(list((self.build / 'pal').glob('.gen-build-*')), [])

    def test_existing_build_stamp_is_preserved(self):
        path = self.write('build/game/pal/pc/build_stamp.c', '/* keep this stamp */\n')
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(path.read_text(), '/* keep this stamp */\n')

    def test_pal_directory_alias_to_us_is_rejected(self):
        pal_table = (self.build / 'pal/rom_segments.json').read_bytes()
        (self.build / 'pal/rom_segments.json').unlink()
        (self.build / 'pal').rmdir()
        # Keep inputs valid so this specifically exercises the path guard.
        (self.build / 'rom_segments.json').write_bytes(pal_table)
        before = self.snapshot()
        (self.build / 'pal').symlink_to(self.build, target_is_directory=True)
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('PAL output directory must not be a symlink', result.stderr)
        self.assertEqual(self.snapshot(), before)

    def test_pal_file_alias_to_us_is_rejected(self):
        target = self.write('build/game/port_effects.c', 'keep US\n')
        (self.build / 'pal/port_effects.c').symlink_to(target)
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('PAL output escapes', result.stderr)
        self.assertEqual(target.read_text(), 'keep US\n')

    def test_pal_object_alias_to_us_is_rejected(self):
        target = self.write('build/game/pc/port/port_host.o', 'synthetic existing object')
        link = self.build / 'pal/pc/port/port_host.o'
        link.parent.mkdir(parents=True)
        link.symlink_to(target)
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('PAL output escapes', result.stderr)
        self.assertEqual(target.read_text(), 'synthetic existing object')

    def test_segment_generator_integration_with_synthetic_splat(self):
        segment_script = Path(__file__).with_name('rom_segments.py')
        shutil.copyfile(segment_script, self.script.with_name('rom_segments.py'))
        for region in ('us', 'pal'):
            table_path = self.build / ('pal/rom_segments.json' if region == 'pal' else 'rom_segments.json')
            table = json.loads(table_path.read_text())
            cfg = {'segments': [{'start': r['rom_start'], 'name': r['name'], 'type': r['type']} for r in table]}
            self.write(f'refs/papermario/ver/{region}/splat.yaml', json.dumps(cfg))
            table_path.unlink()
            result = subprocess.run([sys.executable, '-B', str(self.script.with_name('rom_segments.py')),
                                     '--region', region], cwd=self.root, capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = self.cli('--region', region)
            self.assertEqual(result.returncode, 0, result.stderr)

    def test_image_include_prerequisites_are_checked_without_reading_binary(self):
        ninja = self.decomp / 'build.ninja'
        filename = 'ver/pal/build/img/synthetic.png.bin'
        ninja.write_text(ninja.read_text().replace('build inc_img_bins_pal: phony',
                                                  'build inc_img_bins_pal: phony ' + filename))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('missing ' + filename, result.stderr)
        path = self.decomp / filename
        path.parent.mkdir(parents=True)
        path.write_bytes(b'\xff\xfe synthetic data, no image or ROM')
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_malformed_segment_table_does_not_publish(self):
        target = self.build / 'pal/rom_segments.json'
        for text in ('broken JSON', '{}', '[]', '[null]', '[{"name": true}]'):
            with self.subTest(text=text):
                target.write_text(text)
                result = self.cli('--region', 'pal')
                self.assertNotEqual(result.returncode, 0)
                self.assertNotIn('Traceback', result.stderr)
                self.assertEqual(self.generated('pal'), {})

    def test_foreign_linker_object_is_not_silently_skipped(self):
        path = self.decomp / 'ver/pal/papermario.ld'
        path.write_text(path.read_text().replace('ver/pal/build/src/main.c.o', 'ver/us/build/src/main.c.o'))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('wrong-region linker object', result.stderr)
        self.assertEqual(self.generated('pal'), {})

    def test_empty_effect_headers_do_not_publish(self):
        (self.decomp / 'assets/pal/effects/effect_defs.h').write_text('/* empty */')
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('no fx_ entry points', result.stderr)
        self.assertEqual(self.generated('pal'), {})

    def test_missing_native_kit_source_does_not_publish(self):
        (self.root / 'kit/platform/miyoo_panel.c').unlink()
        result = self.cli('miyoo', '--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('missing native port source', result.stderr)
        self.assertEqual(self.generated('pal'), {})

    def test_pal_ninja_log_alias_to_us_is_rejected(self):
        target = self.write('build/game/.ninja_log', 'keep US log')
        (self.build / 'pal/.ninja_log').symlink_to(target)
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('PAL output escapes', result.stderr)
        self.assertEqual(target.read_text(), 'keep US log')

    def test_zero_length_rom_range_is_preserved(self):
        target = self.build / 'pal/rom_segments.json'
        table = json.loads(target.read_text())
        table.insert(0, {'name': 'heaps', 'type': 'code', 'rom_start': 0, 'rom_end': 0})
        target.write_text(json.dumps(table))
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('heaps_ROM_START = 0x00000000', (self.build / 'pal/rom_syms.ld').read_text())


    def test_us_generated_file_symlink_and_permissions_are_preserved(self):
        target = self.write('elsewhere/segments.json', 'old synthetic metadata')
        target.chmod(0o600)
        (self.build / 'segments.json').symlink_to(target)
        result = self.cli()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertTrue((self.build / 'segments.json').is_symlink())
        self.assertEqual(target.stat().st_mode & 0o777, 0o600)
        self.assertTrue(json.loads(target.read_text()))

    def test_wrong_region_source_path_rejected(self):
        path = self.decomp / 'build.ninja'
        path.write_text(path.read_text().replace(': cc assets/pal/font.c', ': cc assets/us/font.c'))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('wrong region', result.stderr)

    def test_inconsistent_segment_end_rejected(self):
        path = self.build / 'pal/rom_segments.json'
        table = json.loads(path.read_text())
        table[0]['rom_end'] = 15
        path.write_text(json.dumps(table))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('inconsistent ROM segment end', result.stderr)

    def test_common_bss_object_is_not_lost_or_duplicated(self):
        path = self.decomp / 'ver/pal/papermario.ld'
        path.write_text(path.read_text().replace('(.bss);', '(COMMON);'))
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)
        segments = json.loads((self.build / 'pal/segments.json').read_text())
        main = next(segment for segment in segments if segment['name'] == 'main')
        self.assertEqual(main['objs'], ['ver/pal/build/src/main.c.o'])

    def test_asset_wrapper_helper_selects_pal_and_uses_separate_output(self):
        self.write('refs/papermario/assets/pal/synthetic.inc.c', '/* synthetic include */')
        with patch.multiple(gen_build, ROOT=str(self.root), DECOMP=str(self.decomp)):
            self.assertEqual(gen_build.write_asset_wrappers('pal'), 1)
        wrapper = self.build / 'pal/assetwrap/synthetic.inc.c'
        self.assertIn('#include_next "synthetic.inc.c"', wrapper.read_text())
        self.assertFalse((self.build / 'assetwrap').exists())

    def test_existing_sanitizer_and_toolchain_options_remain_supported(self):
        result = self.cli(extra_env={'PM64_SAN': '-fsanitize=address'})
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('-fsanitize=address', (self.build / 'pc_game.mk').read_text())
        result = self.cli('miyoo', '--region', 'pal', extra_env={'MIYOO_TOOLCHAIN': '/synthetic-toolchain'})
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('/synthetic-toolchain/usr/bin/arm-linux-gnueabihf-gcc',
                      (self.build / 'pal/miyoo.ninja').read_text())


    def prepare_published_outputs(self):
        self.assertEqual(self.cli().returncode, 0)
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)

    def assert_collision_preserves_outputs(self, expected):
        before_pal, before_us = self.snapshot('pal'), self.snapshot()
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(expected, result.stderr)
        self.assertNotIn('Traceback', result.stderr)
        self.assertEqual(self.snapshot('pal'), before_pal)
        self.assertEqual(self.snapshot(), before_us)
        self.assertEqual(list((self.build / 'pal').glob('.gen-build-*')), [])

    def test_output_symlink_to_segment_input_is_rejected_before_publication(self):
        self.prepare_published_outputs()
        output = self.build / 'pal/port_effects.c'
        output.unlink()
        output.symlink_to('rom_segments.json')
        self.assert_collision_preserves_outputs('output/input collision')
        self.assertEqual(output.readlink(), Path('rom_segments.json'))

    def test_output_symlink_to_other_generated_file_is_rejected(self):
        self.prepare_published_outputs()
        output = self.build / 'pal/port_effects.c'
        output.unlink()
        output.symlink_to('rom_syms.ld')
        self.assert_collision_preserves_outputs('output/output collision')
        self.assertEqual(output.readlink(), Path('rom_syms.ld'))

    def test_distinct_relative_symlink_chains_to_same_output_are_rejected(self):
        self.prepare_published_outputs()
        self.write('build/game/pal/shared-target', 'keep synthetic shared target')
        links = self.build / 'pal/links'
        links.mkdir()
        (links / 'first').symlink_to('../shared-target')
        (links / 'second').symlink_to('first')
        for name, destination in (('port_effects.c', 'links/first'), ('rom_syms.ld', 'links/second')):
            path = self.build / 'pal' / name
            path.unlink()
            path.symlink_to(destination)
        self.assert_collision_preserves_outputs('output/output collision')
        self.assertTrue((links / 'first').is_symlink())
        self.assertTrue((links / 'second').is_symlink())

    def test_relative_symlink_chain_to_input_is_rejected(self):
        self.prepare_published_outputs()
        links = self.build / 'pal/links'
        links.mkdir()
        (links / 'input').symlink_to('../rom_segments.json')
        output = self.build / 'pal/port_effects.c'
        output.unlink()
        output.symlink_to('links/input')
        self.assert_collision_preserves_outputs('output/input collision')

    def test_output_hardlink_to_input_is_rejected(self):
        self.prepare_published_outputs()
        output = self.build / 'pal/port_effects.c'
        output.unlink()
        output.hardlink_to(self.build / 'pal/rom_segments.json')
        self.assert_collision_preserves_outputs('output/input hardlink collision')

    def test_output_hardlink_to_other_output_is_rejected(self):
        self.prepare_published_outputs()
        output = self.build / 'pal/port_effects.c'
        output.unlink()
        output.hardlink_to(self.build / 'pal/rom_syms.ld')
        self.assert_collision_preserves_outputs('output/output hardlink collision')

    def test_future_pal_object_hardlink_to_us_object_is_rejected(self):
        self.prepare_published_outputs()
        us_object = self.write('build/game/pc/src/main.c.o', 'keep synthetic US object')
        output = self.build / 'pal/pc/src/main.c.o'
        output.parent.mkdir(parents=True)
        output.hardlink_to(us_object)
        self.assert_collision_preserves_outputs('output/input hardlink collision')

    def test_future_pal_log_hardlink_to_us_log_is_rejected(self):
        self.prepare_published_outputs()
        us_log = self.write('.ninja_log', 'keep synthetic US log')
        (self.build / 'pal/.ninja_log').hardlink_to(us_log)
        self.assert_collision_preserves_outputs('output/input hardlink collision')
        self.assertEqual(us_log.read_text(), 'keep synthetic US log')

    def test_unrelated_symlink_target_inside_pal_remains_supported(self):
        self.prepare_published_outputs()
        output = self.build / 'pal/port_effects.c'
        output.unlink()
        output.symlink_to('private-effects.c')
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertTrue(output.is_symlink())
        self.assertIn('load_effect(0x02)', output.read_text())

    def place_required_segment_last_without_end(self, name):
        path = self.build / 'pal/rom_segments.json'
        table = json.loads(path.read_text())
        wanted = next(entry for entry in table if entry['name'] == name)
        table = [entry for entry in table if entry['name'] is not None and entry['name'] != name] + [wanted]
        for index, entry in enumerate(table):
            entry['rom_start'] = index * 16
            entry['rom_end'] = (index + 1) * 16 if index + 1 < len(table) else None
        path.write_text(json.dumps(table))

    def assert_required_range_failure(self, name):
        before_pal, before_us = self.snapshot('pal'), self.snapshot()
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('required PAL ROM range: ' + name, result.stderr)
        self.assertEqual(self.snapshot('pal'), before_pal)
        self.assertEqual(self.snapshot(), before_us)

    def test_missing_end_of_spanish_messages_is_rejected(self):
        self.prepare_published_outputs()
        self.place_required_segment_last_without_end('msg_pal_es')
        self.assert_required_range_failure('msg_pal_es')

    def test_missing_end_of_german_messages_is_rejected(self):
        self.prepare_published_outputs()
        self.place_required_segment_last_without_end('msg_pal_de')
        self.assert_required_range_failure('msg_pal_de')

    def test_missing_end_of_native_segment_is_rejected(self):
        self.prepare_published_outputs()
        self.place_required_segment_last_without_end('main')
        self.assert_required_range_failure('main')

    def test_empty_required_language_and_native_ranges_are_rejected(self):
        self.prepare_published_outputs()
        path = self.build / 'pal/rom_segments.json'
        original = path.read_text()
        for name in ('msg_pal_de', 'msg_pal_es', 'main'):
            with self.subTest(segment=name):
                table = json.loads(original)
                index = next(i for i, entry in enumerate(table) if entry['name'] == name)
                table[index]['rom_end'] = table[index]['rom_start']
                table[index + 1]['rom_start'] = table[index]['rom_start']
                path.write_text(json.dumps(table))
                self.assert_required_range_failure(name)

    def test_required_range_outside_32bit_rom_space_is_rejected(self):
        self.prepare_published_outputs()
        path = self.build / 'pal/rom_segments.json'
        table = json.loads(path.read_text())
        table[-2]['rom_end'] = 0x100000000
        table[-1]['rom_start'] = 0x100000000
        path.write_text(json.dumps(table))
        self.assert_required_range_failure('msg_pal_es')

    def test_pinned_native_bss_segment_can_have_zero_rom_length(self):
        ld = self.decomp / 'ver/pal/papermario.ld'
        ninja = self.decomp / 'build.ninja'
        path = self.build / 'pal/rom_segments.json'
        original_ld, original_ninja, original_table = ld.read_text(), ninja.read_text(), path.read_text()
        for name in ('heaps', 'heaps2', 'heaps3', 'heaps4', 'texture_memory', 'model_clear_render_tasks'):
            with self.subTest(segment=name):
                self.write(f'refs/papermario/src/{name}.c', '/* synthetic BSS-only source at pinned path */')
                ld.write_text(f'    .{name} 0x80195000 :\n        ver/pal/build/src/{name}.c.o(.bss);\n' + original_ld)
                ninja.write_text(original_ninja + f'\nbuild ver/pal/build/src/{name}.c.o: cc src/{name}.c\n  cppflags = -DVERSION_PAL\n')
                table = json.loads(original_table)
                table.insert(0, {'name': name, 'type': 'code', 'rom_start': 0, 'rom_end': 0})
                path.write_text(json.dumps(table))
                result = self.cli('--region', 'pal')
                self.assertEqual(result.returncode, 0, result.stderr)
                symbols = (self.build / 'pal/rom_syms.ld').read_text()
                self.assertIn(name + '_ROM_START = 0x00000000', symbols)
                self.assertIn(name + '_ROM_END = 0x00000000', symbols)
                self.assertNotIn('"' + name + '"', (self.build / 'pal/port_segtab.c').read_text())

    def test_bss_segment_name_alone_does_not_allow_empty_native_code(self):
        ld = self.decomp / 'ver/pal/papermario.ld'
        ld.write_text(ld.read_text().replace('.main ', '.heaps ').replace('.main_bss ', '.heaps_bss '))
        path = self.build / 'pal/rom_segments.json'
        table = json.loads(path.read_text())
        table[0].update(name='heaps', rom_end=0)
        table[1]['rom_start'] = 0
        path.write_text(json.dumps(table))
        self.assert_required_range_failure('heaps')

    def test_bss_type_tag_does_not_allow_empty_native_code(self):
        path = self.build / 'pal/rom_segments.json'
        table = json.loads(path.read_text())
        table[0].update(type='bss', rom_end=0)
        table[1]['rom_start'] = 0
        path.write_text(json.dumps(table))
        self.assert_required_range_failure('main')

    def test_duplicate_required_segment_name_is_rejected(self):
        path = self.build / 'pal/rom_segments.json'
        table = json.loads(path.read_text())
        table[-2]['name'] = 'msg_pal_de'
        # Keep all required names present while adding a second German segment.
        table[-1] = {'name': 'msg_pal_es', 'type': 'pm_msg', 'rom_start': table[-2]['rom_end'],
                     'rom_end': table[-2]['rom_end'] + 16}
        table.append({'name': None, 'type': None, 'rom_start': table[-1]['rom_end'], 'rom_end': None})
        path.write_text(json.dumps(table))
        result = self.cli('--region', 'pal')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('ambiguous PAL ROM segment name', result.stderr)

    def test_complete_pal_ranges_generate_all_required_start_end_symbols(self):
        result = self.cli('--region', 'pal')
        self.assertEqual(result.returncode, 0, result.stderr)
        symbols = (self.build / 'pal/rom_syms.ld').read_text()
        for name in ('main', 'font', 'map', 'msg_pal_en', 'msg_pal_de', 'msg_pal_fr', 'msg_pal_es'):
            self.assertIn('PROVIDE(' + name + '_ROM_START =', symbols)
            self.assertIn('PROVIDE(' + name + '_ROM_END =', symbols)

    def test_failure_on_second_replace_reports_error_and_preserves_us(self):
        self.prepare_published_outputs()
        us = self.snapshot()
        first = self.build / 'pal/assets_noload.ld'
        second = self.build / 'pal/assets_progbits.ld'
        first_inode, second_inode = first.stat().st_ino, second.stat().st_ino
        original_replace = os.replace
        calls = []

        def fail_second(source, target):
            calls.append(target)
            if len(calls) == 2:
                raise OSError('synthetic second replace failure')
            return original_replace(source, target)

        stdout, stderr = io.StringIO(), io.StringIO()
        with patch.multiple(gen_build, ROOT=str(self.root), DECOMP=str(self.decomp)), \
                patch.object(gen_build.os, 'replace', side_effect=fail_second), \
                redirect_stdout(stdout), redirect_stderr(stderr):
            with self.assertRaises(SystemExit) as stopped:
                gen_build.main(['--region', 'pal'])
        self.assertEqual(stopped.exception.code, 1)
        self.assertIn('synthetic second replace failure', stderr.getvalue())
        self.assertNotIn('gen_build.py: wrote', stdout.getvalue())
        self.assertEqual(len(calls), 2)
        self.assertNotEqual(first.stat().st_ino, first_inode)
        self.assertEqual(second.stat().st_ino, second_inode)
        self.assertEqual(self.snapshot(), us)
        self.assertEqual(list((self.build / 'pal').glob('.gen-build-*')), [])

    def test_publication_chmod_failure_reports_error_without_replacing_outputs(self):
        self.prepare_published_outputs()
        pal, us = self.snapshot('pal'), self.snapshot()
        stdout, stderr = io.StringIO(), io.StringIO()
        with patch.multiple(gen_build, ROOT=str(self.root), DECOMP=str(self.decomp)), \
                patch.object(gen_build.os, 'chmod', side_effect=PermissionError('synthetic permission failure')), \
                redirect_stdout(stdout), redirect_stderr(stderr):
            with self.assertRaises(SystemExit) as stopped:
                gen_build.main(['--region', 'pal'])
        self.assertEqual(stopped.exception.code, 1)
        self.assertIn('synthetic permission failure', stderr.getvalue())
        self.assertNotIn('gen_build.py: wrote', stdout.getvalue())
        self.assertEqual(self.snapshot('pal'), pal)
        self.assertEqual(self.snapshot(), us)
        self.assertEqual(list((self.build / 'pal').glob('.gen-build-*')), [])



class LinkerFollowParserTests(unittest.TestCase):
    """Author-created linker text only: no real split, ROM or asset inputs."""
    def setUp(self):
        self.root = Path(self.enterContext(tempfile.TemporaryDirectory()))

    def block(self, name, address, region, bss=False):
        obj = f'ver/{region}/build/src/{name}.c.o'
        text = (f'    .{name} {address} : AT({name}_ROM_START) SUBALIGN(8)\n'
                f'    {{\n        {obj}(.text);\n        . = ALIGN(., 16);\n    }}\n')
        if bss:
            text += (f'    {name}_bss_VRAM = ADDR(.{name}_bss);\n'
                     f'    .{name}_bss (NOLOAD) : SUBALIGN(8)\n'
                     f'    {{\n        {obj}(.bss);\n        . = ALIGN(., 16);\n    }}\n')
        return (text + f'    __romPos += SIZEOF(.{name});\n'
                '    __romPos = ALIGN(__romPos, 16);\n    . = ALIGN(., 16);\n'
                f'    {name}_ROM_END = __romPos;\n    {name}_VRAM_END = .;\n')

    def fixture(self, region, bss=False):
        return (self.block('alpha', '0x80200000', region, bss) +
                '    beta_ROM_START = __romPos;\n    beta_VRAM = ADDR(.beta);\n' +
                self.block('beta', 'alpha_VRAM_END', region, bss))

    def parse(self, text, region):
        path = self.root / 'synthetic.ld'
        path.write_text(text)
        return gen_build.parse_ld(path, region)

    def reject(self, transform):
        for region in ('us', 'pal'):
            with self.subTest(region=region):
                with self.assertRaisesRegex(ValueError, 'unsupported linker section address'):
                    self.parse(transform(self.fixture(region, True), region), region)

    def test_valid_follow(self):
        for region in ('us', 'pal'):
            for bss in (False, True):
                with self.subTest(region=region, bss=bss):
                    segs = self.parse(self.fixture(region, bss), region)
                    self.assertEqual([x['name'] for x in segs], ['alpha', 'beta'])
                    self.assertEqual(segs[0]['vram'], 0x80200000)
                    self.assertIsNone(segs[1]['vram'])
                    self.assertEqual(segs[1]['vram_follow'],
                                     {'segment': 'alpha', 'symbol': 'alpha_VRAM_END'})
                    self.assertEqual(segs[1]['objs'], [f'ver/{region}/build/src/beta.c.o'])
                    self.assertEqual(gen_build.seg_class(segs[1], region), 'follow')

    def test_follow_chain(self):
        for region in ('us', 'pal'):
            segs = self.parse(self.fixture(region, True) +
                              self.block('gamma', 'beta_VRAM_END', region, True), region)
            self.assertEqual([s['name'] for s in segs], ['alpha', 'beta', 'gamma'])
            self.assertEqual(segs[2]['vram_follow']['segment'], 'beta')
            self.assertEqual(segs[2]['objs'], [f'ver/{region}/build/src/gamma.c.o'])

    def test_unknown_symbol(self):
        self.reject(lambda t, r: t.replace('.beta alpha_VRAM_END :', '.beta UNKNOWN :'))

    def test_missing_definition(self):
        self.reject(lambda t, r: t.replace('    alpha_VRAM_END = .;\n', ''))

    def test_duplicate_definition(self):
        self.reject(lambda t, r: t + '    alpha_VRAM_END = .;\n')

    def test_duplicate_predecessor_section(self):
        self.reject(lambda t, r: '    .alpha 0x80100000 :\n    {\n    }\n' + t)

    def test_duplicate_predecessor_after_follower(self):
        self.reject(lambda t, r: t + '    .alpha 0x80300000 :\n    {\n    }\n')

    def test_bss_after_follower(self):
        self.reject(lambda t, r: self.fixture(r) +
                    '    .alpha_bss (NOLOAD) : SUBALIGN(8)\n    {\n    }\n')

    def test_duplicate_predecessor_bss(self):
        self.reject(lambda t, r: t +
                    '    .alpha_bss (NOLOAD) : SUBALIGN(8)\n    {\n    }\n')

    def test_duplicate_follower_after_relation(self):
        self.reject(lambda t, r: t + '    .beta 0x80300000 :\n    {\n    }\n')

    def test_predecessor_bss_with_conflicting_address(self):
        self.reject(lambda t, r: t.replace('.alpha_bss (NOLOAD)', '.alpha_bss 0x80400000'))

    def test_forward_end_definition(self):
        self.reject(lambda t, r: t.replace('    alpha_VRAM_END = .;\n', '') +
                    '    alpha_VRAM_END = .;\n')

    def test_symbolic_first_segment(self):
        self.reject(lambda t, r: t.replace('.alpha 0x80200000 :',
                                           '.alpha alpha_VRAM_END :'))

    def test_compound_redefinition(self):
        self.reject(lambda t, r: t + '    alpha_VRAM_END += 16;\n')

    def test_other_compound_redefinitions(self):
        for operator in ('%=', '<<=', '>>=', '^=', '-='):
            with self.subTest(operator=operator):
                self.reject(lambda t, r: t + f'    alpha_VRAM_END {operator} 16;\n')

    def test_multiline_redefinition(self):
        self.reject(lambda t, r: t + '    alpha_VRAM_END\n        = .;\n')

    def test_non_dot_definition(self):
        self.reject(lambda t, r: t.replace('alpha_VRAM_END = .;', 'alpha_VRAM_END = 0x80200080;'))

    def test_cyclic_alias(self):
        self.reject(lambda t, r: t.replace('alpha_VRAM_END = .;',
                    'alpha_VRAM_END = beta_VRAM_END;'))

    def test_non_immediate_predecessor(self):
        self.reject(lambda t, r: t.replace('    beta_ROM_START = __romPos;',
                    self.block('middle', '0x80300000', r) + '    beta_ROM_START = __romPos;'))

    def test_position_change_after_end(self):
        self.reject(lambda t, r: t.replace('alpha_VRAM_END = .;',
                                           'alpha_VRAM_END = .;\n    . += 16;'))

    def test_alignment_after_end(self):
        self.reject(lambda t, r: t.replace('alpha_VRAM_END = .;',
                                           'alpha_VRAM_END = .;\n    . = ALIGN(., 16);'))

    def test_end_before_bss(self):
        self.reject(lambda t, r: t.replace('    alpha_VRAM_END = .;\n', '').replace(
            '    alpha_bss_VRAM =', '    alpha_VRAM_END = .;\n    alpha_bss_VRAM ='))

    def test_end_inside_body(self):
        self.reject(lambda t, r: t.replace('    alpha_VRAM_END = .;\n', '').replace(
            f'        ver/{r}/build/src/alpha.c.o(.bss);',
            f'        ver/{r}/build/src/alpha.c.o(.bss);\n    alpha_VRAM_END = .;'))

    def test_symbolic_arithmetic(self):
        self.reject(lambda t, r: t.replace('.beta alpha_VRAM_END :',
                                           '.beta alpha_VRAM_END + 16 :'))

    def test_symbolic_function(self):
        self.reject(lambda t, r: t.replace('.beta alpha_VRAM_END :',
                                           '.beta ALIGN(alpha_VRAM_END, 16) :'))

    def test_invalid_alignment(self):
        self.reject(lambda t, r: t.replace('. = ALIGN(., 16);', '. = ALIGN(., 3);'))

    def test_missing_body_close(self):
        self.reject(lambda t, r: t.replace('    }\n', '', 1))

    def test_unrelated_noload(self):
        self.reject(lambda t, r: t.replace('.alpha_bss (NOLOAD)', '.foreign_bss (NOLOAD)'))


if __name__ == '__main__':
    unittest.main()
