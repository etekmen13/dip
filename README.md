# dip: A Digital Image Processing Utility

*A lightweight command-line tool for performing basic image transformations on RAW images.*

**By Emre Tekmen**

---

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
  - [Options](#options)
  - [Transformation Types](#transformation-types)
- [Examples](#examples)
- [Input Format](#input-format)

---

## Installation

### Dependencies
```bash
sudo apt update && sudo apt install -y gcc build-essential  # Debian/Ubuntu
```

### Build from Source
```bash
git clone https://github.com/etekmen13/dip.git
cd dip
make
```

---

## Usage
```
dip [OPTIONS] DEPTH WIDTH HEIGHT FILE1 [FILE2]
```

### Options
| Short | Long          | Description                                  | Default     |
|-------|---------------|----------------------------------------------|-------------|
| `-h`  | `--help`      | Show help screen                             |             |
| `-v`  | `--version`   | Show version                                 |             |
| `-o`  | `--output`    | Output file path                             | `./a.raw`   |
| `-t`  | `--transform` | Transformation type (see below)              | *Required*  |

### Transformation Types
| Type | Name             | Description                          | Required Files |
|------|------------------|--------------------------------------|----------------|
| `1`  | **Subtract**     | Pixel-wise subtraction: `FILE1 - FILE2` | 2 RAW files    |
| `2`  | **Negate**       | Invert pixel intensities             | 1 RAW file     |
| `3`  | **Translate**    | Shift image spatially                | 1 RAW file     |
| `4`  | **Intensity Slice** | Extract intensity range           | 1 RAW file     |
| `5`  | **Histogram EQ/Specialization** | 1 File: Equalization. 2 Files: Specifies `FILE1` histogram to target `FILE2` histogram | 1 or 2 RAW files |
---

## Examples

1. **Subtract Two Images**
   ```bash
   ./dip -t 1 -o result.raw 8 512 512 input1.raw input2.raw
   ```
   - `8`: Bit depth
   - `512 512`: Image dimensions (width × height)
   - Output saved to `result.raw`

2. **Negate an Image**
   ```bash
   ./dip -t 2 -o negated.raw 8 640 480 input.raw
   ```
3. **Histogram Equalization/Specification**
    * Histogram Equalization (one file):
    ```bash
    ./dip -t 5 -o equalized.raw 8 640 480 input.raw
    ```
    * Histogram Specialization (two files):
    ```bash
    ./dip -t 5 -o specified.raw 8 640 480 current.raw target.raw
    ```
---

## Input Format
- **RAW image files**: Uncompressed binary pixel data.
- **Parameters**:
  - `DEPTH`: Bit depth (e.g., 8-bit, 16-bit).
  - `WIDTH`/`HEIGHT`: Image dimensions in pixels.

*For certain transformations (e.g., Translate, Intensity Slice), arguments are requested at runtime*
