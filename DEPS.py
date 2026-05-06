import os

deps = {
  "externals/skity": {
    "type": "solution",
    "url": "git@github.com:RuiwenTang/skity-dev.git",
    "branch": "wgx_spirv_impl",
    "ignore_in_git": True,
    "deps_file": "hab/DEPS",
  },
  "externals/skity/third_party/libjpeg-turbo": {
    "type": "git",
    "url": "https://github.com/libjpeg-turbo/libjpeg-turbo.git",
    "ignore_in_git": True,
    "commit": "f29eda648547b36aa594c4116c7764a6c8a079b9",
    "patches": os.path.join(root_dir, 'externals/skity/patches', 'libjpeg-turbo', '0001-disable-TLS-in-Android.patch'),
    "require": ['externals/skity'],
  },
  "externals/skity/third_party/wuffs": {
    "type": "git",
    "url": "https://github.com/google/wuffs-mirror-release-c.git",
    "ignore_in_git": True,
    "commit": "a29749ebe0be57d2b19d8406475bd2326d0f1a85",
    "require": ['externals/skity'],
  },
  "externals/skity/third_party/libwebp": {
    "type": "git",
    "url": "https://github.com/webmproject/libwebp.git",
    "ignore_in_git": True,
    "commit": "4fa21912338357f89e4fd51cf2368325b59e9bd9",
    "require": ['externals/skity'],
  }
}