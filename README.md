<h1 align="center">yd_gui 📹</h1>

<h4 align="center">A QtQuick GUI wrapper for <a href="https://github.com/yt-dlp/yt-dlp" alt="yt-dlp GitHub">yt-dlp</a> for downloading YouTube videos</h4>

<p align="center">
<a href="" alt="GitHub Releases">
    <img alt="GitHub Release" src="https://img.shields.io/github/v/release/jasonly027/yd_gui?style=for-the-badge&label=Download">
</a>
<a href="https://github.com/jasonly027/yd_gui/actions/workflows/tests.yml" alt="GitHub Actions Tests">
    <img alt="GitHub Actions Workflow Status" src="https://img.shields.io/github/actions/workflow/status/jasonly027/yd_gui/tests.yml?style=for-the-badge&label=Tests">
</a>
<a href="https://github.com/jasonly027/yd_gui/blob/main/LICENSE" alt="License">
    <img alt="GitHub License" src="https://img.shields.io/github/license/jasonly027/yd_gui?style=for-the-badge">
</a>
<a href="https://github.com/jasonly027/yd_gui/commits/main/" alt="Commits">
    <img alt="GitHub Created At" src="https://img.shields.io/github/created-at/jasonly027/yd_gui?style=for-the-badge&logo=github&color=9370db">
</a>
</p>

<p align="center">
    <a href="#preview"><b>Preview</b></a> •
    <a href="#installation"><b>Installation</b></a> •
    <a href="#technologies"><b>Technologies</b></a> •
    <a href="#contributors"><b>Contributors</b></a>
</p>

<h2 id="preview">🎴 Preview</h2>

<h2 id="installation">⬇️ Installation</h2>

**yd_gui requires yt-dlp to work.** Install yt-dlp [here](https://github.com/yt-dlp/yt-dlp) and ensure its on the PATH.

### Pre-built installer (Recommended)
1. Download the installer from the [releases](https://github.com/jasonly027/yd_gui/releases) page.
2. Open installer and follow instructions.

### Building from source
#### Requires:

- Qt 6.6-6.7
- Recommended: Qt Image Formats module

#### Steps:

```
git clone https://github.com/jasonly027/yd_gui

cd yd_gui

cmake -S . -B build \
-D CMAKE_BUILD_TYPE=Release \
-D CMAKE_PREFIX_PATH=<QT INSTALL DIR. e.g.: "C:\Qt\6.7.2\msvc2019_64"> \
-D ENABLE_TESTING=OFF

cmake --build build --config Release --target yd_gui

cmake --install build
```

<h2 id="technologies">⚙️ Technologies</h2>

- [yt-dlp](https://github.com/yt-dlp/yt-dlp)
- [Qt Framework](https://www.qt.io/product/framework)
- [Google Test](https://github.com/google/googletest)
- [nlohmann_json](https://github.com/nlohmann/json)
- [SQLite](https://sqlite.org/)
- [Docker](https://www.docker.com/)
- [CMake](https://cmake.org/)

<h2 id="contributors">👥 Contributors</h2>

- [Jason Ly](https://github.com/jasonly027)
- UI design by [Priscilla Ho]()
