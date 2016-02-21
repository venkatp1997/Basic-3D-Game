#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <set>
#include "SOIL.h" 


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define pb push_back
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>


using namespace std;

#define TRACE

#ifdef TRACE
#define trace1(x)                cerr << #x << ": " << x << endl;
#define trace2(x, y)             cerr << #x << ": " << x << " | " << #y << ": " << y << endl;
#define trace3(x, y, z)          cerr << #x << ": " << x << " | " << #y << ": " << y << " | " << #z << ": " << z << endl;
#define trace4(a, b, c, d)       cerr << #a << ": " << a << " | " << #b << ": " << b << " | " << #c << ": " << c << " | " << #d << ": " << d << endl;
#define trace5(a, b, c, d, e)    cerr << #a << ": " << a << " | " << #b << ": " << b << " | " << #c << ": " << c << " | " << #d << ": " << d << " | " << #e << ": " << e << endl;
#define trace6(a, b, c, d, e, f) cerr << #a << ": " << a << " | " << #b << ": " << b << " | " << #c << ": " << c << " | " << #d << ": " << d << " | " << #e << ": " << e << " | " << #f << ": " << f << endl;

#else

#define trace1(x)
#define trace2(x, y)
#define trace3(x, y, z)
#define trace4(a, b, c, d)
#define trace5(a, b, c, d, e)
#define trace6(a, b, c, d, e, f)

