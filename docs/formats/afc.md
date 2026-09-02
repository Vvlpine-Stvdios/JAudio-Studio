<h1 style="text-align: center;">.afc</h1>

!!! warning "Warning!"
	This site is still under construction, and not all data is known (to me, at least).
	If you find the documentation in this faulty, misguiding, unclear, or straight missing information, please submit a [documentation issue](https://github.com/Vvlpine-Stvdios/JAudio-Studio/issues/new?template=documentation_correction.yml)!
	THIS SITE IS CURRENTLY DOCUMENTING JAUDIO VERSION ***<u>1</u>***. If you want information regarding JAudio Version 2, please go to [Luma’s Workshop](https://www.lumasworkshop.com/wiki/Main_Page).

## General Structure
Each AFC file starts with a 32-byte header, followed by all of the audio data. AFC files are all [ADPCM](./types-and-terminology.md/#what-the-heck-is-adpcm-or-pcm-for-that-matter), and in the case of
Wind Waker, ADPCM4.

### Header

|    Type    |       Name        |                                               Description                                                |
| ---------- | ----------------- | -------------------------------------------------------------------------------------------------------- |
| `u32`      | Size              |                                                                                                          |
| `u32`      | Sample Count      |                                                                                                          |
| `u16`      | Sample Rate       | A much more reasonable data type for this application                                                    |
| `u16`      | Bits per Sample   | This is unverified, but as every file is ADPCM4 and this is always 4, I am going with it.                |
| `u16`      | Samples per Frame | Also a guess, but it’s always 16, and all of the files have 16 samples per frame.                        |
| `u16`      | Unknown           | Either 0x1E (30) or 0x3C (60)                                                                            |
| `ub32`     | Loop?             |                                                                                                          |
| `u32`      | Loop Start        | Measured in Samples. The end is assumed to be the sample count—the end of the file.                      |
| `u16`      | Penult            | These are also my own assumption, as the penult and last are assumed to be zero anyways, which they are. |
| `u16`      | Last              | See above.                                                                                               |
| `u32`      | Padding           |                                                                                                          |

### Audio Format
The audio data is saved using **frames**, a nine-byte header/sample data structure.

|    Type     |         Name         |                                                               Description                                                               |
| ----------- | -------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| `Nibble`    | Scale Factor         | The recorded error is multiplied by this to the power of two                                                                            |
| `Nibble`    | Predicator Index     | This is used to get the coefficients to multiply the previous two samples by                                                            |
| `Nibble[n]` | Scaled Sample Errors | These are the recoreded errors. The number of these per frame is indicated by the header. In the case of Wind Waker, this is always 16. |

### Coefficient Table
In order to predict the next sample, the audio engine has a fixed lookup table of coefficients that each frame indexes into:

| Index | C~1~  | C~2~  |
| ----: | ----: | ----: |
|     0 |     0 |     0 |
|     1 |  2048 |     0 |
|     2 |     0 |  2048 |
|     3 |  1024 |  1024 |
|     4 |  4096 | -2048 |
|     5 |  3584 | -1536 |
|     6 |  3072 | -1024 |
|     7 |  4608 | -2560 |
|     8 |  4200 | -2248 |
|     9 |  4800 | -2300 |
|    10 |  5120 | -3072 |
|    11 |  2048 | -2048 |
|    12 |  1024 | -1024 |
|    13 | -1024 |  1024 |
|    14 | -1024 |     0 |
|    15 | -2048 |     0 |

### Math
So how does ADPCM actually calculate samples with this whole mess?

=== "Math"
    Let $S_n$ be the sample, $E$ be the error, $s$ be the scale factor, $c_1$ be the first coefficient, $c_2$ be the second coefficient, $S_{n-1}$ be the last sample, and $S_{n-2}$ be the penultimate sample.

    $$
    S_n = \frac{(E \cdot 2^{s} \cdot 2048) + c_1S_{n-1} + c_2S_{n-2}}{2048}
    $$

=== "Python"
    ```py
    coefs: list[list[int]] = [
        [     0,     0 ],
        [  2048,     0 ],
        [     0,  2048 ],
        [  1024,  1024 ],
        [  4096, -2048 ],
        [  3584, -1536 ],
        [  3072, -1024 ],
        [  4608, -2560 ],
        [  4200, -2248 ],
        [  4800, -2300 ],
        [  5120, -3072 ],
        [  2048, -2048 ],
        [  1024, -1024 ],
        [ -1024,  1024 ],
        [ -1024,     0 ],
        [ -2048,     0 ]
    ]

    def decode_sample(nibble, scale_factor, predicator_index, last, penult) -> int:
        # The nibble is in some way signed; when it is above 8 (or 0b_1XXX), we subtract 16 to make it negative.
        if nibble >= 8: nibble -= 16

        # Calculate the error by multiplying the recorded nibble by 2 raised to the scale factor.
        # Shifting the expression left by 11 is the same as multiplying by 2048.
        error: int = (nibble * (2 ** scale_factor)) << 11

        # Calculate the prediction by multiplying the previous samples by the given coefficients
        prediction: int = (coefs[predicator_index][0] * last) + (coefs[predicator_index][1] * penult)

        # Calculate the sample as the sum of the prediction and the error.
        # Shifting right by 11 is the same as dividing by 2048.
        sample: int = (prediction + error) >> 11
        
        # Clamp the sample between the minimum and maximum values of a signed 16-bit integer.
        sample = max(min(sample, 32767), -32768)

        return sample
    ```

=== "C++"
    ```cpp
    #include <cstdint>
    #include <algorithm>

    int16_t coefs[16][2] = {
        {     0,     0 },
        {  2048,     0 },
        {     0,  2048 },
        {  1024,  1024 },
        {  4096, -2048 },
        {  3584, -1536 },
        {  3072, -1024 },
        {  4608, -2560 },
        {  4200, -2248 },
        {  4800, -2300 },
        {  5120, -3072 },
        {  2048, -2048 },
        {  1024, -1024 },
        { -1024,  1024 },
        { -1024,     0 },
        { -2048,     0 },
    };

    int16_t decodeSample(int8_t nibble, int scaleFactor, int predicatorIndex, int16_t last, int16_t penult) {
        // The nibble is in some way signed; when it is above 8 (or 0b_1XXX), we subtract 16 to make it negative.
        if (nibble >= 8) { nibble -= 16; }

        // Calculate the error by multiplying the recorded nibble by 2 raised to the scale factor.
        // Shifting the expression left by 11 is the same as multiplying by 2048.
        int error = (nibble * (1 << scaleFactor)) << 11;

        // Calculate the prediction by multiplying the previous samples by the given coefficients
        int prediction = (coefs[predicatorIndex][0] * last) + (coefs[predicatorIndex][1] * penult);

        // Calculate the sample as the sum of the prediction and the error.
        // Shifting right by 11 is the same as dividing by 2048.
        int sample = (prediction + error) >> 11;
        
        // Clamp the sample between the minimum and maximum values of a signed 16-bit integer.
        sample = std::clamp(sample, INT16_MIN, INT16_MAX);

        return static_cast<int16_t>(sample);
    }
    ```

=== "C#"
    ```csharp
    using System;

    public short[,] Coefs = {
        {     0,     0 },
        {  2048,     0 },
        {     0,  2048 },
        {  1024,  1024 },
        {  4096, -2048 },
        {  3584, -1536 },
        {  3072, -1024 },
        {  4608, -2560 },
        {  4200, -2248 },
        {  4800, -2300 },
        {  5120, -3072 },
        {  2048, -2048 },
        {  1024, -1024 },
        { -1024,  1024 },
        { -1024,     0 },
        { -2048,     0 },
    };

    public short DecodeSample(int nibble, int scaleFactor, int predicatorIndex, short last, short penult) {
        // The nibble is in some way signed; when it is above 8 (or 0b_1XXX), we subtract 16 to make it negative.
        if (nibble >= 8) { nibble -= 16; }

        // Calculate the error by multiplying the recorded nibble by 2 raised to the scale factor.
        // Shifting the expression left by 11 is the same as multiplying by 2048.
        int error = (nibble * (1 << scaleFactor)) << 11;

        // Calculate the prediction by multiplying the previous samples by the given coefficients
        int prediction = (Coefs[predicatorIndex, 0] * last) + (Coefs[predicatorIndex, 1] * penult);

        // Calculate the sample as the sum of the prediction and the error.
        // Shifting right by 11 is the same as dividing by 2048.
        int sample = (prediction + error) >> 11;
        
        // Clamp the sample between the minimum and maximum values of a signed 16-bit integer.
        sample = Math.Clamp(sample, Int16.MinValue, Int16.MaxValue);

        return (short)sample;
    }
    ```