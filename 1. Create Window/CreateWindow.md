# <samp>Create Window</samp>

### <samp>Step by step</samp>
- Initialize GLFW
- Setup GLFW for OpenGL
- Create Window
- Take user input
- Open Window
- Close Window

## <samp>Enable 4x MSAA</samp>
```c++
glfwWindowHint(GLFW_SAMPLES, 4);
```
The function enables 4x MSAA. This means that each pixel of the windows buffer is rendered 4 subsample, result in a buffer that is 4 times larger.

In other words, the function takes a 200x100 pixel buffer and renders it as and 800x400 pixel buffer. This can improve the anti-aliasing quality of the image, but it also comes at a performance cost.

| MSAA Level | Buffer Size                   |
|:----------:|:------------------------------|
|     1x     | Original Size                 |
|     2x     | Twice the original size       |
|     4x     | Four times the original size  |
|     8x     | Eight times the original size |