#endif
struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;
    GLuint TextureID;
    GLuint TextureBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
    GLuint TexMatrixID;
} Matrices;
struct Point{
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Point;
glm::mat4 VP;
glm::mat4 MVP;  // MVP = Projection * View * Model
vector<int> obs,obs1;
vector<int> dit;
GLfloat camera_x,camera_y,camera_z,look_x,look_y,look_z,camera_lx,camera_ly,camera_lz,hel_x,hel_y,hel_z,hel_a=0.0,hel_aa=0.0;
int state_a=0,state_d=0,state_w=0,state_s=0,state_fall=0,state_c=0,state_q=0,hel_v=0,state_mouse=0,state_jump=0,state_left=0,state_right=0,state_up=0,state_down=0;
int state_mov=0,state_mov1=0,state_i=0,state_de=0,state_i1=0,state_de1=0,temp=0,state[10],state_ditch=0,state_out=0;  
GLfloat fact[100][100];
int total_no;
GLfloat side_len;
VAO* rect[10];
GLuint text[10];
int camera_1=1,camera_2=0,camera_3=0,camera_4=0,camera_5=0;
vector<pair<int,int> > ff;
int factf[100][100];
set<pair<int,pair<int,int> > > ss;
double mouseX,mouseY,prevX;
int dis[1000];
GLfloat max_h;
GLuint programID, fontProgramID, textureProgramID;
void draw3DObject(struct VAO* vao);
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode);
float camera_rotation_angle=90;
struct VAO* create3DTexturedObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* texture_buffer_data, GLuint textureID, GLenum fill_mode);
void draw3DTexturedObject (struct VAO* vao);
void trans_rot(VAO* object,GLfloat xx,GLfloat yy,GLfloat a){
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translateRectangle = glm::translate (glm::vec3(xx,yy, 0));
    glm::mat4 rotateRectangle = glm::rotate((float)(a*M_PI/180.0f), glm::vec3(0,0,1)); 
    Matrices.model *= (translateRectangle*rotateRectangle);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DTexturedObject(object);
}
void rotate_a(VAO* object,GLfloat a,GLfloat x,GLfloat y){
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translateRectangle = glm::translate (glm::vec3(-x,-y, 0));
    glm::mat4 rotateRectangle = glm::rotate((float)(a*M_PI/180.0f), glm::vec3(0,0,1)); 
    glm::mat4 translateRectangle1 = glm::translate (glm::vec3(x,y, 0));
    Matrices.model *= (translateRectangle1*rotateRectangle*translateRectangle);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(object);
}
struct Point rotate_ap(GLfloat px,GLfloat py,GLfloat pz,GLfloat x,GLfloat y,GLfloat z,GLfloat a){
    GLfloat nx=x-px;
    GLfloat nz=z-pz;
    nx=cos(a)*nx+sin(a)*nz;
    nz=-sin(a)*nx+cos(a)*nz;
    nx+=px,nz+=pz;
    struct Point ret={
        nx,y,nz
    };
    return ret;
}
class Circle{
    public:
        GLfloat x,y,radius,in_hspeed,in_vspeed,h_speed,v_speed,angle,mass;
        VAO* object;
        void init(GLfloat xx,GLfloat yy,GLfloat r,GLfloat rr,GLfloat g,GLfloat b){
            in_hspeed=in_vspeed=h_speed=v_speed=0.0;
            x=xx,y=yy;
            mass=M_PI*r*r;
            radius=r;
            object=createCircle(0.0,0.0,0.0,radius,54,rr,g,b);
        }
        void transform(GLfloat xx,GLfloat yy,GLfloat a){x=xx,y=yy,angle=a,trans_rot(object,x,y,a);}
    private:
        VAO* createCircle( GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides,double r, double g, double bb)
        {
            int numberOfVertices = numberOfSides + 2,i;

            GLfloat twicePi = 2.0f * M_PI;

            GLfloat circleVerticesX[numberOfVertices];
            GLfloat circleVerticesY[numberOfVertices];
            GLfloat circleVerticesZ[numberOfVertices];

            circleVerticesX[0] = x;
            circleVerticesY[0] = y;
            circleVerticesZ[0] = z;

            for ( i = 1; i < numberOfVertices; i++ ) {
                circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
                circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
                circleVerticesZ[i] = z;
            }

            GLfloat allCircleVertices[( numberOfVertices ) * 3];
            GLfloat allCircleColors[( numberOfVertices ) * 3 * 3];

            for ( int i = 0; i < numberOfVertices; i++ )
            {
                allCircleVertices[i * 3] = circleVerticesX[i];
                allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
                allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
            }
            for (int j=0;j<(numberOfVertices*3);j++){
                if(j%3==0)allCircleColors[j]=r;
                else if(j%3==1)allCircleColors[j]=g;
                else allCircleColors[j]=bb;
            }
            VAO* ret=create3DObject(GL_TRIANGLE_FAN,numberOfVertices, allCircleVertices, allCircleColors, GL_TRIANGLES);
            return ret;
        }
};
GLuint textureID,textureID1,textureID2,textureID3;
class Cube{
    public:
        GLfloat l,x,y,z,a,aa;
        void createUnitCube(GLuint textureID,GLfloat length,GLfloat r=0.0f,GLfloat g=0.0f,GLfloat b=0.0f,GLfloat rr=1.0f,GLfloat gg=0.0f,GLfloat bb=1.0f, GLfloat r1=0.0f,GLfloat r2=1.0f,GLfloat r3=1.0f,GLfloat g1=0.0f,GLfloat g2=1.0f,GLfloat g3=0.0f,GLfloat scaleX=1,GLfloat scaleY=1,GLfloat scaleZ=1){
            l=length,x=y=z=a=aa=0.0;
            GLfloat temp=l/2.0;
            GLfloat g_vertex_buffer_data[] = {
                -temp/scaleX,-temp/scaleY,-temp/scaleZ,//1
                -temp/scaleX,-temp/scaleY, temp/scaleZ,
                -temp/scaleX, temp/scaleY, temp/scaleZ,

                temp/scaleX, temp/scaleY,-temp/scaleZ,//2
                -temp/scaleX,-temp/scaleY,-temp/scaleZ,
                -temp/scaleX, temp/scaleY,-temp/scaleZ,

                temp/scaleX,-temp/scaleY, temp/scaleZ,//3
                -temp/scaleX,-temp/scaleY,-temp/scaleZ,
                temp/scaleX,-temp/scaleY,-temp/scaleZ,

                temp/scaleX, temp/scaleY,-temp/scaleZ,//2
                temp/scaleX,-temp/scaleY,-temp/scaleZ,
                -temp/scaleX,-temp/scaleY,-temp/scaleZ,

                -temp/scaleX,-temp/scaleY,-temp/scaleZ,//1
                -temp/scaleX, temp/scaleY, temp/scaleZ,
                -temp/scaleX, temp/scaleY,-temp/scaleZ,

                temp/scaleX,-temp/scaleY, temp/scaleZ,//3
                -temp/scaleX,-temp/scaleY, temp/scaleZ,
                -temp/scaleX,-temp/scaleY,-temp/scaleZ,

                -temp/scaleX, temp/scaleY, temp/scaleZ,//4
                -temp/scaleX,-temp/scaleY, temp/scaleZ,
                temp/scaleX,-temp/scaleY, temp/scaleZ,

                temp/scaleX, temp/scaleY, temp/scaleZ,//5
                temp/scaleX,-temp/scaleY,-temp/scaleZ,
                temp/scaleX, temp/scaleY,-temp/scaleZ,

                temp/scaleX,-temp/scaleY,-temp/scaleZ,//5
                temp/scaleX, temp/scaleY, temp/scaleZ,
                temp/scaleX,-temp/scaleY, temp/scaleZ,

                temp/scaleX, temp/scaleY, temp/scaleZ,//6
                temp/scaleX, temp/scaleY,-temp/scaleZ,
                -temp/scaleX, temp/scaleY,-temp/scaleZ,

                temp/scaleX, temp/scaleY, temp/scaleZ,//6
                -temp/scaleX, temp/scaleY,-temp/scaleZ,
                -temp/scaleX, temp/scaleY, temp/scaleZ,

                temp/scaleX, temp/scaleY, temp/scaleZ,//4
                -temp/scaleX, temp/scaleY, temp/scaleZ,
                temp/scaleX,-temp/scaleY, temp/scaleZ
            };
            GLfloat g_color_buffer_data[] = {
                rr,gg,bb,
                rr,gg,bb,
                rr,gg,bb,

                g1,g2,g3,
                g1,g2,g3,
                g1,g2,g3,

                0.0f,0.0f,1.0f,
                0.0f,0.0f,1.0f,
                0.0f,0.0f,1.0f,

                g1,g2,g3,
                g1,g2,g3,
                g1,g2,g3,

                rr,gg,bb,
                rr,gg,bb,
                rr,gg,bb,

                0.0f,0.0f,1.0f,
                0.0f,0.0f,1.0f,
                0.0f,0.0f,1.0f,

                rr,gg,bb,
                rr,gg,bb,
                rr,gg,bb,

                r1,r2,r3,
                r1,r2,r3,
                r1,r2,r3,

                r1,r2,r3,
                r1,r2,r3,
                r1,r2,r3,

                r,g,b,
                r,g,b,
                r,g,b,

                r,g,b,
                r,g,b,
                r,g,b,

                rr,gg,bb,
                rr,gg,bb,
                rr,gg,bb
            };
            float t[]={
                1,1,
                1,0,
                0,0,
                0,0,
                1,1,
                1,0,
                0,0,
                1,1,
                0,1,
                0,0,
                0,1,
                1,1,
                1,1,
                0,0,
                0,1,
                0,0,
                1,0,
                1,1,
                0,0,
                1,0,
                1,1,
                0,0,
                1,1,
                0,1,
                1,1,
                0,0,
                1,0,
                0,0,
                0,1,
                1,1,
                0,0,
                1,1,
                1,0,
                0,1,
                0,0,
                1,1
            };
            /* object = create3DObject(GL_TRIANGLES, 36, g_vertex_buffer_data, g_color_buffer_data, GL_FILL); */
            object = create3DTexturedObject(GL_TRIANGLES, 36, g_vertex_buffer_data,t, textureID, GL_FILL);

        }
        void transrot(GLfloat xx,GLfloat yy,GLfloat zz,int f=0,GLfloat s=1){
            x=xx,y=yy,z=zz;
            GLfloat aa=a*M_PI/180.0;
            Matrices.model = glm::mat4(1.0f);
            glm::mat4 translateCube=glm::translate(glm::vec3(x,y,z));
            glm::mat4 rotateCube= glm::rotate((float)(aa), glm::vec3(0,1,0)); 
            glm::mat4 scaleCube= glm::scale(glm::vec3(s,s,s));
            Matrices.model *= (scaleCube*translateCube*rotateCube);
            // Matrices.model *= (rotateCube*translateCube*scaleCube);
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            /* if(!f)draw3DObject(object); */
            if(!f)draw3DTexturedObject(object);
        }
    private:
        VAO* object;
};

