OGL playground based on my library libbl99 abstraction, In this example you can explore Pixel Picking or Color Picking using sampled pixel color.

```
--- Help
q   - Terminate program
TAB - Switch to Picking mode (Just for simplicity, probably you will render colors in a separate buffer, take a look at Pixel Buffer Object)
r   - Reload Current Shader
h   - Remove Spacing
```

## Building Examples

Tested and run on Mac arm64

```bash
  $ brew install glfw3 cglm # (instead of cglm you can use linmath.h)
  $ make clean build
  $ ./build/app
```
