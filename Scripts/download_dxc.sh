#!/usr/bin/env bash
set -e

cd ..

DXC_DIR="$(dirname "$0")/../HeliosEngine/HeliosEngine/vendor/dxc"
BIN_DIR="$DXC_DIR/"

mkdir -p "$BIN_DIR"

echo "Detecting platform..."
PLATFORM="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  echo "macOS detected — no official DXC binaries available."
  echo "Please build DXC from source or use alternative shader compilation tools."
  exit 1
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
  echo "Windows detected — please run the PowerShell script for Windows."
  exit 1
else
  echo "Unsupported OS: $OSTYPE"
  exit 1
fi

echo "Platform detected: $PLATFORM"

declare -A URLS=(
  ["linux"]="https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505.1/linux_dxc_2025_07_14.x86_64.tar.gz"
)

DOWNLOAD_URL=${URLS[$PLATFORM]}

if [[ -z "$DOWNLOAD_URL" ]]; then
  echo "No download URL configured for platform: $PLATFORM"
  exit 1
fi

ARCHIVE_NAME="$BIN_DIR/dxcompiler.tar.gz"

echo "Downloading DXC binaries from $DOWNLOAD_URL..."
curl -L "$DOWNLOAD_URL" -o "$ARCHIVE_NAME"

echo "Extracting archive..."
tar -xzf "$ARCHIVE_NAME" -C "$BIN_DIR"

echo "Cleaning up..."
rm "$ARCHIVE_NAME"

echo "DXC binaries installed to $BIN_DIR"