// GLuint programID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
            0,                  // attribute 0. Vertices
            3,                  // size (x,y,z)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
            1,                  // attribute 1. Color
            3,                  // size (r,g,b)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}
glm::vec3 getRGBfromHue (int hue)
{
    float intp;
    float fracp = modff(hue/60.0, &intp);
    float x = 1.0 - abs((float)((int)intp%2)+fracp-1.0);

    if (hue < 60)
        return glm::vec3(1,x,0);
    else if (hue < 120)
        return glm::vec3(x,1,0);
    else if (hue < 180)
        return glm::vec3(0,1,x);
    else if (hue < 240)
        return glm::vec3(0,x,1);
    else if (hue < 300)
        return glm::vec3(x,0,1);
    else
        return glm::vec3(1,0,x);
}
struct VAO* create3DTexturedObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* texture_buffer_data, GLuint textureID, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;
    vao->TextureID = textureID;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->TextureBuffer));  // VBO - textures

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
            0,                  // attribute 0. Vertices
            3,                  // size (x,y,z)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    glBindBuffer (GL_ARRAY_BUFFER, vao->TextureBuffer); // Bind the VBO textures
    glBufferData (GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), texture_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
            2,                  // attribute 2. Textures
            2,                  // size (s,t)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    return vao;
}
void draw3DTexturedObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Bind Textures using texture units
    glBindTexture(GL_TEXTURE_2D, vao->TextureID);

    // Enable Vertex Attribute 2 - Texture
    glEnableVertexAttribArray(2);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->TextureBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

    // Unbind Textures to be safe
    glBindTexture(GL_TEXTURE_2D, 0);
}

