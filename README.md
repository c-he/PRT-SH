

# Precomputed Radiance Transfer

### Paper Information

Sloan P P, Kautz J, Snyder J. Precomputed radiance transfer for real-time rendering in dynamic, low-frequency lighting environments[C]//ACM Transactions on Graphics (TOG). ACM, 2002, 21(3): 527-536. 



### Platform

- IDE: Visual Studio 2017 15.7.2



### Dependencies

| Package name | Version |
| :----------: | :-----: |
| AntTweakBar  | 1.16.3  |
|    Assimp    |  3.0.0  |
|    Eigen     |  3.3.3  |
|     GLEW     | 1.9.0.1 |
|     GLFW     | 3.2.1.5 |
|     glm      | 0.9.8.5 |
|    OpenCV    |  3.1.0  |



### Results

- DIffuse  Surface Self-Transfer

![](https://github.com/Fairyland0902/PRT-SH/raw/master/screenshots/diffuse.png)

> From left to right: diffuse, unshadowed; diffuse, shadowed; diffuse, interreflected (bounce = 1)

- Glossy Surface Self-Transfer

![](https://github.com/Fairyland0902/PRT-SH/raw/master/screenshots/glossy.png)

> From left to right: glossy, unshadowed; glossy, shadowed; glossy, interreflected (bounce = 1)



### Resources

1. Slomp M P B, Oliveira Neto M M, Patrício D I. A gentle introduction to precomputed radiance transfer[J]. Revista de informática teórica e aplicada. Porto Alegre. Vol. 13, n. 2 (2006), p. 131-160, 2006. 
2. Green R. Spherical harmonic lighting: The gritty details[C]//Archives of the Game Developers Conference. 2003, 56: 4. 
3. Kautz J, Snyder J, Sloan P P J. Fast Arbitrary BRDF Shading for Low-Frequency Lighting Using Spherical Harmonics[J]. Rendering Techniques, 2002, 2(291-296): 1. 
4. [Learn OpenGL Tutorial](https://learnopengl.com/)



### Notes

1. It is highly recommended to compile this project in **Release** mode. BTW, you can choose either x86 or x64, but personally I prefer **x64**.
2. Because the data files for glossy objects are too large (**300MB**), we ignore them from git and you may need to generate them manually.
3. Since GLM version 0.9.9, GLM default initializes matrix types to a 0-initialized matrix, instead of the identity matrix. From that version it is required to initialize matrix types as: `glm::mat4 mat=glm::mat4(1.0f)`.