//#include <glshader.h>
#include <glshader.h>

static const char* TAG = "GLSL";
//if code_buffer is NULL this function can be used to determine a buffer size
static int get_raw_code(const char* file, size_t* code_buffer_size, char* code_buffer){
    FILE *fp;
    fp = fopen(file,"r");
    if(fp){        
        if(code_buffer == NULL){
            fseek(fp, 0L, SEEK_END);
            *code_buffer_size = ftell(fp);
            fclose(fp);
            return EXIT_SUCCESS;
        }

        char buff;
        while((buff = fgetc(fp)) != EOF){
            //printf("%c",buff);
            strncat(code_buffer, &buff,1);
        }

        fclose(fp);
    }else{
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;  
}

static void dump_shader_status(GLuint shdrid){
    GLint result = GL_FALSE;
	int infoLogLength = 0;
    // Check Vertex Shader
	glGetShaderiv(shdrid, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shdrid, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0){
        char text[infoLogLength + 1];
        glGetShaderInfoLog(shdrid, infoLogLength, NULL, text);
        LOGE(TAG, text);
    }
} 

GLuint glshader_load(const char* vrtxshdrf, const char* frgmntshdrf){
    //printf("Loading Shaders:\n%s\n%s\n",vrtxshdrf,frgmntshdrf);
    GLuint vsid = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsid = glCreateShader(GL_FRAGMENT_SHADER);
    LOGI(TAG, "Compiling shaders");

    /*---------------------------------*/
    /*         Vertexshader            */
    /*---------------------------------*/
    char* vtxcode = "";
    size_t code_buffer_size = 0;
    get_raw_code(vrtxshdrf, &code_buffer_size, NULL);
    vtxcode = (char*)malloc(code_buffer_size);
    if(!vtxcode){
        return 0;
    }
    strcpy(vtxcode, "");
    get_raw_code(vrtxshdrf, &code_buffer_size, vtxcode);
    glShaderSource(vsid, 1, (const char**)&vtxcode, NULL);
    glCompileShader(vsid);
    dump_shader_status(vsid);
    LOGI(TAG, "Vertex shader DONE!...");
    /*---------------------------------*/
    /*       Fragmentshader            */
    /*---------------------------------*/
    char* fgmtcode = "";
    code_buffer_size = 0;
    get_raw_code(frgmntshdrf, &code_buffer_size, NULL);
    fgmtcode = (char*)malloc(code_buffer_size);
    if(!fgmtcode){
        return 0;
    }
    strcpy(fgmtcode, "");
    get_raw_code(frgmntshdrf, &code_buffer_size, fgmtcode);
    glShaderSource(fsid, 1, (const char**)&fgmtcode, NULL);
    glCompileShader(fsid);
    dump_shader_status(fsid);
    LOGI(TAG, "Fragment shader DONE!...");
    /*---------------------------------*/
    /*            Linking              */
    /*---------------------------------*/
    //printf("[%s] Linking shaders...\n",TAG);
    //LOGI(TAG, "Linking shaders!...");
    GLuint programID = glCreateProgram();
    //LOGI(TAG, "Here 1");
    glAttachShader(programID, vsid);
    //LOGI(TAG, "Here 2");
	glAttachShader(programID, fsid);
    //LOGI(TAG, "Here 3");
    //LOGGLERR(TAG);
	glLinkProgram(programID);
    //LOGGLERR(TAG);
    //LOGI(TAG, "Here 4");
    dump_shader_status(programID);
    //LOGI(TAG, "Here 5");
    glDetachShader(programID, vsid);
    //LOGI(TAG, "Here 6");
    glDetachShader(programID, fsid);
    //LOGI(TAG, "Here 7");
    free(vtxcode);
    //LOGI(TAG, "Here 8");
    free(fgmtcode);
    //LOGI(TAG, "Here 9");
    glDeleteShader(vsid);
    //LOGI(TAG, "Here 10");
    glDeleteShader(fsid);
    //LOGI(TAG, "Created Shader Program");
    return programID;
}
