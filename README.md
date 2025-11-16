# Ving Programming Language

A simple programming language for web coding written in C.

## About

Ving is a programming language written in C that allows you to create websites using simple syntax. Compiles to clean HTML/CSS for maximum performance.

## Features

- Simple and intuitive syntax
- Fast compilation to HTML/CSS
- Ready-to-use GUI elements (buttons, text, images)
- Media elements (video, audio, iframe)
- Dark and light themes
- Buttons with links
- Text styling

## Quick Start

### Download Binary

**Linux:**
```bash
chmod +x vingc
./vingc example.ving output.html
```

**Windows:**
```bash
vingc.exe example.ving output.html
```

### Build from Source

```bash
git clone https://github.com/vingcode/ving.git
cd ving
make
./vingc example.ving output.html
```

## Examples

### Simple Website

```ving
style:black
title: My Website

text(bold,big): Hello, World!

button(st1,link:https://example.com):Click Me

img(big): photo.png
```

### Website with Media

```ving
style:black
title: Media Site

text(bold): Video
video(controls):video.mp4

text(bold): Audio
audio(controls):audio.mp3

text(bold): YouTube
iframe(allowfullscreen,width:560,height:315):https://youtube.com/embed/...
```

## Documentation

### Directives

- `style:black` - set dark theme
- `style:light` - set light theme
- `title: Title` - page title

### Elements

#### Button
```ving
button()                                    # Simple button
button(st1)                                 # Button with style
button(link:https://example.com):Click      # Button with link
button(st1,link:https://example.com):Click # Style and link
```

#### Text
```ving
text(): Plain text
text(bold): Bold text
text(italic): Italic text
text(bold,italic): Bold and italic
text(big): Large text
```

#### Image
```ving
img(): photo.png        # Regular image
img(big): photo.png     # Large image
```

#### Video
```ving
video(controls):video.mp4
video(autoplay,loop,muted):video.mp4
video(controls,width:800,height:450):video.mp4
```

#### Audio
```ving
audio(controls):audio.mp3
audio(controls,autoplay):audio.mp3
```

#### Iframe
```ving
iframe(src:https://example.com)
iframe(allowfullscreen,width:560,height:315):https://youtube.com/embed/...
```

Full documentation is available in [USAGE](USAGE) file.

## Building

### Requirements

- GCC compiler (version 4.9+)
- Make (optional)

### Build Commands

```bash
# Regular build
make

# Static build
make static

# Windows build (requires MinGW)
make windows
```

See [BUILD.md](BUILD.md) for details.

## Project Structure

```
ving/
├── vingc.c          # Compiler source code
├── Makefile         # Build file
├── README.md        # This file
├── USAGE            # Quick reference
├── BUILD.md         # Build instructions
├── index.html       # Main website page
├── docs.html        # Full documentation
└── download.html    # Download page
```

## Usage

```bash
# Basic usage
./vingc input.ving output.html

# If output file is not specified, output.html is created
./vingc input.ving
```

## CI/CD

### Automatic Build

The project automatically builds on every push to main/master/develop branches using GitHub Actions.

### Creating a Release

To create a release:

1. **Using Git tags (automatic):**
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```
   This will automatically trigger the release workflow, build binaries for Linux and Windows, and create a GitHub release.

2. **Using release script:**
   ```bash
   ./release.sh 1.0.0
   git tag v1.0.0
   git push origin v1.0.0
   ```

3. **Using GitHub CLI:**
   ```bash
   gh release create v1.0.0 --title "Release v1.0.0" --notes "Release notes"
   ```

The release workflow will:
- Build static Linux binary
- Build Windows .exe (if MinGW is available)
- Create release archives (tar.gz for Linux, zip for Windows)
- Upload to GitHub Releases automatically

## License

This project is distributed under the MIT license.

## Contributing

We welcome contributions to Ving:

1. Fork the repository
2. Create a branch for your feature
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## Contact

- GitHub Organization: [@vingcode](https://github.com/vingcode)
- Issues: [Create issue](https://github.com/vingcode/ving/issues)

## Project Status

Project is actively developed. Current version supports:
- Basic elements (button, text, img)
- Media elements (video, audio, iframe)
- Themes
- Buttons with links
- Text styling

## Roadmap

- Structural elements (header, footer, nav, section)
- Headings (h1-h6)
- Lists (ul, ol)
- Forms (input, textarea, submit)
- SEO meta tags
- External CSS/JS linking
- Tables
- Variables
- Modularity (include)
