Import("env")

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

#
# build_littlefs actions
#
def pre_build_littlefs(source, target, env):
  print("[PreAction]: before_build_littlefs")
  env.Execute("gzip data/setup/index.htm data/setup/main.js data/main/index.htm data/main/main.js")

def post_build_littlefs(source, target, env):
  print("[PostAction]: pre_build_littlefs")
  env.Execute("gzip -d data/setup/index.htm.gz data/setup/main.js.gz data/main/index.htm.gz data/main/main.js.gz")

env.AddPreAction("$BUILD_DIR/littlefs.bin", pre_build_littlefs)
env.AddPostAction("$BUILD_DIR/littlefs.bin", post_build_littlefs)