/* Create an OpenGL Texture from an image */
GLuint createTexture (const char* filename)
{
    GLuint TextureID;
    // Generate Texture Buffer
    glGenTextures(1, &TextureID);
    // All upcoming GL_TEXTURE_2D operations now have effect on our texture buffer
    glBindTexture(GL_TEXTURE_2D, TextureID);
    // Set our texture parameters
    // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering (interpolation)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load image and create OpenGL texture
    int twidth, theight;
    unsigned char* image = SOIL_load_image(filename, &twidth, &theight, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate MipMaps to use
    SOIL_free_image_data(image); // Free the data read from file after creating opengl texture
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess it up

    return TextureID;
}


/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
       is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    GLfloat fov = 90.0f;

    // sets the viewport of openGL renderer
    glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

    // set the projection matrix as perspective
    /* glMatrixMode (GL_PROJECTION);
       glLoadIdentity ();
       gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
    // Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    //Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
    /* Matrices.projection = glm::ortho(-10.0f, 80.0f, -15.0f, 15.0f, 0.1f, 500.0f); */
}
// Creates the rectangle object used in this sample code
/* Render the scene with openGL */
/* Edit this function according to your assignment */
Cube surface[25000];
Cube player;
Cube camera;
Cube back;
VAO* createRectangle (GLuint textureID)
{
    VAO* rectangle;
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data [] = {
		-300,-300,0, // vertex 1
		300,-300,0, // vertex 2
		300, 300,0, // vertex 3

		300, 300,0, // vertex 3
		-300, 300,0, // vertex 4
		-300,-300,0  // vertex 1
	};

	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 1
		0,0,1, // color 2
		0,1,0, // color 3

		0,1,0, // color 3
		0.3,0.3,0.3, // color 4
		1,0,0  // color 1
	};

	// Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
	static const GLfloat texture_buffer_data [] = {
		0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left
		0,1  // TexCoord 1 - bot left
	};

	// create3DTexturedObject creates and returns a handle to a VAO that can be used later
	rectangle = create3DTexturedObject(GL_TRIANGLES, 6, vertex_buffer_data, texture_buffer_data, textureID, GL_FILL);
        return rectangle;
}
void draw ()
{
    // clear the color and depth in the fra/me buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);
    glUseProgram(textureProgramID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    //  Don't change unless you are sure!!
    GLfloat a=player.a*M_PI/180;
    struct Point t1 = rotate_ap(player.x,player.y,player.z,camera_x,camera_y,camera_z,a);
    struct Point t2 = rotate_ap(player.x,player.y,player.z,look_x,look_y,look_z,a);
    struct Point t3 = rotate_ap(player.x,player.y,player.z,camera_lx,camera_ly,camera_lz,a);
    struct Point t4 = rotate_ap(0.0,20.0,20.0,camera.x,camera.y,camera.z,camera.a*M_PI/180);
    camera_x=t1.x,camera_y=t1.y,camera_z=t1.z;
    look_x=t2.x,look_y=t2.y,look_z=t2.z;
    camera_lx=t3.x,camera_ly=t3.y,camera_lz=t3.z;
    camera.x=t4.x,camera.y=t4.y,camera.z=t4.z;
    player.a=0;
    if(camera_1){
        Matrices.view = glm::lookAt(glm::vec3(0,100,130), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
        hel_v=0;
    }
    else if(camera_2){
        Matrices.view = glm::lookAt(glm::vec3(camera_x,player.y+5,camera_z), glm::vec3(look_x,player.y+5,look_z), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
        hel_v=0;
    }
    else if(camera_3){
        Matrices.view = glm::lookAt(glm::vec3(camera_lx,player.y+5,camera_lz), glm::vec3(look_x,player.y+5,look_z), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
        hel_v=0;
    }
    else if (camera_4){
        Matrices.view = glm::lookAt(glm::vec3(0,150,30), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane 
        hel_v=0;
    }
    else{
        Matrices.view = glm::lookAt(glm::vec3(camera.x,camera.y,camera.z), glm::vec3(0,20,20), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane 
        hel_v=1;
    }

    /* Matricesm.view = glm::lookAt(glm::vec3(1000,10,5), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane */

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    //  Don't change unless you are sure!!
    VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    //  Don't change unless you are sure!!

    // Load identity to model matrix
    GLfloat t=-100;
    GLfloat tt=100;
    int k=0;
    for(int i=0;i<10;i++){
        t=-100;
        for(int j=0;j<11;j++){
            int f=0;
            for(int l=0;l<dit.size();l++){if(k==dit[l]){f=1;surface[k++].transrot(t,10,tt,1);break;}}
            if(!f){
                if((i!=1 || j!=1) && (i!=6 || j!=7) && (i!=3 || j!=6))surface[k++].transrot(t,0,tt);
                if(i==5 && j==5 || i==2 && j==8 || i==8 && j==2 || i==4 && j==1){
                    surface[k++].transrot(t,0,tt);
                    obs.pb(k-1);
                    for(int l=1;l<total_no;l++){
                        if(l<=state_mov)surface[k].transrot(t,surface[k-1].y+2*side_len,tt),max_h=surface[k].y+side_len;
                        k++;
                    }
                }
                if(i==1 && j==1 || i==6 && j==7 || i==3 && j==6){
                    ss.insert(make_pair(k,make_pair(i,j)));
                    surface[k++].transrot(t,0,tt,0,fact[i][j]);
                }
            }
            t+=20;
        }
        tt-=20;
    }
    if(!state_fall)player.transrot(player.x,player.y,player.z,0);
    else player.transrot(player.x,player.y-2,player.z,0);
    back.transrot(0,0,0,0);
    /* trans_rot(rect[1],0,30,0); */
    /* trans_rot(rect[1],60,60,-60); */
    /* /1* trans_rot(rect[2],60,60,0); *1/ */
    /* /1* trans_rot(rect[3],60,60,0); *1/ */
}
/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
       is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    glActiveTexture(GL_TEXTURE0);
    // load an image file directly as a new OpenGL texture
    // GLuint texID = SOIL_load_OGL_texture ("beach.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS); // Buggy for OpenGL3
    textureID = createTexture("block.jpg");
    textureID1 = createTexture("1.jpg");
    textureID2 = createTexture("4.jpg");
    textureID3 = createTexture("nightsky.jpg");
    text[0]=createTexture("back2.png");
    text[1]=createTexture("left2.png");
    text[2]=createTexture("right2.png");
    text[3]=createTexture("down2.png");
    text[4]=createTexture("up2.png");
    text[5]=createTexture("front2.png");
    
    for(int i=0;i<6;i++)
        rect[i]=createRectangle(text[i]);
    // check for an error during the load process
    if(textureID1 == 0 )
        cout << "SOIL loading error: '" << SOIL_last_result() << "'" << endl;

    // Create and compile our GLSL program from the texture shaders
    textureProgramID = LoadShaders( "TextureRender.vert", "TextureRender.frag" );
    // Get a handle for our "MVP" uniform
    Matrices.TexMatrixID = glGetUniformLocation(textureProgramID, "MVP");

    /* Objects should be created before any other gl function and shaders */
    // Create the models

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    int k=0;
    GLfloat scaleY=50;
    total_no=250;
    side_len=10/scaleY;
    for(int i=0;i<10;i++){
        for(int j=0;j<11;j++){
            surface[k++].createUnitCube(textureID1,20,193.0/255.0,154.0/255.0,107.0/255.0,193.0/255.0,111.0/255.0,107.0/255.0,193.0/255.0,154.0/255.0,107.0/255.0);    
            if(i==5 && j==5 || i==2 && j==8 || i==8 && j==2 || i==4 && j==1){
                obs.pb(k-1);
                for(int l=0;l<total_no;l++){
                    surface[k++].createUnitCube(textureID,20,139.0/255.0,69.0/255.0,19.0/255.0,139.0/255.0,69.0/255.0,19.0/255.0,139.0/255.0,69.0/255.0,19.0/255.0,90.0/255.0,45.0/255.0,12.0/255.0,1,scaleY,1);
                }
            }
            if(i==21 && j==8 || i==5 && j==7 || i==9 && j==8 || i==0 && j==5){
                dit.pb(k-1);
            }
        }
        ff.push_back(make_pair(1,1));
        ff.push_back(make_pair(6,7));
        ff.push_back(make_pair(3,6));
    }
    player.createUnitCube(textureID2,10,0,0,0,0,0,0,0,0,0,0,0,0);
    camera.createUnitCube(10,0,0,0,0,0,0,0,0,0,0,0,0);
    back.createUnitCube(textureID3,600,0,0,0,0,0,0,0,0,0,0,0,0);
    player.transrot(-95,20,95);
    camera.transrot(0,150,150);
    camera_x=player.x,camera_y=player.y,camera_z=player.z-player.l;
    camera_lx=player.x,camera_ly=player.y+2*player.l,camera_lz=player.z+2*player.l;
    look_x=player.x,look_y=player.y,look_z=player.z-100;
    hel_x=0.0,hel_y=100.0,hel_z=200.0;
}
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Function is called first on GLFW_PRESS.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_A:
                state_a=1;
                break;
            case GLFW_KEY_D:
                state_d=1;
                break;
            case GLFW_KEY_W:
                state_w=1;
                break;
            case GLFW_KEY_S:
                state_s=1;
                break;
            case GLFW_KEY_C:
                state_c=!state_c;
                break;
            case GLFW_KEY_RIGHT:
                state_right=1;
                break;
            case GLFW_KEY_LEFT:
                state_left=1;
                break;
            case GLFW_KEY_UP:
                state_up=1;
                break;
            case GLFW_KEY_DOWN:
                state_down=1;
                break;
            case GLFW_KEY_Q:
                state_q=1;
                break;
            case GLFW_KEY_SPACE:
                if(!state_jump)state_jump=1;
                break;
            case GLFW_KEY_1:
                camera_1=1;
                break;
            case GLFW_KEY_2:
                camera_1=0;
                camera_2=1;
                break;
            case GLFW_KEY_3:
                camera_1=0;
                camera_2=0;
                camera_3=1;
                break;
            case GLFW_KEY_4:
                camera_1=0;
                camera_2=0;
                camera_3=0;
                camera_4=1;
                break;
            case GLFW_KEY_5:
                camera_1=0;
                camera_2=0;
                camera_3=0;
                camera_4=0;
                camera_5=1;
                break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_A:
                state_a=0;
                break;
            case GLFW_KEY_D:
                state_d=0;
                break;
            case GLFW_KEY_W:
                state_w=0;
                break;
            case GLFW_KEY_S:
                state_s=0;
                break;
            case GLFW_KEY_RIGHT:
                state_right=0;
                break;
            case GLFW_KEY_LEFT:
                state_left=0;
                break;
            case GLFW_KEY_UP:
                state_up=0;
                break;
            case GLFW_KEY_DOWN:
                state_down=0;
                break;
        }
    }

}
bool checkCollision(GLfloat xx,GLfloat yy,GLfloat zz,GLfloat l,GLfloat x,GLfloat y,GLfloat z,GLfloat lengthX,GLfloat lengthY,GLfloat lengthZ)
{
    if(fabs(xx - x) < (l + lengthX))
    {
        if(fabs(yy - y) < (l + lengthY))
        {
            if(fabs(zz - z) < (l + lengthZ))
            {
                return true;
            }
        }
    }

    return false;
} 
bool collide(int i){
    GLfloat l1x=player.x-player.l/2;
    GLfloat l1y=player.z+player.l/2;
    GLfloat r1x=player.x+player.l/2;
    GLfloat r1y=player.z-player.l/2;

    GLfloat l2x=surface[i].x-surface[i].l/2-5;
    GLfloat l2y=surface[i].z+surface[i].l/2+5;
    GLfloat r2x=surface[i].x+surface[i].l/2+5;
    GLfloat r2y=surface[i].z-surface[i].l/2-5;


    if (l1x > r2x || l2x > r1x){
        return false;
    }

    // If one rectangle is above other
    if (l1y < r2y || l2y < r1y){
        return false;
    }
    return true;
}
bool collide1(int i){
    GLfloat l1x=player.x-player.l/2;
    GLfloat l1y=player.z+player.l/2;
    GLfloat r1x=player.x+player.l/2;
    GLfloat r1y=player.z-player.l/2;

    GLfloat l2x=surface[obs[i]].x-surface[obs[i]].l/2-5;
    GLfloat l2y=surface[obs[i]].z+surface[obs[i]].l/2+5;
    GLfloat r2x=surface[obs[i]].x+surface[obs[i]].l/2+5;
    GLfloat r2y=surface[obs[i]].z-surface[obs[i]].l/2-5;


    if (l1x > r2x || l2x > r1x){
        return false;
    }

    // If one rectangle is above other
    if (l1y < r2y || l2y < r1y){
        return false;
    }
    return true;
}
bool collideD(int i){
    GLfloat l1x=player.x-player.l/2;
    GLfloat l1y=player.z+player.l/2;
    GLfloat r1x=player.x+player.l/2;
    GLfloat r1y=player.z-player.l/2;

    GLfloat l2x=surface[dit[i]].x-surface[dit[i]].l/2;
    GLfloat l2y=surface[dit[i]].z+surface[dit[i]].l/2;
    GLfloat r2x=surface[dit[i]].x+surface[dit[i]].l/2;
    GLfloat r2y=surface[dit[i]].z-surface[dit[i]].l/2;


    if (l1x > r2x || l2x > r1x){
        return false;
    }

    // If one rectangle is above other
    if (l1y < r2y || l2y < r1y){
        return false;
    }
    return true;
}
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    double a,b;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if(action == GLFW_PRESS){
                if(!state_mouse)state_mouse=1,prevX=mouseX;
            }
            if(action==GLFW_RELEASE){
                camera.a=0;
                state_mouse=0;
            }
            break;
        default:
            break;
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    GLfloat a=camera.aa*M_PI/180;
    if(yoffset==1){camera.y/=1.02;}//,camera.x/=1.02;}//camera.z-=cos(a);camera.x-=sin(a);}
    if(yoffset==-1){camera.y*=1.02;}//,camera.x*=1.02;}//camera_z++,camera_x++;}//camera.z+=cos(a);camera.x+=sin(a);}
    }
int main (int argc, char** argv)
{
    int width = 600;
    int height = 600;
    int f=0;
    GLfloat dx,dy,dz;
    for(int i=0;i<20;i++){for(int j=0;j<20;j++)fact[i][j]=1,factf[i][j]=1;}

    GLFWwindow* window = initGLFW(width, height);

    initGL (window, width, height);

    double last_update_time = glfwGetTime(), current_time;
    double last_update_time1 = glfwGetTime(), current_time1;

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        f=0,dx=0,dy=0,dz=0;
        if(state_q)break;

        // OpenGL Draw commands
        draw();

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);
        glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
        glfwGetCursorPos(window,&mouseX,&mouseY);
        glfwSetKeyCallback(window, keyboard); 
        glfwSetScrollCallback(window, scroll_callback);


        // Poll for Keyboard and mouse events
        glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        current_time1 = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= 0.0005) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..
            last_update_time = current_time;
            if(state_mov==0)state_i=1,state_de=0;
            if(state_mov==(total_no-1))state_i=0,state_de=1;
            if(state_mov1==0)state_i1=1,state_de1=0;
            if(state_mov1==(total_no-1))state_i1=0,state_de1=1;
            if(state_i)state_mov=(state_mov+1)%total_no;
            if(state_de)state_mov=(state_mov-1)%total_no;
            if(state_i1)state_mov1=(state_mov1+1)%total_no;
            if(state_de1)state_mov1=(state_mov1-1)%total_no;
        }
        if ((current_time1 - last_update_time1) >= 2) { // atleast 0.5s elapsed since last frame
            last_update_time1 = current_time1;
            for(int i=0;i<ff.size();i++){
                if(factf[ff[i].first][ff[i].second])fact[ff[i].first][ff[i].second]-=0.005;
                if(fact[ff[i].first][ff[i].second]<=0.5)factf[ff[i].first][ff[i].second]=0,fact[ff[i].first][ff[i].second]=0.5;
                if(!factf[ff[i].first][ff[i].second])fact[ff[i].first][ff[i].second]+=0.005;
                if(fact[ff[i].first][ff[i].second]>=1)factf[ff[i].first][ff[i].second]=1;
            }
        }
        for (std::set<pair<int,pair<int,int> > >::iterator it=ss.begin(); it!=ss.end(); ++it){
            int kk=(*it).first;
            int ii=(*it).second.first;
            int jj=(*it).second.second;
            if(collide(kk) && fact[ii][jj]<=0.8)state_fall=1,state_ditch=1;
        }
        if(state_a){
            player.a+=2.0;
            player.aa+=2.0;
        }
        if(state_d){
            player.a-=2.0;
            player.aa-=2.0;
        }
        if(state_w){
            int f=0;
            GLfloat a=player.aa*M_PI/180;
            dx=-sin(a);
            dz=-cos(a);
            /* player.x-=sin(a); */
            /* look_x-=sin(a); */
            /* camera_x-=sin(a); */
            /* camera_lx-=sin(a); */

            /* player.z-=cos(a); */
            /* look_z-=cos(a); */
            /* camera_z-=cos(a); */
            /* camera_lz-=cos(a); */
        }
        else if(state_s){
            int f=0;
            GLfloat a=player.aa*M_PI/180; 
            dx=sin(a);
            dy=cos(a);
            /* player.x+=sin(a); */
            /* look_x+=sin(a); */
            /* camera_x+=sin(a); */
            /* camera_lx+=sin(a); */

            /* player.z+=cos(a); */
            /* look_z+=cos(a); */
            /* camera_z+=cos(a); */
            /* camera_lz+=cos(a); */
        }
        if(state_left)
            dx=-1;
        if(state_right)
            dx=1;
        if(state_up)
            dz=-1;
        if(state_down)
            dz=1;
        for(int i=0;i<obs.size();i++){
            if(checkCollision(player.x+dx,player.y+dy,player.z+dz,player.l,surface[obs[i]].x,(max_h/2),surface[obs[i]].z,surface[obs[i]].l/2,(max_h/2),surface[obs[i]].l/2)){
                f=1;
                if((player.y-player.l/2)-(max_h-5)>=0){
                    player.y=max_h+player.l/2,f=0;
                    /* trace2(player.y,max_h+player.l/2); */
                }
            }
        }
        if(!f && !state_jump)state_fall=1;
        for(int i=0;i<dit.size();i++){
            if(collideD(i)){state_fall=1;state_ditch=1;break;}
        }
        if(player.x>110 || player.x<-110 || player.z<-90 || player.z>110)state_fall=1,state_out=1;
        if(state_fall && !state_ditch && !state_out){
            if(player.y<=20)state_fall=0,player.y=20;
        }
        if(!f)player.x+=dx,player.z+=dz,camera_x+=dx,camera_z+=dz,camera_lx+=dx,camera_lz+=dz,look_x+=dx,look_z+=dz;
        if(state_mouse){
            if(mouseX>prevX)camera.a+=0.1,camera.aa+=0.1;
            else if(mouseX<prevX)camera.a-=0.1,camera.aa-=0.1;
        }
        if(state_jump!=0){
            if(state_jump<20)player.y+=2,state_jump++;
            else if(state_jump==20){state_jump=0,state_fall=1;}
        }
        if(player.y<-60)state_q=1;
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
