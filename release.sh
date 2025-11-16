#!/bin/bash
# Script to create a release

set -e

VERSION=${1:-"1.0.0"}
TAG="v${VERSION}"

echo "Creating release ${TAG}..."

# Build Linux version
echo "Building Linux version..."
make clean
make static
chmod +x vingc

# Build Windows version (if MinGW is available)
if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    echo "Building Windows version..."
    x86_64-w64-mingw32-gcc -Wall -Wextra -std=c11 -o vingc.exe vingc.c
else
    echo "Warning: MinGW not found, skipping Windows build"
fi

# Create release directory
mkdir -p release
cp vingc release/vingc-linux-x86_64 2>/dev/null || true
cp vingc.exe release/vingc-windows-x86_64.exe 2>/dev/null || true
cp README.md release/ 2>/dev/null || true
cp USAGE release/ 2>/dev/null || true

# Create archives
if [ -f release/vingc-linux-x86_64 ]; then
    echo "Creating Linux archive..."
    tar -czf ving-${TAG}-linux.tar.gz -C release vingc-linux-x86_64 README.md USAGE
fi

if [ -f release/vingc-windows-x86_64.exe ]; then
    echo "Creating Windows archive..."
    zip -r ving-${TAG}-windows.zip release/vingc-windows-x86_64.exe README.md USAGE
fi

echo "Release files created:"
ls -lh ving-${TAG}-*.tar.gz ving-${TAG}-*.zip 2>/dev/null || true

echo ""
echo "To create GitHub release, run:"
echo "  git tag ${TAG}"
echo "  git push origin ${TAG}"
echo ""
echo "Or use GitHub CLI:"
echo "  gh release create ${TAG} ving-${TAG}-*.tar.gz ving-${TAG}-*.zip --title \"Release ${TAG}\" --notes \"Release ${TAG}\""

