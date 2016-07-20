int init ()
{
    GLuint framebuffer;
    GLuint depthRenderbuffer;
    GLuint texture;
    GLint texWidth = 256, texHeight = 256;
    GLint maxRenderbufferSize;

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
    printf("maxRenderbufferSize=%d\n", maxRenderbufferSize);
// check if GL_MAX_RENDERBUFFER_SIZE is >= texWidth and texHeight
    if((maxRenderbufferSize <= texWidth) ||
       (maxRenderbufferSize <= texHeight))
    {
// cannot use framebuffer objects as we need to create
// a depth buffer as a renderbuffer object
// return with appropriate error
    }
    // generate the framebuffer, renderbuffer, and texture object names
    glGenFramebuffers(1, &framebuffer);
    glGenRenderbuffers(1, &depthRenderbuffer);
    glGenTextures(1, &texture);
    
    // bind texture and load the texture mip-level 0
    // texels are RGB565
    // no texels need to be specified as we are going to draw into
    // the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,
                 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

// bind renderbuffer and create a 16-bit depth buffer
// width and height of renderbuffer = width and height of
// the texture
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                          texWidth, texHeight);
// bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
// specify texture as color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);
// specify depth_renderbufer as depth attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthRenderbuffer);

// check for framebuffer complete
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status == GL_FRAMEBUFFER_COMPLETE)
    {
// render to texture using FBO
// clear color and depth buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// load uniforms for vertex and fragment shader
// used to render to FBO. The vertex shader is the
// ES 1.1 vertex shader described as Example 8-8 in
// Chapter 8. The fragment shader outputs the color
// computed by vertex shader as fragment color and
// is described as Example 1-2 in Chapter 1.
        set_fbo_texture_shader_and_uniforms();
// drawing commands to the framebuffer object
        draw_teapot();
// render to window system provided framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

// Use texture to draw to window system provided framebuffer
// We draw a quad that is the size of the viewport.
//
// The vertex shader outputs the vertex position and texture
// coordinates passed as inputs.
//
// The fragment shader uses the texture coordinate to sample
// the texture and uses this as the per-fragment color value.
        set_screen_shader_and_uniforms();
        draw_screen_quad();
    }
// cleanup
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texture);
}
