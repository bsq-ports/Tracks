#!/usr/bin/env python3
"""Exercise actual base-provider and callback cleanup with strong-root stand-ins."""
from pathlib import Path
import json, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
base=(root/'src/Hooks/BaseProviderHooks.cpp').read_text()
start=base.index('void ClearTracksGameplayReferences() {')
end=base.index('\nMAKE_HOOK_MATCH(GameplayCoreInstaller_InstallBindings',start)
body=base[start:end]
callbacks=(root/'src/Hooks/BeatmapObjectCallbackController.cpp').read_text()
start=callbacks.index('void ClearTracksCallbackReferences() {')
end=callbacks.index('\nMAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start',start)
callback_body=callbacks[start:end]
prefix=r'''#include <memory>
#include <cassert>
namespace Paper {struct ConstLoggerContext { ConstLoggerContext(const char*){} template<class... T> void info(const char*, T...) {}};}
struct Node {};
std::shared_ptr<Node> tempCustomBeatmap,tempPlayerSpecificSettings,tempVariableMovementDataProvider;
struct TracksStatic { inline static std::shared_ptr<Node> bpmController; };
Node* controller=nullptr;
'''
suffix=r'''
int main() {
 for(int cycle=0;cycle<100;++cycle) {
  auto map=std::make_shared<Node>(); std::weak_ptr<Node> weak=map;
  tempCustomBeatmap=map;tempPlayerSpecificSettings=map;tempVariableMovementDataProvider=map;
  TracksStatic::bpmController=map;controller=map.get();map.reset();
  assert(!weak.expired());ClearTracksGameplayReferences();assert(weak.expired());assert(!controller);
  ClearTracksGameplayReferences();
 }
}
'''
with tempfile.TemporaryDirectory(prefix='tracks-roots-') as tmp:
 p=Path(tmp)
 def run(name,method,callback):
  (p/(name+'.cpp')).write_text(prefix+callback+method+suffix)
  subprocess.run(['c++','-std=c++20',str(p/(name+'.cpp')),'-o',str(p/name)],check=True)
  return subprocess.run([str(p/name)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode
 assert run('production',body,callback_body)==0
 assert run('negative-beatmap',body.replace('  tempCustomBeatmap = nullptr;',''),callback_body)!=0
 assert run('negative-bpm',body,callback_body.replace('  TracksStatic::bpmController = nullptr;',''))!=0
print(json.dumps({'production_cleanup_bodies':True,'strong_roots_release_cycles':100,'idempotence':True,'beatmap_and_bpm_omission_negative_controls_failed_as_expected':True,'passed':True}))
